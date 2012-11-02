// BytesStream.h

#ifndef _PPBOX_DATA_BYTES_STREAM_H_
#define _PPBOX_DATA_BYTES_STREAM_H_

#include "ppbox/data/SegmentBuffer.h"

#include <util/buffers/BufferSize.h>
#include <util/buffers/StlBuffer.h>

#include <boost/asio/buffer.hpp>

namespace ppbox
{
    namespace data
    {

        class BytesStream
            : public util::buffers::StlStream<boost::uint8_t>
        {
        public:
            friend class SegmentBuffer;

            typedef SegmentBuffer::segment_t segment_t;

            typedef util::buffers::StlBuffer<
                util::buffers::detail::_read, boost::uint8_t, std::char_traits<boost::uint8_t> > buffer_type;

        public:
            BytesStream(
                SegmentBuffer & buffer)
                : buffer_(buffer)
                , segment_(NULL)
                , pos_(0)
                , buf_(*this)
            {
                setg(NULL, NULL, NULL);
                // TODO: �п����Ѿ���������
            }

        private:
            bool update(
                SegmentBuffer::PositionType::Enum type, 
                pos_type pos)
            {
                boost::uint64_t pos64 = pos;
                boost::uint64_t off32 = 0;
                boost::asio::const_buffer buf;;
                boost::system::error_code ec;
                ec = buffer_.segment_buffer(*segment_, type, pos64, off32, buf);
                pos_ = pos64;
                buf_ = buf;
                gbump(off32);
                return !ec;
            }

            void update()
            {
                pos_type pos = pos_ + off_type(gptr() - eback());
                update(SegmentBuffer::PositionType::set, pos);
            }

            void close()
            {
                setg(NULL, NULL, NULL);
                pos_ = 0;
            }

        private:
            boost::uint64_t position()
            {
                pos_type pos = pos_ + off_type(gptr() - eback());
                return pos;
            }

            void drop_all()
            {
                update(SegmentBuffer::PositionType::set, 0);
            }

            void seek(
                boost::uint64_t pos)
            {
                update(SegmentBuffer::PositionType::set, pos);
            }

            void change_to(
                SegmentBuffer::segment_t & segment)
            {
                segment_ = &segment;
                update(SegmentBuffer::PositionType::set, segment.byte_range.pos);
            }

        private:
            virtual int_type underflow()
            {
                pos_type pos = pos_ + gptr() - eback();
                if (update(SegmentBuffer::PositionType::set, pos) && (gptr() < egptr())) {
                    return *gptr();
                } else {
                    return traits_type::eof();
                }
            }

            virtual pos_type seekoff(
                off_type off, 
                std::ios_base::seekdir dir,
                std::ios_base::openmode mode)
            {
                if (mode != std::ios_base::in) {
                    return pos_type(-1);
                }
                if (dir == std::ios_base::cur) {
                    pos_type pos = pos_ + gptr() - eback();
                    if (off == 0) {
                        return pos;
                    }
                    pos += off;
                    return seekpos(pos, mode);
                } else if (dir == std::ios_base::beg) {
                    return seekpos(off, mode);
                } else if (dir == std::ios_base::end) {
                    assert(off <= 0);
                    if (!update(SegmentBuffer::PositionType::end, pos_type(-off))) {
                        return pos_type(-1);
                    }
                    return pos_ + gptr() - eback();
                } else {
                    return pos_type(-1);
                }
            }

            virtual pos_type seekpos(
                pos_type position, 
                std::ios_base::openmode mode)
            {
                assert(position != pos_type(-1));
                if (mode != std::ios_base::in) {
                    return pos_type(-1);// ģʽ����
                }
                if (!update(SegmentBuffer::PositionType::set, position)) {
                    return pos_type(-1);// ��Чλ��֮ǰ
                }
                return position;
            }

        private:
            SegmentBuffer & buffer_;
            SegmentBuffer::segment_t * segment_;
            pos_type pos_;          // ��iter_��Ӧ�ֶεĿ�ͷ
            buffer_type buf_;       // ��ǰ���ڴ��
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_BYTES_STREAM_H_