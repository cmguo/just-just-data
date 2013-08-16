// UrlSource.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/UrlSource.h"
#include "ppbox/data/base/Error.h"

#include <boost/bind.hpp>

namespace ppbox
{
    namespace data
    {

        boost::system::error_code UrlSource::error_not_found()
        {
            return error::bad_source_type;
        }

        UrlSource * UrlSource::create(
            boost::asio::io_service & io_svc,
            std::string const & proto, 
            boost::system::error_code & ec)
        {
            return factory_type::create(proto, io_svc, ec);
        }

        UrlSource::UrlSource(
            boost::asio::io_service & io_svc)
            : SourceBase(io_svc)
        {
        }

        UrlSource::~UrlSource()
        {
        }

        boost::system::error_code UrlSource::open(
            framework::string::Url const & url,
            boost::system::error_code & ec)
        {
            return open(url, 0, invalid_size, ec);
        }

        void UrlSource::async_open(
            framework::string::Url const & url,
            boost::uint64_t beg, 
            boost::uint64_t end, 
            response_type const & resp)
        {
            boost::system::error_code ec;
            open(url, beg, end, ec);
            get_io_service().post(boost::bind(resp, ec));
        }

        void UrlSource::async_open(
            framework::string::Url const & url,
            response_type const & resp)
        {
            async_open(url, 0, invalid_size, resp);
        }

        boost::uint64_t UrlSource::total(
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::no_data;
            return 0;
        }

        bool UrlSource::recoverable(
            boost::system::error_code const & ec)
        {
            return false;
        }

    } // namespace data
} // namespace ppbox
