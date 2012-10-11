// BodyStrategy.h

#ifndef _PPBOX_DATA_STRATEGY_BODY_STRATEGY_H_
#define _PPBOX_DATA_STRATEGY_BODY_STRATEGY_H_

#include <ppbox/data/SegmentStrategy.h>

namespace ppbox
{
    namespace data
    {

        class BodyStrategy
            : public SegmentStrategy
        {
        public:
            BodyStrategy(
                MediaBase & media);

            virtual ~BodyStrategy();

        private:
            virtual void byte_range(
                SegmentInfo const & info, 
                SegmentRange & range);
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_STRATEGY_BODY_STRATEGY_H_
