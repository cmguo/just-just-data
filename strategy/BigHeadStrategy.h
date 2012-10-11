// BigHeadStrategy.h

#ifndef _PPBOX_DATA_STRATEGY_BIG_HAED_STRATEGY_H_
#define _PPBOX_DATA_STRATEGY_BIG_HAED_STRATEGY_H_

#include <ppbox/data/SegmentStrategy.h>

namespace ppbox
{
    namespace data
    {

        class BigHeadStrategy
            : public SegmentStrategy
        {
        public:
            BigHeadStrategy(
                MediaBase & media);

            virtual ~BigHeadStrategy();

        public:
            virtual bool next_segment(
                SegmentPosition & info, 
                boost::system::error_code & ec);
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_STRATEGY_BIG_HAED_STRATEGY_H_
