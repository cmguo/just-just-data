// SinkBase.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/SinkBase.h"

namespace ppbox
{
    namespace data
    {

        SinkBase::SinkBase(
            boost::asio::io_service & io_svc)
            : util::stream::Sink(io_svc)
        {
        }

        SinkBase::~SinkBase()
        {
        }

        boost::system::error_code SinkBase::set_non_block(
            bool non_block, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        boost::system::error_code SinkBase::set_time_out(
            boost::uint32_t time_out, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
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

    } // namespace data
} // namespace ppbox
