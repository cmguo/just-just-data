// UrlSink.h

#ifndef _PPBOX_DATA_BASE_URL_SINK_H_
#define _PPBOX_DATA_BASE_URL_SINK_H_

#include "ppbox/data/base/SinkBase.h"

#include <ppbox/common/ClassFactory.h>

#include <framework/string/Url.h>

namespace ppbox
{
    namespace data
    {

        class UrlSink
            : public SinkBase
            , public ppbox::common::ClassFactory<
                UrlSink, 
                std::string, 
                UrlSink * (boost::asio::io_service &)
            >
        {
        public:
            typedef boost::function<void (
                boost::system::error_code const &)
            > response_type;

        public:
            static UrlSink * create(
                boost::asio::io_service & io_svc,
                std::string const & proto);

        public:
            UrlSink(
                boost::asio::io_service & io_svc);

            virtual ~UrlSink();

        public:
            virtual boost::system::error_code open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec) = 0;

            virtual boost::system::error_code open(
                framework::string::Url const & url,
                boost::system::error_code & ec);

            virtual void async_open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                response_type const & resp);

            virtual void async_open(
                framework::string::Url const & url,
                response_type const & resp);

            virtual bool is_open(
                boost::system::error_code & ec) = 0;

            virtual boost::system::error_code close(
                boost::system::error_code & ec) = 0;

        public:
            virtual bool recoverable(
                boost::system::error_code const & ec);
        };

    } // namespace data
} // namespace ppbox

#define PPBOX_REGISTER_URL_SINK(k, c) PPBOX_REGISTER_CLASS(k, c)

#endif // _PPBOX_DATA_BASE_URL_SINK_H_
