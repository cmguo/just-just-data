// SegmentBuffer.h

#ifndef _PPBOX_DATA_SEGMENT_SEGMENT_BUFFER_H_
#define _PPBOX_DATA_SEGMENT_SEGMENT_BUFFER_H_

#include "ppbox/data/segment/SegmentStrategy.h"
#include "ppbox/data/base/Buffer.h"

#include <util/event/Event.h>
#include <util/stream/StreamBuffers.h>

#include <framework/system/LogicError.h>

namespace ppbox
{
    namespace data
    {
        class SegmentSource;
    }

    namespace data
    {

        class SegmentStream;

        class SegmentBuffer
            : public Buffer
        {
        public:
            typedef boost::function<void (
                boost::system::error_code const &,
                size_t)
            > prepare_response_type;

            //typedef ppbox::data::SegmentInfoEx segment_t;
            typedef ppbox::data::SegmentPosition segment_t;

        public:
            SegmentBuffer(
                ppbox::data::SegmentSource & source, 
                boost::uint32_t buffer_size, 
                boost::uint32_t prepare_size);

            ~SegmentBuffer();

            // 目前只发生在，seek到一个分段，还没有该分段头部数据时，
            // 此时size为head_size_头部数据大小
            // TO BE FIXED
            bool seek(
                segment_t const & base,
                segment_t const & pos,
                boost::uint64_t end, 
                boost::system::error_code & ec);

            // seek到分段的具体位置offset
            // TO BE FIXED
            bool seek(
                segment_t const & base,
                segment_t const & pos, 
                boost::system::error_code & ec);

            bool seek(
                segment_t const & pos,
                boost::uint64_t end, 
                boost::system::error_code & ec);

            bool seek(
                segment_t const & pos, 
                boost::system::error_code & ec);

            // Parameter: boost::uint32_t amount 需要下载的数据大小
            // Parameter: boost::system::error_code & ec
            size_t prepare(
                size_t amount, 
                boost::system::error_code & ec);

            size_t prepare_some(
                size_t amount, 
                boost::system::error_code & ec)
            {
                return prepare(amount > prepare_size_ ? amount : prepare_size_, ec);
            }

            size_t prepare_some(
                boost::system::error_code & ec)
            {
                return prepare(prepare_size_, ec);
            }

            size_t prepare_at_least(
                size_t amount, 
                boost::system::error_code & ec);

            void async_prepare(
                size_t amount, 
                prepare_response_type const & resp);

            void async_prepare_some(
                size_t amount, 
                prepare_response_type const & resp)
            {
                async_prepare(amount > prepare_size_ ? amount : prepare_size_, resp);
            }

            bool fetch(
                boost::uint64_t offset, 
                boost::uint32_t size, 
                std::deque<boost::asio::const_buffer> & data, 
                boost::system::error_code & ec);

            bool drop(
                boost::system::error_code & ec);

            bool drop_to(
                boost::uint64_t offset, 
                boost::system::error_code & ec);

            /**
                drop_all 
                丢弃当前分段的所有剩余数据，并且更新当前分段信息
             */
            bool drop_all(
                boost::uint64_t duration, 
                boost::system::error_code & ec);

            bool write_next(
                segment_t & segment, 
                boost::system::error_code & ec);

            void clear();

        public:
            ppbox::data::SegmentSource const & source() const
            {
                return source_;
            }

            // 获取最后一次错误的错误码
            boost::system::error_code last_error() const
            {
                return last_ec_;
            }

            void pause_stream();

            void resume_stream();

        public:
            // 写分段
            segment_t const & base_segment() const
            {
                return base_;
            }

            // 读BytesStream
            // 读分段
            segment_t const & read_segment() const
            {
                return read_;
            }

            // 写分段
            segment_t const & write_segment() const
            {
                return write_;
            }

        public:
            void detach_stream(
                SegmentStream & stream);

            void attach_stream(
                SegmentStream & stream, 
                bool read);

            void change_stream(
                SegmentStream & stream, 
                bool read);

        private:
            friend class SegmentStream;

            struct PositionType
            {
                enum Enum
                {
                    set, // 相对分段文件起始位置
                    beg, // 相对有效数据起始位置
                    end, // 相对有效数据结束位置
                };
            };

            bool segment_seek(
                segment_t const & segment, 
                boost::uint64_t pos);

            bool segment_buffer(
                segment_t const & segment, 
                PositionType::Enum pos_type, 
                boost::uint64_t & pos, 
                boost::uint64_t & off, 
                boost::asio::const_buffer & buffer);

        private:
            virtual void on_event(
                util::event::Event const & event);

        private:
            void reset(
                segment_t const & base, 
                segment_t const & pos);

            void clear_segments();

            void insert_segment(
                bool is_read, 
                segment_t const & seg);

            void find_segment(
                boost::uint64_t offset, 
                segment_t & seg);

            void handle_async(
                boost::system::error_code const & ecc, 
                size_t bytes_transferred);

            void dump();

        private:
            ppbox::data::SegmentSource & source_;
            boost::uint32_t prepare_size_;  // 下载一次，最大的下载数据大小
            util::stream::StreamMutableBuffers prepare_buffers_;

            segment_t base_;
            std::deque<segment_t> segments_;
            segment_t read_;
            segment_t write_;

            SegmentStream * read_stream_;    // 读Stream
            SegmentStream * write_stream_;   // 写Stream

            prepare_response_type resp_;

            boost::system::error_code last_ec_;
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_SEGMENT_SEGMENT_BUFFER_H_
