// M3u8Media.h

#ifndef _PPBOX_DATA_MEDIA_M3U8_MEDIA_H_
#define _PPBOX_DATA_MEDIA_M3U8_MEDIA_H_

#include "ppbox/data/segment/SegmentMedia.h"

#include <boost/asio/streambuf.hpp>

namespace ppbox 
{
    namespace data 
    {

        class SourceBase;

        class M3u8Media
            : public SegmentMedia
        {
        public:
            typedef boost::function<
                void (boost::system::error_code const &) > 
                response_type;

        public:
            M3u8Media(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

            ~M3u8Media();

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

        public:
            virtual size_t segment_count() const;

            virtual std::string const segment_protocol() const;

            virtual bool segment_url(
                size_t segment, 
                framework::string::Url & url,
                boost::system::error_code & ec) const;

            virtual void segment_info(
                size_t segment, 
                SegmentInfo & info) const;

        private:
            void handle_open(
                response_type const & resp, 
                boost::system::error_code ec);

            void handle_read(
                response_type const & resp, 
                boost::system::error_code ec);

            void parse_m3u8(
                boost::system::error_code & ec);

        private:
            MediaInfo info_;
            SourceBase * source_;
            boost::asio::streambuf buf_;
            size_t sequence_;
            std::vector<SegmentInfo> segments_;
            std::vector<std::string> segment_urls_;
        };

        PPBOX_REGISTER_MEDIA_BY_FORMAT("m3u8", M3u8Media);

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_MEDIA_M3U8_MEDIA_H_
