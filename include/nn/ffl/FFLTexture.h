#ifndef FFL_TEXTURE_H_
#define FFL_TEXTURE_H_

#include <nn/ffl/types.h>

#if FFL_USE_RIO && !defined(FFL_USE_TEXTURE_CALLBACK)
    #include <gpu/rio_Texture.h>
    typedef rio::Texture2D FFLTexture;
    #define FFL_GET_RIO_NATIVE_TEXTURE_HANDLE(texture2D) (texture2D)->getNativeTextureHandle()
#elif defined(FFL_USE_TEXTURE_CALLBACK)

    typedef void FFLTexture; // this is the texture handle
                             // i.e. the caller defines it

#else
    #include <nn/ffl/FFLRIOInterop.h>
    typedef FFLRIOTexture2D FFLTexture;
#endif

// Marks that a faceline texture is supposed
// to be bound but render textures are not supported.
#define FFL_TEXTURE_PLACEHOLDER (FFLTexture*)0x01

#endif // FFL_TEXTURE_H_
