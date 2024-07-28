#include <nn/ffl/FFLColor.h>

#include <nn/ffl/FFLiColor.h>

#include <cstring>

namespace {

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

FFLiColorContainer s_ColorContainer[FFLI_CONTAINER_TYPE_MAX];
FFLiContainerType s_ContainerType = FFLI_CONTAINER_TYPE_NORMAL;
bool s_UseOffScreenSrgbFetch = true;
bool s_IsIntializedColorContainer = false;

}

static bool UseOffScreenSrgbFetch()
{
    return s_UseOffScreenSrgbFetch;
}

bool FFLiUseOffScreenSrgbFetch()
{
    return UseOffScreenSrgbFetch() && s_ContainerType == FFLI_CONTAINER_TYPE_SRGB;
}

namespace {

const FFLiColorContainer& GetSrgbFetchColorContainer();
const FFLiColorContainer& GetColorContainer();

const FFLColor& GetFacelineColor(const FFLiColorContainer& container, s32 index);
const FFLColor& GetBeardColor(const FFLiColorContainer& container, s32 index);
const FFLColor& GetEyebrowColor(const FFLiColorContainer& container, s32 index);
const FFLColor& GetMustacheColor(const FFLiColorContainer& container, s32 index);
const FFLColor& GetHairColor(const FFLiColorContainer& container, s32 index);
const FFLColor& GetEyeColorR(const FFLiColorContainer& container, s32 index);
const FFLColor& GetEyeColorG(const FFLiColorContainer& container, s32 index);
const FFLColor& GetEyeColorB(const FFLiColorContainer& container, s32 index);
const FFLColor& GetGlassColor(const FFLiColorContainer& container, s32 index);
const FFLColor& GetMouthColorR(const FFLiColorContainer& container, s32 index);
const FFLColor& GetMouthColorG(const FFLiColorContainer& container, s32 index);
const FFLColor& GetMouthColorB(const FFLiColorContainer& container, s32 index);
const FFLColor& GetMoleColor(const FFLiColorContainer& container);
const FFLColor& GetFavoriteColor(const FFLiColorContainer& container, s32 index);
const FFLColor& GetFaceLine(const FFLiColorContainer& container);
const FFLColor& GetCapColor(const FFLiColorContainer& container, s32 index);
const FFLColor& GetNoselineColor(const FFLiColorContainer& container);

}

const FFLColor& FFLiGetSrgbFetchFacelineColor(s32 index)
{
    return GetFacelineColor(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchBeardColor(s32 index)
{
    return GetBeardColor(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchEyebrowColor(s32 index)
{
    return GetEyebrowColor(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchMustacheColor(s32 index)
{
    return GetMustacheColor(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchHairColor(s32 index)
{
    return GetHairColor(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchEyeColorR(s32 index)
{
    return GetEyeColorR(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchEyeColorG(s32 index)
{
    return GetEyeColorG(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchEyeColorB(s32 index)
{
    return GetEyeColorB(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchGlassColor(s32 index)
{
    return GetGlassColor(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchMouthColorR(s32 index)
{
    return GetMouthColorR(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchMouthColorG(s32 index)
{
    return GetMouthColorG(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchMouthColorB(s32 index)
{
    return GetMouthColorB(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchMoleColor()
{
    return GetMoleColor(GetSrgbFetchColorContainer());
}

const FFLColor& FFLiGetSrgbFetchFavoriteColor(s32 index)
{
    return GetFavoriteColor(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchFaceLine()
{
    return GetFaceLine(GetSrgbFetchColorContainer());
}

const FFLColor& FFLiGetSrgbFetchCapColor(s32 index)
{
    return GetCapColor(GetSrgbFetchColorContainer(), index);
}

const FFLColor& FFLiGetSrgbFetchNoselineColor()
{
    return GetNoselineColor(GetSrgbFetchColorContainer());
}

const FFLColor& FFLiGetFacelineColor(s32 index)
{
    return GetFacelineColor(GetColorContainer(), index);
}

const FFLColor& FFLiGetBeardColor(s32 index)
{
    return GetBeardColor(GetColorContainer(), index);
}

const FFLColor& FFLiGetEyebrowColor(s32 index)
{
    return GetEyebrowColor(GetColorContainer(), index);
}

const FFLColor& FFLiGetMustacheColor(s32 index)
{
    return GetMustacheColor(GetColorContainer(), index);
}

const FFLColor& FFLiGetHairColor(s32 index)
{
    return GetHairColor(GetColorContainer(), index);
}

const FFLColor& FFLiGetEyeColorR(s32 index)
{
    return GetEyeColorR(GetColorContainer(), index);
}

const FFLColor& FFLiGetEyeColorG(s32 index)
{
    return GetEyeColorG(GetColorContainer(), index);
}

const FFLColor& FFLiGetEyeColorB(s32 index)
{
    return GetEyeColorB(GetColorContainer(), index);
}

const FFLColor& FFLiGetGlassColor(s32 index)
{
    return GetGlassColor(GetColorContainer(), index);
}

const FFLColor& FFLiGetMouthColorR(s32 index)
{
    return GetMouthColorR(GetColorContainer(), index);
}

const FFLColor& FFLiGetMouthColorG(s32 index)
{
    return GetMouthColorG(GetColorContainer(), index);
}

const FFLColor& FFLiGetMouthColorB(s32 index)
{
    return GetMouthColorB(GetColorContainer(), index);
}

const FFLColor& FFLiGetMoleColor()
{
    return GetMoleColor(GetColorContainer());
}

const FFLColor& FFLiGetFavoriteColor(s32 index)
{
    return GetFavoriteColor(GetColorContainer(), index);
}

const FFLColor& FFLiGetFaceLine()
{
    return GetFaceLine(GetColorContainer());
}

const FFLColor& FFLiGetCapColor(s32 index)
{
    return GetCapColor(GetColorContainer(), index);
}

const FFLColor& FFLiGetNoselineColor()
{
    return GetNoselineColor(GetColorContainer());
}

namespace {

void SetupColorContainer(FFLiColorContainer& container)
{
    const FFLColor facelineColor[FFLI_FACELINE_COLOR_NUM] = {
        { 1.000f, 0.827f, 0.678f, 1.000f },
        { 1.000f, 0.714f, 0.420f, 1.000f },
        { 0.870f, 0.475f, 0.259f, 1.000f },
        { 1.000f, 0.667f, 0.549f, 1.000f },
        { 0.678f, 0.318f, 0.161f, 1.000f },
        { 0.388f, 0.173f, 0.094f, 1.000f }
    };

    const FFLColor hairColor[FFLI_HAIR_COLOR_NUM] = {
        { 0.118f, 0.102f, 0.094f, 1.000f },
        { 0.251f, 0.125f, 0.063f, 1.000f },
        { 0.361f, 0.094f, 0.039f, 1.000f },
        { 0.486f, 0.227f, 0.078f, 1.000f },
        { 0.471f, 0.471f, 0.502f, 1.000f },
        { 0.306f, 0.243f, 0.063f, 1.000f },
        { 0.533f, 0.345f, 0.094f, 1.000f },
        { 0.816f, 0.627f, 0.290f, 1.000f }
    };

    const FFLColor eyeColorR[FFLI_EYE_COLOR_R_NUM] = {
        { 0.000f, 0.000f, 0.000f, 1.000f },
        { 1.000f, 0.510f, 0.000f, 1.000f },
        { 0.000f, 1.000f, 1.000f, 1.000f }
    };

    const FFLColor eyeColorG =
        { 1.000f, 1.000f, 1.000f, 1.000f };

    const FFLColor eyeColorB[FFLI_EYE_COLOR_B_NUM] = {
        { 0.000f, 0.000f, 0.000f, 1.000f },
        { 0.424f, 0.439f, 0.439f, 1.000f },
        { 0.400f, 0.235f, 0.173f, 1.000f },
        { 0.376f, 0.369f, 0.188f, 1.000f },
        { 0.275f, 0.329f, 0.659f, 1.000f },
        { 0.220f, 0.439f, 0.345f, 1.000f }
    };

    const FFLColor glassColor[FFLI_GLASS_COLOR_NUM] = {
        { 0.094f, 0.094f, 0.094f, 1.000f },
        { 0.376f, 0.219f, 0.062f, 1.000f },
        { 0.658f, 0.062f, 0.031f, 1.000f },
        { 0.125f, 0.188f, 0.407f, 1.000f },
        { 0.658f, 0.376f, 0.000f, 1.000f },
        { 0.470f, 0.439f, 0.407f, 1.000f },
    };

    const FFLColor mouthColorR[FFLI_MOUTH_COLOR_R_NUM] = {
        { 0.847f, 0.322f, 0.031f, 1.000f },
        { 0.941f, 0.047f, 0.031f, 1.000f },
        { 0.961f, 0.282f, 0.282f, 1.000f },
        { 0.941f, 0.604f, 0.455f, 1.000f },
        { 0.549f, 0.314f, 0.251f, 1.000f }
    };

    const FFLColor mouthColorG[FFLI_MOUTH_COLOR_G_NUM] = {
        { 0.510f, 0.188f, 0.094f, 1.000f },
        { 0.471f, 0.047f, 0.047f, 1.000f },
        { 0.533f, 0.125f, 0.157f, 1.000f },
        { 0.863f, 0.471f, 0.314f, 1.000f },
        { 0.275f, 0.118f, 0.039f, 1.000f }
    };

    const FFLColor mouthColorB =
        { 1.000f, 1.000f, 1.000f, 1.000f };

    const FFLColor moleColor =
        { 0.071f, 0.059f, 0.059f, 1.000f };

    const FFLColor favoriteColor[FFLI_FAVORITE_COLOR_NUM] = {
        { 0.824f, 0.118f, 0.078f, 1.000f },
        { 1.000f, 0.431f, 0.098f, 1.000f },
        { 1.000f, 0.847f, 0.125f, 1.000f },
        { 0.471f, 0.824f, 0.125f, 1.000f },
        { 0.000f, 0.471f, 0.188f, 1.000f },
        { 0.039f, 0.282f, 0.706f, 1.000f },
        { 0.235f, 0.667f, 0.871f, 1.000f },
        { 0.961f, 0.353f, 0.490f, 1.000f },
        { 0.451f, 0.157f, 0.678f, 1.000f },
        { 0.282f, 0.220f, 0.094f, 1.000f },
        { 0.878f, 0.878f, 0.878f, 1.000f },
        { 0.094f, 0.094f, 0.078f, 1.000f }
    };

    const FFLColor faceLine =
        { 0.000f, 0.000f, 0.000f, 1.000f };

    std::memcpy(&(container.colors[FFLI_COLOR_TYPE_FACELINE_COLOR_0]),  facelineColor,  sizeof(facelineColor));
    std::memcpy(&(container.colors[FFLI_COLOR_TYPE_HAIR_COLOR_0]),      hairColor,      sizeof(hairColor));
    std::memcpy(&(container.colors[FFLI_COLOR_TYPE_EYE_COLOR_R_0]),     eyeColorR,      sizeof(eyeColorR));
    std::memcpy(&(container.colors[FFLI_COLOR_TYPE_EYE_COLOR_B_0]),     eyeColorB,      sizeof(eyeColorB));
    std::memcpy(&(container.colors[FFLI_COLOR_TYPE_GLASS_COLOR_0]),     glassColor,     sizeof(glassColor));
    std::memcpy(&(container.colors[FFLI_COLOR_TYPE_MOUTH_COLOR_R_0]),   mouthColorR,    sizeof(mouthColorR));
    std::memcpy(&(container.colors[FFLI_COLOR_TYPE_MOUTH_COLOR_G_0]),   mouthColorG,    sizeof(mouthColorG));
    std::memcpy(&(container.colors[FFLI_COLOR_TYPE_FAVORITE_COLOR_0]),  favoriteColor,  sizeof(favoriteColor));

    container.colors[FFLI_COLOR_TYPE_EYE_COLOR_G]   = eyeColorG;
    container.colors[FFLI_COLOR_TYPE_MOUTH_COLOR_B] = mouthColorB;
    container.colors[FFLI_COLOR_TYPE_MOLE_COLOR]    = moleColor;
    container.colors[FFLI_COLOR_TYPE_FACE_LINE]     = faceLine;
}

f32 SrgbFetchValue(f32 value)
{
    const u8 srgbFetchTable[256] = {
          0,   0,   0,   0,
          0,   0,   0,   1,
          1,   1,   1,   1,
          1,   1,   1,   1,
          1,   1,   2,   2,
          2,   2,   2,   2,
          2,   2,   3,   3,
          3,   3,   3,   3,
          4,   4,   4,   4,
          5,   5,   5,   5,
          5,   6,   6,   6,
          6,   7,   7,   7,
          8,   8,   8,   8,
          9,   9,   9,  10,
         10,  10,  11,  11,
         12,  12,  12,  13,
         13,  14,  14,  14,
         15,  15,  16,  16,
         17,  17,  17,  18,
         18,  19,  19,  20,
         20,  21,  22,  22,
         23,  23,  24,  24,
         25,  26,  26,  27,
         27,  28,  29,  29,
         30,  31,  31,  32,
         32,  33,  34,  35,
         35,  36,  37,  38,
         38,  39,  40,  41,
         41,  42,  43,  44,
         45,  45,  46,  47,
         48,  49,  50,  51,
         51,  52,  53,  54,
         55,  56,  57,  58,
         59,  60,  61,  62,
         63,  64,  65,  66,
         67,  68,  69,  70,
         71,  72,  73,  74,
         76,  77,  78,  79,
         80,  81,  82,  84,
         85,  86,  87,  89,
         90,  91,  92,  94,
         95,  96,  97,  99,
        100, 101, 103, 104,
        105, 107, 108, 109,
        111, 112, 114, 115,
        117, 118, 120, 121,
        122, 124, 125, 127,
        128, 129, 131, 132,
        134, 136, 137, 139,
        140, 142, 144, 145,
        147, 149, 150, 152,
        154, 155, 157, 159,
        160, 162, 164, 166,
        168, 169, 171, 173,
        175, 177, 178, 180,
        182, 184, 186, 188,
        190, 192, 194, 196,
        198, 200, 202, 204,
        206, 208, 210, 212,
        214, 216, 218, 220,
        222, 224, 226, 228,
        231, 233, 235, 237,
        239, 242, 244, 246,
        248, 250, 253, 255
    };

    return srgbFetchTable[u8(value * 255.0f)] / 255.0f;
}

void DegammaColorContainer(FFLiColorContainer& container)
{
    for (u32 i = 0; i < FFLI_COLOR_TYPE_MAX; i++)
    {
        FFLColor& color = container.colors[i];
        color.r = SrgbFetchValue(color.r);
        color.g = SrgbFetchValue(color.g);
        color.b = SrgbFetchValue(color.b);
    }
}

void InitializeColorContainerIfUninitialized()
{
    if (!s_IsIntializedColorContainer)
    {
        SetupColorContainer(s_ColorContainer[FFLI_CONTAINER_TYPE_NORMAL]);

        SetupColorContainer(s_ColorContainer[FFLI_CONTAINER_TYPE_SRGB]);
        DegammaColorContainer(s_ColorContainer[FFLI_CONTAINER_TYPE_SRGB]);

        s_IsIntializedColorContainer = true;
    }
}

const FFLiColorContainer& GetSrgbFetchColorContainer()
{
    InitializeColorContainerIfUninitialized();

    return
        s_UseOffScreenSrgbFetch
            ? s_ColorContainer[FFLI_CONTAINER_TYPE_NORMAL]
            : GetColorContainer();
}

const FFLiColorContainer& GetColorContainer()
{
    InitializeColorContainerIfUninitialized();
    return s_ColorContainer[s_ContainerType];
}

// nn::mii::detail::CommonColorTable SRGB fields
// Common colors as used in Switch Miis
/*
struct Color3 {
    float r;
    float g;
    float b;
};
struct CommonColorElement {
    Color3 linear;
    Color3 srgb;
    int    _18;
    int    _1c;
};
struct CommonColorTable {
    CommonColorElement colors[100];
};
*/
/* Referenced by...
 * GetCommonColor
 * GetOrderIndexByCommonColor
 * GetHairColorToVer3
 * GetEyeColorToVer3
 * GetMouthColorToVer3
 * GetGlassColorToVer3
 * GetGlassColorToVer3
 */

// NOTE: FFLiColorContainer contains the color in linear/normal gamma and THEN sRGB
// NOTE NOTE IF YOU ARE READING: YOU WANT THE SECOND COLORS IN THESE ARRAYS

const FFLColor nnmiiCommonColors[100][FFLI_CONTAINER_TYPE_MAX] = {
    { { 0.02624122f, 0.02121902f, 0.02121902f, 1.0f }, { 0.1764706f, 0.1568628f, 0.1568628f, 1.0f } },
    { { 0.05126946f, 0.01444384f, 0.00518152f, 1.0f }, { 0.2509804f, 0.1254902f, 0.0627451f, 1.0f } },
    { { 0.1070232f, 0.00913407f, 0.00303527f, 1.0f }, { 0.3607844f, 0.0941177f, 0.0392157f, 1.0f } },
    { { 0.2015563f, 0.04231142f, 0.00699541f, 1.0f }, { 0.4862746f, 0.227451f, 0.0784314f, 1.0f } },
    { { 0.1878208f, 0.1878208f, 0.2158605f, 1.0f }, { 0.4705883f, 0.4705883f, 0.5019608f, 1.0f } },
    { { 0.07618541f, 0.04817184f, 0.00518152f, 1.0f }, { 0.3058824f, 0.2431373f, 0.0627451f, 1.0f } },
    { { 0.2462014f, 0.09758739f, 0.00913407f, 1.0f }, { 0.5333334f, 0.3450981f, 0.0941177f, 1.0f } },
    { { 0.6307572f, 0.3515327f, 0.06847817f, 1.0f }, { 0.8156863f, 0.627451f, 0.2901961f, 1.0f } },
    { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } },
    { { 0.1499599f, 0.1620294f, 0.1620294f, 1.0f }, { 0.4235295f, 0.4392157f, 0.4392157f, 1.0f } },
    { { 0.1328683f, 0.04518623f, 0.02518688f, 1.0f }, { 0.4f, 0.2352942f, 0.1725491f, 1.0f } },
    { { 0.1169707f, 0.1119325f, 0.02955684f, 1.0f }, { 0.3764706f, 0.3686275f, 0.1882353f, 1.0f } },
    { { 0.0612461f, 0.08865561f, 0.3915726f, 1.0f }, { 0.2745099f, 0.3294118f, 0.6588236f, 1.0f } },
    { { 0.03954626f, 0.1620294f, 0.09758739f, 1.0f }, { 0.2196079f, 0.4392157f, 0.3450981f, 1.0f } },
    { { 0.1169707f, 0.03954626f, 0.00518152f, 1.0f }, { 0.3764706f, 0.2196079f, 0.0627451f, 1.0f } },
    { { 0.3915726f, 0.00518152f, 0.00242822f, 1.0f }, { 0.6588236f, 0.0627451f, 0.0313726f, 1.0f } },
    { { 0.01444384f, 0.02955684f, 0.1384317f, 1.0f }, { 0.1254902f, 0.1882353f, 0.4078432f, 1.0f } },
    { { 0.3915726f, 0.1169707f, 0.0f, 1.0f }, { 0.6588236f, 0.3764706f, 0.0f, 1.0f } },
    { { 0.1878208f, 0.1620294f, 0.1384317f, 1.0f }, { 0.4705883f, 0.4392157f, 0.4078432f, 1.0f } },
    { { 0.6866855f, 0.08437625f, 0.00242822f, 1.0f }, { 0.8470589f, 0.3215687f, 0.0313726f, 1.0f } },
    { { 0.8713672f, 0.00367651f, 0.00242822f, 1.0f }, { 0.9411765f, 0.0470589f, 0.0313726f, 1.0f } },
    { { 0.9130989f, 0.0648033f, 0.0648033f, 1.0f }, { 0.9607844f, 0.282353f, 0.282353f, 1.0f } },
    { { 0.8713672f, 0.3231433f, 0.1746474f, 1.0f }, { 0.9411765f, 0.6039216f, 0.454902f, 1.0f } },
    { { 0.2622508f, 0.08021983f, 0.05126946f, 1.0f }, { 0.5490197f, 0.3137255f, 0.2509804f, 1.0f } },
    { { 0.2307401f, 0.01938238f, 0.01938238f, 1.0f }, { 0.5176471f, 0.1490197f, 0.1490197f, 1.0f } },
    { { 1.0f, 0.1714411f, 0.1328683f, 1.0f }, { 1.0f, 0.4509804f, 0.4f, 1.0f } },
    { { 1.0f, 0.3813261f, 0.3813261f, 1.0f }, { 1.0f, 0.6509804f, 0.6509804f, 1.0f } },
    { { 1.0f, 0.5271152f, 0.4910209f, 1.0f }, { 1.0f, 0.7529412f, 0.7294118f, 1.0f } },
    { { 0.1714411f, 0.0273209f, 0.04373505f, 1.0f }, { 0.4509804f, 0.1803922f, 0.2313726f, 1.0f } },
    { { 0.3185468f, 0.0137021f, 0.04666509f, 1.0f }, { 0.6f, 0.1215687f, 0.2392157f, 1.0f } },
    { { 0.2541521f, 0.00856813f, 0.04817184f, 1.0f }, { 0.5411765f, 0.0901961f, 0.2431373f, 1.0f } },
    { { 0.4620771f, 0.04817184f, 0.05448031f, 1.0f }, { 0.709804f, 0.2431373f, 0.2588236f, 1.0f } },
    { { 0.571125f, 0.01298304f, 0.09305902f, 1.0f }, { 0.7803922f, 0.1176471f, 0.337255f, 1.0f } },
    { { 0.4341537f, 0.08650047f, 0.2195262f, 1.0f }, { 0.6901961f, 0.3254902f, 0.5058824f, 1.0f } },
    { { 0.571125f, 0.08865561f, 0.1559265f, 1.0f }, { 0.7803922f, 0.3294118f, 0.4313726f, 1.0f } },
    { { 0.9559736f, 0.1778885f, 0.309469f, 1.0f }, { 0.9803922f, 0.4588236f, 0.5921569f, 1.0f } },
    { { 0.9734454f, 0.4125428f, 0.5840785f, 1.0f }, { 0.9882353f, 0.6745099f, 0.7882353f, 1.0f } },
    { { 1.0f, 0.5840785f, 0.6866855f, 1.0f }, { 1.0f, 0.7882353f, 0.8470589f, 1.0f } },
    { { 0.03071345f, 0.01161226f, 0.05126946f, 1.0f }, { 0.1921569f, 0.109804f, 0.2509804f, 1.0f } },
    { { 0.03820438f, 0.02121902f, 0.04666509f, 1.0f }, { 0.2156863f, 0.1568628f, 0.2392157f, 1.0f } },
    { { 0.07227188f, 0.00913407f, 0.07421357f, 1.0f }, { 0.2980393f, 0.0941177f, 0.3019608f, 1.0f } },
    { { 0.1589609f, 0.05448031f, 0.4507858f, 1.0f }, { 0.4352942f, 0.2588236f, 0.7019608f, 1.0f } },
    { { 0.2345507f, 0.1070232f, 0.4793203f, 1.0f }, { 0.5215687f, 0.3607844f, 0.7215687f, 1.0f } },
    { { 0.5271152f, 0.2269659f, 0.6038274f, 1.0f }, { 0.7529412f, 0.5137255f, 0.8f, 1.0f } },
    { { 0.3915726f, 0.2917707f, 0.5840785f, 1.0f }, { 0.6588236f, 0.5764706f, 0.7882353f, 1.0f } },
    { { 0.5583405f, 0.4125428f, 0.791298f, 1.0f }, { 0.7725491f, 0.6745099f, 0.9019608f, 1.0f } },
    { { 0.8549928f, 0.5149179f, 0.9559736f, 1.0f }, { 0.9333334f, 0.7450981f, 0.9803922f, 1.0f } },
    { { 0.6444799f, 0.5583405f, 0.8468735f, 1.0f }, { 0.8235295f, 0.7725491f, 0.9294118f, 1.0f } },
    { { 0.00972123f, 0.0137021f, 0.05126946f, 1.0f }, { 0.0980393f, 0.1215687f, 0.2509804f, 1.0f } },
    { { 0.00604884f, 0.0497066f, 0.1328683f, 1.0f }, { 0.0705883f, 0.2470589f, 0.4f, 1.0f } },
    { { 0.02315337f, 0.2232281f, 0.658375f, 1.0f }, { 0.1647059f, 0.509804f, 0.8313726f, 1.0f } },
    { { 0.09530749f, 0.4564111f, 0.8879234f, 1.0f }, { 0.3411765f, 0.7058824f, 0.9490197f, 1.0f } },
    { { 0.1946179f, 0.5583405f, 0.7304609f, 1.0f }, { 0.4784314f, 0.7725491f, 0.8705883f, 1.0f } },
    { { 0.2501584f, 0.3813261f, 0.9559736f, 1.0f }, { 0.537255f, 0.6509804f, 0.9803922f, 1.0f } },
    { { 0.2307401f, 0.5088814f, 0.9559736f, 1.0f }, { 0.5176471f, 0.7411765f, 0.9803922f, 1.0f } },
    { { 0.3564002f, 0.7681512f, 1.0f, 1.0f }, { 0.6313726f, 0.8901961f, 1.0f, 1.0f } },
    { { 0.00334654f, 0.0273209f, 0.03688948f, 1.0f }, { 0.0431373f, 0.1803922f, 0.2117648f, 1.0f } },
    { { 0.00030353f, 0.04666509f, 0.04373505f, 1.0f }, { 0.0039216f, 0.2392157f, 0.2313726f, 1.0f } },
    { { 0.00402472f, 0.07818746f, 0.09989879f, 1.0f }, { 0.0509804f, 0.309804f, 0.3490197f, 1.0f } },
    { { 0.0168074f, 0.1328683f, 0.1247718f, 1.0f }, { 0.137255f, 0.4f, 0.3882353f, 1.0f } },
    { { 0.02955684f, 0.2086369f, 0.2622508f, 1.0f }, { 0.1882353f, 0.4941177f, 0.5490197f, 1.0f } },
    { { 0.07818746f, 0.4232678f, 0.4341537f, 1.0f }, { 0.309804f, 0.682353f, 0.6901961f, 1.0f } },
    { { 0.1946179f, 0.5520115f, 0.3419146f, 1.0f }, { 0.4784314f, 0.7686275f, 0.6196079f, 1.0f } },
    { { 0.2122308f, 0.658375f, 0.5271152f, 1.0f }, { 0.4980393f, 0.8313726f, 0.7529412f, 1.0f } },
    { { 0.2422812f, 0.783538f, 0.4677839f, 1.0f }, { 0.5294118f, 0.8980393f, 0.7137255f, 1.0f } },
    { { 0.00303527f, 0.06847817f, 0.03560133f, 1.0f }, { 0.0392157f, 0.2901961f, 0.2078432f, 1.0f } },
    { { 0.05612849f, 0.1946179f, 0.0f, 1.0f }, { 0.2627451f, 0.4784314f, 0.0f, 1.0f } },
    { { 0.00060706f, 0.1778885f, 0.1221388f, 1.0f }, { 0.0078432f, 0.4588236f, 0.3843138f, 1.0f } },
    { { 0.03688948f, 0.3185468f, 0.1620294f, 1.0f }, { 0.2117648f, 0.6f, 0.4392157f, 1.0f } },
    { { 0.07036012f, 0.4178851f, 0.01032982f, 1.0f }, { 0.2941177f, 0.6784314f, 0.1019608f, 1.0f } },
    { { 0.287441f, 0.5209957f, 0.00303527f, 1.0f }, { 0.5725491f, 0.7490197f, 0.0392157f, 1.0f } },
    { { 0.1247718f, 0.571125f, 0.2462014f, 1.0f }, { 0.3882353f, 0.7803922f, 0.5333334f, 1.0f } },
    { { 0.3419146f, 0.7454043f, 0.05448031f, 1.0f }, { 0.6196079f, 0.8784314f, 0.2588236f, 1.0f } },
    { { 0.3049874f, 0.7304609f, 0.2086369f, 1.0f }, { 0.5882353f, 0.8705883f, 0.4941177f, 1.0f } },
    { { 0.4969332f, 0.8879234f, 0.4019778f, 1.0f }, { 0.7333334f, 0.9490197f, 0.6666667f, 1.0f } },
    { { 0.3185468f, 0.2917707f, 0.02415765f, 1.0f }, { 0.6f, 0.5764706f, 0.1686275f, 1.0f } },
    { { 0.3813261f, 0.3005439f, 0.1247718f, 1.0f }, { 0.6509804f, 0.5843138f, 0.3882353f, 1.0f } },
    { { 0.6038274f, 0.5271152f, 0.04091523f, 1.0f }, { 0.8f, 0.7529412f, 0.2235295f, 1.0f } },
    { { 0.6038274f, 0.48515f, 0.2422812f, 1.0f }, { 0.8f, 0.7254902f, 0.5294118f, 1.0f } },
    { { 0.6938719f, 0.6038274f, 0.2232281f, 1.0f }, { 0.8509804f, 0.8f, 0.509804f, 1.0f } },
    { { 0.6653875f, 0.6938719f, 0.1589609f, 1.0f }, { 0.8352942f, 0.8509804f, 0.4352942f, 1.0f } },
    { { 0.6653875f, 0.791298f, 0.2269659f, 1.0f }, { 0.8352942f, 0.9019608f, 0.5137255f, 1.0f } },
    { { 0.6866855f, 0.9559736f, 0.3371637f, 1.0f }, { 0.8470589f, 0.9803922f, 0.6156863f, 1.0f } },
    { { 0.2050788f, 0.05951127f, 0.0f, 1.0f }, { 0.4901961f, 0.2705883f, 0.0f, 1.0f } },
    { { 0.791298f, 0.4969332f, 0.1946179f, 1.0f }, { 0.9019608f, 0.7333334f, 0.4784314f, 1.0f } },
    { { 0.9911023f, 0.7605247f, 0.06847817f, 1.0f }, { 0.9960785f, 0.8862746f, 0.2901961f, 1.0f } },
    { { 0.9559736f, 0.7304609f, 0.2232281f, 1.0f }, { 0.9803922f, 0.8705883f, 0.509804f, 1.0f } },
    { { 0.9301111f, 0.822786f, 0.3324517f, 1.0f }, { 0.9686275f, 0.9176471f, 0.6117648f, 1.0f } },
    { { 0.9559736f, 0.938686f, 0.3277782f, 1.0f }, { 0.9803922f, 0.9725491f, 0.6078432f, 1.0f } },
    { { 0.3813261f, 0.07421357f, 0.01298304f, 1.0f }, { 0.6509804f, 0.3019608f, 0.1176471f, 1.0f } },
    { { 1.0f, 0.3049874f, 0.00402472f, 1.0f }, { 1.0f, 0.5882353f, 0.0509804f, 1.0f } },
    { { 0.6375971f, 0.3277782f, 0.1412634f, 1.0f }, { 0.8196079f, 0.6078432f, 0.4117648f, 1.0f } },
    { { 1.0f, 0.4452013f, 0.1328683f, 1.0f }, { 1.0f, 0.6980393f, 0.4f, 1.0f } },
    { { 1.0f, 0.5394797f, 0.2622508f, 1.0f }, { 1.0f, 0.7607844f, 0.5490197f, 1.0f } },
    { { 0.783538f, 0.6239606f, 0.4396573f, 1.0f }, { 0.8980393f, 0.8117648f, 0.6941177f, 1.0f } },
    { { 0.05286067f, 0.05286067f, 0.05286067f, 1.0f }, { 0.254902f, 0.254902f, 0.254902f, 1.0f } },
    { { 0.3277782f, 0.3277782f, 0.3277782f, 1.0f }, { 0.6078432f, 0.6078432f, 0.6078432f, 1.0f } },
    { { 0.5149179f, 0.5149179f, 0.5149179f, 1.0f }, { 0.7450981f, 0.7450981f, 0.7450981f, 1.0f } },
    { { 0.7156936f, 0.6795427f, 0.6104956f, 1.0f }, { 0.8627451f, 0.8431373f, 0.8039216f, 1.0f } },
    { { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }
};

// nn::mii::detail::UpperLipColorTable
// ColorElement[100], which is the same
// as a CommonColorElement without last two ints
// Switch Upper Lip (MouthGreen) Colors
// Referenced by GetMouthGreenColor
const FFLColor nnmiiUpperLipColors[100][FFLI_CONTAINER_TYPE_MAX] = {
    { { 0.00856813f, 0.00699541f, 0.00699541f, 1.0f }, { 0.0901961f, 0.0784314f, 0.0784314f, 1.0f } },
    { { 0.01444384f, 0.00518152f, 0.00242822f, 1.0f }, { 0.1254902f, 0.0627451f, 0.0313726f, 1.0f } },
    { { 0.0273209f, 0.00367651f, 0.00151764f, 1.0f }, { 0.1803922f, 0.0470589f, 0.0196079f, 1.0f } },
    { { 0.06847817f, 0.0168074f, 0.00367651f, 1.0f }, { 0.2901961f, 0.137255f, 0.0470589f, 1.0f } },
    { { 0.08865561f, 0.08865561f, 0.1022417f, 1.0f }, { 0.3294118f, 0.3294118f, 0.3529412f, 1.0f } },
    { { 0.02028857f, 0.0137021f, 0.00242822f, 1.0f }, { 0.1529412f, 0.1215687f, 0.0313726f, 1.0f } },
    { { 0.08437625f, 0.03560133f, 0.00439145f, 1.0f }, { 0.3215687f, 0.2078432f, 0.054902f, 1.0f } },
    { { 0.4396573f, 0.2158605f, 0.02121902f, 1.0f }, { 0.6941177f, 0.5019608f, 0.1568628f, 1.0f } },
    { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f, 1.0f } },
    { { 0.07227188f, 0.07618541f, 0.07618541f, 1.0f }, { 0.2980393f, 0.3058824f, 0.3058824f, 1.0f } },
    { { 0.03310477f, 0.01298304f, 0.00802321f, 1.0f }, { 0.2f, 0.1176471f, 0.0862746f, 1.0f } },
    { { 0.04231142f, 0.03954626f, 0.01228649f, 1.0f }, { 0.227451f, 0.2196079f, 0.1137255f, 1.0f } },
    { { 0.02315337f, 0.03189605f, 0.1301365f, 1.0f }, { 0.1647059f, 0.1960785f, 0.3960785f, 1.0f } },
    { { 0.02028857f, 0.07618541f, 0.04817184f, 1.0f }, { 0.1529412f, 0.3058824f, 0.2431373f, 1.0f } },
    { { 0.02955684f, 0.01161226f, 0.00242822f, 1.0f }, { 0.1882353f, 0.109804f, 0.0313726f, 1.0f } },
    { { 0.1301365f, 0.00303527f, 0.00151764f, 1.0f }, { 0.3960785f, 0.0392157f, 0.0196079f, 1.0f } },
    { { 0.00518152f, 0.00913407f, 0.03433982f, 1.0f }, { 0.0627451f, 0.0941177f, 0.2039216f, 1.0f } },
    { { 0.1811642f, 0.05612849f, 0.0f, 1.0f }, { 0.4627451f, 0.2627451f, 0.0f, 1.0f } },
    { { 0.08865561f, 0.07618541f, 0.06662599f, 1.0f }, { 0.3294118f, 0.3058824f, 0.2862746f, 1.0f } },
    { { 0.2232281f, 0.02955684f, 0.00913407f, 1.0f }, { 0.509804f, 0.1882353f, 0.0941177f, 1.0f } },
    { { 0.1878208f, 0.00367651f, 0.00367651f, 1.0f }, { 0.4705883f, 0.0470589f, 0.0470589f, 1.0f } },
    { { 0.2462014f, 0.01444384f, 0.02121902f, 1.0f }, { 0.5333334f, 0.1254902f, 0.1568628f, 1.0f } },
    { { 0.7156936f, 0.1878208f, 0.08021983f, 1.0f }, { 0.8627451f, 0.4705883f, 0.3137255f, 1.0f } },
    { { 0.0612461f, 0.01298304f, 0.00303527f, 1.0f }, { 0.2745099f, 0.1176471f, 0.0392157f, 1.0f } },
    { { 0.07818746f, 0.00856813f, 0.00856813f, 1.0f }, { 0.309804f, 0.0901961f, 0.0901961f, 1.0f } },
    { { 0.3185468f, 0.05951127f, 0.04666509f, 1.0f }, { 0.6f, 0.2705883f, 0.2392157f, 1.0f } },
    { { 0.791298f, 0.2345507f, 0.2345507f, 1.0f }, { 0.9019608f, 0.5215687f, 0.5215687f, 1.0f } },
    { { 0.791298f, 0.3564002f, 0.3277782f, 1.0f }, { 0.9019608f, 0.6313726f, 0.6078432f, 1.0f } },
    { { 0.05951127f, 0.01161226f, 0.0168074f, 1.0f }, { 0.2705883f, 0.109804f, 0.137255f, 1.0f } },
    { { 0.1070232f, 0.0065121f, 0.01850023f, 1.0f }, { 0.3607844f, 0.0745099f, 0.1450981f, 1.0f } },
    { { 0.08650047f, 0.00439145f, 0.01850023f, 1.0f }, { 0.3254902f, 0.054902f, 0.1450981f, 1.0f } },
    { { 0.1529262f, 0.01850023f, 0.02121902f, 1.0f }, { 0.427451f, 0.1450981f, 0.1568628f, 1.0f } },
    { { 0.184475f, 0.00604884f, 0.03433982f, 1.0f }, { 0.4666667f, 0.0705883f, 0.2039216f, 1.0f } },
    { { 0.1441285f, 0.03189605f, 0.07421357f, 1.0f }, { 0.4156863f, 0.1960785f, 0.3019608f, 1.0f } },
    { { 0.184475f, 0.03189605f, 0.05448031f, 1.0f }, { 0.4666667f, 0.1960785f, 0.2588236f, 1.0f } },
    { { 0.4286906f, 0.08437625f, 0.1441285f, 1.0f }, { 0.6862746f, 0.3215687f, 0.4156863f, 1.0f } },
    { { 0.7681512f, 0.2622508f, 0.4125428f, 1.0f }, { 0.8901961f, 0.5490197f, 0.6745099f, 1.0f } },
    { { 0.791298f, 0.4072404f, 0.4969332f, 1.0f }, { 0.9019608f, 0.6705883f, 0.7333334f, 1.0f } },
    { { 0.00972123f, 0.00439145f, 0.01444384f, 1.0f }, { 0.0980393f, 0.054902f, 0.1254902f, 1.0f } },
    { { 0.01161226f, 0.00699541f, 0.0137021f, 1.0f }, { 0.109804f, 0.0784314f, 0.1215687f, 1.0f } },
    { { 0.01938238f, 0.00367651f, 0.02028857f, 1.0f }, { 0.1490197f, 0.0470589f, 0.1529412f, 1.0f } },
    { { 0.05612849f, 0.02121902f, 0.1470273f, 1.0f }, { 0.2627451f, 0.1568628f, 0.4196079f, 1.0f } },
    { { 0.08021983f, 0.03820438f, 0.1559265f, 1.0f }, { 0.3137255f, 0.2156863f, 0.4313726f, 1.0f } },
    { { 0.2383976f, 0.1070232f, 0.2746774f, 1.0f }, { 0.5254902f, 0.3607844f, 0.5607844f, 1.0f } },
    { { 0.1811642f, 0.1356334f, 0.2663557f, 1.0f }, { 0.4627451f, 0.4039216f, 0.5529412f, 1.0f } },
    { { 0.4072404f, 0.2788943f, 0.6239606f, 1.0f }, { 0.6705883f, 0.5647059f, 0.8117648f, 1.0f } },
    { { 0.658375f, 0.3515327f, 0.7529424f, 1.0f }, { 0.8313726f, 0.627451f, 0.882353f, 1.0f } },
    { { 0.4793203f, 0.4019778f, 0.6653875f, 1.0f }, { 0.7215687f, 0.6666667f, 0.8352942f, 1.0f } },
    { { 0.00402472f, 0.00518152f, 0.01444384f, 1.0f }, { 0.0509804f, 0.0627451f, 0.1254902f, 1.0f } },
    { { 0.00273175f, 0.01444384f, 0.03310477f, 1.0f }, { 0.0352942f, 0.1254902f, 0.2f, 1.0f } },
    { { 0.01228649f, 0.1046165f, 0.2961383f, 1.0f }, { 0.1137255f, 0.3568628f, 0.5803922f, 1.0f } },
    { { 0.03189605f, 0.309469f, 0.7011021f, 1.0f }, { 0.1960785f, 0.5921569f, 0.854902f, 1.0f } },
    { { 0.1070232f, 0.4178851f, 0.5775806f, 1.0f }, { 0.3607844f, 0.6784314f, 0.7843138f, 1.0f } },
    { { 0.1356334f, 0.2383976f, 0.7529424f, 1.0f }, { 0.4039216f, 0.5254902f, 0.882353f, 1.0f } },
    { { 0.1221388f, 0.3467042f, 0.7529424f, 1.0f }, { 0.3843138f, 0.6235295f, 0.882353f, 1.0f } },
    { { 0.2158605f, 0.571125f, 0.791298f, 1.0f }, { 0.5019608f, 0.7803922f, 0.9019608f, 1.0f } },
    { { 0.00182117f, 0.00856813f, 0.0109601f, 1.0f }, { 0.0235295f, 0.0901961f, 0.1058824f, 1.0f } },
    { { 0.00030353f, 0.0137021f, 0.01298304f, 1.0f }, { 0.0039216f, 0.1215687f, 0.1176471f, 1.0f } },
    { { 0.00212469f, 0.02121902f, 0.02624122f, 1.0f }, { 0.027451f, 0.1568628f, 0.1764706f, 1.0f } },
    { { 0.00913407f, 0.06301002f, 0.05951127f, 1.0f }, { 0.0941177f, 0.2784314f, 0.2705883f, 1.0f } },
    { { 0.01599631f, 0.09758739f, 0.1221388f, 1.0f }, { 0.1333334f, 0.3450981f, 0.3843138f, 1.0f } },
    { { 0.02955684f, 0.258183f, 0.2663557f, 1.0f }, { 0.1882353f, 0.5450981f, 0.5529412f, 1.0f } },
    { { 0.1169707f, 0.4341537f, 0.2422812f, 1.0f }, { 0.3764706f, 0.6901961f, 0.5294118f, 1.0f } },
    { { 0.1247718f, 0.5209957f, 0.3967553f, 1.0f }, { 0.3882353f, 0.7490197f, 0.6627451f, 1.0f } },
    { { 0.1412634f, 0.6172068f, 0.3277782f, 1.0f }, { 0.4117648f, 0.8078432f, 0.6078432f, 1.0f } },
    { { 0.00151764f, 0.01850023f, 0.0109601f, 1.0f }, { 0.0196079f, 0.1450981f, 0.1058824f, 1.0f } },
    { { 0.02121902f, 0.06662599f, 0.0f, 1.0f }, { 0.1568628f, 0.2862746f, 0.0f, 1.0f } },
    { { 0.00030353f, 0.0612461f, 0.04373505f, 1.0f }, { 0.0039216f, 0.2745099f, 0.2313726f, 1.0f } },
    { { 0.01938238f, 0.1470273f, 0.07618541f, 1.0f }, { 0.1490197f, 0.4196079f, 0.3058824f, 1.0f } },
    { { 0.02518688f, 0.2541521f, 0.0f, 1.0f }, { 0.1725491f, 0.5411765f, 0.0f, 1.0f } },
    { { 0.1559265f, 0.3185468f, 0.0f, 1.0f }, { 0.4313726f, 0.6f, 0.0f, 1.0f } },
    { { 0.06301002f, 0.4507858f, 0.1589609f, 1.0f }, { 0.2784314f, 0.7019608f, 0.4352942f, 1.0f } },
    { { 0.2232281f, 0.5906189f, 0.0137021f, 1.0f }, { 0.509804f, 0.7921569f, 0.1215687f, 1.0f } },
    { { 0.1946179f, 0.5775806f, 0.1169707f, 1.0f }, { 0.4784314f, 0.7843138f, 0.3764706f, 1.0f } },
    { { 0.3419146f, 0.7011021f, 0.2622508f, 1.0f }, { 0.6196079f, 0.854902f, 0.5490197f, 1.0f } },
    { { 0.1470273f, 0.1356334f, 0.01298304f, 1.0f }, { 0.4196079f, 0.4039216f, 0.1176471f, 1.0f } },
    { { 0.1746474f, 0.1384317f, 0.05951127f, 1.0f }, { 0.454902f, 0.4078432f, 0.2705883f, 1.0f } },
    { { 0.3662527f, 0.3139888f, 0.00802321f, 1.0f }, { 0.6392157f, 0.5960785f, 0.0862746f, 1.0f } },
    { { 0.4793203f, 0.3662527f, 0.1529262f, 1.0f }, { 0.7215687f, 0.6392157f, 0.427451f, 1.0f } },
    { { 0.5457245f, 0.4620771f, 0.1301365f, 1.0f }, { 0.7647059f, 0.709804f, 0.3960785f, 1.0f } },
    { { 0.5209957f, 0.5457245f, 0.08228273f, 1.0f }, { 0.7490197f, 0.7647059f, 0.3176471f, 1.0f } },
    { { 0.5088814f, 0.6239606f, 0.1274377f, 1.0f }, { 0.7411765f, 0.8117648f, 0.3921569f, 1.0f } },
    { { 0.5028866f, 0.7529424f, 0.2050788f, 1.0f }, { 0.737255f, 0.882353f, 0.4901961f, 1.0f } },
    { { 0.07036012f, 0.0221739f, 0.0f, 1.0f }, { 0.2941177f, 0.1607844f, 0.0f, 1.0f } },
    { { 0.6239606f, 0.3564002f, 0.1022417f, 1.0f }, { 0.8117648f, 0.6313726f, 0.3529412f, 1.0f } },
    { { 0.783538f, 0.5647116f, 0.01599631f, 1.0f }, { 0.8980393f, 0.7764706f, 0.1333334f, 1.0f } },
    { { 0.7529424f, 0.5457245f, 0.1144354f, 1.0f }, { 0.882353f, 0.7647059f, 0.3725491f, 1.0f } },
    { { 0.7304609f, 0.6307572f, 0.2015563f, 1.0f }, { 0.8705883f, 0.8156863f, 0.4862746f, 1.0f } },
    { { 0.7529424f, 0.7379106f, 0.1946179f, 1.0f }, { 0.882353f, 0.8745099f, 0.4784314f, 1.0f } },
    { { 0.1274377f, 0.0273209f, 0.00604884f, 1.0f }, { 0.3921569f, 0.1803922f, 0.0705883f, 1.0f } },
    { { 0.6038274f, 0.1878208f, 0.00303527f, 1.0f }, { 0.8f, 0.4705883f, 0.0392157f, 1.0f } },
    { { 0.5028866f, 0.2232281f, 0.07227188f, 1.0f }, { 0.737255f, 0.509804f, 0.2980393f, 1.0f } },
    { { 0.791298f, 0.287441f, 0.05126946f, 1.0f }, { 0.9019608f, 0.5725491f, 0.2509804f, 1.0f } },
    { { 0.791298f, 0.3712377f, 0.1412634f, 1.0f }, { 0.9019608f, 0.6431373f, 0.4117648f, 1.0f } },
    { { 0.6172068f, 0.4677839f, 0.3049874f, 1.0f }, { 0.8078432f, 0.7137255f, 0.5882353f, 1.0f } },
    { { 0.01520852f, 0.01520852f, 0.01520852f, 1.0f }, { 0.1294118f, 0.1294118f, 0.1294118f, 1.0f } },
    { { 0.2015563f, 0.2015563f, 0.2015563f, 1.0f }, { 0.4862746f, 0.4862746f, 0.4862746f, 1.0f } },
    { { 0.4072404f, 0.4072404f, 0.4072404f, 1.0f }, { 0.6705883f, 0.6705883f, 0.6705883f, 1.0f } },
    { { 0.5647116f, 0.5332766f, 0.4677839f, 1.0f }, { 0.7764706f, 0.7568628f, 0.7137255f, 1.0f } },
    { { 0.6938719f, 0.6938719f, 0.6938719f, 1.0f }, { 0.8509804f, 0.8509804f, 0.8509804f, 1.0f } }
};

// nn::mii::detail::FacelineColorTable
// FacelineElementColor[10], same as
// CommonColorElement structure
// Switch Faceline Colors
/* Referenced by...
 * GetSkinColor
 * GetOrderIndexByFacelineColor
 * GetFacelineColorToVer3
 */
const FFLColor nnmiiFacelineColors[10][FFLI_CONTAINER_TYPE_MAX] = {
    // NOTE: these are passed to switch shaders with A = 0
    { { 1.0f, 0.6514057f, 0.4178851f, 1.0f }, { 1.0f, 0.827451f, 0.6784314f, 1.0f } },
    { { 1.0f, 0.4677839f, 0.1470273f, 1.0f }, { 1.0f, 0.7137255f, 0.4196079f, 1.0f } },
    { { 0.7304609f, 0.1912018f, 0.05448031f, 1.0f }, { 0.8705883f, 0.4745099f, 0.2588236f, 1.0f } },
    { { 1.0f, 0.4019778f, 0.2622508f, 1.0f }, { 1.0f, 0.6666667f, 0.5490197f, 1.0f } },
    { { 0.4178851f, 0.08228273f, 0.0221739f, 1.0f }, { 0.6784314f, 0.3176471f, 0.1607844f, 1.0f } },
    { { 0.1247718f, 0.02518688f, 0.00913407f, 1.0f }, { 0.3882353f, 0.1725491f, 0.0941177f, 1.0f } },
    { { 1.0f, 0.5149179f, 0.3762622f, 1.0f }, { 1.0f, 0.7450981f, 0.6470589f, 1.0f } },
    { { 1.0f, 0.5583405f, 0.2746774f, 1.0f }, { 1.0f, 0.7725491f, 0.5607844f, 1.0f } },
    { { 0.2622508f, 0.04518623f, 0.0168074f, 1.0f }, { 0.5490197f, 0.2352942f, 0.137255f, 1.0f } },
    { { 0.04518623f, 0.02624122f, 0.0168074f, 1.0f }, { 0.2352942f, 0.1764706f, 0.137255f, 1.0f } }
};

const FFLColor& GetFacelineColor(const FFLiColorContainer& container, s32 index)
{
    // NOTE: ver3 faceline colors ARE COMPATIBLE WITH the switch faceline color table
    // 1 = sRGB always
    return nnmiiFacelineColors[index][1];
}

const FFLColor& GetBeardColor(const FFLiColorContainer& container, s32 index)
{
    if (isCommonColorMarked(index))
        return nnmiiCommonColors[unmarkCommonColor(index)][1];
    return container.colors[FFLI_COLOR_TYPE_HAIR_COLOR_0 + index];
}

const FFLColor& GetEyebrowColor(const FFLiColorContainer& container, s32 index)
{
    if (isCommonColorMarked(index))
        return nnmiiCommonColors[unmarkCommonColor(index)][1];
    return container.colors[FFLI_COLOR_TYPE_HAIR_COLOR_0 + index];
}

const FFLColor& GetMustacheColor(const FFLiColorContainer& container, s32 index)
{
    if (isCommonColorMarked(index))
        return nnmiiCommonColors[unmarkCommonColor(index)][1];
    return container.colors[FFLI_COLOR_TYPE_HAIR_COLOR_0 + index];
}

const FFLColor& GetHairColor(const FFLiColorContainer& container, s32 index)
{
    if (isCommonColorMarked(index))
        return nnmiiCommonColors[unmarkCommonColor(index)][1];
    return container.colors[FFLI_COLOR_TYPE_HAIR_COLOR_0 + index];
}

const FFLColor& GetEyeColorR(const FFLiColorContainer& container, s32 index)
{
    return container.colors[FFLI_COLOR_TYPE_EYE_COLOR_R_0 + 2];
}

const FFLColor& GetEyeColorG(const FFLiColorContainer& container, s32 index)
{
    return container.colors[FFLI_COLOR_TYPE_EYE_COLOR_G];
}

const FFLColor& GetEyeColorB(const FFLiColorContainer& container, s32 index)
{
    if (isCommonColorMarked(index))
        return nnmiiCommonColors[unmarkCommonColor(index)][1];
    return container.colors[FFLI_COLOR_TYPE_EYE_COLOR_B_0 + index];
}

const FFLColor& GetGlassColor(const FFLiColorContainer& container, s32 index)
{
    if (isCommonColorMarked(index))
        return nnmiiCommonColors[unmarkCommonColor(index)][1];
    return container.colors[FFLI_COLOR_TYPE_GLASS_COLOR_0 + index];
}

const FFLColor& GetMouthColorR(const FFLiColorContainer& container, s32 index)
{
    if (isCommonColorMarked(index))
        return nnmiiCommonColors[unmarkCommonColor(index)][1];
    return container.colors[FFLI_COLOR_TYPE_MOUTH_COLOR_R_0 + index];
}

const FFLColor& GetMouthColorG(const FFLiColorContainer& container, s32 index)
{
    // NOTE: supposed to be slightly darker than the lower lip color
    if (isCommonColorMarked(index))
        return nnmiiUpperLipColors[unmarkCommonColor(index)][1];
    return container.colors[FFLI_COLOR_TYPE_MOUTH_COLOR_G_0 + index];
}

const FFLColor& GetMouthColorB(const FFLiColorContainer& container, s32 index)
{
    return container.colors[FFLI_COLOR_TYPE_MOUTH_COLOR_B];
}

const FFLColor& GetMoleColor(const FFLiColorContainer& container)
{
    return container.colors[FFLI_COLOR_TYPE_MOLE_COLOR];
}

const FFLColor& GetFavoriteColor(const FFLiColorContainer& container, s32 index)
{
    return container.colors[FFLI_COLOR_TYPE_FAVORITE_COLOR_0 + index];
}

const FFLColor& GetFaceLine(const FFLiColorContainer& container)
{
    return container.colors[FFLI_COLOR_TYPE_FACE_LINE];
}

const FFLColor& GetCapColor(const FFLiColorContainer& container, s32 index)
{
    return container.colors[FFLI_COLOR_TYPE_FAVORITE_COLOR_0 + index];
}

const FFLColor& GetNoselineColor(const FFLiColorContainer& container)
{
    return container.colors[FFLI_COLOR_TYPE_FACE_LINE];
}

}

// outside of the namespace so that this is exported
bool isCommonColorMarked(s32 index) {
    return (index & COMMON_COLOR_MARK_BIT) != 0;
}
s32 markCommonColor(s32 index) {
    return index | COMMON_COLOR_MARK_BIT;
}
s32 unmarkCommonColor(s32 index) {
    return index & ~COMMON_COLOR_MARK_BIT;
}
