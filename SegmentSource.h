// SegmentSource

#ifndef   _PPBOX_SEGMENT_SEGMENT_SOURCE_H_
#define   _PPBOX_SEGMENT_SEGMENT_SOURCE_H_

#include "ppbox/data/SourceBase.h"
#include "ppbox/data/DataStatistic.h"
#include "ppbox/data/strategy/Strategy.h"

#include <ppbox/cdn/Cdn.h>
#include <framework/string/Url.h>

namespace ppbox
{
    namespace data
    {

        struct SegmentInfoEx2
            : public SegmentInfoEx
        {
            boost::uint32_t try_times;
            framework::string::Url url;
        };

        class SegmentSource
            : public SourceBase
            , public DataObserver
        {
        public:
            struct State
            {
                enum Enum
                {
                    not_open = 0, 
                    segment_opening, 
                    segment_opened, 
                    source_not_open, 
                    source_opening, 
                    source_opened, 
                };
            };

        public:
            SegmentSource(boost::asio::io_service & io_srv);

            virtual ~SegmentSource();

            // inheritance from : source base
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
            boost::system::error_code byte_seek(
                size_t offset, 
                boost::system::error_code & ec);

            boost::system::error_code byte_seek(
                size_t offset, 
                size_t size, 
                boost::system::error_code & ec);

            boost::system::error_code time_seek(
                boost::uint32_t time_ms,
                boost::system::error_code & ec);

            void reset(void);

            void set_max_try(boost::uint32_t times);

            void set_time_out(boost::uint32_t time);

            void set_strategy(Strategy * strategy);

            void current_segment(SegmentInfoEx & info);

            MediaBase * media(void) const;

            SourceBase * source(void) const;

        private:
            // implement util::stream::Source
            virtual std::size_t private_read_some(
                util::stream::StreamMutableBuffers const & buffers,
                boost::system::error_code & ec);

            virtual void private_async_read_some(
                util::stream::StreamMutableBuffers const & buffers,
                util::stream::StreamHandler const & handler);

        private:
            void hand_async_open(boost::system::error_code const & ec);

            std::size_t prepare_read(
                util::stream::StreamMutableBuffers const & buffers,
                boost::system::error_code & ec);

            bool handle_error(
                boost::system::error_code& ec);

            void update_segments(void);

            void update_video_info(void);

            boost::system::error_code open_segment(
                bool is_next_segment, 
                boost::system::error_code & ec);

            boost::system::error_code close_segment(
                boost::system::error_code & ece);

            boost::system::error_code open_request(
                boost::system::error_code & ec);

            boost::system::error_code close_request(
                boost::system::error_code & ec);

            boost::system::error_code hand_seek(
                boost::system::error_code & ec);

        private:
            MediaBase * media_;
            SourceBase * source_;
            framework::string::Url playlink_;

            Strategy * strategy_;
            bool source_closed_;
            bool has_seek_;
            boost::uint64_t need_size_;
            boost::uint32_t time_out_;
            boost::uint32_t max_try_;
            State::Enum state_;

            response_type resp_;
            SegmentInfoEx2 cur_segment_;

             boost::system::error_code source_error_;
        };
    }
}

#endif // END _PPBOX_SEGMENT_SEGMENT_SOURCE_H_

