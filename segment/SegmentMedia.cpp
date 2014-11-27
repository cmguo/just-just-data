// SegmentMedia.cpp

#include "just/data/Common.h"
#include "just/data/segment/SegmentMedia.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.data.SegmentMedia", framework::logger::Debug);

namespace just
{
    namespace data
    {

        SegmentMedia::SegmentMedia(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
            : MediaBase(io_svc, url)
        {
        }

        SegmentMedia::~SegmentMedia()
        {
        }

        bool SegmentMedia::get_basic_info(
            MediaBasicInfo & info,
            boost::system::error_code & ec) const
        {
            info.format_type = url_.param("format");
            info.flags = MediaBasicInfo::f_segment;
            ec.clear();
            return true;
        }

    } // data
} // just
