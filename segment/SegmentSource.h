// SegmentSource.h

#ifndef _JUST_DATA_SEGMENT_SEGMENT_SOURCE_H_
#define _JUST_DATA_SEGMENT_SEGMENT_SOURCE_H_

#include "just/data/segment/SegmentStrategy.h"
#include "just/data/segment/SegmentEvent.h"
#include "just/data/base/DataStat.h"

#include <util/stream/Source.h>
#include <util/stream/UrlSource.h>

namespace just
{
    namespace data
    {

        class SegmentSource
            : public util::stream::Source
            , public DataWatcher
        {
        public:
            typedef SegmentPosition segment_t;

            typedef SegmentRange range_t;

            typedef util::stream::UrlSource::response_type response_t;

        public:
            SegmentSource(
                SegmentStrategy & strategy, 
                util::stream::UrlSource & source, 
                size_t total_req = 1);

            ~SegmentSource();

        public:
            boost::system::error_code seek(
                segment_t const & segment, 
                boost::uint64_t size, 
                boost::system::error_code & ec);

            boost::system::error_code seek(
                boost::uint64_t offset, 
                boost::uint64_t size, 
                boost::system::error_code & ec);

            boost::system::error_code seek(
                segment_t const & segment, 
                boost::system::error_code & ec)
            {
                return seek(segment, invalid_size, ec);
            }

            boost::system::error_code seek(
                boost::uint64_t offset, 
                boost::system::error_code & ec)
            {
                return seek(offset, invalid_size, ec);
            }

            void reset();

            void pause(
                boost::uint32_t time = 0);

        public:
            SegmentEvent segment_open;
            SegmentEvent segment_opened;
            SegmentEvent segment_close;

        public:
            util::stream::UrlSource const & source() const
            {
                return source_;
            }

            segment_t const & write_segment() const
            {
                return write_;
            }

        public:
            virtual bool cancel(
                boost::system::error_code & ec);

            virtual bool close(
                boost::system::error_code & ec);

            // ����false��ʾ�����ټ�����
        public:
            void set_max_try(
                size_t max_try)
            {
                max_try_ = max_try;
            }

            void set_max_req(
                size_t num);

            void set_time_out(
                boost::uint32_t time_out);

            void set_strategy(
                SegmentStrategy & strategy)
            {
                strategy_ = &strategy;
            }

            void clear_error()
            {
                error_ = boost::system::error_code();
            }

        private:
            virtual std::size_t private_read_some(
                buffers_t const & buffers,
                boost::system::error_code & ec);

            virtual void private_async_read_some(
                buffers_t const & buffers, 
                handler_t const & handler);

        private:
            void async_prepare(
                buffers_t const & buffers, 
                handler_t const & handler);

            void handle_async(
                buffers_t const & buffers, 
                handler_t const & handler, 
                boost::system::error_code const & ecc, 
                size_t bytes_transferred);

            void async_open_segment(
                bool is_next_segment, 
                response_t const & resp);

            void response(
                handler_t const & handler, 
                boost::system::error_code const & ec, 
                size_t bytes_transferred);

        private:
            size_t prepare(
                buffers_t const & buffers, 
                boost::system::error_code & ec);

            void update_segment(
                boost::system::error_code & ec);

            bool next_segment(
                segment_t & seg, 
                range_t & range, 
                boost::system::error_code & ec);

            bool open_request(
                bool is_next_segment,
                boost::system::error_code & ec);

            bool close_request(
                boost::system::error_code & ec);

            bool close_all_request(
                boost::system::error_code & ec);

            bool open_segment(
                bool is_next_segment, 
                boost::system::error_code & ec);

            bool segment_is_open(
                boost::system::error_code & ec);

            bool close_segment(
                boost::system::error_code & ec);

        private:
            bool handle_error(
                boost::system::error_code& ec);

        private:
            size_t max_try_;                // ����ֵ�������������������������ϵ������ı�־
            size_t max_req_;                // ����ֵ�������д���������
            boost::uint32_t time_out_;      // ����ֵ����ʱʱ�䣨�룩

            SegmentStrategy * strategy_;
            util::stream::UrlSource & source_;

            size_t sended_req_;             // ��ǰ���д���������
            bool source_closed_;            // ture�����Ե��� open_segment(), ���� source_->open_segment()���ʧ�ܣ�Ϊfalse
            boost::uint32_t time_block_;    // �����������룩

            boost::system::error_code error_;    // ���صĴ�����

            segment_t write_;
            range_t write_range_;
            segment_t write_tmp_;

            boost::uint64_t seek_end_;      // һ����seek����ʱ�������ȡͷ�����ݣ�ֵΪ��ǰ�ֶ�֮ǰ�ķֶ��ܳ�+��ǰ�ֶε�head_size_������Ϊ-1

            Time expire_pause_time_;
        };

    } // namespace demux
} // namespace just

#endif // _JUST_DATA_SEGMENT_SEGMENT_SOURCE_H_
