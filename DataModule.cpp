// DataModule.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/DataModule.h"
#include "ppbox/data/SegmentSource.h"

using namespace boost::system;

namespace ppbox
{
    namespace data
    {

        DataModule::DataModule(
            util::daemon::Daemon & daemon)
            : ppbox::common::CommonModuleBase<DataModule>(daemon, "DataModule")
            , cdn_module_(util::daemon::use_module<ppbox::cdn::Cdn>(daemon))
        {
        }

        DataModule::~DataModule()
        {
        }

        error_code DataModule::startup()
        {
            error_code ec;
            return ec;
        }

        void DataModule::shutdown()
        {
        }

        // TEST
        SegmentSource * DataModule::open()
        {
            return new SegmentSource(io_svc());
        }

        void DataModule::close(SegmentSource * source)
        {
            delete source;
            source = NULL;
        }

    }
}
