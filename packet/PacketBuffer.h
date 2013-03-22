// PacketBuffer.h

#ifndef _PPBOX_DATA_PACKET_PACKET_BUFFER_H_
#define _PPBOX_DATA_PACKET_PACKET_BUFFER_H_

#include "ppbox/data/packet/PacketFeature.h"

#include <ppbox/avformat/Format.h>

#include <util/stream/StreamBuffers.h>

#include <framework/memory/PrivateMemory.h>
#include <framework/container/List.h>

namespace ppbox
{
    namespace data
    {

        class PacketBuffer
        {
        public:
            typedef util::stream::StreamMutableBuffers buffers_t;
            typedef std::deque<boost::asio::const_buffer> blocks_t;

        public:
            PacketBuffer(
                PacketFeature const & feature);

            ~PacketBuffer();

        public:
            // get Memory Lock
            void * fetch(
                boost::uint32_t & size_out, 
                blocks_t & blocks);

            void putback(
                void * mem);

            bool empty() const
            {
                return packets_.empty();
            }

            void clear();

        protected:
            bool prepare(
                boost::system::error_code & ec);

            void commit(
                size_t size);

            util::stream::StreamMutableBuffers const & buffers() const
            {
                return buffers_;
            }

            PacketFeature const & feature()
            {
                return feature_;
            }

        private:
            struct PieceHeader
            {
                PieceHeader * next_piece;
            };

            struct Packet
                : framework::container::ListHook<Packet>::type
            {
                PieceHeader * pieces;
                size_t size;
            };

        private:
            PieceHeader * alloc_piece();

            void free_piece(
                PieceHeader * ptr);

            Packet * alloc_packet();

            void free_packet(
                Packet * ptr);

        private:
            void dump();

            void handle_async(
                boost::system::error_code & ec, 
                size_t bytes_transferred);

        private:
            PacketFeature feature_;
            framework::memory::PrivateMemory memory_;
            std::vector<void *> blocks_;
            PieceHeader * free_pieces_;
            util::stream::StreamMutableBuffers buffers_;
            framework::container::List<Packet> packets_;
            framework::container::List<Packet> locked_packets_;
            framework::container::List<Packet> free_packets_;
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_PACKET_PACKET_BUFFER_H_
