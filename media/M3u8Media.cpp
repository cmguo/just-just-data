
// M3u8Media.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/media/M3u8Media.h"
#include "ppbox/data/base/SourceBase.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

#include <boost/bind.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <sstream>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.M3u8Media", framework::logger::Debug);

namespace ppbox
{
    namespace data
    {

        class M3u8Media::Impl
            : public boost::enable_shared_from_this<M3u8Media::Impl>
        {
        public:
            Impl(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

            ~Impl();

        public:
            void async_open(
                MediaBase::response_type const & resp);

            void cancel(
                boost::system::error_code & ec);

            void close(
                boost::system::error_code & ec);

        public:
            bool get_basic_info(
                MediaBasicInfo & info,
                boost::system::error_code & ec) const;

            bool get_info(
                MediaInfo & info,
                boost::system::error_code & ec) const;

        public:
            size_t segment_count() const;

            std::string const segment_protocol() const;

            bool segment_url(
                size_t segment, 
                framework::string::Url & url,
                boost::system::error_code & ec) const;

            void segment_info(
                size_t segment, 
                SegmentInfo & info) const;

        private:
            void handle_open(
                boost::system::error_code ec);

            void handle_read(
                boost::system::error_code ec);

            void handle_timer(
                boost::system::error_code ec);

            void parse_m3u8(
                boost::system::error_code & ec);

        private:
            framework::string::Url url_;
            MediaInfo info_;
            SourceBase * source_;
            boost::asio::deadline_timer timer_;
            boost::asio::streambuf buf_;
            size_t seq_start_;
            size_t seq_lastest_;
            boost::uint64_t duration_;
            std::vector<SegmentInfo> segments_;
            std::vector<std::string> segment_urls_;
            MediaBase::response_type resp_;
        };

        M3u8Media::M3u8Media(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
            : SegmentMedia(io_svc, url)
            , impl_(new Impl(io_svc, url))
        {
        }

        M3u8Media::~M3u8Media()
        {
        }

        void M3u8Media::async_open(
            response_type const & resp)
        {
            impl_->async_open(resp);
        }

        void M3u8Media::cancel(
            boost::system::error_code & ec)
        {
            impl_->cancel(ec);
        }

        void M3u8Media::close(
            boost::system::error_code & ec)
        {
            impl_->close(ec);
            impl_.reset();
        }

        bool M3u8Media::get_basic_info(
            MediaBasicInfo & info,
            boost::system::error_code & ec) const
        {
            return impl_->get_basic_info(info, ec);
        }

        bool M3u8Media::get_info(
            MediaInfo & info,
            boost::system::error_code & ec) const
        {
            return impl_->get_info(info, ec);
        }

        size_t M3u8Media::segment_count() const
        {
            return impl_->segment_count();
        }

        std::string const M3u8Media::segment_protocol() const
        {
            return impl_->segment_protocol();
        }

        bool M3u8Media::segment_url(
            size_t segment, 
            framework::string::Url & url,
            boost::system::error_code & ec) const
        {
            return impl_->segment_url(segment, url, ec);
        }

        void M3u8Media::segment_info(
            size_t segment, 
            SegmentInfo & info) const
        {
            return impl_->segment_info(segment, info);
        }

        M3u8Media::Impl::Impl(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
            : url_(url)
            , source_(NULL)
            , timer_(io_svc)
            , seq_start_(0)
            , seq_lastest_(0)
        {
            source_ = SourceBase::create(io_svc, url.protocol());
            info_.flags = MediaBasicInfo::f_segment;
            info_.flags |= MediaBasicInfo::f_time_smoth;
            info_.format = "ts";
        }

        M3u8Media::Impl::~Impl()
        {
            if (source_) {
                delete source_;
            }
        }

        void M3u8Media::Impl::async_open(
            response_type const & resp)
        {
            resp_ = resp;
            source_->async_open(url_, 0, invalid_size, 
                boost::bind(&Impl::handle_open, shared_from_this(), _1));
        }

        void M3u8Media::Impl::cancel(
            boost::system::error_code & ec)
        {
            source_->cancel(ec);
        }

        void M3u8Media::Impl::close(
            boost::system::error_code & ec)
        {
            timer_.cancel(ec);
            source_->cancel(ec);
        }

        bool M3u8Media::Impl::get_basic_info(
            MediaBasicInfo & info,
            boost::system::error_code & ec) const
        {
            info = info_;
            ec.clear();
            return true;
        }

        bool M3u8Media::Impl::get_info(
            MediaInfo & info,
            boost::system::error_code & ec) const
        {
            info = info_;
            ec.clear();
            return true;
        }

        size_t M3u8Media::Impl::segment_count() const
        {
            return info_.type == MediaInfo::vod ? segments_.size() : size_t(-1);
        }

        std::string const M3u8Media::Impl::segment_protocol() const
        {
            framework::string::Url url;
            if (!segment_urls_.empty())
                url.from_string(segment_urls_[0]);
            return url.protocol();
        }

        bool M3u8Media::Impl::segment_url(
            size_t segment, 
            framework::string::Url & url,
            boost::system::error_code & ec) const
        {
            if (segment >= seq_lastest_ && segment - seq_lastest_ < segments_.size()) {
                std::string const & str = segment_urls_[segment - seq_lastest_];
                if (str[0] == '.') {
                    url.from_string(url_.protocol() + "://" + url_.host_svc() + str.substr(1));
                } else if (str[0] == '/') {
                    url.from_string(url_.protocol() + "://" + url_.host_svc() + str);
                } else {
                    url.from_string(str);
                }
                ec.clear();
                return true;
            }
            ec = framework::system::logic_error::out_of_range;
            return false;
        }

        void M3u8Media::Impl::segment_info(
            size_t segment, 
            SegmentInfo & info) const
        {
            if (segment >= seq_lastest_ && segment - seq_lastest_ < segments_.size()) {
                info = segments_[segment - seq_lastest_];
            }
        }

        void M3u8Media::Impl::handle_open(
            boost::system::error_code ec)
        {
            if (ec) {
                handle_read(ec);
                return;
            }
            boost::uint64_t total = source_->total(ec);
            if (ec) {
                boost::asio::async_read(*source_, buf_, 
                    boost::bind(&Impl::handle_read, shared_from_this(), _1));
            } else {
                boost::asio::async_read(*source_, buf_, boost::asio::transfer_at_least(total), 
                    boost::bind(&Impl::handle_read, shared_from_this(), _1));
            }
        }

        void M3u8Media::Impl::handle_read(
            boost::system::error_code ec)
        {
            if (ec == boost::asio::error::eof) {
                ec.clear();
            }
            boost::system::error_code ec1;
            source_->close(ec1);
            if (!ec) {
                parse_m3u8(ec);
            }
            if (!resp_.empty()) {
                MediaBase::response_type resp;
                resp.swap(resp_);
                resp(ec);
            }
            if (info_.type ==  MediaBasicInfo::live && shared_from_this().use_count() > 3) {
                timer_.expires_from_now(
                    boost::posix_time::seconds(duration_));
                timer_.async_wait(
                    boost::bind(&Impl::handle_timer, shared_from_this(), _1));
            }
        }

        void M3u8Media::Impl::handle_timer(
            boost::system::error_code ec)
        {
            if (!ec && shared_from_this().use_count() > 3) {
                source_->async_open(url_, 0, invalid_size, 
                    boost::bind(&Impl::handle_open, shared_from_this(), _1));
            }
        }

        void M3u8Media::Impl::parse_m3u8(
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
                    iss >> duration_;
                } else if (token == M3U8_SEQUENCE) {
                    if (iss >> seq_lastest_) {
                        if (seq_start_ == 0) {
                            seq_start_ = seq_lastest_;
                        }
                        seq_lastest_ -= seq_start_;
                    }
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
            if (end) {
                info_.duration = duration;
            } else {
                info_.type = MediaInfo::live;
                info_.current = duration;
                info_.start_time = time(NULL) - duration / 1000;
                boost::uint32_t delay = 0;
                for (size_t i = segments_.size() - 1; i != size_t(-1) && i + 4 > segments_.size(); --i) {
                    delay += (boost::uint32_t)segments_[i].duration;
                }
                info_.delay = delay;
                info_.shift = duration;
            }
        }

    } // data
} // ppbox
