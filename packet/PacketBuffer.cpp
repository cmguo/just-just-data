// PacketBuffer.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/packet/PacketBuffer.h"

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
            , write_offset_(0)
            , read_offset_(0)
            , write_index_(0)
            , read_index_(0)
            , locked_count_(0)
            , free_count_(0)
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
            if (feature_.prepare_size == 0) {
                feature_.prepare_size = 1024 * 10;
            }
        }

        PacketBuffer::~PacketBuffer()
        {
            clear();
            free_packets_.clear();
            buffers_.clear();
            for (size_t i = 0; i < blocks_.size(); ++i) {
                memory_.free_block(blocks_[i], feature_.block_size);
            }
            blocks_.clear();
        }

        void PacketBuffer::putback(
            MemoryLock * mlock)
        {
            while (!mlock->join.empty()) {
                MemoryLock * l = mlock->join.first();
                l->unlink();
                putback(l);
            }
            mlock->unlink();
            free_packet(mlock);
            --locked_count_;
        }

        void PacketBuffer::clear()
        {
            packets_.clear();
            assert(locked_packets_.empty());
            locked_packets_.clear();
            read_offset_ = write_offset_ = 0;
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
            PieceHeader ** pph = (PieceHeader **)&pkt->pointer;
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
            write_offset_ += pkt->size;
            ++write_index_;
        }

        PacketBuffer::PieceHeader * PacketBuffer::alloc_piece()
        {
            if (free_pieces_ == NULL) {
                if (feature_.block_size * (blocks_.size() + 1) > feature_.buffer_size)
                    return NULL;
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
                if (feature_.block_size * (blocks_.size() + 1) > feature_.buffer_size)
                    return NULL;
                void * ptr = memory_.alloc_block(feature_.block_size);
                if (ptr == NULL) {
                    assert(false);
                    return NULL;
                }
                blocks_.push_back(ptr);
                void * end = (char *)ptr + feature_.block_size - sizeof(Packet);
                while (ptr <= end) {
                    Packet * pkt = new (ptr) Packet;
                    free_packets_.push_back(pkt);
                    ++free_count_;
                    ptr = ++pkt;
                }
            }
            Packet * pkt = free_packets_.first();
            free_packets_.pop_front();
            --free_count_;
            return pkt;
        }

        void PacketBuffer::free_packet(
            Packet * pkt)
        {
            while (pkt->pointer) {
                PieceHeader * p = (PieceHeader *)pkt->pointer;
                pkt->pointer = p->next_piece;
                free_piece(p);
            }
            free_packets_.push_front(pkt);
            ++free_count_;
        }

    } // namespace data
} // namespace ppbox
