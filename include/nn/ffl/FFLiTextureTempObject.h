#ifndef FFLI_TEXTURE_TEMP_OBJECT_H_
#define FFLI_TEXTURE_TEMP_OBJECT_H_

#include <nn/ffl/FFLiFacelineTextureTempObject.h>
#include <nn/ffl/FFLiMaskTexturesTempObject.h>

#ifdef __cplusplus
extern "C" {
#else
#include <nn/ffl/FFLiRawMask.h>
#endif

typedef struct FFLiCharModel FFLiCharModel;

typedef struct FFLiTextureTempObject
{
    FFLiMaskTexturesTempObject      maskTextures;
    FFLiFacelineTextureTempObject   facelineTexture;
}
FFLiTextureTempObject;
NN_STATIC_ASSERT32(sizeof(FFLiTextureTempObject) == 0x4D4);

// defined in FFLiCharModelCreator.cpp
void FFLiDeleteTextureTempObject(FFLiCharModel* pModel);

#ifdef __cplusplus
}
#endif

#endif // FFLI_TEXTURE_TEMP_OBJECT_H_
