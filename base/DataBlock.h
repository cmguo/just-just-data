// DataBlock.h

#ifndef _PPBOX_DATA_BASE_DATA_BLOCK_H_
#define _PPBOX_DATA_BASE_DATA_BLOCK_H_

namespace ppbox 
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
} // namespace ppbox

#endif//_PPBOX_DATA_BASE_DATA_BLOCK_H_
