// HeadStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/HeadStrategy.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {
        HeadStrategy::HeadStrategy(MediaBase & media)
            : Strategy(media)
        {
        }

        HeadStrategy::~HeadStrategy()
        {
        }

        bool HeadStrategy::next_segment(
            SegmentInfoEx & info)
        {
            bool res = false;
            boost::uint32_t pos = ++info.index;
            if (pos < media_.segment_count()) {
                media_.segment_info(pos, info);
                info.index = pos;
                info.begin = 0;
                info.end = info.head_size;
                info.small_offset = 0;
                res = true;
            } else {
                res = false;
            }
            return res;
        }

        error_code HeadStrategy::byte_seek(
            size_t offset,
            SegmentInfoEx & info, 
            error_code & ec)
        {
            ec = framework::system::logic_error::out_of_range;
            std::size_t offset_t = offset;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, info);
                if (offset < info.head_size) {
                    info.begin = offset;
                    info.end = info.head_size;
                    info.small_offset = offset;
                    info.big_offset = offset_t;
                    info.size = info.end - info.begin;
                    info.index = i;
                    url(info.index, info.url);
                    ec.clear();
                    break;
                } else {
                    offset -= info.head_size;
                }
            }
            return ec;
        }

        error_code HeadStrategy::byte_seek(
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            /*if (info.index < media_.segment_count()) {
                media_.segment_info(info.index, info);
                if (info.small_offset > info.head_size) {
                    ec = framework::system::logic_error::out_of_range;
                } else {
                    info.begin = info.small_offset;
                    info.end = info.head_size;
                    info.size = info.end - info.begin;
                }
            } else {
                ec = framework::system::logic_error::out_of_range;
            }*/
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        error_code HeadStrategy::time_seek(
            boost::uint32_t time_ms, 
            SegmentInfoEx & info, 
            error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        std::size_t HeadStrategy::size(void)
        {
            std::size_t length = 0;
            SegmentInfo tmp;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, tmp);
                length += tmp.head_size;
            }
            return length;
        }

    }
}
