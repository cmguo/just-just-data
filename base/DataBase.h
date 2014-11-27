// DataBase.h

#ifndef _JUST_DATA_BASE_DATA_BASE_H_
#define _JUST_DATA_BASE_DATA_BASE_H_

#include <just/avbase/MediaInfo.h>

#include <framework/string/Url.h>

namespace just 
{
    namespace data 
    {

        using just::avbase::MediaBasicInfo;
        using just::avbase::MediaInfo;

        using just::avbase::invalid_size;

        struct DataStat;

        class MediaBase;
        class SingleMedia;
        class SegmentMedia;
        class PacketMedia;

    } // namespace data
} // namespace just

#endif//_JUST_DATA_BASE_DATA_BASE_H_
