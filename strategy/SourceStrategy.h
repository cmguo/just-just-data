// SourceStrategy.h

#ifndef     _PPBOX_DATA_SOURCE_STRATEGY_
#define     _PPBOX_DATA_SOURCE_STRATEGY_

#include <ppbox/common/Call.h>
#include <ppbox/common/Create.h>
#include <ppbox/data/SegmentBase.h>

namespace ppbox
{
    namespace data
    {

        struct SegmentInfoEx
            : SegmentInfo
        {
            SegmentInfoEx()
                : try_times(0)
                , offset(0)
                , begin(0)
                , end(0)

            {
            }

            boost::uint32_t try_times;
            boost::uint64_t offset;
            boost::uint64_t begin;
            boost::uint64_t end;
            framework::string::Url url;
        };

#define PPBOX_REGISTER_STRATEGY(n, c) \
    static ppbox::common::Call reg_ ## n(ppbox::data::SourceStrategy::regist_strategy, BOOST_PP_STRINGIZE(n), ppbox::common::Creator<c>())

        class SourceStrategy
        {
        public:

            typedef boost::function<SourceStrategy * (
                std::vector<SegmentInfoEx> const &,
                VideoInfo const & video_info)
            > register_type;

            static void regist_strategy(
                std::string const & type, 
                SourceStrategy::register_type func);

            static SourceStrategy *create(
                std::string const & type, 
                std::vector<SegmentInfoEx> const & segments,
                VideoInfo const & video_info);

            static void destory(SourceStrategy* & strategy);

        public:
            SourceStrategy(
                std::vector<SegmentInfoEx> const & segments, 
                VideoInfo const & video_info);

            virtual ~SourceStrategy();

            virtual bool next_segment(
                bool is_next,
                SegmentInfoEx & info);

            virtual boost::system::error_code on_seek(
                size_t offset,
                SegmentInfoEx & info, 
                boost::system::error_code & ec);

            virtual boost::system::error_code on_seek(
                boost::uint32_t segment_index,
                size_t offset, 
                SegmentInfoEx & info, 
                boost::system::error_code & ec);

            virtual std::size_t size(void);

        protected:
            std::vector<SegmentInfoEx> const & segments_;
            VideoInfo const & video_info_;
            boost::uint32_t pos_;

        private:
            static std::map<std::string, SourceStrategy::register_type> & strategy_map();
        };

        PPBOX_REGISTER_STRATEGY(full, SourceStrategy);
    }
}

#endif // End _PPBOX_DATA_SOURCE_STRATEGY_ 
