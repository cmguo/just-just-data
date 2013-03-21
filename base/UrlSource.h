// UrlSource.h

#ifndef _PPBOX_DATA_BASE_URL_SOURCE_H_
#define _PPBOX_DATA_BASE_URL_SOURCE_H_

#include "ppbox/data/base/SourceBase.h"

#include <ppbox/common/ClassFactory.h>

#include <util/stream/Source.h>

namespace ppbox
{
    namespace data
    {

        class UrlSource
            : public SourceBase
            , public ppbox::common::ClassFactory<
                UrlSource, 
                std::string, 
                UrlSource * (boost::asio::io_service &)
            >
        {
        public:
            typedef boost::function<void (
                boost::system::error_code const &)
            > response_type;

        public:
            static UrlSource * create(
                boost::asio::io_service & io_svc,
                std::string const & proto);

        public:
            UrlSource(
                boost::asio::io_service & io_svc);

            virtual ~UrlSource();

        public:
            virtual boost::system::error_code open(
                framework::string::Url const & url, 
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec) = 0;

            virtual void async_open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                response_type const & resp);

            virtual bool is_open(
                boost::system::error_code & ec)= 0;

            virtual boost::system::error_code close(
                boost::system::error_code & ec)= 0;

            virtual boost::uint64_t total(
                boost::system::error_code & ec);

        public:
            virtual bool recoverable(
                boost::system::error_code const & ec);
        };

    } // namespace data
} // namespace ppbox

#define PPBOX_REGISTER_URL_SOURCE(k, c) PPBOX_REGISTER_CLASS(k, c)

#endif // _PPBOX_DATA_BASE_URL_SOURCE_H_
