// SourceEvent.h

#ifndef _PPBOX_DATA_BASE_SOURCE_EVENT_H_
#define _PPBOX_DATA_BASE_SOURCE_EVENT_H_

#include <util/event/Event.h>

namespace ppbox
{
    namespace data
    {

        struct SourceStatisticData;

        class SourceStatisticEvent
            : public util::event::Event
        {
        public:
            SourceStatisticData const & stat;

            SourceStatisticEvent(
                SourceStatisticData const & stat)
                : stat(stat)
            {
            }
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_BASE_SOURCE_EVENT_H_
