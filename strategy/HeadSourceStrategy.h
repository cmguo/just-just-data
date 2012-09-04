// HeadSourceStrategy.h

#ifndef     _PPBOX_DATA_HEAD_SOURCE_STRATEGY_
#define     _PPBOX_DATA_HEAD_SOURCE_STRATEGY_

#include <ppbox/data/strategy/SourceStrategy.h>

namespace ppbox
{
    namespace data
    {

        class HeadSourceStrategy
            : public SourceStrategy
        {
        public:
            HeadSourceStrategy(
                std::vector<SegmentInfoEx> const & segments,
                VideoInfo const & video_info);

            ~HeadSourceStrategy();

            virtual bool next_segment(
                bool is_next,
                SegmentInfoEx & info);

            virtual boost::system::error_code on_seek(
                size_t offset,
                SegmentInfoEx & info, 
                boost::system::error_code & ec);

            virtual boost::system::error_code on_seek(
                boost::uint32_t segment_index,
                size_t offset, 
                SegmentInfoEx & info, 
                boost::system::error_code & ec);

            virtual std::size_t size(void);

        };

        PPBOX_REGISTER_STRATEGY(head, HeadSourceStrategy);

    }
}

#endif // End _PPBOX_DATA_HEAD_SOURCE_STRATEGY_
