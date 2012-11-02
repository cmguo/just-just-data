// PlayInfo.h

#ifndef _PPBOX_DATA_PLAY_INFO_H_
#define _PPBOX_DATA_PLAY_INFO_H_

namespace ppbox 
{
    namespace data 
    {

        struct PlayRange
        {
            PlayRange()
                : beg(0)
                , end(0)
                , pos(0)
            {
            }

            boost::uint64_t beg;
            boost::uint64_t end;
            boost::uint64_t pos;
        };

        struct PlayInfo
        {
            PlayInfo()
            {
            }

            PlayRange byte_range;
            PlayRange time_range;
            std::string config;
        }; 

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_PLAY_INFO_H_
