// DataModule.h

#include "ppbox/common/CommonModuleBase.h"

namespace ppbox
{
    namespace data
    {

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
        };
    }
}
