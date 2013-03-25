// PacketSource.h

#ifndef _PPBOX_DATA_PACKET_PACKET_SOURCE_H_
#define _PPBOX_DATA_PACKET_PACKET_SOURCE_H_

#include "ppbox/data/packet/PacketBuffer.h"
#include "ppbox/data/packet/PacketMedia.h"

#include <util/stream/StreamBuffers.h>

#include <framework/memory/PrivateMemory.h>
#include <framework/container/List.h>

namespace ppbox
{
    namespace data
    {

        class PacketSource
            : public PacketBuffer
        {
        public:
            typedef boost::function<void (
                boost::system::error_code const &,
                size_t)
            > prepare_response_type;

        public:
            PacketSource(
                PacketMedia & media);

            ~PacketSource();

        public:
            size_t prepare(
                boost::system::error_code & ec);

            size_t prepare_some(
                boost::system::error_code & ec);

            void async_prepare(
                prepare_response_type const & resp);

            void pause_stream();

            void resume_stream();

            MemoryLock * fetch(
                boost::uint32_t & size_out, 
                std::deque<boost::asio::const_buffer> & data, 
                boost::system::error_code & ec);

        public:
            SourceBase & source() const
            {
                return source_;
            }

        private:
            void handle_async(
                boost::system::error_code & ec, 
                size_t bytes_transferred);

        private:
            SourceBase & source_;
            boost::system::error_code last_ec_;
            prepare_response_type resp_;
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_PACKET_PACKET_SOURCE_H_
