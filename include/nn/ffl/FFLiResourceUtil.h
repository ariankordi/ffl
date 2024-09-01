#ifndef FFLI_RESOURCE_UTIL_H_
#define FFLI_RESOURCE_UTIL_H_

#include <nn/ffl/FFLResult.h>

class FFLiResourceHeader;

FFLResult FFLiIsVaildResourceHeader(const FFLiResourceHeader* pHeader);

FFLiResourceHeader* DetermineAndAllocateResourceHeaderType(void* pData, bool* needsEndianSwap);

#endif // FFLI_RESOURCE_UTIL_H_
