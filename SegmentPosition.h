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
                return big_offset + end;
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

            friend bool operator<(
                SegmentRange const & l, 
                SegmentRange const & r)
            {
                return (l.beg < r.beg);
            }

            friend bool operator==(
                SegmentRange const & l, 
                SegmentRange const & r)
            {
                return (l.beg == r.beg && l.end == r.end);
            }

            friend bool operator!=(
                SegmentRange const & l, 
                SegmentRange const & r)
            {
                return !(l == r);
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
            void * item_context;    // 树、列表的节点信息

            friend bool operator<(
                SegmentPosition const & l, 
                SegmentPosition const & r)
            {
                return (l.index < r.index || 
                    (l.index == r.index && l.byte_range < r.byte_range));
            }

            friend bool operator==(
                SegmentPosition const & l, 
                SegmentPosition const & r)
            {
                return (l.index == r.index && l.byte_range == r.byte_range);
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
