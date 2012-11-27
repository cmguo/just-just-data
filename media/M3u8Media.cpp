
// M3u8Media.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/media/M3u8Media.h"
#include "ppbox/data/base/SourceBase.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>
#include <boost/asio/read.hpp>

#include <sstream>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.M3u8Media", framework::logger::Debug);

namespace ppbox
{
    namespace data
    {

        M3u8Media::M3u8Media(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
            : SegmentMedia(io_svc, url)
            , source_(NULL)
            , sequence_(0)
        {
            source_ = SourceBase::create(io_svc, url.protocol());
            info_.flags = MediaBasicInfo::f_segment;
            info_.flags |= MediaBasicInfo::f_time_smoth;
            info_.format = "ts";
        }

        M3u8Media::~M3u8Media()
        {
            if (source_) {
                SourceBase::destroy(source_);
            }
        }

        void M3u8Media::async_open(
            response_type const & resp)
        {
            source_->async_open(url_, 0, invalid_size, 
                boost::bind(&M3u8Media::handle_open, this, resp, _1));
        }

        void M3u8Media::cancel(
            boost::system::error_code & ec)
        {
            source_->cancel(ec);
        }

        void M3u8Media::close(
            boost::system::error_code & ec)
        {
            source_->close(ec);
        }

        bool M3u8Media::get_basic_info(
            MediaBasicInfo & info,
            boost::system::error_code & ec) const
        {
            info = info_;
            ec.clear();
            return true;
        }

        bool M3u8Media::get_info(
            MediaInfo & info,
            boost::system::error_code & ec) const
        {
            info = info_;
            ec.clear();
            return true;
        }

        size_t M3u8Media::segment_count() const
        {
            return info_.type == MediaInfo::vod ? segments_.size() : size_t(-1);
        }

        std::string const M3u8Media::segment_protocol() const
        {
            framework::string::Url url;
            if (!segment_urls_.empty())
                url.from_string(segment_urls_[0]);
            return url.protocol();
        }

        bool M3u8Media::segment_url(
            size_t segment, 
            framework::string::Url & url,
            boost::system::error_code & ec) const
        {
            if (segment >= sequence_ && segment - sequence_ < segments_.size()) {
                url.from_string(segment_urls_[segment - sequence_]);
                ec.clear();
                return true;
            }
            ec = framework::system::logic_error::out_of_range;
            return false;
        }

        void M3u8Media::segment_info(
            size_t segment, 
            SegmentInfo & info) const
        {
            if (segment >= sequence_ && segment - sequence_ < segments_.size()) {
                info = segments_[segment - sequence_];
            }
        }

        void M3u8Media::handle_open(
            response_type const & resp, 
            boost::system::error_code ec)
        {
            if (ec) {
                handle_read(resp, ec);
                return;
            }
            boost::uint64_t total = source_->total(ec);
            if (ec) {
                boost::asio::async_read(*source_, buf_, 
                    boost::bind(&M3u8Media::handle_read, this, resp, _1));
            } else {
                boost::asio::async_read(*source_, buf_, boost::asio::transfer_at_least(total), 
                    boost::bind(&M3u8Media::handle_read, this, resp, _1));
            }
        }

        void M3u8Media::handle_read(
            response_type const & resp, 
            boost::system::error_code ec)
        {
            if (ec == boost::asio::error::eof) {
                ec.clear();
            }
            if (!ec) {
                parse_m3u8(ec);
            }
            resp(ec);
        }

        void M3u8Media::parse_m3u8(
            boost::system::error_code & ec)
        {
            char const * const M3U8_BEGIN = "#EXTM3U";
            char const * const M3U8_TARGETDURATION = "#EXT-X-TARGETDURATION";
            char const * const M3U8_SEQUENCE = "#EXT-X-MEDIA-SEQUENCE";
            char const * const M3U8_EXTINF = "#EXTINF";
            char const * const M3U8_END  = "#EXT-X-ENDLIST";

            std::string line;
            std::istream is(&buf_);
            bool begin = false;
            bool end = false;
            size_t line_num = 0;
            boost::uint64_t duration = 0;
            while (std::getline(is, line)) {
                ++line_num;
                std::istringstream iss(line);
                std::string token;
                std::getline(iss, token, ':');
                if (token == M3U8_BEGIN) {
                    begin = true;
                } else if (!begin) {
                    LOG_WARN("[parse_m3u8] line before begin tag: " << line); 
                } else if (end) {
                    LOG_WARN("[parse_m3u8] line before end tag: " << line); 
                } else if (token == M3U8_END) {
                    end = true;
                } else if (token == M3U8_TARGETDURATION) {
                    
                } else if (token == M3U8_SEQUENCE) {
                    if (iss >> sequence_)
                        --sequence_;
                } else if (token == M3U8_EXTINF) {
                    SegmentInfo info;
                    if (iss >> info.duration)
                        info.duration *= 1000;
                    duration += info.duration;
                    if (!std::getline(is, line)) {
                        LOG_WARN("[parse_m3u8] no url at line: " << line_num); 
                    }
                    segments_.push_back(info);
                    segment_urls_.push_back(line);
                }
            }
            info_.duration = duration;
            if (!end) {
                info_.type = MediaInfo::live;
                info_.current = duration;
                info_.start_time = time(NULL) - duration / 1000;
                boost::uint32_t delay = 0;
                for (size_t i = segments_.size() - 1; i != size_t(-1) && i > segments_.size() - 4; --i) {
                    delay += (boost::uint32_t)segments_[i].duration;
                }
                info_.delay = delay;
            }
        }

    } // data
} // ppbox
