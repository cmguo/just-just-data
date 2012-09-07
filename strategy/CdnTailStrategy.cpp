// CdnTailStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/CdnTailStrategy.h"

namespace ppbox
{
    namespace data
    {
        CdnTailStrategy::CdnTailStrategy(
            std::vector<SegmentInfoEx> const & segments,
            VideoInfo const & video_info)
            : SourceStrategy(segments, video_info)
        {
            assert(!segments.empty());
            boost::uint64_t head_size = segments[0].ofs;
            boost::uint64_t body_size = 0;
            for (boost::uint32_t i = 0; i < segments.size(); ++i) {
                body_size += (segments[i].size - segments[i].head_size);
            }

            info_.begin = head_size + body_size;
            info_.end = video_info.filesize;
            info_.url = video_info.url;
            info_.size = info_.end - info_.begin;
            info_.try_times = 0;
            info_.offset = 0;
        }

        CdnTailStrategy::~CdnTailStrategy()
        {
        }

        bool CdnTailStrategy::next_segment(
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

        boost::system::error_code CdnTailStrategy::on_seek(
            size_t offset,
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec.clear();
            if (info_.size == 0 || offset > info_.size) {
                ec = framework::system::logic_error::out_of_range;
            } else {
                info = info_;
                info.offset = offset;
            }
            return ec;
        }

        boost::system::error_code CdnTailStrategy::on_seek(
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
                info.offset = offset;
            }
            return ec;
        }

        std::size_t CdnTailStrategy::size(void)
        {
            return info_.size;
        }
    }
}
