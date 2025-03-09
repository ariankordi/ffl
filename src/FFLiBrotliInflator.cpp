#include <nn/ffl/FFLiBrotliInflator.h>

#include <misc/rio_MemUtil.h>

#ifdef FFL_USE_BROTLI

FFLiBrotliInflator::FFLiBrotliInflator()
    : m_IsStreamEnd(false)
{
    // args: allocFunc, freeFunc, opaque
    m_DecoderState = BrotliDecoderCreateInstance(nullptr,
                                                nullptr,
                                                nullptr);
    // BrotliDecoderSetParameter, BROTLI_DECODER_PARAM_LARGE_WINDOW ?
}

FFLiBrotliInflator::~FFLiBrotliInflator()
{
    BrotliDecoderDestroyInstance(m_DecoderState);
}

s32 FFLiBrotliInflator::Process(
    void** ppDst, u32* pDstSize,
    const void** ppSrc, u32* pSrcSize,
    s32 flush)
{
    uint8_t* dstPtr = static_cast<uint8_t*>(*ppDst);
    size_t dstBytesAvailable = *pDstSize;

    const uint8_t* srcPtr = static_cast<const uint8_t*>(*ppSrc);
    size_t srcBytesAvailable = *pSrcSize;

    // BrotliDecoderResult: BROTLI_DECODER_RESULT_SUCCESS, NEEDS_MORE_INPUT...
    BrotliDecoderResult result = BrotliDecoderDecompressStream(
        m_DecoderState,
        &srcBytesAvailable, &srcPtr,
        &dstBytesAvailable, &dstPtr,
        nullptr // total_out
    );

    if (result == BROTLI_DECODER_RESULT_SUCCESS)
    {
        m_IsStreamEnd = true;
        // Like Z_STREAM_END
    }
    //else if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT
    //        || result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT)
    //{
        // Ignored, treated like "Z_OK"
    //}
    else
    {
        // In zlib code, if "inflate" doesn't return Z_OK or Z_STREAM_END,
        // we typically assert or handle error.
        RIO_ASSERT(false);
    }

    // Update how many bytes remain after decoding.
    *ppDst = dstPtr;
    *pDstSize = static_cast<u32>(dstBytesAvailable);
    *ppSrc = srcPtr;
    *pSrcSize = static_cast<u32>(srcBytesAvailable);

    // For zlib, we return Z_STREAM_END or Z_OK. Let's do the same approach:
    return m_IsStreamEnd ? 1 : 0;
}

#endif // FFL_USE_BROTLI
