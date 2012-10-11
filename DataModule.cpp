// DataModule.cpp

#include "ppbox/data/Common.h"
#include "ppbox/data/DataModule.h"
#include "ppbox/data/SegmentSource.h"

#include <boost/bind.hpp>
#include <boost/thread/locks.hpp>
using namespace boost::system;

namespace ppbox
{
    namespace data
    {

        DataModule::DataModule(
            util::daemon::Daemon & daemon)
            : ppbox::common::CommonModuleBase<DataModule>(daemon, "DataModule")
            , io_srv_(daemon.io_svc())
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

    }
}
