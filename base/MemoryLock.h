// MemoryLock.h

#ifndef _PPBOX_DATA_BASE_MEMORY_LOCK_H_
#define _PPBOX_DATA_BASE_MEMORY_LOCK_H_

#include <framework/container/List.h>

namespace ppbox 
{
    namespace data 
    {

        struct MemoryLock
            : framework::container::ListHook<MemoryLock>::type
        {
            MemoryLock()
                : pointer(NULL)
                , offset(0)
            {
            }

            void * pointer;
            boost::uint64_t offset;
            framework::container::List<MemoryLock> join;
        };

    } // namespace data
} // namespace ppbox

#endif//_PPBOX_DATA_BASE_MEMORY_LOCK_H_
