#include <nn/ffl/FFLiManager.h>
#include <nn/ffl/FFLiPartsTextures.h>
#include <nn/ffl/FFLiResourceLoader.h>
#include <nn/ffl/FFLiTexture.h>
#include <nn/ffl/FFLiUtil.h>

#include <nn/ffl/detail/FFLiCharInfo.h>

#if RIO_IS_CAFE
#include <gx2/mem.h>
#endif // RIO_IS_CAFE

namespace {

static const FFLiEyeMouthTypeElement EYE_MOUTH_TYPE_ELEMENT[FFL_EXPRESSION_MAX] = {
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_0, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_1, FFLI_EYE_TEXTURE_TYPE_1 }, FFLI_MOUTH_TEXTURE_TYPE_0, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_1, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_2, FFLI_EYE_TEXTURE_TYPE_2 }, FFLI_MOUTH_TEXTURE_TYPE_2, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_3, FFLI_EYE_TEXTURE_TYPE_3 }, FFLI_MOUTH_TEXTURE_TYPE_0, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_4, FFLI_EYE_TEXTURE_TYPE_4 }, FFLI_MOUTH_TEXTURE_TYPE_0, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_3, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_1, FFLI_EYE_TEXTURE_TYPE_1 }, FFLI_MOUTH_TEXTURE_TYPE_3, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_3, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_2, FFLI_EYE_TEXTURE_TYPE_2 }, FFLI_MOUTH_TEXTURE_TYPE_3, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_3, FFLI_EYE_TEXTURE_TYPE_3 }, FFLI_MOUTH_TEXTURE_TYPE_3, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_4, FFLI_EYE_TEXTURE_TYPE_4 }, FFLI_MOUTH_TEXTURE_TYPE_3, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_5, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_0, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_5 }, FFLI_MOUTH_TEXTURE_TYPE_0, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_5, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_3, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_5 }, FFLI_MOUTH_TEXTURE_TYPE_3, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_5, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_5, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_5 }, FFLI_MOUTH_TEXTURE_TYPE_5, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_5, FFLI_EYE_TEXTURE_TYPE_5 }, FFLI_MOUTH_TEXTURE_TYPE_2, FFLI_EYEBROW_TEXTURE_TYPE_0 },

    // miitomo begins here

    { { FFLI_EYE_TEXTURE_TYPE_13, FFLI_EYE_TEXTURE_TYPE_13 }, FFLI_MOUTH_TEXTURE_TYPE_6, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_13, FFLI_EYE_TEXTURE_TYPE_13 }, FFLI_MOUTH_TEXTURE_TYPE_3, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_4, FFLI_EYE_TEXTURE_TYPE_4 }, FFLI_MOUTH_TEXTURE_TYPE_6, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_4, FFLI_EYE_TEXTURE_TYPE_4 }, FFLI_MOUTH_TEXTURE_TYPE_19, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_8, FFLI_EYE_TEXTURE_TYPE_8 }, FFLI_MOUTH_TEXTURE_TYPE_6, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_8, FFLI_EYE_TEXTURE_TYPE_8 }, FFLI_MOUTH_TEXTURE_TYPE_15, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_11, FFLI_EYE_TEXTURE_TYPE_11 }, FFLI_MOUTH_TEXTURE_TYPE_6, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_11, FFLI_EYE_TEXTURE_TYPE_11 }, FFLI_MOUTH_TEXTURE_TYPE_12, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_6, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_11, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_5, FFLI_EYE_TEXTURE_TYPE_5 }, FFLI_MOUTH_TEXTURE_TYPE_6, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_5, FFLI_EYE_TEXTURE_TYPE_5 }, FFLI_MOUTH_TEXTURE_TYPE_19, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_9, FFLI_EYE_TEXTURE_TYPE_9 }, FFLI_MOUTH_TEXTURE_TYPE_6, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_9, FFLI_EYE_TEXTURE_TYPE_9 }, FFLI_MOUTH_TEXTURE_TYPE_12, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_8, FFLI_EYE_TEXTURE_TYPE_24 }, FFLI_MOUTH_TEXTURE_TYPE_18, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_8, FFLI_EYE_TEXTURE_TYPE_24 }, FFLI_MOUTH_TEXTURE_TYPE_18, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_14, FFLI_EYE_TEXTURE_TYPE_14 }, FFLI_MOUTH_TEXTURE_TYPE_13, FFLI_EYEBROW_TEXTURE_TYPE_24 },
    { { FFLI_EYE_TEXTURE_TYPE_14, FFLI_EYE_TEXTURE_TYPE_14 }, FFLI_MOUTH_TEXTURE_TYPE_14, FFLI_EYEBROW_TEXTURE_TYPE_24 },
    { { FFLI_EYE_TEXTURE_TYPE_13, FFLI_EYE_TEXTURE_TYPE_13 }, FFLI_MOUTH_TEXTURE_TYPE_9, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_13, FFLI_EYE_TEXTURE_TYPE_13 }, FFLI_MOUTH_TEXTURE_TYPE_3, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_18, FFLI_EYE_TEXTURE_TYPE_18 }, FFLI_MOUTH_TEXTURE_TYPE_16, FFLI_EYEBROW_TEXTURE_TYPE_27 },
    { { FFLI_EYE_TEXTURE_TYPE_18, FFLI_EYE_TEXTURE_TYPE_18 }, FFLI_MOUTH_TEXTURE_TYPE_17, FFLI_EYEBROW_TEXTURE_TYPE_27 },
    { { FFLI_EYE_TEXTURE_TYPE_12, FFLI_EYE_TEXTURE_TYPE_12 }, FFLI_MOUTH_TEXTURE_TYPE_20, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_12, FFLI_EYE_TEXTURE_TYPE_12 }, FFLI_MOUTH_TEXTURE_TYPE_20, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_15, FFLI_EYE_TEXTURE_TYPE_15 }, FFLI_MOUTH_TEXTURE_TYPE_8, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_15, FFLI_EYE_TEXTURE_TYPE_15 }, FFLI_MOUTH_TEXTURE_TYPE_8, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_8, FFLI_EYE_TEXTURE_TYPE_8 }, FFLI_MOUTH_TEXTURE_TYPE_21, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_8, FFLI_EYE_TEXTURE_TYPE_8 }, FFLI_MOUTH_TEXTURE_TYPE_21, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_24, FFLI_EYE_TEXTURE_TYPE_23 }, FFLI_MOUTH_TEXTURE_TYPE_10, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_24, FFLI_EYE_TEXTURE_TYPE_23 }, FFLI_MOUTH_TEXTURE_TYPE_10, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_16, FFLI_EYE_TEXTURE_TYPE_16 }, FFLI_MOUTH_TEXTURE_TYPE_22, FFLI_EYEBROW_TEXTURE_TYPE_25 },
    { { FFLI_EYE_TEXTURE_TYPE_16, FFLI_EYE_TEXTURE_TYPE_16 }, FFLI_MOUTH_TEXTURE_TYPE_22, FFLI_EYEBROW_TEXTURE_TYPE_25 },
    { { FFLI_EYE_TEXTURE_TYPE_17, FFLI_EYE_TEXTURE_TYPE_17 }, FFLI_MOUTH_TEXTURE_TYPE_23, FFLI_EYEBROW_TEXTURE_TYPE_26 },
    { { FFLI_EYE_TEXTURE_TYPE_17, FFLI_EYE_TEXTURE_TYPE_17 }, FFLI_MOUTH_TEXTURE_TYPE_23, FFLI_EYEBROW_TEXTURE_TYPE_26 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_24, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_24, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_22, FFLI_EYE_TEXTURE_TYPE_21 }, FFLI_MOUTH_TEXTURE_TYPE_6, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_22, FFLI_EYE_TEXTURE_TYPE_21 }, FFLI_MOUTH_TEXTURE_TYPE_12, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_20, FFLI_EYE_TEXTURE_TYPE_19 }, FFLI_MOUTH_TEXTURE_TYPE_6, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_20, FFLI_EYE_TEXTURE_TYPE_19 }, FFLI_MOUTH_TEXTURE_TYPE_19, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_7, FFLI_EYE_TEXTURE_TYPE_7 }, FFLI_MOUTH_TEXTURE_TYPE_6, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_7, FFLI_EYE_TEXTURE_TYPE_7 }, FFLI_MOUTH_TEXTURE_TYPE_12, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_0, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_0, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_6, FFLI_EYEBROW_TEXTURE_TYPE_13 },
    { { FFLI_EYE_TEXTURE_TYPE_0, FFLI_EYE_TEXTURE_TYPE_0 }, FFLI_MOUTH_TEXTURE_TYPE_20, FFLI_EYEBROW_TEXTURE_TYPE_13 },
    { { FFLI_EYE_TEXTURE_TYPE_26, FFLI_EYE_TEXTURE_TYPE_25 }, FFLI_MOUTH_TEXTURE_TYPE_6, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_26, FFLI_EYE_TEXTURE_TYPE_25 }, FFLI_MOUTH_TEXTURE_TYPE_12, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_28, FFLI_EYE_TEXTURE_TYPE_28 }, FFLI_MOUTH_TEXTURE_TYPE_25, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_28, FFLI_EYE_TEXTURE_TYPE_28 }, FFLI_MOUTH_TEXTURE_TYPE_25, FFLI_EYEBROW_TEXTURE_TYPE_0 },
    { { FFLI_EYE_TEXTURE_TYPE_27, FFLI_EYE_TEXTURE_TYPE_27 }, FFLI_MOUTH_TEXTURE_TYPE_7, FFLI_EYEBROW_TEXTURE_TYPE_0 }
};


void ExpressionToEyeUseFlag(bool* pUseFlag, FFLExpressionFlag expressionFlag);
void ExpressionToMouthUseFlag(bool* pUseFlag, FFLExpressionFlag expressionFlag);
void ExpressionToEyebrowUseFlag(bool* pUseFlag, FFLExpressionFlag expressionFlag);

void DeleteTextures_Eye(FFLiPartsTextures* pPartsTextures, FFLExpressionFlag expressionFlag, bool isExpand);
void DeleteTextures_Mouth(FFLiPartsTextures* pPartsTextures, FFLExpressionFlag expressionFlag, bool isExpand);
void DeleteTextures_Eyebrow(FFLiPartsTextures* pPartsTextures, FFLExpressionFlag expressionFlag, bool isExpand);
void DeleteTexture_Mustache(FFLiPartsTextures* pPartsTextures, bool isExpand);
void DeleteTexture_Mole(FFLiPartsTextures* pPartsTextures, bool isExpand);

#if RIO_IS_CAFE
void InvalidateTexture(const GX2Texture& texture);
#endif // RIO_IS_CAFE

void InvalidateTextures(rio::Texture2D** ppTextures, u32 count);

}

s32 FFLiCharInfoAndTypeToEyeIndex(const FFLiCharInfo* pCharInfo, FFLiEyeTextureType type)
{
    switch (type)
    {
    case FFLI_EYE_TEXTURE_TYPE_0:
    case FFLI_EYE_TEXTURE_TYPE_2:
        return pCharInfo->parts.eyeType;
    case FFLI_EYE_TEXTURE_TYPE_1:
    case FFLI_EYE_TEXTURE_TYPE_9:
        return 60;
    case FFLI_EYE_TEXTURE_TYPE_3:
    case FFLI_EYE_TEXTURE_TYPE_10:
        return 61;
    case FFLI_EYE_TEXTURE_TYPE_4:
        return 26;
    case FFLI_EYE_TEXTURE_TYPE_5:
    case FFLI_EYE_TEXTURE_TYPE_8:
        return 47;
    case FFLI_EYE_TEXTURE_TYPE_6:
        return 18;
    case FFLI_EYE_TEXTURE_TYPE_7:
        return 23;
    case FFLI_EYE_TEXTURE_TYPE_11:
        return 62;
    case FFLI_EYE_TEXTURE_TYPE_12:
        return 63;
    case FFLI_EYE_TEXTURE_TYPE_13:
        return 64;
    case FFLI_EYE_TEXTURE_TYPE_14:
        return 65;
    case FFLI_EYE_TEXTURE_TYPE_15:
        return 66;
    case FFLI_EYE_TEXTURE_TYPE_16:
        return 67;
    case FFLI_EYE_TEXTURE_TYPE_17:
        return 68;
    case FFLI_EYE_TEXTURE_TYPE_18:
        return 69;
    case FFLI_EYE_TEXTURE_TYPE_19:
        return 70;
    case FFLI_EYE_TEXTURE_TYPE_20:
        return 71;
    case FFLI_EYE_TEXTURE_TYPE_21:
        return 72;
    case FFLI_EYE_TEXTURE_TYPE_22:
        return 73;
    case FFLI_EYE_TEXTURE_TYPE_23:
        return 74;
    case FFLI_EYE_TEXTURE_TYPE_24:
        return 75;
    case FFLI_EYE_TEXTURE_TYPE_25:
        return 76;
    case FFLI_EYE_TEXTURE_TYPE_26:
        return 77;
    case FFLI_EYE_TEXTURE_TYPE_27:
        return 78;
    case FFLI_EYE_TEXTURE_TYPE_28:
        return 79;
    default:
        return 0;
    }
}

s32 FFLiCharInfoAndTypeToMouthIndex(const FFLiCharInfo* pCharInfo, FFLiMouthTextureType type)
{
    switch (type)
    {
    case FFLI_MOUTH_TEXTURE_TYPE_0:
        return pCharInfo->parts.mouthType;
    case FFLI_MOUTH_TEXTURE_TYPE_1:
        return 10;
    case FFLI_MOUTH_TEXTURE_TYPE_2:
        return 12;
    case FFLI_MOUTH_TEXTURE_TYPE_3:
        return 36;
    case FFLI_MOUTH_TEXTURE_TYPE_4:
    case FFLI_MOUTH_TEXTURE_TYPE_5:
        return 19;
    case FFLI_MOUTH_TEXTURE_TYPE_6:
        return 23;
    case FFLI_MOUTH_TEXTURE_TYPE_7:
        return 2;
    case FFLI_MOUTH_TEXTURE_TYPE_8:
        return 14;
    case FFLI_MOUTH_TEXTURE_TYPE_9:
        return 15;
    case FFLI_MOUTH_TEXTURE_TYPE_10:
        return 32;
    case FFLI_MOUTH_TEXTURE_TYPE_11:
        return 37;
    case FFLI_MOUTH_TEXTURE_TYPE_12:
        return 38;
    case FFLI_MOUTH_TEXTURE_TYPE_13:
        return 39;
    case FFLI_MOUTH_TEXTURE_TYPE_14:
        return 40;
    case FFLI_MOUTH_TEXTURE_TYPE_15:
        return 41;
    case FFLI_MOUTH_TEXTURE_TYPE_16:
        return 42;
    case FFLI_MOUTH_TEXTURE_TYPE_17:
        return 43;
    case FFLI_MOUTH_TEXTURE_TYPE_18:
        return 44;
    case FFLI_MOUTH_TEXTURE_TYPE_19:
        return 45;
    case FFLI_MOUTH_TEXTURE_TYPE_20:
        return 46;
    case FFLI_MOUTH_TEXTURE_TYPE_21:
        return 47;
    case FFLI_MOUTH_TEXTURE_TYPE_22:
        return 48;
    case FFLI_MOUTH_TEXTURE_TYPE_23:
        return 49;
    case FFLI_MOUTH_TEXTURE_TYPE_24:
        return 50;
    case FFLI_MOUTH_TEXTURE_TYPE_25:
        return 51;
    default:
        return 0;
    }
}

s32 FFLiCharInfoAndTypeToEyebrowIndex(const FFLiCharInfo* pCharInfo, FFLiEyebrowTextureType type)
{
    if (type == 0)
        return static_cast<FFLiEyebrowTextureType>(pCharInfo->parts.eyebrowType);
    return type;
}

FFLResult FFLiLoadPartsTextures(FFLiPartsTextures* pPartsTextures, const FFLiCharInfo* pCharInfo, FFLExpressionFlag expressionFlag, FFLiResourceLoader* pResLoader)
{
    rio::MemUtil::set(pPartsTextures, 0, sizeof(FFLiPartsTextures));

    bool useFlag[FFLI_EYE_TEXTURE_TYPE_MAX];    // max(FFLI_EYE_TEXTURE_TYPE_MAX, FFLI_MOUTH_TEXTURE_TYPE_MAX)

    {
        ExpressionToEyeUseFlag(useFlag, expressionFlag);

        for (u32 i = 0; i < FFLI_EYE_TEXTURE_TYPE_MAX; i++)
        {
            if (useFlag[i])
            {
                FFLResult result = FFLiLoadTextureWithAllocate(&(pPartsTextures->pTexturesEye[i]), FFLI_TEXTURE_PARTS_TYPE_EYE, FFLiCharInfoAndTypeToEyeIndex(pCharInfo, FFLiEyeTextureType(i)), pResLoader);
                if (result != FFL_RESULT_OK)
                {
                    for (u32 j = i; j > 0; j--)
                        if (useFlag[j - 1])
                            FFLiDeleteTexture(&(pPartsTextures->pTexturesEye[j - 1]), pResLoader->IsExpand());

                    return result;
                }
            }
        }
    }

    {
        ExpressionToMouthUseFlag(useFlag, expressionFlag);

        for (u32 i = 0; i < FFLI_MOUTH_TEXTURE_TYPE_MAX; i++)
        {
            if (useFlag[i])
            {
                FFLResult result = FFLiLoadTextureWithAllocate(&(pPartsTextures->pTexturesMouth[i]), FFLI_TEXTURE_PARTS_TYPE_MOUTH, FFLiCharInfoAndTypeToMouthIndex(pCharInfo, FFLiMouthTextureType(i)), pResLoader);
                if (result != FFL_RESULT_OK)
                {
                    for (u32 j = i; j > 0; j--)
                        if (useFlag[j - 1])
                            FFLiDeleteTexture(&(pPartsTextures->pTexturesMouth[j - 1]), pResLoader->IsExpand());

                    DeleteTextures_Eye(pPartsTextures, expressionFlag, pResLoader->IsExpand());
                    return result;
                }
            }
        }
    }

    // NOTE: EYEBROW, MUSTACHE, AND MOLE ARE ALL BLANK WHEN THEY ARE ZERO

    FFLResult result;

    {
        ExpressionToEyebrowUseFlag(useFlag, expressionFlag);

        for (u32 i = 0; i < FFLI_EYEBROW_TEXTURE_TYPE_MAX; i++)
        {
            if (useFlag[i])
            {
                u32 eyebrowIndex = FFLiCharInfoAndTypeToEyebrowIndex(pCharInfo, FFLiEyebrowTextureType(i));
                // skip eyebrow type 23 specifically bc it is blank
                if (eyebrowIndex == 23)
                    continue;
                FFLResult result = FFLiLoadTextureWithAllocate(&(pPartsTextures->pTexturesEyebrow[i]), FFLI_TEXTURE_PARTS_TYPE_EYEBROW, eyebrowIndex, pResLoader);
                // here eyebrow type corresponds directly to the resource type
                if (result != FFL_RESULT_OK)
                {
                    for (u32 j = i; j > 0; j--)
                        if (useFlag[j - 1])
                            FFLiDeleteTexture(&(pPartsTextures->pTexturesEyebrow[j - 1]), pResLoader->IsExpand());

                    DeleteTextures_Mouth(pPartsTextures, expressionFlag, pResLoader->IsExpand());
                    DeleteTextures_Eye(pPartsTextures, expressionFlag, pResLoader->IsExpand());
                    return result;
                }
            }
        }
    }

    if (pCharInfo->parts.mustacheType != 0) {
        result = FFLiLoadTextureWithAllocate(&pPartsTextures->pTextureMustache, FFLI_TEXTURE_PARTS_TYPE_MUSTACHE, pCharInfo->parts.mustacheType, pResLoader);
        if (result != FFL_RESULT_OK)
        {
            DeleteTextures_Eyebrow(pPartsTextures, expressionFlag, pResLoader->IsExpand());
            DeleteTextures_Mouth(pPartsTextures, expressionFlag, pResLoader->IsExpand());
            DeleteTextures_Eye(pPartsTextures, expressionFlag, pResLoader->IsExpand());
            return result;
        }
    } else {
        pPartsTextures->pTextureMustache = NULL;
    }

    if (pCharInfo->parts.moleType != 0) {
        result = FFLiLoadTextureWithAllocate(&pPartsTextures->pTextureMole, FFLI_TEXTURE_PARTS_TYPE_MOLE, pCharInfo->parts.moleType, pResLoader);
        if (result != FFL_RESULT_OK)
        {
            DeleteTexture_Mustache(pPartsTextures, pResLoader->IsExpand());
            DeleteTextures_Eyebrow(pPartsTextures, expressionFlag, pResLoader->IsExpand());
            DeleteTextures_Mouth(pPartsTextures, expressionFlag, pResLoader->IsExpand());
            DeleteTextures_Eye(pPartsTextures, expressionFlag, pResLoader->IsExpand());
            return result;
        }
    } else {
        pPartsTextures->pTextureMole = NULL;
    }

    return FFL_RESULT_OK;
}

void FFLiDeletePartsTextures(FFLiPartsTextures* pPartsTextures, FFLExpressionFlag expressionFlag, FFLResourceType resourceType)
{
    RIO_ASSERT(FFLiManager::IsConstruct());
    bool isExpand = FFLiManager::GetInstance()->GetResourceManager().IsExpand(resourceType);

    DeleteTexture_Mole(pPartsTextures, isExpand);
    DeleteTexture_Mustache(pPartsTextures, isExpand);
    DeleteTextures_Eyebrow(pPartsTextures, expressionFlag, isExpand);
    DeleteTextures_Mouth(pPartsTextures, expressionFlag, isExpand);
    DeleteTextures_Eye(pPartsTextures, expressionFlag, isExpand);
}

void FFLiInvalidatePartsTextures(FFLiPartsTextures* pPartsTextures)
{
    InvalidateTextures(pPartsTextures->pTexturesEye, FFLI_EYE_TEXTURE_TYPE_MAX);
    InvalidateTextures(pPartsTextures->pTexturesMouth, FFLI_MOUTH_TEXTURE_TYPE_MAX);
    InvalidateTextures(pPartsTextures->pTexturesEyebrow, FFLI_EYEBROW_TEXTURE_TYPE_MAX);

#if RIO_IS_CAFE
    /*if (pPartsTextures->pTextureEyebrow != NULL)
        InvalidateTexture(pPartsTextures->pTextureEyebrow->getNativeTexture());
    */

    if (pPartsTextures->pTextureMustache != NULL)
        InvalidateTexture(pPartsTextures->pTextureMustache->getNativeTexture());

    if (pPartsTextures->pTextureMole != NULL)
        InvalidateTexture(pPartsTextures->pTextureMole->getNativeTexture());
#endif // RIO_IS_CAFE
}

const FFLiEyeMouthTypeElement& FFLiGetEyeMouthTypeElement(FFLExpression expression)
{
    return EYE_MOUTH_TYPE_ELEMENT[expression];
}

u32 FFLiGetMaxMouthNum(FFLExpressionFlag expressionFlagCount)
{
    return FFLiMin<u32>(expressionFlagCount, FFLI_MOUTH_TEXTURE_TYPE_MAX);
}

u32 FFLiGetMaxEyeNum(FFLExpressionFlag expressionFlagCount)
{
    return FFLiMin<u32>(expressionFlagCount + 1, FFLI_EYE_TEXTURE_TYPE_MAX);
}

u32 FFLiGetMaxEyebrowNum(FFLExpressionFlag expressionFlagCount)
{
    return FFLiMin<u32>(expressionFlagCount, FFLI_EYEBROW_TEXTURE_TYPE_MAX);
}

namespace {

void ExpressionToEyeUseFlag(bool* pUseFlag, FFLExpressionFlag expressionFlag)
{
    rio::MemUtil::set(pUseFlag, 0, sizeof(bool) * FFLI_EYE_TEXTURE_TYPE_MAX);

    for (u32 i = 0; i < FFL_EXPRESSION_LIMIT; i++)
    {
        if (expressionFlag & static_cast<FFLExpressionFlag>(1) << i)
        {
            pUseFlag[EYE_MOUTH_TYPE_ELEMENT[i].eyeTextureType[0]] = true;
            pUseFlag[EYE_MOUTH_TYPE_ELEMENT[i].eyeTextureType[1]] = true;
        }
    }
}

void ExpressionToMouthUseFlag(bool* pUseFlag, FFLExpressionFlag expressionFlag)
{
    rio::MemUtil::set(pUseFlag, 0, sizeof(bool) * FFLI_MOUTH_TEXTURE_TYPE_MAX);

    for (u32 i = 0; i < FFL_EXPRESSION_LIMIT; i++)
        if (expressionFlag & static_cast<FFLExpressionFlag>(1) << i)
            pUseFlag[EYE_MOUTH_TYPE_ELEMENT[i].mouthTextureType] = true;

}

void ExpressionToEyebrowUseFlag(bool* pUseFlag, FFLExpressionFlag expressionFlag)
{
    rio::MemUtil::set(pUseFlag, 0, sizeof(bool) * FFLI_EYEBROW_TEXTURE_TYPE_MAX);

    for (u32 i = 0; i < FFL_EXPRESSION_LIMIT; i++)
        if (expressionFlag & static_cast<FFLExpressionFlag>(1) << i)
            pUseFlag[EYE_MOUTH_TYPE_ELEMENT[i].eyebrowTextureType] = true;

}

void DeleteTextures_Eye(FFLiPartsTextures* pPartsTextures, FFLExpressionFlag expressionFlag, bool isExpand)
{
    bool useFlag[FFLI_EYE_TEXTURE_TYPE_MAX];
    ExpressionToEyeUseFlag(useFlag, expressionFlag);

    for (u32 j = FFLI_EYE_TEXTURE_TYPE_MAX; j > 0; j--)
        if (useFlag[j - 1])
            FFLiDeleteTexture(&(pPartsTextures->pTexturesEye[j - 1]), isExpand);
}

void DeleteTextures_Mouth(FFLiPartsTextures* pPartsTextures, FFLExpressionFlag expressionFlag, bool isExpand)
{
    bool useFlag[FFLI_MOUTH_TEXTURE_TYPE_MAX];
    ExpressionToMouthUseFlag(useFlag, expressionFlag);

    for (u32 j = FFLI_MOUTH_TEXTURE_TYPE_MAX; j > 0; j--)
        if (useFlag[j - 1])
            FFLiDeleteTexture(&(pPartsTextures->pTexturesMouth[j - 1]), isExpand);
}

void DeleteTextures_Eyebrow(FFLiPartsTextures* pPartsTextures, FFLExpressionFlag expressionFlag, bool isExpand)
{
    bool useFlag[FFLI_EYEBROW_TEXTURE_TYPE_MAX];
    ExpressionToEyebrowUseFlag(useFlag, expressionFlag);

    for (u32 j = FFLI_EYEBROW_TEXTURE_TYPE_MAX; j > 0; j--)
        if (useFlag[j - 1] && pPartsTextures->pTexturesEyebrow[j - 1] != NULL)
            FFLiDeleteTexture(&(pPartsTextures->pTexturesEyebrow[j - 1]), isExpand);
}

void DeleteTexture_Mustache(FFLiPartsTextures* pPartsTextures, bool isExpand)
{
    if (pPartsTextures->pTextureMustache != NULL)
        FFLiDeleteTexture(&pPartsTextures->pTextureMustache, isExpand);
}

void DeleteTexture_Mole(FFLiPartsTextures* pPartsTextures, bool isExpand)
{
    if (pPartsTextures->pTextureMole != NULL)
        FFLiDeleteTexture(&pPartsTextures->pTextureMole, isExpand);
}

#if RIO_IS_CAFE

void InvalidateTexture(const GX2Texture& texture)
{
    if (texture.surface.image)
        GX2Invalidate(GX2_INVALIDATE_MODE_TEXTURE, texture.surface.image, texture.surface.imageSize);

    if (texture.surface.mipmaps)
        GX2Invalidate(GX2_INVALIDATE_MODE_TEXTURE, texture.surface.mipmaps, texture.surface.mipmapSize);
}

#endif // RIO_IS_CAFE

void InvalidateTextures(rio::Texture2D** ppTextures, u32 count)
{
#if RIO_IS_CAFE
    for (u32 i = 0; i < count; i++)
        if (ppTextures[i] != NULL)
            InvalidateTexture(ppTextures[i]->getNativeTexture());
#endif // RIO_IS_CAFE
}

}
