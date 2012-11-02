// SinkBase.h

#ifndef _PPBOX_DATA_SINK_BASE_H_
#define _PPBOX_DATA_SINK_BASE_H_

#include <ppbox/common/Call.h>
#include <ppbox/common/Create.h>

#include <util/stream/Sink.h>

#include <framework/string/Url.h>

#define PPBOX_REGISTER_SINK(n, c) \
    static ppbox::common::Call reg_ ## n(ppbox::data::SinkBase::register_sink, BOOST_PP_STRINGIZE(n), ppbox::common::Creator<c>())

namespace ppbox
{
    namespace data
    {

        class SinkBase
            : public util::stream::Sink
        {
        public:
            typedef boost::function<void (
                boost::system::error_code const &)
            > response_type;

            typedef boost::function<SinkBase * (
                boost::asio::io_service & )
            > register_type;

        public:
            static void register_sink(
                std::string const & name,
                register_type func);
            
            static SinkBase * create(
                boost::asio::io_service & io_svc,
                std::string const & proto);

            static void destory(
                SinkBase* & sink);

        public:
            SinkBase(
                boost::asio::io_service & io_svc);

            virtual ~SinkBase();

        public:
            virtual boost::system::error_code open(
                framework::string::Url const & url,
                boost::system::error_code & ec) = 0;

            virtual void async_open(
                framework::string::Url const & url,
                response_type const & resp);

            virtual bool is_open(
                boost::system::error_code & ec) = 0;

            virtual boost::system::error_code cancel(
                boost::system::error_code & ec);

            virtual boost::system::error_code close(
                boost::system::error_code & ec) = 0;

        public:
            virtual bool continuable(
                boost::system::error_code const & ec);

            virtual bool recoverable(
                boost::system::error_code const & ec);

        private:
            static std::map< std::string, SinkBase::register_type > & sink_map();
        };

    } // namespace data
} // namespace ppbox

#endif // _PPBOX_DATA_SINK_BASE_H_
