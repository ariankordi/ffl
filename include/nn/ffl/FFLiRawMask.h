#ifndef FFLI_RAW_MASK_H_
#define FFLI_RAW_MASK_H_

#include <nn/ffl/FFLiRawMaskParts.h>

#if FFL_USE_RIO
    #include <math/rio_Matrix.h>
#endif

#ifdef __cplusplus
extern "C" {
#else
#include <nn/ffl/FFLShaderCallback.h>
#endif

typedef struct FFLiRawMaskDrawParam
{
    FFLiRawMaskPartsDrawParam   drawParamRawMaskPartsEye[2];
    FFLiRawMaskPartsDrawParam   drawParamRawMaskPartsEyebrow[2];
    FFLiRawMaskPartsDrawParam   drawParamRawMaskPartsMouth;
    FFLiRawMaskPartsDrawParam   drawParamRawMaskPartsMustache[2];
    FFLiRawMaskPartsDrawParam   drawParamRawMaskPartsMole;
    FFLiRawMaskPartsDrawParam   drawParamRawMaskPartsFill;
}
FFLiRawMaskDrawParam;
NN_STATIC_ASSERT32(sizeof(FFLiRawMaskDrawParam) == 0x3A8);

#ifdef __cplusplus
}

#include <nn/ffl/FFLTexture.h>

struct FFLiRawMaskTextureDesc
{
    FFLTexture* pTexturesEye[2];
    FFLTexture* pTexturesEyebrow[2];
    FFLTexture* pTextureMouth;
    FFLTexture* pTexturesMustache[2];
    FFLTexture* pTextureMole;
};
NN_STATIC_ASSERT32(sizeof(FFLiRawMaskTextureDesc) == 0x20);

struct  FFLiCharInfo;
class   FFLiShaderCallback;

void FFLiInitDrawParamRawMask(FFLiRawMaskDrawParam* pDrawParam, const FFLiCharInfo* pCharInfo, s32 resolution, s32 leftEyeIndex, s32 rightEyeIndex, s32 eyebrowIndex, s32 mouthIndex, const FFLiRawMaskTextureDesc* pDesc); // eyebrowIndex and mouthIndex are from AFL
void FFLiDeleteDrawParamRawMask(FFLiRawMaskDrawParam* pDrawParam);

#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

void FFLiInvalidateRawMask(FFLiRawMaskDrawParam* pDrawParam);
void FFLiDrawRawMask(const FFLiRawMaskDrawParam* pDrawParam,
#ifdef __cplusplus
    const FFLiShaderCallback* pCallback);
#else
    FFLShaderCallback** pCallback);
#endif // __cplusplus

void FFLiGetMaskMatrix(
#if FFL_USE_RIO
    rio::BaseMtx44f* pBaseMtx44f, f32 resolution);
#else
    float pBaseMtx44f[16], f32 resolution);
#endif // FFL_USE_RIO

#ifdef __cplusplus
}
#endif

#endif // FFLI_RAW_MASK_H_
