#ifndef FFLI_MASK_TEXTURES_TEMP_OBJECT_H_
#define FFLI_MASK_TEXTURES_TEMP_OBJECT_H_

#include <nn/ffl/FFLExpression.h>

#include <nn/ffl/FFLiPartsTextures.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
#include <nn/ffl/FFLiRawMask.h>
#else
struct  FFLiRawMaskDrawParam;
#endif

typedef struct FFLiMaskTexturesTempObject
{
    FFLiPartsTextures     partsTextures;
    FFLiRawMaskDrawParam* pRawMaskDrawParam[FFL_EXPRESSION_MAX];
    void* _84;
    void* _88[FFL_EXPRESSION_MAX];    // Deleted
}
FFLiMaskTexturesTempObject;
NN_STATIC_ASSERT32(sizeof(FFLiMaskTexturesTempObject) == 0x388);

#ifdef __cplusplus
}
#endif

#endif // FFLI_MASK_TEXTURES_TEMP_OBJECT_H_
