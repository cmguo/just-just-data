// UrlSource.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/UrlSource.h"
#include "ppbox/data/base/MediaBase.h"

#include <boost/bind.hpp>

namespace ppbox
{
    namespace data
    {

        UrlSource * UrlSource::create(
            boost::asio::io_service & io_svc,
            std::string const & proto)
        {
            return factory_type::create(proto, io_svc);
        }

        UrlSource::UrlSource(
            boost::asio::io_service & io_svc)
            : SourceBase(io_svc)
        {
        }

        UrlSource::~UrlSource()
        {
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
