// FileSink.h

#ifndef _PPBOX_DATA_SINK_FILE_SINK_H_
#define _PPBOX_DATA_SINK_FILE_SINK_H_

#include "ppbox/data/base/UrlSink.h"

#include <framework/filesystem/File.h>

namespace ppbox
{
    namespace data
    {

        class FileSink
            : public UrlSink
        {
        public:
            FileSink(
                boost::asio::io_service & io_svc);

            virtual ~FileSink();

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

            virtual boost::uint64_t total(
                boost::system::error_code & ec);

        private:
            // implement util::stream::Sink
            virtual std::size_t private_write_some(
                boost::asio::const_buffer const & buffer,
                boost::system::error_code & ec);

        private:
            framework::filesystem::File file_;
            bool is_open_;
        };

        PPBOX_REGISTER_SINK("file", FileSink);

    } // namespace demux
} // namespace ppbox

#endif // _PPBOX_DATA_SINK_FILE_SINK_H_
