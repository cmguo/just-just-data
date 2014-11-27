// PacketMedia.h

#ifndef _JUST_DATA_PACKET_PACKET_MEDIA_H_
#define _JUST_DATA_PACKET_PACKET_MEDIA_H_

#include "just/data/base/MediaBase.h"
#include "just/data/packet/PacketFeature.h"

#include <framework/timer/ClockTime.h>

namespace util { namespace stream { 
    class Source;
}}

namespace just 
{
    namespace data 
    {

        class PacketMedia
            : public MediaBase
        {
        public:
            typedef boost::function<
                void (boost::system::error_code const &) > 
                response_type;

        public:
            PacketMedia(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

            virtual ~PacketMedia();

        public:
            virtual bool get_basic_info(
                MediaBasicInfo & info,
                boost::system::error_code & ec) const;

            virtual bool get_info(
                MediaInfo & info,
                boost::system::error_code & ec) const;

        public:
            virtual bool get_packet_feature(
                PacketFeature & feature,
                boost::system::error_code & ec) const;

            virtual util::stream::Source & source() = 0;

        private:
            framework::timer::Time start_time_;
        };

    } // namespace data
} // namespace just

#endif//_JUST_DATA_PACKET_PACKET_MEDIA_H_
