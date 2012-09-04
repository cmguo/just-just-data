// HeadSourceStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/BodySourceStrategy.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {
        BodySourceStrategy::BodySourceStrategy(
            std::vector<SegmentInfoEx> const & segments,
            VideoInfo const & video_info)
            : SourceStrategy(segments, video_info)
        {
        }

        BodySourceStrategy::~BodySourceStrategy()
        {
        }

        bool BodySourceStrategy::next_segment(
            bool is_next, 
            SegmentInfoEx & info)
        {
            bool res = SourceStrategy::next_segment(is_next, info);
            if (res) {
                info.begin = info.head_size;
                info.end = info.size;
                info.size = info.end - info.begin;
            }
            return res;
        }

        error_code BodySourceStrategy::on_seek(
            size_t offset,
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            bool find = false;
            for (boost::uint32_t i = 0; i < segments_.size(); ++i) {
                if (offset <= (segments_[i].size-segments_[i].head_size)) {
                    info = segments_[i];
                    info.begin = info.head_size;
                    info.end = info.size;
                    info.offset = offset;
                    info.size = info.end - info.begin;
                    find = true;
                    pos_ = i;
                    break;
                } else {
                    offset -= (segments_[i].size-segments_[i].head_size);
                }
            }
            if (!find) {
                ec = framework::system::logic_error::out_of_range;
            }
            return ec;
        }

        error_code BodySourceStrategy::on_seek(
            boost::uint32_t segment_index,
            size_t offset, 
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            if (segment_index < segments_.size()) {
                info = segments_[segment_index];
                if (offset > (info.size - info.head_size)) {
                    ec = framework::system::logic_error::out_of_range;
                } else {
                    info.begin = info.head_size;
                    info.end = info.size;
                    info.offset = offset;
                    info.size = info.end - info.begin;
                    pos_ = segment_index;
                }
            } else {
                ec = framework::system::logic_error::out_of_range;
            }
            return ec;
        }

        std::size_t BodySourceStrategy::size(void)
        {
            std::size_t length = 0;
            for (boost::uint32_t i = 0; i < segments_.size(); ++i) {
                length += (segments_[i].size - segments_[i].head_size);
            }
            return length;
        }

    }
}
