// Strategy.h

#ifndef _PPBOX_DATA_SEGMENT_POSITION_H_
#define _PPBOX_DATA_SEGMENT_POSITION_H_

#include <ppbox/data/MediaBase.h>

namespace ppbox
{
    namespace data
    {

        struct SegmentRange
        {
            SegmentRange()
                : big_offset(0)
                , beg(0)
                , end(0)
                , pos(0)
            {
            }

            // ���зֶε��ۻ�ƫ����
            boost::uint64_t big_offset;
            boost::uint64_t beg;
            boost::uint64_t end;
            boost::uint64_t pos;

            boost::uint64_t big_beg() const
            {
                return big_offset + beg;
            }

            boost::uint64_t big_end() const
            {
                return end == invalid_size ? invalid_size : big_offset + end;
            }

            boost::uint64_t big_pos() const
            {
                return big_offset + pos;
            }

            void before_next()
            {
                big_offset += end;
            }

            void after_next()
            {
                big_offset -= beg;
            }
        };

        struct SegmentPosition
            : SegmentInfo
        {
            SegmentPosition()
                : index(boost::uint32_t(-1))
                , item_context(NULL)
            {
            }

            boost::uint32_t index;
            framework::string::Url url;
            SegmentRange byte_range;
            SegmentRange time_range;
            void * item_context;    // �����б�Ľڵ���Ϣ

            friend bool operator==(
                SegmentPosition const & l, 
                SegmentPosition const & r)
            {
                return l.byte_range.big_beg() == r.byte_range.big_beg();
            }

            friend bool operator!=(
                SegmentPosition const & l, 
                SegmentPosition const & r)
            {
                return !(l == r);
            }
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_SEGMENT_POSITION_H_
