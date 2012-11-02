// SegmentBuffer.h

#ifndef _PPBOX_DATA_SEGMENT_BUFFER_H_
#define _PPBOX_DATA_SEGMENT_BUFFER_H_

#include "ppbox/data/SegmentStrategy.h"
#include "ppbox/data/Buffer.h"

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

        class BytesStream;

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

            // Ŀǰֻ�����ڣ�seek��һ���ֶΣ���û�и÷ֶ�ͷ������ʱ��
            // ��ʱsizeΪhead_size_ͷ�����ݴ�С
            // TO BE FIXED
            bool seek(
                segment_t const & base,
                segment_t const & pos,
                boost::uint64_t end, 
                boost::system::error_code & ec);

            // seek���ֶεľ���λ��offset
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
                ������ǰ�ֶε�����ʣ�����ݣ����Ҹ��µ�ǰ�ֶ���Ϣ
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

            // ��ȡ���һ�δ���Ĵ�����
            boost::system::error_code last_error() const
            {
                return last_ec_;
            }

            void pause_stream();

            void resume_stream();

        public:
            // д�ֶ�
            segment_t const & base_segment() const
            {
                return base_;
            }

            // ��BytesStream
            // ���ֶ�
            segment_t const & read_segment() const
            {
                return read_;
            }

            // д�ֶ�
            segment_t const & write_segment() const
            {
                return write_;
            }

        public:
            void detach_stream(
                BytesStream & stream);

            void attach_stream(
                BytesStream & stream, 
                bool read);

            void change_stream(
                BytesStream & stream, 
                bool read);

        private:
            friend class BytesStream;

            struct PositionType
            {
                enum Enum
                {
                    set, // ��Էֶ��ļ���ʼλ��
                    beg, // �����Ч������ʼλ��
                    end, // �����Ч���ݽ���λ��
                };
            };

            boost::system::error_code segment_buffer(
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
            boost::uint32_t prepare_size_;  // ����һ�Σ������������ݴ�С
            util::stream::StreamMutableBuffers prepare_buffers_;

            segment_t base_;
            std::deque<segment_t> segments_;
            segment_t read_;
            segment_t write_;

            BytesStream * read_stream_;    // ��Stream
            BytesStream * write_stream_;   // дStream

            prepare_response_type resp_;

            boost::system::error_code last_ec_;
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_SEGMENT_BUFFER_H_