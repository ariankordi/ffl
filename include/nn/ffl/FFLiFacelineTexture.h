#ifndef FFLI_FACELINE_TEXTURE_H_
#define FFLI_FACELINE_TEXTURE_H_

#include <nn/ffl/FFLResult.h>

#include <nn/ffl/types.h>

#include <nn/ffl/FFLResourceType.h>

#ifndef __cplusplus
#include <nn/ffl/detail/FFLiCharInfo.h>
#include <nn/ffl/FFLShaderCallback.h>
#include <nn/ffl/FFLiFacelineTextureTempObject.h>

#else
struct  FFLiCharInfo;

#if RIO_IS_CAFE
class   FFLiCopySurface;
#endif // RIO_IS_CAFE
struct  FFLiFacelineTextureTempObject;
struct  FFLiRenderTexture;
class   FFLiResourceLoader;
class   FFLiResourceManager;
class   FFLiShaderCallback;

void FFLiInitFacelineTexture(FFLiRenderTexture* pRenderTexture, u32 resolution, bool enableMipMap);
void FFLiDeleteFacelineTexture(FFLiRenderTexture* pRenderTexture);

FFLResult FFLiInitTempObjectFacelineTexture(FFLiFacelineTextureTempObject* pObject, FFLiRenderTexture* pRenderTexture, const FFLiCharInfo* pCharInfo, u32 resolution, bool enableMipMap, FFLiResourceLoader* pResLoader);

void FFLiRenderFacelineTexture(FFLiRenderTexture* pRenderTexture, const FFLiCharInfo* pCharInfo, u32 resolution, FFLiFacelineTextureTempObject* pObject, const FFLiShaderCallback* pCallback
#if RIO_IS_CAFE
    , FFLiCopySurface* pCopySurface
#endif // RIO_IS_CAFE
);

#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

void FFLiDeleteTempObjectFacelineTexture(FFLiFacelineTextureTempObject* pObject, const FFLiCharInfo* pCharInfo, FFLResourceType resourceType);
void FFLiInvalidateTempObjectFacelineTexture(FFLiFacelineTextureTempObject* pObject);
void FFLiDrawFacelineTexture(FFLiFacelineTextureTempObject* pObject,
#ifdef __cplusplus
      const FFLiShaderCallback*
#else
      FFLShaderCallback**
#endif
pCallback);

#ifdef __cplusplus
}
#endif


#endif // FFLI_FACELINE_TEXTURE_H_
