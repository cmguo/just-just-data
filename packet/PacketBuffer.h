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
            boost::uint64_t out_position() const
            {
                return write_offset_;
            }

            size_t out_avail() const
            {
                return feature_.buffer_size - in_avail();
            }

            // ∂¡÷∏’Î∆´“∆
            boost::uint64_t in_position() const
            {
                return read_offset_;
            }

            size_t in_avail() const
            {
                return (size_t)(in_position() - in_position());
            }

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
                pkt_out(pkt, size_out, buffers);
                locked_packets_.push_back(pkt);
                read_offset_ += pkt->size;
                ++read_index_;
                ++locked_count_;
                return (MemoryLock *)pkt;
            }

            void putback(
                MemoryLock * mlock);

            template <
                typename BufferSequence
            >
            void peek_next(
                boost::uint32_t & size_out, 
                BufferSequence & buffers)
            {
                assert(!packets_.empty());
                Packet * pkt = packets_.first();
                pkt_out(pkt, size_out, buffers);
            }

            template <
                typename BufferSequence
            >
            void peek_last(
                boost::uint32_t & size_out, 
                BufferSequence & buffers)
            {
                assert(!packets_.empty());
                Packet * pkt = packets_.last();
                pkt_out(pkt, size_out, buffers);
            }

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

            typedef MemoryLock Packet;

        private:
            template <
                typename BufferSequence
            >
            void pkt_out(
                Packet * pkt, 
                boost::uint32_t & size_out, 
                BufferSequence & buffers)
            {
                PieceHeader * ph = (PieceHeader *)pkt->pointer;
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
            }

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
            boost::uint64_t write_offset_;
            boost::uint64_t read_offset_;
            boost::uint64_t write_index_;
            boost::uint64_t read_index_;
            framework::container::List<Packet> locked_packets_;
            framework::container::List<Packet> free_packets_;
            boost::uint32_t locked_count_;
            boost::uint32_t free_count_;
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_PACKET_PACKET_BUFFER_H_
