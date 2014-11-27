// DataModule.h

#ifndef _JUST_DATA_DATA__MODULE_H_
#define _JUST_DATA_DATA__MODULE_H_

#include "just/common/CommonModuleBase.h"

namespace just
{
    namespace data
    {

        class DataModule
            : public just::common::CommonModuleBase<DataModule>
        {
        public:
            DataModule(
                util::daemon::Daemon & daemon);

            ~DataModule();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();
        };

    } // namespace data
} // namespace just

#endif // _JUST_DATA_DATA__MODULE_H_
