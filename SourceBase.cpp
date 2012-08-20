//SourceBase.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/SourceBase.h"

namespace ppbox
{
    namespace data
    {

        std::map< std::string, SourceBase::register_type > & SourceBase::source_map()
        {
            static std::map< std::string, SourceBase::register_type > get_map;
            return get_map;
        }

        void SourceBase::register_source(
            std::string const & name, 
            register_type func)
        {
            source_map().insert(std::make_pair(name, func));
            return;
        }

        SourceBase::SourceBase(
            boost::asio::io_service & io_svc)
            : util::stream::Source(io_svc)
        {}

        SourceBase::~SourceBase()
        {
        }

        SourceBase * SourceBase::create(
            boost::asio::io_service & io_svc,
            std::string const & proto)
        {
            std::map< std::string, register_type >::iterator iter = source_map().find(proto);
            if (source_map().end() == iter) {
                return NULL;
            }
            return iter->second(io_svc);
        }

    } // data
} // ppbox
