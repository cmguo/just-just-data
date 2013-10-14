// SingleBuffer.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/single/SingleBuffer.h"
#include <ppbox/data/single/SingleSource.h>
#include <ppbox/data/base/Error.h>

#include <framework/system/LogicError.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>

namespace ppbox
{
    namespace data
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.SingleBuffer", framework::logger::Debug);

        SingleBuffer::SingleBuffer(
            ppbox::data::SingleSource & source, 
            boost::uint32_t buffer_size, 
            boost::uint32_t prepare_size)
            : Buffer(buffer_size)
            , source_(source)
            , prepare_size_(prepare_size)
            , stream_pos_(0)
            , total_size_(invalid_size)
        {
        }

        SingleBuffer::~SingleBuffer()
        {
        }

        // 目前只发生在，seek到一个分段，还没有该分段头部数据时，
        // 此时size为head_size_头部数据大小 
        // TO BE FIXED
        bool SingleBuffer::seek(
            boost::uint64_t pos, 
            boost::uint64_t size, 
            boost::system::error_code & ec)
        {
            bool write_change = Buffer::seek(pos, size);

            if (write_change) {
                source_seek(ec);
            }

            read_seek(pos, ec);

            last_ec_ = ec;

            return !ec;
        }

        bool SingleBuffer::seek(
            boost::uint64_t pos, 
            boost::system::error_code & ec)
        {
            return seek(pos, invalid_size, ec);
        }

        size_t SingleBuffer::prepare(
            size_t amount, 
            boost::system::error_code & ec)
        {
            if (check_hole(ec)) {
                if (ec == boost::asio::error::eof) {
                    next_write_hole();
                    source_seek(ec);
                }
                if (ec) {
                    last_ec_ = ec;
                    return 0;
                }
            }
            prepare_buffers_.clear();
            Buffer::prepare(amount, prepare_buffers_);
            amount = source_.read_some(prepare_buffers_, ec);
            commit(amount);
            last_ec_ = ec;
            return amount;
        }

        size_t SingleBuffer::prepare_at_least(
            size_t amount, 
            boost::system::error_code & ec)
        {
            ec = last_ec_;
            size_t n = 0;
            do {
                n += prepare_some(amount - n, ec);
            } while (!ec && amount > n);
            return n;
        }

        void SingleBuffer::async_prepare(
            size_t amount, 
            prepare_response_type const & resp)
        {
            resp_ = resp;
            boost::system::error_code ec;
            if (check_hole(ec)) {
                if (ec == boost::asio::error::eof) {
                    next_write_hole();
                    source_seek(ec);
                }
            }
            if (ec) {
                last_ec_ = ec;
                resp_(ec, 0);
                return;
            }
            prepare_buffers_.clear();
            Buffer::prepare(amount, prepare_buffers_);
            source_.async_read_some(prepare_buffers_, 
                boost::bind(&SingleBuffer::handle_async, this, _1, _2));
        }

        void SingleBuffer::handle_async(
            boost::system::error_code const & ec, 
            size_t bytes_transferred)
        {
            last_ec_ = ec;
            commit(bytes_transferred);
            pause_stream();
            prepare_response_type resp;
            resp.swap(resp_);
            resp(ec, 0);
        }

        MemoryLock * SingleBuffer::fetch(
            boost::uint32_t track, 
            std::vector<DataBlock> & blocks, 
            std::deque<boost::asio::const_buffer> & data, 
            boost::system::error_code & ec)
        {
            boost::uint64_t offset = blocks.front().offset;
            boost::uint32_t size = blocks.front().end() - offset;
            assert(offset >= in_position() && offset + size <= total_size_);
            if (offset < in_position()) {
                ec = framework::system::logic_error::out_of_range;
            } else if (offset + size > total_size_) {
                ec = boost::asio::error::eof;
            } else {
                if (offset + size > out_position()) {
                    ec = last_ec_;
                    if (!ec) {
                        prepare_at_least((boost::uint32_t)(offset + size - out_position()), ec);
                    }
                }
                if (offset + size <= out_position()) {
                    ec.clear();
                    return Buffer::fetch(track, blocks, data);
                }
            }
            return NULL;
        }

        void SingleBuffer::putback(
            MemoryLock * mlock)
        {
            if (Buffer::putback(mlock)) {
                boost::system::error_code ec;
                read_seek(position(), ec);
            }
        }

        bool SingleBuffer::read_seek(
            boost::uint64_t pos, 
            boost::system::error_code & ec)
        {
            boost::uint64_t beg = in_position();
            boost::uint64_t end = out_position();
            if (pos < beg) {
                pos = beg;
                //assert(false); // mp4 demuxer 有可能读指针停留在靠前位置
                ec = framework::system::logic_error::out_of_range;
            } else if (pos >= end) {
                if (pos > total_size_) {
                    pos = total_size_;
                    assert(false);
                    ec = framework::system::logic_error::out_of_range;
                }
                if (pos >= out_position()) {
                    boost::system::error_code ec1 = last_ec_;
                    if (!ec1) {
                        prepare_at_least((size_t)(pos - out_position()), ec1); // 尽量让buffer有数据
                    }
                    if (pos > out_position()) { // 如果没有，也不算失败
                        pos = out_position();
                        if (!ec) ec = ec1;
                    }
                    end = out_position();
                }
                assert(pos <= end);
            }
            boost::uint8_t * ptr = (boost::uint8_t *)read_buffer(beg, pos, end);
            pos -= beg;
            end -= beg;
            stream_pos_ = beg;
            setg(ptr, ptr + pos, ptr + end);
            return !ec;
        }

        void SingleBuffer::pause_stream()
        {
            if (!last_ec_)
                last_ec_ = boost::asio::error::would_block;
        }

        void SingleBuffer::resume_stream()
        {
            if (last_ec_ == boost::asio::error::would_block)
                last_ec_.clear();
        }

        bool SingleBuffer::source_seek(
            boost::system::error_code & ec)
        {
            boost::uint64_t size = write_hole_size();
            if (size == 0) {
                ec = error::at_end_point;
            }
            source_.seek(out_position(), size, ec);
            return !ec;
        }

        void SingleBuffer::on_event(
            util::event::Event const & ec)
        {
        }

        boost::uint64_t SingleBuffer::position()
        {
            pos_type pos = stream_pos_ + off_type(gptr() - eback());
            return pos;
        }

        void SingleBuffer::update()
        {
            boost::system::error_code ec;
            read_seek(position(), ec);
            assert(!ec);
        }

        SingleBuffer::int_type SingleBuffer::underflow()
        {
            update();
            if (gptr() < egptr()) {
                return *gptr();
            } else {
                return traits_type::eof();
            }
        }

        SingleBuffer::pos_type SingleBuffer::seekoff(
            off_type off, 
            std::ios_base::seekdir dir,
            std::ios_base::openmode mode)
        {
            if (dir == std::ios_base::cur) {
                pos_type pos = position();
                if (off == 0) {
                    return pos;
                }
                pos += off;
                return seekpos(pos, mode);
            } else if (dir == std::ios_base::beg) {
                return seekpos(off, mode);
            } else if (dir == std::ios_base::end) {
                assert(off <= 0);
                return seekpos((pos_type)out_position() + off, mode);
            } else {
                return pos_type(-1);
            }
        }

        SingleBuffer::pos_type SingleBuffer::seekpos(
            pos_type position, 
            std::ios_base::openmode mode)
        {
            assert(position != pos_type(-1));
            if (mode == (std::ios_base::in | std::ios_base::out)) {
                boost::system::error_code ec;
                if (!seek(position, ec)) {
                    return pos_type(-1);
                }
                mode = std::ios_base::in;
            }
            if (mode != std::ios_base::in) {
                return pos_type(-1);// 模式错误
            }
            boost::system::error_code ec;
            if (!read_seek(position, ec)) {
                return pos_type(-1);// 有效位置之前
            }
            return position;
        }

    } // namespace data
} // namespace ppbox
