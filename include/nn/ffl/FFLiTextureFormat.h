#ifndef FFLI_TEXTURE_FORMAT_H_
#define FFLI_TEXTURE_FORMAT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum FFLiTextureFormat
{
    FFLI_TEXTURE_FORMAT_R8 = 0,
    FFLI_TEXTURE_FORMAT_RG8 = 1,
    FFLI_TEXTURE_FORMAT_RGBA8 = 2
}
FFLiTextureFormat;

#ifdef __cplusplus
}
#endif

#endif // FFLI_TEXTURE_FORMAT_H_
