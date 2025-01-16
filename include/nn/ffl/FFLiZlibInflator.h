#ifndef FFLI_ZLIB_INFLATOR_H_
#define FFLI_ZLIB_INFLATOR_H_

#include <nn/ffl/FFLiZlibInterface.h>

#ifndef FFL_NO_ZLIB

#ifdef FFL_USE_MINIZ
    #include <miniz.h>
    #ifndef Z_OK
        #define Z_OK MZ_OK
    #endif // Z_OK
#else
    #include <zlib.h>
#endif // FFL_USE_MINIZ

class FFLiZlibInflator : public FFLiZlibInterface
{
public:
    FFLiZlibInflator(s32 windowBits);
    virtual ~FFLiZlibInflator();

    virtual s32 Process(void** ppDst, u32* pDstSize, const void** ppSrc, u32* pSrcSize, s32 flush);

private:
    //u32                 _0[4 / sizeof(u32)];    // Deleted
    z_stream            m_Stream;
    bool                m_IsStreamEnd;
};
NN_STATIC_ASSERT32(sizeof(FFLiZlibInflator) == 0x40);

#endif // FFL_NO_ZLIB

#endif // FFLI_ZLIB_INFLATOR_H_
