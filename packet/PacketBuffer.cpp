// PacketBuffer.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/packet/PacketBuffer.h"
#include "ppbox/data/base/SourceBase.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>

namespace ppbox
{
    namespace data
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.PacketBuffer", framework::logger::Debug);

        PacketBuffer::PacketBuffer(
            PacketFeature const & feature)
            : feature_(feature)
            , free_pieces_(NULL)
        {
            boost::system::error_code ec;
            if (feature_.piece_num == 0) {
                feature_.piece_num = (feature_.packet_max_size + feature_.piece_size - 1) / feature_.piece_size;
            }
            if (feature_.block_size == 0) {
                feature_.block_size = memory_.align_page(feature_.piece_size * 128);
            } else {
                feature_.block_size = memory_.align_page(feature_.block_size);
            }
        }

        PacketBuffer::~PacketBuffer()
        {
            packets_.clear();
            locked_packets_.clear();
            free_packets_.clear();
            for (size_t i = 0; i < blocks_.size(); ++i) {
                memory_.free_block(blocks_[i], feature_.block_size);
            }
            blocks_.clear();
        }

        void * PacketBuffer::fetch(
            boost::uint32_t & size_out, 
            blocks_t & blocks)
        {
            assert(!packets_.empty());
            Packet * pkt = packets_.first();
            packets_.pop_front();
            PieceHeader * ph = pkt->pieces;
            size_t size = size_out = pkt->size;
            while (ph && size) {
                if (size > feature_.piece_size) {
                    blocks.push_back(boost::asio::buffer(ph + 1, feature_.piece_size));
                    size -= feature_.piece_size;
                } else {
                    blocks.push_back(boost::asio::buffer(ph + 1, size));
                    size = 0;
                }
                ph = ph->next_piece;
            }
            locked_packets_.push_back(pkt);
            return ph;
        }

        void PacketBuffer::putback(
            void * mem)
        {
            Packet * pkt = (Packet *)mem;
            free_packet(pkt);
        }

        void PacketBuffer::clear()
        {
            buffers_.clear();
        }

        bool PacketBuffer::prepare(
            boost::system::error_code & ec)
        {
            while (buffers_.size() < feature_.piece_num) {
                PieceHeader * ph = alloc_piece();
                if (ph  == NULL) {
                    ec = boost::asio::error::no_buffer_space;
                    return false;
                }
                buffers_.push_back(boost::asio::buffer(ph + 1, feature_.piece_size));
            }
            return true;
        }

        void PacketBuffer::commit(
            size_t size)
        {
            Packet * pkt = alloc_packet();
            pkt->size = size;
            PieceHeader ** pph = &pkt->pieces;
            while (size) {
                PieceHeader * ph = 
                    boost::asio::buffer_cast<PieceHeader *>(*buffers_.begin()) - 1;
                buffers_.pop_front();
                *pph = ph;
                pph = &ph->next_piece;
                if (size > feature_.piece_size) {
                    size -= feature_.piece_size;
                } else {
                    size = 0;
                }
            }
            *pph = NULL;
            packets_.push_back(pkt);
        }

        PacketBuffer::PieceHeader * PacketBuffer::alloc_piece()
        {
            if (free_pieces_ == NULL) {
                void * ptr = memory_.alloc_block(feature_.block_size);
                if (ptr == NULL) {
                    assert(false);
                    return NULL;
                }
                blocks_.push_back(ptr);
                free_pieces_ = (PieceHeader *)ptr;
                void * end = (char *)ptr + feature_.block_size - feature_.piece_size - sizeof(PieceHeader);
                while (ptr <= end) {
                    PieceHeader * ph = (PieceHeader *)ptr;
                    ptr = ph->next_piece = (PieceHeader *)((char *)(ph + 1) + feature_.piece_size);
                }
                PieceHeader * ph = (PieceHeader *)((char *)ptr - feature_.piece_size - sizeof(PieceHeader));
                ph->next_piece = NULL;
            }
            PieceHeader * ph = free_pieces_;
            free_pieces_ = free_pieces_->next_piece;
            return ph;
        }

        void PacketBuffer::free_piece(
            PieceHeader * ptr)
        {
            ptr->next_piece = free_pieces_;
            free_pieces_ = ptr;
        }

        PacketBuffer::Packet * PacketBuffer::alloc_packet()
        {
            if (free_packets_.empty()) {
                void * ptr = memory_.alloc_block(feature_.block_size);
                if (ptr == NULL) {
                    assert(false);
                    return NULL;
                }
                blocks_.push_back(ptr);
                void * end = (char *)ptr + feature_.block_size - sizeof(Packet);
                while (ptr <= end) {
                    Packet * pkt = (Packet *)ptr;
                    free_packets_.push_back(pkt);
                    ptr = ++pkt;
                }
            }
            Packet * pkt = free_packets_.first();
            free_packets_.pop_front();
            return pkt;
        }

        void PacketBuffer::free_packet(
            Packet * pkt)
        {
            pkt->unlink();
            free_packets_.push_front(pkt);
        }

    } // namespace data
} // namespace ppbox
