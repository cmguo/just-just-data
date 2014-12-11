// PacketFeature.h

#ifndef _JUST_DATA_PACKET_PACKET_FEATURE_H_
#define _JUST_DATA_PACKET_PACKET_FEATURE_H_

#include "just/data/base/MediaBase.h"

namespace just 
{
    namespace data 
    {

        struct PacketMediaFlags
        {
            enum Enum
            {
                f_non_ordered   = 0x1000, 
                f_has_time      = 0x2000,
            };
        };

        struct PacketFeature 
        {
            boost::uint32_t buffer_size;
            boost::uint32_t piece_size;
            boost::uint32_t packet_max_size;
            boost::uint32_t block_size;
            boost::uint32_t piece_num; // 一个 Packet 中最大 Piece 数
            boost::uint32_t prepare_size;

            PacketFeature()
                : buffer_size(0)
                , piece_size(0)
                , packet_max_size(0)
                , block_size(0)
                , piece_num(0)
                , prepare_size(0)
            {
            }
        };

    } // namespace data
} // namespace just

#endif//_JUST_DATA_PACKET_PACKET_FEATURE_H_
