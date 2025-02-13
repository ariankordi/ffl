#include <nn/ffl/detail/FFLiBug.h>

void FFLiBugEndianSwap(void* ptr, u32 size)
{
}

u32 FFLiBugCanSwapSize(u32 size)
{
    return (size + (4 - 1)) & ~(4 - 1);
}

void* FFLiBugVgtFixedIndexPtr(void* ptr)
{
    // 0x20 = rio::Drawer::cIdxAlignment
    return (u8*)ptr + (0x20 - 4);
}

void* FFLiBugVgtFixedIndexOriginalPtr(void* ptr)
{
    // 0x20 = rio::Drawer::cIdxAlignment
    return (u8*)ptr - (0x20 - 4);
}

u32 FFLiBugCanVgtFixedIndexSize(u32 size)
{
    // 0x20 = rio::Drawer::cIdxAlignment
    return size + (0x20 - 4);
}
