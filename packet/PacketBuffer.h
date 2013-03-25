// PacketBuffer.h

#ifndef _PPBOX_DATA_PACKET_PACKET_BUFFER_H_
#define _PPBOX_DATA_PACKET_PACKET_BUFFER_H_

#include "ppbox/data/packet/PacketFeature.h"
#include "ppbox/data/base/MemoryLock.h"

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

        public:
            PacketBuffer(
                PacketFeature const & feature);

            ~PacketBuffer();

        public:
            // get Memory Lock
            template <
                typename BufferSequence
            >
            MemoryLock * fetch(
                boost::uint32_t & size_out, 
                BufferSequence & buffers)
            {
                assert(!packets_.empty());
                Packet * pkt = packets_.first();
                packets_.pop_front();
                PieceHeader * ph = pkt->pieces;
                size_t size = size_out = pkt->size;
                while (ph && size) {
                    if (size > feature_.piece_size) {
                        buffers.push_back(boost::asio::buffer(ph + 1, feature_.piece_size));
                        size -= feature_.piece_size;
                    } else {
                        buffers.push_back(boost::asio::buffer(ph + 1, size));
                        size = 0;
                    }
                    ph = ph->next_piece;
                }
                locked_packets_.push_back(pkt);
                return (MemoryLock *)ph;
            }

            void putback(
                MemoryLock * mlock);

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
