// SourceBase.h

#ifndef _PPBOX_DATA_BASE_SOURCE_BASE_H_
#define _PPBOX_DATA_BASE_SOURCE_BASE_H_

#include "ppbox/data/base/DataBase.h"

#include <util/stream/Source.h>

namespace ppbox
{
    namespace data
    {

        class SourceBase
            : public util::stream::Source
        {
        public:
            SourceBase(
                boost::asio::io_service & io_svc);

            virtual ~SourceBase();

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

#endif // _PPBOX_DATA_BASE_SOURCE_BASE_H_
