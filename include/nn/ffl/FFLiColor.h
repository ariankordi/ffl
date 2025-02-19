#ifndef FFLI_COLOR_H_
#define FFLI_COLOR_H_

#include <nn/ffl/types.h>

#include <nn/ffl/FFLColor.h>

void FFLiSetLinearGammaColor(bool isLinear); // Deleted in NSMBU
void FFLiSetOffScreenSrgbFetch(bool isLinear); // Deleted in NSMBU

bool FFLiUseOffScreenSrgbFetch();
bool FFLiGetOffScreenSrgbFetch(); // Deleted in NSMBU

// FFLSetLinearGammaMode(u32): in FFLGlobal.cpp

struct FFLColor;

enum FFLiColorType
{
    FFLI_COLOR_TYPE_FACELINE_COLOR_0    = 0,
    FFLI_COLOR_TYPE_HAIR_COLOR_0        = FFLI_COLOR_TYPE_FACELINE_COLOR_0  + FFLI_FACELINE_COLOR_NUM,
    FFLI_COLOR_TYPE_EYE_COLOR_R_0       = FFLI_COLOR_TYPE_HAIR_COLOR_0      + FFLI_HAIR_COLOR_NUM,
    FFLI_COLOR_TYPE_EYE_COLOR_G         = FFLI_COLOR_TYPE_EYE_COLOR_R_0     + FFLI_EYE_COLOR_R_NUM,
    FFLI_COLOR_TYPE_EYE_COLOR_B_0,   // = FFLI_COLOR_TYPE_EYE_COLOR_G       + 1,
    FFLI_COLOR_TYPE_GLASS_COLOR_0       = FFLI_COLOR_TYPE_EYE_COLOR_B_0     + FFLI_EYE_COLOR_B_NUM,
    FFLI_COLOR_TYPE_MOUTH_COLOR_R_0     = FFLI_COLOR_TYPE_GLASS_COLOR_0     + FFLI_GLASS_COLOR_NUM,
    FFLI_COLOR_TYPE_MOUTH_COLOR_G_0     = FFLI_COLOR_TYPE_MOUTH_COLOR_R_0   + FFLI_MOUTH_COLOR_R_NUM,
    FFLI_COLOR_TYPE_MOUTH_COLOR_B       = FFLI_COLOR_TYPE_MOUTH_COLOR_G_0   + FFLI_MOUTH_COLOR_G_NUM,
    FFLI_COLOR_TYPE_MOLE_COLOR,      // = FFLI_COLOR_TYPE_MOUTH_COLOR_B     + 1,
    FFLI_COLOR_TYPE_FAVORITE_COLOR_0,// = FFLI_COLOR_TYPE_MOLE_COLOR        + 1,
    FFLI_COLOR_TYPE_FACE_LINE           = FFLI_COLOR_TYPE_FAVORITE_COLOR_0  + FFLI_FAVORITE_COLOR_NUM,
    FFLI_COLOR_TYPE_MAX              // = FFLI_COLOR_TYPE_FACE_LINE         + 1
};
NN_STATIC_ASSERT(FFLI_COLOR_TYPE_MAX == 55);

enum FFLiContainerType
{
    FFLI_CONTAINER_TYPE_NORMAL  = 0,
    FFLI_CONTAINER_TYPE_SRGB    = 1,
    FFLI_CONTAINER_TYPE_MAX     = 2
};

struct FFLiColorContainer
{
    FFLColor    colors[FFLI_COLOR_TYPE_MAX];
};

const FFLColor& FFLiGetSrgbFetchFacelineColor(s32 index);
const FFLColor& FFLiGetSrgbFetchBeardColor(s32 index);
const FFLColor& FFLiGetSrgbFetchEyebrowColor(s32 index);
const FFLColor& FFLiGetSrgbFetchMustacheColor(s32 index);
const FFLColor& FFLiGetSrgbFetchHairColor(s32 index);       // Deleted in NSMBU
const FFLColor& FFLiGetSrgbFetchEyeColorR(s32 index);
const FFLColor& FFLiGetSrgbFetchEyeColorG(s32 index);
const FFLColor& FFLiGetSrgbFetchEyeColorB(s32 index);
const FFLColor& FFLiGetSrgbFetchGlassColor(s32 index);      // Deleted in NSMBU
const FFLColor& FFLiGetSrgbFetchMouthColorR(s32 index);
const FFLColor& FFLiGetSrgbFetchMouthColorG(s32 index);
const FFLColor& FFLiGetSrgbFetchMouthColorB(s32 index);
const FFLColor& FFLiGetSrgbFetchMoleColor();
const FFLColor& FFLiGetSrgbFetchFavoriteColor(s32 index);   // Deleted in NSMBU
const FFLColor& FFLiGetSrgbFetchFaceLine();                 // Deleted in NSMBU
const FFLColor& FFLiGetSrgbFetchCapColor(s32 index);        // Deleted in NSMBU
const FFLColor& FFLiGetSrgbFetchNoselineColor();            // Deleted in NSMBU

const FFLColor& FFLiGetFacelineColor(s32 index);
const FFLColor& FFLiGetBeardColor(s32 index);               // Deleted in NSMBU
const FFLColor& FFLiGetEyebrowColor(s32 index);             // Deleted in NSMBU
const FFLColor& FFLiGetMustacheColor(s32 index);            // Deleted in NSMBU
const FFLColor& FFLiGetHairColor(s32 index);
const FFLColor& FFLiGetEyeColorR(s32 index);                // Deleted in NSMBU
const FFLColor& FFLiGetEyeColorG(s32 index);                // Deleted in NSMBU
const FFLColor& FFLiGetEyeColorB(s32 index);                // Deleted in NSMBU
const FFLColor& FFLiGetGlassColor(s32 index);
const FFLColor& FFLiGetMouthColorR(s32 index);              // Deleted in NSMBU
const FFLColor& FFLiGetMouthColorG(s32 index);              // Deleted in NSMBU
const FFLColor& FFLiGetMouthColorB(s32 index);              // Deleted in NSMBU
const FFLColor& FFLiGetMoleColor();                         // Deleted in NSMBU
const FFLColor& FFLiGetFavoriteColor(s32 index);
const FFLColor& FFLiGetFaceLine();
const FFLColor& FFLiGetCapColor(s32 index);
const FFLColor& FFLiGetNoselineColor();

u32 FFLiGetColorNum(FFLiColorType type); // Deleted in NSMBU

void FFLiSetColor(FFLColor color, FFLiColorType type, u32 index); // Deleted in NSMBU
void FFLiGetColor(FFLColor* pColor, FFLiColorType type, u32 index); // Deleted in NSMBU

void FFLiSetColorContainer(const FFLiColorContainer* pContainer, FFLiContainerType type); // Deleted in NSMBU
void FFLiGetColorContainer(FFLiColorContainer* pContainer, FFLiContainerType type); // Deleted in NSMBU


// Somewhat of a HACK: Mark s32 color values as being colors
// meant to be looked up in the common color table (Switch Mii colors)
#define FFLI_NN_MII_COMMON_COLOR_MASK ~(1 << 31)
#define FFLI_NN_MII_COMMON_COLOR_ENABLE_MASK (1 << 31)

#define FFLI_NN_MII_COMMON_COLOR_MAX 100
#define FFLI_NN_MII_FACELINE_COLOR_MAX 10

#endif // FFLI_COLOR_H_
