// MemoryLock.h

#ifndef _JUST_DATA_BASE_MEMORY_LOCK_H_
#define _JUST_DATA_BASE_MEMORY_LOCK_H_

#include <framework/container/List.h>

namespace just 
{
    namespace data 
    {

        struct MemoryLock
            : framework::container::ListHook<MemoryLock>::type
        {
            MemoryLock()
                : offset(0)
                , size(0)
                , pointer(NULL)
            {
            }

            boost::uint64_t offset;
            size_t size;
            void * pointer;
            framework::container::List<MemoryLock> join;
        };

    } // namespace data
} // namespace just

#endif//_JUST_DATA_BASE_MEMORY_LOCK_H_
