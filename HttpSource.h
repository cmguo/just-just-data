// HttpSource.h

#ifndef _PPBOX_DATA_HTTPSOURCE_H_
#define _PPBOX_DATA_HTTPSOURCE_H_

#include "ppbox/data/SourceBase.h"
#include "ppbox/data/SegmentBase.h"

#include <util/protocol/http/HttpClient.h>

namespace ppbox
{
    namespace data
    {

        class HttpSource;
        PPBOX_REGISTER_SOURCE(ppvod, HttpSource);
        PPBOX_REGISTER_SOURCE(pplive, HttpSource);
        PPBOX_REGISTER_SOURCE(ppvod2, HttpSource);
        PPBOX_REGISTER_SOURCE(pplive2, HttpSource);

        class HttpSource
            : public SourceBase
        {
        public:
            HttpSource(
                boost::asio::io_service & io_svc);

            ~HttpSource();

        public:
            boost::system::error_code open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec);

            void async_open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                response_type const & resp);
           
            bool is_open(
                boost::system::error_code & ec);

            boost::system::error_code cancel(
                size_t segment, 
                boost::system::error_code & ec);

            boost::system::error_code close(
                size_t segment, 
                boost::system::error_code & ec);

            boost::uint64_t total(
                boost::system::error_code & ec);

            boost::system::error_code set_non_block(
                bool non_block, 
                boost::system::error_code & ec);

            boost::system::error_code set_time_out(
                boost::uint32_t time_out, 
                boost::system::error_code & ec);

            bool continuable(
                boost::system::error_code const & ec);

            bool recoverable(
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
            framework::network::NetName addr_;
            util::protocol::HttpRequest request_;
            util::protocol::HttpClient http_;
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_HTTPSOURCE_H_
