// SegmentStrategy.h

#ifndef _PPBOX_DATA_SEGMENT_STRATEGY_H_
#define _PPBOX_DATA_SEGMENT_STRATEGY_H_

#include <ppbox/data/SegmentPosition.h>

namespace ppbox
{
    namespace data
    {

        class SegmentStrategy
        {
        public:
            SegmentStrategy(
                MediaBase const & media);

            virtual ~SegmentStrategy();

        public:
            virtual bool next_segment(
                SegmentPosition & pos, 
                boost::system::error_code & ec);

            virtual bool byte_seek(
                boost::uint64_t offset, 
                SegmentPosition & pos, 
                boost::system::error_code & ec);

            virtual bool time_seek(
                boost::uint64_t offset, 
                SegmentPosition & pos, 
                boost::system::error_code & ec);

            virtual bool get_url(
                SegmentPosition & pos, 
                boost::system::error_code & ec);

            virtual boost::uint64_t byte_size();

            virtual boost::uint64_t time_size();

            virtual void on_error(
                SegmentPosition const & pos, 
                boost::system::error_code & ec);

        private:
            virtual void byte_range(
                SegmentPosition const & pos, 
                SegmentRange & range);

            virtual void time_range(
                SegmentPosition const & pos, 
                SegmentRange & range);

        protected:
            MediaBase const & media_;
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_SEGMENT_STRATEGY_H_
