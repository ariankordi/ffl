#ifndef FFLI_BROTLI_INFLATOR_H_
#define FFLI_BROTLI_INFLATOR_H_

#include <nn/ffl/FFLiZlibInterface.h>

#ifdef FFL_USE_BROTLI

#include <brotli/decode.h>

class FFLiBrotliInflator : public FFLiZlibInterface
{
public:
    FFLiBrotliInflator();
    virtual ~FFLiBrotliInflator();

    virtual s32 Process(void** ppDst, u32* pDstSize, const void** ppSrc, u32* pSrcSize, s32 flush);

private:
    BrotliDecoderState* m_DecoderState;
    bool                m_IsStreamEnd;
};
//NN_STATIC_ASSERT32(sizeof(FFLiZlibInflator) == 0x44);

#endif // FFL_USE_BROTLI

#endif // FFLI_BROTLI_INFLATOR_H_
