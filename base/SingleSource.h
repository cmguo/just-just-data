// SingleSource.h

#ifndef _PPBOX_DATA_BASE_SINGLE_SOURCE_H_
#define _PPBOX_DATA_BASE_SINGLE_SOURCE_H_

#include "ppbox/data/base/DataStatistic.h"
#include "ppbox/data/base/SourceBase.h"
#include "ppbox/data/segment/SegmentPosition.h" // for SegmentRange

#include <util/stream/Source.h>
#include <util/event/Observable.h>

namespace ppbox
{
    namespace data
    {

        class SingleSource
            : public util::stream::Source
            , public DataObserver
            , public util::event::Observable
        {
        public:
            typedef SegmentRange range_t;

        public:
            SingleSource(
                framework::string::Url const & url, 
                SourceBase & source, 
                size_t total_req = 1);

            ~SingleSource();

        public:
            boost::system::error_code seek(
                boost::uint64_t offset, 
                boost::uint64_t size, 
                boost::system::error_code & ec);

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
            ppbox::data::SourceBase const & source() const
            {
                return source_;
            }

            size_t num_try() const
            {
                return num_try_;
            }

        public:
            virtual boost::system::error_code cancel(
                boost::system::error_code & ec);

            virtual boost::system::error_code close(
                boost::system::error_code & ec);

        public:
            void set_max_try(
                size_t max_try)
            {
                max_try_ = max_try;
            }

            void set_time_out(
                boost::uint32_t time_out);

            void clear_error()
            {
                source_error_ = boost::system::error_code();
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
                SourceBase::response_type const & resp);

            void response(
                handler_t const & handler, 
                boost::system::error_code const & ec, 
                size_t bytes_transferred);

        private:
            size_t prepare(
                buffers_t const & buffers, 
                boost::system::error_code & ec);

            void update_size(
                boost::system::error_code & ec);

            bool open_source(
                boost::system::error_code & ec);

            bool source_is_open(
                boost::system::error_code & ec);

            bool close_source(
                boost::system::error_code & ec);

        private:
            bool handle_error(
                boost::system::error_code& ec);

        private:
            size_t max_try_;                // ����ֵ�������������������������ϵ������ı�־
            size_t max_req_;                // ����ֵ�������д���������
            boost::uint32_t time_out_;      // ����ֵ����ʱʱ�䣨�룩

            framework::string::Url const & url_;
            SourceBase & source_;

            size_t num_try_;                // ��ǰ�ֶγ��Դ���
            bool source_open_;
            bool source_is_open_;

            boost::system::error_code source_error_;    // ���صĴ�����

            range_t write_range_;
            boost::uint64_t seek_end_;      // һ����seek����ʱ�������ȡͷ�����ݣ�ֵΪ��ǰ�ֶ�֮ǰ�ķֶ��ܳ�+��ǰ�ֶε�head_size_������Ϊ-1

            Time expire_pause_time_;
        };

    } // namespace demux
} // namespace ppbox

#endif // _PPBOX_DATA_BASE_SINGLE_SOURCE_H_
