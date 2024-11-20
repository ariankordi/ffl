#ifndef FFLI_FACELINE_TEXTURE_TEMP_OBJECT_H_
#define FFLI_FACELINE_TEXTURE_TEMP_OBJECT_H_

#include <nn/ffl/FFLDrawParam.h>

#include <nn/ffl/FFLTexture.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FFLiFacelineTextureTempObject
{
    FFLRIOTexture2D* pTextureFaceLine;
    FFLDrawParam     drawParamFaceLine;
    FFLRIOTexture2D* pTextureFaceMake;
    FFLDrawParam     drawParamFaceMake;
    FFLRIOTexture2D* pTextureFaceBeard;
    FFLDrawParam     drawParamFaceBeard;
    void*            _144;   // Deleted
    void*            _148;   // Deleted
}
FFLiFacelineTextureTempObject;
NN_STATIC_ASSERT32(sizeof(FFLiFacelineTextureTempObject) == 0x14C);

#ifdef __cplusplus
}
#endif

#endif // FFLI_FACELINE_TEXTURE_TEMP_OBJECT_H_
