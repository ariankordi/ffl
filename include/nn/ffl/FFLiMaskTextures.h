#ifndef FFLI_MASK_TEXTURES_H_
#define FFLI_MASK_TEXTURES_H_

#include <nn/ffl/FFLExpression.h>
#include <nn/ffl/FFLResourceType.h>
#include <nn/ffl/FFLResult.h>

//#include <misc/rio_Types.h>

#ifdef __cplusplus
    struct FFLiRenderTexture;
#else
    #include <nn/ffl/FFLiMaskTexturesTempObject.h>
#endif

typedef struct FFLiMaskTextures
{
#ifdef __cplusplus
    FFLiRenderTexture*
#else
    FFLTexture**
#endif
    pRenderTextures[FFL_EXPRESSION_MAX];
}
FFLiMaskTextures;
NN_STATIC_ASSERT32(sizeof(FFLiMaskTextures) == 0x118);

#ifdef __cplusplus

struct  FFLiCharInfo;
#if RIO_IS_CAFE
class   FFLiCopySurface;
#endif // RIO_IS_CAFE
struct  FFLiMaskTexturesTempObject;
class   FFLiResourceLoader;
class   FFLiResourceManager;
class   FFLiShaderCallback;

FFLExpression FFLiInitMaskTextures(FFLiMaskTextures* pMaskTextures, FFLExpressionFlag expressionFlag, u32 resolution, bool enableMipMap);
void FFLiDeleteMaskTextures(FFLiMaskTextures* pMaskTextures);

FFLResult FFLiInitTempObjectMaskTextures(FFLiMaskTexturesTempObject* pObject, const FFLiMaskTextures* pMaskTextures, const FFLiCharInfo* pCharInfo, FFLExpressionFlag expressionFlag, u32 resolution, bool enableMipMap, FFLiResourceLoader* pResLoader);

void FFLiRenderMaskTextures(FFLiMaskTextures* pMaskTextures, FFLiMaskTexturesTempObject* pObject, const FFLiShaderCallback* pCallback
#if RIO_IS_CAFE
    , FFLiCopySurface* pCopySurface
#endif // RIO_IS_CAFE
);

extern "C" {
#endif

void FFLiDeleteTempObjectMaskTextures(FFLiMaskTexturesTempObject* pObject, FFLExpressionFlag expressionFlag, FFLResourceType resourceType);

#ifdef __cplusplus
}
#endif

#endif // FFLI_MASK_TEXTURES_H_
