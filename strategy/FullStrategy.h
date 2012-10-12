// FullStrategy.h

#ifndef _PPBOX_DATA_STRATEGY_FULL_STRATEGY_H_
#define _PPBOX_DATA_STRATEGY_FULL_STRATEGY_H_

#include "ppbox/data/SegmentStrategy.h"

namespace ppbox
{
    namespace data
    {

        class FullStrategy
            : public SegmentStrategy
        {
        public:
            FullStrategy(
                MediaBase & media);

            virtual ~FullStrategy();

        private:
            virtual void byte_range(
                SegmentPosition const & info, 
                SegmentRange & range);
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_STRATEGY_FULL_STRATEGY_H_
