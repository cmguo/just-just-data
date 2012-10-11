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
        {
        }

        BodyStrategy::~BodyStrategy()
        {
        }

        void BodyStrategy::byte_range(
            SegmentInfo const & info, 
            SegmentRange & range)
        {
            range.beg = info.head_size;
            range.end = info.size;
        }

    } // namespace data
} // namespace ppbox
