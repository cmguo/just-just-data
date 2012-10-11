// BigBodyStrategy.h

#ifndef _PPBOX_DATA_STRATEGY_BIG_BODY_STRATEGY_H_
#define _PPBOX_DATA_STRATEGY_BIG_BODY_STRATEGY_H_

#include <ppbox/data/SegmentStrategy.h>

namespace ppbox
{
    namespace data
    {

        class BigBodyStrategy
            : public SegmentStrategy
        {
        public:
            BigBodyStrategy(
                MediaBase & media);

            virtual ~BigBodyStrategy();

        private:
            virtual void byte_range(
                SegmentInfo const & info, 
                SegmentRange & range);
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_STRATEGY_BIG_BODY_STRATEGY_H_
