// FullStrategy.h

#ifndef     _PPBOX_DATA_FULL_STRATEGY_
#define     _PPBOX_DATA_FULL_STRATEGY_

#include "ppbox/data/strategy/Strategy.h"

namespace ppbox
{
    namespace data
    {

        class FullStrategy
            : public Strategy
        {
        public:
            FullStrategy(MediaBase & media);

            virtual ~FullStrategy();

            virtual bool next_segment(
                bool is_next,
                SegmentInfoEx & info);

            virtual boost::system::error_code byte_seek(
                size_t offset,
                SegmentInfoEx & info, 
                boost::system::error_code & ec);

            virtual boost::system::error_code time_seek(
                boost::uint32_t time_ms,
                SegmentInfoEx & info, 
                boost::system::error_code & ec);

            virtual std::size_t size(void);
        };

        PPBOX_REGISTER_STRATEGY(full, FullStrategy);
    }
}

#endif // End _PPBOX_DATA_FULL_STRATEGY_ 
