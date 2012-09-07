// CdnTailStrategy.h

#ifndef     _PPBOX_DATA_CDN_TAIL_STRATEGY_
#define     _PPBOX_DATA_CDN_TAIL_STRATEGY_

#include <ppbox/data/strategy/SourceStrategy.h>

namespace ppbox
{
    namespace data
    {

        class CdnTailStrategy
            : public SourceStrategy
        {
        public:
            CdnTailStrategy(
                std::vector<SegmentInfoEx> const & segments,
                VideoInfo const & video_info);

            virtual ~CdnTailStrategy();

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

        private:
            SegmentInfoEx info_;

        };

        PPBOX_REGISTER_STRATEGY(cdnt, CdnTailStrategy);

    }
}

#endif // End _PPBOX_DATA_CDN_TAIL_STRATEGY_
