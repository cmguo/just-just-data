// ListStrategy.h

#ifndef _JUST_DATA_STRATEGY_LIST_STRATEGY_H_
#define _JUST_DATA_STRATEGY_LIST_STRATEGY_H_

#include <just/data/segment/SegmentStrategy.h>

namespace just
{
    namespace data
    {

        class ListStrategy
            : public SegmentStrategy
        {
        public:
            ListStrategy(
                SegmentMedia & media);

            virtual ~ListStrategy();

        public:
            virtual bool next_segment(
                SegmentPosition & pos, 
                boost::system::error_code & ec);

            virtual bool get_url(
                SegmentPosition const & pos, 
                framework::string::Url & url, 
                boost::system::error_code & ec);

            virtual void on_error(
                SegmentPosition const & pos, 
                boost::system::error_code & ec);

        public:
            void insert(
                SegmentStrategy & strategy);

            void clear();

        private:
            struct ListItem
            {
                ListItem()
                    : strategy(NULL)
                    , next(NULL)
                {
                }

                SegmentStrategy * strategy;
                ListItem * next;
            };

        private:
            ListItem * strategys_;
            ListItem * last_strategy_;
        };

    } // namespace data
} // namespace just

#endif // _JUST_DATA_STRATEGY_LIST_STRATEGY_H_
