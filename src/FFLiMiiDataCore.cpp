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

static const FFLiSwapEndianDesc SWAP_ENDIAN_DESC_HIDDEN[2] = {
    { FFLI_SWAP_ENDIAN_TYPE_U32, 1 },
    { FFLI_SWAP_ENDIAN_TYPE_U8,  8 },
};

static const FFLiSwapEndianDesc SWAP_ENDIAN_DESC_RFL[3] = {
    { FFLI_SWAP_ENDIAN_TYPE_U16, 11 },
    { FFLI_SWAP_ENDIAN_TYPE_U8,  10 },
    { FFLI_SWAP_ENDIAN_TYPE_U16, 11 },
};

void FFLiMiiDataCore::SwapEndian()
{
    [[maybe_unused]] u32 size = FFLiSwapEndianGroup(this, SWAP_ENDIAN_DESC, sizeof(SWAP_ENDIAN_DESC) / sizeof(FFLiSwapEndianDesc));
    RIO_ASSERT(size == sizeof(FFLiMiiDataCore));
}

void FFLiMiiDataOfficial::SwapEndian()
{
    FFLiMiiDataCore::SwapEndian();
    FFLiSwapEndianArrayImpl<u16>(m_CreatorName, sizeof(m_CreatorName) / sizeof(u16));
}

void FFLiStoreData::SetCRC()
{
    FFLiPutCRC16(this, FFL_STOREDATA_SIZE);
    RIO_ASSERT(FFLiIsValidCRC16(this, FFL_STOREDATA_SIZE));
}

// Duplicated to FFLiStoreDataCFL
bool FFLiStoreData::IsValidCRC()
{
    return FFLiIsValidCRC16(this, sizeof(FFLiStoreData));
}

void FFLiStoreData::SwapEndian()
{
    // thunk/copy of FFLiStoreDataCFL::SwapEndian
    //RIO_ASSERT(IsValidCRC());
    FFLiMiiDataOfficial::SwapEndian();
    _5c = FFLiSwapEndianImpl<u16>(_5c);
    m_Crc = FFLiSwapEndianImpl<u16>(m_Crc);
    SetCRC();
}

void FFLiMiiDataHidden::SwapEndian()
{
    FFLiMiiDataCore::SwapEndian();

    // Original code:
    [[maybe_unused]] u32 size = FFLiSwapEndianGroup(&m_DateTime, SWAP_ENDIAN_DESC_HIDDEN, sizeof(SWAP_ENDIAN_DESC_HIDDEN) / sizeof(FFLiSwapEndianDesc));
    RIO_ASSERT(size == 0xc); // sizeof(m_DateTime) + sizeof(m_ApplicationID)
    // Suggests it is one field??

    //m_DateTime = FFLiSwapEndianImpl<u32>(m_DateTime);
    //FFLiSwapEndianArrayImpl<u8>(reinterpret_cast<u8*>(&m_ApplicationID), sizeof(m_ApplicationID) / sizeof(u8));
}

// Not originally in FFL:
void FFLiMiiDataCoreRFL::SwapEndian()
{
    FFLiSwapEndianGroup(this, SWAP_ENDIAN_DESC_RFL, sizeof(SWAP_ENDIAN_DESC_RFL) / sizeof(FFLiSwapEndianDesc));
}

void FFLiMiiDataOfficialRFL::SwapEndian()
{
    FFLiMiiDataCoreRFL::SwapEndian();
    FFLiSwapEndianArrayImpl<u16>(m_CreatorName, 10);
}
