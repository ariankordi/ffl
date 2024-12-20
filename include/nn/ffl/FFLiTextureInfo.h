#ifndef FFLI_TEXTURE_INFO_H_
#define FFLI_TEXTURE_INFO_H_

// Completely custom header to house an implementation
// to simply load the texture/mipmap info and pointers
// info FFLTextureInfo for FFLiTexture.cpp to use.

#include <nn/ffl/FFLResourceType.h>
#include <nn/ffl/FFLResult.h>

#include <nn/ffl/FFLiTexturePartsType.h>

#include <nn/ffl/FFLTextureInfo.h>

class FFLiResourceLoader;

FFLResult FFLiLoadTextureInfo(FFLTextureInfo* textureInfo, FFLiTexturePartsType partsType, u32 index, FFLiResourceLoader* pResLoader, void** pData);

#endif // FFLI_TEXTURE_INFO_H_
