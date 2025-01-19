#include <nn/ffl/FFLiCharModel.h>
#include <nn/ffl/FFLiCharModelCreator.h>
#include <nn/ffl/FFLiFacelineTexture.h>
#include <nn/ffl/FFLiManager.h>
#include <nn/ffl/FFLiModulate.h>
#include <nn/ffl/FFLiTextureTempObject.h>

#include <misc/rio_MemUtil.h>

FFLResult FFLiInitCharModelCPUStep(FFLiCharModel* pModel, const FFLCharModelSource* pSource, const FFLCharModelDesc* pDesc
#ifdef FFL_USE_TEXTURE_CALLBACK
    , const FFLTextureCallback* pCallback)
#else
    )
#endif
{
    if (!FFLiManager::IsConstruct())
        return FFL_RESULT_MANAGER_NOT_CONSTRUCT;

    FFLiManager* pManager = FFLiManager::GetInstance();
    RIO_ASSERT(pManager->CanInitCharModel());
    if (!pManager->CanInitCharModel())
        return FFL_RESULT_ERROR;

    FFLiCharModelCreator creator(&pManager->GetCharModelCreateParam(), pManager);
    return creator.ExecuteCPUStep(pModel, pSource, pDesc
#ifdef FFL_USE_TEXTURE_CALLBACK
        , pCallback);
#else
    );
#endif
}

void FFLiInitCharModelGPUStep(FFLiCharModel* pModel, const FFLShaderCallback* pCallback)
{
    if (!FFLiManager::IsConstruct())
        return;

    FFLiManager* pManager = FFLiManager::GetInstance();
    FFLiCharModelCreator creator(&pManager->GetCharModelCreateParam(), pManager);
    return creator.ExecuteGPUStep(pModel, pCallback);
}

void FFLiDeleteCharModel(FFLiCharModel* pModel)
{
    FFLiCharModelCreator::Delete(pModel);
}

void FFLiSetExpression(FFLiCharModel* pModel, FFLExpression expression)
{
    if (FFLiIsAvailableExpression(pModel, expression))
    //if (1 << expression & pModel->charModelDesc.expressionFlag)
    {
        pModel->expression = expression;
#ifndef FFL_NO_RENDER_TEXTURE
        FFLiInitModulateShapeMask(&pModel->drawParam[FFLI_SHAPE_TYPE_XLU_MASK].modulateParam, pModel->maskTextures.pRenderTextures[expression]->pTexture2D);
#endif // FFL_NO_RENDER_TEXTURE
    }
}

FFLExpression FFLiGetExpression(const FFLiCharModel* pModel)
{
    return pModel->expression;
}

void FFLiGetPartsTransform(FFLPartsTransform* pTransform, const FFLiCharModel* pModel)
{
    *pTransform = pModel->partsTransform;
}

void FFLiSetViewModelType(FFLiCharModel* pModel, FFLModelType type)
{
    if (type < FFL_MODEL_TYPE_MAX &&
        (pModel->charModelDesc).modelFlag & 1 << type)
    {
        pModel->modelType = type;
    }
}

const FFLDrawParam* FFLiGetDrawParamOpaFacelineFromCharModel(const FFLiCharModel* pModel)
{
    return &(pModel->drawParam[FFLI_SHAPE_TYPE_OPA_FACELINE]);
}

const FFLDrawParam* FFLiGetDrawParamOpaBeardFromCharModel(const FFLiCharModel* pModel)
{
    return &(pModel->drawParam[FFLI_SHAPE_TYPE_OPA_BEARD]);
}

const FFLDrawParam* FFLiGetDrawParamOpaNoseFromCharModel(const FFLiCharModel* pModel)
{
    return &(pModel->drawParam[FFLI_SHAPE_TYPE_OPA_NOSE]);
}

namespace {

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

}

const FFLDrawParam* FFLiGetDrawParamOpaForeheadFromCharModel(const FFLiCharModel* pModel)
{
    const FFLDrawParam* pDrawParam = NULL;
    if (pModel->modelType != FFL_MODEL_TYPE_FACE_ONLY)
    {
        const FFLDrawParam* drawParam = pModel->drawParam;
        pDrawParam = &(drawParam[GetShapeTypeInfo(pModel->modelType).foreheadIndex]);
    }
    return pDrawParam;
}

const FFLDrawParam* FFLiGetDrawParamOpaHairFromCharModel(const FFLiCharModel* pModel)
{
    const FFLDrawParam* pDrawParam = NULL;
    if (pModel->modelType != FFL_MODEL_TYPE_FACE_ONLY)
    {
        const FFLDrawParam* drawParam = pModel->drawParam;
        pDrawParam = &(drawParam[GetShapeTypeInfo(pModel->modelType).hairIndex]);
    }
    return pDrawParam;
}

const FFLDrawParam* FFLiGetDrawParamOpaCapFromCharModel(const FFLiCharModel* pModel)
{
    if (pModel->modelType == FFL_MODEL_TYPE_FACE_ONLY || pModel->pCapTexture == NULL)
        return NULL;

    const FFLDrawParam* drawParam = pModel->drawParam;
    return &(drawParam[GetShapeTypeInfo(pModel->modelType).capIndex]);
}

const FFLDrawParam* FFLiGetDrawParamXluMaskFromCharModel(const FFLiCharModel* pModel)
{
    return &(pModel->drawParam[FFLI_SHAPE_TYPE_XLU_MASK]);
}

const FFLDrawParam* FFLiGetDrawParamXluNoseLineFromCharModel(const FFLiCharModel* pModel)
{
    return
        pModel->pNoselineTexture == NULL
            ? NULL
            : &(pModel->drawParam[FFLI_SHAPE_TYPE_XLU_NOSELINE]);
}

const FFLDrawParam* FFLiGetDrawParamXluGlassFromCharModel(const FFLiCharModel* pModel)
{
    return
        pModel->pGlassTexture == NULL
            ? NULL
            : &(pModel->drawParam[FFLI_SHAPE_TYPE_XLU_GLASS]);
}


const FFLiRenderTexture* FFLiGetFaceTextureFromCharModel(const FFLiCharModel* pModel)
{
    return &pModel->facelineRenderTexture;
}

const FFLiRenderTexture* FFLiGetMaskTextureFromCharModel(const FFLiCharModel* pModel, FFLExpression expression)
{
    return pModel->maskTextures.pRenderTextures[expression];
}

void FFLiGetCharInfoFromCharModel(FFLiCharInfo* pCharInfo, const FFLiCharModel* pModel)
{
    rio::MemUtil::copy(pCharInfo, &pModel->charInfo, sizeof(FFLiCharInfo));
}
bool FFLiIsAvailableExpression(const FFLiCharModel* pModel, FFLExpression expression)
{
    if (pModel != NULL && expression < FFL_EXPRESSION_MAX)
        return pModel->maskTextures.pRenderTextures[expression] != NULL;
    return false;
}

void FFLiSetCoordinat(FFLCoordinateType upType, FFLCoordinateType frontType)
{
    if (!FFLiManager::IsConstruct())
        return;

    FFLiManager* pManager = FFLiManager::GetInstance();

    FFLiCharModelCreateParam& createParam = pManager->GetCharModelCreateParam();

    return createParam.Set(upType, frontType);
}

void FFLiSetScale(f32 scale)
{
    if (!FFLiManager::IsConstruct())
        return;

    FFLiManager* pManager = FFLiManager::GetInstance();

    FFLiCharModelCreateParam& createParam = pManager->GetCharModelCreateParam();

    return createParam.SetScale(scale);
}
