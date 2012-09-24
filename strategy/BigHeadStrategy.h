// BigHeadStrategy.h

#ifndef     _PPBOX_DATA_BIG_HAED_STRATEGY_
#define     _PPBOX_DATA_BIG_HAED_STRATEGY_

#include <ppbox/data/strategy/Strategy.h>

namespace ppbox
{
    namespace data
    {

        class BigHeadStrategy
            : public Strategy
        {
        public:
            BigHeadStrategy(MediaBase & media);

            virtual ~BigHeadStrategy();

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

        private:
            SegmentInfoEx info_;

        };

        PPBOX_REGISTER_STRATEGY(bigh, BigHeadStrategy);

    }
}

#endif // End _PPBOX_DATA_BIG_HAED_STRATEGY_
