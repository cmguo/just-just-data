// MediaBase.cpp

#include "just/data/Common.h"
#include "just/data/base/MediaBase.h"
#include "just/data/base/Error.h"

#include "just/data/single/SingleMedia.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.data.MediaBase", framework::logger::Debug);

namespace just
{
    namespace data
    {

        MediaBase * MediaBase::create(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url, 
            boost::system::error_code & ec)
        {
            MediaBase * media = MediaProtocolFactory::create(url.protocol(), io_svc, url, ec);
            if (media == NULL) {
                std::string format = url.param("format");
                if (format.empty()) {
                    std::string::size_type pos = url.path().rfind(".");
                    if (pos != std::string::npos) {
                        format = url.path().substr(pos + 1);
                    }
                }
                if (!format.empty()) {
                    media = MediaFormatFactory::create(format, io_svc, url, ec);
                }
            }
            if (media == NULL) {
                ec.clear();
                media = new SingleMedia(io_svc, url);
            }
            return media;
        }

        MediaBase::MediaBase(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
            : io_svc_(io_svc)
            , url_(url)
        {
        }

        MediaBase::~MediaBase()
        {
        }

        bool MediaBase::get_url(
            framework::string::Url & url,
            boost::system::error_code & ec) const
        {
            url = url_;
            ec.clear();
            return true;
        }

        boost::system::error_code MediaTraits::error_not_found()
        {
            return error::bad_media_type;
        }

    } // data
} // just
