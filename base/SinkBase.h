// SinkBase.h

#ifndef _PPBOX_DATA_BASE_SINK_BASE_H_
#define _PPBOX_DATA_BASE_SINK_BASE_H_

#include "ppbox/data/base/DataBase.h"

#include <util/stream/Sink.h>

namespace ppbox
{
    namespace data
    {

        class SinkBase
            : public util::stream::Sink
        {
        public:
            SinkBase(
                boost::asio::io_service & io_svc);

            virtual ~SinkBase();

        public:
            virtual boost::system::error_code cancel(
                boost::system::error_code & ec);

        public:
            virtual boost::system::error_code set_non_block(
                bool non_block, 
                boost::system::error_code & ec);

            virtual boost::system::error_code set_time_out(
                boost::uint32_t time_out, 
                boost::system::error_code & ec);
        };

    } // namespace data
} // namespace ppbox

#define PPBOX_REGISTER_SINK(k, c) PPBOX_REGISTER_CLASS(k, c)

#endif // _PPBOX_DATA_BASE_SINK_BASE_H_
