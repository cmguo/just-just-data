// SourceStrategy.cpp

#include "ppbox/data/Common.h"
#include <ppbox/data/strategy/SourceStrategy.h>

using namespace boost::system;

namespace ppbox
{
    namespace data
    {
        std::map< std::string, SourceStrategy::register_type > & SourceStrategy::strategy_map()
        {
            static std::map< std::string, SourceStrategy::register_type > get_map;
            return get_map;
        }

        void SourceStrategy::regist_strategy(
            std::string const & type, 
            SourceStrategy::register_type func)
        {
            strategy_map().insert(std::make_pair(type, func));
        }

        SourceStrategy * SourceStrategy::create(
            std::string const & type, 
            std::vector<SegmentInfoEx> const & segments,
            MediaInfo const & video_info)
        {
            std::map<std::string, SourceStrategy::register_type >::iterator iter = 
                strategy_map().find(type);
            if (strategy_map().end() == iter) {
                return NULL;
            }
            return iter->second(segments, video_info);
        }

        void SourceStrategy::destory(SourceStrategy* & strategy)
        {
            delete strategy;
            strategy = NULL;
        }

        SourceStrategy::SourceStrategy(
            std::vector<SegmentInfoEx> const & segments,
            MediaInfo const & video_info)
            : segments_(segments)
            , video_info_(video_info)
            , pos_(0)
        {
        }

        SourceStrategy::~SourceStrategy()
        {
        }

        bool SourceStrategy::next_segment(
            bool is_next,
            SegmentInfoEx & info)
        {
            assert(!segments_.empty());
            bool res = false;
            is_next ? pos_++ : pos_;
            if (pos_ < segments_.size()) {
                info = segments_[pos_];
                info.try_times = 0;
                info.begin = 0;
                info.end = boost::uint64_t(-1);
                info.position = 0;
                res = true;
            } else {
                pos_--;
                res = false;
            }
            return res;
        }

        error_code SourceStrategy::on_seek(
            size_t offset,
            SegmentInfoEx & info, 
            error_code & ec)
        {
            bool find = false;
            for (boost::uint32_t i = 0; i < segments_.size(); ++i) {
                if (offset <= segments_[i].size) {
                    info = segments_[i];
                    info.begin = offset;
                    info.end = info.size;
                    info.size = info.end - info.begin;
                    find = true;
                    pos_ = i;
                    break;
                } else {
                    offset -= segments_[i].size;
                }
            }
            if (!find) {
                ec = framework::system::logic_error::out_of_range;
            }
            return ec;
        }

        error_code SourceStrategy::on_seek(
            boost::uint32_t segment_index,
            size_t offset, 
            SegmentInfoEx & info, 
            error_code & ec)
        {
            if (segment_index < segments_.size()) {
                info = segments_[segment_index];
                if (offset > info.size) {
                    ec = framework::system::logic_error::out_of_range;
                } else {
                    pos_ = segment_index;
                    info.begin = offset;
                    info.end = info.size;
                    info.size = info.end - info.begin;
                    info.position = offset;
                }
            } else {
                ec = framework::system::logic_error::out_of_range;
            }
            return ec;
        }

        std::size_t SourceStrategy::size(void)
        {
            std::size_t length = 0;
            for (boost::uint32_t i = 0; i < segments_.size(); ++i) {
                length += segments_[i].size;
            }
            return length;
        }

    }
}
