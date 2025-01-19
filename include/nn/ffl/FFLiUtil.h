#ifndef FFLI_UTIL_H_
#define FFLI_UTIL_H_

#include <nn/ffl/types.h>

template <typename T>
T FFLiMin(T a, T b)
{
    if (a >= b) return b;
    else        return a;
}

template <typename T>
T FFLiMax(T a, T b)
{
    if (a <= b) return b;
    else        return a;
}

template <typename T>
bool FFLiRange(T min, T max, T value)
{
    return min <= value && value <= max;
}

template <typename T>
T FFLiClamp(T min, T max, T value)
{
    if (value < min)  return min;
    if (max <= value) return max;
    return value;
}

u32 FFLiRoundUp(u32 value, u32 alignment);
void* FFLiRoundUpPtr(void* ptr, u32 alignment);
//void* FFLiRoundUpPtrWithOffset(void* ptr, u32 alignment, u32 offset); // Deleted in NSMBU
//u32 FFLiRoundDown(u32 value, u32 alignment); // Deleted in NSMBU

bool FFLiCheckPowerOfTwo(u32 value); // Deleted in NSMBU
bool FFLiCheckAlign(u32 value, u32 alignment);
bool FFLiCheckAlignPtr(const void* ptr, u32 alignment);

void FFLiCopyWcharT2U16(u16* dst, u32 size, const wchar_t* src);
//void FFLiSetQuestionMarkName(u16* dst, u32 num); // num >= ARRAY_NUM(s_QuestionMarkName) (== 4???)
//void FFLiCopyString16(u16* dst, const u16* src, u32 len); // Deleted in NSMBU
s32 FFLiCompareString16(const u16* s1, const u16* s2, s32 n);
bool FFLiIsNumberCharCode(u16 c);

#endif // FFLI_UTIL_H_
