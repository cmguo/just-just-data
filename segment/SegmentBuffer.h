// SegmentBuffer.h

#ifndef _PPBOX_DATA_SEGMENT_SEGMENT_BUFFER_H_
#define _PPBOX_DATA_SEGMENT_SEGMENT_BUFFER_H_

#include "ppbox/data/segment/SegmentStrategy.h"
#include "ppbox/data/base/Buffer.h"

#include <util/event/Event.h>
#include <util/stream/StreamBuffers.h>

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

        public:
            SegmentBuffer(
                ppbox::data::SegmentSource & source, 
                boost::uint32_t buffer_size, 
                boost::uint32_t prepare_size);

            ~SegmentBuffer();

        public:
            // Ŀǰֻ�����ڣ�seek��һ���ֶΣ���û�и÷ֶ�ͷ������ʱ��
            // ��ʱsizeΪhead_size_ͷ�����ݴ�С
            // TO BE FIXED
            bool seek(
                SegmentPosition const & base,
                SegmentPosition const & pos,
                boost::uint64_t size, 
                boost::system::error_code & ec);

            // seek���ֶεľ���λ��offset
            // TO BE FIXED
            bool seek(
                SegmentPosition const & base,
                SegmentPosition const & pos, 
                boost::system::error_code & ec);

            bool seek(
                SegmentPosition const & pos,
                boost::uint64_t size, 
                boost::system::error_code & ec);

            bool seek(
                SegmentPosition const & pos, 
                boost::system::error_code & ec);

            // Parameter: boost::uint32_t amount ��Ҫ���ص����ݴ�С
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

            MemoryLock * fetch(
                boost::uint32_t track, 
                boost::uint64_t offset, 
                boost::uint32_t size, 
                bool merge, 
                std::deque<boost::asio::const_buffer> & data, 
                boost::system::error_code & ec);

            MemoryLock * fetch(
                boost::uint32_t track, 
                std::vector<DataBlock> & blocks, 
                bool merge, 
                std::deque<boost::asio::const_buffer> & data, 
                boost::system::error_code & ec);

            void putback(
                MemoryLock * mlock);

            /**
                read_next 
                ������ǰ�ֶε�����ʣ�����ݣ����Ҹ��µ�ǰ�ֶ���Ϣ
             */
            bool read_next(
                boost::uint64_t duration, 
                boost::system::error_code & ec);

            bool read_next(
                boost::uint64_t duration, 
                boost::uint64_t min_offset, 
                boost::system::error_code & ec);

            bool write_next(
                boost::uint64_t duration, 
                boost::system::error_code & ec);

            void clear();

        public:
            ppbox::data::SegmentSource const & source() const
            {
                return source_;
            }

            // ��ȡ���һ�δ���Ĵ�����
            boost::system::error_code last_error() const
            {
                return last_ec_;
            }

            void pause_stream();

            void resume_stream();

        public:
            // д�ֶ�
            SegmentPosition const & base_segment() const
            {
                return base_;
            }

            // ��BytesStream
            // ���ֶ�
            SegmentPosition const & read_segment() const
            {
                return read_;
            }

            // д�ֶ�
            SegmentPosition const & write_segment() const
            {
                return write_;
            }

            bool read_has_more() const;

            bool write_has_more() const;

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
                    set, // ��Էֶ��ļ���ʼλ��
                    beg, // �����Ч������ʼλ��
                    end, // �����Ч���ݽ���λ��
                };
            };

            bool segment_seek(
                SegmentPosition & segment, 
                bool merge, 
                boost::uint64_t pos);

            bool segment_buffer(
                SegmentPosition & segment, 
                PositionType::Enum pos_type, 
                bool merge, 
                boost::uint64_t & pos, 
                boost::uint64_t & off, 
                boost::asio::const_buffer & buffer);

        private:
            void on_event(
                util::event::Observable const & sender, 
                util::event::Event const & event);

        private:
            void reset(
                SegmentPosition const & base, 
                SegmentPosition const & pos, 
                boost::system::error_code & ec);

            void clear_segments();

            void insert_segment(
                bool is_read, 
                SegmentPosition const & seg);

            void find_segment(
                boost::uint64_t offset, 
                SegmentPosition & seg);

            void handle_async(
                boost::system::error_code const & ecc, 
                size_t bytes_transferred);

            void dump();

        private:
            ppbox::data::SegmentSource & source_;
            boost::uint32_t prepare_size_;  // ����һ�Σ������������ݴ�С
            util::stream::StreamMutableBuffers prepare_buffers_;

            SegmentPosition base_;
            std::deque<SegmentPosition> segments_;
            SegmentPosition read_;
            SegmentPosition write_;

            SegmentStream * read_stream_;    // ��Stream
            SegmentStream * write_stream_;   // дStream

            prepare_response_type resp_;

            boost::system::error_code last_ec_;
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_SEGMENT_SEGMENT_BUFFER_H_
