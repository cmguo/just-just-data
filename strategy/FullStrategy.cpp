// FullStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/FullStrategy.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {

        FullStrategy::FullStrategy(MediaBase & media)
            : Strategy(media)
        {
        }

        FullStrategy::~FullStrategy()
        {
        }

        bool FullStrategy::next_segment(
            bool is_next,
            SegmentInfoEx & info)
        {
            bool res = false;
            is_next ? pos_++ : pos_;
            if (pos_ < media_.segment_count()) {
                media_.segment_info(pos_, info);
                info.try_times = 0;
                info.begin = 0;
                info.end = boost::uint64_t(-1);
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

        error_code FullStrategy::seek(
            size_t offset,
            SegmentInfoEx & info, 
            error_code & ec)
        {
            ec = framework::system::logic_error::out_of_range;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, info);
                if (offset < info.size) {
                    info.begin = offset;
                    info.end = info.size;
                    info.position = offset;
                    info.size = info.end - info.begin;
                    pos_ = i;
                    ec.clear();
                    break;
                } else {
                    offset -= info.size;
                }
            }
            return ec;
        }

        error_code FullStrategy::seek(
            boost::uint32_t segment_index,
            size_t offset, 
            SegmentInfoEx & info, 
            error_code & ec)
        {
            if (segment_index < media_.segment_count()) {
                media_.segment_info(segment_index, info);
                if (offset > info.size) {
                    ec = framework::system::logic_error::out_of_range;
                } else {
                    pos_ = segment_index;
                    info.begin = offset;
                    info.end = info.size;
                    info.size = info.end - info.begin;
                    info.position = offset;
                }
            } else {
                ec = framework::system::logic_error::out_of_range;
            }
            return ec;
        }

        std::size_t FullStrategy::size(void)
        {
            std::size_t length = 0;
            SegmentInfo tmp;
            for (boost::uint32_t i = 0; i < media_.segment_count(); ++i) {
                media_.segment_info(i, tmp);
                length += tmp.size;
            }
            return length;
        }

    }
}
