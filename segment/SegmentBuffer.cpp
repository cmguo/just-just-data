// SegmentBuffer.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/segment/SegmentBuffer.h"
#include "ppbox/data/segment/SegmentStream.h"
#include "ppbox/data/segment/SegmentSource.h"
#include "ppbox/data/segment/SegmentEvent.h"
#include "ppbox/data/base/Error.h"

#include <framework/system/LogicError.h>
#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>

namespace ppbox
{
    namespace data
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.SegmentBuffer", framework::logger::Debug);

        SegmentBuffer::SegmentBuffer(
            ppbox::data::SegmentSource & source, 
            boost::uint32_t buffer_size, 
            boost::uint32_t prepare_size)
            : Buffer(buffer_size)
            , source_(source)
            , prepare_size_(prepare_size)
            , read_stream_(NULL)
            , write_stream_(NULL)
        {
            //base_.index = 1; //
            source_.segment_open.on(boost::bind(&SegmentBuffer::on_event, this, _1, _2));
            source_.segment_close.on(boost::bind(&SegmentBuffer::on_event, this, _1, _2));
        }

        SegmentBuffer::~SegmentBuffer()
        {
            source_.segment_close.un(boost::bind(&SegmentBuffer::on_event, this, _1, _2));
            source_.segment_open.un(boost::bind(&SegmentBuffer::on_event, this, _1, _2));
        }

        // Ŀǰֻ�����ڣ�seek��һ���ֶΣ���û�и÷ֶ�ͷ������ʱ��
        // ��ʱsizeΪhead_size_ͷ�����ݴ�С 
        // TO BE FIXED
        bool SegmentBuffer::seek(
            SegmentPosition const & base,
            SegmentPosition const & pos,
            boost::uint64_t size, 
            boost::system::error_code & ec)
        {
            if (!base.is_same_segment(base_)) {
                boost::system::error_code ec1;
                source_.seek(pos, size, ec);
                reset(base, pos);
                return !ec;
            }

            bool write_change = Buffer::seek(pos.byte_range.big_pos(), size);

            insert_segment(false, pos);
            find_segment(in_position(), read_);

            if (write_change || !write_.valid()) {
                find_segment(out_position(), write_);
                source_.seek(write_, write_hole_size(), ec);
                if (write_stream_)
                    write_stream_->seek(write_.byte_range.pos);
            }

            if (read_stream_) {
                read_stream_->seek(read_.byte_range.pos);
            }

            if (last_ec_ != boost::asio::error::would_block)
                last_ec_ = ec;

            return !ec;
        }

        bool SegmentBuffer::seek(
            SegmentPosition const & base,
            SegmentPosition const & pos, 
            boost::system::error_code & ec)
        {
            return seek(base, pos, invalid_size, ec);
        }

        bool SegmentBuffer::seek(
            SegmentPosition const & pos, 
            boost::uint64_t size, 
            boost::system::error_code & ec)
        {
            return seek(base_, pos, size, ec);
        }

        bool SegmentBuffer::seek(
            SegmentPosition const & pos, 
            boost::system::error_code & ec)
        {
            return seek(base_, pos, ec);
        }

        //************************************
        // Method:    prepare
        // FullName:  ppbox::data::SegmentBuffer::prepare
        // Access:    public 
        // Returns:   boost::system::error_code
        // Qualifier:
        // Parameter: boost::uint32_t amount ��Ҫ���ص����ݴ�С
        // Parameter: boost::system::error_code & ec
        //************************************
        size_t SegmentBuffer::prepare(
            size_t amount, 
            boost::system::error_code & ec)
        {
            if (check_hole(ec)) {
                if (ec == boost::asio::error::eof) {
                    boost::uint64_t hole_size = next_write_hole();
                    SegmentPosition pos;
                    find_segment(out_position(), pos);
                    source_.seek(pos, hole_size, ec);
                    if (hole_size == 0) {
                        ec = ppbox::data::error::at_end_point;
                    }
                }
                if (ec) {
                    last_ec_ = ec;
                    return 0;
                }
            }
            prepare_buffers_.clear();
            Buffer::prepare(amount, prepare_buffers_);
            amount = source_.read_some(prepare_buffers_, ec);
            commit(amount);
            last_ec_ = ec;
            return amount;
        }

        size_t SegmentBuffer::prepare_at_least(
            size_t amount, 
            boost::system::error_code & ec)
        {
            size_t n = 0;
            do {
                n += prepare_some(amount - n, ec);
            } while (!ec && amount > n);
            return n;
        }

        void SegmentBuffer::async_prepare(
            size_t amount, 
            prepare_response_type const & resp)
        {
            resp_ = resp;
            boost::system::error_code ec;
            if (check_hole(ec)) {
                if (ec == boost::asio::error::eof) {
                    boost::uint64_t hole_size = next_write_hole();
                    SegmentPosition pos;
                    find_segment(out_position(), pos);
                    source_.seek(pos, hole_size, ec);
                }
            }
            if (ec) {
                last_ec_ = ec;
                resp_(ec, 0);
                return;
            }
            prepare_buffers_.clear();
            Buffer::prepare(amount, prepare_buffers_);
            source_.async_read_some(prepare_buffers_, 
                boost::bind(&SegmentBuffer::handle_async, this, _1, _2));
        }

        void SegmentBuffer::handle_async(
            boost::system::error_code const & ec, 
            size_t bytes_transferred)
        {
            last_ec_ = ec;
            commit(bytes_transferred);
            pause_stream();
            prepare_response_type resp;
            resp.swap(resp_);
            resp(ec, 0);
        }

        MemoryLock * SegmentBuffer::fetch(
            boost::uint32_t track, 
            boost::uint64_t offset, 
            boost::uint32_t size, 
            bool merge, 
            std::deque<boost::asio::const_buffer> & data, 
            boost::system::error_code & ec)
        {
            if (!merge) {
                offset += read_.byte_range.big_offset;
            }
            assert(offset >= in_position() && (merge || offset + size <= read_.byte_range.big_end()));
            if (offset < in_position()) {
                ec = framework::system::logic_error::out_of_range;
            } else if (!merge && offset + size > read_.byte_range.big_end()) {
                ec = boost::asio::error::eof;
            } else {
                if (offset + size > out_position()) {
                    ec = last_ec_;
                    if (!ec) {
                        prepare_at_least((boost::uint32_t)(offset + size - out_position()), ec);
                    }
                }
                if (offset + size <= out_position()) {
                    ec.clear();
                    return Buffer::fetch(track, offset, size, data);
                }
            }
            return NULL;
        }

        MemoryLock * SegmentBuffer::fetch(
            boost::uint32_t track, 
            std::vector<DataBlock> & blocks, 
            bool merge, 
            std::deque<boost::asio::const_buffer> & data, 
            boost::system::error_code & ec)
        {
            if (!merge) {
                for (size_t i = 0; i < blocks.size(); ++i) {
                    blocks[i].offset += read_.byte_range.big_offset;
                }
            }
            boost::uint64_t offset = blocks.front().offset;
            boost::uint32_t size = (boost::uint32_t)(blocks.back().end() - offset);
            assert(offset >= in_position() && (merge || offset + size <= read_.byte_range.big_end()));
            if (offset < in_position()) {
                ec = framework::system::logic_error::out_of_range;
            } else if (!merge && offset + size > read_.byte_range.big_end()) {
                ec = boost::asio::error::eof;
            } else {
                if (offset + size > out_position()) {
                    ec = last_ec_;
                    if (!ec) {
                        prepare_at_least((boost::uint32_t)(offset + size - out_position()), ec);
                    }
                }
                if (offset + size <= out_position()) {
                    ec.clear();
                    return Buffer::fetch(track, blocks, data);
                }
            }
            return NULL;
        }

        void SegmentBuffer::putback(
            MemoryLock * mlock)
        {
            if (Buffer::putback(mlock)) {
                if (read_stream_)
                    read_stream_->update();
                if (read_ == write_ && write_stream_)
                    write_stream_->update();
            }
        }

        /**
        drop_all 
        ������ǰ�ֶε�����ʣ�����ݣ����Ҹ��µ�ǰ�ֶ���Ϣ
        */
        // TO BE FIXED
        bool SegmentBuffer::read_next(
            boost::uint64_t duration, 
            boost::system::error_code & ec)
        {
            return read_next(duration, read_.byte_range.end, ec);
        }

        /**
        drop_all 
        ������ǰ�ֶε�����ʣ�����ݣ����Ҹ��µ�ǰ�ֶ���Ϣ
        */
        // TO BE FIXED
        bool SegmentBuffer::read_next(
            boost::uint64_t duration, 
            boost::uint64_t min_offset, 
            boost::system::error_code & ec)
        {
            // TODO
            //if (read_.segment == write_.segment) {
            // source_.drop_all();
            //}
            if (try_drop_to(read_.byte_range.big_offset + min_offset)) {
                clear_segments();
                read_.time_range.end = duration;
                insert_segment(true, read_);
                find_segment(read_.byte_range.big_end(), read_);
                // ������DropAll
                if (read_stream_)
                    read_stream_->drop_all();
            } else {
                ec = framework::system::logic_error::out_of_range;
            }
            return !ec;
        }

        bool SegmentBuffer::write_next(
            boost::uint64_t duration, 
            boost::system::error_code & ec)
        {
            write_.time_range.end = duration;
            insert_segment(true, write_);
            find_segment(write_.byte_range.big_end(), write_);
            if (write_stream_)
                write_stream_->drop_all();
            ec.clear();
            return true;
        }

        void SegmentBuffer::pause_stream()
        {
            if (!last_ec_)
                last_ec_ = boost::asio::error::would_block;
        }

        void SegmentBuffer::resume_stream()
        {
            if (last_ec_ == boost::asio::error::would_block)
                last_ec_.clear();
        }

/*
        void SegmentBuffer::update_base(
            segment_t const & seg, 
            boost::uint32_t offset)
        {
            if (seg == base_)
                return;

            base_ = seg;
            
            read_.size_beg      -= offset;
            read_.shard_beg     -= offset;
            read_.size_beg      -= offset;
            read_.size_end      -= offset;

            write_.size_beg     -= offset;
            write_.shard_beg    -= offset;
            write_.size_end     -= offset;
            write_.shard_end    -= offset;
        }
*/
        bool SegmentBuffer::read_has_more() const
        {
            return read_ != source_.write_segment();
        }

        bool SegmentBuffer::write_has_more() const
        {
            return write_ != source_.write_segment();
        }

        void SegmentBuffer::attach_stream(
            SegmentStream & stream, 
            bool read)
        {
            assert ((read ? read_stream_ : write_stream_) == NULL);
            SegmentPosition & segment = read ? read_ : write_;
            if (segment.byte_range.big_pos() < in_position())
                segment.byte_range.pos = in_position() - segment.byte_range.big_offset;
            stream.change_to(segment);
            (read ? read_stream_ : write_stream_) = &stream;
        }

        void SegmentBuffer::detach_stream(
            SegmentStream & stream)
        {
            assert (&stream == read_stream_ || &stream == write_stream_);
            (&stream == read_stream_ ? read_stream_ : write_stream_) = NULL;
        }

        void SegmentBuffer::change_stream(
            SegmentStream & stream, 
            bool read)
        {
            detach_stream(stream);
            attach_stream(stream, read);
        }

        bool SegmentBuffer::segment_seek(
            SegmentPosition & segment, 
            bool merge, 
            boost::uint64_t pos)
        {
            assert(&segment == &read_);
            SegmentPosition read = read_;
            if (merge) {
                find_segment(pos, read);
            } else {
                read.byte_range.pos = pos;
            }
            boost::system::error_code ec;
            if (read.byte_range.big_pos() >= in_position() && read.byte_range.big_pos() <= out_position()) {
                // clear Buffer::seek_end_
                Buffer::seek(read.byte_range.big_pos());
                ec.clear();
                return true;
            }
            return seek(read, ec);
        }

        bool SegmentBuffer::segment_buffer(
            SegmentPosition & segment, 
            PositionType::Enum pos_type, 
            bool merge, 
            boost::uint64_t & pos, 
            boost::uint64_t & off, 
            boost::asio::const_buffer & buffer)
        {
            boost::uint64_t beg = merge ? 0 : segment.byte_range.big_beg();
            boost::uint64_t end = merge ? invalid_size : segment.byte_range.big_end();
            if (beg < in_position()) {
                beg = in_position();
            }
            if (end > out_position()) {
                end = out_position();
            }
            boost::system::error_code ec;
            if (pos_type == PositionType::beg) {
                pos += beg;
                if (pos > end) {
                    pos = end;
                    assert(false);
                    ec = framework::system::logic_error::out_of_range;
                }
            } else if (pos_type == PositionType::end) {
                pos = end - pos;
                if (pos < beg) {
                    pos = beg;
                    assert(false);
                    ec = framework::system::logic_error::out_of_range;
                }
            } else {
                if (!merge)
                    pos += segment.byte_range.big_offset;
                if (pos < beg) {
                   pos = beg;
                   //assert(false); // mp4 demuxer �п��ܶ�ָ��ͣ���ڿ�ǰλ��
                   ec = framework::system::logic_error::out_of_range;
                } else if (pos >= end) {
                    if (!merge && pos > segment.byte_range.big_end()) {
                        pos = segment.byte_range.big_end();
                        //assert(false);
                        ec = framework::system::logic_error::out_of_range;
                    }
                    if (pos >= out_position()) {
                        boost::system::error_code ec1 = last_ec_;
                        if (!ec1) {
                            prepare_at_least((size_t)(pos - out_position()), ec1); // ������buffer������
                        }
                        if (pos > out_position()) { // ���û�У�Ҳ����ʧ��
                            pos = out_position();
                            if (!ec) ec = ec1;
                        }
                        end = merge ? invalid_size : segment.byte_range.big_end();
                        if (end > out_position())
                            end = out_position();
                    }
                    assert(pos <= end);
                }
            }
            char const * ptr = read_buffer(beg, pos, end); // read_buffer��������beg����end
            if (merge) {
                find_segment(beg, segment);
            }
            off = pos - beg;
            pos = beg - segment.byte_range.big_offset;
            buffer = boost::asio::const_buffer(ptr, (size_t)(end - beg));
            return !ec;
        }

        void SegmentBuffer::on_event(
            util::event::Observable const & sender, 
            util::event::Event const & event)
        {
            assert(sender == source_);
            if (event == source_.segment_open) {
                insert_segment(false, source_.segment_open.segment);
                //find_segment(out_position(), write_);
            } else if (event == source_.segment_close) {
                insert_segment(false, source_.segment_close.segment);
                clear_segments();
            }
        }

        void SegmentBuffer::clear()
        {
            SegmentPosition seg = read_;
            seg.byte_range.pos = in_position() - seg.byte_range.big_offset;
            reset(base_, seg);
        }

        void SegmentBuffer::reset(
            SegmentPosition const & base, 
            SegmentPosition const & pos)
        {
            base_ = base;
            Buffer::reset(pos.byte_range.big_pos());
            segments_.clear();
            
            insert_segment(false, pos);

            read_ = pos;
            write_ = pos;

            if (read_stream_)
                read_stream_->seek(pos.byte_range.pos);
            if (write_stream_)
                write_stream_->seek(pos.byte_range.pos);
        }

        void SegmentBuffer::clear_segments()
        {
            while (!segments_.empty() && segments_.front().byte_range.big_end() <= data_begin()) {
                segments_.pop_front();
            }
            while (!segments_.empty() && segments_.back().byte_range.big_beg() > data_end()) {
                segments_.pop_back();
            }
        }

        struct comp_big_beg
        {
            bool operator()(
                SegmentPosition const & l, 
                SegmentPosition const & r)
            {
                return l.byte_range.big_beg() < r.byte_range.big_beg() 
                    || (l.byte_range.big_beg() == r.byte_range.big_beg() 
                        && l.index < r.index);
            }
        };

        void SegmentBuffer::insert_segment(
            bool is_read, 
            SegmentPosition const & seg1)
        {
            SegmentPosition seg = seg1;
            std::deque<SegmentPosition>::iterator iter = 
                std::lower_bound(segments_.begin(), segments_.end(), seg, comp_big_beg());
            if (iter != segments_.end() && !comp_big_beg()(seg, *iter)) { // ���
                SegmentPosition & segment = *iter;
                if (!is_read && (segment.byte_range.end == invalid_size 
                    || (segment.byte_range.end > seg.byte_range.end && segment.byte_range.big_end() == (++iter)->byte_range.big_beg()))) {
                    segment.byte_range = seg.byte_range;
                    if (read_ == seg) {
                        read_.byte_range.end = seg.byte_range.end;
                    }
                    if (write_ == seg) {
                        write_.byte_range.end = seg.byte_range.end;
                    }
                } else if (is_read && segment.time_range.end == invalid_size) {
                    segment.time_range = seg.time_range;
                    seg.byte_range = segment.byte_range;
                    if (read_ == seg) {
                        read_.time_range.end = seg.time_range.end;
                    }
                    if (write_ == seg) {
                        write_.time_range.end = seg.time_range.end;
                    }
                    ppbox::data::SegmentRange & byte_range = seg.byte_range; // ����һ��
                    ppbox::data::SegmentRange & time_range = seg.time_range; // ����һ��
                    while (++iter != segments_.end()) {
                        SegmentPosition & segment = *iter;
                        if (!byte_range.followed_by(segment.byte_range))
                            break;
                        segment.time_range.follow(time_range);
                        if (write_ == segment) {
                            write_.time_range.big_offset = segment.time_range.big_offset;
                        }
                        byte_range = segment.time_range;
                        time_range = segment.time_range;
                        if (segment.time_range.end == invalid_size)
                            break;
                    }
                }
            } else {
                if (seg.byte_range.big_end() == invalid_size && iter != segments_.end()) {
                    // �ϻص�ǰ��ͬһ�㣬�Ѿ����ص����ݻ�����Ч������ǰ��ֶ���Ϣ��ʧ��
                    seg.byte_range.end = iter->byte_range.big_beg() - seg.byte_range.big_offset;
                }
                assert(iter == segments_.end() || seg.byte_range.big_end() <= iter->byte_range.big_beg());
                if (iter != segments_.begin()) {
                    SegmentPosition & segment = *--iter;
                    if (segment.byte_range.followed_by(seg.byte_range)) {
                        seg.time_range.follow(segment.time_range);
                    }
                    ++iter;
                }
                segments_.insert(iter, seg);
            }
        }

        struct comp_big_end
        {
            bool operator()(
                SegmentPosition const & l, 
                SegmentPosition const & r)
            {
                return l.byte_range.big_end() < r.byte_range.big_end();
            }
        };

        void SegmentBuffer::find_segment(
            boost::uint64_t offset, 
            SegmentPosition & seg)
        {
            seg.byte_range.big_offset = offset;
            seg.byte_range.beg = 0;
            seg.byte_range.end = 0;
            std::deque<SegmentPosition>::iterator iter = 
                std::upper_bound(segments_.begin(), segments_.end(), seg, comp_big_end());
            if (iter == segments_.end()) { // �п��ܲ����ڡ�����
                iter = std::lower_bound(segments_.begin(), segments_.end(), seg, comp_big_end());
            }
            assert (iter != segments_.end() && offset >= iter->byte_range.big_beg());
            if (iter != segments_.end() && offset >= iter->byte_range.big_beg()) {
                seg = *iter;
                seg.byte_range.pos = offset - seg.byte_range.big_offset;
            }
        }

    } // namespace data
} // namespace ppbox
