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

    } // namespace data
} // namespace ppbox
