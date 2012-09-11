// SegmentSource.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/SourceError.h"
#include "ppbox/data/SegmentSource.h"
#include "ppbox/data/HttpSource.h"
#include "ppbox/data/strategy/HeadSourceStrategy.h"
#include "ppbox/data/strategy/BodySourceStrategy.h"
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
            , segment_(NULL)
            , source_(NULL)
            , source_closed_(true)
            , has_seek_(false)
            , time_out_(0)
            , max_try_(0)
        {
        }

        SegmentSource::~SegmentSource()
        {
            if (source_) {
                SourceBase::destory(source_);
            }
            if (segment_) {
                MediaBase::destory(segment_);
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
            assert(segment_ == NULL);
            resp_ = resp;
            playlink_ = url;
            segment_ = MediaBase::create(get_io_service(), url);
            assert(segment_ != NULL);
            state_ = SegmentSource::State::segment_opening;
            segment_->async_open(
                boost::bind(&SegmentSource::hand_async_open, this, _1));
        }

        void SegmentSource::hand_async_open(error_code const & ec)
        {
            if (ec) {
                state_ = SegmentSource::State::not_open;
            } else {
                source_ = SourceBase::create(get_io_service(), playlink_.protocol());
                assert(source_);
                update_segments();
                update_video_info();
                //source_strategy_ = SourceStrategy::create("full", segments_);
                //assert(source_strategy_);
                //reset();
                state_ = SegmentSource::State::segment_opened;
            }
            resp_(ec);
        }

        std::size_t SegmentSource::private_read_some(
            util::stream::StreamMutableBuffers const & buffers, 
            boost::system::error_code & ec)
        {
            if (has_seek_ && hand_seek(ec)) {
            } // TODO: handseek�Ĵ�����û�д���
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
                } else if (cur_segment_.position >= cur_segment_.size) {
                    ec = boost::asio::error::eof;
                } else if (source_closed_ && open_segment(false, ec)) {
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
                    cur_segment_.position += bytes_transferred;
                    if (ec && !source()->continuable(ec)) {
                        //LOG_S(framework::logger::Logger::kLevelAlarm, 
                        //    "[prepare_read] read_some: " << ec.message() << 
                        //    " - failed " << cur_segment_.try_times << " times");
                        LOG_WARN("[prepare_read] read_some: " << ec.message() << 
                                " - failed " << cur_segment_.try_times << " times");
                        if (ec == boost::asio::error::eof) {
                            //LOG_S(framework::logger::Logger::kLevelDebug, 
                            //    "[prepare_read] read eof, offset: " << cur_segment_.position);
                            LOG_WARN("[prepare_read] read eof, offset: " << cur_segment_.position);
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
                        open_segment(true, ec);
                        if (ec && !handle_error(ec)) {
                            break;
                        }
                    } else {
                        open_segment(false, ec);
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
                if (cur_segment_.position >= cur_segment_.size) {
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
            if (ec) {
                source_error_ = ec;
            }
            return !ec;
        }

        error_code SegmentSource::open_segment(
            bool is_next_segment, 
            error_code & ec)
        {
            assert(segment_ != NULL);
            assert(source_strategy_ != NULL);
            ec.clear();
            close_segment(ec); // TODO: �������Ƿ���Ҫ����
            if (!is_next_segment || source_strategy_->next_segment(
                true, 
                cur_segment_)) {
                    open_request(ec);
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
            LOG_WARN("[open_request] url: " << cur_segment_.url.to_string() 
                << " range: " << cur_segment_.begin + cur_segment_.position
                << " : " << cur_segment_.end);
            source()->open(
                cur_segment_.url, 
                cur_segment_.begin + cur_segment_.position, 
                cur_segment_.end, 
                ec);
            cur_segment_.try_times++;
            state_ = SegmentSource::State::source_opening;
            source_closed_ = false;

            return ec;
        }

        error_code SegmentSource::close_request(
            error_code & ec)
        {
            ec.clear();
            if (!source_closed_ && source()) {
                //LOG_S(framework::logger::Logger::kLevelAlarm,
                //    "[close_request] url: " << cur_segment_.url.to_string() 
                //    << ", begin: " << cur_segment_.begin << ", end: " << cur_segment_.end 
                //    << ", offset: " << cur_segment_.position);
                LOG_WARN("[close_request] url: " << cur_segment_.url.to_string() 
                    << ", begin: " << cur_segment_.begin << ", end: " << cur_segment_.end 
                    << ", offset: " << cur_segment_.position);
                source()->close(0, ec);
                source_closed_ = true;
            }
            return ec;
        }

        error_code SegmentSource::close_segment(
            error_code & ec)
        {
            close_request(ec);
            state_ = SegmentSource::State::source_not_open;
            return ec;
        }

        void SegmentSource::update_segments(void)
        {
            assert(segment_);
            segments_.clear();
            boost::uint32_t size = segment_->segment_count();
            for (boost::uint32_t i = 0; i < size; ++i) {
                SegmentInfoEx infoEx;
                segment_->segment_info(i, infoEx);
                boost::system::error_code lec;
                segment_->segment_url(i, infoEx.url, lec);
                segments_.push_back(infoEx);
            }
        }

        void SegmentSource::update_video_info(void)
        {
            assert(segment_);
            boost::system::error_code ec;
            segment_->get_info(video_info_, ec);
            assert(!ec);
        }

        void SegmentSource::reset(void)
        {
            boost::system::error_code ec;
            source_strategy_->on_seek(0, cur_segment_, ec);
            has_seek_ = true;
        }

        error_code SegmentSource::seek(
            size_t offset, 
            boost::system::error_code & ec)
        {
            assert(state_ >= SegmentSource::State::segment_opened);
            assert(source_strategy_);
            if (!source_strategy_->on_seek(offset, cur_segment_, ec)) {
                has_seek_ = true;
            }
            return ec;
        }

        error_code SegmentSource::seek(
            boost::uint32_t segment_index, 
            size_t offset, 
            boost::system::error_code & ec)
        {
            assert(state_ >= SegmentSource::State::segment_opened);
            if (!source_strategy_->on_seek(
                segment_index, 
                offset, 
                cur_segment_, 
                ec)) {
                    has_seek_ = true;
            }
            return ec;
        }

        error_code SegmentSource::hand_seek(error_code & ec)
        {
            close_segment(ec);
            source_error_.clear();
            has_seek_ = false;
            return ec;
        }

        void SegmentSource::set_strategy(
            SourceStrategy * strategy)
        {
            source_strategy_ = strategy;
        }

        std::vector<SegmentInfoEx> const & SegmentSource::segments(void) const
        {
            return segments_;
        }

        MediaInfo const & SegmentSource::mediainfo(void) const
        {
            return video_info_;
        }

        error_code SegmentSource::cancel(
            size_t segment, 
            boost::system::error_code & ec)
        {
            // �ӿ����Ƶ��̵߳���
            ec.clear();
            if (state_ == SegmentSource::State::segment_opening) {
                segment_->cancel();
            } else if (state_ == SegmentSource::State::source_opening) {
                source()->cancel(segment, ec);
            }
            return ec;
        }

        error_code SegmentSource::close(
            size_t segment, 
            boost::system::error_code & ec)
        {
            ec.clear();
            if (state_ == SegmentSource::State::source_opened) {
                source()->close(segment, ec);
            }
            if (state_ >= SegmentSource::State::segment_opened) {
                segment_->close();
            }
            return ec;
        }

        boost::uint64_t SegmentSource::total(
            error_code & ec)
        {
            ec.clear();
            boost::uint64_t size = 0;
            for (boost::uint32_t i = 0; i < segments_.size(); ++i) {
                size += segments_[i].size;
            }
            return size;
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

        SourceBase * SegmentSource::source(void)
        {
            return source_;
        }

    }
}
