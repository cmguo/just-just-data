// DataStat.h

#ifndef _JUST_DATA_BASE_DATA_STAT_H_
#define _JUST_DATA_BASE_DATA_STAT_H_

#include "just/data/base/DataEvent.h"

#include <util/event/Observable.h>

namespace just
{
    namespace data
    {

        struct SpeedStat
        {
            SpeedStat()
                : interval(0)
                , cur_speed(0)
                , peak_speed(0)
            { 
            }

            boost::uint32_t interval;           // 采样集的类型（秒为单位）
            boost::uint32_t cur_speed;
            boost::uint32_t peak_speed;
        };

        struct ConnectionStatus
        {
            enum Enum
            {
                closed = 0, 
                opening, 
                transfering, 
            };
        };

        struct DataStat
        {
            DataStat();

            //定义采样集中需要采样的时间
            static const boost::uint32_t ONE_SECOND = 1;
            static const boost::uint32_t FIVE_SECONDS = 5;
            static const boost::uint32_t TWENTY_SECONDS = 20;
            static const boost::uint32_t SIXTY_SECONDS = 60;

            time_t start_time;
            boost::uint64_t total_bytes;        // 记录下载总字节
            boost::uint32_t connection_status;
            boost::uint32_t num_try;            // 当前分段尝试次数
            boost::uint64_t zero_time;
            SpeedStat speeds[4];          // 定义4个采样数据统计集
        };

        class DataWatcher
            : public DataStat
            , public util::event::Observable
        {
        public: 
            DataWatcher();

            ~DataWatcher();

        public:
            DataStatEvent stat_update;

        public:
            boost::uint32_t num_try() const
            {
                return DataStat::num_try;
            }

            boost::uint32_t get_zero_interval();

            void reset_zero_interval();

            //每次下载数据所调用的统计接口
            void increase_bytes(
                boost::uint32_t byte_size);

        protected:
            void on_seek();

            void on_next();

            void on_open();

            void on_opened();

            void on_close();

        private:
            struct Impl;
            Impl * impl_;
        };

    } // namespace data
} // namespace just

#endif // _JUST_DATA_BASE_DATA_STAT_H_
