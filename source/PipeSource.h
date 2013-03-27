// PipeSource.h

#ifndef _PPBOX_DATA_SOURCE_PIPE_SOURCE_H_
#define _PPBOX_DATA_SOURCE_PIPE_SOURCE_H_

#include <ppbox/data/base/UrlSource.h>

#ifndef BOOST_WINDOWS_API
#  include <boost/asio/posix/stream_descriptor.hpp>
typedef boost::asio::posix::stream_descriptor source_descriptor;
#else
#  include <boost/asio/windows/stream_handle.hpp>
#  if (defined BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE)
typedef boost::asio::windows::stream_handle source_descriptor;
#  else
#    define PPBOX_NO_PIPE_SOURCE
struct source_descriptor
{
	typedef HANDLE native_type;
};
#  endif
#endif

namespace ppbox
{
    namespace data
    {

        class PipeSource
            : public UrlSource
        {
        public:
            typedef source_descriptor::native_type native_descriptor;

        public:
            PipeSource(
                boost::asio::io_service & io_svc);

            virtual ~PipeSource();

        public:
            virtual boost::system::error_code open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec);

            virtual bool is_open(
                boost::system::error_code & ec);

            virtual boost::system::error_code close(
                boost::system::error_code & ec);

        public:
            virtual boost::system::error_code set_non_block(
                bool non_block, 
                boost::system::error_code & ec);

            virtual bool continuable(
                boost::system::error_code const & ec);

        private:
            // implement util::stream::Source
            virtual std::size_t private_read_some(
                buffers_t const & buffers,
                boost::system::error_code & ec);

        private:
            source_descriptor descriptor_;
            bool is_open_;
        };

#ifndef PPBOX_NO_PIPE_SOURCE
        PPBOX_REGISTER_URL_SOURCE("pipe", PipeSource);
#endif

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_SOURCE_PIPE_SOURCE_H_
