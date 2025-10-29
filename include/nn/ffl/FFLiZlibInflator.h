#ifndef FFLI_ZLIB_INFLATOR_H_
#define FFLI_ZLIB_INFLATOR_H_

#include <nn/ffl/FFLiZlibInterface.h>

#if !defined(FFL_NO_ZLIB) || defined(FFL_USE_EM_INFLATE)

#ifdef FFL_USE_EM_INFLATE
    #include "em_inflate/lib/em_inflate.h"
#elif defined(FFL_USE_MINIZ)
    #include <miniz.h>
#else
    #include <zlib.h>
#endif

#ifndef Z_OK // redefine zlib consts
    #define Z_STREAM_END 1
    #define Z_FINISH 4
    #define Z_OK 0
#endif // Z_OK

class FFLiZlibInflator : public FFLiZlibInterface
{
public:
    FFLiZlibInflator(s32 windowBits);
    virtual ~FFLiZlibInflator();

    virtual s32 Process(void** ppDst, u32* pDstSize, const void** ppSrc, u32* pSrcSize, s32 flush);

private:
    //u32                 _0[4 / sizeof(u32)];    // Deleted
#ifndef FFL_USE_EM_INFLATE
    z_stream            m_Stream;
    bool                m_IsStreamEnd;
#endif
};
NN_STATIC_ASSERT32(sizeof(FFLiZlibInflator) == 0x40);

#endif // FFL_NO_ZLIB

#endif // FFLI_ZLIB_INFLATOR_H_
