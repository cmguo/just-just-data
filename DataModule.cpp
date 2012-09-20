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
        // 查找segmentsource的函数对象
        struct SegmentSourceFind
        {
            SegmentSourceFind(
                SegmentSource const * source)
                : source_(source)
            {
            }

            bool operator()(SegmentSourceStatisic statisic)
            {
                if (statisic.source == source_) {
                    return true;
                } else {
                    return false;
                }
            }

            SegmentSource const * source_;
        };

        struct SegmentSourceFindMedia
        {
            SegmentSourceFindMedia(
                MediaBase const * media)
                : media_(media)
            {
            }

            bool operator()(SegmentSourceStatisic statisic)
            {
                if (statisic.source->media() == media_) {
                    return true;
                } else {
                    return false;
                }
            }

            MediaBase const * media_;
        };

        struct SegmentSourceFindSource
        {
            SegmentSourceFindSource(
                SourceBase const * source)
                : source_(source)
            {
            }

            bool operator()(SegmentSourceStatisic statisic)
            {
                if (statisic.source->source() == source_) {
                    return true;
                } else {
                    return false;
                }
            }

            SourceBase const * source_;
        };

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

        void DataModule::async_open(
            framework::string::Url const & url,
            open_respone_trpe resp)
        {
            boost::mutex::scoped_lock lock(mutex_);
            SegmentSource * segment_source = new SegmentSource(io_srv_);
            SegmentSourceStatisic statisic;
            statisic.resp = resp;
            statisic.source = segment_source;
            statisic.state = State::closed;
            segment_source->async_open(
                url, 
                0, 
                0, 
                boost::bind(&DataModule::open_callback, this, statisic.source, _1));
            statisic.state = State::opening;
            segment_sources_.push_back(statisic);

        }

        void DataModule::open_callback(
            SegmentSource * segment_source, 
            error_code const & ec)
        {
            boost::mutex::scoped_lock lock(mutex_);
            std::vector<SegmentSourceStatisic>::iterator iter = std::find_if(
                segment_sources_.begin(), 
                segment_sources_.end(), 
                SegmentSourceFind(segment_source));
            if (iter != segment_sources_.end()) {
                if ((*iter).state == State::canceled) {
                    (*iter).state = State::closed;
                    (*iter).resp(ec, NULL);
                    delete (*iter).source;
                    segment_sources_.erase(iter);
                } else {
                    (*iter).state = State::opened;
                    (*iter).resp(ec, (*iter).source);
                }
            } else {
                assert(0);
            }
        }

        void DataModule::close(
            SegmentSource const * segment_source, 
            error_code & ec)
        {
            boost::mutex::scoped_lock lock(mutex_);
            std::vector<SegmentSourceStatisic>::iterator iter = std::find_if(
                segment_sources_.begin(), 
                segment_sources_.end(), 
                SegmentSourceFind(segment_source));
            if (iter != segment_sources_.end()) {
                if ((*iter).state == State::opening) {
                    (*iter).source->cancel(ec);
                    (*iter).state = State::canceled;
                } else {
                    (*iter).source->close(ec);
                    (*iter).state = State::closed;
                    delete (*iter).source;
                    segment_sources_.erase(iter);
                }
            }
        }

        SourceBase const * DataModule::get_media_source(
            MediaBase const * media)
        {
            boost::mutex::scoped_lock lock(mutex_);
            std::vector<SegmentSourceStatisic>::iterator iter = std::find_if(
                segment_sources_.begin(), 
                segment_sources_.end(), 
                SegmentSourceFindMedia(media));
            if (iter != segment_sources_.end()) {
                return (*iter).source->source();
            } else {
                return NULL;
            }
        }

        MediaBase const * DataModule::get_source_media(
            SourceBase const * source)
        {
            boost::mutex::scoped_lock lock(mutex_);
            std::vector<SegmentSourceStatisic>::iterator iter = std::find_if(
                segment_sources_.begin(), 
                segment_sources_.end(), 
                SegmentSourceFindSource(source));
            if (iter != segment_sources_.end()) {
                return (*iter).source->media();
            } else {
                return NULL;
            }
        }

    }
}
