#ifndef FFL_TEXTURE_CALLBACK_H_
#define FFL_TEXTURE_CALLBACK_H_

#include <nn/ffl/FFLTexture.h>
#include <nn/ffl/FFLTextureInfo.h>

#ifdef __cplusplus
extern "C" {
#endif

// class to define your own functions to create/delete textures
typedef struct FFLTextureCallback
{
    void* pObj;

    bool  useOriginalTileMode; // will not convert to linear
    u8    _padding[3]; // alignment

    void (*pCreateFunc)(void* pObj, const FFLTextureInfo* pTextureInfo, FFLTexture* pTexture);
    void (*pDeleteFunc)(void* pObj, FFLTexture* pTexture);
}
FFLTextureCallback;
// TODO: add NN_STATIC_ASSERT32

void FFLSetTextureCallback(const FFLTextureCallback* pCallback);

#ifdef __cplusplus
}
#endif

#endif // FFL_TEXTURE_CALLBACK_H_
