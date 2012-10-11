// BigBodyStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/BigBodyStrategy.h"

namespace ppbox
{
    namespace data
    {

        BigBodyStrategy::BigBodyStrategy(
            MediaBase & media)
            : SegmentStrategy(media)
        {
        }

        BigBodyStrategy::~BigBodyStrategy()
        {
        }

        void BigBodyStrategy::byte_range(
            SegmentInfo const & info, 
            SegmentRange & range)
        {
            range.beg = info.head_size + (range.big_offset - info.offset);
            range.end = info.size;
        }

    } // namespace data
} // namespace ppbox
