// FullStrategy.h

#ifndef _JUST_DATA_STRATEGY_FULL_STRATEGY_H_
#define _JUST_DATA_STRATEGY_FULL_STRATEGY_H_

#include "just/data/segment/SegmentStrategy.h"

namespace just
{
    namespace data
    {

        class FullStrategy
            : public SegmentStrategy
        {
        public:
            FullStrategy(
                SegmentMedia & media);

            virtual ~FullStrategy();
        };

    } // namespace data
} // namespace just

#endif // _JUST_DATA_STRATEGY_FULL_STRATEGY_H_
