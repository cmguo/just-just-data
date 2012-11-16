// MediaInfo.h

#ifndef _PPBOX_DATA_MEDIA_INFO_H_
#define _PPBOX_DATA_MEDIA_INFO_H_

#include "ppbox/data/base/DataBase.h"

namespace ppbox 
{
    namespace data 
    {

        struct MediaInfo
        {
            MediaInfo()
                : type(vod)
                , flags(0)
                , file_size(invalid_size)
                , duration(invalid_size)
                , bitrate(0)
                , start_time(0)
                , delay(0)
                , current(0)
            {
            }

            enum TypeEnum
            {
                vod, 
                live, 
            };

            enum FlagEnum
            {
                f_seekable, 
                // �ֶ�����
                f_segment = 10, 
                f_time_smoth = 20, 
                f_fix_duration = 40, 
                f_segment_seek = 80, 
            };

            TypeEnum type;
            boost::uint32_t flags;
            boost::uint64_t file_size;
            boost::uint64_t duration;
            boost::uint32_t bitrate;    // ƽ��������

            // live only
            time_t start_time;
            boost::uint32_t delay;
            boost::uint64_t current;    // ��ǰʱ�̣�������Ч�������������ֳ���Ҫ����delay����

            // description
            std::string name;
            std::string format;
            std::string format_data;
        }; 

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_MEDIA_INFO_H_
