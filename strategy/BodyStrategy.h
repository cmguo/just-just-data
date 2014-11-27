// BodyStrategy.h

#ifndef _JUST_DATA_STRATEGY_BODY_STRATEGY_H_
#define _JUST_DATA_STRATEGY_BODY_STRATEGY_H_

#include <just/data/segment/SegmentStrategy.h>

namespace just
{
    namespace data
    {

        class BodyStrategy
            : public SegmentStrategy
        {
        public:
            BodyStrategy(
                SegmentMedia & media);

            virtual ~BodyStrategy();

        private:
            virtual void byte_range(
                SegmentPosition const & info, 
                SegmentRange & range);

            virtual void time_range(
                SegmentPosition const & info, 
                SegmentRange & range);
        };

    } // namespace data
} // namespace just

#endif // _JUST_DATA_STRATEGY_BODY_STRATEGY_H_
