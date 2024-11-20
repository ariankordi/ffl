#include <nn/ffl/FFLiZlibInflator.h>

#include <misc/rio_MemUtil.h>

#ifndef FFL_NO_ZLIB

FFLiZlibInflator::FFLiZlibInflator(s32 windowBits)
    : m_IsStreamEnd(false)
{
#ifdef FFL_USE_MINIZ
    // NOTE: when you use miniz the window bits HAS to be 15
    // ... meaning it will not work with any public FFL resources :(
    RIO_ASSERT(windowBits == Z_DEFAULT_WINDOW_BITS || windowBits == -Z_DEFAULT_WINDOW_BITS);
#endif
    rio::MemUtil::set(&m_Stream, 0, sizeof(z_stream));
    [[maybe_unused]] s32 ret = inflateInit2(&m_Stream, windowBits);
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

#endif // FFL_NO_ZLIB
