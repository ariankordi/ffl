#include <nn/ffl/FFLAdditionalInfo.h>

#include <nn/ffl/FFLiAdditionalInfo.h>
#include <nn/ffl/FFLiAuthorID.h>
#include <nn/ffl/FFLiColor.h>
#include <nn/ffl/FFLiMisc.h>
#include <nn/ffl/FFLiUtil.h>

#include <nn/ffl/detail/FFLiCharInfo.h>

#include <misc/rio_MemUtil.h>

namespace {

void ReplaceInvalidNameChar(u16* s, s32 len)
{
    for (s32 i = 0; i < len; ++i)
    {
        u16 c = s[i];
        if (c == L'\0')
            return;

        if (c < L' ' || c == 0x7F)
            s[i] = L'?';
    }
}

bool IsAvailableFontRegion(FFLFontRegion a, FFLFontRegion b)
{
    return a == b;
}

void ReplaceNonAsciiNameChar(u16* s, s32 len)
{
    for (s32 i = 0; i < len; ++i)
    {
        u16 c = s[i];
        if (c == L'\0')
            return;

        if (c >= 0x80)
            s[i] = L'?';
    }
}

}

void FFLiGetAdditionalInfo(FFLAdditionalInfo* pAdditionalInfo, const FFLiCharInfo* pCharInfo, bool checkFontRegion, FFLFontRegion fontRegion)
{
    if (pCharInfo->ngWord && !FFLiIsHomeAuthorID(&pCharInfo->authorID))
    {
        FFLiCopyWcharT2U16(pAdditionalInfo->name,    10 + 1, L"???");
        FFLiCopyWcharT2U16(pAdditionalInfo->creator, 10 + 1, L"???");
    }
    else
    {
        rio::MemUtil::copy(pAdditionalInfo->name, pCharInfo->name, sizeof(u16) * (10 + 1));
        pAdditionalInfo->name[10] = L'\0';

        rio::MemUtil::copy(pAdditionalInfo->creator, pCharInfo->creatorName, sizeof(u16) * (10 + 1));
        pAdditionalInfo->creator[10] = L'\0';

        ReplaceInvalidNameChar(pAdditionalInfo->name, 10);
        ReplaceInvalidNameChar(pAdditionalInfo->creator, 10);

        if (checkFontRegion && !IsAvailableFontRegion(FFLFontRegion(pCharInfo->fontRegion), fontRegion))
        {
            ReplaceNonAsciiNameChar(pAdditionalInfo->name, 10);
            ReplaceNonAsciiNameChar(pAdditionalInfo->creator, 10);
        }
    }

    rio::MemUtil::copy(&pAdditionalInfo->createID, &pCharInfo->createID, sizeof(FFLCreateID));

    pAdditionalInfo->gender = pCharInfo->gender;

    s32 birthMonth;
    u8 birthDay;

    if (FFLiVerifyBirthday(pCharInfo->birthMonth, pCharInfo->birthDay))
    {
        birthMonth = pCharInfo->birthMonth;
        birthDay = pCharInfo->birthDay;
    }
    else
    {
        birthMonth = 0;
        birthDay = 0;
    }

    pAdditionalInfo->birthMonth = birthMonth;
    pAdditionalInfo->birthDay = birthDay;

    pAdditionalInfo->favoriteColor = pCharInfo->favoriteColor;

    u8 height = pCharInfo->height;
    u8 build = pCharInfo->build;

    if (height > 127)
        height = 127;

    if (build > 127)
        build = 127;

    pAdditionalInfo->height = height;
    pAdditionalInfo->build = build;

    pAdditionalInfo->ngWord = bool(pCharInfo->ngWord);
    pAdditionalInfo->fontRegion = pCharInfo->fontRegion;
    pAdditionalInfo->hairFlip = pCharInfo->parts.hairDir;
    pAdditionalInfo->facelineType = pCharInfo->parts.faceType;
    pAdditionalInfo->hairType = pCharInfo->parts.hairType;
    pAdditionalInfo->skinColor = FFLiGetFacelineColor(pCharInfo->parts.facelineColor);
}
