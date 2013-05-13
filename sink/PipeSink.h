// PipeSink.h

#ifndef _PPBOX_DATA_SINK_PIPE_SINK_H_
#define _PPBOX_DATA_SINK_PIPE_SINK_H_

#include "ppbox/data/base/UrlSink.h"

#ifndef BOOST_WINDOWS_API
#  include <boost/asio/posix/stream_descriptor.hpp>
typedef boost::asio::posix::stream_descriptor sink_descriptor;
#else
#  include <boost/asio/windows/stream_handle.hpp>
#  if (defined BOOST_ASIO_HAS_WINDOWS_STREAM_HANDLE)
typedef boost::asio::windows::stream_handle sink_descriptor;
#  else
#    define PPBOX_NO_PIPE_SINK
struct sink_descriptor
{
    typedef HANDLE native_type;
};
#  endif
#endif

namespace ppbox
{
    namespace data
    {

        class PipeSink
            : public UrlSink
        {
        public:
            typedef sink_descriptor::native_type native_descriptor;

        public:
            PipeSink(
                boost::asio::io_service & io_svc);

            virtual ~PipeSink();

        public:
            virtual boost::system::error_code open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec);

            using UrlSink::open;

            virtual bool is_open(
                boost::system::error_code & ec);

            virtual boost::system::error_code close(
                boost::system::error_code & ec);

        public:
            virtual boost::system::error_code set_non_block(
                bool non_block, 
                boost::system::error_code & ec);

        private:
            // implement util::stream::Sink
            virtual std::size_t private_write_some(
                buffers_t const & buffers,
                boost::system::error_code & ec);

        private:
            sink_descriptor descriptor_;
            bool is_open_;
        };

#ifndef PPBOX_NO_PIPE_SINK
        PPBOX_REGISTER_SINK("pipe", PipeSink);
#endif

    } // namespace demux
} // namespace ppbox

#endif // _PPBOX_DATA_SINK_PIPE_SINK_H_
