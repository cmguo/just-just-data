//  BigHeadStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/BigHeadStrategy.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {
        BigHeadStrategy::BigHeadStrategy(MediaBase & media)
            : Strategy(media)
        {
            assert(media_.segment_count() > 0);
            SegmentInfo sinfo;
            media_.segment_info(0, sinfo);
            info_.index = boost::uint32_t(-1);
            info_.begin = 0;
            info_.end = sinfo.offset;
            info_.size = info_.end - info_.begin;
            info_.small_offset = 0;
            info_.big_offset = 0;
        }

        BigHeadStrategy::~BigHeadStrategy()
        {
        }

        bool BigHeadStrategy::next_segment(
            bool is_next,
            SegmentInfoEx & info)
        {
            bool res = false;
            if (!is_next) {
                info = info_;
                res = true;
            }
            return res;
        }

        error_code BigHeadStrategy::byte_seek(
            size_t offset,
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec.clear();
            if (offset > info_.size) {
                ec = framework::system::logic_error::out_of_range;
            } else {
                info = info_;
                info.small_offset = offset;
                info.big_offset = offset;
            }
            return ec;
        }

        error_code BigHeadStrategy::time_seek(
            boost::uint32_t time_ms,
            SegmentInfoEx & info, 
            error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        std::size_t BigHeadStrategy::size(void)
        {
            return (std::size_t)info_.size;
        }
    }
}
