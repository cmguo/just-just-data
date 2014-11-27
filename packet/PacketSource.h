// PacketSource.h

#ifndef _JUST_DATA_PACKET_PACKET_SOURCE_H_
#define _JUST_DATA_PACKET_PACKET_SOURCE_H_

#include "just/data/packet/PacketBuffer.h"
#include "just/data/base/DataStat.h"

namespace util { namespace stream { 
    class Source;
}}

namespace just
{
    namespace data
    {

        class PacketSource
            : public PacketBuffer
            , public DataWatcher
        {
        public:
            typedef boost::function<void (
                boost::system::error_code const &,
                size_t)
            > prepare_response_type;

        public:
            PacketSource(
                PacketFeature const & feature, 
                util::stream::Source & source);

            ~PacketSource();

        public:
            size_t prepare(
                boost::system::error_code & ec);

            size_t prepare_some(
                boost::system::error_code & ec);

            void async_prepare(
                prepare_response_type const & resp);

        public:
            void pause_stream();

            void resume_stream();

        public:
            MemoryLock * fetch(
                boost::uint32_t & size_out, 
                std::deque<boost::asio::const_buffer> & data, 
                boost::system::error_code & ec);

            bool peek_next(
                boost::uint32_t & size_out, 
                std::deque<boost::asio::const_buffer> & data, 
                boost::system::error_code & ec);

            bool peek_last(
                boost::uint32_t & size_out, 
                std::deque<boost::asio::const_buffer> & data, 
                boost::system::error_code & ec);

        public:
            util::stream::Source & source() const
            {
                return source_;
            }

            boost::system::error_code last_error() const
            {
                return last_ec_;
            }

        private:
            void handle_async(
                boost::system::error_code & ec, 
                size_t bytes_transferred);

        private:
            util::stream::Source & source_;
            boost::system::error_code last_ec_;
            prepare_response_type resp_;
        };

    } // namespace data
} // namespace just

#endif // _JUST_DATA_PACKET_PACKET_SOURCE_H_
