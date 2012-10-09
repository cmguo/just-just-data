// SegmentSource

#ifndef   _PPBOX_SEGMENT_SEGMENT_SOURCE_H_
#define   _PPBOX_SEGMENT_SEGMENT_SOURCE_H_

#include "ppbox/data/SourceBase.h"
#include "ppbox/data/DataStatistic.h"
#include "ppbox/data/strategy/Strategy.h"

#include <ppbox/cdn/CdnModule.h>
#include <framework/string/Url.h>

namespace ppbox
{
    namespace data
    {

        struct SegmentInfoEx2
            : public SegmentInfoEx
        {
            SegmentInfoEx2()
                : try_times(0)
            {
            }

            boost::uint32_t try_times;

            void reset()
            {
                index = boost::uint32_t(-1);
            }
        };

        class SegmentSource
            : public SourceBase
            , public DataObserver
        {
        public:
            SegmentSource(boost::asio::io_service & io_srv);

            virtual ~SegmentSource();

            virtual boost::system::error_code open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec);

            virtual void async_open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                response_type const & resp);

            virtual bool is_open(
                boost::system::error_code & ec);

            virtual boost::system::error_code cancel(
                boost::system::error_code & ec);

            virtual boost::system::error_code close(
                boost::system::error_code & ec);

            virtual boost::uint64_t total(
                boost::system::error_code & ec);

            virtual boost::system::error_code set_non_block(
                bool non_block, 
                boost::system::error_code & ec);

            virtual boost::system::error_code set_time_out(
                boost::uint32_t time_out, 
                boost::system::error_code & ec);

            virtual bool continuable(
                boost::system::error_code const & ec);

            virtual bool recoverable(
                boost::system::error_code const & ec);

        public:
            boost::system::error_code seek(
                boost::uint64_t offset, 
                boost::system::error_code & ec);

            boost::system::error_code seek(
                boost::uint64_t offset, 
                boost::uint32_t size, 
                boost::system::error_code & ec);

            boost::system::error_code seek(
                SegmentInfoEx & info, 
                boost::system::error_code & ec);

            boost::system::error_code seek(
                SegmentInfoEx & info, 
                boost::uint32_t size, 
                boost::system::error_code & ec);

            void reset(void);

            void set_max_try(boost::uint32_t times);

            SourceBase * source(void) const;

            void source(SourceBase * source);

            void strategy(Strategy * strategy);

            Strategy * strategy(void) const;

            void current_segment(SegmentInfoEx & info);

        private:
            // implement util::stream::Source
            virtual std::size_t private_read_some(
                util::stream::StreamMutableBuffers const & buffers,
                boost::system::error_code & ec);

            virtual void private_async_read_some(
                util::stream::StreamMutableBuffers const & buffers,
                util::stream::StreamHandler const & handler);

        private:
            std::size_t prepare_read(
                util::stream::StreamMutableBuffers const & buffers,
                boost::system::error_code & ec);

            bool handle_error(
                boost::system::error_code& ec);

            void update_segments(void);

            void update_video_info(void);

            boost::system::error_code open_segment(
                boost::system::error_code & ec);

            boost::system::error_code close_segment(
                boost::system::error_code & ece);

            boost::system::error_code open_request(
                boost::system::error_code & ec);

            boost::system::error_code close_request(
                boost::system::error_code & ec);

        private:
            MediaBase * media_;
            SourceBase * source_;
            Strategy * strategy_;

            // 已经关闭当前请求
            bool request_closed_;
            // 已经关闭当前分段
            bool segment_closed_;
            boost::uint64_t download_end_offset_;
            boost::uint32_t time_out_;
            boost::uint32_t max_try_;

            SegmentInfoEx2 cur_segment_;
            boost::system::error_code source_error_;
        };
    }
}

#endif // END _PPBOX_SEGMENT_SEGMENT_SOURCE_H_

