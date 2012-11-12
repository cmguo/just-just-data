// MediaInfo.h

#ifndef _PPBOX_DATA_MEDIA_INFO_H_
#define _PPBOX_DATA_MEDIA_INFO_H_

namespace ppbox 
{
    namespace data 
    {

        static boost::uint64_t const invalid_size = boost::uint64_t(-1);

        struct MediaInfo
        {
            MediaInfo()
                : type(vod)
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

            TypeEnum type;
            boost::uint64_t file_size;
            boost::uint64_t duration;
            boost::uint32_t bitrate;    // 平均码流率

            // live only
            time_t start_time;
            boost::uint32_t delay;
            boost::uint64_t current;    // 当前时刻，描述有效数据量，播放现场还要根据delay计算

            // description
            std::string name;
            std::string format;
            std::string format_data;
        }; 

        struct SegmentInfo
        {
            SegmentInfo()
                : head_size(invalid_size)
                , size(invalid_size)
                , offset(invalid_size)
                , duration(invalid_size)
            {
            }

            boost::uint64_t head_size;
            boost::uint64_t size;
            boost::uint64_t offset;
            boost::uint64_t duration;
        };

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_MEDIA_BASE_H_
