// DataModule.cpp

#include "just/data/Common.h"
#include "just/data/DataModule.h"
#include "just/data/media/MediaTypes.h"

namespace just
{
    namespace data
    {

        DataModule::DataModule(
            util::daemon::Daemon & daemon)
            : just::common::CommonModuleBase<DataModule>(daemon, "DataModule")
        {
        }

        DataModule::~DataModule()
        {
        }

        boost::system::error_code DataModule::startup()
        {
            boost::system::error_code ec;
            return ec;
        }

        void DataModule::shutdown()
        {
        }

    } // namespace data
} // namespace just
