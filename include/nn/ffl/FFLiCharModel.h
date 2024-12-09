#ifndef FFLI_CHAR_MODEL_H_
#define FFLI_CHAR_MODEL_H_

#include <nn/ffl/FFLBoundingBox.h>
#include <nn/ffl/FFLCharModelDesc.h>
#include <nn/ffl/FFLDrawParam.h>
#include <nn/ffl/FFLExpression.h>
#include <nn/ffl/FFLModelType.h>
#include <nn/ffl/FFLPartsTransform.h>
#include <nn/ffl/FFLResult.h>

#include <nn/ffl/FFLiRenderTexture.h>
#include <nn/ffl/FFLiMaskTextures.h>
#include <nn/ffl/FFLiShapeType.h>

#include <nn/ffl/FFLiTextureTempObject.h>
#include <nn/ffl/detail/FFLiCharInfo.h>

#ifndef __cplusplus
typedef struct FFLCharModelSource FFLCharModelSource;
typedef struct FFLShaderCallback FFLShaderCallback;
#endif


typedef struct FFLiCharModel
{
    FFLiCharInfo            charInfo;
    FFLCharModelDesc        charModelDesc;
    FFLExpression           expression;
    FFLiTextureTempObject*  pTextureTempObject;
    FFLDrawParam            drawParam[FFLI_SHAPE_TYPE_MAX];
    void*                   pShapeData[FFLI_SHAPE_TYPE_MAX];
    FFLiRenderTexture       facelineRenderTexture;
    FFLTexture*             pCapTexture;
    FFLTexture*             pGlassTexture;
    FFLTexture*             pNoselineTexture;
    FFLiMaskTextures        maskTextures;
    FFLVec3                 beardPos;
    FFLVec3                 hairPos;
    FFLVec3                 faceCenterPos;  // Used to calculate nose(line) and glass position
    FFLPartsTransform       partsTransform;
    FFLModelType            modelType;
    FFLBoundingBox          boundingBox[3];
}
FFLiCharModel;
#ifndef __EMSCRIPTEN__
NN_STATIC_ASSERT32(sizeof(FFLiCharModel) == 0x848);
#endif

struct FFLCharModelSource;
struct FFLShaderCallback;

FFLResult FFLiInitCharModelCPUStep(FFLiCharModel* pModel, const FFLCharModelSource* pSource, const FFLCharModelDesc* pDesc);

void FFLiInitCharModelGPUStep(FFLiCharModel* pModel, const FFLShaderCallback* pCallback);

void FFLiDeleteCharModel(FFLiCharModel* pModel);

#ifdef __cplusplus

// names collide (even though this isnt extern C???)
void FFLiGetCharInfoFromCharModel(FFLiCharInfo* pCharInfo, const FFLiCharModel* pModel);
bool FFLiIsAvailableExpression(const FFLiCharModel* pModel, FFLExpression expression);
//FFLiGetDrawParamFromCharModel(FFLiCharModel const *, FFLDrawType)
    // Meant to be exported as FFLGetDrawParamFromCharModel
    // ... but it needs FFLDrawType enum which is used in shader stuff
//FFLiGetModelInfo
//FFLiSetModelInfo // Calls FFLInitCharModel
//FFLiInitCharModel(FFLiCharModel *, FFLDataSource, void const *, unsigned short, FFLResolution, unsigned int, unsigned int, void *)
//FFLiCopyCharModel(FFLiCharModel *, void *, unsigned int, FFLiCharModel const *)

#endif // __cplusplus

void FFLiSetExpression(FFLiCharModel* pModel, FFLExpression expression);
FFLExpression FFLiGetExpression(const FFLiCharModel* pModel);

void FFLiGetPartsTransform(FFLPartsTransform* pTransform, const FFLiCharModel* pModel);

void FFLiSetViewModelType(FFLiCharModel* pModel, FFLModelType type);

const FFLDrawParam* FFLiGetDrawParamOpaFacelineFromCharModel(const FFLiCharModel* pModel);
const FFLDrawParam* FFLiGetDrawParamOpaBeardFromCharModel(const FFLiCharModel* pModel);
const FFLDrawParam* FFLiGetDrawParamOpaNoseFromCharModel(const FFLiCharModel* pModel);
const FFLDrawParam* FFLiGetDrawParamOpaForeheadFromCharModel(const FFLiCharModel* pModel);
const FFLDrawParam* FFLiGetDrawParamOpaHairFromCharModel(const FFLiCharModel* pModel);
const FFLDrawParam* FFLiGetDrawParamOpaCapFromCharModel(const FFLiCharModel* pModel);

const FFLDrawParam* FFLiGetDrawParamXluMaskFromCharModel(const FFLiCharModel* pModel);
const FFLDrawParam* FFLiGetDrawParamXluNoseLineFromCharModel(const FFLiCharModel* pModel);
const FFLDrawParam* FFLiGetDrawParamXluGlassFromCharModel(const FFLiCharModel* pModel);

#ifdef __cplusplus

// Deleted in NSMBU:
const FFLiRenderTexture* FFLiGetFaceTextureFromCharModel(const FFLiCharModel* pModel);
const FFLiRenderTexture* FFLiGetMaskTextureFromCharModel(const FFLiCharModel* pModel, FFLExpression expression);

#endif // __cplusplus

void FFLiSetScale(f32 scale);

#endif // FFLI_CHAR_MODEL_H_
