#include <nn/ffl/FFLiResourceHeader.h>
#include <nn/ffl/FFLiSwapEndian.h>
#include <nn/ffl/FFLiUtil.h>

#include <nn/ffl/detail/FFLiResourceTexture.h>

#define SIZE_OF_MEMBER(cls, member) sizeof( ((cls*)0)->member )

template <typename T>
u32 GetTextureResourceNumImpl(FFLiTexturePartsType partsType)
{
    switch (partsType)
    {
    case FFLI_TEXTURE_PARTS_TYPE_BEARD:
        return SIZE_OF_MEMBER(T, partsInfoBeard) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_CAP:
        return SIZE_OF_MEMBER(T, partsInfoCap) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_EYE:
        return SIZE_OF_MEMBER(T, partsInfoEye) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_EYEBROW:
        return SIZE_OF_MEMBER(T, partsInfoEyebrow) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_FACELINE:
        return SIZE_OF_MEMBER(T, partsInfoFaceline) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_FACE_MAKEUP:
        return SIZE_OF_MEMBER(T, partsInfoFaceMakeup) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_GLASS:
        return SIZE_OF_MEMBER(T, partsInfoGlass) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_MOLE:
        return SIZE_OF_MEMBER(T, partsInfoMole) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_MOUTH:
        return SIZE_OF_MEMBER(T, partsInfoMouth) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_MUSTACHE:
        return SIZE_OF_MEMBER(T, partsInfoMustache) / sizeof(FFLiResourcePartsInfo);
    case FFLI_TEXTURE_PARTS_TYPE_NOSELINE:
        return SIZE_OF_MEMBER(T, partsInfoNoseline) / sizeof(FFLiResourcePartsInfo);
    default:
        return 0;
    }
}


u32 FFLiResourceHeaderDefault::GetTextureResourceNum(FFLiTexturePartsType partsType) const
{
    return GetTextureResourceNumImpl<FFLiResourceTextureHeader>(partsType);
}

u32 FFLiResourceHeaderAFL::GetTextureResourceNum(FFLiTexturePartsType partsType) const
{
    return GetTextureResourceNumImpl<FFLiResourceTextureHeaderAFL>(partsType);
}

u32 FFLiResourceHeaderAFL_2_3::GetTextureResourceNum(FFLiTexturePartsType partsType) const
{
    return GetTextureResourceNumImpl<FFLiResourceTextureHeaderAFL_2_3>(partsType);
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
    case FFLI_SHAPE_PARTS_TYPE_HAT_NORMAL:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoHatNormal) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_HAT_CAP:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoHatCap) / sizeof(FFLiResourcePartsInfo);
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
    case FFLI_SHAPE_PARTS_TYPE_HAIR_NORMAL:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoHairNormal) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_HAIR_CAP:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoHairCap) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_FOREHEAD_NORMAL:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoForeheadNormal) / sizeof(FFLiResourcePartsInfo);
    case FFLI_SHAPE_PARTS_TYPE_FOREHEAD_CAP:
        return SIZE_OF_MEMBER(FFLiResourceShapeHeader, partsInfoForeheadCap) / sizeof(FFLiResourcePartsInfo);
    default:
        return 0;
    }
}

template <typename T>
FFLiResourcePartsInfo* GetPartsInfosImpl(T* pHeader, FFLiTexturePartsType partsType)
{
    switch (partsType)
    {
    case FFLI_TEXTURE_PARTS_TYPE_BEARD:
        return pHeader->partsInfoBeard;
    case FFLI_TEXTURE_PARTS_TYPE_CAP:
        return pHeader->partsInfoCap;
    case FFLI_TEXTURE_PARTS_TYPE_EYE:
        return pHeader->partsInfoEye;
    case FFLI_TEXTURE_PARTS_TYPE_EYEBROW:
        return pHeader->partsInfoEyebrow;
    case FFLI_TEXTURE_PARTS_TYPE_FACELINE:
        return pHeader->partsInfoFaceline;
    case FFLI_TEXTURE_PARTS_TYPE_FACE_MAKEUP:
        return pHeader->partsInfoFaceMakeup;
    case FFLI_TEXTURE_PARTS_TYPE_GLASS:
        return pHeader->partsInfoGlass;
    case FFLI_TEXTURE_PARTS_TYPE_MOLE:
        return pHeader->partsInfoMole;
    case FFLI_TEXTURE_PARTS_TYPE_MOUTH:
        return pHeader->partsInfoMouth;
    case FFLI_TEXTURE_PARTS_TYPE_MUSTACHE:
        return pHeader->partsInfoMustache;
    case FFLI_TEXTURE_PARTS_TYPE_NOSELINE:
        return pHeader->partsInfoNoseline;
    default:
        return nullptr;
    }
}


FFLiResourcePartsInfo* FFLiResourceTextureHeader::GetPartsInfos(FFLiTexturePartsType partsType)
{
    return GetPartsInfosImpl(this, partsType);
}


FFLiResourcePartsInfo* FFLiResourceTextureHeaderAFL_2_3::GetPartsInfos(FFLiTexturePartsType partsType)
{
    return GetPartsInfosImpl(this, partsType);
}

FFLiResourcePartsInfo* FFLiResourceTextureHeaderAFL::GetPartsInfos(FFLiTexturePartsType partsType)
{
    return GetPartsInfosImpl(this, partsType);
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
    case FFLI_SHAPE_PARTS_TYPE_HAT_NORMAL:
        return pHeader->partsInfoHatNormal;
    case FFLI_SHAPE_PARTS_TYPE_HAT_CAP:
        return pHeader->partsInfoHatCap;
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
    case FFLI_SHAPE_PARTS_TYPE_HAIR_NORMAL:
        return pHeader->partsInfoHairNormal;
    case FFLI_SHAPE_PARTS_TYPE_HAIR_CAP:
        return pHeader->partsInfoHairCap;
    case FFLI_SHAPE_PARTS_TYPE_FOREHEAD_NORMAL:
        return pHeader->partsInfoForeheadNormal;
    case FFLI_SHAPE_PARTS_TYPE_FOREHEAD_CAP:
        return pHeader->partsInfoForeheadCap;
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
    bool alignResult = FFLiCheckAlignPtr(GetHeaderRaw(), 4);
    RIO_ASSERT(alignResult);

    if (!alignResult)
        return FFL_RESULT_ERROR;

    if (GetMagic() != 0x46465241)  // FFRA
    {
        RIO_LOG("FFLiResourceHeader::GetResult: Invalid magic. Found: 0x%08X, Expected: 0x46465241\n", GetMagic());
        return FFL_RESULT_FILE_INVALID;
    }

    if (GetVersion() != FFLI_RESOURCE_HEADER_VERSION)
    {
        RIO_LOG("FFLiResourceHeader::GetResult: Invalid version. Found: 0x%08X, Expected: 0x%08X\n", GetVersion(), FFLI_RESOURCE_HEADER_VERSION);
        return FFL_RESULT_FILE_INVALID;
    }

    return FFL_RESULT_OK;
}

namespace {

static const FFLiSwapEndianDesc SWAP_ENDIAN_DESC_RESOURCE_PARTS_INFO[2] = {
    // dataPos, dataSize, compressedSize
    { FFLI_SWAP_ENDIAN_TYPE_U32, 3 },
    { FFLI_SWAP_ENDIAN_TYPE_U8,  4 },
};

void SwapEndianResourcePartsInfo(FFLiResourcePartsInfo* pPartsInfo, u32 num)
{
    for (u32 i = 0; i < num; i++)
    {
        u32 size = FFLiSwapEndianGroup(&pPartsInfo[i], SWAP_ENDIAN_DESC_RESOURCE_PARTS_INFO, sizeof(SWAP_ENDIAN_DESC_RESOURCE_PARTS_INFO) / sizeof(FFLiSwapEndianDesc));
        RIO_ASSERT(size == sizeof(*pPartsInfo));
    }
}

}

// NOTE: both of the below SwapEndian functions
// assume that everything before parts is already swapped

// Originally FFLiResourceHeader::SwapEndian()
template <typename T>
void HeaderSwapEndianImpl(T* pHeader)
{
    /*
    m_Header->m_Magic = FFLiSwapEndianImpl<u32>(m_Header->m_Magic);
    m_Header->m_Version = FFLiSwapEndianImpl<u32>(m_Header->m_Version);
    m_Header->m_UncompressBufferSize = FFLiSwapEndianImpl<u32>(m_Header->m_UncompressBufferSize);
    m_Header->m_TotalUncompressSize = FFLiSwapEndianImpl<u32>(m_Header->m_TotalUncompressSize); // _c field
    m_Header->m_IsExpand = FFLiSwapEndianImpl<u32>(m_Header->m_IsExpand);
    */
    FFLiSwapEndianArrayImpl<u32>(pHeader->m_Header->m_TextureHeader.partsMaxSize, FFLI_TEXTURE_PARTS_TYPE_MAX);
    FFLiSwapEndianArrayImpl<u32>(pHeader->GetShapeHeader()->partsMaxSize, FFLI_SHAPE_PARTS_TYPE_MAX);

    // Below is inlined in FFL, the
    // offsets are hardcoded and it only
    // calls SwapEndianResourcePartsInfo

    // Swap texture PartsInfo
    for (u32 i = 0; i < FFLI_TEXTURE_PARTS_TYPE_MAX; i++)
    {
        u32 num;
        FFLiResourcePartsInfo* pPartsInfo = FFLiGetTextureResoucePartsInfos(&num, pHeader, FFLiTexturePartsType(i));
        SwapEndianResourcePartsInfo(pPartsInfo, num);
    }

    // Swap shape PartsInfo
    for (u32 i = 0; i < FFLI_SHAPE_PARTS_TYPE_MAX; i++)
    {
        u32 num;
        FFLiResourcePartsInfo* pPartsInfo = FFLiGetShapeResoucePartsInfos(&num, pHeader->GetShapeHeader(), FFLiShapePartsType(i));
        SwapEndianResourcePartsInfo(pPartsInfo, num);
    }

    //FFLiSwapEndianArrayImpl<u32>(_49d0, sizeof(_49d0) / sizeof(u32));
}

void FFLiResourceHeaderDefault::SwapEndian()
{
    return HeaderSwapEndianImpl(this);
}

void FFLiResourceHeaderAFL::SwapEndian()
{
    return HeaderSwapEndianImpl(this);
}

void FFLiResourceHeaderAFL_2_3::SwapEndian()
{
    return HeaderSwapEndianImpl(this);
}
