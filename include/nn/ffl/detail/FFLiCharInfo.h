#ifndef FFLI_CHARINFO_H_
#define FFLI_CHARINFO_H_

#include <nn/ffl/FFLBirthPlatform.h>
#include <nn/ffl/FFLCreateID.h>
#include <nn/ffl/FFLFontRegion.h>
#include <nn/ffl/FFLGender.h>
#include <nn/ffl/FFLFavoriteColor.h>

#include <nn/ffl/FFLiAuthorID.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: Replace existing FFLiCharInfo with this
more accurate struct from DWARF information for CFL.
Also closer to the structure of RFLiCharInfo: https://github.com/SMGCommunity/Petari/blob/d34c595ba7dfcd92ef776964ecf668f37cbb7123/libs/RVLFaceLib/include/RFLi_Types.h#L140

// Seems to be the same as CFLiCharInfo:
typedef struct FFLiCharInfo
{
    s32          miiVersion;
    struct {
        s32      type;
        s32      color;
        s32      texture;
        s32      make;
    } faceline;
    struct {
        s32      type;
        s32      color;
        s32      flip;
    } hair;
    struct {
        s32      type;
        s32      color;
        s32      scale;
        s32      aspect;
        s32      rotate;
        s32      x;
        s32      y;
    } eye;
    struct {
        s32      type;
        s32      color;
        s32      scale;
        s32      aspect;
        s32      rotate;
        s32      x;
        s32      y;
    } eyebrow;
    struct {
        s32      type;
        s32      scale;
        s32      y;
    } nose;
    struct {
        s32      type;
        s32      color;
        s32      scale;
        s32      aspect;
        s32      y;
    } mouth;
    struct {
        s32      mustache;
        s32      type;
        s32      color;
        s32      scale;
        s32      y;
    } beard;
    struct {
        s32      type;
        s32      color;
        s32      scale;
        s32      y;
    } glass;
    struct {
        s32      type;
        s32      scale;
        s32      x;
        s32      y;
    } mole;
    struct {
        s32      height;
        s32      build;
    } body;
    struct {
        u16      name[10 + 1];
        u16      creator[10 + 1];
        s32      gender;
        s32      birthMonth;
        s32      birthDay;
        s32      favoriteColor;
        u8       favorite;  // GLboolean/bool
        u8       copyable;  // GLboolean/bool
        u8       ngWord;    // GLboolean/bool
        u8       localonly; // GLboolean/bool
        s32      regionMove;
        s32      fontRegion;
        s32      roomIndex;
        s32      positionInRoom;
        s32      birthPlatform;
    } personal;
    FFLCreateID  createID;
    u16          padding_0;
    s32          authorType; // Copied to FFLiMiiDataCore, unused?
    FFLiAuthorID authorID;
}
FFLiCharInfo;
*/

typedef struct FFLiCharInfo
{
    s32                 miiVersion;
    struct
    {
        s32             faceType;
        s32             facelineColor;     // faceColor
        s32             faceLine;          // faceTex
        s32             faceMakeup;        // faceMake
        s32             hairType;
        s32             hairColor;
        s32             hairDir;           // hairFlip
        s32             eyeType;
        s32             eyeColor;
        s32             eyeScale;
        s32             eyeScaleY;         // eyeAspect
        s32             eyeRotate;
        s32             eyeSpacingX;       // eyeX
        s32             eyePositionY;      // eyeY
        s32             eyebrowType;
        s32             eyebrowColor;
        s32             eyebrowScale;
        s32             eyebrowScaleY;     // eyebrowAspect
        s32             eyebrowRotate;
        s32             eyebrowSpacingX;   // eyebrowX
        s32             eyebrowPositionY;  // eyebrowY
        s32             noseType;
        s32             noseScale;
        s32             nosePositionY;     // noseY
        s32             mouthType;
        s32             mouthColor;
        s32             mouthScale;
        s32             mouthScaleY;       // mouthAspect
        s32             mouthPositionY;    // mouthY
        s32             mustacheType;
        s32             beardType;
        s32             beardColor;
        s32             mustacheScale;     // beardScale
        s32             mustachePositionY; // beardY
        s32             glassType;
        s32             glassColor;
        s32             glassScale;
        s32             glassPositionY;    // glassY
        s32             moleType;
        s32             moleScale;
        s32             molePositionX;     // moleX
        s32             molePositionY;     // moleY
    } parts;
    s32                 height;
    s32                 build;
    u16                 name[10 + 1];
    u16                 creatorName[10 + 1];
    s32                 gender;
    s32                 birthMonth;
    s32                 birthDay;
    s32                 favoriteColor; // NOTE: u32
    u8                  favoriteMii;
    u8                  copyable;
    u8                  ngWord;
    u8                  localOnly;
    s32                 regionMove;
    s32                 fontRegion;
    s32                 pageIndex;
    s32                 slotIndex;
    s32                 birthPlatform;
    FFLCreateID         createID;
    u16                 padding_0;
    s32                 authorType;   // previously _0_24_27
    FFLiAuthorID        authorID;
}
FFLiCharInfo;
NN_STATIC_ASSERT(sizeof(FFLiCharInfo) == 0x120);

typedef enum FFLiVerifyReason
{
    FFLI_VERIFY_REASON_OK                         =  0,
    FFLI_VERIFY_REASON_FACELINE_TYPE_INVALID      =  1,
    FFLI_VERIFY_REASON_FACELINE_COLOR_INVALID     =  2,
    FFLI_VERIFY_REASON_FACELINE_TEXTURE_INVALID   =  3,
    FFLI_VERIFY_REASON_FACELINE_MAKE_INVALID      =  4,
    FFLI_VERIFY_REASON_HAIR_TYPE_INVALID          =  5,
    FFLI_VERIFY_REASON_HAIR_COLOR_INVALID         =  6,
    FFLI_VERIFY_REASON_HAIR_FLIP_INVALID          =  7,
    FFLI_VERIFY_REASON_EYE_TYPE_INVALID           =  8,
    FFLI_VERIFY_REASON_EYE_COLOR_INVALID          =  9,
    FFLI_VERIFY_REASON_EYE_SCALE_INVALID          = 10,
    FFLI_VERIFY_REASON_EYE_ASPECT_INVALID         = 11,
    FFLI_VERIFY_REASON_EYE_ROTATE_INVALID         = 12,
    FFLI_VERIFY_REASON_EYE_X_INVALID              = 13,
    FFLI_VERIFY_REASON_EYE_Y_INVALID              = 14,
    FFLI_VERIFY_REASON_EYEBROW_TYPE_INVALID       = 15,
    FFLI_VERIFY_REASON_EYEBROW_COLOR_INVALID      = 16,
    FFLI_VERIFY_REASON_EYEBROW_SCALE_INVALID      = 17,
    FFLI_VERIFY_REASON_EYEBROW_ASPECT_INVALID     = 18,
    FFLI_VERIFY_REASON_EYEBROW_ROTATE_INVALID     = 19,
    FFLI_VERIFY_REASON_EYEBROW_X_INVALID          = 20,
    FFLI_VERIFY_REASON_EYEBROW_Y_INVALID          = 21,
    FFLI_VERIFY_REASON_NOSE_TYPE_INVALID          = 22,
    FFLI_VERIFY_REASON_NOSE_SCALE_INVALID         = 23,
    FFLI_VERIFY_REASON_NOSE_Y_INVALID             = 24,
    FFLI_VERIFY_REASON_MOUTH_TYPE_INVALID         = 25,
    FFLI_VERIFY_REASON_MOUTH_COLOR_INVALID        = 26,
    FFLI_VERIFY_REASON_MOUTH_SCALE_INVALID        = 27,
    FFLI_VERIFY_REASON_MOUTH_ASPECT_INVALID       = 28,
    FFLI_VERIFY_REASON_MOUTH_Y_INVALID            = 29,
    FFLI_VERIFY_REASON_BEARD_TYPE_INVALID         = 30,
    FFLI_VERIFY_REASON_BEARD_COLOR_INVALID        = 31,
    FFLI_VERIFY_REASON_MUSTACHE_TYPE_INVALID      = 32,
    FFLI_VERIFY_REASON_MUSTACHE_SCALE_INVALID     = 33,
    FFLI_VERIFY_REASON_MUSTACHE_Y_INVALID         = 34,
    FFLI_VERIFY_REASON_GLASS_TYPE_INVALID         = 35,
    FFLI_VERIFY_REASON_GLASS_COLOR_INVALID        = 36,
    FFLI_VERIFY_REASON_GLASS_SCALE_INVALID        = 37,
    FFLI_VERIFY_REASON_GLASS_Y_INVALID            = 38,
    FFLI_VERIFY_REASON_MOLE_TYPE_INVALID          = 39,
    FFLI_VERIFY_REASON_MOLE_SCALE_INVALID         = 40,
    FFLI_VERIFY_REASON_MOLE_X_INVALID             = 41,
    FFLI_VERIFY_REASON_MOLE_Y_INVALID             = 42,
    FFLI_VERIFY_REASON_HEIGHT_INVALID             = 43,
    FFLI_VERIFY_REASON_BUILD_INVALID              = 44,
    FFLI_VERIFY_REASON_NAME_INVALID               = 45,
    FFLI_VERIFY_REASON_CREATORNAME_INVALID        = 46,
    FFLI_VERIFY_REASON_GENDER_INVALID             = 47,
    FFLI_VERIFY_REASON_BIRTHDAY_INVALID           = 48,
    FFLI_VERIFY_REASON_FAVORITECOLOR_INVALID      = 49,
    FFLI_VERIFY_REASON_REGIONMOVE_INVALID         = 50,
    FFLI_VERIFY_REASON_FONTREGION_INVALID         = 51,
    FFLI_VERIFY_REASON_ROOM_INDEX_INVALID         = 52,
    FFLI_VERIFY_REASON_POSITION_IN_ROOM_INVALID   = 53,
    FFLI_VERIFY_REASON_BIRTH_PLATFORM_INVALID     = 54,
    FFLI_VERIFY_REASON_CREATEID_INVALID           = 55
}
FFLiVerifyReason;

bool FFLiVerifyCharInfo(const FFLiCharInfo* pCharInfo, bool verifyName);

// Enum name found from string in FFLUtility: "Cannot pick up FFLAdditionalInfo: FFLResult[%u] FFLiVerifyReason[%u]"
FFLiVerifyReason FFLiVerifyCharInfoWithReason(const FFLiCharInfo* pCharInfo, bool verifyName);

#ifdef __cplusplus

}

// --------------------------------------------------------------------------

bool FFLiiVerifyCharInfo(const FFLiCharInfo* pCharInfo, bool verifyName);

s32 FFLiiGetEyeRotateOffset(s32 type);
s32 FFLiiGetEyebrowRotateOffset(s32 type);
f32 FFLiiGetAdjustedMouthH(f32 height, s32 type);
f32 FFLiiGetAdjustedEyeH(f32 height, s32 type);

// --------------------------------------------------------------------------

bool FFLiIsValidCharacterForName(u16 c);

// --------------------------------------------------------------------------

struct FFLAdditionalInfo;

enum FFLiCompareCharInfoFlag
{
    FFLI_COMPARE_CHAR_INFO_FLAG_PARTS           = 1 <<  0,
    FFLI_COMPARE_CHAR_INFO_FLAG_NAME            = 1 <<  1,
    FFLI_COMPARE_CHAR_INFO_FLAG_CREATOR_NAME    = 1 <<  2,
    FFLI_COMPARE_CHAR_INFO_FLAG_CREATE_ID       = 1 <<  3,
    FFLI_COMPARE_CHAR_INFO_FLAG_GENDER          = 1 <<  4,
    FFLI_COMPARE_CHAR_INFO_FLAG_BIRTH_MONTH     = 1 <<  5,
    FFLI_COMPARE_CHAR_INFO_FLAG_BIRTH_DAY       = 1 <<  6,
    FFLI_COMPARE_CHAR_INFO_FLAG_FAVORITE_COLOR  = 1 <<  7,
    FFLI_COMPARE_CHAR_INFO_FLAG_HEIGHT          = 1 <<  8,
    FFLI_COMPARE_CHAR_INFO_FLAG_BUILD           = 1 <<  9,
    FFLI_COMPARE_CHAR_INFO_FLAG_FACELINE_COLOR  = 1 << 10,
    FFLI_COMPARE_CHAR_INFO_FLAG_MII_VERSION     = 1 << 11,
    FFLI_COMPARE_CHAR_INFO_FLAG_COPYABLE        = 1 << 12,
    FFLI_COMPARE_CHAR_INFO_FLAG_NG_WORD         = 1 << 13,
    FFLI_COMPARE_CHAR_INFO_FLAG_LOCAL_ONLY      = 1 << 14,
    FFLI_COMPARE_CHAR_INFO_FLAG_REGION_MOVE     = 1 << 15,
    FFLI_COMPARE_CHAR_INFO_FLAG_FONT_REGION     = 1 << 16,
    FFLI_COMPARE_CHAR_INFO_FLAG_BIRTH_PLATFORM  = 1 << 17,
};

bool FFLiCompareCharInfoWithAdditionalInfo(s32* pFlagOut, s32 flagIn, const FFLiCharInfo* pCharInfoA, const FFLiCharInfo* pCharInfoB, const FFLAdditionalInfo* pAdditionalInfoA, const FFLAdditionalInfo* pAdditionalInfoB);

#endif

#endif // FFLI_CHARINFO_H_
