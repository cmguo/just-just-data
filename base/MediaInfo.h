// MediaInfo.h

#ifndef _PPBOX_DATA_BASE_MEDIA_INFO_H_
#define _PPBOX_DATA_BASE_MEDIA_INFO_H_

#include "ppbox/data/base/DataBase.h"

namespace ppbox 
{
    namespace data 
    {

        struct MediaBasicInfo
        {
            MediaBasicInfo()
                : type(vod)
                , flags(0)
            {
            }

            enum TypeEnum
            {
                vod, 
                live, 
            };

            enum FlagEnum
            {
                f_seekable      = 1, 
                f_pauseable     = 2, 
                f_is_live       = 4, 

                f_extend        = 0xf00, 
                f_segment       = 0x100, 
                f_packet        = 0x200, 
            };

            TypeEnum type;
            boost::uint32_t flags;
            std::string format;
        };

        struct MediaInfo
            : MediaBasicInfo
        {
            MediaInfo()
                : file_size(invalid_size)
                , duration(invalid_size)
                , bitrate(0)
                , start_time(0)
                , delay(0)
                , current(0)
                , shift(0)
            {
            }

            boost::uint64_t file_size;
            boost::uint64_t duration;
            boost::uint32_t bitrate;    // 平均码流率

            // live only
            time_t start_time;
            boost::uint32_t delay;
            boost::uint64_t current;    // 当前时刻，描述有效数据量，播放现场还要根据delay计算
            boost::uint64_t shift;      // 能够时移的长度

            // description
            std::string name;
            std::string format_data;
        }; 

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_BASE_MEDIA_INFO_H_
