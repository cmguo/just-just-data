// BodyStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/BodyStrategy.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {
        BodyStrategy::BodyStrategy(MediaBase & media)
            : Strategy(media)
        {
        }

        BodyStrategy::~BodyStrategy()
        {
        }

        bool BodyStrategy::next_segment(
            SegmentInfoEx & info)
        {
            bool res = false;
            boost::uint32_t pos = info.index;
            if (pos < media_.segment_count()) {
                media_.segment_info(pos, info);
                info.index = pos;
                info.begin = info.head_size;
                info.end = info.size;
                info.small_offset = 0;
                res = true;
            } else {
                res = false;
            }
            return res;
        }

        error_code BodyStrategy::byte_seek(
            size_t offset,
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::out_of_range;
            std::size_t offset_t = offset;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, info);
                if (offset < (info.size - info.head_size)) {
                    info.begin = offset + info.head_size;
                    info.end = info.size;
                    info.small_offset = offset + info.head_size;
                    info.big_offset = offset_t;
                    info.size = info.end - info.begin;
                    info.index = i;
                    ec.clear();
                    break;
                } else {
                    offset -= (info.size - info.head_size);
                }
            }
            return ec;
        }

        error_code BodyStrategy::time_seek(
            boost::uint32_t time_t,
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        std::size_t BodyStrategy::size(void)
        {
            std::size_t length = 0;
            SegmentInfo tmp;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, tmp);
                length += (tmp.size - tmp.head_size);
            }
            return length;
        }

    }
}
