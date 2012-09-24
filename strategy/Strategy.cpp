// Strategy.cpp

#include "ppbox/data/Common.h"
#include <ppbox/data/strategy/Strategy.h>

using namespace boost::system;

namespace ppbox
{
    namespace data
    {
        std::map< std::string, Strategy::register_type > & Strategy::strategy_map()
        {
            static std::map< std::string, Strategy::register_type > get_map;
            return get_map;
        }

        void Strategy::regist_strategy(
            std::string const & type, 
            Strategy::register_type func)
        {
            strategy_map().insert(std::make_pair(type, func));
        }

        Strategy * Strategy::create(
            std::string const & type, 
            MediaBase & media)
        {
            std::map<std::string, Strategy::register_type >::iterator iter = 
                strategy_map().find(type);
            if (strategy_map().end() == iter) {
                return NULL;
            }
            return iter->second(media);
        }

        void Strategy::destory(Strategy* & strategy)
        {
            delete strategy;
            strategy = NULL;
        }

        Strategy::Strategy(MediaBase & media)
            : media_(media)
            , pos_(0)
        {
        }

        Strategy::~Strategy()
        {
        }

    }
}
