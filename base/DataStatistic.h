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

            boost::uint32_t interval;           // �����������ͣ���Ϊ��λ��
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

            //�������������Ҫ������ʱ��
            static const boost::uint32_t ONE_SECOND = 1;
            static const boost::uint32_t FIVE_SECONDS = 5;
            static const boost::uint32_t TWENTY_SECONDS = 20;
            static const boost::uint32_t SIXTY_SECONDS = 60;

            time_t start_time;
            boost::uint64_t total_bytes;        // ��¼�������ֽ�
            boost::uint32_t connection_status;
            boost::uint32_t num_try;            // ��ǰ�ֶγ��Դ���
            boost::uint64_t zero_time;
            SpeedStatistics speeds[4];          // ����4����������ͳ�Ƽ�
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

            //ÿ���������������õ�ͳ�ƽӿ�
            void increase_bytes(
                boost::uint32_t byte_size);

        private:
            struct Impl;
            Impl * impl_;
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_BASE_DATA_STATISTIC_H_
