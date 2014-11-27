// HeadStrategy.cpp

#include "just/data/Common.h"
#include "just/data/strategy/HeadStrategy.h"

using namespace boost::system;

namespace just
{
    namespace data
    {

        HeadStrategy::HeadStrategy(
            SegmentMedia & media)
            : SegmentStrategy(media)
        {
        }

        HeadStrategy::~HeadStrategy()
        {
        }

        void HeadStrategy::byte_range(
            SegmentPosition const & info, 
            SegmentRange & range)
        {
            range.beg = 0;
            range.end = info.head_size;
        }

        void HeadStrategy::time_range(
            SegmentPosition const & info, 
            SegmentRange & range)
        {
            range.beg = 0;
            range.end = info.head_size;
        }

    }
}
