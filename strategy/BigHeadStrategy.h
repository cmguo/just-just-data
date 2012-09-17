// CdnHeadStrategy.h

#ifndef     _PPBOX_DATA_CDN_HAED_STRATEGY_
#define     _PPBOX_DATA_CDN_HAED_STRATEGY_

#include <ppbox/data/strategy/SourceStrategy.h>

namespace ppbox
{
    namespace data
    {

        class CdnHeadStrategy
            : public SourceStrategy
        {
        public:
            CdnHeadStrategy(
                std::vector<SegmentInfoEx> const & segments,
                MediaInfo const & video_info);

            virtual ~CdnHeadStrategy();

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

        PPBOX_REGISTER_STRATEGY(cdnh, CdnHeadStrategy);

    }
}

#endif // End _PPBOX_DATA_CDN_HAED_STRATEGY_
