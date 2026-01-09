#include <nn/ffl/FFLiZlibInflator.h>

#include <misc/rio_MemUtil.h>

#if !defined(FFL_NO_ZLIB) || defined(FFL_USE_MINIZ) || defined(FFL_USE_EM_INFLATE)

#ifdef FFL_USE_EM_INFLATE
    // Include the .c file so there is no extra build step.
    #include "em_inflate.c"
#endif

#ifndef FFL_USE_EM_INFLATE // zlib implementation

FFLiZlibInflator::FFLiZlibInflator(s32 windowBits)
    : m_IsStreamEnd(false)
{
    rio::MemUtil::set(&m_Stream, 0, sizeof(z_stream));
    [[maybe_unused]] s32 ret =
#ifdef FFL_NO_ZLIB
        inflateInit(&m_Stream); // miniz does not like our windowBits
#else
        inflateInit2(&m_Stream, windowBits);
#endif
    RIO_ASSERT(ret == Z_OK);
}

FFLiZlibInflator::~FFLiZlibInflator()
{
    [[maybe_unused]] s32 ret = inflateEnd(&m_Stream);
    RIO_ASSERT(ret == Z_OK);
}

s32 FFLiZlibInflator::Process(void** ppDst, u32* pDstSize, const void** ppSrc, u32* pSrcSize, s32 flush)
{
    m_Stream.next_out = (Bytef*)*ppDst;
    m_Stream.avail_out = *pDstSize;
    m_Stream.next_in = (Bytef*)*ppSrc;
    m_Stream.avail_in = *pSrcSize;

    s32 ret = inflate(&m_Stream, flush);
    if (ret == Z_STREAM_END)
        m_IsStreamEnd = true;
    else
        RIO_ASSERT(ret == Z_OK);

    *ppDst = m_Stream.next_out;
    *pDstSize = m_Stream.avail_out;
    *ppSrc = m_Stream.next_in;
    *pSrcSize = m_Stream.avail_in;

    return ret;
}

#else // FFL_USE_EM_INFLATE
FFLiZlibInflator::FFLiZlibInflator(s32) { }
FFLiZlibInflator::~FFLiZlibInflator() { }
s32 FFLiZlibInflator::Process(void** ppDst, u32* pDstSize, const void** ppSrc, u32* pSrcSize, s32 flush)
{
    em_inflate(*ppSrc, *pSrcSize, (u8*)*ppDst, *pDstSize);
    return Z_STREAM_END;
}
#endif // FFL_USE_EM_INFLATE

#endif // FFL_NO_ZLIB
