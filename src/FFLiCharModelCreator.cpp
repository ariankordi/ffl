#include <nn/ffl/FFLBoundingBox.h>
#include <nn/ffl/FFLCharModelDesc.h>
#include <nn/ffl/FFLCharModelSource.h>
#include <nn/ffl/FFLModelType.h>
#include <nn/ffl/FFLModelFlag.h>

#include <nn/ffl/FFLiCharModel.h>
#include <nn/ffl/FFLiCharModelCreateParam.h>
#include <nn/ffl/FFLiCharModelCreator.h>
#include <nn/ffl/FFLiFacelineTexture.h>
#include <nn/ffl/FFLiResourceLoader.h>
#include <nn/ffl/FFLiResourceLoaderBuffer.h>
#include <nn/ffl/FFLiManager.h>
#include <nn/ffl/FFLiModulate.h>
#include <nn/ffl/FFLiShape.h>
#include <nn/ffl/FFLiShapePartsType.h>
#include <nn/ffl/FFLiShapeType.h>
#include <nn/ffl/FFLiTexture.h>
#include <nn/ffl/FFLiTextureTempObject.h>
#include <nn/ffl/FFLiUtil.h>

#include <gfx/rio_Window.h>
#include <math/rio_Matrix.h>

#if RIO_IS_CAFE
#include <gx2/event.h>
#include <gx2/mem.h>
#endif // RIO_IS_CAFE

union F32BitCast
{
    f32 f;
    u32 u;
    struct
    {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        u32 sign        : 1;    // (MSB)
        u32 exponent    : 8;
        u32 mantissa    : 23;   // (LSB)
#else
        u32 mantissa    : 23;   // (LSB)
        u32 exponent    : 8;
        u32 sign        : 1;    // (MSB)
#endif // __BYTE_ORDER__
    };
};
NN_STATIC_ASSERT(sizeof(F32BitCast) == 4);

static bool IsNaN(f32 value)
{
    F32BitCast x = { value };
    // Basically:
    // return x.exponent == 0xff && x.mantissa > 0;
    return (x.u << 1) > 0xff000000;
}

FFLiCharModelCreator::FFLiCharModelCreator(FFLiCharModelCreateParam* pParam, FFLiManager* pManager)
    : m_pCharModelCreateParam(pParam)
    , m_pManager(pManager)
{
}

FFLiCharModelCreator::~FFLiCharModelCreator()
{
}

namespace {

FFLModelType ModelFlagToModelType(u32 flag);
FFLResult InitShapes(FFLiCharModel* pModel, FFLiResourceLoader * pResLoader, const FFLiCoordinate* pCoordinate);
void DeleteShapes(FFLiCharModel* pModel);
FFLResult InitTextures(FFLiCharModel* pModel, FFLiResourceLoader* pResLoader);
void DeleteTextures(FFLiCharModel* pModel);
void AdjustPartsTransform(FFLiCharModel* pModel, const FFLiCoordinate* pCoordinate);
void SetupDrawParam(FFLiCharModel* pModel);

}

FFLResult FFLiCharModelCreator::ExecuteCPUStep(FFLiCharModel* pModel, const FFLCharModelSource* pSource, const FFLCharModelDesc* pDesc)
{
    if (!FFLiCharModelCreateParam::CheckModelDesc(pDesc))
        return FFL_RESULT_ERROR;

    rio::MemUtil::set(pModel, 0, sizeof(FFLiCharModel));

    pModel->charModelDesc = *pDesc;

    // Initialize FFLAllExpressionFlag mid and high u32s
    // to zeroes if the model flag does not indicate they
    // are used - because they are not always initialized.
    if (!(pModel->charModelDesc.modelFlag & FFL_MODEL_FLAG_NEW_EXPRESSIONS))
    {
#ifdef FFL_LOG_CHARMODEL_CLEANUP
        RIO_LOG("model flag does not have FFL_MODEL_FLAG_NEW_EXPRESSIONS, truncating pModel->charModelDesc.allExpressionFlag\n");
#endif
        pModel->charModelDesc.allExpressionFlag.flag.mid = 0;
        pModel->charModelDesc.allExpressionFlag.flag.high = 0;
    }

    pModel->modelType = ModelFlagToModelType(pModel->charModelDesc.modelFlag);

    FFLResult result;

    if (pSource->dataSource == FFL_DATA_SOURCE_DIRECT_POINTER)
    {
        pModel->charInfo = *reinterpret_cast<const FFLiCharInfo*>(pSource->pBuffer);
    } else {
        result = m_pCharModelCreateParam->GetDatabaseManager()->PickupCharInfo(&pModel->charInfo, pSource->dataSource, pSource->pBuffer, pSource->index);
        if (result != FFL_RESULT_OK)
            return result;
    }

    u32 resolution = FFLiCharModelCreateParam::GetResolution(pDesc->resolution);
    bool isEnabledMipMap = FFLiCharModelCreateParam::IsEnabledMipMap(pDesc->resolution);

    FFLResourceType resourceType = pDesc->resourceType;

    if (!m_pCharModelCreateParam->GetResourceManager()->IsValid(resourceType))
        return FFL_RESULT_ERROR;

    FFLiResourceLoaderBuffer resLoaderBuffer(m_pCharModelCreateParam->GetResourceManager(), resourceType);
    FFLiResourceLoader resLoader(m_pCharModelCreateParam->GetResourceManager(), &resLoaderBuffer, resourceType);

    pModel->pTextureTempObject = new FFLiTextureTempObject;

    pModel->expression = FFLiInitMaskTextures(&pModel->maskTextures, pModel->charModelDesc.allExpressionFlag, resolution, isEnabledMipMap);

    result = FFLiInitTempObjectMaskTextures(&pModel->pTextureTempObject->maskTextures, &pModel->maskTextures, &pModel->charInfo, pModel->charModelDesc.allExpressionFlag, resolution, isEnabledMipMap, &resLoader);
    if (result != FFL_RESULT_OK)
    {
        FFLiDeleteMaskTextures(&pModel->maskTextures);
        FFLiDeleteTextureTempObject(pModel);
        return result;
    }

    bool enableFacelineTexture = pModel->charInfo.parts.faceLine != 0 ||
        pModel->charInfo.parts.faceMakeup != 0 ||
        pModel->charInfo.parts.beardType >= FFL_BEARD_SHAPE_MAX; // FFLiInitTempObjectFacelineTexture
    if (enableFacelineTexture)
#ifndef FFL_NO_RENDER_TEXTURE
        FFLiInitFacelineTexture(&pModel->facelineRenderTexture, resolution, isEnabledMipMap);
#else
        pModel->facelineRenderTexture.pTexture2D = FFL_TEXTURE_PLACEHOLDER;
        // HACK to use the pointer as indication
        // to go ahead and make the faceline texture
        // (check if the pTexture2D is not NULL)
#endif
    else
        // if faceline texture is not needed
        pModel->facelineRenderTexture.pTexture2D = NULL;

    if (enableFacelineTexture) {
        result = FFLiInitTempObjectFacelineTexture(&pModel->pTextureTempObject->facelineTexture, &pModel->facelineRenderTexture, &pModel->charInfo, resolution, isEnabledMipMap, &resLoader);
        if (result != FFL_RESULT_OK)
        {
            FFLiDeleteFacelineTexture(&pModel->facelineRenderTexture);
            FFLiDeleteTempObjectMaskTextures(&pModel->pTextureTempObject->maskTextures, pModel->charModelDesc.allExpressionFlag, pDesc->resourceType);
            FFLiDeleteMaskTextures(&pModel->maskTextures);
            FFLiDeleteTextureTempObject(pModel);
            return result;
        }
    }
#ifdef FFL_ENABLE_NEW_MASK_ONLY_FLAG
    if (!(pModel->charModelDesc.modelFlag & FFL_MODEL_FLAG_NEW_MASK_ONLY))
    {
#endif
        result = InitShapes(pModel, &resLoader, &m_pCharModelCreateParam->GetCoordinate());
        if (result != FFL_RESULT_OK)
        {
            if (enableFacelineTexture) {
                FFLiDeleteTempObjectFacelineTexture(&pModel->pTextureTempObject->facelineTexture, &pModel->charInfo, pModel->charModelDesc.resourceType);
                FFLiDeleteFacelineTexture(&pModel->facelineRenderTexture);
            }
            FFLiDeleteTempObjectMaskTextures(&pModel->pTextureTempObject->maskTextures, pModel->charModelDesc.allExpressionFlag, pDesc->resourceType);
            FFLiDeleteMaskTextures(&pModel->maskTextures);
            FFLiDeleteTextureTempObject(pModel);
            return result;
        }
#ifdef FFL_ENABLE_NEW_MASK_ONLY_FLAG
    }
#endif

    result = InitTextures(pModel, &resLoader);
    if (result != FFL_RESULT_OK)
    {
        DeleteShapes(pModel);
        if (enableFacelineTexture) {
            FFLiDeleteTempObjectFacelineTexture(&pModel->pTextureTempObject->facelineTexture, &pModel->charInfo, pModel->charModelDesc.resourceType);
            FFLiDeleteFacelineTexture(&pModel->facelineRenderTexture);
        }
        FFLiDeleteTempObjectMaskTextures(&pModel->pTextureTempObject->maskTextures, pModel->charModelDesc.allExpressionFlag, pDesc->resourceType);
        FFLiDeleteMaskTextures(&pModel->maskTextures);
        FFLiDeleteTextureTempObject(pModel);
        return result;
    }

#ifdef FFL_ENABLE_NEW_MASK_ONLY_FLAG
    if (!(pModel->charModelDesc.modelFlag & FFL_MODEL_FLAG_NEW_MASK_ONLY))
    {
#endif
        AdjustPartsTransform(pModel, &m_pCharModelCreateParam->GetCoordinate());
        SetupDrawParam(pModel);
#ifdef FFL_ENABLE_NEW_MASK_ONLY_FLAG
    }
#endif

    return FFL_RESULT_OK;
}

void FFLiCharModelCreator::ExecuteGPUStep(FFLiCharModel* pModel, const FFLShaderCallback* pCallback)
{
#ifdef FFL_NO_RENDER_TEXTURE
    RIO_LOG("WARNING: FFLInitCharModelGPUStep was called. When FFL_NO_RENDER_TEXTURE is enabled, you need " \
    "to make your own faceline and mask textures. This function will try to make RIO_GL_CALL()s, which may or " \
    "may not crash you right now by jumping to undefined GL function pointers. Proceed with caution.\n");
#else
//#endif // FFL_NO_RENDER_TEXTURE
    u32 resolution = FFLiCharModelCreateParam::GetResolution(pModel->charModelDesc.resolution);

    FFLiShaderCallback shaderCallback;
    shaderCallback.Set(pCallback);

    rio::Matrix44f mvpMatrix = rio::Matrix44f::ident;

    // Flip Y in the view matrix when the
    // default GL clip control is being used

    // (NOTE: Now being done to the primitives directly based on g_TextureFlipY)
/*
#ifdef RIO_NO_CLIP_CONTROL
    mvpMatrix.m[1][1] *= -1.f;
#endif
*/

    shaderCallback.CallSetMatrix(&mvpMatrix);

    FFLiRenderMaskTextures(&pModel->maskTextures, &pModel->pTextureTempObject->maskTextures, &shaderCallback
#if RIO_IS_CAFE
        , &m_pManager->GetCopySurface()
#endif // RIO_IS_CAFE
    );
    if (pModel->facelineRenderTexture.pTexture2D != NULL)
        FFLiRenderFacelineTexture(&pModel->facelineRenderTexture, &pModel->charInfo, resolution, &pModel->pTextureTempObject->facelineTexture, &shaderCallback
#if RIO_IS_CAFE
            , &m_pManager->GetCopySurface()
#endif // RIO_IS_CAFE
        );

    AfterExecuteGPUStep(pModel);

    if (pModel->facelineRenderTexture.pTexture2D != NULL)
        FFLiDeleteTempObjectFacelineTexture(&pModel->pTextureTempObject->facelineTexture, &pModel->charInfo, pModel->charModelDesc.resourceType);
    FFLiDeleteTempObjectMaskTextures(&pModel->pTextureTempObject->maskTextures, pModel->charModelDesc.allExpressionFlag, pModel->charModelDesc.resourceType);

    FFLiDeleteTextureTempObject(pModel);
#endif // FFL_NO_RENDER_TEXTURE
}

void FFLiCharModelCreator::Delete(FFLiCharModel* pModel)
{
#ifdef FFL_LOG_CHARMODEL_CLEANUP
    RIO_LOG("in FFLiCharModelCreator::Delete(%p)\n", pModel);
#endif

#ifndef FFL_NO_RENDER_TEXTURE

#if RIO_IS_CAFE
    GX2DrawDone();
#elif RIO_IS_WIN
    RIO_GL_CALL(glFinish());
#endif

#endif // FFL_NO_RENDER_TEXTURE

    DeleteTextures(pModel);
    DeleteShapes(pModel);
    if (pModel->facelineRenderTexture.pTexture2D != NULL)
    {
#ifdef FFL_LOG_CHARMODEL_CLEANUP
        RIO_LOG("faceline render texture2D != NULL (%p), calling FFLiDeleteFacelineTexture(%p)\n", pModel->facelineRenderTexture.pTexture2D, &pModel->facelineRenderTexture);
#endif
        FFLiDeleteFacelineTexture(&pModel->facelineRenderTexture);
    }
#ifdef FFL_LOG_CHARMODEL_CLEANUP
    else
        RIO_LOG("faceline render texture2D == NULL\n");
#endif

    if (pModel->pTextureTempObject != NULL)
    {
#ifdef FFL_LOG_CHARMODEL_CLEANUP
        RIO_LOG("pTextureTempObject != NULL (%p)\n", pModel->pTextureTempObject);
#endif
        if (pModel->facelineRenderTexture.pTexture2D != NULL)
        {
#ifdef FFL_LOG_CHARMODEL_CLEANUP
            RIO_LOG("faceline render texture2D != NULL (%p), calling FFLiDeleteTempObjectFacelineTexture(%p)\n", pModel->facelineRenderTexture.pTexture2D, &pModel->pTextureTempObject->facelineTexture);
#endif
            FFLiDeleteTempObjectFacelineTexture(&pModel->pTextureTempObject->facelineTexture, &pModel->charInfo, pModel->charModelDesc.resourceType);
        }
#ifdef FFL_LOG_CHARMODEL_CLEANUP
        RIO_LOG("FFLiDeleteTempObjectMaskTextures(%p)\n", &pModel->pTextureTempObject->maskTextures);
#endif
        FFLiDeleteTempObjectMaskTextures(&pModel->pTextureTempObject->maskTextures, pModel->charModelDesc.allExpressionFlag, pModel->charModelDesc.resourceType);

#ifdef FFL_LOG_CHARMODEL_CLEANUP
        RIO_LOG("FFLiDeleteTextureTempObject(%p)\n", &pModel->pTextureTempObject);
#endif

        FFLiDeleteTextureTempObject(pModel);
    }

#ifdef FFL_LOG_CHARMODEL_CLEANUP
    RIO_LOG("FFLiDeleteMaskTextures(%p)\n", &pModel->maskTextures);
#endif

    FFLiDeleteMaskTextures(&pModel->maskTextures);

#ifdef FFL_LOG_CHARMODEL_CLEANUP
    RIO_LOG("exiting FFLiCharModelCreator::Delete()\n");
#endif
}

// this method is needed in case you don't have "delete" in C
void FFLiDeleteTextureTempObject(FFLiCharModel* pModel)
{
    delete pModel->pTextureTempObject;
    pModel->pTextureTempObject = NULL;
}

namespace {

FFLModelType ModelFlagToModelType(u32 flag)
{
    for (u32 i = 0; i < FFL_MODEL_TYPE_MAX; ++i)
        if (flag & 1 << i)
            return FFLModelType(i);

    return FFL_MODEL_TYPE_NORMAL;
}

FFLiShapeType ConvertShapePartsTypeToShapeType(FFLiShapePartsType partsType)
{
    switch (partsType)
    {
    case FFLI_SHAPE_PARTS_TYPE_BEARD:
        return FFLI_SHAPE_TYPE_OPA_BEARD;
    case FFLI_SHAPE_PARTS_TYPE_HAT_NORMAL:
        return FFLI_SHAPE_TYPE_OPA_HAT_NORMAL;
    case FFLI_SHAPE_PARTS_TYPE_HAT_CAP:
        return FFLI_SHAPE_TYPE_OPA_HAT_CAP;
    case FFLI_SHAPE_PARTS_TYPE_FACELINE:
        return FFLI_SHAPE_TYPE_OPA_FACELINE;
    case FFLI_SHAPE_PARTS_TYPE_GLASS:
        return FFLI_SHAPE_TYPE_XLU_GLASS;
    case FFLI_SHAPE_PARTS_TYPE_MASK:
        return FFLI_SHAPE_TYPE_XLU_MASK;
    case FFLI_SHAPE_PARTS_TYPE_NOSELINE:
        return FFLI_SHAPE_TYPE_XLU_NOSELINE;
    case FFLI_SHAPE_PARTS_TYPE_NOSE:
        return FFLI_SHAPE_TYPE_OPA_NOSE;
    case FFLI_SHAPE_PARTS_TYPE_HAIR_NORMAL:
        return FFLI_SHAPE_TYPE_OPA_HAIR_NORMAL;
    case FFLI_SHAPE_PARTS_TYPE_HAIR_CAP:
        return FFLI_SHAPE_TYPE_OPA_HAIR_CAP;
    case FFLI_SHAPE_PARTS_TYPE_FOREHEAD_NORMAL:
        return FFLI_SHAPE_TYPE_OPA_FOREHEAD_NORMAL;
    case FFLI_SHAPE_PARTS_TYPE_FOREHEAD_CAP:
        return FFLI_SHAPE_TYPE_OPA_FOREHEAD_CAP;
    default:
        return FFLI_SHAPE_TYPE_MAX;
    }
}

void UpdateBoundingBox(FFLBoundingBox* pDst, const FFLBoundingBox* pSrc)
{
    if (!IsNaN(pSrc->min.x))
    {
        if (IsNaN(pDst->min.x))
        {
            *pDst = *pSrc;
            return;
        }

        pDst->min.x = FFLiMin(pDst->min.x, pSrc->min.x);
        pDst->min.y = FFLiMin(pDst->min.y, pSrc->min.y);
        pDst->min.z = FFLiMin(pDst->min.z, pSrc->min.z);

        pDst->max.x = FFLiMax(pDst->max.x, pSrc->max.x);
        pDst->max.y = FFLiMax(pDst->max.y, pSrc->max.y);
        pDst->max.z = FFLiMax(pDst->max.z, pSrc->max.z);
    }
}

static const bool UPDATE_BOUNDING_BOX[FFLI_SHAPE_PARTS_TYPE_MAX][3] = {
    {  true,  true,  true },
    {  true, false, false },
    { false,  true, false },
    {  true,  true,  true },
    { false, false, false },
    { false, false, false },
    {  true,  true,  true },
    {  true,  true,  true },
    {  true, false, false },
    { false,  true, false },
    {  true, false, false },
    { false,  true, false }
};

void CalcluateBoundingBox(FFLBoundingBox* pDst, const FFLBoundingBox* pSrc, FFLiShapePartsType partsType)
{
    for (u32 i = 0; i < 3; i++)
        if (UPDATE_BOUNDING_BOX[partsType][i])
            UpdateBoundingBox(&(pDst[i]), pSrc);
}

FFLResult InitShape(FFLiCharModel* pModel, FFLiShapePartsType partsType, u32 index, f32 scaleX, f32 scaleY, const FFLVec3* pTranslate, bool flipX, FFLiResourceLoader* pResLoader, const FFLiCoordinate* pCoordinate)
{
    FFLiShapeType type = ConvertShapePartsTypeToShapeType(partsType);
    void** ppShapeData = &(pModel->pShapeData[type]);
    FFLDrawParam* pDrawParam = &(pModel->drawParam[type]);
    FFLBoundingBox boundingBox;

    FFLResult result = FFLiLoadShape(ppShapeData, pDrawParam, &boundingBox, pModel, partsType, index, pResLoader);
    if (result != FFL_RESULT_OK)
        return result;

#ifdef FFL_USE_ADJUST_MTX
    pDrawParam->primitiveParam.pAdjustMatrix = NULL;
#endif
    FFLiAdjustShape(pDrawParam, &boundingBox, scaleX, scaleY, pTranslate, flipX, pCoordinate, partsType, pModel->charModelDesc.modelFlag & FFL_MODEL_FLAG_FLATTEN_NOSE);
    CalcluateBoundingBox(pModel->boundingBox, &boundingBox, partsType);
    return FFL_RESULT_OK;
}

void DeleteShape(FFLiCharModel* pModel, FFLiShapePartsType partsType)
{
    FFLiShapeType type = ConvertShapePartsTypeToShapeType(partsType);
    void** ppShapeData = &(pModel->pShapeData[type]);
    FFLDrawParam* pDrawParam = &(pModel->drawParam[type]);

    FFLiDeleteShape(ppShapeData, pDrawParam);
}

void DeleteShape_Faceline(FFLiCharModel* pModel)
{
    DeleteShape(pModel, FFLI_SHAPE_PARTS_TYPE_FACELINE);
}

struct ModelTypeShapePartsInfo
{
    bool                enable;
    FFLiShapePartsType  partsType;
};

const ModelTypeShapePartsInfo* GetModelTypeShapePartsInfo(u32 modelFlag)
{
    static ModelTypeShapePartsInfo modelTypeShapePartsInfo[2 * 3] = {
        // FFL_MODEL_TYPE_NORMAL
        { false, FFLI_SHAPE_PARTS_TYPE_HAIR_NORMAL },
        { false, FFLI_SHAPE_PARTS_TYPE_HAT_NORMAL },
        { false, FFLI_SHAPE_PARTS_TYPE_FOREHEAD_NORMAL },
        // FFL_MODEL_TYPE_HAT
        { false, FFLI_SHAPE_PARTS_TYPE_HAIR_CAP },
        { false, FFLI_SHAPE_PARTS_TYPE_HAT_CAP },
        { false, FFLI_SHAPE_PARTS_TYPE_FOREHEAD_CAP }
    };

    bool modelType0Enable = modelFlag & FFL_MODEL_FLAG_NORMAL;
    bool modelType1Enable = modelFlag & FFL_MODEL_FLAG_HAT;

    modelTypeShapePartsInfo[0 * 3 + 0].enable = modelType0Enable;
    modelTypeShapePartsInfo[0 * 3 + 1].enable = modelType0Enable;
    modelTypeShapePartsInfo[0 * 3 + 2].enable = modelType0Enable;

    modelTypeShapePartsInfo[1 * 3 + 0].enable = modelType1Enable;
    modelTypeShapePartsInfo[1 * 3 + 1].enable = modelType1Enable;
    modelTypeShapePartsInfo[1 * 3 + 2].enable = modelType1Enable;

    return modelTypeShapePartsInfo;
}

void DeleteShape_Hair(FFLiCharModel* pModel, u32 count = 2 * 3)
{
    u32 modelFlag = pModel->charModelDesc.modelFlag & 7;

    if (modelFlag & (FFL_MODEL_FLAG_NORMAL |
                     FFL_MODEL_FLAG_HAT))
    {
        const ModelTypeShapePartsInfo* modelTypeShapePartsInfo = GetModelTypeShapePartsInfo(modelFlag);
        for (u32 j = count; j > 0; j--)
            DeleteShape(pModel, modelTypeShapePartsInfo[j - 1].partsType);
    }
}

void DeleteShape_Beard(FFLiCharModel* pModel)
{
    if (pModel->charInfo.parts.beardType < FFL_BEARD_SHAPE_MAX)
        DeleteShape(pModel, FFLI_SHAPE_PARTS_TYPE_BEARD);
}

void DeleteShape_Nose(FFLiCharModel* pModel)
{
    DeleteShape(pModel, FFLI_SHAPE_PARTS_TYPE_NOSE);
}

void DeleteShape_Noseline(FFLiCharModel* pModel)
{
    DeleteShape(pModel, FFLI_SHAPE_PARTS_TYPE_NOSELINE);
}

void DeleteShape_Mask(FFLiCharModel* pModel)
{
    DeleteShape(pModel, FFLI_SHAPE_PARTS_TYPE_MASK);
}

void DeleteShape_Glass(FFLiCharModel* pModel)
{
    if (pModel->charInfo.parts.glassType > 0)
        DeleteShape(pModel, FFLI_SHAPE_PARTS_TYPE_GLASS);
}

FFLResult InitShapes(FFLiCharModel* pModel, FFLiResourceLoader * pResLoader, const FFLiCoordinate* pCoordinate)
{
    u32 modelFlag = pModel->charModelDesc.modelFlag & 7;

    FFLResult result = InitShape(pModel, FFLI_SHAPE_PARTS_TYPE_FACELINE, pModel->charInfo.parts.faceType, 1.0f, 1.0f, NULL, false, pResLoader, pCoordinate);
    if (result != FFL_RESULT_OK)
        return result;

    if (modelFlag & (FFL_MODEL_FLAG_NORMAL |
                     FFL_MODEL_FLAG_HAT))
    {
        const ModelTypeShapePartsInfo* modelTypeShapePartsInfo = GetModelTypeShapePartsInfo(modelFlag);

        bool flipHair = pModel->charInfo.parts.hairDir > 0;

        for (u32 i = 0; i < 2 * 3; i++)
        {
            if (modelTypeShapePartsInfo[i].enable)
            {
                result = InitShape(pModel, modelTypeShapePartsInfo[i].partsType, pModel->charInfo.parts.hairType, 1.0f, 1.0f, &pModel->hairPos, flipHair, pResLoader, pCoordinate);
                if (result != FFL_RESULT_OK)
                {
                    DeleteShape_Hair(pModel, i);
                    DeleteShape_Faceline(pModel);
                    return result;
                }
            }
        }

    }

    if (pModel->charInfo.parts.beardType < FFL_BEARD_SHAPE_MAX)
    {
        result = InitShape(pModel, FFLI_SHAPE_PARTS_TYPE_BEARD, pModel->charInfo.parts.beardType, 1.0f, 1.0f, &pModel->beardPos, false, pResLoader, pCoordinate);
        if (result != FFL_RESULT_OK)
        {
            DeleteShape_Hair(pModel);
            DeleteShape_Faceline(pModel);
            return result;
        }
    }

    /* HACK: Skip initializing certain shapes based on expressions.
     * Ideally this would be implemented in FFLiSetExpression...
     * ... however, there would need to be a new property in
     * DrawParam that can "disable" a shape, and then re-enable it.
     * Nose: Dog/cat (49, 50, 51, 52), Blank (61, 62)
     * Mask: Blank (61, 62)
     */
    if (pModel->expression != FFL_EXPRESSION_49
        && pModel->expression != FFL_EXPRESSION_50
        && pModel->expression != FFL_EXPRESSION_51
        && pModel->expression != FFL_EXPRESSION_52
        // probably blank?
        && pModel->expression != FFL_EXPRESSION_61
        && pModel->expression != FFL_EXPRESSION_62
    )
    {
        f32 noseScale = pModel->charInfo.parts.noseScale * 0.175f + 0.4f;

        FFLVec3 nosePos = {
            .x = pModel->faceCenterPos.x,
            .y = pModel->faceCenterPos.y + (pModel->charInfo.parts.nosePositionY - 8) * -1.5f,
            .z = pModel->faceCenterPos.z
        };

        result = InitShape(pModel, FFLI_SHAPE_PARTS_TYPE_NOSE, pModel->charInfo.parts.noseType, noseScale, noseScale, &nosePos, false, pResLoader, pCoordinate);
        if (result != FFL_RESULT_OK)
        {
            DeleteShape_Beard(pModel);
            DeleteShape_Hair(pModel);
            DeleteShape_Faceline(pModel);
            return result;
        }

        result = InitShape(pModel, FFLI_SHAPE_PARTS_TYPE_NOSELINE, pModel->charInfo.parts.noseType, noseScale, noseScale, &nosePos, false, pResLoader, pCoordinate);
        if (result != FFL_RESULT_OK)
        {
            DeleteShape_Nose(pModel);
            DeleteShape_Beard(pModel);
            DeleteShape_Hair(pModel);
            DeleteShape_Faceline(pModel);
            return result;
        }
    }

    // Skip mask shape for blank expression.

    if (pModel->expression != FFL_EXPRESSION_61
        && pModel->expression != FFL_EXPRESSION_62
    )
    {
        result = InitShape(pModel, FFLI_SHAPE_PARTS_TYPE_MASK, pModel->charInfo.parts.faceType, 1.0f, 1.0f, NULL, false, pResLoader, pCoordinate);
        if (result != FFL_RESULT_OK)
        {
            DeleteShape_Noseline(pModel);
            DeleteShape_Nose(pModel);
            DeleteShape_Beard(pModel);
            DeleteShape_Hair(pModel);
            DeleteShape_Faceline(pModel);
            return result;
        }
    }

    if (pModel->charInfo.parts.glassType > 0)
    {
        f32 glassScale = pModel->charInfo.parts.glassScale * 0.15f + 0.4f;

        FFLVec3 glassPos = {
            .x = pModel->faceCenterPos.x,
            .y = pModel->faceCenterPos.y + (pModel->charInfo.parts.glassPositionY - 11) * -1.5f + 5.0f,
            .z = pModel->faceCenterPos.z + 2.0f,
        };

        result = InitShape(pModel, FFLI_SHAPE_PARTS_TYPE_GLASS, 0, glassScale, glassScale, &glassPos, false, pResLoader, pCoordinate);
        if (result != FFL_RESULT_OK)
        {
            DeleteShape_Mask(pModel);
            DeleteShape_Noseline(pModel);
            DeleteShape_Nose(pModel);
            DeleteShape_Beard(pModel);
            DeleteShape_Hair(pModel);
            DeleteShape_Faceline(pModel);
            return result;
        }
    }

    return FFL_RESULT_OK;
}

void DeleteShapes(FFLiCharModel* pModel)
{
    DeleteShape_Glass(pModel);
    DeleteShape_Mask(pModel);
    DeleteShape_Noseline(pModel);
    DeleteShape_Nose(pModel);
    DeleteShape_Beard(pModel);
    DeleteShape_Hair(pModel);
    DeleteShape_Faceline(pModel);
}

void DeleteTexture_Cap(FFLiCharModel* pModel, bool isExpand)
{
    if (FFLiCanDrawShape(&(pModel->drawParam[FFLI_SHAPE_TYPE_OPA_HAT_NORMAL])) ||
        FFLiCanDrawShape(&(pModel->drawParam[FFLI_SHAPE_TYPE_OPA_HAT_CAP])))
    {
        FFLiDeleteTexture(&pModel->pCapTexture, isExpand);
    }
}

void DeleteTexture_Noseline(FFLiCharModel* pModel, bool isExpand)
{
    if (FFLiCanDrawShape(&(pModel->drawParam[FFLI_SHAPE_TYPE_XLU_NOSELINE])))
        FFLiDeleteTexture(&pModel->pNoselineTexture, isExpand);
}

void DeleteTexture_Glass(FFLiCharModel* pModel, bool isExpand)
{
    if (pModel->charInfo.parts.glassType > 0)
        FFLiDeleteTexture(&pModel->pGlassTexture, isExpand);
}

FFLResult InitTextures(FFLiCharModel* pModel, FFLiResourceLoader* pResLoader)
{
    if (FFLiCanDrawShape(&(pModel->drawParam[FFLI_SHAPE_TYPE_OPA_HAT_NORMAL])) ||
        FFLiCanDrawShape(&(pModel->drawParam[FFLI_SHAPE_TYPE_OPA_HAT_CAP])))
    {
        FFLResult result = FFLiLoadTextureWithAllocate(&pModel->pCapTexture, FFLI_TEXTURE_PARTS_TYPE_CAP, pModel->charInfo.parts.hairType, pResLoader);
        if (result != FFL_RESULT_OK)
            return result;
    }

    if (FFLiCanDrawShape(&(pModel->drawParam[FFLI_SHAPE_TYPE_XLU_NOSELINE])))
    {
        FFLResult result = FFLiLoadTextureWithAllocate(&pModel->pNoselineTexture, FFLI_TEXTURE_PARTS_TYPE_NOSELINE, pModel->charInfo.parts.noseType, pResLoader);
        if (result != FFL_RESULT_OK)
        {
            DeleteTexture_Cap(pModel, pResLoader->IsExpand());
            return result;
        }
    }

    if (pModel->charInfo.parts.glassType > 0)
    {
        FFLResult result = FFLiLoadTextureWithAllocate(&pModel->pGlassTexture, FFLI_TEXTURE_PARTS_TYPE_GLASS, pModel->charInfo.parts.glassType, pResLoader);
        if (result != FFL_RESULT_OK)
        {
            DeleteTexture_Noseline(pModel, pResLoader->IsExpand());
            DeleteTexture_Cap(pModel, pResLoader->IsExpand());
            return result;
        }
    }

    return FFL_RESULT_OK;
}

void DeleteTextures(FFLiCharModel* pModel)
{
    RIO_ASSERT(FFLiManager::IsConstruct());
    bool isExpand = FFLiManager::GetInstance()->GetResourceManager().IsExpand(pModel->charModelDesc.resourceType);

    DeleteTexture_Glass(pModel, isExpand);
    DeleteTexture_Noseline(pModel, isExpand);
    DeleteTexture_Cap(pModel, isExpand);
}

void AddVec3(FFLVec3* pDst, FFLVec3 vec)
{
    pDst->x += vec.x;
    pDst->y += vec.y;
    pDst->z += vec.z;
}

void AdjustPartsTransform(FFLiCharModel* pModel, const FFLiCoordinate* pCoordinate)
{
    AddVec3(&pModel->partsTransform.headFrontTranslate, pModel->hairPos);
    AddVec3(&pModel->partsTransform.headSideTranslate, pModel->hairPos);
    AddVec3(&pModel->partsTransform.headTopTranslate, pModel->hairPos);

    pCoordinate->Transform              (&pModel->partsTransform.hatTranslate);
    pCoordinate->TransformWithoutScale  (&pModel->partsTransform.headFrontRotate);
    pCoordinate->Transform              (&pModel->partsTransform.headFrontTranslate);
    pCoordinate->TransformWithoutScale  (&pModel->partsTransform.headSideRotate);
    pCoordinate->Transform              (&pModel->partsTransform.headSideTranslate);
    pCoordinate->TransformWithoutScale  (&pModel->partsTransform.headTopRotate);
    pCoordinate->Transform              (&pModel->partsTransform.headTopTranslate);
}

struct FFLiShapeTypeInfo
{
    FFLiShapeType   hairIndex;
    FFLiShapeType   foreheadIndex;
    FFLiShapeType   capIndex;
};

static const FFLiShapeTypeInfo SHAPE_TYPE_INFO_NORMAL = {
    FFLI_SHAPE_TYPE_OPA_HAIR_NORMAL,
    FFLI_SHAPE_TYPE_OPA_FOREHEAD_NORMAL,
    FFLI_SHAPE_TYPE_OPA_HAT_NORMAL
};

static const FFLiShapeTypeInfo SHAPE_TYPE_INFO_HAT = {
    FFLI_SHAPE_TYPE_OPA_HAIR_CAP,
    FFLI_SHAPE_TYPE_OPA_FOREHEAD_CAP,
    FFLI_SHAPE_TYPE_OPA_HAT_CAP
};

const FFLiShapeTypeInfo& GetShapeTypeInfo(FFLModelType type)
{
    switch (type)
    {
    case FFL_MODEL_TYPE_NORMAL:
        return SHAPE_TYPE_INFO_NORMAL;
    case FFL_MODEL_TYPE_HAT:
        return SHAPE_TYPE_INFO_HAT;
    default:
        return SHAPE_TYPE_INFO_NORMAL;
    }
}

static const FFLModelType MODEL_TYPE[2] = {
    FFL_MODEL_TYPE_NORMAL,
    FFL_MODEL_TYPE_HAT
};

void SetupDrawParam(FFLiCharModel* pModel)
{
    FFLCullMode hairCullMode = FFL_CULL_MODE_BACK;

    pModel->drawParam[FFLI_SHAPE_TYPE_OPA_FACELINE].cullMode = FFL_CULL_MODE_BACK;

    FFLiInitModulateShapeFaceline(&pModel->drawParam[FFLI_SHAPE_TYPE_OPA_FACELINE].modulateParam, pModel->charInfo.parts.facelineColor, pModel->facelineRenderTexture.pTexture2D);

    pModel->drawParam[FFLI_SHAPE_TYPE_OPA_BEARD].cullMode = FFL_CULL_MODE_BACK;
    FFLiInitModulateShapeBeard(&pModel->drawParam[FFLI_SHAPE_TYPE_OPA_BEARD].modulateParam, pModel->charInfo.parts.beardColor);

    pModel->drawParam[FFLI_SHAPE_TYPE_OPA_NOSE].cullMode = FFL_CULL_MODE_BACK;
    FFLiInitModulateShapeNose(&pModel->drawParam[FFLI_SHAPE_TYPE_OPA_NOSE].modulateParam, pModel->charInfo.parts.facelineColor);

    if (pModel->charInfo.parts.hairDir > 0 && g_FrontCullForFlipX)
        hairCullMode = FFL_CULL_MODE_FRONT;

    for (u32 i = 0; i < 2; i++)
    {
        if (pModel->charModelDesc.modelFlag & 1 << MODEL_TYPE[i])
        {
            const FFLiShapeTypeInfo& shapeTypeInfo = GetShapeTypeInfo(MODEL_TYPE[i]);

            FFLDrawParam& drawParamForehead = pModel->drawParam[shapeTypeInfo.foreheadIndex];
            drawParamForehead.cullMode = hairCullMode;
            FFLiInitModulateShapeForehead(&drawParamForehead.modulateParam, pModel->charInfo.parts.facelineColor);

            FFLDrawParam& drawParamHair = pModel->drawParam[shapeTypeInfo.hairIndex];
            drawParamHair.cullMode = hairCullMode;
            FFLiInitModulateShapeHair(&drawParamHair.modulateParam, pModel->charInfo.parts.hairColor);

            const FFLTexture* pCapTexture = pModel->pCapTexture;
            if (pCapTexture != NULL)
            {
                FFLDrawParam& drawParamCap = pModel->drawParam[shapeTypeInfo.capIndex];
                drawParamCap.cullMode = hairCullMode;
                FFLiInitModulateShapeCap(&drawParamCap.modulateParam, pModel->charInfo.favoriteColor, pCapTexture);
            }
        }
    }

    RIO_ASSERT(pModel->expression < FFL_EXPRESSION_LIMIT); // set by FFLiInitMaskTextures's return value
    // not sure what to do if this fails
    if (pModel->expression < FFL_EXPRESSION_LIMIT)
    {
        const FFLiRenderTexture* pMaskRenderTexture = pModel->maskTextures.pRenderTextures[pModel->expression];
        if (pMaskRenderTexture != NULL)
        {
            pModel->drawParam[FFLI_SHAPE_TYPE_XLU_MASK].cullMode = FFL_CULL_MODE_BACK;
            FFLiInitModulateShapeMask(&pModel->drawParam[FFLI_SHAPE_TYPE_XLU_MASK].modulateParam,
#ifndef FFL_NO_RENDER_TEXTURE
                pMaskRenderTexture->pTexture2D);
#else
                // do not dereference pMaskRenderTexture
                // but still indicate this slot is active
                FFLI_RENDER_TEXTURE_PLACEHOLDER);
#endif
        }
    }

    const FFLTexture* pNoselineTexture = pModel->pNoselineTexture;
    if (pNoselineTexture != NULL)
    {
        pModel->drawParam[FFLI_SHAPE_TYPE_XLU_NOSELINE].cullMode = FFL_CULL_MODE_BACK;
        FFLiInitModulateShapeNoseline(&pModel->drawParam[FFLI_SHAPE_TYPE_XLU_NOSELINE].modulateParam, pNoselineTexture);
    }

    const FFLTexture* pGlassTexture = pModel->pGlassTexture;
    if (pGlassTexture != NULL)
    {
        pModel->drawParam[FFLI_SHAPE_TYPE_XLU_GLASS].cullMode = FFL_CULL_MODE_NONE;
        FFLiInitModulateShapeGlass(&pModel->drawParam[FFLI_SHAPE_TYPE_XLU_GLASS].modulateParam, pModel->charInfo.parts.glassColor, pGlassTexture);
    }
}

}

void FFLiCharModelCreator::InvalidateShapes(FFLiCharModel* pModel)
{
    for (u32 i = 0; i < FFLI_SHAPE_TYPE_MAX; i++)
        FFLiInvalidateShape(&(pModel->drawParam[i]));
}

namespace {

#if RIO_IS_CAFE

void InvalidateTexture(const GX2Texture& texture)
{
    if (texture.surface.image)
        GX2Invalidate(GX2_INVALIDATE_MODE_TEXTURE, texture.surface.image, texture.surface.imageSize);

    if (texture.surface.mipmaps)
        GX2Invalidate(GX2_INVALIDATE_MODE_TEXTURE, texture.surface.mipmaps, texture.surface.mipmapSize);
}

#endif // RIO_IS_CAFE

void InvalidateTextures(FFLiCharModel* pModel)
{
#if RIO_IS_CAFE
    if (pModel->pCapTexture != NULL)
        InvalidateTexture(pModel->pCapTexture->getNativeTexture());

    if (pModel->pNoselineTexture != NULL)
        InvalidateTexture(pModel->pNoselineTexture->getNativeTexture());

    if (pModel->pGlassTexture != NULL)
        InvalidateTexture(pModel->pGlassTexture->getNativeTexture());
#endif // RIO_IS_CAFE
}

}

void FFLiCharModelCreator::AfterExecuteGPUStep(FFLiCharModel* pModel)
{
    InvalidateShapes(pModel);
    InvalidateTextures(pModel);
#ifndef RIO_NO_GLFW_CALLS
    rio::Window::instance()->makeContextCurrent();

    u32 width = rio::Window::instance()->getWidth();
    u32 height = rio::Window::instance()->getHeight();

    rio::Graphics::setViewport(0, 0, width, height);
    rio::Graphics::setScissor(0, 0, width, height);
#endif // RIO_NO_GLFW_CALLS
#ifndef FFL_NO_RENDER_TEXTURE
    #if RIO_IS_CAFE
        GX2DrawDone();
    #elif RIO_IS_WIN
        RIO_GL_CALL(glFinish());
    #endif
#endif // FFL_NO_RENDER_TEXTURE
}
