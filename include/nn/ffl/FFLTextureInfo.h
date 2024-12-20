#ifndef FFL_TEXTURE_INFO_H_
#define FFL_TEXTURE_INFO_H_

#include <nn/ffl/types.h>
#include <nn/ffl/FFLTextureFormat.h> // so anything importing this
                                     // can also import that

//#define FFL_TEXTURE_FORMAT_IS_GX2_TILED_FLAG

// texture information that is passed over to
// the texture callback for it to make the texture
typedef struct FFLTextureInfo
{
    // roughly same order as GX2Surface
    u16   width;
    u16   height;
    u8    mipCount;
    u8    format; // FFLTextureFormat
    // indicates if texture is GX2_TILE_MODE_DEFAULT
    bool  isGX2Tiled;
    u8    _padding[1]; // alignment

    u32   imageSize;
    void* imagePtr;
    u32   mipSize;
    // Pointer to mipmaps, usually inside imagePtr.
    void* mipPtr;
    // Location of each mipmap level.
    u32   mipLevelOffset[13];
}
FFLTextureInfo;
// TODO: add NN_STATIC_ASSERT32

#endif // FFL_TEXTURE_INFO_H_
