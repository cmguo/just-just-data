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
                f_segment       = 2, 
                // �ֶ�����
                f_smoth         = 0x10, 
                f_time_smoth    = 0x20, 
                f_fix_duration  = 0x40, 
                f_segment_seek  = 0x80, 
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
            boost::uint32_t bitrate;    // ƽ��������

            // live only
            time_t start_time;
            boost::uint32_t delay;
            boost::uint64_t current;    // ��ǰʱ�̣�������Ч�������������ֳ���Ҫ����delay����
            boost::uint64_t shift;      // �ܹ�ʱ�Ƶĳ���

            // description
            std::string name;
            std::string format_data;
        }; 

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_BASE_MEDIA_INFO_H_
