// HeadStrategy.h

#ifndef _PPBOX_DATA_STRATEGY_HEAD_STRATEGY_H_
#define _PPBOX_DATA_STRATEGY_HEAD_STRATEGY_H_

#include <ppbox/data/SegmentStrategy.h>

namespace ppbox
{
    namespace data
    {

        class HeadStrategy
            : public SegmentStrategy
        {
        public:
            HeadStrategy(
                MediaBase & media);

            virtual ~HeadStrategy();

        private:
            virtual void byte_range(
                SegmentInfo const & info, 
                SegmentRange & range);
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_STRATEGY_HEAD_STRATEGY_H_
