// SingleSource.h

#ifndef _JUST_DATA_BASE_SINGLE_SOURCE_H_
#define _JUST_DATA_BASE_SINGLE_SOURCE_H_

#include "just/data/base/DataStat.h"
#include "just/data/segment/SegmentPosition.h" // for SegmentRange

#include <util/stream/Source.h>
#include <util/stream/UrlSource.h>

namespace just
{
    namespace data
    {

        class SingleSource
            : public util::stream::Source
            , public DataWatcher
        {
        public:
            typedef SegmentRange range_t;

            typedef util::stream::UrlSource::response_type response_t;

        public:
            SingleSource(
                framework::string::Url const & url, 
                util::stream::UrlSource & source, 
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
            util::stream::UrlSource const & source() const
            {
                return source_;
            }

            boost::uint64_t total_size() const
            {
                return write_range_.end;
            }

        public:
            virtual bool cancel(
                boost::system::error_code & ec);

            virtual bool close(
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

            void async_open_source(
                response_t const & resp);

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
            size_t max_try_;                // 配置值：尝试重连最大次数，决定不断点续传的标志
            size_t max_req_;                // 配置值：最大队列串行请求数
            boost::uint32_t time_out_;      // 配置值：超时时间（秒）

            framework::string::Url const & url_;
            util::stream::UrlSource & source_;

            bool source_open_;
            bool source_is_open_;

            boost::system::error_code error_;    // 下载的错误码

            range_t write_range_;
            boost::uint64_t seek_end_;      // 一般在seek操作时，如果获取头部数据，值为当前分段之前的分段总长+当前分段的head_size_；否则为-1

            framework::timer::Time expire_pause_time_;
        };

    } // namespace demux
} // namespace just

#endif // _JUST_DATA_BASE_SINGLE_SOURCE_H_
