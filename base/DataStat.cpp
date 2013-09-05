// DataStat.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/base/DataStat.h"

#include <framework/timer/Ticker.h>
#include <framework/container/SafeCycle.h>

namespace ppbox
{
    namespace data
    {

        DataStat::DataStat()
            : start_time(time(NULL))
            , total_bytes(0)
            , connection_status(ConnectionStatus::closed)
            , num_try(0)
            , zero_time(0)
        {
            speeds[0].interval = ONE_SECOND;
            speeds[1].interval = FIVE_SECONDS;
            speeds[2].interval = TWENTY_SECONDS;
            speeds[3].interval = SIXTY_SECONDS;
        }

        struct IntervalRecord
        {
            IntervalRecord(
                boost::uint64_t milli_sec = 0, 
                boost::uint64_t bytes = 0)
                : milli_sec(milli_sec)
                , bytes(bytes)
            {
            }
            boost::uint64_t milli_sec;     // 上一次的采样的时间
            boost::uint64_t bytes;
        };

        struct DataWatcher::Impl
        {
            Impl()
                : ticker(1000)
                , cycle(60)
            {
                IntervalRecord r;
                while (!cycle.full())
                    cycle.push(r);
            }
            framework::timer::Ticker ticker;
            framework::container::SafeCycle<IntervalRecord> cycle;
        };

        DataWatcher::DataWatcher()
            : stat_update(*this)
        {
            impl_ = new Impl();
        }

        DataWatcher::~DataWatcher()
        {
            delete impl_;
            impl_ = NULL;
        }

        void DataWatcher::on_seek()
        {
            DataStat::num_try = 0;
        }

        void DataWatcher::on_next()
        {
            DataStat::num_try = 0;
        }

        void DataWatcher::on_open()
        {
            connection_status = ConnectionStatus::opening;
            ++DataStat::num_try;
        }

        void DataWatcher::on_opened()
        {
            connection_status = ConnectionStatus::transfering;
        }

        void DataWatcher::on_close()
        {
            connection_status = ConnectionStatus::closed;
        }

        boost::uint32_t DataWatcher::get_zero_interval()
        {
            if (zero_time == 0) {
                return 0;
            } else {
                return (boost::uint32_t)((impl_->ticker.elapse() - zero_time) / 1000);
            }
        }

        void DataWatcher::reset_zero_interval()
        {
            zero_time = 0;
        }

        void DataWatcher::increase_bytes(
            boost::uint32_t byte_size)
        {
            total_bytes += byte_size; //记录总下载字节数
            boost::uint64_t milli_sec = 0;
            if (impl_->ticker.check(milli_sec)) {
                if ((boost::uint32_t)total_bytes == impl_->cycle.back().bytes) {
                    if (zero_time == 0)
                        zero_time = milli_sec;
                } else {
                    zero_time = 0;
                }
                for (size_t i = 0; i < sizeof(speeds) / sizeof(SpeedStat); i++) {
                    IntervalRecord const & r2 = impl_->cycle[60 - speeds[i].interval];
                    if (milli_sec != r2.milli_sec) {
                        speeds[i].cur_speed = 
                            (boost::uint32_t)((total_bytes - r2.bytes) * 1000 / (milli_sec - r2.milli_sec));
                        if (speeds[i].cur_speed > speeds[i].peak_speed) {
                            speeds[i].peak_speed = speeds[i].cur_speed;
                        }
                    }
                }
                IntervalRecord r(milli_sec, total_bytes);
                impl_->cycle.pop();
                impl_->cycle.push(r);

                raise(stat_update);
            }
        }
     
    } // namespace data
} // namespace ppbox
