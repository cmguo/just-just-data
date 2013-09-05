// DataEvent.h

#ifndef _PPBOX_DATA_BASE_DATA_EVENT_H_
#define _PPBOX_DATA_BASE_DATA_EVENT_H_

#include <util/event/Event.h>

namespace ppbox
{
    namespace data
    {

        struct DataStat;

        class DataStatEvent
            : public util::event::Event
        {
        public:
            DataStat const & stat;

            DataStatEvent(
                DataStat const & stat)
                : stat(stat)
            {
            }
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_BASE_DATA_EVENT_H_
