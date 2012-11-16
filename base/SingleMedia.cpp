// SingleMedia.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/SingleMedia.h"

#include <boost/bind.hpp>

namespace ppbox 
{
    namespace data 
    {

        void SingleMedia::async_open(
            response_type const & resp)
        {
            boost::system::error_code ec;
            get_io_service().post(boost::bind(resp, ec));
        }

        void SingleMedia::cancel(
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
        }

        void SingleMedia::close(
            boost::system::error_code & ec)
        {
            ec.clear();
        }

        bool SingleMedia::get_info(
            MediaInfo & info,
            boost::system::error_code & ec) const
        {
            info.format = url_.param("format");
            if (info.format.empty()) {
                std::string::size_type pos = url_.path().rfind(".");
                if (pos != std::string::npos) {
                    info.format = url_.path().substr(pos + 1);
                }
            }
            ec.clear();
            return true;
        }

    } // namespace data
} // namespace ppbox