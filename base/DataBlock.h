// DataBlock.h

#ifndef _JUST_DATA_BASE_DATA_BLOCK_H_
#define _JUST_DATA_BASE_DATA_BLOCK_H_

namespace just 
{
    namespace data 
    {

        struct DataBlock
        {
            DataBlock()
                : offset(0)
                , size(0)
            {
            }

            DataBlock(
                boost::uint64_t o, 
                boost::uint32_t s)
                : offset(o)
                , size(s)
            {
            }

            boost::uint64_t end() const
            {
                return offset + size;
            }

            boost::uint64_t offset;
            boost::uint32_t size;
        };

    } // namespace data
} // namespace just

#endif//_JUST_DATA_BASE_DATA_BLOCK_H_
