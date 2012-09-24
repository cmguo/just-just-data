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
            bool is_next,
            SegmentInfoEx & info)
        {
            bool res = false;
            is_next ? pos_++ : pos_;
            if (pos_ < media_.segment_count()) {
                media_.segment_info(pos_, info);
                info.index = pos_;
                info.begin = 0;
                info.end = boost::uint64_t(-1);
                info.small_offset = 0;
                res = true;
            } else {
                pos_--;
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
                    pos_ = i;
                    info.index = pos_;
                    ec.clear();
                    break;
                } else {
                    offset -= info.size;
                }
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
                    pos_ = i;
                    info.index = pos_;
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
