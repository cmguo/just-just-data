// SingleMedia.h

#ifndef _JUST_DATA_BASE_SINGLE_MEDIA_H_
#define _JUST_DATA_BASE_SINGLE_MEDIA_H_

#include "just/data/base/MediaBase.h"

namespace just 
{
    namespace data 
    {

        class SingleMedia
            : public MediaBase
        {
        public:
            typedef boost::function<
                void (boost::system::error_code const &) > 
                response_type;

        public:
            SingleMedia(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

            virtual ~SingleMedia();

        public:
            virtual void async_open(
                response_type const & resp);

            virtual void cancel(
                boost::system::error_code & ec);

            virtual void close(
                boost::system::error_code & ec);

        public:
            virtual bool get_basic_info(
                MediaBasicInfo & info,
                boost::system::error_code & ec) const;

            virtual bool get_info(
                MediaInfo & info,
                boost::system::error_code & ec) const;
        };

    } // namespace data
} // namespace just

#endif//_JUST_DATA_BASE_SINGLE_MEDIA_H_
