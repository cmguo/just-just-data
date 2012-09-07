// CdnHeadStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/CdnHeadStrategy.h"

namespace ppbox
{
    namespace data
    {
        CdnHeadStrategy::CdnHeadStrategy(
            std::vector<SegmentInfoEx> const & segments,
            VideoInfo const & video_info)
            : SourceStrategy(segments, video_info)
        {
            assert(!segments.empty());
            info_.begin = 0;
            info_.end = segments[0].ofs;
            info_.url = video_info.url;
            info_.size = info_.end - info_.begin;
            info_.try_times = 0;
            info_.offset = 0; // 已经下载的数据大小
        }

        CdnHeadStrategy::~CdnHeadStrategy()
        {
        }

        bool CdnHeadStrategy::next_segment(
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

        boost::system::error_code CdnHeadStrategy::on_seek(
            size_t offset,
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec.clear();
            if (offset > info_.size) {
                ec = framework::system::logic_error::out_of_range;
            } else {
                info = info_;
                info.offset = offset;
            }
            return ec;
        }

        boost::system::error_code CdnHeadStrategy::on_seek(
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
                info.offset = offset;
            }
            return ec;
        }

        std::size_t CdnHeadStrategy::size(void)
        {
            return info_.size;
        }
    }
}
