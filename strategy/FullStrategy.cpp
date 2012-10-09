// FullStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/FullStrategy.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {

        FullStrategy::FullStrategy(MediaBase & media)
            : Strategy(media)
        {
        }

        FullStrategy::~FullStrategy()
        {
        }

        bool FullStrategy::next_segment(
            SegmentInfoEx & info)
        {
            bool res = false;
            boost::uint32_t pos = info.index;
            if (pos < media_.segment_count()) {
                media_.segment_info(pos, info);
                info.index = pos;
                info.begin = 0;
                info.end = boost::uint64_t(-1);
                info.small_offset = 0;
                res = true;
            } else {
                res = false;
            }
            return res;
        }

        error_code FullStrategy::byte_seek(
            size_t offset,
            SegmentInfoEx & info, 
            error_code & ec)
        {
            ec = framework::system::logic_error::out_of_range;
            std::size_t offset_t = offset;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, info);
                if (offset < info.size) {
                    info.begin = offset;
                    info.end = info.size;
                    info.small_offset = offset;
                    info.size = info.end - info.begin;
                    info.big_offset = offset_t;
                    info.index = i;
                    ec.clear();
                    break;
                } else {
                    offset -= info.size;
                }
            }
            return ec;
        }

        error_code FullStrategy::byte_seek(
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            if (info.index < media_.segment_count()) {
                media_.segment_info(info.index, info);
                if (info.small_offset > info.size) {
                    ec = framework::system::logic_error::out_of_range;
                } else {
                    info.begin = info.small_offset;
                    info.end = info.size;
                    info.size = info.end - info.begin;
                    info.big_offset = 0;
                    for (boost::uint32_t i = 0; i < info.index; ++i) {
                        SegmentInfo tmp;
                        media_.segment_info(i, tmp);
                        info.big_offset += tmp.size;
                    }
                    info.big_offset += info.small_offset;
                }
            } else {
                ec = framework::system::logic_error::out_of_range;
            }
            return ec;
        }

        error_code FullStrategy::time_seek(
            boost::uint32_t time_ms,
            SegmentInfoEx & info, 
            error_code & ec)
        {
            ec = framework::system::logic_error::out_of_range;
            std::size_t offset = 0;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, info);
                if (time_ms < info.duration) {
                    info.begin = 0;
                    info.end = info.size;
                    info.small_offset = 0;
                    info.big_offset = offset;
                    info.index = i;
                    url(info.index, info.url);
                    ec.clear();
                    break;
                } else {
                    time_ms -= info.duration;
                    offset += info.size;
                }
            }
            return ec;
        }

        std::size_t FullStrategy::size(void)
        {
            std::size_t length = 0;
            SegmentInfo tmp;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, tmp);
                length += tmp.size;
            }
            return length;
        }

    }
}
