// SegmentInfo.h

#ifndef _JUST_DATA_SEGMENT_SEGMENT_INFO_H_
#define _JUST_DATA_SEGMENT_SEGMENT_INFO_H_

#include "just/data/base/DataBase.h"

namespace just 
{
    namespace data 
    {

        struct SegmentMediaFlags
        {
            enum Enum
            {
                // ∑÷∂Œ Ù–‘
                f_smoth         = 0x1000, 
                f_time_smoth    = 0x2000, 
                f_fix_duration  = 0x4000, 
                f_segment_seek  = 0x8000, 
            };
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
} // namespace just

#endif//_JUST_DATA_SEGMENT_SEGMENT_INFO_H_
