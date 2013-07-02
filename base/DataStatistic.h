// DataStatistic.h

#ifndef _PPBOX_DATA_BASE_DATA_STATISTIC_H_
#define _PPBOX_DATA_BASE_DATA_STATISTIC_H_

namespace ppbox
{
    namespace data
    {

        struct SpeedStatistics
        {
            SpeedStatistics()
                : interval(0)
                , cur_speed(0)
                , peak_speed(0)
            { 
            }

            boost::uint32_t interval;           // 采样集的类型（秒为单位）
            boost::uint32_t cur_speed;
            boost::uint32_t peak_speed;
        };

        struct DataConnectionStatus
        {
            enum Enum
            {
                closed = 0, 
                opening, 
                receiving, 
            };
        };

        struct DataStatistic
        {
            DataStatistic();

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
            SpeedStatistics speeds[4];          // 定义4个采样数据统计集
        };

        class DataObserver
            : public DataStatistic
        {
        public: 
            DataObserver();

            ~DataObserver();

        public: 
            void on_next();

            void on_open();

            void on_opened();

            void on_close();

            boost::uint32_t num_try() const
            {
                return DataStatistic::num_try;
            }

            boost::uint32_t get_zero_interval();

            void reset_zero_interval();

            //每次下载数据所调用的统计接口
            void increase_bytes(
                boost::uint32_t byte_size);

        private:
            struct Impl;
            Impl * impl_;
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_BASE_DATA_STATISTIC_H_
