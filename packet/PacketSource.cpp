// PacketSource.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/packet/PacketSource.h"

#include <util/stream/Source.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>

namespace ppbox
{
    namespace data
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.PacketSource", framework::logger::Debug);

        PacketSource::PacketSource(
            PacketFeature const & feature, 
            util::stream::Source & source)
            : PacketBuffer(feature)
            , source_(source)
        {
        }

        PacketSource::~PacketSource()
        {
        }

        size_t PacketSource::prepare(
            boost::system::error_code & ec)
        {
            if (!PacketBuffer::prepare(ec)) {
                last_ec_ = ec;
                return false;
            }
            size_t size = source_.read_some(
                buffers(), ec);
            if (size) {
                commit(size);
            } else {
                last_ec_ = ec;
            }
            increase_bytes(size);
            return size;
        }

        size_t PacketSource::prepare_some(
            boost::system::error_code & ec)
        {
            size_t n = 0;
            do {
                n += prepare(ec);
            } while (!ec && feature().prepare_size > n);
            return n;
        }

        void PacketSource::async_prepare(
            prepare_response_type const & resp)
        {
            boost::system::error_code ec;
            if (!PacketBuffer::prepare(ec)) {
                last_ec_ = ec;
                resp(ec, 0);
                return;
            }
            resp_ = resp;
            source_.async_read_some(buffers(), 
                boost::bind(&PacketSource::handle_async, this, _1, _2));
        }

        void PacketSource::handle_async(
            boost::system::error_code & ec, 
            size_t bytes_transferred)
        {
            if (bytes_transferred) {
                commit(bytes_transferred);
            }
            last_ec_ = ec;
            prepare_response_type resp;
            resp.swap(resp_);
            resp(ec, bytes_transferred);
        }

        void PacketSource::pause_stream()
        {
            if (!last_ec_)
                last_ec_ = boost::asio::error::would_block;
        }

        void PacketSource::resume_stream()
        {
            if (last_ec_ == boost::asio::error::would_block)
                last_ec_.clear();
        }

        MemoryLock * PacketSource::fetch(
            boost::uint32_t & size_out, 
            std::deque<boost::asio::const_buffer> & data, 
            boost::system::error_code & ec)
        {
            if (empty()) {
                ec = last_ec_;
                if (ec || prepare(ec) == 0) {
                    return NULL;
                }
            } else {
                ec.clear();
            }
            return PacketBuffer::fetch(size_out, data);
        }

        bool PacketSource::peek_next(
            boost::uint32_t & size_out, 
            std::deque<boost::asio::const_buffer> & data, 
            boost::system::error_code & ec)
        {
            if (empty()) {
                ec = last_ec_;
                if (ec || prepare(ec) == 0) {
                    return false;
                }
            } else {
                ec.clear();
            }
            PacketBuffer::peek_next(size_out, data);
            return true;
        }

        bool PacketSource::peek_last(
            boost::uint32_t & size_out, 
            std::deque<boost::asio::const_buffer> & data, 
            boost::system::error_code & ec)
        {
            if (empty()) {
                ec = last_ec_;
                if (ec || prepare(ec) == 0) {
                    return false;
                }
            } else {
                ec.clear();
            }
            PacketBuffer::peek_last(size_out, data);
            return true;
        }

    } // namespace data
} // namespace ppbox
