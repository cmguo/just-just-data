// MediaBase.h

#ifndef _PPBOX_DATA_MEDIA_BASE_H_
#define _PPBOX_DATA_MEDIA_BASE_H_

#include <ppbox/common/Call.h>
#include <ppbox/common/Create.h>

#include <framework/string/Url.h>
#include <framework/network/NetName.h>

#define PPBOX_REGISTER_MEDIA(n, c) \
    static ppbox::common::Call reg ## n(ppbox::data::MediaBase::register_media, BOOST_PP_STRINGIZE(n), ppbox::common::Creator<c>())

namespace ppbox 
{
    namespace data 
    {

        struct MediaInfo
        {
            MediaInfo()
                : file_size(0)
                , bitrate(0)
                , duration(0)
                , is_live(false)
                , delay(0)
            {
            }

            std::string name;
            boost::uint64_t file_size;
            boost::uint32_t bitrate;    // 平均码流率
            boost::uint32_t duration;
            boost::uint32_t is_live;
            boost::uint32_t delay;
            framework::string::Url cdn_url;
        }; 

        struct SegmentInfo
        {
            SegmentInfo()
                : head_size(0)
                , size(0)
                , offset(0)
                , duration(0)
            {
            }

            boost::uint64_t head_size;
            boost::uint64_t size;
            boost::uint64_t offset;
            boost::uint64_t duration;
        };

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

            virtual void cancel() = 0;

            virtual void close()= 0;

        public:
            std::string const & get_protocol() const
            {
                return url_.protocol();
            }

            virtual boost::system::error_code get_info(
                MediaInfo & info,
                boost::system::error_code & ec) = 0;

        public:
            virtual size_t segment_count() const = 0;

            virtual std::string const segment_protocol() const
            {
                return get_protocol();
            }

            virtual boost::system::error_code segment_url(
                size_t segment, 
                framework::string::Url & url,
                boost::system::error_code & ec) = 0;

            virtual void segment_info(
                size_t segment, 
                SegmentInfo & info) const = 0;

        protected:
            framework::string::Url url_;

        private:
            static std::map<std::string, register_type> & media_map();
        };

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_MEDIA_BASE_H_
