//MediaBase.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/MediaBase.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
using namespace framework::logger;

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("MediaBase", 0);

namespace ppbox
{
    namespace data
    {

        std::map<std::string, MediaBase::register_type> & MediaBase::media_map()
        {
            static std::map<std::string, MediaBase::register_type> g_map;
            return g_map;
        }

        void MediaBase::register_media(
            std::string const & name,
            register_type func)
        {
            media_map().insert(std::make_pair(name, func));
            return;
        }

        MediaBase * MediaBase::create(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
        {
            std::map<std::string, register_type>::iterator iter = 
                media_map().find(url.protocol());
            if (media_map().end() == iter) {
                return NULL;
            } else {
                register_type func = iter->second;
                MediaBase * segment = func(io_svc);
                segment->set_url(url);
                return segment;
            }
        }

        void MediaBase::destory(
            MediaBase* & segment)
        {
            delete segment;
            segment = NULL;
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

    } // data
} // ppbox
