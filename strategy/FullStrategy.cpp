// FullStrategy.cpp

#include "just/data/Common.h"
#include "just/data/strategy/FullStrategy.h"

namespace just
{
    namespace data
    {

        FullStrategy::FullStrategy(
            SegmentMedia & media)
            : SegmentStrategy(media)
        {
        }

        FullStrategy::~FullStrategy()
        {
        }

    } // namespace data
} // namespace just
