// StreamStatistic.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/StreamStatistic.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/Section.h>
#include <framework/timer/Ticker.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.data.StreamStatistic", framework::logger::Debug);

namespace ppbox
{
    namespace data
    {

        StreamStatistic::StreamStatistic()
            : status_changed(*this)
            , buffer_update(*this)
            , status_history_(0)
            , need_seek_time_(false)
            , seek_position_(0)
            , play_position_(0)
            , last_time_(0)
        {
            ticker_ = new framework::timer::Ticker(1000,true);
            status_infos_.push_back(StatusInfo(closed, false, play_position_));
        }

        void StreamStatistic::update_stat()
        {
            boost::system::error_code ec;
            update_stat(ec);

            if (blocked()) {
                LOG_DEBUG("[buf_time] buf_time: " << buf_time() << " ms");
            }

            raise(buffer_update);
        }
        /*
        static char const * type_str[] = {
            "closed", 
            "opening", 
            "opened", 
            "paused",
            "playing", 
            "buffering"
        };
        */
        void StreamStatistic::change_status(
            StatusEnum status, 
            bool blocked)
        {
            boost::uint64_t now_time = TimeCounter::elapse();
            boost::uint64_t elapse = now_time - last_time_;
            last_time_ = now_time;
            // 两个Buffering状态，原因相同，并且中间间隔一个playing状态，并且持续play时间（sample时间）小于3秒
            // 那么合并Buffering状态，并且如果接下来是playing状态，也合并
            boost::uint32_t const block_play_block = ((playing | 0x80) << 16) | (playing << 8) | (playing | 0x80);
            if ((status_history_ & 0xffffff) == block_play_block
                && (play_position_ - status_infos_[status_infos_.size() - 2].play_position < 3000)) {
                    // 合并Buffering状态，此后剩下一个Buffering状态加一个playing状态
                    status_infos_[status_infos_.size() - 3].elapse += elapse;
                    status_infos_.pop_back();
                    // 如果接下来是playing状态，也合并
                    if (status == playing) {
                        last_time_ -= status_infos_.back().elapse;
                        status_infos_.pop_back();
                    }
            } else {
                // 合并时间
                status_infos_.back().elapse = elapse;
            }
            StatusInfo info(status, blocked, play_position_);
            status_infos_.push_back(info);

            bool changed = this->status() != status;
            status_ex = blocked ? (status | 0x80) : status;
            status_history_ = (status_history_ << 8) | status_ex;

            if (changed)
                raise(status_changed);
        }

        void StreamStatistic::open_beg_media()
        {
            assert(status() == closed);
            change_status(media_opening, true);
        }

        void StreamStatistic::open_beg_stream()
        {
            assert(status() == media_opening);
            change_status(stream_opening, true);
        }

        void StreamStatistic::open_end()
        {
            assert(status() <= stream_opening);
            change_status(opened);
            play_position_ = 0;
        }

        void StreamStatistic::pause()
        {
            if (status_ex == paused)
                return;

            change_status(paused);
        }

        void StreamStatistic::resume()
        {
            if (status_ex != paused)
                return;

            change_status(playing);
        }

        void StreamStatistic::play_on(
            boost::uint64_t sample_time)
        {
            if (ticker_->check()) {
                update_stat();
            }

            play_position_ = sample_time;

            if (status_ex == playing)
                return;

            if (need_seek_time_) {
                seek_position_ = play_position_;
                need_seek_time_ = false;
            }

            change_status(playing);
        }

        void StreamStatistic::block_on()
        {
            if (ticker_->check()) {
                update_stat();
            }

            if (blocked())
                return;

            change_status(status(), true);
        }

        void StreamStatistic::seek(
            bool ok, 
            boost::uint64_t seek_time)
        {
            // 如果拖动的时间与当前播放的时间相同，则不处理
            if (play_position_ == seek_time)
                return;

            if (ok) {
                change_status(seeking, false);
                seek_position_ = seek_time;
                need_seek_time_ = false;
                play_on(seek_time);
            } else {
                change_status(seeking, true);
                seek_position_ = seek_time;
                need_seek_time_ = true;
                play_position_ = seek_time;
            }

            update_stat();
        }

        void StreamStatistic::last_error(
            boost::system::error_code const & ec)
        {
            if (ec == boost::asio::error::would_block) {
                block_on();
                return;
            }
            last_error_ = ec;
        }

        void StreamStatistic::close()
        {
            change_status(closed);
            status_infos_.back().elapse = 0;
        }

    }
}
