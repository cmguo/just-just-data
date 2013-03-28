// HttpSink.h

#ifndef _PPBOX_DATA_SINK_HTTP_SINK_H_
#define _PPBOX_DATA_SINK_HTTP_SINK_H_

#include "ppbox/data/base/UrlSink.h"

#include <util/protocol/http/HttpClient.h>

namespace ppbox
{
    namespace data
    {

        class HttpSink
            : public UrlSink
        {
        public:
            HttpSink(
                boost::asio::io_service & io_svc);

            virtual ~HttpSink();

        public:
            virtual boost::system::error_code open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec);

            virtual void async_open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                response_type const & resp);

            virtual bool is_open(
                boost::system::error_code & ec);

            virtual boost::system::error_code close(
                boost::system::error_code & ec);

        public:
            virtual bool recoverable(
                boost::system::error_code const & ec);

        public:
            virtual boost::system::error_code cancel(
                boost::system::error_code & ec);

        public:
            virtual boost::system::error_code set_non_block(
                bool non_block, 
                boost::system::error_code & ec);

            virtual boost::system::error_code set_time_out(
                boost::uint32_t time_out, 
                boost::system::error_code & ec);

            virtual bool continuable(
                boost::system::error_code const & ec);

        private:
            // implement util::stream::Source
            virtual std::size_t private_read_some(
                util::stream::StreamMutableBuffers const & buffers,
                boost::system::error_code & ec);

            virtual void private_async_read_some(
                util::stream::StreamMutableBuffers const & buffers,
                util::stream::StreamHandler const & handler);

        protected:
            bool flag_;
            util::protocol::HttpClient http_;
        };

        PPBOX_REGISTER_URL_SINK("http", HttpSink);

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_SINK_HTTP_SINK_H_