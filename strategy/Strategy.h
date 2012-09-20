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
                : try_times(0)
                , position(0)
                , begin(0)
                , end(0)

            {
            }

            boost::uint32_t try_times;
            boost::uint64_t position;
            boost::uint64_t begin;
            boost::uint64_t end;
            framework::string::Url url;
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
                SegmentInfoEx & info);

            virtual boost::system::error_code seek(
                size_t offset,
                SegmentInfoEx & info, 
                boost::system::error_code & ec);

            virtual boost::system::error_code seek(
                boost::uint32_t segment_index,
                size_t offset, 
                SegmentInfoEx & info, 
                boost::system::error_code & ec);

            virtual std::size_t size(void);

        protected:
            MediaBase & media_;
            boost::uint32_t pos_;

        private:
            static std::map<std::string, Strategy::register_type> & strategy_map();
        };

    }
}

#endif // End _PPBOX_DATA_STRATEGY_ 
