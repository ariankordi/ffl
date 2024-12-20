#ifndef FFL_TEXTURE_FORMAT_H_
#define FFL_TEXTURE_FORMAT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum FFLTextureFormat
{
    FFL_TEXTURE_FORMAT_R8_UNORM = 0,
    FFL_TEXTURE_FORMAT_R8_G8_UNORM = 1,
    FFL_TEXTURE_FORMAT_R8_G8_B8_A8_UNORM = 2,
    FFL_TEXTURE_FORMAT_MAX = 3,
/* theoretical, from nn::mii:
    FFL_TEXTURE_FORMAT_BC4_UNORM = 3,
    FFL_TEXTURE_FORMAT_BC5_UNORM = 4,
    FFL_TEXTURE_FORMAT_BC7_UNORM = 5,
    FFL_TEXTURE_FORMAT_ASTC4x4_UNORM = 6,
*/
}
FFLTextureFormat;

#ifdef __cplusplus
}
#endif

#endif // FFL_TEXTURE_FORMAT_H_
