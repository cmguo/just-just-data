// MediaBase.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/MediaBase.h"
#include "ppbox/data/SourceBase.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
using namespace framework::logger;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.MediaBase", Debug);

namespace ppbox
{
    namespace data
    {

        MediaBase * MediaBase::create(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
        {
            MediaBase * media = factory_type::create(url.protocol(), io_svc);
            if (media)
                media->set_url(url);
            return media;
        }

        MediaBase::MediaBase(
            boost::asio::io_service & io_svc)
            : io_svc_(io_svc)
        {
        }

        MediaBase::~MediaBase()
        {
        }

        void MediaBase::set_url(
            framework::string::Url const &url)
        {
            url_ = url;
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
