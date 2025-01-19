#include <nn/ffl/FFLiSwapEndian.h>

namespace {

u32 TypeToSize(FFLiSwapEndianType type)
{
    RIO_ASSERT(type < 3);
    switch (type)
    {
    case FFLI_SWAP_ENDIAN_TYPE_U8:
        return 1;
    case FFLI_SWAP_ENDIAN_TYPE_U16:
        return 2;
    case FFLI_SWAP_ENDIAN_TYPE_U32:
        return 4;
    }
    return 0;
}

}

u32 FFLiSwapEndianGroup(void* ptr, const FFLiSwapEndianDesc* pDesc, u32 num)
{
    void* pPtr = ptr;

    for (u32 i = 0; i < num; i++)
    {
        const FFLiSwapEndianDesc& current = pDesc[i];
        RIO_ASSERT(current.count > 0);

        switch (current.type)
        {
        case FFLI_SWAP_ENDIAN_TYPE_U8:
            FFLiSwapEndianArrayImpl<u8>(static_cast<u8*>(pPtr), current.count);
            break;
        case FFLI_SWAP_ENDIAN_TYPE_U16:
            FFLiSwapEndianArrayImpl<u16>(static_cast<u16*>(pPtr), current.count);
            break;
        case FFLI_SWAP_ENDIAN_TYPE_U32:
            FFLiSwapEndianArrayImpl<u32>(static_cast<u32*>(pPtr), current.count);
            break;
        }

        pPtr = static_cast<u8*>(pPtr) + TypeToSize(current.type) * current.count;
    }

#if RIO_DEBUG
    // Check for overflow
    [[maybe_unused]] size_t diff = static_cast<u8*>(pPtr) - static_cast<u8*>(ptr);
    RIO_ASSERT(diff <= UINT32_MAX && "Pointer difference exceeds u32 range.");
#endif // RIO_DEBUG

    return static_cast<u8*>(pPtr) - static_cast<u8*>(ptr); // NOTE: warning below
    // warning C4244: 'return': conversion from '__int64' to 'u32', possible loss of data
}
