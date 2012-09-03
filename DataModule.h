// DataModule.h

#include "ppbox/cdn/Cdn.h"
#include "ppbox/common/CommonModuleBase.h"

namespace ppbox
{
    namespace data
    {
        class SegmentSource;

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

            // TEST
            SegmentSource * open();

            void close(SegmentSource * source);

        private:
            ppbox::cdn::Cdn & cdn_module_;

        };
    }
}
