// SourceBase.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/SourceBase.h"
#include "ppbox/data/MediaBase.h"

#include <boost/bind.hpp>

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

        void SourceBase::destory(
            SourceBase* & source)
        {
            delete source;
            source = NULL;
        }

        SourceBase::SourceBase(
            boost::asio::io_service & io_svc)
            : util::stream::Source(io_svc)
        {
        }

        SourceBase::~SourceBase()
        {
        }

        void SourceBase::async_open(
            framework::string::Url const & url,
            boost::uint64_t beg, 
            boost::uint64_t end, 
            response_type const & resp)
        {
            boost::system::error_code ec;
            open(url, beg, end, ec);
            get_io_service().post(boost::bind(resp, ec));
        }

        boost::system::error_code SourceBase::cancel(
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        boost::uint64_t SourceBase::total(
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::no_data;
            return 0;
        }

        boost::system::error_code SourceBase::set_non_block(
            bool non_block, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        boost::system::error_code SourceBase::set_time_out(
            boost::uint32_t time_out, 
            boost::system::error_code & ec)
        {
            ec = framework::system::logic_error::not_supported;
            return ec;
        }

        bool SourceBase::continuable(
            boost::system::error_code const & ec)
        {
            return false;
        }

        bool SourceBase::recoverable(
            boost::system::error_code const & ec)
        {
            return false;
        }

        void SourceBase::media(MediaBase * media)
        {
            media_ = media;
        }

        MediaBase * SourceBase::media(void)
        {
            return media_;
        }

    } // namespace data
} // namespace ppbox
