#include <nn/ffl/FFLiUtil.h>

namespace
{

// Deleted in NSMBU:

bool CheckPower2(u32 value)
{
    if (value == 0)
        return false;

    return ((value - 1) & value) == 0;
}

}

u32 FFLiRoundUp(u32 value, u32 alignment)
{
    RIO_ASSERT(CheckPower2(alignment)); // CheckPower2(digit)
    return (value + alignment - 1) & ~(alignment - 1);
}

void* FFLiRoundUpPtr(void* ptr, u32 alignment)
{
    uintptr_t uintptr = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t rounded = (uintptr + static_cast<uintptr_t>(alignment) - 1)
        & ~static_cast<uintptr_t>(alignment - 1);
    return reinterpret_cast<void*>(rounded);
}

bool FFLiCheckPowerOfTwo(u32 value)
{
    return CheckPower2(value);
}

bool FFLiCheckAlign(u32 value, u32 alignment)
{
    RIO_ASSERT(FFLiCheckPowerOfTwo(alignment));
    return (value & (alignment - 1)) == 0;
}

bool FFLiCheckAlignPtr(const void* ptr, u32 alignment)
{
    RIO_ASSERT(FFLiCheckPowerOfTwo(alignment));
    uintptr_t uintptr = reinterpret_cast<uintptr_t>(ptr);
    return (uintptr & (alignment - 1)) == 0;
}

void FFLiCopyWcharT2U16(u16* dst, u32 num, const wchar_t* src)
{
    for (u32 i = 0, j = 0; i < num; i++)
    {
        const wchar_t c = src[j];
        // Make sure character is in range.
        RIO_ASSERT(FFLiRange<wchar_t>(0, 0xFFFF, c));

        if ((dst[i] = c) != L'\0')
            j++;
    }
    // Ensure null termination.
    RIO_ASSERT(dst[num-1] == '\0');
}

s32 FFLiCompareString16(const u16* s1, const u16* s2, s32 n)
{
    for (s32 i = 0; i < n; i++)
    {
        s32 cmp = s1[i] - s2[i];
        if (cmp != 0)
            return cmp;

        if (s1[i] == L'\0')
            break;
    }
    return 0;
}

bool FFLiIsNumberCharCode(u16 c)
{
    return (
        (L'0'   <= c && c <= L'9'  ) ||
        (0xFF10 <= c && c <= 0xFF19) || // 0xFF10 == L'０', 0xFF19 == L'９'
        (0x2160 <= c && c <= 0x2169) || // 0x2160 == L'Ⅰ', 0x2169 == L'Ⅹ'
        (0x2170 <= c && c <= 0x2179) || // 0x2170 == L'ⅰ', 0x2179 == L'ⅹ'
        (0x2460 <= c && c <= 0x2473) || // 0x2460 == L'①', 0x2473 == L'⑳'
        (0x2776 <= c && c <= 0x277E)    // 0x2776 == L'❶', 0x277E == L'❾'
    );
}
