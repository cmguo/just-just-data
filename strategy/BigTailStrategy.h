// BigTailStrategy.h

#ifndef     _PPBOX_DATA_BIG_TAIL_STRATEGY_
#define     _PPBOX_DATA_BIG_TAIL_STRATEGY_

#include <ppbox/data/strategy/Strategy.h>

namespace ppbox
{
    namespace data
    {

        class BigTailStrategy
            : public Strategy
        {
        public:
            BigTailStrategy(MediaBase & media);

            virtual ~BigTailStrategy();

            virtual bool next_segment(
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

        private:
            bool next_flag_;
            SegmentInfoEx info_;

        };

        PPBOX_REGISTER_STRATEGY(bigt, BigTailStrategy);

    }
}

#endif // End _PPBOX_DATA_BIG_TAIL_STRATEGY_
