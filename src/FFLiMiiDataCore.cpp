#include <nn/ffl/FFLiMiiDataCore.h>
#include <nn/ffl/FFLiSwapEndian.h>

#include <nn/ffl/detail/FFLiCrc.h>

static const FFLiSwapEndianDesc SWAP_ENDIAN_DESC[7] = {
    { FFLI_SWAP_ENDIAN_TYPE_U32,  1 },
    { FFLI_SWAP_ENDIAN_TYPE_U8,   8 },
    { FFLI_SWAP_ENDIAN_TYPE_U8,  10 },
    { FFLI_SWAP_ENDIAN_TYPE_U8,   2 },
    { FFLI_SWAP_ENDIAN_TYPE_U16, 11 },
    { FFLI_SWAP_ENDIAN_TYPE_U8,   2 },
    { FFLI_SWAP_ENDIAN_TYPE_U16, 12 }
};

static const FFLiSwapEndianDesc SWAP_ENDIAN_DESC_RFL[] = {
    { FFLI_SWAP_ENDIAN_TYPE_U16, 1 },  // m_Flag
    { FFLI_SWAP_ENDIAN_TYPE_U16, 10 }, // m_Name
    { FFLI_SWAP_ENDIAN_TYPE_U8,  1 },  // m_Height
    { FFLI_SWAP_ENDIAN_TYPE_U8,  1 },  // m_Build
    { FFLI_SWAP_ENDIAN_TYPE_U8,  4 },  // m_CreatorID
    { FFLI_SWAP_ENDIAN_TYPE_U8,  4 },  // m_SystemID
    { FFLI_SWAP_ENDIAN_TYPE_U16, 1 },  // m_FaceFlag
    { FFLI_SWAP_ENDIAN_TYPE_U16, 1 },  // m_HairFlag
    { FFLI_SWAP_ENDIAN_TYPE_U16, 2 },  // m_EyebrowFlag
    { FFLI_SWAP_ENDIAN_TYPE_U16, 2 },  // m_EyeFlag
    { FFLI_SWAP_ENDIAN_TYPE_U16, 1 },  // m_NoseFlag
    { FFLI_SWAP_ENDIAN_TYPE_U16, 1 },  // m_MouthFlag
    { FFLI_SWAP_ENDIAN_TYPE_U16, 1 },  // m_GlassFlag
    { FFLI_SWAP_ENDIAN_TYPE_U16, 1 },  // m_BeardFlag
    { FFLI_SWAP_ENDIAN_TYPE_U16, 1 },  // m_MoleFlag
    //{ FFLI_SWAP_ENDIAN_TYPE_U16, 10 } // creator name
};

void FFLiMiiDataCore::SwapEndian()
{
    FFLiSwapEndianGroup(this, SWAP_ENDIAN_DESC, 7);
}

void FFLiMiiDataOfficial::SwapEndian()
{
    FFLiMiiDataCore::SwapEndian();
    FFLiSwapEndianArrayImpl<u16>(m_CreatorName, 10);
}

void FFLiStoreData::SetCRC()
{
    FFLiPutCRC16(this, FFL_STOREDATA_SIZE);
    RIO_ASSERT(FFLiIsValidCRC16(this, FFL_STOREDATA_SIZE));
}

void FFLiStoreData::SwapEndian()
{
    FFLiMiiDataOfficial::SwapEndian();
    _5c = FFLiSwapEndianImpl<u16>(_5c);
    m_Crc = FFLiSwapEndianImpl<u16>(m_Crc);
    SetCRC();
}

void FFLiMiiDataHidden::SwapEndian()
{
    // This function is deleted in NSMBU.
    // Therefore, its implementation is only theoretical.

    FFLiMiiDataCore::SwapEndian();

    // Dunno what to do with this
    // _48
}

void FFLiMiiDataCoreRFL::SwapEndian()
{
    FFLiSwapEndianGroup(this, SWAP_ENDIAN_DESC_RFL, sizeof(SWAP_ENDIAN_DESC_RFL) / sizeof(FFLiSwapEndianDesc));
}

void FFLiMiiDataOfficialRFL::SwapEndian()
{
    FFLiMiiDataCoreRFL::SwapEndian();
    FFLiSwapEndianArrayImpl<u16>(m_CreatorName, 10);
}
