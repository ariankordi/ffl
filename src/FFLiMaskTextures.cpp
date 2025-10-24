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

#include <gpu/rio_RenderState.h>

namespace {

#ifndef FFL_NO_RENDER_TEXTURE

rio::TextureFormat GetTextureFormat(bool useOffScreenSrgbFetch);

FFLiRenderTexture* FFLiRenderTextureAllocate();

void FFLiRenderTextureDelete(FFLiRenderTexture* pRenderTexture);

#endif // FFL_NO_RENDER_TEXTURE

void SetupExpressionCharInfo(FFLiCharInfo* pExpressionCharInfo, const FFLiCharInfo* pCharInfo, FFLExpression expression);

// Inline function to find the index of the least significant set bit
inline s32 FindNextSetBit(u32* v)
{
    if (*v == 0) return -1; // No set bits
#if defined(__GNUC__) || defined(__clang__)
        s32 bitIndex = __builtin_ctz(*v);
//#pragma message("using __builtin__ctz for mask FindNextSetBit")
#elif defined(_MSC_VER)
        unsigned long bitIndex;
        _BitScanForward(&bitIndex, *v);
#else
//#pragma message("using de bruijn for mask FindNextSetBit")
    s32 bitIndex;

    static const s32 MultiplyDeBruijnBitPosition[32] =
    {
        0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
        31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
    };

    /* The De Bruijn bit-scan was devised in 1997, according to Donald Knuth
     * by Martin Lauter. The constant 0x077CB531UL is a De Bruijn sequence,
     * which produces a unique pattern of bits into the high 5 bits for each
     * possible bit position that it is multiplied against.
     * See http://graphics.stanford.edu/~seander/bithacks.html
     * and http://chessprogramming.wikispaces.com/BitScan */

    const u32 val = *v;
    bitIndex = MultiplyDeBruijnBitPosition[((u32)((val & (~val + 1)) * 0x077CB531UL)) >> 27];
#endif
    *v &= *v - 1; // Clear the least significant set bit
    return static_cast<s32>(bitIndex);
}

}

#include <nn/ffl/FFLTexture.h> // FFL_GET_RIO_NATIVE_TEXTURE_HANDLE

FFLExpression FFLiInitMaskTextures(FFLiMaskTextures* pMaskTextures, FFLAllExpressionFlag expressionFlag, u32 resolution, bool enableMipMap)
{
    FFLExpression expression = FFL_EXPRESSION_NORMAL; // will be set to final expression
    bool firstExpression = true;
#ifndef FFL_NO_RENDER_TEXTURE
    u32 numMips = enableMipMap ? FFLiGetMipMapNum(resolution, resolution) : 1;
#endif

/*
    for (u32 i = 0; expressionFlag != 0; i++, expressionFlag >>= 1)
    {
        //RIO_LOG("FFLiInitMaskTextures iteration: %i\n", i);

        if ((expressionFlag & 1) == 0)
        {
            pMaskTextures->pRenderTextures[i] = NULL;
            continue;
        }
*/
/*
    for (u32 i = 0; i < FFL_EXPRESSION_LIMIT; i++)
    {
        if ((expressionFlag & static_cast<FFLAllExpressionFlag>(1) << i) == 0)
        {
            pMaskTextures->pRenderTextures[i] = NULL;
            continue;
        }
*/
    // initialize all render texture pointers to null, disable all masks by default
    rio::MemUtil::set(pMaskTextures->pRenderTextures, 0, sizeof(FFLiMaskTextures)); // only thing in here is pRenderTextures

    for (s32 fi = 0; fi < 3; ++fi) // 3 = Amount of u32s in FFLAllExpressionFlag
    {
        u32 word = expressionFlag.flags[fi]; // flag index
        u32 baseIndex = fi * 32;
        while (word)
        {
            s32 bitIndex = FindNextSetBit(&word);
            s32 i = baseIndex + bitIndex;
            //RIO_LOG("FFLiInitMaskTextures passing: %i\n", i);

            if (i >= FFL_EXPRESSION_LIMIT) // outside range?
                break;

            if (firstExpression) // set expression to first one
            {
                firstExpression = false;
                expression = FFLExpression(i);
            }

            // ... but FFLiCharModelCreateParam::CheckModelDesc makes sure of the above

#ifndef FFL_NO_RENDER_TEXTURE
            pMaskTextures->pRenderTextures[i] = FFLiRenderTextureAllocate();
            rio::TextureFormat format = GetTextureFormat(FFLiUseOffScreenSrgbFetch());
            FFLiInitRenderTexture(pMaskTextures->pRenderTextures[i], resolution, resolution, format, numMips);
#else // FFL_NO_RENDER_TEXTURE
            // HACK used for faceline texture but also here to indicate
            // just that this mask is active, used by FFLIsAvailableExpression
            pMaskTextures->pRenderTextures[i] = FFLI_RENDER_TEXTURE_PLACEHOLDER;
#endif // FFL_NO_RENDER_TEXTURE

        }
    }

    //}

    return expression;
}


void FFLiDeleteMaskTextures(FFLiMaskTextures* pMaskTextures)
{
    for (u32 j = FFL_EXPRESSION_LIMIT; j > 0; j--)
    {
        if (pMaskTextures->pRenderTextures[j - 1] != NULL)
        {
#ifndef FFL_NO_RENDER_TEXTURE // which does NOT allocate it
            FFLiDeleteRenderTexture(pMaskTextures->pRenderTextures[j - 1]);
            FFLiRenderTextureDelete(pMaskTextures->pRenderTextures[j - 1]);
#endif
            pMaskTextures->pRenderTextures[j - 1] = NULL;
        }
    }
}


FFLResult FFLiInitTempObjectMaskTextures(FFLiMaskTexturesTempObject* pObject, const FFLiMaskTextures* pMaskTextures, const FFLiCharInfo* pCharInfo, FFLAllExpressionFlag expressionFlag, u32 resolution, bool enableMipMap, FFLiResourceLoader* pResLoader)
{
    rio::MemUtil::set(pObject, 0, sizeof(FFLiMaskTexturesTempObject));

    FFLResult result = FFLiLoadPartsTextures(&pObject->partsTextures, pCharInfo, expressionFlag, pResLoader);
    if (result != FFL_RESULT_OK)
        return result;

    //InitRawMask(pObject, expressionFlag);
    FFL_LOG_VERBOSE("FFLiInitTempObjectMaskTextures: input expression flags: %d/%d/%d\n",
            expressionFlag.flags[0], expressionFlag.flags[1], expressionFlag.flags[2]);

    for (s32 fi = 0; fi < 3; ++fi) // 3 = Amount of u32s in FFLAllExpressionFlag
    {
        u32 word = expressionFlag.flags[fi]; // flag index
        u32 baseIndex = fi * 32;
        while (word)
        {
            s32 bitIndex = FindNextSetBit(&word);
            u32 i = baseIndex + bitIndex;

            if (i >= FFL_EXPRESSION_LIMIT)
                break;

            RIO_ASSERT(i < FFL_EXPRESSION_LIMIT);

            FFL_LOG_VERBOSE("FFLiInitTempObjectMaskTextures(%p):   Creating mask %i (%p)\n", pObject, (i), pObject->pRawMaskDrawParam[i]);

            FFLiRawMaskTextureDesc desc;
            FFLiCharInfo expressionCharInfo = *pCharInfo;
            const FFLiEyeMouthTypeElement& element = FFLiGetEyeMouthTypeElement(FFLExpression(i));

            SetupExpressionCharInfo(&expressionCharInfo, pCharInfo, FFLExpression(i));

            desc.pTexturesEye[0] = pObject->partsTextures.pTexturesEye[element.eyeTextureType[0]];
            desc.pTexturesEye[1] = pObject->partsTextures.pTexturesEye[element.eyeTextureType[1]];

            desc.pTexturesEyebrow[0] = pObject->partsTextures.pTexturesEyebrow[element.eyebrowTextureType];
            desc.pTexturesEyebrow[1] = pObject->partsTextures.pTexturesEyebrow[element.eyebrowTextureType];

            desc.pTextureMouth = pObject->partsTextures.pTexturesMouth[element.mouthTextureType];

            if (expressionCharInfo.parts.mustacheType != FFL_MUSTACHE_TYPE_EMPTY)
            {
                desc.pTexturesMustache[0] = pObject->partsTextures.pTextureMustache;
                desc.pTexturesMustache[1] = pObject->partsTextures.pTextureMustache;
            }
            else
            {
                desc.pTexturesMustache[0] = NULL;
                desc.pTexturesMustache[1] = NULL;
            }

            desc.pTextureMole = pObject->partsTextures.pTextureMole;

            // InitRawMask
            pObject->pRawMaskDrawParam[i] = new FFLiRawMaskDrawParam;

            FFLiInitDrawParamRawMask(
                pObject->pRawMaskDrawParam[i],
                &expressionCharInfo,
                resolution,
                FFLiCharInfoAndTypeToEyeIndex(pCharInfo, element.eyeTextureType[0]),
                FFLiCharInfoAndTypeToEyeIndex(pCharInfo, element.eyeTextureType[1]),

                FFLiCharInfoAndTypeToEyebrowIndex(pCharInfo, element.eyebrowTextureType),
                FFLiCharInfoAndTypeToMouthIndex(pCharInfo, element.mouthTextureType),

                &desc
            );
        }
    }

    return FFL_RESULT_OK;
}

void FFLiDeleteTempObjectMaskTextures(FFLiMaskTexturesTempObject* pObject, FFLAllExpressionFlag expressionFlag, FFLResourceType resourceType)
{
    /*
    for (u32 j = FFL_EXPRESSION_LIMIT; j > 0; j--)
        if (CanUseExpression(expressionFlag, FFLExpression(j - 1)))
        {
    */
    for (s32 fi = 0; fi < 3; ++fi) // 3 = Amount of u32s in FFLAllExpressionFlag
    {
        u32 word = expressionFlag.flags[fi]; // flag index
        u32 baseIndex = fi * 32;
        while (word)
        {
            s32 bitIndex = FindNextSetBit(&word);
            u32 j = baseIndex + bitIndex;

            if (j >= FFL_EXPRESSION_LIMIT)
                break;
            FFL_LOG_VERBOSE("FFLiDeleteTempObjectMaskTextures(%p): Deleting mask %i (%p)\n", pObject, (j), pObject->pRawMaskDrawParam[j]);

            FFLiDeleteDrawParamRawMask(pObject->pRawMaskDrawParam[j]);
            delete pObject->pRawMaskDrawParam[j];
        }
    }
    //DeleteRawMask(pObject, expressionFlag);

    FFLiDeletePartsTextures(&pObject->partsTextures, expressionFlag, resourceType);
}

#ifndef FFL_NO_RENDER_TEXTURE

void FFLiRenderMaskTextures(FFLiMaskTextures* pMaskTextures, FFLiMaskTexturesTempObject* pObject, const FFLiShaderCallback* pCallback
#if RIO_IS_CAFE
, FFLiCopySurface* pCopySurface
#endif // RIO_IS_CAFE
)
{
    static const FFLColor BLACK = { };

    FFLiInvalidatePartsTextures(&pObject->partsTextures);

    for (u32 i = 0; i < FFL_EXPRESSION_LIMIT; i++)
    {
        if (pMaskTextures->pRenderTextures[i] != NULL && pObject->pRawMaskDrawParam[i] != NULL)
        {
            FFLiInvalidateRawMask(pObject->pRawMaskDrawParam[i]);

            rio::RenderState renderState;
            renderState.setBlendEnable(true);
            renderState.setDepthEnable(false, false);
            renderState.setCullingMode(rio::Graphics::CULLING_MODE_NONE);
            renderState.setBlendFactorSeparate(
                rio::Graphics::BLEND_MODE_ONE_MINUS_DST_ALPHA, rio::Graphics::BLEND_MODE_DST_ALPHA,
#ifdef FFL_NO_DRAW_MASK_ALPHA_VALUES
                rio::Graphics::BLEND_MODE_SRC_ALPHA, rio::Graphics::BLEND_MODE_DST_ALPHA
#else
                rio::Graphics::BLEND_MODE_ONE, rio::Graphics::BLEND_MODE_ONE
#endif
            );
#ifndef FFL_NO_DRAW_MASK_ALPHA_VALUES
            renderState.setBlendEquationSeparate(
                rio::Graphics::BLEND_FUNC_ADD,
                rio::Graphics::BLEND_FUNC_MAX
            );
#else
            renderState.setBlendEquation(rio::Graphics::BLEND_FUNC_ADD);
#endif

            renderState.apply();

            FFLiRenderTexture& renderTexture = *(pMaskTextures->pRenderTextures[i]);

            FFLiInvalidateRenderTexture(&renderTexture);
            RIO_ASSERT(renderTexture.pTexture2D->getTextureFormat() == rio::TEXTURE_FORMAT_R8_G8_B8_A8_UNORM);
            FFLiSetupRenderTexture(&renderTexture, &BLACK, NULL, 0, pCallback);

            pCallback->CallApplyAlphaTestEnable();

            FFLiDrawRawMask(pObject->pRawMaskDrawParam[i], pCallback);

            pCallback->CallApplyAlphaTestDisable();

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
#endif // RIO_IS_WIN
            }

            FFLiFlushRenderTexture(&renderTexture);
        }
    }
}

#endif // FFL_NO_RENDER_TEXTURE

namespace {

#ifndef FFL_NO_RENDER_TEXTURE

void FFLiRenderTextureDelete(FFLiRenderTexture* pRenderTexture)
{
    delete pRenderTexture;
}

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

#endif // FFL_NO_RENDER_TEXTURE

/*
bool CanUseExpression(FFLExpressionFlag expressionFlag, FFLExpression expression)
{
    return (expressionFlag & static_cast<FFLExpressionFlag>(1) << expression) != 0;
}
bool CanUseExpression(const FFLAllExpressionFlag flag, FFLExpression expression)
{
    if (expression >= FFL_EXPRESSION_LIMIT)
        return false;             // Out of range
    int block = expression / 32;  // Determine which 32-bit block
    int bit = expression % 32;    // Determine bit position within the block
    return (flag.flags[block] & (1 << bit)) != 0;
}


void InitRawMask(FFLiMaskTexturesTempObject* pObject, FFLAllExpressionFlag expressionFlag)
{
    for (u32 i = 0; i < FFL_EXPRESSION_LIMIT; i++)
        if (CanUseExpression(expressionFlag, FFLExpression(i)))
            pObject->pRawMaskDrawParam[i] = new FFLiRawMaskDrawParam;
}
void DeleteRawMask(FFLiMaskTexturesTempObject* pObject, FFLAllExpressionFlag expressionFlag)
{
    for (u32 j = FFL_EXPRESSION_LIMIT; j > 0; j--)
        if (CanUseExpression(expressionFlag, FFLExpression(j - 1)))
            delete pObject->pRawMaskDrawParam[j - 1];
}
*/

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
    // courtesy of miitomo
    switch(expression)
    {
    case FFL_EXPRESSION_19:
        [[fallthrough]];
    case FFL_EXPRESSION_20:
        pExpressionCharInfo->parts.eyeScale = 4;
        pExpressionCharInfo->parts.eyeScaleY = 3;
        [[fallthrough]];
    case FFL_EXPRESSION_45:
        [[fallthrough]];
    case FFL_EXPRESSION_46:
        [[fallthrough]];
    case FFL_EXPRESSION_53:
        [[fallthrough]];
    case FFL_EXPRESSION_54:
        pExpressionCharInfo->parts.mouthScaleY = 3;
        pExpressionCharInfo->parts.mouthScale = 4;
        break;
    case FFL_EXPRESSION_25:
        [[fallthrough]];
    case FFL_EXPRESSION_26:
        [[fallthrough]];
    case FFL_EXPRESSION_37:
        [[fallthrough]];
    case FFL_EXPRESSION_38:
        [[fallthrough]];
    case FFL_EXPRESSION_55:
        [[fallthrough]];
    case FFL_EXPRESSION_56:
        [[fallthrough]];
    case FFL_EXPRESSION_57:
        [[fallthrough]];
    case FFL_EXPRESSION_58:
        pExpressionCharInfo->parts.eyeScale = 4;
        pExpressionCharInfo->parts.eyeScaleY = 3;
        pExpressionCharInfo->parts.eyeRotate = 4;
        break;
    case FFL_EXPRESSION_33:
        [[fallthrough]];
    case FFL_EXPRESSION_34:
        pExpressionCharInfo->parts.eyeRotate = 4;
        pExpressionCharInfo->parts.mouthScaleY = 3;
        pExpressionCharInfo->parts.mouthScale = 4;
        break;
    case FFL_EXPRESSION_35:
        [[fallthrough]];
    case FFL_EXPRESSION_36:
        pExpressionCharInfo->parts.eyeRotate = 4;
        pExpressionCharInfo->parts.eyeScaleY = 3;
        pExpressionCharInfo->parts.eyeScale = 4;
        pExpressionCharInfo->parts.mouthScale = 4;
        pExpressionCharInfo->parts.mouthScaleY = 3;
        break;
    case FFL_EXPRESSION_39:
        [[fallthrough]];
    case FFL_EXPRESSION_40:
        pExpressionCharInfo->parts.eyeRotate = 4;
        pExpressionCharInfo->parts.eyeScaleY = 3;
        pExpressionCharInfo->parts.eyeScale = 4;
        pExpressionCharInfo->parts.eyebrowRotate = 6;
        break;
    case FFL_EXPRESSION_43:
        [[fallthrough]];
    case FFL_EXPRESSION_44:
        [[fallthrough]];
    case FFL_EXPRESSION_47:
        [[fallthrough]];
    case FFL_EXPRESSION_48:
        pExpressionCharInfo->parts.eyeRotate = 4;
        break;
    case FFL_EXPRESSION_49: // Cat
        [[fallthrough]];
    case FFL_EXPRESSION_50: // Cat duplicate
        [[fallthrough]];
    case FFL_EXPRESSION_51: // Dog
        [[fallthrough]];
    case FFL_EXPRESSION_52: // Dog duplicate
        pExpressionCharInfo->parts.eyeRotate = 4;
        pExpressionCharInfo->parts.eyeScaleY = 3;
        pExpressionCharInfo->parts.eyeScale = 4;
        pExpressionCharInfo->parts.eyebrowScale = 4;
        pExpressionCharInfo->parts.eyebrowScaleY = 3;
        pExpressionCharInfo->parts.eyebrowRotate = 6;
        pExpressionCharInfo->parts.mouthScaleY = 3;
        pExpressionCharInfo->parts.mouthScale = 4;
        pExpressionCharInfo->parts.eyeSpacingX = 2;
        pExpressionCharInfo->parts.eyePositionY = 12;
        pExpressionCharInfo->parts.eyebrowSpacingX = 2;
        pExpressionCharInfo->parts.eyebrowPositionY = 10;
        pExpressionCharInfo->parts.mouthPositionY = 13;

        pExpressionCharInfo->parts.mustacheType = 0;
        break;
    case FFL_EXPRESSION_67:
        [[fallthrough]];
    case FFL_EXPRESSION_68:
        pExpressionCharInfo->parts.mouthScaleY = 3;
        break;
    case FFL_EXPRESSION_69:
        pExpressionCharInfo->parts.eyeScaleY = 3;
        break;
    default:
        break;
    }

    const CorrectParam& param = GetCorrectParam(expression);

    if (param.mouthType >= 0)
        pExpressionCharInfo->parts.mouthType = param.mouthType;

    s32 eyeRotateOffset = param.eyeRotateOffset;
    if (param.eyeType >= 0 && param.eyeType != pExpressionCharInfo->parts.eyeType)
    {
        s32 expEyeOffset = FFLiiGetEyeRotateOffset(pExpressionCharInfo->parts.eyeType);
        s32 srcEyeOffset = FFLiiGetEyeRotateOffset(param.eyeType);
        if (param.eyeType < 62)
            eyeRotateOffset = (eyeRotateOffset + expEyeOffset) - srcEyeOffset;
        else
            eyeRotateOffset = (eyeRotateOffset - expEyeOffset) + srcEyeOffset;
    }

    s32 eyeRotate;
    if (eyeRotateOffset != 0)
    {
        // pExpressionCharInfo->parts.eyeRotate = clamp(pExpressionCharInfo->parts.eyeRotate + eyeRotateOffset, 0, 7);
        eyeRotate = pExpressionCharInfo->parts.eyeRotate + eyeRotateOffset;
        if (eyeRotate < 0)
            eyeRotate = 0;
        else if (eyeRotate > 7)
            eyeRotate = 7;
        pExpressionCharInfo->parts.eyeRotate = eyeRotate;
    }

    if (pExpressionCharInfo->parts.mouthType == 48)
        pExpressionCharInfo->parts.mouthScale = 8;

    s32 mouthScaleY;
    if (expression == 67)
    {
        if (pExpressionCharInfo->parts.mouthScaleY < 3)
            mouthScaleY = 0;
        else
        {
            mouthScaleY = pExpressionCharInfo->parts.mouthScaleY + -3;
            if (mouthScaleY > 6)
                mouthScaleY = 6;
        }
        pExpressionCharInfo->parts.mouthScaleY = mouthScaleY;
    }

    s32 eyebrowRotateOffset = param.eyebrowRotateOffset;
    pExpressionCharInfo->parts.eyebrowPositionY += param.eyebrowPositionY;
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
