// SourceBase.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/SourceBase.h"

namespace ppbox
{
    namespace data
    {

        SourceBase::SourceBase(
            boost::asio::io_service & io_svc)
            : util::stream::Source(io_svc)
        {
        }

        SourceBase::~SourceBase()
        {
        }

        boost::system::error_code SourceBase::cancel(
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        boost::system::error_code SourceBase::set_non_block(
            bool non_block, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        boost::system::error_code SourceBase::set_time_out(
            boost::uint32_t time_out, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        bool SourceBase::continuable(
            boost::system::error_code const & ec)
        {
            return false;
        }

    } // namespace data
} // namespace ppbox
