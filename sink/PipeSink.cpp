// PipeSink.h

#include <ppbox/data/Common.h>
#include <ppbox/data/sink/PipeSink.h>

#include <framework/system/ErrorCode.h>

namespace ppbox
{
    namespace data
    {

        PipeSink::PipeSink(
            boost::asio::io_service & io_svc)
            : UrlSink(io_svc)
            , descriptor_(io_svc)
            , is_open_(false)
        {
        }

        PipeSink::~PipeSink()
        {
        }

        boost::system::error_code PipeSink::open(
            framework::string::Url const & url,
            boost::uint64_t beg, 
            boost::uint64_t end, 
            boost::system::error_code & ec)
        {
            if (is_open_)
                descriptor_.close(ec);
            if (beg != 0 && end != boost::uint64_t(-1)) {
                ec = framework::system::logic_error::unknown_error;
                return ec;
            }
            native_descriptor nd = framework::string::parse<native_descriptor>(url.path().substr(1));
            descriptor_.assign(nd);
            is_open_ = descriptor_.is_open();
            if (!is_open_) {
                ec = framework::system::last_system_error();
                if (!ec) {
                    ec = framework::system::logic_error::unknown_error;
                }
            }
            return ec;
        }

        bool PipeSink::is_open(
            boost::system::error_code & ec)
        {
            return is_open_;
        }

        boost::system::error_code PipeSink::close(
            boost::system::error_code & ec)
        {
            descriptor_.close();
            is_open_ = false;
            return ec = boost::system::error_code();
        }

        std::size_t PipeSink::private_write_some(
            buffers_t const & buffers,
            boost::system::error_code & ec)
        {
            return descriptor_.write_some(buffers, ec);
        }

        boost::system::error_code PipeSink::set_non_block(
            bool non_block, 
            boost::system::error_code & ec)
        {
#ifndef BOOST_WINDOWS_API
            boost::asio::posix::descriptor_base::non_blocking_io cmd(non_block);
            return descriptor_.io_control(cmd, ec);
#else
            return ec = boost::system::error_code();
#endif    
        }

        bool PipeSink::continuable(
            boost::system::error_code const & ec)
        {
            return ec == boost::asio::error::would_block;
        }

    } // namespace data
} // namespace ppbox
