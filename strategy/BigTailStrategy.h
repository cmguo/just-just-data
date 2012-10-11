// BigTailStrategy.h

#ifndef _PPBOX_DATA_STRATEGY_BIG_TAIL_STRATEGY_H_
#define _PPBOX_DATA_STRATEGY_BIG_TAIL_STRATEGY_H_

#include <ppbox/data/SegmentStrategy.h>

namespace ppbox
{
    namespace data
    {

        class BigTailStrategy
            : public SegmentStrategy
        {
        public:
            BigTailStrategy(
                MediaBase & media);

            virtual ~BigTailStrategy();

        public:
            virtual bool next_segment(
                SegmentPosition & info, 
                boost::system::error_code & ec);
        };

    } // namespace data
} // namespace ppbox

#endif // End _PPBOX_DATA_BIG_TAIL_STRATEGY_
