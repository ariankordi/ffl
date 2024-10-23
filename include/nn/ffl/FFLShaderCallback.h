#ifndef FFL_SHADER_CALLBACK_H_
#define FFL_SHADER_CALLBACK_H_

#include <nn/ffl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FFLDrawParam FFLDrawParam;

typedef struct FFLShaderCallback
{
    void*               pObj;
    // allow shader to specify that faceline color should have A set to 0
    bool                facelineColorIsTransparent;

    void (*pApplyAlphaTestFunc)(void* pObj, bool enable, FFLRIOCompareFunc func, f32 ref);
    void (*pDrawFunc)(void* pObj, const FFLDrawParam* drawParam);
    void (*pSetMatrixFunc)(void* pObj, const FFLRIOBaseMtx44f* matrix);
}
FFLShaderCallback;
NN_STATIC_ASSERT32(sizeof(FFLShaderCallback) == 0x14);

void FFLSetShaderCallback(const FFLShaderCallback* pCallback);

#ifdef __cplusplus
}
#endif

#endif // FFL_SHADER_CALLBACK_H_
