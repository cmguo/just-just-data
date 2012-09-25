// Strategy.h

#ifndef     _PPBOX_DATA_STRATEGY_
#define     _PPBOX_DATA_STRATEGY_

#include <ppbox/common/Call.h>
#include <ppbox/common/Create.h>
#include <ppbox/data/MediaBase.h>

namespace ppbox
{
    namespace data
    {

        struct SegmentInfoEx
            : SegmentInfo
        {
            SegmentInfoEx()
                : index(0)
                , begin(0)
                , end(0)
                , big_offset(0)
                , small_offset(0)
            {
            }

            boost::uint32_t index;
            boost::uint64_t begin;
            boost::uint64_t end;
            // 相对于所有的偏移量
            boost::uint64_t big_offset;
            // 是相对当前段的偏移量
            boost::uint64_t small_offset;

            boost::uint64_t big_beg() const
            {
                return big_offset + begin;
            }

            boost::uint64_t big_end() const
            {
                return big_offset + end;
            }

            boost::uint64_t big_pos() const
            {
                return big_offset + small_offset;
            }

            friend bool operator<(
                SegmentInfoEx const & l, 
                SegmentInfoEx const & r)
            {
                return (l.index < r.index || 
                    (l.index == r.index && l.begin < r.begin));
            }

            friend bool operator==(
                SegmentInfoEx const & l, 
                SegmentInfoEx const & r)
            {
                return (l.index == r.index && l.begin == r.begin && l.end == r.end);
            }

            friend bool operator!=(
                SegmentInfoEx const & l, 
                SegmentInfoEx const & r)
            {
                return !(l == r);
            }

        };

#define PPBOX_REGISTER_STRATEGY(n, c) \
    static ppbox::common::Call \
    reg_ ## n(ppbox::data::Strategy::regist_strategy, \
    BOOST_PP_STRINGIZE(n), \
    ppbox::common::Creator<c>())

        class Strategy
        {
        public:

            typedef boost::function<Strategy * (
                MediaBase &)
            > register_type;

            static void regist_strategy(
                std::string const & type, 
                Strategy::register_type func);

            static Strategy *create(
                std::string const & type, 
                MediaBase & media);

            static void destory(Strategy* & strategy);

        public:
            Strategy(MediaBase & media);

            virtual ~Strategy();

            virtual bool next_segment(
                bool is_next,
                SegmentInfoEx & info) = 0;

            virtual boost::system::error_code byte_seek(
                size_t offset,
                SegmentInfoEx & info, 
                boost::system::error_code & ec) = 0;

            virtual boost::system::error_code time_seek(
                boost::uint32_t time_ms, 
                SegmentInfoEx & info, 
                boost::system::error_code & ec) = 0;

            //virtual boost::system::error_code seek(
            //    boost::uint32_t segment_index,
            //    size_t offset, 
            //    SegmentInfoEx & info, 
            //    boost::system::error_code & ec) = 0;

            virtual std::size_t size(void) = 0;

        protected:
            MediaBase & media_;
            boost::uint32_t pos_;

        private:
            static std::map<std::string, Strategy::register_type> & strategy_map();
        };

    }
}

#endif // End _PPBOX_DATA_STRATEGY_ 
