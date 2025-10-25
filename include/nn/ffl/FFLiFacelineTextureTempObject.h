#ifndef FFLI_FACELINE_TEXTURE_TEMP_OBJECT_H_
#define FFLI_FACELINE_TEXTURE_TEMP_OBJECT_H_

#include <nn/ffl/FFLDrawParam.h>

#include <nn/ffl/FFLTexture.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FFLiFacelineTextureTempObject
{
    /* Original order:
    FFLTexture*   pTextureFaceLine;
    FFLDrawParam  drawParamFaceLine;
    FFLTexture*   pTextureFaceMake;
    FFLDrawParam  drawParamFaceMake;
    FFLTexture*   pTextureFaceBeard;
    FFLDrawParam  drawParamFaceBeard;
    void*         _144;   // Deleted
    void*         _148;   // Deleted
    */
    // More closely matches the draw order.
    // FFLDrawParam  drawParamFaceSkin; // New: Solid skin color.
    FFLDrawParam  drawParamFaceMake;
    FFLDrawParam  drawParamFaceLine;
    FFLDrawParam  drawParamFaceBeard;
    FFLTexture*   pTextureFaceLine;
    FFLTexture*   pTextureFaceMake;
    FFLTexture*   pTextureFaceBeard;
    void*         _144;   // Deleted
    void*         _148;   // Deleted
}
FFLiFacelineTextureTempObject;
NN_STATIC_ASSERT32(sizeof(FFLiFacelineTextureTempObject) == 0x14C); // 0x1B4);

#ifdef __cplusplus
}
#endif

#endif // FFLI_FACELINE_TEXTURE_TEMP_OBJECT_H_
