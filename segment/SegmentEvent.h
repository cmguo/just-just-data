// SegmentEvent.h

#ifndef _PPBOX_DATA_SEGMENT_SOURCE_EVENT_H_
#define _PPBOX_DATA_SEGMENT_SOURCE_EVENT_H_

#include <util/event/Event.h>

namespace ppbox
{
    namespace data
    {

        struct SegmentPosition;

        class SegmentEvent
            : public util::event::Event
        {
        public:
            SegmentPosition const & segment;

            SegmentEvent(
                SegmentPosition const & segment)
                : segment(segment)
            {
            }
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_SEGMENT_SOURCE_EVENT_H_
