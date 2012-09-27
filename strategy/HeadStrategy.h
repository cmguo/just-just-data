// HeadStrategy.h

#ifndef     _PPBOX_DATA_HEAD_STRATEGY_
#define     _PPBOX_DATA_HEAD_STRATEGY_

#include <ppbox/data/strategy/Strategy.h>

namespace ppbox
{
    namespace data
    {

        class HeadStrategy
            : public Strategy
        {
        public:
            HeadStrategy(MediaBase & media);

            virtual ~HeadStrategy();

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

            //virtual boost::system::error_code seek(
            //    boost::uint32_t segment_index,
            //    size_t offset, 
            //    SegmentInfoEx & info, 
            //    boost::system::error_code & ec);

            virtual std::size_t size(void);

        };

        PPBOX_REGISTER_STRATEGY(head, HeadStrategy);

    }
}

#endif // End _PPBOX_DATA_HEAD_STRATEGY_
