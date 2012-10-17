// MediaInfo.h

#ifndef _PPBOX_DATA_MEDIA_INFO_H_
#define _PPBOX_DATA_MEDIA_INFO_H_

#include <framework/string/Url.h>

namespace ppbox 
{
    namespace data 
    {

        static boost::uint64_t const invalid_size = boost::uint64_t(-1);

        struct MediaInfo
        {
            MediaInfo()
                : file_size(invalid_size)
                , duration(invalid_size)
                , bitrate(0)
                , is_live(false)
                , delay(0)
            {
            }

            std::string name;
            std::string format;
            boost::uint64_t file_size;
            boost::uint64_t duration;
            boost::uint32_t bitrate;    // 平均码流率
            bool is_live;
            boost::uint32_t delay;
            framework::string::Url url;
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
