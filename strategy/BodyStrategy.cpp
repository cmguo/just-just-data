// BodyStrategy.cpp

#include "just/data/Common.h"
#include "just/data/strategy/BodyStrategy.h"

namespace just
{
    namespace data
    {

        BodyStrategy::BodyStrategy(
            SegmentMedia & media)
            : SegmentStrategy(media)
        {
        }

        BodyStrategy::~BodyStrategy()
        {
        }

        void BodyStrategy::byte_range(
            SegmentPosition const & info, 
            SegmentRange & range)
        {
            range.beg = info.head_size;
            range.end = info.size;
        }

        void BodyStrategy::time_range(
            SegmentPosition const & info, 
            SegmentRange & range)
        {
            range.beg = info.head_size;
            range.end = info.size;
        }

    } // namespace data
} // namespace just
