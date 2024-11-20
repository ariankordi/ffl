#ifndef FFL_SHADER_CALLBACK_H_
#define FFL_SHADER_CALLBACK_H_

#include <nn/ffl/types.h>

#if FFL_USE_RIO
    #include <gfx/rio_Graphics.h>
    typedef rio::Graphics::CompareFunc FFLRIOCompareFunc;

    #include <math/rio_Matrix.h>
    typedef rio::BaseMtx44f FFLRIOBaseMtx44f;
#else
    #include <nn/ffl/FFLRIOInterop.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FFLDrawParam FFLDrawParam;

typedef struct FFLShaderCallback
{
    void*               pObj;
#ifdef FFL_USE_FACELINE_COLOR_IS_TRANSPARENT_PROPERTY
    // allow shader to specify that faceline color should have A set to 0, needed for switch shader when drawType is set to faceline
    bool                facelineColorIsTransparent;
#endif // FFL_USE_FACELINE_COLOR_IS_TRANSPARENT_PROPERTY
    void (*pApplyAlphaTestFunc)(void* pObj, bool enable, FFLRIOCompareFunc func, f32 ref);
    void (*pDrawFunc)(void* pObj, const FFLDrawParam* drawParam);
    void (*pSetMatrixFunc)(void* pObj, const FFLRIOBaseMtx44f* matrix);
}
FFLShaderCallback;
NN_STATIC_ASSERT32(sizeof(FFLShaderCallback) == 0x10);

void FFLSetShaderCallback(const FFLShaderCallback* pCallback);

#ifdef __cplusplus
}
#endif

#endif // FFL_SHADER_CALLBACK_H_
