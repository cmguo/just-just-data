// BodyStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/BodyStrategy.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {
        BodyStrategy::BodyStrategy(MediaBase & media)
            : Strategy(media)
        {
        }

        BodyStrategy::~BodyStrategy()
        {
        }

        bool BodyStrategy::next_segment(
            bool is_next, 
            SegmentInfoEx & info)
        {
            bool res = false;
            is_next ? pos_++ : pos_;
            if (pos_ < media_.segment_count()) {
                media_.segment_info(pos_, info);
                info.try_times = 0;
                info.begin = info.head_size;
                info.end = info.size;
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

        error_code BodyStrategy::seek(
            size_t offset,
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::out_of_range;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, info);
                if (offset < (info.size - info.head_size)) {
                    info.begin = offset + info.head_size;
                    info.end = info.size;
                    info.position = offset + info.head_size;
                    info.size = info.end - info.begin;
                    pos_ = i;
                    ec.clear();
                    break;
                } else {
                    offset -= (info.size - info.head_size);
                }
            }
            return ec;
        }

        error_code BodyStrategy::seek(
            boost::uint32_t segment_index,
            size_t offset, 
            SegmentInfoEx & info, 
            boost::system::error_code & ec)
        {
            if (segment_index < media_.segment_count()) {
                media_.segment_info(segment_index, info);
                if (offset > (info.size - info.head_size)) {
                    ec = framework::system::logic_error::out_of_range;
                } else {
                    pos_ = segment_index;
                    info.begin = offset + info.head_size;
                    info.end = info.size;
                    info.size = info.end - info.begin;
                    info.position = offset + info.head_size;
                }
            } else {
                ec = framework::system::logic_error::out_of_range;
            }
            return ec;
        }

        std::size_t BodyStrategy::size(void)
        {
            std::size_t length = 0;
            SegmentInfo tmp;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, tmp);
                length += (tmp.size - tmp.head_size);
            }
            return length;
        }

    }
}
