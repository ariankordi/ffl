#ifndef FFLI_RENDER_TEXTURE_H_
#define FFLI_RENDER_TEXTURE_H_

#include <nn/ffl/types.h>

#include <nn/ffl/FFLTexture.h>

#if !defined(__cplusplus) || defined(FFL_NO_RENDER_TEXTURE)

typedef struct FFLiRenderTexture
{
    FFLTexture* pTexture2D;
    void*       pRenderBuffer;
    void*       pColorTarget;
    void*       pDepthTarget;
}
FFLiRenderTexture;
#elif !defined(FFL_NO_RENDER_TEXTURE)

#include <gpu/rio_RenderBuffer.h>
#include <gpu/rio_RenderTarget.h>

struct FFLiRenderTexture
{
    FFLTexture*             pTexture2D; // rio::Texture2D

    rio::RenderBuffer*      pRenderBuffer;
    rio::RenderTargetColor* pColorTarget;
    rio::RenderTargetDepth* pDepthTarget;
};


NN_STATIC_ASSERT32(sizeof(FFLiRenderTexture) == 0x10);

struct FFLColor;

class   FFLiShaderCallback;

void FFLiInitRenderTexture(FFLiRenderTexture* pRenderTexture, u32 width, u32 height, rio::TextureFormat format, u32 numMips);
void FFLiInvalidateRenderTexture(FFLiRenderTexture* pRenderTexture);
void FFLiSetupRenderTexture(FFLiRenderTexture* pRenderTexture, const FFLColor* pClearColor, rio::Texture2D* pDepthBuffer, u32 mipLevel, const FFLiShaderCallback* pCallback);
void FFLiFlushRenderTexture(FFLiRenderTexture* pRenderTexture);

#endif // !defined(__cplusplus) || defined(FFL_NO_RENDER_TEXTURE)

#define FFLI_RENDER_TEXTURE_PLACEHOLDER (FFLiRenderTexture*)0x01

void FFLiDeleteRenderTexture(FFLiRenderTexture* pRenderTexture);

#endif // FFLI_RENDER_TEXTURE_H_
