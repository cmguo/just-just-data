// SegmentEvent.h

#ifndef _JUST_DATA_SEGMENT_SOURCE_EVENT_H_
#define _JUST_DATA_SEGMENT_SOURCE_EVENT_H_

#include <util/event/Event.h>

namespace just
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
} // namespace just

#endif // _JUST_DATA_SEGMENT_SOURCE_EVENT_H_
