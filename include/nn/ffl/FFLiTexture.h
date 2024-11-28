#ifndef FFLI_TEXTURE_H_
#define FFLI_TEXTURE_H_

#include <nn/ffl/FFLResourceType.h>
#include <nn/ffl/FFLResult.h>

#include <nn/ffl/FFLiTexturePartsType.h>

#include <nn/ffl/FFLTexture.h>

class FFLiResourceLoader;
class FFLiResourceManager;

FFLResult FFLiLoadTextureWithAllocate(FFLTexture** ppTexture2D, FFLiTexturePartsType partsType, u32 index, FFLiResourceLoader* pResLoader);
void FFLiDeleteTexture(FFLTexture** ppTexture2D, bool isExpand);

#endif // FFLI_TEXTURE_H_
