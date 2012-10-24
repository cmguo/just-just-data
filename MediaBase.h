// MediaBase.h

#ifndef _PPBOX_DATA_MEDIA_BASE_H_
#define _PPBOX_DATA_MEDIA_BASE_H_

#include "ppbox/data/MediaInfo.h"

#include <ppbox/common/Call.h>
#include <ppbox/common/Create.h>

#define PPBOX_REGISTER_MEDIA(n, c) \
    static ppbox::common::Call reg ## n(ppbox::data::MediaBase::register_media, BOOST_PP_STRINGIZE(n), ppbox::common::Creator<c>())

namespace ppbox 
{
    namespace data 
    {

        class MediaBase
        {

        public:
            typedef boost::function<
                void(boost::system::error_code const &) > 
                response_type;

            typedef boost::function<MediaBase * (
                boost::asio::io_service & )
            > register_type;


        public:
            MediaBase(
                boost::asio::io_service & io_svc);

            ~MediaBase();

        public:
            static MediaBase * create(
                boost::asio::io_service & io_svc,
                framework::string::Url const & playlink);

            static void register_media(
                std::string const & proto,
                register_type func);

            static void destory(MediaBase* & segment);

        public:
            virtual void set_url(
                framework::string::Url const & url);

            virtual void async_open(
                response_type const & resp) = 0;

            virtual void cancel(
                boost::system::error_code & ec) = 0;

            virtual void close(
                boost::system::error_code & ec)= 0;

        public:
            std::string const & get_protocol() const
            {
                return url_.protocol();
            }

            virtual bool get_info(
                MediaInfo & info,
                boost::system::error_code & ec) const = 0;

            virtual bool get_url(
                framework::string::Url & url,
                boost::system::error_code & ec) const;

        public:
            virtual size_t segment_count() const = 0;

            virtual std::string const segment_protocol() const
            {
                return get_protocol();
            }

            virtual bool segment_url(
                size_t segment, 
                framework::string::Url & url,
                boost::system::error_code & ec) const = 0;

            virtual void segment_info(
                size_t segment, 
                SegmentInfo & info) const = 0;

        public:
            virtual void on_error(
                boost::system::error_code & ec) const { /* do nothing */ };

        public:
            boost::asio::io_service & get_io_service()
            {
                return io_svc_;
            }

        protected:
            framework::string::Url url_;

        private:
            boost::asio::io_service & io_svc_;

        private:
            static std::map<std::string, register_type> & media_map();
        };

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_MEDIA_BASE_H_
