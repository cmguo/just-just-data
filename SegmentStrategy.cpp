// SegmentStrategy.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/SegmentStrategy.h"
#include "ppbox/data/SourceError.h"

namespace ppbox
{
    namespace data
    {

        SegmentStrategy::SegmentStrategy(
            MediaBase const & media)
            : media_(media)
        {
        }

        SegmentStrategy::~SegmentStrategy()
        {
        }

        bool SegmentStrategy::next_segment(
            SegmentPosition & pos, 
            boost::system::error_code & ec)
        {
            if (++pos.index >= media_.segment_count()) {
                ec = source_error::no_more_segment;
                return false;
            }
            media_.segment_info(pos.index, pos);
            if (pos.url.is_valid())
                media_.segment_url(pos.index, pos.url, ec);

            pos.byte_range.before_next();
            byte_range(pos, pos.byte_range);
            pos.byte_range.after_next();

            pos.time_range.before_next();
            time_range(pos, pos.time_range);
            pos.time_range.after_next();

            return true;
        }

        bool SegmentStrategy::byte_seek(
            boost::uint64_t offset, 
            SegmentPosition & pos, 
            boost::system::error_code & ec)
        {
            pos.url.protocol("");

            if (offset < pos.byte_range.big_beg()) {
                pos = SegmentPosition();
                if (!next_segment(pos, ec)) {
                    assert(0);
                    return false;
                }
            }

            while (offset >= pos.byte_range.big_beg()) {
                if (!next_segment(pos, ec)) {
                    return false;
                }
            }

            media_.segment_url(pos.index, pos.url, ec);

            return true;
        }

        bool SegmentStrategy::time_seek(
            boost::uint64_t offset, 
            SegmentPosition & pos, 
            boost::system::error_code & ec)
        {
            pos.url.protocol("");

            if (offset < pos.time_range.big_beg()) {
                pos = SegmentPosition();
                if (!next_segment(pos, ec)) {
                    assert(0);
                    return false;
                }
            }

            while (offset >= pos.time_range.big_beg()) {
                if (!next_segment(pos, ec)) {
                    return false;
                }
            }

            media_.segment_url(pos.index, pos.url, ec);

            return true;
        }

        boost::uint64_t SegmentStrategy::byte_size()
        {
            SegmentPosition pos;
            boost::system::error_code ec;
            while (next_segment(pos, ec));
            return pos.byte_range.big_end();
        }

        boost::uint64_t SegmentStrategy::time_size()
        {
            SegmentPosition pos;
            boost::system::error_code ec;
            while (next_segment(pos, ec));
            return pos.time_range.big_end();
        }

        void SegmentStrategy::byte_range(
            SegmentPosition const & pos, 
            SegmentRange & range)
        {
            range.beg = 0;
            range.end = pos.size;
        }

        void SegmentStrategy::time_range(
            SegmentPosition const & pos, 
            SegmentRange & range)
        {
            range.beg = 0;
            range.end = pos.duration;
        }

    } // namespace data
} // namespace ppbox
