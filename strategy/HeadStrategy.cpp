// HeadStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/HeadStrategy.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {

        HeadStrategy::HeadStrategy(
            MediaBase & media)
            : SegmentStrategy(media)
        {
        }

        HeadStrategy::~HeadStrategy()
        {
        }

        void HeadStrategy::byte_range(
            SegmentInfo const & info, 
            SegmentRange & range)
        {
            range.beg = 0;
            range.end = info.head_size;
        }

    }
}
