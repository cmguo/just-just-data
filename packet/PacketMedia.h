// PacketMedia.h

#ifndef _PPBOX_DATA_PACKET_PACKET_MEDIA_H_
#define _PPBOX_DATA_PACKET_PACKET_MEDIA_H_

#include "ppbox/data/base/MediaBase.h"
#include "ppbox/data/packet/PacketFeature.h"

namespace ppbox 
{
    namespace data 
    {

        class SourceBase;

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

        public:
            virtual bool get_packet_feature(
                PacketFeature & feature,
                boost::system::error_code & ec) const;

            virtual SourceBase & source() = 0;
        };

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_PACKET_PACKET_MEDIA_H_
