// FullStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/FullStrategy.h"

namespace ppbox
{
    namespace data
    {

        FullStrategy::FullStrategy(
            MediaBase & media)
            : SegmentStrategy(media)
        {
        }

        FullStrategy::~FullStrategy()
        {
        }

        void FullStrategy::byte_range(
            SegmentPosition const & info, 
            SegmentRange & range)
        {
            range.beg = 0;
            range.end = info.size;
        }

    }
}
