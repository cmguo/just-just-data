// Strategy.h

#ifndef _JUST_DATA_SEGMENT_SEGMENT_POSITION_H_
#define _JUST_DATA_SEGMENT_SEGMENT_POSITION_H_

#include <just/data/segment/SegmentInfo.h>

namespace just
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

            // 所有分段的累积偏移量
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

            void follow(
                SegmentRange const & before)
            {
                big_offset = before.big_end() - beg;
            }

            bool followed_by(
                SegmentRange const & after) const
            {
                return big_end() == after.big_beg();
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
            SegmentRange byte_range;
            SegmentRange time_range;
            void * item_context;    // 树、列表的节点信息

            friend bool operator==(
                SegmentPosition const & l, 
                SegmentPosition const & r)
            {
                return l.is_same_segment(r);
            }

            friend bool operator!=(
                SegmentPosition const & l, 
                SegmentPosition const & r)
            {
                return !(l == r);
            }

            bool valid() const
            {
                return item_context != NULL;
            }

            bool is_same_segment( 
                SegmentPosition const & r) const
            {
                return (this->item_context == r.item_context 
                    && this->index == r.index);
            }
        };

    } // namespace data
} // namespace just

#endif // _JUST_DATA_SEGMENT_SEGMENT_POSITION_H_
