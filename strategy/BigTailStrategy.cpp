// BigTailStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/BigTailStrategy.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {
        BigTailStrategy::BigTailStrategy(MediaBase & media)
            : Strategy(media)
            , next_flag_(false)
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
            info_.index = boost::uint32_t(-1);
            url(info_.index, info_.url);
            info_.begin = head_size + body_size;
            boost::system::error_code ec;
            MediaInfo media_info;
            media_.get_info(media_info, ec);
            info_.end = media_info.file_size;
            info_.size = info_.end - info_.begin;
            info_.small_offset = info_.begin;
            info_.big_offset = info_.begin;
        }

        BigTailStrategy::~BigTailStrategy()
        {
        }

        bool BigTailStrategy::next_segment(
            SegmentInfoEx & info)
        {
            bool res = false;
            if (next_flag_) {
                res = false;
            } else {
                info = info_;
                res = true;
                next_flag_ = true;
            }
            return res;
        }

        error_code BigTailStrategy::byte_seek(
            size_t offset, 
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec.clear();
            if (info_.size == 0 || offset > info_.size) {
                ec = framework::system::logic_error::out_of_range;
            } else {
                info = info_;
                info.small_offset = offset;
                info.big_offset = offset;
            }
            return ec;
        }

        error_code BigTailStrategy::byte_seek(
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        error_code BigTailStrategy::time_seek(
            boost::uint32_t time_ms, 
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        std::size_t BigTailStrategy::size(void)
        {
            return (std::size_t)info_.size;
        }
    }
}
