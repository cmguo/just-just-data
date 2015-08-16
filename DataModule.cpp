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

        bool DataModule::startup(
            boost::system::error_code & ec)
        {
            return true;
        }

        bool DataModule::shutdown(
            boost::system::error_code & ec)
        {
            return true;
        }

    } // namespace data
} // namespace just
