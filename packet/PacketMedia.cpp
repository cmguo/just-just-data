// PacketMedia.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/packet/PacketMedia.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.PacketMedia", framework::logger::Debug);

namespace ppbox
{
    namespace data
    {

        PacketMedia::PacketMedia(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
            : MediaBase(io_svc, url)
        {
        }

        PacketMedia::~PacketMedia()
        {
        }

        bool PacketMedia::get_basic_info(
            MediaBasicInfo & info, 
            boost::system::error_code & ec) const
        {
            info.flags = MediaBasicInfo::f_packet;
            ec.clear();
            return true;
        }

        bool PacketMedia::get_info(
            MediaInfo & info, 
            boost::system::error_code & ec) const
        {
            if (info.type == info.live) {
                info.current = (framework::timer::Time::now() - start_time_).total_milliseconds();
            }
            return true;
        }

        bool PacketMedia::get_packet_feature(
            PacketFeature & feature,
            boost::system::error_code & ec) const
        {
            feature.piece_size = 1500; // MTU
            feature.packet_max_size = 1024 * 500;
            ec.clear();
            return true;
        }

    } // data
} // ppbox
