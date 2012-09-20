//  BigHeadStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/BigHeadStrategy.h"

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
            info_.begin = 0;
            info_.end = sinfo.offset;
            MediaInfo minfo;
            boost::system::error_code ec;
            media_.get_info(minfo, ec);
            info_.url = minfo.cdn_url;
            info_.size = info_.end - info_.begin;
            info_.try_times = 0;
            info_.position = 0; // 已经下载的数据大小
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

        boost::system::error_code BigHeadStrategy::seek(
            size_t offset,
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec.clear();
            if (offset > info_.size) {
                ec = framework::system::logic_error::out_of_range;
            } else {
                info = info_;
                info.position = offset;
            }
            return ec;
        }

        boost::system::error_code BigHeadStrategy::seek(
            boost::uint32_t segment_index,
            size_t offset, 
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec.clear();
            if (segment_index > 0 || offset > info_.size) {
                ec = framework::system::logic_error::out_of_range;
            } else {
                info = info_;
                info.position = offset;
            }
            return ec;
        }

        std::size_t BigHeadStrategy::size(void)
        {
            return (std::size_t)info_.size;
        }
    }
}
