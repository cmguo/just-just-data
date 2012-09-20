// HeadStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/HeadStrategy.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {
        HeadStrategy::HeadStrategy(MediaBase & media)
            : Strategy(media)
        {
        }

        HeadStrategy::~HeadStrategy()
        {
        }

        bool HeadStrategy::next_segment(
            bool is_next,
            SegmentInfoEx & info)
        {
            bool res = false;
            is_next ? pos_++ : pos_;
            if (pos_ < media_.segment_count()) {
                media_.segment_info(pos_, info);
                info.try_times = 0;
                info.begin = 0;
                info.end = info.head_size;
                info.position = 0;
                error_code ec;
                media_.segment_url(pos_, info.url, ec);
                // TODO: ´íÎóÂë´¦Àí
                res = true;
            } else {
                pos_--;
                res = false;
            }
            return res;
        }

        error_code HeadStrategy::seek(
            size_t offset,
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::out_of_range;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, info);
                if (offset < info.head_size) {
                    info.begin = offset;
                    info.end = info.head_size;
                    info.position = offset;
                    info.size = info.end - info.begin;
                    pos_ = i;
                    ec.clear();
                    break;
                } else {
                    offset -= info.head_size;
                }
            }
            return ec;
        }

        error_code HeadStrategy::seek(
            boost::uint32_t segment_index,
            size_t offset, 
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            if (segment_index < media_.segment_count()) {
                media_.segment_info(segment_index, info);
                if (offset > info.head_size) {
                    ec = framework::system::logic_error::out_of_range;
                } else {
                    pos_ = segment_index;
                    info.begin = offset;
                    info.end = info.head_size;
                    info.size = info.end - info.begin;
                    info.position = offset;
                }
            } else {
                ec = framework::system::logic_error::out_of_range;
            }
            return ec;
        }

        std::size_t HeadStrategy::size(void)
        {
            std::size_t length = 0;
            SegmentInfo tmp;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, tmp);
                length += tmp.head_size;
            }
            return length;
        }

    }
}
