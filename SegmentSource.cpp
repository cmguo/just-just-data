// SegmentSource.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/SegmentSource.h"
#include "ppbox/data/SourceError.h"
#include "ppbox/data/SourceEvent.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/timer/TimeCounter.h>

#include <boost/bind.hpp>

namespace ppbox
{
    namespace data
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.SegmentSource", framework::logger::Debug);

        SegmentSource::SegmentSource(
            ppbox::data::SegmentStrategy & strategy, 
            ppbox::data::SourceBase & source, 
            size_t total_req)
            : util::stream::Source(source.get_io_service())
            , max_try_(size_t(-1))
            , max_req_(total_req)
            , time_block_(0)
            , strategy_(&strategy)
            , source_(source)
            , num_try_(0)
            , sended_req_(0)
            , source_closed_(true)
            , time_out_(0)
            , seek_end_(0)
            , expire_pause_time_(framework::timer::Time::now())
        {
        }

        SegmentSource::~SegmentSource()
        {
        }

        boost::system::error_code SegmentSource::seek(
            segment_t segment, 
            boost::uint64_t size, 
            boost::system::error_code & ec)
        {
            close_segment(ec);
            close_all_request(ec);
            write_tmp_ = write_ = segment;
            seek_end_ = (size == invalid_size) ? invalid_size : write_.byte_range.big_pos() + size;
            ec.clear();
            return ec;
        }

        boost::system::error_code SegmentSource::seek(
            boost::uint64_t offset, 
            boost::uint64_t size, 
            boost::system::error_code & ec)
        {
            segment_t seg;
            if (strategy_->byte_seek(offset, seg, ec))
                return ec;
            return seek(seg, size, ec);
        }

        void SegmentSource::reset()
        {
            boost::system::error_code ec;
            seek(0, ec);
        }

        void SegmentSource::pause(
            boost::uint32_t time)
        {
            boost::system::error_code ec;
            close_segment(ec);
            close_all_request(ec);
            expire_pause_time_ = framework::timer::Time::now() + framework::timer::Duration::milliseconds(time);
        }

        boost::system::error_code SegmentSource::cancel(
            boost::system::error_code & ec)
        {
            source_error_ = boost::asio::error::operation_aborted;
            return source_.cancel(ec);
        }

        boost::system::error_code SegmentSource::close(
            boost::system::error_code & ec)
        {
            close_all_request(ec);
            return ec;
        }

        std::size_t SegmentSource::private_read_some(
            buffers_t const & buffers,
            boost::system::error_code & ec)
        {
            return prepare(buffers, ec);
        }

        void SegmentSource::private_async_read_some(
            buffers_t const & buffers, 
            handler_t const & handler)
        {
            async_prepare(buffers, handler);
        }

        size_t SegmentSource::prepare(
            buffers_t const & buffers, 
            boost::system::error_code & ec)
        {
            size_t bytes_transferred = 0;
            ec = source_error_;
            while (1) {
                if (ec) {
                } else if (write_.byte_range.pos >= write_.byte_range.end) {
                    ec = boost::asio::error::eof;
                } else if (sended_req_ == 0 && !open_segment(false, ec)) {
                } else if (!source_closed_ || segment_is_open(ec)) {
                    framework::timer::TimeCounter tc;
                    bytes_transferred = source_.read_some(buffers, ec);
                    if (tc.elapse() >= 20) {
                        LOG_DEBUG("[prepare] read elapse: " << tc.elapse() 
                            << " bytes_transferred: " << bytes_transferred);
                    }
                    increase_download_byte(bytes_transferred);
                    write_.byte_range.pos += bytes_transferred;
                    if (ec && !source_.continuable(ec)) {
                        LOG_WARN("[prepare] read_some: " << ec.message() << 
                            " --- failed " << num_try_ << " times");
                        if (ec == boost::asio::error::eof) {
                            LOG_DEBUG("[prepare] read eof, write_.offset: " << write_.byte_range.pos
                                << " write_hole_.this_end: " << write_.byte_range.end);
                        }
                    }
                } else {
                    if (!source_.continuable(ec)) {
                        LOG_WARN("[prepare] open_segment: " << ec.message() << 
                            " --- failed " << num_try_ << " times");
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
                    } else {
                        open_segment(false, ec);
                    }
                } else {
                    break;
                }
            }
            return bytes_transferred;
        }

        // 返回false表示不能再继续了
        bool SegmentSource::handle_error(
            boost::system::error_code& ec)
        {
            if (source_.continuable(ec)) {
                time_block_ = get_zero_interval();
                if (time_out_ > 0 && time_block_ > time_out_) {
                    LOG_WARN("source.read_some: timeout" << 
                        " --- failed " << num_try_ << " times");
                    ec = boost::asio::error::timed_out;
                    if (num_try_ < max_try_) {
                        return true;
                    }
                } else {
                    return false;
                }
            } else if (ec == boost::asio::error::eof) {
                if (write_.byte_range.pos >= write_.byte_range.end) {
                    return true;
                } else if (write_.size == invalid_size) {
                    write_.size = write_.byte_range.end = write_.byte_range.pos;
                    LOG_INFO("[handle_error] guess segment size " << write_.size);
                    return true;
                } else if (num_try_ < max_try_) {
                    ec = boost::asio::error::connection_aborted;
                    return true;
                }
            } else if(source_.recoverable(ec)) {
                if (num_try_ < max_try_) {
                    return true;
                }
            }
            //source_.on_error(ec);
            if (ec)
                source_error_ = ec;
            return !ec;
        }

        void SegmentSource::async_prepare(
            buffers_t const & buffers, 
            handler_t const & handler)
        {
            handle_async(buffers, handler, boost::system::error_code(), 0);
        }

        void SegmentSource::handle_async(
            buffers_t const & buffers, 
            handler_t const & handler, 
            boost::system::error_code const & ecc, 
            size_t bytes_transferred)
        {
            boost::system::error_code ec = ecc;
            bool is_open_callback = false;
            if (bytes_transferred == (size_t)-1) {
                is_open_callback = true;
                bytes_transferred = 0;
            }
            if (ec && !source_.continuable(ec)) {
                if (is_open_callback) {
                    if (ec != source_error::no_more_segment) {
                        LOG_DEBUG("[handle_async] open_segment: " << ec.message() << 
                            " --- failed " << num_try_ << " times");
                    }
                }
                if (!source_closed_) {
                    LOG_WARN("[handle_async] read_some: " << ec.message() << 
                        " --- failed " << num_try_ << " times");
                    if (ec == boost::asio::error::eof) {
                        LOG_DEBUG("[handle_async] read eof, write_.offset: " << write_.byte_range.pos
                            << " write_hole_.this_end: " << write_.byte_range.end);
                    }
                }
            }
            increase_download_byte(bytes_transferred);
            write_.byte_range.pos += bytes_transferred;
            if (source_error_) {
                ec = source_error_;
            }
            if (ec) {
                bool is_error = handle_error(ec);
                if (is_error) {
                    if (ec == boost::asio::error::eof) {
                        reset_zero_interval();
                        time_block_ = 0;
                        async_open_segment(true, boost::bind(&SegmentSource::handle_async, this, buffers, handler, _1, (size_t)-1));
                    } else {
                        async_open_segment(false, boost::bind(&SegmentSource::handle_async, this, buffers, handler, _1, (size_t)-1));
                    }
                } else {
                    boost::system::error_code ec1;
                    close_request(ec1);
                    response(handler, ec, 0);
                }
            } else if (bytes_transferred > 0) {
                response(handler, ec, bytes_transferred);
            } else if (write_.byte_range.pos >= write_.byte_range.end) {
                ec = boost::asio::error::eof;
                handle_async(buffers, handler, ec, 0);
            } else if (sended_req_ == 0) {
                async_open_segment(false, boost::bind(&SegmentSource::handle_async, this, buffers, handler, _1, (size_t)-1));
            } else {
                update_segment(ec);
                source_.async_read_some(buffers, boost::bind(&SegmentSource::handle_async, this, buffers, handler, _1, _2));
            }
        }

        void SegmentSource::response(
            handler_t const & handler, 
            boost::system::error_code const & ec, 
            size_t bytes_transferred)
        {
            write_tmp_ = write_;
            boost::system::error_code ecc;
            open_request(true, ecc);
            handler(ec, bytes_transferred);
        }

        void SegmentSource::set_max_req(
            size_t num)
        {
            max_req_ = num;
            boost::system::error_code ec;
            open_request(true, ec);
        }

        void SegmentSource::update_segment(
            boost::system::error_code & ec)
        {
            if (write_.size == invalid_size) {
                write_.size = source_.total(ec);
                if (ec) {
                    write_.size = invalid_size; // source_.total()失败时返回0，需要重置
                } else {
                    write_.byte_range.end = write_.size;
                }
            }
        }

        bool SegmentSource::next_segment(
            segment_t & seg, 
            boost::system::error_code & ec)
        {
            if (seg.byte_range.big_end() >= seek_end_ || !strategy_->next_segment(seg, ec)) {
                ec = source_error::no_more_segment;
                return false;
            }
            if (seg.byte_range.big_end() > seek_end_) {
                seg.byte_range.end = seek_end_ - seg.byte_range.big_offset;
            }
            ec.clear();
            return true;
        }

        bool SegmentSource::open_request(
            bool is_next_segment,
            boost::system::error_code & ec)
        {
            for (; sended_req_ < max_req_; ) {
                segment_t write_tmp = write_tmp_;
                if (is_next_segment) {
                    if (!next_segment(write_tmp_, ec)) {
                        if (sended_req_ && ec == source_error::no_more_segment) {
                            ec.clear();
                        }
                        break;
                    }
                }
                LOG_TRACE("[open_request] segment: " << write_tmp_.index << " sended_req: " << sended_req_ << "/" << max_req_);
                ++num_try_;
                ++sended_req_;
                source_.open(
                    write_tmp_.url, 
                    write_tmp_.byte_range.pos, 
                    write_tmp_.byte_range.end == write_tmp_.size ? invalid_size : write_tmp_.byte_range.end, ec);
                if (ec) {
                    if (source_.continuable(ec)) {
                        if (sended_req_) // 如果已经发过一个请求
                            ec.clear();
                    } else {
                        write_tmp_ = write_tmp;
                        break;
                    }
                }
                is_next_segment = true;
            }
            return !ec;
        }

        bool SegmentSource::close_request(
            boost::system::error_code & ec)
        {
            if (sended_req_) {
                source_.close(ec);
                --sended_req_;
                LOG_TRACE("[close_request] segment: " << write_.index << " sended_req: " << sended_req_ << "/" << max_req_);
                return !ec;
            } else {
                ec.clear();
                return false;
            }
        }

        bool SegmentSource::close_all_request(
            boost::system::error_code & ec)
        {
            write_tmp_ = write_;
            for (size_t i = 0; i < sended_req_; ++i) {
                source_.close(ec);
                --sended_req_;
                LOG_TRACE("[close_all_request] segment: " << write_.index << " sended_req: " << sended_req_ << "/" << max_req_);
                next_segment(write_tmp_, ec);
            }
            write_tmp_ = write_;
            ec.clear();
            return true;
        }

        bool SegmentSource::open_segment(
            bool is_next_segment, 
            boost::system::error_code & ec)
        {
            close_segment(ec);

            if (is_next_segment) {
                reset_zero_interval();
                close_request(ec);
                num_try_ = 0;
                time_block_ = 0;
            } else {
                reset_zero_interval();
                close_all_request(ec);
            }

            if ((framework::timer::Time::now() - expire_pause_time_) < framework::timer::Duration::milliseconds(0)) {
                ec = boost::asio::error::would_block;
                return false;
            }

            open_request(is_next_segment, ec);

            if (ec && !source_.continuable(ec)) {
                if (ec != source_error::no_more_segment) {
                    LOG_DEBUG("[open_segment] source_.open_segment: " << ec.message() << 
                        " --- failed " << num_try_ << " times");
                }
                return false;
            }

            if (is_next_segment) {
                if (!next_segment(write_, ec)) {
                    assert(0);
                    return false;
                }
            }

            raise(SegmentStartEvent(write_));

            LOG_DEBUG("[open_segment] write_.offset: " << write_.byte_range.pos << 
                " segment: " << write_.index << 
                " range: " << write_.byte_range.pos << " - " << write_.byte_range.end);

            return true;
        }

        void SegmentSource::async_open_segment(
            bool is_next_segment, 
            ppbox::data::SourceBase::response_type const & resp)
        {
            boost::system::error_code ec;

            close_segment(ec);

            if (is_next_segment) {
                close_request(ec);
                if (next_segment(write_, ec)) {
                    resp(ec);
                    return;
                }
                num_try_ = 0;
            } else {
                reset_zero_interval();
                close_request(ec);
            }

            ++sended_req_;
            ++num_try_;

            source_.async_open(
                write_.url, 
                write_.byte_range.pos, 
                write_.byte_range.end == write_.size ? invalid_size : write_.byte_range.end, 
                resp);
        }

        bool SegmentSource::segment_is_open(
            boost::system::error_code & ec)
        {
            if (source_closed_ && source_.is_open(ec)) {
                update_segment(ec);
                source_closed_ = false;
            }
            return !source_closed_;
        }

        bool SegmentSource::close_segment(
            boost::system::error_code & ec)
        {
            if (sended_req_) {
                LOG_DEBUG("[close_segment] write_.offset: " << write_.byte_range.pos << 
                    " segment: " << write_.index << 
                    " range: ? - "<< write_.byte_range.end);
                source_closed_ = true;

                raise(SegmentStopEvent(write_));
            }
            ec.clear();
            return true;
        }

    } // namespace demux
} // namespace ppbox
