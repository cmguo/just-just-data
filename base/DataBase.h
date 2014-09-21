// DataBase.h

#ifndef _PPBOX_DATA_BASE_DATA_BASE_H_
#define _PPBOX_DATA_BASE_DATA_BASE_H_

#include <ppbox/avbase/MediaInfo.h>

#include <framework/string/Url.h>

namespace ppbox 
{
    namespace data 
    {

        using ppbox::avbase::MediaBasicInfo;
        using ppbox::avbase::MediaInfo;

        using ppbox::avbase::invalid_size;

        struct DataStat;

        class MediaBase;
        class SingleMedia;
        class SegmentMedia;
        class PacketMedia;

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_BASE_DATA_BASE_H_
