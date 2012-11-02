// SinkBase.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/SinkBase.h"
#include "ppbox/data/MediaBase.h"

#include <boost/bind.hpp>

namespace ppbox
{
    namespace data
    {

        std::map<std::string, SinkBase::register_type> & SinkBase::sink_map()
        {
            static std::map<std::string, register_type> get_map;
            return get_map;
        }

        void SinkBase::register_sink(
            std::string const & name, 
            register_type func)
        {
            sink_map().insert(std::make_pair(name, func));
            return;
        }

        SinkBase * SinkBase::create(
            boost::asio::io_service & io_svc,
            std::string const & proto)
        {
            std::map<std::string, register_type>::const_iterator iter = sink_map().find(proto);
            if (iter == sink_map().end()) {
                return NULL;
            }
            return iter->second(io_svc);
        }

        void SinkBase::destory(
            SinkBase* & sink)
        {
            delete sink;
            sink = NULL;
        }

        SinkBase::SinkBase(
            boost::asio::io_service & io_svc)
            : util::stream::Sink(io_svc)
        {
        }

        SinkBase::~SinkBase()
        {
        }

        void SinkBase::async_open(
            framework::string::Url const & url,
            response_type const & resp)
        {
            boost::system::error_code ec;
            open(url, ec);
            get_io_service().post(boost::bind(resp, ec));
        }

        boost::system::error_code SinkBase::cancel(
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        bool SinkBase::continuable(
            boost::system::error_code const & ec)
        {
            return false;
        }

        bool SinkBase::recoverable(
            boost::system::error_code const & ec)
        {
            return false;
        }

    } // namespace data
} // namespace ppbox
