// BodyStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/BodyStrategy.h"

namespace ppbox
{
    namespace data
    {

        BodyStrategy::BodyStrategy(
            MediaBase & media)
            : SegmentStrategy(media)
            , is_drop_redundancy_(false)
        {
        }

        BodyStrategy::~BodyStrategy()
        {
        }

        void BodyStrategy::drop_redundancy(void)
        {
            is_drop_redundancy_ = true;
        }

        void BodyStrategy::byte_range(
            SegmentPosition const & info, 
            SegmentRange & range)
        {
            range.beg = info.head_size;
            range.end = info.size;
            if (is_drop_redundancy_ && 
                info.index < (media_.segment_count() - 1)) {
                SegmentInfo next;
                media_.segment_info(info.index + 1, next);
                assert(info.size >= (next.offset - info.offset));
                range.end = 
                    info.head_size + next.offset - info.offset;
            }
        }

    } // namespace data
} // namespace ppbox
