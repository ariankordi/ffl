#include <nn/ffl/FFLiResourceHeader.h>
#include <nn/ffl/FFLiResourceUtil.h>

FFLResult FFLiIsVaildResourceHeader(const FFLiResourceHeader* pHeader)
{
    return pHeader->GetResult();
}



enum ResHeaderHint {
    RES_HINT_DEFAULT = 0,
    RES_HINT_AFL     = 1,
    RES_HINT_AFL_2_3 = 2
    // todo remove 2 type shapes and add a new resource header for that
    //RES_HINT_AFL_2_3_NO_2_SHAPES = 3,
};

FFLiResourceHeader* DetermineAndAllocateResourceHeaderType(void* pData, bool* needsEndianSwap)
{
    // Check header, flip endianness and determine which type it is

    FFLiResourceHeaderDefaultData* pHeaderDefault;
    pHeaderDefault = reinterpret_cast<FFLiResourceHeaderDefaultData*>(pData);

    // is it backwards?
    if (pHeaderDefault->m_Magic == 0x41524646) // ARFF (should be FFRA)
    {
        *needsEndianSwap = true;
        // NOTE: Only swaps endian BEFORE texture/shape headers
        pHeaderDefault->SwapEndian();
    } else {
        *needsEndianSwap = false;
    }

    // NOTE: size checking happens HERE

    u32 totalUncompressSizeNoVersion = pHeaderDefault->m_TotalUncompressSize & 0x1FFFFFFF; // only last 29 bits

    // take first 30 bits, use last 3 as ResourceHeaderTypeHint enum
    ResHeaderHint hint = static_cast<ResHeaderHint>(pHeaderDefault->m_TotalUncompressSize >> 29); // first 3 bits

    //RIO_LOG("header m_TotalUncompressSize: 0x%04X\n", totalUncompressSizeNoVersion);
    //RIO_LOG("header resource hint: %i (raw: 0x%04X)\n", hint, pHeaderDefault->m_TotalUncompressSize);

    // ig old versions of FFLResource.py set it to this
    RIO_ASSERT(pHeaderDefault->m_TotalUncompressSize != 0);

    if (totalUncompressSizeNoVersion == 0x239D5E0)
        hint = RES_HINT_AFL;
    switch (hint)
    {
        case RES_HINT_AFL:
            RIO_LOG("sorry but AFLResHigh.dat is not supported at this time, use AFLResHigh_2_3.dat\n");
            RIO_ASSERT(false);
            break;
        case RES_HINT_AFL_2_3:
            return new FFLiResourceHeaderAFL_2_3();
            break;
        default:
            break;
        // fall through to uncompress size logic below
    }
    /* uncompress size values:
     * AFLResHigh_2_3.dat: 0x2502DE0
     * AFLResHigh.dat:     0x239D5E0
     * FFLResHigh.dat:     0x0CBBDE0
     */
    if (totalUncompressSizeNoVersion > 0x0CBBDE0)
        return new FFLiResourceHeaderAFL_2_3();
    else
        return new FFLiResourceHeaderDefault();
}
