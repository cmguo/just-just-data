// UrlSink.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/UrlSink.h"

#include <boost/bind.hpp>

namespace ppbox
{
    namespace data
    {

        UrlSink * UrlSink::create(
            boost::asio::io_service & io_svc,
            std::string const & proto)
        {
            return factory_type::create(proto, io_svc);
        }

        UrlSink::UrlSink(
            boost::asio::io_service & io_svc)
            : SinkBase(io_svc)
        {
        }

        UrlSink::~UrlSink()
        {
        }

        boost::system::error_code UrlSink::open(
            framework::string::Url const & url,
            boost::system::error_code & ec)
        {
            return open(url, 0, invalid_size, ec);
        }

        void UrlSink::async_open(
            framework::string::Url const & url,
            boost::uint64_t beg, 
            boost::uint64_t end, 
            response_type const & resp)
        {
            boost::system::error_code ec;
            open(url, ec);
            get_io_service().post(boost::bind(resp, ec));
        }

        void UrlSink::async_open(
            framework::string::Url const & url,
            response_type const & resp)
        {
            async_open(url, 0, invalid_size, resp);
        }

        bool UrlSink::recoverable(
            boost::system::error_code const & ec)
        {
            return false;
        }

    } // namespace data
} // namespace ppbox
