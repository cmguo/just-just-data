// BodySourceStrategy.h

#ifndef     _PPBOX_DATA_BODY_SOURCE_STRATEGY_
#define     _PPBOX_DATA_BODY_SOURCE_STRATEGY_

#include <ppbox/data/strategy/SourceStrategy.h>

namespace ppbox
{
    namespace data
    {

        class BodySourceStrategy
            : public SourceStrategy
        {
        public:
            BodySourceStrategy(
                std::vector<SegmentInfoEx> const & segments);

            ~BodySourceStrategy();

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

        };

        PPBOX_REGISTER_STRATEGY(body, BodySourceStrategy);

    }
}

#endif // End _PPBOX_DATA_BODY_SOURCE_STRATEGY_
