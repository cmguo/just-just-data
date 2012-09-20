// BigTailStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/BigTailStrategy.h"

namespace ppbox
{
    namespace data
    {
        BigTailStrategy::BigTailStrategy(MediaBase & media)
            : Strategy(media)
        {
            assert(media_.segment_count() > 0);
            SegmentInfo tmp;
            media_.segment_info(0, tmp);
            boost::uint64_t head_size = tmp.offset;
            boost::uint64_t body_size = 0;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, tmp);
                body_size += (tmp.size - tmp.head_size);
            }
            info_.begin = head_size + body_size;
            boost::system::error_code ec;
            MediaInfo media_info;
            media_.get_info(media_info, ec);
            info_.end = media_info.file_size;
            info_.url = media_info.cdn_url;
            info_.size = info_.end - info_.begin;
            info_.try_times = 0;
            info_.position = 0;
        }

        BigTailStrategy::~BigTailStrategy()
        {
        }

        bool BigTailStrategy::next_segment(
            bool is_next,
            SegmentInfoEx & info)
        {
            bool res = false;
            if (!is_next && info_.size != 0) {
                info = info_;
                res = true;
            }
            return res;
        }

        boost::system::error_code BigTailStrategy::seek(
            size_t offset,
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec.clear();
            if (info_.size == 0 || offset > info_.size) {
                ec = framework::system::logic_error::out_of_range;
            } else {
                info = info_;
                info.position = offset;
            }
            return ec;
        }

        boost::system::error_code BigTailStrategy::seek(
            boost::uint32_t segment_index,
            size_t offset, 
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec.clear();
            if (info_.size == 0 || segment_index > 0 || offset > info_.size) {
                ec = framework::system::logic_error::out_of_range;
            } else {
                info = info_;
                info.position = offset;
            }
            return ec;
        }

        std::size_t BigTailStrategy::size(void)
        {
            return (std::size_t)info_.size;
        }
    }
}
