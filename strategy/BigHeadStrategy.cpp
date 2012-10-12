//  BigHeadStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/strategy/BigHeadStrategy.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {
        BigHeadStrategy::BigHeadStrategy(
            MediaBase & media)
            : SegmentStrategy(media)
        {
        }

        BigHeadStrategy::~BigHeadStrategy()
        {
        }

        bool BigHeadStrategy::next_segment(
            SegmentPosition & pos, 
            boost::system::error_code & ec)
        {
            if (pos.item_context == NULL || pos.index == size_t(-1)) {
                pos.item_context = this;
                MediaInfo minfo;
                media_.get_info(minfo, ec);
                SegmentInfo sinfo;
                media_.segment_info(0, sinfo);
                pos.index = 0;
                if (pos.url.is_valid())
                    pos.url = minfo.cdn_url;
                pos.size = minfo.file_size;
                pos.byte_range.before_next();
                pos.byte_range.beg = 0;
                pos.byte_range.end = sinfo.offset;
                pos.byte_range.after_next();
                return true;
            } else {
                return false;
            }
        }

    } // namespace data
} // namespace ppbox
