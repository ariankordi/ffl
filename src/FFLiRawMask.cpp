#include <nn/ffl/FFLiModulate.h>
#include <nn/ffl/FFLiRawMask.h>
#include <nn/ffl/FFLiRawMaskParts.h>
#include <nn/ffl/FFLiShaderCallback.h>
#include <nn/ffl/FFLiPartsTextures.h>

#include <nn/ffl/FFLiUtil.h>
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
    FFLiRawMaskPartsDesc eyeR;
    FFLiRawMaskPartsDesc eyeL;
    FFLiRawMaskPartsDesc eyebrowR;
    FFLiRawMaskPartsDesc eyebrowL;
    FFLiRawMaskPartsDesc mouth;
    FFLiRawMaskPartsDesc mustacheR;
    FFLiRawMaskPartsDesc mustacheL;
    FFLiRawMaskPartsDesc mole;
};

void CalcRawMask(RawMasks* pRawMasks, const FFLiCharInfo* pCharInfo, s32 resolution, s32 leftEyeIndex, s32 rightEyeIndex);

}

void FFLiGetMaskMatrix(rio::BaseMtx44f* pBaseMtx44f, f32 width)
{
    //MatrixOrthographicOffCenterRightHanded(pBaseMtx44f,
    //g_TextureFlipY, 0.0f, 64, 0.0f, 64, 200.0f, -200.0f);

    const float col0011 = 2.0 / 64;
    // First row.
    pBaseMtx44f->m[0][0] = col0011; // X-axis.
    pBaseMtx44f->m[1][0] = 0.0f;
    pBaseMtx44f->m[2][0] = 0.0f;
    pBaseMtx44f->m[3][0] = 0.0f;

    // Second row.
    pBaseMtx44f->m[0][1] = 0.0f;
    pBaseMtx44f->m[1][1] = col0011; // Y-axis.
    pBaseMtx44f->m[2][1] = 0.0f;
    pBaseMtx44f->m[3][1] = 0.0f;

    // Third row.
    pBaseMtx44f->m[0][2] = 0.0f;
    pBaseMtx44f->m[1][2] = 0.0f;
    // Z-axis is not needed for 2D planes. It can be 1 or 0.
    pBaseMtx44f->m[2][2] = 1.0f; // 0.64 / width;
    pBaseMtx44f->m[3][2] = 0.0f;

    // Fourth row (translation).
    // Set translation to -1.0.
    pBaseMtx44f->m[0][3] = -1.0f;
    pBaseMtx44f->m[1][3] = -1.0f;
    pBaseMtx44f->m[2][3] = 0.0f; // Can also be -1.0f.
    pBaseMtx44f->m[3][3] = 1.0f; // W-axis kept to 1.0.
    if (!g_TextureFlipY)
    {
        pBaseMtx44f->m[1][1] *= -1.0f;
        pBaseMtx44f->m[1][3] *= -1.0f;
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
    CalcRawMask(&rawMasks, pCharInfo, /* resolution */ 2, leftEyeIndex, rightEyeIndex);

    //const rio::OrthoProjection proj = rio::OrthoProjection(-200.0f, 200.0f, 0.0f, static_cast<f32>(resolution), 0.0f, static_cast<f32>(resolution));
    //rio::BaseMtx44f& projMatrix = const_cast<rio::BaseMtx44f&>(proj.getMatrix());

    rio::BaseMtx44f projMatrix;
    FFLiGetMaskMatrix(&projMatrix, 64); // static_cast<f32>(resolution));

    if (pDesc->pTextureMustacheR != NULL)
    {
        FFLiInitModulateMustache(&pDrawParam->mustacheR.modulateParam, pCharInfo->parts.beardColor, pDesc->pTextureMustacheR);
        FFLiInitDrawParamRawMaskParts(&(pDrawParam->mustacheR), &(rawMasks.mustacheR), &projMatrix);
    }
    else
    {
        pDrawParam->mustacheR.primitiveParam.indexCount = 0;
    }

    if (pDesc->pTextureMustacheL != NULL)
    {
        FFLiInitModulateMustache(&pDrawParam->mustacheL.modulateParam, pCharInfo->parts.beardColor, pDesc->pTextureMustacheL);
        FFLiInitDrawParamRawMaskParts(&(pDrawParam->mustacheL), &(rawMasks.mustacheL), &projMatrix);
    }
    else
    {
        pDrawParam->mustacheL.primitiveParam.indexCount = 0;
    }

    // for all new AFL/miitomo mouth types starting from 37/type 12...
    // ... they actually do not need colors
    if (mouthIndex > excludeColorFromMouthTypeThreshold)
        FFLiInitModulateMouthEx(&pDrawParam->mouth.modulateParam, pCharInfo->parts.mouthColor, pDesc->pTextureMouth);
    else
        FFLiInitModulateMouth(&pDrawParam->mouth.modulateParam, pCharInfo->parts.mouthColor, pDesc->pTextureMouth);
    FFLiInitDrawParamRawMaskParts(&pDrawParam->mouth, &rawMasks.mouth, &projMatrix);

    if (pDesc->pTextureEyebrowR != NULL)
    {
#ifdef FFL_USE_MODULATE_EYEBROW_EX
        if (eyebrowIndex > excludeColorFromEyebrowTypeThreshold)
            FFLiInitModulateEyebrowEx(&pDrawParam->eyebrowR.modulateParam, pCharInfo->parts.eyebrowColor, pDesc->pTextureEyebrowR);
        else
#endif
            FFLiInitModulateEyebrow(&pDrawParam->eyebrowR.modulateParam, pCharInfo->parts.eyebrowColor, pDesc->pTextureEyebrowR);
        FFLiInitDrawParamRawMaskParts(&(pDrawParam->eyebrowR), &(rawMasks.eyebrowR), &projMatrix);
    }
    else
    {
        pDrawParam->eyebrowR.primitiveParam.indexCount = 0;
    }

    if (pDesc->pTextureEyebrowL != NULL)
    {
#ifdef FFL_USE_MODULATE_EYEBROW_EX
        if (eyebrowIndex > excludeColorFromEyebrowTypeThreshold)
            FFLiInitModulateEyebrowEx(&pDrawParam->eyebrowL.modulateParam, pCharInfo->parts.eyebrowColor, pDesc->pTextureEyebrowL);
        else
#endif
            FFLiInitModulateEyebrow(&pDrawParam->eyebrowL.modulateParam, pCharInfo->parts.eyebrowColor, pDesc->pTextureEyebrowL);
        FFLiInitDrawParamRawMaskParts(&(pDrawParam->eyebrowL), &(rawMasks.eyebrowL), &projMatrix);
    }
    else
    {
        pDrawParam->eyebrowL.primitiveParam.indexCount = 0;
    }

    // for certain eye indices...
    // ... exclude color entirely by setting modulate mode to 1
    bool isEyeRDirect = false;
    bool isEyeLDirect = false;
    for (u32 i = 0; i < (sizeof(excludeColorFromEyeTextureTypes) / sizeof(u32)); i++)
    {
        if (leftEyeIndex == excludeColorFromEyeTextureTypes[i])
            isEyeRDirect = true;
        if (rightEyeIndex == excludeColorFromEyeTextureTypes[i])
            isEyeLDirect = true;
    }

    if (isEyeRDirect)
        FFLiInitModulateEyeEx(&pDrawParam->eyeR.modulateParam, pCharInfo->parts.eyeColor, pCharInfo->parts.eyeType, pDesc->pTextureEyeR);
    else
        FFLiInitModulateEye(&pDrawParam->eyeR.modulateParam, pCharInfo->parts.eyeColor, pCharInfo->parts.eyeType, pDesc->pTextureEyeR);

    if (isEyeLDirect)
        FFLiInitModulateEyeEx(&pDrawParam->eyeL.modulateParam, pCharInfo->parts.eyeColor, pCharInfo->parts.eyeType, pDesc->pTextureEyeL);
    else
        FFLiInitModulateEye(&pDrawParam->eyeL.modulateParam, pCharInfo->parts.eyeColor, pCharInfo->parts.eyeType, pDesc->pTextureEyeL);

    FFLiInitDrawParamRawMaskParts(&(pDrawParam->eyeR), &(rawMasks.eyeR), &projMatrix);
    FFLiInitDrawParamRawMaskParts(&(pDrawParam->eyeL), &(rawMasks.eyeL), &projMatrix);

    if (pDesc->pTextureMole != NULL)
    {
        FFLiInitModulateMole(&pDrawParam->mole.modulateParam, pDesc->pTextureMole);
        FFLiInitDrawParamRawMaskParts(&pDrawParam->mole, &rawMasks.mole, &projMatrix);
    }
    else
    {
        pDrawParam->mole.primitiveParam.indexCount = 0;
    }

    FFLiInitModulateFill(&pDrawParam->fill.modulateParam);
    FFLiInitDrawParamRawMaskPartsFill(&pDrawParam->fill);
}

void FFLiDeleteDrawParamRawMask(FFLiRawMaskDrawParam* pDrawParam)
{
    FFL_LOG_VERBOSE("in FFLiDeleteDrawParamRawMask(%p)\n", pDrawParam);
    FFLiDeleteDrawParamRawMaskPartsFill(&pDrawParam->fill);
    if (pDrawParam->mole.primitiveParam.indexCount != 0)
    {
        FFL_LOG_VERBOSE("FFLiDeleteDrawParamRawMaskParts(&pDrawParam->drawParamRawMaskPartsMole)\n");
        FFLiDeleteDrawParamRawMaskParts(&pDrawParam->mole);
    }
    FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->eyeL));
    FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->eyeR));
    if (pDrawParam->eyebrowL.primitiveParam.indexCount != 0)
    {
        FFL_LOG_VERBOSE("FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[1]))\n");
        FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->eyebrowL));
    }
    if (pDrawParam->eyebrowR.primitiveParam.indexCount != 0)
    {
        FFL_LOG_VERBOSE("FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsEyebrow[0]))\n");
        FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->eyebrowR));
    }
    FFLiDeleteDrawParamRawMaskParts(&pDrawParam->mouth);
    if (pDrawParam->mustacheL.primitiveParam.indexCount != 0)
    {
        FFL_LOG_VERBOSE("FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[1]))\n");
        FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->mustacheL));
    }
    if (pDrawParam->mustacheR.primitiveParam.indexCount != 0)
    {
        FFL_LOG_VERBOSE("FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->drawParamRawMaskPartsMustache[0]))\n");
        FFLiDeleteDrawParamRawMaskParts(&(pDrawParam->mustacheR));
    }

    FFL_LOG_VERBOSE("exiting FFLiDeleteDrawParamRawMask\n");
}

void FFLiInvalidateRawMask(FFLiRawMaskDrawParam* pDrawParam)
{
    if (pDrawParam->mustacheR.primitiveParam.indexCount != 0)
        FFLiInvalidateDrawParamRawMaskParts(&(pDrawParam->mustacheR));
    if (pDrawParam->mustacheL.primitiveParam.indexCount != 0)
        FFLiInvalidateDrawParamRawMaskParts(&(pDrawParam->mustacheL));
    FFLiInvalidateDrawParamRawMaskParts(&pDrawParam->mouth);
    if (pDrawParam->eyebrowR.primitiveParam.indexCount != 0)
        FFLiInvalidateDrawParamRawMaskParts(&(pDrawParam->eyebrowR));
    if (pDrawParam->eyebrowL.primitiveParam.indexCount != 0)
        FFLiInvalidateDrawParamRawMaskParts(&(pDrawParam->eyebrowL));
    FFLiInvalidateDrawParamRawMaskParts(&(pDrawParam->eyeR));
    FFLiInvalidateDrawParamRawMaskParts(&(pDrawParam->eyeL));
    if (pDrawParam->mole.primitiveParam.indexCount != 0)
        FFLiInvalidateDrawParamRawMaskParts(&pDrawParam->mole);
    FFLiInvalidateDrawParamRawMaskParts(&pDrawParam->fill);
}

void FFLiDrawRawMask(const FFLiRawMaskDrawParam* pDrawParam, const FFLiShaderCallback* pCallback)
{
    if (pDrawParam->mustacheR.primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->mustacheR), pCallback);
    if (pDrawParam->mustacheL.primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->mustacheL), pCallback);
    FFLiDrawRawMaskParts(&pDrawParam->mouth, pCallback);
    if (pDrawParam->eyebrowR.primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->eyebrowR), pCallback);
    if (pDrawParam->eyebrowL.primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->eyebrowL), pCallback);
    FFLiDrawRawMaskParts(&(pDrawParam->eyeR), pCallback);
    FFLiDrawRawMaskParts(&(pDrawParam->eyeL), pCallback);
    if (pDrawParam->mole.primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&pDrawParam->mole, pCallback);

#ifndef FFL_NO_DRAW_MASK_ALPHA_VALUES
    rio::RenderState renderState;
    renderState.setBlendEnable(true);
    renderState.setDepthEnable(false, false);
    renderState.setCullingMode(rio::Graphics::CULLING_MODE_NONE);
    renderState.setBlendFactorSeparate(
        rio::Graphics::BLEND_MODE_ONE_MINUS_DST_ALPHA, rio::Graphics::BLEND_MODE_DST_ALPHA,
        rio::Graphics::BLEND_MODE_ONE, rio::Graphics::BLEND_MODE_ONE
    );
    renderState.setBlendEquationSeparate(
        rio::Graphics::BLEND_FUNC_ADD,
        rio::Graphics::BLEND_FUNC_MAX
    );

    renderState.setColorMask(false, false, false, true);
    renderState.applyColorMask();
    renderState.setBlendFactor(rio::Graphics::BLEND_MODE_ZERO, rio::Graphics::BLEND_MODE_ZERO);
    renderState.setBlendEquation(rio::Graphics::BLEND_FUNC_ADD);
    renderState.applyBlendAndFastZ();
    pCallback->CallApplyAlphaTestDisable();

    FFLiDrawRawMaskParts(&pDrawParam->drawParamRawMaskPartsFill, pCallback);

    renderState.setBlendFactor(rio::Graphics::BLEND_MODE_SRC_ALPHA, rio::Graphics::BLEND_MODE_ONE);
    renderState.applyBlendAndFastZ();
    pCallback->CallApplyAlphaTestEnable();

    if (pDrawParam->mustacheR.primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->mustacheR), pCallback);
    if (pDrawParam->mustacheL.primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->mustacheL), pCallback);
    FFLiDrawRawMaskParts(&pDrawParam->mouth, pCallback);
    if (pDrawParam->eyebrowR.primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->eyebrowR), pCallback);
    if (pDrawParam->eyebrowL.primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&(pDrawParam->eyebrowL), pCallback);
    FFLiDrawRawMaskParts(&(pDrawParam->eyeR), pCallback);
    FFLiDrawRawMaskParts(&(pDrawParam->eyeL), pCallback);
    if (pDrawParam->mole.primitiveParam.indexCount != 0)
        FFLiDrawRawMaskParts(&pDrawParam->mole, pCallback);

    renderState.setColorMask(true, true, true, true);
    renderState.applyColorMask();
    pCallback->CallApplyAlphaTestDisable();

#endif // FFL_NO_DRAW_MASK_ALPHA_VALUES

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

    //f32 baseScale = resolution * (1.f / 64.f);
    const f32 baseScale = 1.0f;

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

    pRawMasks->eyeR.pos.x = (32 - eyeSpacingX) * baseScale;
    pRawMasks->eyeR.pos.y = eyePosY * baseScale;
    pRawMasks->eyeR.scale.x = eyeScaleX * baseScale;
    //pRawMasks->eyeR.scale.y = FFLiiGetAdjustedEyeH(eyeScaleY * baseScale, leftEyeIndex);
    pRawMasks->eyeR.scale.y = eyeScaleY * baseScale;
    pRawMasks->eyeR.rot = eyeRotate;
    pRawMasks->eyeR.originPos = FFLI_ORIGIN_POSITION_LEFT;

    pRawMasks->eyeL.pos.x = (eyeSpacingX + 32) * baseScale;
    pRawMasks->eyeL.pos.y = eyePosY * baseScale;
    pRawMasks->eyeL.scale.x = eyeScaleX * baseScale;
    //pRawMasks->eyeL.scale.y = FFLiiGetAdjustedEyeH(eyeScaleY * baseScale, rightEyeIndex);
    pRawMasks->eyeL.scale.y = eyeScaleY * baseScale;
    pRawMasks->eyeL.rot = 360.0f - eyeRotate;
    pRawMasks->eyeL.originPos = FFLI_ORIGIN_POSITION_RIGHT;

    pRawMasks->eyebrowR.pos.x = (32 - eyebrowSpacingX) * baseScale;
    pRawMasks->eyebrowR.pos.y = eyebrowPosY * baseScale;
    pRawMasks->eyebrowR.scale.x = eyebrowScaleX * baseScale;
    pRawMasks->eyebrowR.scale.y = eyebrowScaleY * baseScale;
    pRawMasks->eyebrowR.rot = eyebrowRotate;
    pRawMasks->eyebrowR.originPos = FFLI_ORIGIN_POSITION_LEFT;

    pRawMasks->eyebrowL.pos.x = (eyebrowSpacingX + 32) * baseScale;
    pRawMasks->eyebrowL.pos.y = eyebrowPosY * baseScale;
    pRawMasks->eyebrowL.scale.x = eyebrowScaleX * baseScale;
    pRawMasks->eyebrowL.scale.y = eyebrowScaleY * baseScale;
    pRawMasks->eyebrowL.rot = 360.0f - eyebrowRotate;
    pRawMasks->eyebrowL.originPos = FFLI_ORIGIN_POSITION_RIGHT;

    pRawMasks->mouth.pos.x = 32 * baseScale;
    pRawMasks->mouth.pos.y = mouthPosY * baseScale;
    pRawMasks->mouth.scale.x = mouthScaleX * baseScale;
    // pRawMasks->mouth.scale.y = FFLiiGetAdjustedMouthH(mouthScaleY * baseScale, pCharInfo->parts.mouthType);
    pRawMasks->mouth.scale.y = mouthScaleY * baseScale;
    pRawMasks->mouth.rot = 0.0f;
    pRawMasks->mouth.originPos = FFLI_ORIGIN_POSITION_CENTER;

    pRawMasks->mustacheR.pos.x = 32 * baseScale;
    pRawMasks->mustacheR.pos.y = mustachePosY * baseScale;
    pRawMasks->mustacheR.scale.x = mustacheScaleX * baseScale;
    pRawMasks->mustacheR.scale.y = mustacheScaleY * baseScale;
    pRawMasks->mustacheR.rot = 0.0f;
    pRawMasks->mustacheR.originPos = FFLI_ORIGIN_POSITION_LEFT;

    pRawMasks->mustacheL.pos.x = 32 * baseScale;
    pRawMasks->mustacheL.pos.y = mustachePosY * baseScale;
    pRawMasks->mustacheL.scale.x = mustacheScaleX * baseScale;
    pRawMasks->mustacheL.scale.y = mustacheScaleY * baseScale;
    pRawMasks->mustacheL.rot = 0.0f;
    pRawMasks->mustacheL.originPos = FFLI_ORIGIN_POSITION_RIGHT;

    pRawMasks->mole.pos.x = molePosX * baseScale;
    pRawMasks->mole.pos.y = molePosY * baseScale;
    pRawMasks->mole.scale.x = moleScale * baseScale;
    pRawMasks->mole.scale.y = moleScale * baseScale;
    pRawMasks->mole.rot = 0.0f;
    pRawMasks->mole.originPos = FFLI_ORIGIN_POSITION_CENTER;
}

}
