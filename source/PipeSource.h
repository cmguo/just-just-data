// PipeSource.h

#ifndef _PPBOX_DATA_PIPE_SOURCE_H_
#define _PPBOX_DATA_PIPE_SOURCE_H_

#include <ppbox/data/SourceBase.h>

#ifndef BOOST_WINDOWS_API
#  include <boost/asio/posix/stream_descriptor.hpp>
typedef boost::asio::posix::stream_descriptor descriptor;
#else
#  include <boost/asio/windows/stream_handle.hpp>
typedef boost::asio::windows::stream_handle descriptor;
#endif

namespace ppbox
{
    namespace data
    {

        class PipeSource
            : public SourceBase
        {
        public:
            typedef descriptor::native_type native_descriptor;

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

            virtual boost::system::error_code set_non_block(
                bool non_block, 
                boost::system::error_code & ec);

            virtual bool continuable(
                boost::system::error_code const & ec);

        private:
            // implement util::stream::Source
            virtual std::size_t private_read_some(
                util::stream::StreamMutableBuffers const & buffers,
                boost::system::error_code & ec);

        private:
            descriptor descriptor_;
            bool is_open_;
        };

        PPBOX_REGISTER_SOURCE("pipe", PipeSource);

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_PIPE_SOURCE_H_
