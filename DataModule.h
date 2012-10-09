// DataModule.h

#include "ppbox/cdn/CdnModule.h"
#include "ppbox/common/CommonModuleBase.h"

#include <framework/string/Url.h>
#include <boost/thread/mutex.hpp>

namespace ppbox
{
    namespace data
    {
        class SegmentSource;
        class MediaBase;
        class SourceBase;

        typedef boost::function< void ( 
            boost::system::error_code const & ec, 
            SegmentSource *) 
        > open_respone_trpe;

        struct State
        {
             enum Enum
             {
                 closed,
                 opening,
                 canceled,
                 opened,
             };
        };

        struct SegmentSourceStatisic
        {
            State::Enum state;
            SegmentSource * source;
            open_respone_trpe resp;
        };

        class DataModule
            : public ppbox::common::CommonModuleBase<DataModule>
        {
        public:
            DataModule(
                util::daemon::Daemon & daemon);

            ~DataModule();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();

            void async_open(
                framework::string::Url const & url,
                open_respone_trpe resp);

            void close(
                SegmentSource const *, 
                boost::system::error_code &);

            SourceBase const * get_media_source(
                MediaBase const *);

            MediaBase const * get_source_media(
                SourceBase const *);

        private:
            void open_callback(
                SegmentSource * segment_source, 
                boost::system::error_code const & ec);

        private:
            boost::asio::io_service & io_srv_;
            std::vector<SegmentSourceStatisic> segment_sources_;
            boost::mutex mutex_;

        };
    }
}
