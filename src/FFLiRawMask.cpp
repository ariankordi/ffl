#include <nn/ffl/FFLiModulate.h>
#include <nn/ffl/FFLiRawMask.h>
#include <nn/ffl/FFLiRawMaskParts.h>
#include <nn/ffl/FFLiShaderCallback.h>
#include <nn/ffl/FFLiPartsTextures.h>

#include <nn/ffl/FFLiManager.h> // for g_TextureFlipY

#include <nn/ffl/detail/FFLiCharInfo.h>

#include <gfx/rio_Projection.h>
#include <gpu/rio_RenderState.h>

#if RIO_IS_CAFE
#include <gx2/registers.h>
#endif // RIO_IS_CAFE

namespace {

enum
{
    RAW_MASK_TYPE_MAX = sizeof(FFLiRawMaskDrawParam) / sizeof(FFLiRawMaskPartsDrawParam)
};

struct RawMasks
{
    FFLiRawMaskPartsDesc    rawMaskPartsDescEye[2];
    FFLiRawMaskPartsDesc    rawMaskPartsDescEyebrow[2];
    FFLiRawMaskPartsDesc    rawMaskPartsDescMouth;
    FFLiRawMaskPartsDesc    rawMaskPartsDescMustache[2];
    FFLiRawMaskPartsDesc    rawMaskPartsDescMole;
};

void CalcRawMask(RawMasks* pRawMasks, const FFLiCharInfo* pCharInfo, s32 resolution, s32 leftEyeIndex, s32 rightEyeIndex);


/* void __cdecl nn::util::general::MatrixOrthographicOffCenterRightHanded(struct
   nn::util::general::MatrixRowMajor4x4fType * __ptr64,float,float,float,float,float,float) */
void MatrixOrthographicOffCenterRightHanded(rio::BaseMtx44f* pOutValue, bool flipY, f32 left, f32 right, f32 bottom, f32 top, f32 nearZ, f32 farZ)
{
    f32 col00 = 1.0f / (right - left);
    f32 col11 = 1.0f / (top - bottom);
    f32 col22 = -1.0f / (farZ - nearZ);
    pOutValue->m[0][0] = col00 * 2.0f;
    pOutValue->m[1][0] = 0.0f;
    pOutValue->m[2][0] = 0.0f;
    pOutValue->m[3][0] = 0.0f;
    pOutValue->m[0][1] = 0.0f;

    if (flipY)
        pOutValue->m[1][1] = col11 * 2.0f;
    else
        pOutValue->m[1][1] = -col11 * 2.0f;

    pOutValue->m[2][1] = 0.0f;
    pOutValue->m[3][1] = 0.0f;
    pOutValue->m[0][2] = 0.0f;
    pOutValue->m[1][2] = 0.0f;
    pOutValue->m[2][2] = col22;
    pOutValue->m[3][2] = 0.0f;
    pOutValue->m[0][3] = -(right + left) * col00;

    if (flipY)
        pOutValue->m[1][3] = -(top + bottom) * col11;
    else
        pOutValue->m[1][3] = (top + bottom) * col11;


    pOutValue->m[2][3] = nearZ * col22;
    pOutValue->m[3][3] = 1.0f;
}


}

const s32 excludeColorFromEyeTextureTypes[] = {
    60, 62, 65, 69, 70, 71, 72, 73, 74, 75, 78, 79 // in AFLResHigh
};
const s32 excludeColorFromMouthTypeThreshold = 36;
#ifdef FFL_USE_MODULATE_EYEBROW_EX
const s32 excludeColorFromEyebrowTypeThreshold = 23;
#endif

void FFLiInitDrawParamRawMask(FFLiRawMaskDrawParam* pDrawParam, const FFLiCharInfo* pCharInfo, s32 resolution, s32 leftEyeIndex, s32 rightEyeIndex, s32 eyebrowIndex, s32 mouthIndex, const FFLiRawMaskTextureDesc* pDesc)
{
    RawMasks rawMasks;
    CalcRawMask(&rawMasks, pCharInfo, resolution, leftEyeIndex, rightEyeIndex);

    //const rio::OrthoProjection proj = rio::OrthoProjection(-200.0f, 200.0f, 0.0f, static_cast<f32>(resolution), 0.0f, static_cast<f32>(resolution));
    //rio::BaseMtx44f& projMatrix = const_cast<rio::BaseMtx44f&>(proj.getMatrix());
    rio::BaseMtx44f projMatrix;
    // This ortho matrix below will have [1][1] and [1][3] flipped depending on g_TextureFlipY.
    MatrixOrthographicOffCenterRightHanded(&projMatrix, g_TextureFlipY, 0.0f, static_cast<f32>(resolution),
        0.0f, static_cast<f32>(resolution), 200.0f, -200.0f);

    if (pDesc->pTexturesMustache[0] != NULL) {
        FFLiInitModulateMustache(&pDrawParam->drawParamRawMaskPartsMustache[0].modulateParam, pCharInfo->parts.beardColor, pDesc->pTexturesMustache[0]);
        FFLiInitDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[0]), &(rawMasks.rawMaskPartsDescMustache[0]), &projMatrix);
    } else {
        pDrawParam->drawParamRawMaskPartsMustache[0].primitiveParam.indexCount = 0;
    }

    if (pDesc->pTexturesMustache[1] != NULL) {
        FFLiInitModulateMustache(&pDrawParam->drawParamRawMaskPartsMustache[1].modulateParam, pCharInfo->parts.beardColor, pDesc->pTexturesMustache[1]);
        FFLiInitDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[1]), &(rawMasks.rawMaskPartsDescMustache[1]), &projMatrix);
    } else {
        pDrawParam->drawParamRawMaskPartsMustache[1].primitiveParam.indexCount = 0;
    }

    // for all new AFL/miitomo mouth types starting from 37/type 12...
    // ... they actually do not need colors
    if (mouthIndex > excludeColorFromMouthTypeThreshold)
        FFLiInitModulateMouthEx(&pDrawParam->drawParamRawMaskPartsMouth.modulateParam, pCharInfo->parts.mouthColor, pDesc->pTextureMouth);
    else
        FFLiInitModulateMouth(&pDrawParam->drawParamRawMaskPartsMouth.modulateParam, pCharInfo->parts.mouthColor, pDesc->pTextureMouth);
    FFLiInitDrawParamRawMaskParts(&pDrawParam->drawParamRawMaskPartsMouth, &rawMasks.rawMaskPartsDescMouth, &projMatrix);

    if (pDesc->pTexturesEyebrow[0] != NULL) {
#ifdef FFL_USE_MODULATE_EYEBROW_EX
        if (eyebrowIndex > excludeColorFromEyebrowTypeThreshold)
            FFLiInitModulateEyebrowEx(&pDrawParam->drawParamRawMaskPartsEyebrow[0].modulateParam, pCharInfo->parts.eyebrowColor, pDesc->pTexturesEyebrow[0]);
        else
#endif
            FFLiInitModulateEyebrow(&pDrawParam->drawParamRawMaskPartsEyebrow[0].modulateParam, pCharInfo->parts.eyebrowColor, pDesc->pTexturesEyebrow[0]);
        FFLiInitDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[0]), &(rawMasks.rawMaskPartsDescEyebrow[0]), &projMatrix);
    } else {
        pDrawParam->drawParamRawMaskPartsEyebrow[0].primitiveParam.indexCount = 0;
    }

    if (pDesc->pTexturesEyebrow[1] != NULL) {
#ifdef FFL_USE_MODULATE_EYEBROW_EX
        if (eyebrowIndex > excludeColorFromEyebrowTypeThreshold)
            FFLiInitModulateEyebrowEx(&pDrawParam->drawParamRawMaskPartsEyebrow[1].modulateParam, pCharInfo->parts.eyebrowColor, pDesc->pTexturesEyebrow[1]);
        else
#endif
            FFLiInitModulateEyebrow(&pDrawParam->drawParamRawMaskPartsEyebrow[1].modulateParam, pCharInfo->parts.eyebrowColor, pDesc->pTexturesEyebrow[1]);
        FFLiInitDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[1]), &(rawMasks.rawMaskPartsDescEyebrow[1]), &projMatrix);
    } else {
        pDrawParam->drawParamRawMaskPartsEyebrow[1].primitiveParam.indexCount = 0;
    }

    // for certain eye indices...
    // ... exclude color entirely by setting modulate mode to 1
    bool isLeftEyeUsingTextureDirect = false;
    bool isRightEyeUsingTextureDirect = false;
    for (u32 i = 0; i < (sizeof(excludeColorFromEyeTextureTypes) / sizeof(u32)); i++) {
        if (leftEyeIndex == excludeColorFromEyeTextureTypes[i])
            isLeftEyeUsingTextureDirect = true;
        if (rightEyeIndex == excludeColorFromEyeTextureTypes[i])
            isRightEyeUsingTextureDirect = true;
    }

    if (isLeftEyeUsingTextureDirect)
        FFLiInitModulateEyeEx(&pDrawParam->drawParamRawMaskPartsEye[0].modulateParam, pCharInfo->parts.eyeColor, pCharInfo->parts.eyeType, pDesc->pTexturesEye[0]);
    else
        FFLiInitModulateEye(&pDrawParam->drawParamRawMaskPartsEye[0].modulateParam, pCharInfo->parts.eyeColor, pCharInfo->parts.eyeType, pDesc->pTexturesEye[0]);

    if (isRightEyeUsingTextureDirect)
        FFLiInitModulateEyeEx(&pDrawParam->drawParamRawMaskPartsEye[1].modulateParam, pCharInfo->parts.eyeColor, pCharInfo->parts.eyeType, pDesc->pTexturesEye[1]);
    else
        FFLiInitModulateEye(&pDrawParam->drawParamRawMaskPartsEye[1].modulateParam, pCharInfo->parts.eyeColor, pCharInfo->parts.eyeType, pDesc->pTexturesEye[1]);

    FFLiInitDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEye[0]), &(rawMasks.rawMaskPartsDescEye[0]), &projMatrix);
    FFLiInitDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEye[1]), &(rawMasks.rawMaskPartsDescEye[1]), &projMatrix);

    if (pDesc->pTextureMole != NULL) {
        FFLiInitModulateMole(&pDrawParam->drawParamRawMaskPartsMole.modulateParam, pDesc->pTextureMole);
        FFLiInitDrawParamRawMaskParts(&pDrawParam->drawParamRawMaskPartsMole, &rawMasks.rawMaskPartsDescMole, &projMatrix);
    } else {
        pDrawParam->drawParamRawMaskPartsMole.primitiveParam.indexCount = 0;
    }

    FFLiInitModulateFill(&pDrawParam->drawParamRawMaskPartsFill.modulateParam);
    FFLiInitDrawParamRawMaskPartsFill(&pDrawParam->drawParamRawMaskPartsFill);
}

void FFLiDeleteDrawParamRawMask(FFLiRawMaskDrawParam* pDrawParam)
{
#ifdef FFL_LOG_CHARMODEL_CLEANUP
    RIO_LOG("in FFLiDeleteDrawParamRawMask(%p)\n", pDrawParam);
#endif
    FFLiDeleteDrawParamRawMaskPartsFill(&pDrawParam->drawParamRawMaskPartsFill);
    if (pDrawParam->drawParamRawMaskPartsMole.primitiveParam.indexCount != 0)
    {
#ifdef FFL_LOG_CHARMODEL_CLEANUP
        RIO_LOG("FFLiDeleteDrawParamRawMaskParts(&pDrawParam->drawParamRawMaskPartsMole)\n");
#endif
        FFLiDeleteDrawParamRawMaskParts(&pDrawParam->drawParamRawMaskPartsMole);
    }
    FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEye[1]));
    FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEye[0]));
    if (pDrawParam->drawParamRawMaskPartsEyebrow[1].primitiveParam.indexCount != 0)
    {
#ifdef FFL_LOG_CHARMODEL_CLEANUP
        RIO_LOG("FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[1]))\n");
#endif
        FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[1]));
    }
    if (pDrawParam->drawParamRawMaskPartsEyebrow[0].primitiveParam.indexCount != 0)
    {
#ifdef FFL_LOG_CHARMODEL_CLEANUP
        RIO_LOG("FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[0]))\n");
#endif
        FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[0]));
    }
    FFLiDeleteDrawParamRawMaskParts(&pDrawParam->drawParamRawMaskPartsMouth);
    if (pDrawParam->drawParamRawMaskPartsMustache[1].primitiveParam.indexCount != 0)
    {
#ifdef FFL_LOG_CHARMODEL_CLEANUP
        RIO_LOG("FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[1]))\n");
#endif
        FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[1]));
    }
    if (pDrawParam->drawParamRawMaskPartsMustache[0].primitiveParam.indexCount != 0)
    {
#ifdef FFL_LOG_CHARMODEL_CLEANUP
        RIO_LOG("FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[0]))\n");
#endif
        FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[0]));
    }
#ifdef FFL_LOG_CHARMODEL_CLEANUP
    RIO_LOG("exiting FFLiDeleteDrawParamRawMask\n");
#endif
}

void FFLiInvalidateRawMask(FFLiRawMaskDrawParam* pDrawParam)
{
    if (pDrawParam->drawParamRawMaskPartsMustache[0].primitiveParam.indexCount != 0)
        FFLiInvalidateDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[0]));
    if (pDrawParam->drawParamRawMaskPartsMustache[1].primitiveParam.indexCount != 0)
        FFLiInvalidateDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[1]));
    FFLiInvalidateDrawParamRawMaskParts(&pDrawParam->drawParamRawMaskPartsMouth);
    if (pDrawParam->drawParamRawMaskPartsEyebrow[0].primitiveParam.indexCount != 0)
        FFLiInvalidateDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[0]));
    if (pDrawParam->drawParamRawMaskPartsEyebrow[1].primitiveParam.indexCount != 0)
        FFLiInvalidateDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[1]));
    FFLiInvalidateDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEye[0]));
    FFLiInvalidateDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEye[1]));
    if (pDrawParam->drawParamRawMaskPartsMole.primitiveParam.indexCount != 0)
        FFLiInvalidateDrawParamRawMaskParts(&pDrawParam->drawParamRawMaskPartsMole);
    FFLiInvalidateDrawParamRawMaskParts(&pDrawParam->drawParamRawMaskPartsFill);
}

void FFLiDrawRawMask(const FFLiRawMaskDrawParam* pDrawParam, const FFLiShaderCallback* pCallback)
{
    pCallback->CallApplyAlphaTestEnable();
// does not make a huge difference but mask is
// a bit inaccurate, has outlines without this
#ifndef FFL_NO_DRAW_MASK_ALPHA_VALUES
    if (pDrawParam->drawParamRawMaskPartsMustache[0].primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[0]), pCallback);
    if (pDrawParam->drawParamRawMaskPartsMustache[1].primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[1]), pCallback);
    FFLiDrawRawMaskParts(&pDrawParam->drawParamRawMaskPartsMouth, pCallback);
    if (pDrawParam->drawParamRawMaskPartsEyebrow[0].primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[0]), pCallback);
    if (pDrawParam->drawParamRawMaskPartsEyebrow[1].primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[1]), pCallback);
    FFLiDrawRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEye[0]), pCallback);
    FFLiDrawRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEye[1]), pCallback);
    if (pDrawParam->drawParamRawMaskPartsMole.primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&pDrawParam->drawParamRawMaskPartsMole, pCallback);

    rio::RenderState renderState;
    renderState.setColorMask(false, false, false, true);
    renderState.applyColorMask();
    renderState.setBlendFactor(rio::Graphics::BLEND_MODE_ZERO, rio::Graphics::BLEND_MODE_ZERO);
    renderState.setBlendEquation(rio::Graphics::BLEND_FUNC_ADD);
    renderState.applyBlendAndFastZ();
    pCallback->CallApplyAlphaTestDisable();

    FFLiDrawRawMaskParts(&pDrawParam->drawParamRawMaskPartsFill, pCallback);

    renderState.setBlendFactor(rio::Graphics::BLEND_MODE_SRC_ALPHA, rio::Graphics::BLEND_MODE_ONE);
    renderState.setBlendFactorSrcAlpha(rio::Graphics::BLEND_MODE_ONE);
    renderState.applyBlendAndFastZ();
    pCallback->CallApplyAlphaTestEnable();
#endif

    if (pDrawParam->drawParamRawMaskPartsMustache[0].primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[0]), pCallback);
    if (pDrawParam->drawParamRawMaskPartsMustache[1].primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[1]), pCallback);
    FFLiDrawRawMaskParts(&pDrawParam->drawParamRawMaskPartsMouth, pCallback);
    if (pDrawParam->drawParamRawMaskPartsEyebrow[0].primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[0]), pCallback);
    if (pDrawParam->drawParamRawMaskPartsEyebrow[1].primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[1]), pCallback);
    FFLiDrawRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEye[0]), pCallback);
    FFLiDrawRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEye[1]), pCallback);
    if (pDrawParam->drawParamRawMaskPartsMole.primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&pDrawParam->drawParamRawMaskPartsMole, pCallback);
#ifndef FFL_NO_DRAW_MASK_ALPHA_VALUES
    renderState.setColorMask(true, true, true, true);
    renderState.applyColorMask();
#endif
    pCallback->CallApplyAlphaTestDisable();
}

namespace {

void CalcRawMask(RawMasks* pRawMasks, const FFLiCharInfo* pCharInfo, s32 resolution, s32 leftEyeIndex, s32 rightEyeIndex)
{
    static f32 POS_X_ADD    = 3.5323312f;
    static f32 POS_Y_ADD    = 4.629278f;

    static f32 SPACING_MUL  = 0.88961464f;
    static f32 POS_X_MUL    = 1.7792293f;
    static f32 POS_Y_MUL    = 1.0760943f;

    static f32 POS_Y_ADD_EYE        = POS_Y_ADD + 13.822246f;
    static f32 POS_Y_ADD_EYEBROW    = POS_Y_ADD + 11.920528f;
    static f32 POS_Y_ADD_MOUTH      = POS_Y_ADD + 24.629572f;
    static f32 POS_Y_ADD_MUSTACHE   = POS_Y_ADD + 27.134275f;
    static f32 POS_X_ADD_MOLE       = POS_X_ADD + 14.233834f;
    static f32 POS_Y_ADD_MOLE       = POS_Y_ADD + 11.178394f + 2 * POS_Y_MUL;

    f32 baseScale = resolution * (1.f / 64.f);

    f32 eyeSpacingX = pCharInfo->parts.eyeSpacingX * SPACING_MUL;

    f32 eyeBaseScale = 0.4f * pCharInfo->parts.eyeScale + 1.0f;
    f32 eyeBaseScaleY = 0.12f * pCharInfo->parts.eyeScaleY + 0.64f;
    f32 eyeScaleX = 5.34375f * eyeBaseScale;
    f32 eyeScaleY = 4.5f * eyeBaseScale * eyeBaseScaleY;

    f32 eyePosY = pCharInfo->parts.eyePositionY * POS_Y_MUL + POS_Y_ADD_EYE;

    s32 eyeBaseRotate = pCharInfo->parts.eyeRotate + FFLiiGetEyeRotateOffset(pCharInfo->parts.eyeType);
    f32 eyeRotate = (eyeBaseRotate % 32) * (360.f / 32.f);

    f32 eyebrowSpacingX = pCharInfo->parts.eyebrowSpacingX * SPACING_MUL;

    f32 eyebrowBaseScale = 0.4f * pCharInfo->parts.eyebrowScale + 1.0f;
    f32 eyebrowBaseScaleY = 0.12f * pCharInfo->parts.eyebrowScaleY + 0.64f;
    f32 eyebrowScaleX = 5.0625f * eyebrowBaseScale;
    f32 eyebrowScaleY = 4.5f * eyebrowBaseScale * eyebrowBaseScaleY;

    f32 eyebrowPosY = pCharInfo->parts.eyebrowPositionY * POS_Y_MUL + POS_Y_ADD_EYEBROW;

    s32 eyebrowBaseRotate = pCharInfo->parts.eyebrowRotate + FFLiiGetEyebrowRotateOffset(pCharInfo->parts.eyebrowType);
    f32 eyebrowRotate = (eyebrowBaseRotate % 32) * (360.0f / 32);

    f32 mouthBaseScale = 0.4f * pCharInfo->parts.mouthScale + 1.0f;
    f32 mouthBaseScaleY = 0.12f * pCharInfo->parts.mouthScaleY + 0.64f;
    f32 mouthScaleX = 6.1875f * mouthBaseScale;
    f32 mouthScaleY = 4.5f * mouthBaseScale * mouthBaseScaleY;

    f32 mouthPosY = pCharInfo->parts.mouthPositionY * POS_Y_MUL + POS_Y_ADD_MOUTH;

    f32 mustacheBaseScale = 0.4f * pCharInfo->parts.mustacheScale + 1.0f;
    f32 mustacheScaleX = 4.5f * mustacheBaseScale;
    f32 mustacheScaleY = 9.0f * mustacheBaseScale;

    f32 mustachePosY = pCharInfo->parts.mustachePositionY * POS_Y_MUL + POS_Y_ADD_MUSTACHE;

    f32 moleScale = 0.4f * pCharInfo->parts.moleScale + 1.0f;

    f32 molePosX = pCharInfo->parts.molePositionX * POS_X_MUL + POS_X_ADD_MOLE;
    f32 molePosY = pCharInfo->parts.molePositionY * POS_Y_MUL + POS_Y_ADD_MOLE;

    pRawMasks->rawMaskPartsDescEye[0].pos.x = (32 - eyeSpacingX) * baseScale;
    pRawMasks->rawMaskPartsDescEye[0].pos.y = eyePosY * baseScale;
    pRawMasks->rawMaskPartsDescEye[0].scale.x = eyeScaleX * baseScale;
    pRawMasks->rawMaskPartsDescEye[0].scale.y = FFLiiGetAdjustedEyeH(eyeScaleY * baseScale, leftEyeIndex);
    pRawMasks->rawMaskPartsDescEye[0].rot = eyeRotate;
    pRawMasks->rawMaskPartsDescEye[0].originPos = FFLI_ORIGIN_POSITION_LEFT;

    pRawMasks->rawMaskPartsDescEye[1].pos.x = (eyeSpacingX + 32) * baseScale;
    pRawMasks->rawMaskPartsDescEye[1].pos.y = eyePosY * baseScale;
    pRawMasks->rawMaskPartsDescEye[1].scale.x = eyeScaleX * baseScale;
    pRawMasks->rawMaskPartsDescEye[1].scale.y = FFLiiGetAdjustedEyeH(eyeScaleY * baseScale, rightEyeIndex);
    pRawMasks->rawMaskPartsDescEye[1].rot = 360.0f - eyeRotate;
    pRawMasks->rawMaskPartsDescEye[1].originPos = FFLI_ORIGIN_POSITION_RIGHT;

    pRawMasks->rawMaskPartsDescEyebrow[0].pos.x = (32 - eyebrowSpacingX) * baseScale;
    pRawMasks->rawMaskPartsDescEyebrow[0].pos.y = eyebrowPosY * baseScale;
    pRawMasks->rawMaskPartsDescEyebrow[0].scale.x = eyebrowScaleX * baseScale;
    pRawMasks->rawMaskPartsDescEyebrow[0].scale.y = eyebrowScaleY * baseScale;
    pRawMasks->rawMaskPartsDescEyebrow[0].rot = eyebrowRotate;
    pRawMasks->rawMaskPartsDescEyebrow[0].originPos = FFLI_ORIGIN_POSITION_LEFT;

    pRawMasks->rawMaskPartsDescEyebrow[1].pos.x = (eyebrowSpacingX + 32) * baseScale;
    pRawMasks->rawMaskPartsDescEyebrow[1].pos.y = eyebrowPosY * baseScale;
    pRawMasks->rawMaskPartsDescEyebrow[1].scale.x = eyebrowScaleX * baseScale;
    pRawMasks->rawMaskPartsDescEyebrow[1].scale.y = eyebrowScaleY * baseScale;
    pRawMasks->rawMaskPartsDescEyebrow[1].rot = 360.0f - eyebrowRotate;
    pRawMasks->rawMaskPartsDescEyebrow[1].originPos = FFLI_ORIGIN_POSITION_RIGHT;

    pRawMasks->rawMaskPartsDescMouth.pos.x = 32 * baseScale;
    pRawMasks->rawMaskPartsDescMouth.pos.y = mouthPosY * baseScale;
    pRawMasks->rawMaskPartsDescMouth.scale.x = mouthScaleX * baseScale;
    pRawMasks->rawMaskPartsDescMouth.scale.y = FFLiiGetAdjustedMouthH(mouthScaleY * baseScale, pCharInfo->parts.mouthType);
    pRawMasks->rawMaskPartsDescMouth.rot = 0.0f;
    pRawMasks->rawMaskPartsDescMouth.originPos = FFLI_ORIGIN_POSITION_CENTER;

    pRawMasks->rawMaskPartsDescMustache[0].pos.x = 32 * baseScale;
    pRawMasks->rawMaskPartsDescMustache[0].pos.y = mustachePosY * baseScale;
    pRawMasks->rawMaskPartsDescMustache[0].scale.x = mustacheScaleX * baseScale;
    pRawMasks->rawMaskPartsDescMustache[0].scale.y = mustacheScaleY * baseScale;
    pRawMasks->rawMaskPartsDescMustache[0].rot = 0.0f;
    pRawMasks->rawMaskPartsDescMustache[0].originPos = FFLI_ORIGIN_POSITION_LEFT;

    pRawMasks->rawMaskPartsDescMustache[1].pos.x = 32 * baseScale;
    pRawMasks->rawMaskPartsDescMustache[1].pos.y = mustachePosY * baseScale;
    pRawMasks->rawMaskPartsDescMustache[1].scale.x = mustacheScaleX * baseScale;
    pRawMasks->rawMaskPartsDescMustache[1].scale.y = mustacheScaleY * baseScale;
    pRawMasks->rawMaskPartsDescMustache[1].rot = 0.0f;
    pRawMasks->rawMaskPartsDescMustache[1].originPos = FFLI_ORIGIN_POSITION_RIGHT;

    pRawMasks->rawMaskPartsDescMole.pos.x = molePosX * baseScale;
    pRawMasks->rawMaskPartsDescMole.pos.y = molePosY * baseScale;
    pRawMasks->rawMaskPartsDescMole.scale.x = moleScale * baseScale;
    pRawMasks->rawMaskPartsDescMole.scale.y = moleScale * baseScale;
    pRawMasks->rawMaskPartsDescMole.rot = 0.0f;
    pRawMasks->rawMaskPartsDescMole.originPos = FFLI_ORIGIN_POSITION_CENTER;
}

}
