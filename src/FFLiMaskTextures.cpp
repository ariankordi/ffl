#include <nn/ffl/FFLColor.h>

#include <nn/ffl/FFLiColor.h>
#include <nn/ffl/FFLiMaskTextures.h>
#include <nn/ffl/FFLiMaskTexturesTempObject.h>
#include <nn/ffl/FFLiMipMapUtil.h>
#include <nn/ffl/FFLiPartsTextures.h>
#include <nn/ffl/FFLiRawMask.h>
#include <nn/ffl/FFLiRenderTexture.h>
#include <nn/ffl/FFLiShaderCallback.h>
#include <nn/ffl/FFLiTexture.h>
#include <nn/ffl/FFLiUtil.h>

#include <nn/ffl/detail/FFLiCharInfo.h>
#include <nn/ffl/detail/FFLiCopySurface.h>

#include <misc/rio_MemUtil.h>

namespace {

rio::TextureFormat GetTextureFormat(bool useOffScreenSrgbFetch);

FFLiRenderTexture* FFLiRenderTextureAllocate();
void FFLiRenderTextureDelete(FFLiRenderTexture* pRenderTexture);

bool CanUseExpression(u64 expressionFlag, FFLExpression expression);

void InitRawMask(FFLiMaskTexturesTempObject* pObject, u64 expressionFlag);
void DeleteRawMask(FFLiMaskTexturesTempObject* pObject, u64 expressionFlag);

void SetupExpressionCharInfo(FFLiCharInfo* pExpressionCharInfo, const FFLiCharInfo* pCharInfo, FFLExpression expression);

}

FFLExpression FFLiInitMaskTextures(FFLiMaskTextures* pMaskTextures, u64 expressionFlag, u32 resolution, bool enableMipMap)
{
    FFLExpression expression = FFL_EXPRESSION_MAX;

    u32 numMips = enableMipMap ? FFLiGetMipMapNum(resolution, resolution) : 1;

    for (u32 i = 0; i < FFL_EXPRESSION_MAX; i++)
    {
        if ((expressionFlag & static_cast<u64>(1) << i) == 0)
        {
            pMaskTextures->pRenderTextures[i] = NULL;
            continue;
        }

        if (expression == FFL_EXPRESSION_MAX)
            expression = FFLExpression(i);

        pMaskTextures->pRenderTextures[i] = FFLiRenderTextureAllocate();
        rio::TextureFormat format = GetTextureFormat(FFLiUseOffScreenSrgbFetch());
        FFLiInitRenderTexture(pMaskTextures->pRenderTextures[i], resolution, resolution, format, numMips);
    }

    return expression;
}

void FFLiDeleteMaskTextures(FFLiMaskTextures* pMaskTextures)
{
    for (u32 j = FFL_EXPRESSION_MAX; j > 0; j--)
    {
        if (pMaskTextures->pRenderTextures[j - 1] != NULL)
        {
            FFLiDeleteRenderTexture(pMaskTextures->pRenderTextures[j - 1]);
            FFLiRenderTextureDelete(pMaskTextures->pRenderTextures[j - 1]);
            pMaskTextures->pRenderTextures[j - 1] = NULL;
        }
    }
}

FFLResult FFLiInitTempObjectMaskTextures(FFLiMaskTexturesTempObject* pObject, const FFLiMaskTextures* pMaskTextures, const FFLiCharInfo* pCharInfo, u64 expressionFlag, u32 resolution, bool enableMipMap, FFLiResourceLoader* pResLoader)
{
    rio::MemUtil::set(pObject, 0, sizeof(FFLiMaskTexturesTempObject));

    FFLResult result = FFLiLoadPartsTextures(&pObject->partsTextures, pCharInfo, expressionFlag, pResLoader);
    if (result != FFL_RESULT_OK)
        return result;

    InitRawMask(pObject, expressionFlag);

    for (u32 i = 0; i < FFL_EXPRESSION_MAX; i++)
    {
        if (CanUseExpression(expressionFlag, FFLExpression(i)))
        {
            FFLiRawMaskTextureDesc desc;
            FFLiCharInfo expressionCharInfo = *pCharInfo;
            const FFLiEyeMouthTypeElement& element = FFLiGetEyeMouthTypeElement(FFLExpression(i));

            SetupExpressionCharInfo(&expressionCharInfo, pCharInfo, FFLExpression(i));

            desc.pTexturesEye[0] = pObject->partsTextures.pTexturesEye[element.eyeTextureType[0]];
            desc.pTexturesEye[1] = pObject->partsTextures.pTexturesEye[element.eyeTextureType[1]];

            desc.pTexturesEyebrow[0] = pObject->partsTextures.pTextureEyebrow;
            desc.pTexturesEyebrow[1] = pObject->partsTextures.pTextureEyebrow;

            desc.pTextureMouth = pObject->partsTextures.pTexturesMouth[element.mouthTextureType];

            desc.pTexturesMustache[0] = pObject->partsTextures.pTextureMustache;
            desc.pTexturesMustache[1] = pObject->partsTextures.pTextureMustache;

            desc.pTextureMole = pObject->partsTextures.pTextureMole;

            FFLiInitDrawParamRawMask(
                pObject->pRawMaskDrawParam[i],
                &expressionCharInfo,
                resolution,
                FFLiCharInfoAndTypeToEyeIndex(pCharInfo, element.eyeTextureType[0]),
                FFLiCharInfoAndTypeToEyeIndex(pCharInfo, element.eyeTextureType[1]),
                &desc
            );
        }
    }

    return FFL_RESULT_OK;
}

void FFLiDeleteTempObjectMaskTextures(FFLiMaskTexturesTempObject* pObject, u64 expressionFlag, FFLResourceType resourceType)
{
    for (u32 j = FFL_EXPRESSION_MAX; j > 0; j--)
        if (CanUseExpression(expressionFlag, FFLExpression(j - 1)))
            FFLiDeleteDrawParamRawMask(pObject->pRawMaskDrawParam[j - 1]);

    DeleteRawMask(pObject, expressionFlag);

    FFLiDeletePartsTextures(&pObject->partsTextures, expressionFlag, resourceType);
}

void FFLiRenderMaskTextures(FFLiMaskTextures* pMaskTextures, FFLiMaskTexturesTempObject* pObject, const FFLiShaderCallback* pCallback
#if RIO_IS_CAFE
, FFLiCopySurface* pCopySurface
#endif // RIO_IS_CAFE
)
{
    static const FFLColor BLACK = { };

    FFLiInvalidatePartsTextures(&pObject->partsTextures);

    for (u32 i = 0; i < FFL_EXPRESSION_MAX; i++)
    {
        if (pMaskTextures->pRenderTextures[i] != NULL && pObject->pRawMaskDrawParam[i] != NULL)
        {
            FFLiInvalidateRawMask(pObject->pRawMaskDrawParam[i]);

            FFLiRenderTexture& renderTexture = *(pMaskTextures->pRenderTextures[i]);

            FFLiInvalidateRenderTexture(&renderTexture);
            RIO_ASSERT(renderTexture.pTexture2D->getTextureFormat() == rio::TEXTURE_FORMAT_R8_G8_B8_A8_UNORM);
            FFLiSetupRenderTexture(&renderTexture, &BLACK, NULL, 0, pCallback);

            FFLiDrawRawMask(pObject->pRawMaskDrawParam[i], pCallback);

            if (renderTexture.pTexture2D->getNumMips() > 1)
            {
#if RIO_IS_WIN
                RIO_GL_CALL(glBindTexture(GL_TEXTURE_2D, renderTexture.pTexture2D->getNativeTextureHandle()));
                RIO_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
#elif RIO_IS_CAFE
                pCopySurface->Begin();

                GX2Surface* pSurface = const_cast<GX2Surface*>(&renderTexture.pTexture2D->getNativeTexture().surface);

                for (u32 i = 1; i < renderTexture.pTexture2D->getNumMips(); i++)
                    pCopySurface->Execute(pSurface, i, pSurface, i - 1);

                pCopySurface->End();
#endif
            }

            FFLiFlushRenderTexture(&renderTexture);
        }
    }
}

namespace {

rio::TextureFormat GetTextureFormat(bool useOffScreenSrgbFetch)
{
    if (useOffScreenSrgbFetch)
        return rio::TEXTURE_FORMAT_R8_G8_B8_A8_SRGB;

    else
        return rio::TEXTURE_FORMAT_R8_G8_B8_A8_UNORM;
}

FFLiRenderTexture* FFLiRenderTextureAllocate()
{
    return new FFLiRenderTexture;
}

void FFLiRenderTextureDelete(FFLiRenderTexture* pRenderTexture)
{
    delete pRenderTexture;
}

bool CanUseExpression(u64 expressionFlag, FFLExpression expression)
{
    return (expressionFlag & static_cast<u64>(1) << expression) != 0;
}

void InitRawMask(FFLiMaskTexturesTempObject* pObject, u64 expressionFlag)
{
    for (u32 i = 0; i < FFL_EXPRESSION_MAX; i++)
        if (CanUseExpression(expressionFlag, FFLExpression(i)))
            pObject->pRawMaskDrawParam[i] = new FFLiRawMaskDrawParam;
}

void DeleteRawMask(FFLiMaskTexturesTempObject* pObject, u64 expressionFlag)
{
    for (u32 j = FFL_EXPRESSION_MAX; j > 0; j--)
        if (CanUseExpression(expressionFlag, FFLExpression(j - 1)))
            delete pObject->pRawMaskDrawParam[j - 1];
}

struct CorrectParam
{
    s32 eyeType;
    s32 mouthType;
    s32 eyeRotateOffset;
    s32 eyebrowRotateOffset;
    s32 eyebrowPositionY;
};

static const CorrectParam CORRECT_PARAM[FFL_EXPRESSION_MAX] = {
    { -1, -1,  0,  0,  0 },
    { 60, -1,  0,  0,  0 },
    { -1, 10,  2,  2,  0 },
    { -1, 12, -2, -2,  0 },
    { 61, -1,  0,  0, -2 },
    { 26, -1,  0,  0,  0 },
    { -1, 36,  0,  0,  0 },
    { 60, 36,  0,  0,  0 },
    { -1, 36,  2,  2,  0 },
    { -1, 36, -2, -2,  0 },
    { 61, 36,  0,  0, -2 },
    { 26, 36,  0,  0,  0 },
    { 47, -1,  0,  0,  0 },
    { 47, -1,  0,  0,  0 },
    { 47, 36,  0,  0,  0 },
    { 47, 36,  0,  0,  0 },
    { 47, -1,  0,  0,  0 },
    { 47, -1,  0,  0,  0 },
    { 47, 12,  0,  0,  0 },

    // miitomo

    { 64, 23, 0, 0, 0 },
    { 64, 36, 0, 0, 0 },
    { 26, 23, -2, -2, -2 },
    { 26, 45, -2, -2, -2 },
    { 47, 23, 2, 2, 0 },
    { 47, 41, 2, 2, 0 },
    { 62, 23, 0, 0, -1 },
    { 62, 38, 0, 0, -1 },
    { -1, 23, 2, 2, 0 },
    { -1, 37, 2, 2, 0 },
    { 47, 23, 0, -2, -3 },
    { 47, 45, 0, -2, -3 },
    { 60, 23, 0, 0, 0 },
    { 60, 38, 0, 0, 0 },
    { 75, 44, 0, 0, 0 },
    { 75, 44, 0, 0, 0 },
    { 65, 39, 0, 1, 0 },
    { 65, 40, 0, 1, 0 },
    { 64, 15, 0, 2, 0 },
    { 64, 36, 0, 2, 0 },
    { 69, 42, 0, 0, 2 },
    { 69, 43, 0, 0, 2 },
    { 63, 46, 0, 2, 0 },
    { 63, 46, 0, 2, 0 },
    { 66, 14, 0, 2, 1 },
    { 66, 14, 0, 2, 1 },
    { 47, 47, 1, 0, 0 },
    { 47, 47, 1, 0, 0 },
    { 74, 32, 0, 0, 0 },
    { 74, 32, 0, 0, 0 },
    { 67, 48, 0, 0, -1 },
    { 67, 48, 0, 0, -1 },
    { 68, 49, 0, 0, 0 },
    { 68, 49, 0, 0, 0 },
    { -1, 50, 0, 0, 0 },
    { -1, 50, 0, 0, 0 },
    { 72, 23, 0, -2, -3 },
    { 72, 38, 0, -2, -3 },
    { 70, 23, 0, -2, -2 },
    { 70, 45, 0, -2, -2 },
    { 23, 23, 0, -2, 0 },
    { 23, 38, 0, -2, 0 },
    { -1, -1, 0, 0, 0 },
    { -1, -1, 0, 0, 0 },
    { -1, 23, 0, -1, -2 },
    { -1, 46, 0, -1, -2 },
    { 76, 23, 0, -2, -3 },
    { 76, 38, 0, -2, -3 },
    { 79, 51, 0, 0, 0 },
    { 79, 51, 0, 0, 0 },
    { 78, 2, 0, 0, 0 },
};

const CorrectParam& GetCorrectParam(FFLExpression expression)
{
    return CORRECT_PARAM[expression];
}

void SetupExpressionCharInfo(FFLiCharInfo* pExpressionCharInfo, const FFLiCharInfo*, FFLExpression expression)
{
    const CorrectParam& param = GetCorrectParam(expression);

    if (param.mouthType >= 0)
        pExpressionCharInfo->parts.mouthType = param.mouthType;

    s32 eyeRotateOffset = param.eyeRotateOffset;
    if (param.eyeType >= 0 && param.eyeType != pExpressionCharInfo->parts.eyeType)
    {
        eyeRotateOffset += FFLiiGetEyeRotateOffset(pExpressionCharInfo->parts.eyeType);
        eyeRotateOffset -= FFLiiGetEyeRotateOffset(param.eyeType);
    }

    if (eyeRotateOffset != 0)
    {
        // pExpressionCharInfo->parts.eyeRotate = clamp(pExpressionCharInfo->parts.eyeRotate + eyeRotateOffset, 0, 7);
        s32 eyeRotate = pExpressionCharInfo->parts.eyeRotate + eyeRotateOffset;
        if (eyeRotate < 0)
            eyeRotate = 0;
        else if (eyeRotate > 7)
            eyeRotate = 7;
        pExpressionCharInfo->parts.eyeRotate = eyeRotate;
    }

    pExpressionCharInfo->parts.eyebrowPositionY += param.eyebrowPositionY;

    s32 eyebrowRotateOffset = param.eyebrowRotateOffset;
    if (eyebrowRotateOffset != 0)
    {
        // pExpressionCharInfo->parts.eyebrowRotate = clamp(pExpressionCharInfo->parts.eyebrowRotate + eyebrowRotateOffset, 0, 11);
        s32 eyebrowRotate = pExpressionCharInfo->parts.eyebrowRotate + eyebrowRotateOffset;
        if (eyebrowRotate < 0)
            eyebrowRotate = 0;
        else if (eyebrowRotate > 11)
            eyebrowRotate = 11;
        pExpressionCharInfo->parts.eyebrowRotate = eyebrowRotate;
    }
}

}
