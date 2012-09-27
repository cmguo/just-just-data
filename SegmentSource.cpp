// SegmentSource.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/SourceError.h"
#include "ppbox/data/SegmentSource.h"
#include "ppbox/data/HttpSource.h"
#include <ppbox/cdn/PptvVod1.h>
#include <ppbox/cdn/PptvVod2.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/StringRecord.h>

using namespace util::stream;
using namespace boost::system;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("SegmentSource", 0);

namespace ppbox
{
    namespace data
    {

        SegmentSource::SegmentSource(boost::asio::io_service & io_srv)
            : SourceBase(io_srv)
            , media_(NULL)
            , source_(NULL)
            , request_closed_(true)
            , segment_closed_(true)
            , time_out_(0)
            , max_try_(0)
        {
        }

        SegmentSource::~SegmentSource()
        {
            if (source_) {
                SourceBase::destory(source_);
            }
            if (media_) {
                MediaBase::destory(media_);
            }
        }

        error_code SegmentSource::open(
            framework::string::Url const & url,
            boost::uint64_t beg, 
            boost::uint64_t end, 
            error_code & ec)
        {
            // assert(segment_ == NULL);
            // segment_ = SourceBase::create(get_io_service(), url.to_string());
            // segment_->open(url);
            return ec;
        }

        bool SegmentSource::is_open(
            error_code & ec)
        {
            // return segment_->is_open(ec);
            return true;
        }

        void SegmentSource::async_open(
            framework::string::Url const & url,
            boost::uint64_t beg, 
            boost::uint64_t end, 
            response_type const & resp)
        {
            assert(media_ == NULL);
            resp_ = resp;
            playlink_ = url;
            media_ = MediaBase::create(get_io_service(), url);
            assert(media_ != NULL);
            state_ = SegmentSource::State::segment_opening;
            media_->async_open(
                boost::bind(&SegmentSource::hand_async_open, this, _1));
        }

        void SegmentSource::hand_async_open(error_code const & ec)
        {
            if (ec) {
                state_ = SegmentSource::State::not_open;
            } else {
                source_ = SourceBase::create(get_io_service(), playlink_.protocol());
                assert(source_);
                media_->source(source_);
                source_->media(media_);
                state_ = SegmentSource::State::segment_opened;
            }
            resp_(ec);
        }

        std::size_t SegmentSource::private_read_some(
            util::stream::StreamMutableBuffers const & buffers, 
            boost::system::error_code & ec)
        {
            return prepare_read(buffers, ec);
        }

        void SegmentSource::private_async_read_some(
            util::stream::StreamMutableBuffers const & buffers,
            util::stream::StreamHandler const & handler)
        {
        }

        std::size_t SegmentSource::prepare_read(
            StreamMutableBuffers const & buffers,
            error_code & ec)
        {
            std::size_t bytes_transferred = 0;
            ec = source_error_;
            while (1) {
                if (ec) {
                } else if (cur_segment_.small_offset >= cur_segment_.size) {
                    ec = boost::asio::error::eof;
                } else if (segment_closed_ && open_segment(ec)) {
                } else if (request_closed_ && open_request(ec)) {
                } else if (source()->is_open(ec)) {
                    // update_segments();
                    framework::timer::TimeCounter tc;
                    bytes_transferred = source()->read_some(
                        buffers,
                        ec);
                    if (tc.elapse() > 10) {
                        LOG_WARN("[prepare] read elapse: " << tc.elapse() 
                            << " bytes_transferred: " << bytes_transferred);
                    }
                    increase_download_byte(bytes_transferred);
                    cur_segment_.small_offset += bytes_transferred;
                    if (ec && !source()->continuable(ec)) {
                        //LOG_S(framework::logger::Logger::kLevelAlarm, 
                        //    "[prepare_read] read_some: " << ec.message() << 
                        //    " - failed " << cur_segment_.try_times << " times");
                        LOG_WARN("[prepare_read] read_some: " << ec.message() << 
                                " - failed " << cur_segment_.try_times << " times");
                        if (ec == boost::asio::error::eof) {
                            //LOG_S(framework::logger::Logger::kLevelDebug, 
                            //    "[prepare_read] read eof, offset: " << cur_segment_.position);
                            LOG_WARN("[prepare_read] read eof, offset: " << cur_segment_.small_offset);
                        }
                    }
                } else {
                    if (!source()->continuable(ec)) {
                        //LOG_S(framework::logger::Logger::kLevelAlarm, 
                        //    "[prepare_read] open_segment: " << ec.message() << 
                        //    " - failed " << cur_segment_.try_times << " times");
                        LOG_WARN("[prepare_read] open_segment: " << ec.message() << 
                            " - failed " << cur_segment_.try_times << " times");
                    } else {
                        increase_download_byte(0);
                    }
                }
                if (source_error_) {
                    ec = source_error_;
                }
                if (!ec) {
                    break;
                }

                bool is_error = handle_error(ec);
                if (is_error) {
                    if (ec == boost::asio::error::eof) {
                        open_segment(ec);
                        if (ec && !handle_error(ec)) {
                            break;
                        }
                    } else {
                        open_segment(ec);
                    }
                } else {
                    break;
                }
            }
            return bytes_transferred;
        }

        bool SegmentSource::handle_error(
            boost::system::error_code& ec)
        {
            if (source()->continuable(ec)) {
                if (time_out_ == 0) {
                    return false; // no timeout
                } else if (get_zero_interval() > time_out_) {
                    LOG_WARN("source.read_some: timeout" << 
                        " - failed " << cur_segment_.try_times << " times");
                    ec = boost::asio::error::timed_out;
                    if (cur_segment_.try_times < max_try_) {
                        return true;
                    }
                } else {
                    return false;
                }
            } else if (ec == boost::asio::error::eof) {
                if (cur_segment_.small_offset >= cur_segment_.size) {
                    return true;
                } else if (cur_segment_.try_times < max_try_) {
                    ec = boost::asio::error::connection_aborted;
                    return true;
                }
            } else if(source()->recoverable(ec)) {
                if (cur_segment_.try_times < max_try_) {
                    return true;
                }
            }
            // source()->on_error(ec);
            //if (ec) {
            //    media()->on_error(ec);
            //}
            if (ec) {
                source_error_ = ec;
            }
            return !ec;
        }

        error_code SegmentSource::open_segment(
            error_code & ec)
        {
            assert(media_ != NULL);
            assert(strategy_ != NULL);
            ec.clear();
            close_segment(ec);
            if (strategy_->next_segment(cur_segment_)) {
                segment_closed_ = false;
            } else {
                ec = source_error::no_more_segment;
            }
            return ec;
        }

        error_code SegmentSource::open_request(
            error_code & ec)
        {
            assert(source());
            //LOG_S(Logger::kLevelDebug, 
            //    "[open_request] url: " << cur_segment_.url.to_string() 
            //    << " range: " << cur_segment_.begin + cur_segment_.position
            //    << " : " << cur_segment_.end);
            source()->media()->segment_url(cur_segment_.index, cur_segment_.url, ec);
            assert(!ec);
            LOG_WARN("[open_request] url: " << cur_segment_.url.to_string() 
                << " range: " << cur_segment_.begin + cur_segment_.small_offset
                << " : " << cur_segment_.end);
            source()->open(
                cur_segment_.url, 
                cur_segment_.begin + cur_segment_.small_offset, 
                cur_segment_.end, 
                ec);
            cur_segment_.try_times++;
            state_ = SegmentSource::State::source_opening;
            request_closed_ = false;

            return ec;
        }

        error_code SegmentSource::close_request(
            error_code & ec)
        {
            ec.clear();
            if (!request_closed_ && source()) {
                //LOG_S(framework::logger::Logger::kLevelAlarm,
                //    "[close_request] url: " << cur_segment_.url.to_string() 
                //    << ", begin: " << cur_segment_.begin << ", end: " << cur_segment_.end 
                //    << ", offset: " << cur_segment_.position);
                LOG_WARN("[close_request] url: " << cur_segment_.url.to_string() 
                    << ", begin: " << cur_segment_.begin << ", end: " << cur_segment_.end 
                    << ", offset: " << cur_segment_.small_offset);
                source()->close(ec);
                request_closed_ = true;
            }
            return ec;
        }

        error_code SegmentSource::close_segment(
            error_code & ec)
        {
            // state_ = SegmentSource::State::source_not_open;
            segment_closed_ = true;
            return ec;
        }

        void SegmentSource::reset(void)
        {
            boost::system::error_code ec;
            cur_segment_.reset();
            strategy_->byte_seek(0, cur_segment_, ec);
        }

        error_code SegmentSource::seek(
            size_t offset, 
            boost::system::error_code & ec)
        {
            assert(state_ >= SegmentSource::State::segment_opened);
            assert(strategy_);
            if (!strategy_->byte_seek(offset, cur_segment_, ec)) {
                close_request(ec);
                // �Ѿ�ͨ��byte_seek��һ��segment
                segment_closed_ = false;
                source_error_.clear();
            }
            return ec;
        }

        error_code SegmentSource::seek(
            size_t offset, 
            size_t size, 
            boost::system::error_code & ec)
        {
            seek(offset, ec);
            if (!ec) {
                need_size_ = offset + size;
            }
            return ec;
        }

        error_code SegmentSource::seek(
            SegmentInfoEx const & info, 
            boost::system::error_code & ec)
        {
            return ec;
        }

        void SegmentSource::set_strategy(
            Strategy * strategy)
        {
            strategy_ = strategy;
        }

        error_code SegmentSource::cancel(
            boost::system::error_code & ec)
        {
            // �ӿ����Ƶ��̵߳���
            ec.clear();
            if (state_ == SegmentSource::State::segment_opening) {
                media()->cancel();
            } else if (state_ == SegmentSource::State::source_opening) {
                // source()->cancel(ec);
            }
            return ec;
        }

        error_code SegmentSource::close(
            boost::system::error_code & ec)
        {
            ec.clear();
            if (state_ == SegmentSource::State::source_opened) {
                source()->close(ec);
            }
            if (state_ >= SegmentSource::State::segment_opened) {
                media()->close();
            }
            //media()->close();
            //if (source_is)
            //source()->close();
            return ec;
        }

        boost::uint64_t SegmentSource::total(
            error_code & ec)
        {
            if (strategy_) {
                return strategy_->size();
            } else {
                return 0;
            }
        }

        void SegmentSource::current_segment(SegmentInfoEx & info)
        {
            info = cur_segment_;
        }

        error_code SegmentSource::set_non_block(
            bool non_block, 
            error_code & ec)
        {
            return source_->set_non_block(non_block, ec);
        }

        error_code SegmentSource::set_time_out(
            boost::uint32_t time_out, 
            error_code & ec)
        {
            ec.clear();
            time_out_ = time_out / 1000;
            return ec;
        }

        void SegmentSource::set_max_try(boost::uint32_t times)
        {
            max_try_ = times;
        }

        bool SegmentSource::continuable(
            boost::system::error_code const & ec)
        {
            return (ec == boost::asio::error::would_block 
                || ec == boost::asio::error::eof);
        }

        bool SegmentSource::recoverable(
            boost::system::error_code const & ec)
        {
            return true;
        }

        MediaBase * SegmentSource::media(void) const
        {
            return media_;
        }

        SourceBase * SegmentSource::source(void) const
        {
            return source_;
        }

    }
}
