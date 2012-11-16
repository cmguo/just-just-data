// SegmentMedia.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data//segment/SegmentMedia.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.SegmentMedia", framework::logger::Debug);

namespace ppbox
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

    } // data
} // ppbox
