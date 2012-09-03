// HeadSourceStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/HeadSourceStrategy.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {
        HeadSourceStrategy::HeadSourceStrategy(
            std::vector<SegmentInfoEx> const & segments)
            : SourceStrategy(segments)
        {
        }

        HeadSourceStrategy::~HeadSourceStrategy()
        {
        }

        bool HeadSourceStrategy::next_segment(
            bool is_next,
            SegmentInfoEx & info)
        {
            bool res = SourceStrategy::next_segment(is_next, info);
            if (res) {
                info.begin = 0;
                info.end = info.head_size;
                info.size = info.head_size;
            }
            return res;
        }

        error_code HeadSourceStrategy::on_seek(
            size_t offset,
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            bool find = false;
            for (boost::uint32_t i = 0; i < segments_.size(); ++i) {
                if (offset <= segments_[i].head_size) {
                    info = segments_[i];
                    info.begin = 0;
                    info.end = info.head_size;
                    info.offset = offset;
                    info.size = info.end - info.begin;
                    pos_ = i;
                    find = true;
                    break;
                } else {
                    offset -= segments_[i].head_size;
                }
            }
            if (!find) {
                ec = framework::system::logic_error::out_of_range;
            }
            return ec;
        }

        error_code HeadSourceStrategy::on_seek(
            boost::uint32_t segment_index,
            size_t offset, 
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            if (segment_index < segments_.size()) {
                info = segments_[segment_index];
                if (offset > info.head_size) {
                    ec = framework::system::logic_error::out_of_range;
                } else {
                    info.begin = 0;
                    info.end = info.head_size;
                    info.offset = offset;
                    info.size = info.end - info.begin;
                    pos_ = segment_index;
                }
            } else {
                ec = framework::system::logic_error::out_of_range;
            }
            return ec;
        }

    }
}
