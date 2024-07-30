#ifndef FFLI_RESOURCE_HEADER_H_
#define FFLI_RESOURCE_HEADER_H_

#include <nn/ffl/FFLResourceType.h>
#include <nn/ffl/FFLResult.h>

#include <nn/ffl/FFLiShapePartsType.h>
#include <nn/ffl/FFLiTexturePartsType.h>

#include <nn/ffl/detail/FFLiResourceShape.h>
#include <nn/ffl/detail/FFLiResourceTexture.h>

struct  FFLiResourcePartsInfo;
struct  FFLiResourceShapeHeader;
struct  FFLiResourceTextureHeader;

struct  FFLiResourceHeader;

enum    FFLiResourceWindowBits;

u32 FFLiGetTextureResourceNum(FFLiTexturePartsType partsType);
u32 FFLiGetShapeResourceNum(FFLiShapePartsType partsType);

FFLiResourcePartsInfo* FFLiGetTextureResoucePartsInfos(u32* pNum, FFLiResourceHeader* pHeader, FFLiTexturePartsType partsType);
FFLiResourcePartsInfo* FFLiGetShapeResoucePartsInfos(u32* pNum, FFLiResourceShapeHeader* pHeader, FFLiShapePartsType partsType);

s32 FFLiResourceWindowBitsToZlibWindowBits(FFLiResourceWindowBits windowBits);

// --------------------------------------------------------------------------

#define FFLI_RESOURCE_HEADER_VERSION    (0x00070000)

class FFLiResourceHeader
{
public:
    FFLResult GetResult() const;

    u32 GetUncompressBufferSize() const
    {
        return m_UncompressBufferSize;
    }

    bool IsExpand() const
    {
        return m_IsExpand == 1;
    }

    FFLiResourceTextureHeader* GetTextureHeader()
    {
        return &m_TextureHeader;
    }

    u32 GetTextureMaxSize(FFLiTexturePartsType partsType) const
    {
        return m_TextureHeader.partsMaxSize[partsType];
    }

    bool VersionHasTextureHeaderAndGlassTypeMaxSizes() const {
        // is the first 8 bits AND the last 16 bits set?
        return (m_Version & 0xFF0000FF) != 0;
    }

    u32 GetTextureGlassTypeMax() const {
        if (VersionHasTextureHeaderAndGlassTypeMaxSizes())
            // first 8 bits
            return (m_Version >> 24) & 0xFF;
        else
            return FFL_GLASS_TYPE_MAX;
    }

    s32 GetTextureHeaderOffsetAfterGlassTypeAdjust() const {
        if (VersionHasTextureHeaderAndGlassTypeMaxSizes())
            return 0; // no offset adjustment needed, the struct is currently reflecting the new ver
        // what is the size of the texture header...
        // ... without the new glass types?

        // current parts info glass size
        const int sizeCurPartsInfoWNewGlassTypes = sizeof(m_TextureHeader.partsInfoGlass);
        // vs what it SHOULD be in YOUR file with 9
        const int sizePartsInfoNormGlassTypes = sizeof(FFLiResourcePartsInfo[FFL_GLASS_TYPE_MAX]);
        // ... okay, let's take the current WRONG size, subtract the RIGHT size, and we get the difference.
        const int howMuchMoreSizeNotSupposedToBeTaken = sizeCurPartsInfoWNewGlassTypes - sizePartsInfoNormGlassTypes;

        return howMuchMoreSizeNotSupposedToBeTaken;
    }

    u32 GetTextureHeaderSize() const {
        if (VersionHasTextureHeaderAndGlassTypeMaxSizes())
            // last 16 bits
            return (m_Version & 0xFFFF);
        else
            // subtract how much there shouldn't be from how much there is.
            return sizeof(FFLiResourceTextureHeader)
                - GetTextureHeaderOffsetAfterGlassTypeAdjust();
    }

    FFLiResourceShapeHeader* GetShapeHeader()
    {
        //return &m_ShapeHeader;
        // assumes that the texture header is DIRECTLY before the shape header
        /*
        u32 baseOffset = sizeof(FFLiResourceTextureHeader);

        baseOffset += GetTextureHeaderSize();

        // Return the shape header by adjusting the base pointer with the calculated offset
        return reinterpret_cast<FFLiResourceShapeHeader*>(this + baseOffset+1);


        // assumes that this is DIRECTLY after texture header
        FFLiResourceTextureHeader* baseOffset = &m_TextureHeader;
        // use our method to get the size, so we should be in the right place now
        baseOffset += GetTextureHeaderSize();
        // cast the offset
        return reinterpret_cast<FFLiResourceShapeHeader*>(baseOffset);
        */

        char* baseAddress = reinterpret_cast<char*>(&m_TextureHeader);
        baseAddress += GetTextureHeaderSize();
        return reinterpret_cast<FFLiResourceShapeHeader*>(baseAddress);
    }

    u32 GetShapeMaxSize(FFLiShapePartsType partsType)
    {
        return GetShapeHeader()->partsMaxSize[partsType];
    }


    void SwapEndian();  // Deleted in NSMBU

private:
    u32                         m_Magic;
    u32                         m_Version;                  // I think? Always 0x00070000
    u32                         m_UncompressBufferSize;
    u32                         _c[4 / sizeof(u32)];        // Unused, but non-zero in files
    int                         m_IsExpand;                 // Must be 1 specifically to be true, not just non-zero
    FFLiResourceTextureHeader   m_TextureHeader;
    FFLiResourceShapeHeader     m_ShapeHeader;
    u32                         _49d0[48 / sizeof(u32)];    // Unused AND zero in files
};
//NN_STATIC_ASSERT(sizeof(BOOL) == 4);
NN_STATIC_ASSERT_IS_POD(FFLiResourceHeader);
//NN_STATIC_ASSERT(sizeof(FFLiResourceHeader) == 0x4A00);

struct FFLiResourceMultiHeader
{
    FFLiResourceHeader  header[FFL_RESOURCE_TYPE_MAX];
};
//NN_STATIC_ASSERT(sizeof(FFLiResourceMultiHeader) == 0x9400);

#endif // FFLI_RESOURCE_HEADER_H_
