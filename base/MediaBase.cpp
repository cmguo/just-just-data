// MediaBase.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/MediaBase.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.MediaBase", framework::logger::Debug);

namespace ppbox
{
    namespace data
    {

        MediaBase * MediaBase::create(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
        {
            MediaBase * media = factory_type::create(url.protocol(), io_svc, url);
            if (media == NULL) {
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

    } // data
} // ppbox
