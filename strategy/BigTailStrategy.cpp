// BigTailStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/BigTailStrategy.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {

        BigTailStrategy::BigTailStrategy(
            MediaBase & media)
            : SegmentStrategy(media)
        {
        }

        BigTailStrategy::~BigTailStrategy()
        {
        }

        bool BigTailStrategy::next_segment(
            SegmentPosition & pos, 
            boost::system::error_code & ec)
        {
            if (pos.item_context == NULL || pos.index == size_t(-1)) {
                pos.item_context = this;
                MediaInfo minfo;
                media_.get_info(minfo, ec);
                SegmentInfo sinfo;
                media_.segment_info(media_.segment_count() - 1, sinfo);
                pos.index = 0;
                if (pos.url.is_valid())
                    media_.get_url(pos.url, ec);
                pos.size = minfo.file_size;
                pos.byte_range.before_next();
                pos.byte_range.beg = sinfo.offset + sinfo.size - sinfo.head_size;
                pos.byte_range.end = minfo.file_size;
                pos.byte_range.after_next();
                return true;
            } else {
                return false;
            }
        }

    } // namespace data
} // namespace ppbox
