// StreamStatus.h

#ifndef _PPBOX_DATA_BASE_STREAM_STATUS_H_
#define _PPBOX_DATA_BASE_STREAM_STATUS_H_

namespace ppbox 
{
    namespace data 
    {

        struct StreamRange
        {
            StreamRange()
                : beg(0)
                , end(0)
                , pos(0)
                , buf(0)
            {
            }

            boost::uint64_t beg;
            boost::uint64_t end;
            boost::uint64_t pos;
            boost::uint64_t buf;
        };

        struct StreamStatus
        {
            enum StatusEnum
            {
                closed, 
                media_opening, 
                stream_opening, 
                opened, 
                paused,
                playing, 
                seeking, 
            };

            StreamStatus()
                : status_ex(closed)
            {
            }

            StatusEnum status() const
            {
                return StatusEnum(status_ex & 0x007f);
            }

            bool blocked() const
            {
                return (status_ex & 0x80) != 0;
            }

            boost::uint64_t buf_time() const
            {
                return time_range.buf - time_range.pos;
            }

            boost::uint32_t status_ex;
            StreamRange byte_range;
            StreamRange time_range;
            boost::system::error_code buf_ec;
            std::string desc;
        }; 

    } // namespace data
} // namespace ppbox

#endif //_PPBOX_DATA_BASE_STREAM_STATUS_H_
