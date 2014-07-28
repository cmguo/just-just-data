// StreamEvent.h

#ifndef _PPBOX_DATA_BASE_STREAM_EVENT_H_
#define _PPBOX_DATA_BASE_STREAM_EVENT_H_

#include <util/event/Event.h>

namespace ppbox
{
    namespace data
    {


        class StreamStatistic;

        class StreamEvent
            : public util::event::Event
        {
        public:
            StreamStatistic const & stat;

            StreamEvent(
                StreamStatistic const & stat)
                : stat(stat)
            {
            }

        private:
            StreamEvent(
                StreamEvent const & stat);

        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_BASE_DATA_EVENT_H_
