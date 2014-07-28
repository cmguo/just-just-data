// StreamStatistic.h

#ifndef _PPBOX_DATA_BASE_DATA_STATISTIC_H_
#define _PPBOX_DATA_BASE_DATA_STATISTIC_H_

#include "ppbox/data/base/DataBase.h"
#include "ppbox/data/base/StreamEvent.h"

#include <ppbox/data/base/StreamStatus.h>

#include <framework/timer/TimeCounter.h>

#include <util/event/Observable.h>

namespace framework { namespace timer { class Ticker; } }

namespace ppbox
{
    namespace data
    {

        class StreamStatistic
            : public StreamStatus
            , public util::event::Observable
            , public framework::timer::TimeCounter
        {
        public:
            struct StatusInfo
            {
                StatusInfo()
                    : status(closed)
                    , blocked(false)
                    , play_position((boost::uint64_t)-1)
                    , elapse((boost::uint64_t)-1)
                {
                }

                StatusInfo(
                    StatusEnum status, 
                    bool blocked, 
                    boost::uint64_t play_position)
                    : status(status)
                    , blocked(blocked)
                    , play_position(play_position)
                    , elapse(0)
                {
                }

                StatusEnum status;
                bool blocked;
                boost::uint64_t play_position;
                boost::uint64_t elapse;
            };

        public:
            StreamEvent status_changed;

            // 定期发出的缓存状态
            StreamEvent buffer_update;

        protected:
            StreamStatistic();

        protected:
            void open_beg_media();

            void open_beg_stream();

            void open_end();

            void pause();

            void resume();

            void play_on(
                boost::uint64_t sample_time);

            void block_on();

            void seek(
                bool ok, 
                boost::uint64_t seek_time);

            void last_error(
                boost::system::error_code const & ec);

            void close();

        public:
            boost::uint64_t play_position() const
            {
                return play_position_;
            }

            boost::uint64_t seek_position() const
            {
                return seek_position_;
            }

            std::vector<StatusInfo> const & status_info() const
            {
                return status_infos_;
            }

            boost::system::error_code last_error() const
            {
                return last_error_;
            }

        private:
            void change_status(
                StatusEnum status, 
                bool blocked = false);

            void update_stat();

            virtual void update_stat(
                boost::system::error_code & ec) = 0;

        private:
            framework::timer::Ticker * ticker_;

        protected:
            boost::uint32_t status_history_;
            bool need_seek_time_;
            boost::uint64_t seek_position_;        // 拖动后的位置（时间毫秒）
            boost::uint64_t play_position_;        // 播放的位置（时间毫秒）
            boost::uint64_t last_time_;
            std::vector<StatusInfo> status_infos_;
            boost::system::error_code last_error_;
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_BASE_DATA_STATISTIC_H_
