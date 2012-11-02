// SourceBase.h

#ifndef _PPBOX_DATA_SOURCE_BASE_H_
#define _PPBOX_DATA_SOURCE_BASE_H_

#include <ppbox/common/Call.h>
#include <ppbox/common/Create.h>

#include <util/stream/Source.h>

#include <framework/string/Url.h>

#define PPBOX_REGISTER_SOURCE(n, c) \
    static ppbox::common::Call reg_ ## n(ppbox::data::SourceBase::register_source, BOOST_PP_STRINGIZE(n), ppbox::common::Creator<c>())

namespace ppbox
{
    namespace data
    {

        class MediaBase;

        class SourceBase
            : public util::stream::Source
        {
        public:
            typedef boost::function<void (
                boost::system::error_code const &)
            > response_type;

            typedef boost::function<SourceBase * (
                boost::asio::io_service & )
            > register_type;

        public:
            static void register_source(
                std::string const & name,
                register_type func);
            
            static SourceBase * create(
                boost::asio::io_service & io_svc,
                std::string const & proto);

            static SourceBase * create(
                boost::asio::io_service & io_svc,
                MediaBase & media);

            static void destory(
                SourceBase* & source);

        public:
            SourceBase(
                boost::asio::io_service & io_svc);

            virtual ~SourceBase();

        public:
            virtual boost::system::error_code open(
                framework::string::Url const & url, 
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec) = 0;

            virtual void async_open(
                framework::string::Url const & url,
                boost::uint64_t beg, 
                boost::uint64_t end, 
                response_type const & resp);

            virtual bool is_open(
                boost::system::error_code & ec)= 0;

            virtual boost::system::error_code cancel(
                boost::system::error_code & ec);

            virtual boost::system::error_code close(
                boost::system::error_code & ec)= 0;

            virtual boost::uint64_t total(
                boost::system::error_code & ec);

        public:
            virtual boost::system::error_code set_non_block(
                bool non_block, 
                boost::system::error_code & ec);

            virtual boost::system::error_code set_time_out(
                boost::uint32_t time_out, 
                boost::system::error_code & ec);

            virtual bool continuable(
                boost::system::error_code const & ec);

            virtual bool recoverable(
                boost::system::error_code const & ec);

        private:
            static std::map<std::string, register_type> & source_map();
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_SOURCE_BASE_H_
