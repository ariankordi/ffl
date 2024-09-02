#ifndef FFLI_PARTS_TEXTURES_H_
#define FFLI_PARTS_TEXTURES_H_

#include <nn/ffl/FFLExpression.h>
#include <nn/ffl/FFLResourceType.h>
#include <nn/ffl/FFLResult.h>

#include <gpu/rio_Texture.h>

enum FFLiEyeTextureType {
    FFLI_EYE_TEXTURE_TYPE_0   = 0,
    FFLI_EYE_TEXTURE_TYPE_1   = 1,
    FFLI_EYE_TEXTURE_TYPE_2   = 2,
    FFLI_EYE_TEXTURE_TYPE_3   = 3,
    FFLI_EYE_TEXTURE_TYPE_4   = 4,
    FFLI_EYE_TEXTURE_TYPE_5   = 5,
    FFLI_EYE_TEXTURE_TYPE_6   = 6,
    FFLI_EYE_TEXTURE_TYPE_7   = 7,
    FFLI_EYE_TEXTURE_TYPE_8   = 8,
    FFLI_EYE_TEXTURE_TYPE_9   = 9,
    FFLI_EYE_TEXTURE_TYPE_10  = 10,
    FFLI_EYE_TEXTURE_TYPE_11  = 11,
    FFLI_EYE_TEXTURE_TYPE_12  = 12,
    FFLI_EYE_TEXTURE_TYPE_13  = 13,
    FFLI_EYE_TEXTURE_TYPE_14  = 14,
    FFLI_EYE_TEXTURE_TYPE_15  = 15,
    FFLI_EYE_TEXTURE_TYPE_16  = 16,
    FFLI_EYE_TEXTURE_TYPE_17  = 17,
    FFLI_EYE_TEXTURE_TYPE_18  = 18,
    FFLI_EYE_TEXTURE_TYPE_19  = 19,
    FFLI_EYE_TEXTURE_TYPE_20  = 20,
    FFLI_EYE_TEXTURE_TYPE_21  = 21,
    FFLI_EYE_TEXTURE_TYPE_22  = 22,
    FFLI_EYE_TEXTURE_TYPE_23  = 23,
    FFLI_EYE_TEXTURE_TYPE_24  = 24,
    FFLI_EYE_TEXTURE_TYPE_25  = 25,
    FFLI_EYE_TEXTURE_TYPE_26  = 26,
    FFLI_EYE_TEXTURE_TYPE_27  = 27,
    FFLI_EYE_TEXTURE_TYPE_28  = 28,
    FFLI_EYE_TEXTURE_TYPE_MAX = 29
};

enum FFLiMouthTextureType {
    FFLI_MOUTH_TEXTURE_TYPE_0   = 0,
    FFLI_MOUTH_TEXTURE_TYPE_1   = 1,
    FFLI_MOUTH_TEXTURE_TYPE_2   = 2,
    FFLI_MOUTH_TEXTURE_TYPE_3   = 3,
    FFLI_MOUTH_TEXTURE_TYPE_4   = 4,
    FFLI_MOUTH_TEXTURE_TYPE_5   = 5,
    FFLI_MOUTH_TEXTURE_TYPE_6   = 6,
    FFLI_MOUTH_TEXTURE_TYPE_7   = 7,
    FFLI_MOUTH_TEXTURE_TYPE_8   = 8,
    FFLI_MOUTH_TEXTURE_TYPE_9   = 9,
    FFLI_MOUTH_TEXTURE_TYPE_10  = 10,
    FFLI_MOUTH_TEXTURE_TYPE_11  = 11,
    FFLI_MOUTH_TEXTURE_TYPE_12  = 12,
    FFLI_MOUTH_TEXTURE_TYPE_13  = 13,
    FFLI_MOUTH_TEXTURE_TYPE_14  = 14,
    FFLI_MOUTH_TEXTURE_TYPE_15  = 15,
    FFLI_MOUTH_TEXTURE_TYPE_16  = 16,
    FFLI_MOUTH_TEXTURE_TYPE_17  = 17,
    FFLI_MOUTH_TEXTURE_TYPE_18  = 18,
    FFLI_MOUTH_TEXTURE_TYPE_19  = 19,
    FFLI_MOUTH_TEXTURE_TYPE_20  = 20,
    FFLI_MOUTH_TEXTURE_TYPE_21  = 21,
    FFLI_MOUTH_TEXTURE_TYPE_22  = 22,
    FFLI_MOUTH_TEXTURE_TYPE_23  = 23,
    FFLI_MOUTH_TEXTURE_TYPE_24  = 24,
    FFLI_MOUTH_TEXTURE_TYPE_25  = 25,
    FFLI_MOUTH_TEXTURE_TYPE_MAX = 26
};

enum FFLiEyebrowTextureType {
    FFLI_EYEBROW_TEXTURE_TYPE_0   = 0,
    FFLI_EYEBROW_TEXTURE_TYPE_13  = 13,
    FFLI_EYEBROW_TEXTURE_TYPE_24  = 24,
    FFLI_EYEBROW_TEXTURE_TYPE_25  = 25,
    FFLI_EYEBROW_TEXTURE_TYPE_26  = 26,
    FFLI_EYEBROW_TEXTURE_TYPE_27  = 27,
    FFLI_EYEBROW_TEXTURE_TYPE_MAX = 28
};

struct FFLiPartsTextures
{
    rio::Texture2D* pTexturesEye[FFLI_EYE_TEXTURE_TYPE_MAX];
    rio::Texture2D* pTexturesMouth[FFLI_MOUTH_TEXTURE_TYPE_MAX];
    rio::Texture2D* pTexturesEyebrow[FFLI_EYEBROW_TEXTURE_TYPE_MAX];
    rio::Texture2D* pTextureMustache;
    rio::Texture2D* pTextureMole;
};
NN_STATIC_ASSERT32(sizeof(FFLiPartsTextures) == 0x154);

struct FFLiEyeMouthTypeElement
{
    FFLiEyeTextureType      eyeTextureType[2];
    FFLiMouthTextureType    mouthTextureType;
    FFLiEyebrowTextureType  eyebrowTextureType;
};
NN_STATIC_ASSERT(sizeof(FFLiEyeMouthTypeElement) == 0x10);

struct  FFLiCharInfo;
class   FFLiResourceLoader;

s32 FFLiCharInfoAndTypeToEyeIndex(const FFLiCharInfo* pCharInfo, FFLiEyeTextureType type);
s32 FFLiCharInfoAndTypeToMouthIndex(const FFLiCharInfo* pCharInfo, FFLiMouthTextureType type);
s32 FFLiCharInfoAndTypeToEyebrowIndex(const FFLiCharInfo* pCharInfo, FFLiEyebrowTextureType type);

FFLResult FFLiLoadPartsTextures(FFLiPartsTextures* pPartsTextures, const FFLiCharInfo* pCharInfo, FFLExpressionFlag expressionFlag, FFLiResourceLoader* pResLoader);
void FFLiDeletePartsTextures(FFLiPartsTextures* pPartsTextures, FFLExpressionFlag expressionFlag, FFLResourceType resourceType);

void FFLiInvalidatePartsTextures(FFLiPartsTextures* pPartsTextures);

const FFLiEyeMouthTypeElement& FFLiGetEyeMouthTypeElement(FFLExpression expression);

u32 FFLiGetMaxMouthNum(FFLExpressionFlag expressionFlagCount);
u32 FFLiGetMaxEyeNum(FFLExpressionFlag expressionFlagCount);
u32 FFLiGetMaxEyebrowNum(FFLExpressionFlag expressionFlagCount);

#endif // FFLI_PARTS_TEXTURES_H_
