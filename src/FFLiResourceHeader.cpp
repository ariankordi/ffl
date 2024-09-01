#include <nn/ffl/FFLiResourceHeader.h>
#include <nn/ffl/FFLiSwapEndian.h>
#include <nn/ffl/FFLiUtil.h>

#include <nn/ffl/detail/FFLiResourceTexture.h>

#define SIZE_OF_MEMBER(cls, member) sizeof( ((cls*)0)->member )

u32 FFLiResourceHeaderDefault::GetTextureResourceNum(FFLiTexturePartsType partsType) const
{
    switch (partsType)
    {
    case FFLI_TEXTURE_PARTS_TYPE_BEARD:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeader, partsInfoBeard) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_CAP:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeader, partsInfoCap) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_EYE:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeader, partsInfoEye) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_EYEBROW:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeader, partsInfoEyebrow) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_FACELINE:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeader, partsInfoFaceline) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_FACE_MAKEUP:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeader, partsInfoFaceMakeup) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_GLASS:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeader, partsInfoGlass) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_MOLE:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeader, partsInfoMole) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_MOUTH:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeader, partsInfoMouth) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_MUSTACHE:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeader, partsInfoMustache) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_NOSELINE:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeader, partsInfoNoseline) / sizeof(FFLiResourcePartsInfo);
    default:
        return 0;
    }
}

u32 FFLiResourceHeaderAFL_2_3::GetTextureResourceNum(FFLiTexturePartsType partsType) const
{
    switch (partsType)
    {
    case FFLI_TEXTURE_PARTS_TYPE_BEARD:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeaderAFL_2_3, partsInfoBeard) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_CAP:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeaderAFL_2_3, partsInfoCap) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_EYE:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeaderAFL_2_3, partsInfoEye) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_EYEBROW:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeaderAFL_2_3, partsInfoEyebrow) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_FACELINE:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeaderAFL_2_3, partsInfoFaceline) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_FACE_MAKEUP:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeaderAFL_2_3, partsInfoFaceMakeup) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_GLASS:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeaderAFL_2_3, partsInfoGlass) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_MOLE:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeaderAFL_2_3, partsInfoMole) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_MOUTH:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeaderAFL_2_3, partsInfoMouth) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_MUSTACHE:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeaderAFL_2_3, partsInfoMustache) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_NOSELINE:
        return SIZE_OF_MEMBER(FFLiResourceTextureHeaderAFL_2_3, partsInfoNoseline) / sizeof(FFLiResourcePartsInfo);
    default:
        return 0;
    }
}

u32 FFLiGetTextureResourceNum(FFLiResourceHeader* pHeader, FFLiTexturePartsType partsType)
{
    return pHeader->GetTextureResourceNum(partsType);
}


u32 FFLiGetShapeResourceNum(FFLiShapePartsType partsType)
{
    switch (partsType)
    {
    case FFLI_SHAPE_PARTS_TYPE_BEARD:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoBeard) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_CAP_1:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoCap1) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_CAP_2:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoCap2) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_FACELINE:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoFaceline) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_GLASS:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoGlass) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_MASK:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoMask) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_NOSELINE:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoNoseline) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_NOSE:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoNose) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_HAIR_1:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoHair1) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_HAIR_2:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoHair2) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_FOREHEAD_1:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoForehead1) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_FOREHEAD_2:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoForehead2) / sizeof(FFLiResourcePartsInfo);
    default:
        return 0;
    }
}


FFLiResourcePartsInfo* FFLiResourceTextureHeader::GetPartsInfos(FFLiTexturePartsType partsType)
{
    switch (partsType)
    {
    case FFLI_TEXTURE_PARTS_TYPE_BEARD:
        return this->partsInfoBeard;
    case FFLI_TEXTURE_PARTS_TYPE_CAP:
        return this->partsInfoCap;
    case FFLI_TEXTURE_PARTS_TYPE_EYE:
        return this->partsInfoEye;
    case FFLI_TEXTURE_PARTS_TYPE_EYEBROW:
        return this->partsInfoEyebrow;
    case FFLI_TEXTURE_PARTS_TYPE_FACELINE:
        return this->partsInfoFaceline;
    case FFLI_TEXTURE_PARTS_TYPE_FACE_MAKEUP:
        return this->partsInfoFaceMakeup;
    case FFLI_TEXTURE_PARTS_TYPE_GLASS:
        return this->partsInfoGlass;
    case FFLI_TEXTURE_PARTS_TYPE_MOLE:
        return this->partsInfoMole;
    case FFLI_TEXTURE_PARTS_TYPE_MOUTH:
        return this->partsInfoMouth;
    case FFLI_TEXTURE_PARTS_TYPE_MUSTACHE:
        return this->partsInfoMustache;
    case FFLI_TEXTURE_PARTS_TYPE_NOSELINE:
        return this->partsInfoNoseline;
    default:
        return NULL;
    }
}


FFLiResourcePartsInfo* FFLiResourceTextureHeaderAFL_2_3::GetPartsInfos(FFLiTexturePartsType partsType)
{
    switch (partsType)
    {
    case FFLI_TEXTURE_PARTS_TYPE_BEARD:
        return this->partsInfoBeard;
    case FFLI_TEXTURE_PARTS_TYPE_CAP:
        return this->partsInfoCap;
    case FFLI_TEXTURE_PARTS_TYPE_EYE:
        return this->partsInfoEye;
    case FFLI_TEXTURE_PARTS_TYPE_EYEBROW:
        return this->partsInfoEyebrow;
    case FFLI_TEXTURE_PARTS_TYPE_FACELINE:
        return this->partsInfoFaceline;
    case FFLI_TEXTURE_PARTS_TYPE_FACE_MAKEUP:
        return this->partsInfoFaceMakeup;
    case FFLI_TEXTURE_PARTS_TYPE_GLASS:
        return this->partsInfoGlass;
    case FFLI_TEXTURE_PARTS_TYPE_MOLE:
        return this->partsInfoMole;
    case FFLI_TEXTURE_PARTS_TYPE_MOUTH:
        return this->partsInfoMouth;
    case FFLI_TEXTURE_PARTS_TYPE_MUSTACHE:
        return this->partsInfoMustache;
    case FFLI_TEXTURE_PARTS_TYPE_NOSELINE:
        return this->partsInfoNoseline;
    default:
        return NULL;
    }
}

FFLiResourcePartsInfo* FFLiGetTextureResoucePartsInfos(u32* pNum, FFLiResourceHeader* pResHeader, FFLiTexturePartsType partsType)
{
    return pResHeader->GetTextureResourcePartsInfos(pNum, partsType);
}

FFLiResourcePartsInfo* FFLiGetShapeResoucePartsInfos(u32* pNum, FFLiResourceShapeHeader* pHeader, FFLiShapePartsType partsType)
{
    *pNum = FFLiGetShapeResourceNum(partsType);
    switch (partsType)
    {
    case FFLI_SHAPE_PARTS_TYPE_BEARD:
        return pHeader->partsInfoBeard;
    case FFLI_SHAPE_PARTS_TYPE_CAP_1:
        return pHeader->partsInfoCap1;
    case FFLI_SHAPE_PARTS_TYPE_CAP_2:
        return pHeader->partsInfoCap2;
    case FFLI_SHAPE_PARTS_TYPE_FACELINE:
        return pHeader->partsInfoFaceline;
    case FFLI_SHAPE_PARTS_TYPE_GLASS:
        return pHeader->partsInfoGlass;
    case FFLI_SHAPE_PARTS_TYPE_MASK:
        return pHeader->partsInfoMask;
    case FFLI_SHAPE_PARTS_TYPE_NOSELINE:
        return pHeader->partsInfoNoseline;
    case FFLI_SHAPE_PARTS_TYPE_NOSE:
        return pHeader->partsInfoNose;
    case FFLI_SHAPE_PARTS_TYPE_HAIR_1:
        return pHeader->partsInfoHair1;
    case FFLI_SHAPE_PARTS_TYPE_HAIR_2:
        return pHeader->partsInfoHair2;
    case FFLI_SHAPE_PARTS_TYPE_FOREHEAD_1:
        return pHeader->partsInfoForehead1;
    case FFLI_SHAPE_PARTS_TYPE_FOREHEAD_2:
        return pHeader->partsInfoForehead2;
    default:
        return NULL;
    }
}

s32 FFLiResourceWindowBitsToZlibWindowBits(FFLiResourceWindowBits windowBits)
{
    switch (windowBits)
    {
    case FFLI_RESOURCE_WINDOW_BITS_ZLIB_8:
        return 8;
    case FFLI_RESOURCE_WINDOW_BITS_ZLIB_9:
        return 9;
    case FFLI_RESOURCE_WINDOW_BITS_ZLIB_10:
        return 10;
    case FFLI_RESOURCE_WINDOW_BITS_ZLIB_11:
        return 11;
    case FFLI_RESOURCE_WINDOW_BITS_ZLIB_12:
        return 12;
    case FFLI_RESOURCE_WINDOW_BITS_ZLIB_13:
        return 13;
    case FFLI_RESOURCE_WINDOW_BITS_ZLIB_14:
        return 14;
    case FFLI_RESOURCE_WINDOW_BITS_ZLIB_15:
        return 15;
    case FFLI_RESOURCE_WINDOW_BITS_GZIP_8:
        return 16 + 8;
    case FFLI_RESOURCE_WINDOW_BITS_GZIP_9:
        return 16 + 9;
    case FFLI_RESOURCE_WINDOW_BITS_GZIP_10:
        return 16 + 10;
    case FFLI_RESOURCE_WINDOW_BITS_GZIP_11:
        return 16 + 11;
    case FFLI_RESOURCE_WINDOW_BITS_GZIP_12:
        return 16 + 12;
    case FFLI_RESOURCE_WINDOW_BITS_GZIP_13:
        return 16 + 13;
    case FFLI_RESOURCE_WINDOW_BITS_GZIP_14:
        return 16 + 14;
    case FFLI_RESOURCE_WINDOW_BITS_GZIP_15:
        return 16 + 15;
    case FFLI_RESOURCE_WINDOW_BITS_ZLIB_OR_GZIP_15:
        return 32 + 15;
    default:
        return 15;
    }
}

FFLResult FFLiResourceHeader::GetResult() const
{
    if (!FFLiCheckAlignPtr(GetHeaderRaw(), 4))
        return FFL_RESULT_ERROR;

    if (GetMagic() != 0x46465241)  // FFRA
        return FFL_RESULT_FILE_INVALID;

    if (GetVersion() != FFLI_RESOURCE_HEADER_VERSION)
        return FFL_RESULT_FILE_INVALID;

    return FFL_RESULT_OK;
}

namespace {

void SwapEndianResourcePartsInfo(FFLiResourcePartsInfo* pPartsInfo, u32 num)
{
    // This function is deleted in NSMBU.
    // Therefore, its implementation is only theoretical.

    for (u32 i = 0; i < num; i++)
    {
        pPartsInfo[i].dataPos = FFLiSwapEndianImpl<u32>(pPartsInfo[i].dataPos);
        pPartsInfo[i].dataSize = FFLiSwapEndianImpl<u32>(pPartsInfo[i].dataSize);
        pPartsInfo[i].compressedSize = FFLiSwapEndianImpl<u32>(pPartsInfo[i].compressedSize);
    }
}

}

// NOTE: both of the below SwapEndian functions
// assume that everything before parts is already swapped

void FFLiResourceHeaderDefault::SwapEndian()
{
    // This function is deleted in NSMBU.
    // Therefore, its implementation is only theoretical.

    /*
    m_Header->m_Magic = FFLiSwapEndianImpl<u32>(m_Header->m_Magic);
    m_Header->m_Version = FFLiSwapEndianImpl<u32>(m_Header->m_Version);
    m_Header->m_UncompressBufferSize = FFLiSwapEndianImpl<u32>(m_Header->m_UncompressBufferSize);

    // Dunno what to do with this
    // _c

    m_Header->m_IsExpand = FFLiSwapEndianImpl<u32>(m_Header->m_IsExpand);
    */
    FFLiSwapEndianArrayImpl<u32>(m_Header->m_TextureHeader.partsMaxSize, FFLI_TEXTURE_PARTS_TYPE_MAX);
    for (u32 i = 0; i < FFLI_TEXTURE_PARTS_TYPE_MAX; i++)
    {
        u32 num;
        FFLiResourcePartsInfo* pPartsInfo = FFLiGetTextureResoucePartsInfos(&num, this, FFLiTexturePartsType(i));
        SwapEndianResourcePartsInfo(pPartsInfo, num);
    }

    FFLiSwapEndianArrayImpl<u32>(GetShapeHeader()->partsMaxSize, FFLI_SHAPE_PARTS_TYPE_MAX);
    for (u32 i = 0; i < FFLI_SHAPE_PARTS_TYPE_MAX; i++)
    {
        u32 num;
        FFLiResourcePartsInfo* pPartsInfo = FFLiGetShapeResoucePartsInfos(&num, GetShapeHeader(), FFLiShapePartsType(i));
        SwapEndianResourcePartsInfo(pPartsInfo, num);
    }

    // Dunno what to do with this
    // _49d0
}

void FFLiResourceHeaderAFL_2_3::SwapEndian()
{
    // This function is deleted in NSMBU.
    // Therefore, its implementation is only theoretical.
    /*
    m_Header->m_Magic = FFLiSwapEndianImpl<u32>(m_Header->m_Magic);
    m_Header->m_Version = FFLiSwapEndianImpl<u32>(m_Header->m_Version);
    m_Header->m_UncompressBufferSize = FFLiSwapEndianImpl<u32>(m_Header->m_UncompressBufferSize);

    // Dunno what to do with this
    // _c

    m_Header->m_IsExpand = FFLiSwapEndianImpl<u32>(m_Header->m_IsExpand);
    */
    FFLiSwapEndianArrayImpl<u32>(m_Header->m_TextureHeader.partsMaxSize, FFLI_TEXTURE_PARTS_TYPE_MAX);
    for (u32 i = 0; i < FFLI_TEXTURE_PARTS_TYPE_MAX; i++)
    {
        u32 num;
        FFLiResourcePartsInfo* pPartsInfo = FFLiGetTextureResoucePartsInfos(&num, this, FFLiTexturePartsType(i));
        SwapEndianResourcePartsInfo(pPartsInfo, num);
    }

    FFLiSwapEndianArrayImpl<u32>(GetShapeHeader()->partsMaxSize, FFLI_SHAPE_PARTS_TYPE_MAX);
    for (u32 i = 0; i < FFLI_SHAPE_PARTS_TYPE_MAX; i++)
    {
        u32 num;
        FFLiResourcePartsInfo* pPartsInfo = FFLiGetShapeResoucePartsInfos(&num, GetShapeHeader(), FFLiShapePartsType(i));
        SwapEndianResourcePartsInfo(pPartsInfo, num);
    }

    // Dunno what to do with this
    // _49d0
}
