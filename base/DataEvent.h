// DataEvent.h

#ifndef _JUST_DATA_BASE_DATA_EVENT_H_
#define _JUST_DATA_BASE_DATA_EVENT_H_

#include <util/event/Event.h>

namespace just
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
} // namespace just

#endif // _JUST_DATA_BASE_DATA_EVENT_H_
