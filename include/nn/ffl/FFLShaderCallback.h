#ifndef FFL_SHADER_CALLBACK_H_
#define FFL_SHADER_CALLBACK_H_

#include <nn/ffl/types.h>

#if FFL_USE_RIO
    #include <gfx/rio_Graphics.h>
    #include <math/rio_Matrix.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FFLDrawParam FFLDrawParam;

typedef struct FFLShaderCallback
{
    void*               pObj;
    // allow shader to specify that faceline color should
    // have A set to 0, needed for switch shader when
    // drawType is set to faceline (only when letting FFL make render textures)
    bool                facelineColorIsTransparent;
    // ^^ Apparently this is called "facelineLeaveAlpha"
    // in nn::mii::detail::TextureShaderInfoData/TextureShaderImpl
    u8                  _padding[3];

#if FFL_USE_RIO
    // this is not used for no render texture
    void (*pApplyAlphaTestFunc)(void* pObj, bool enable, rio::Graphics::CompareFunc func, f32 ref);
#else
    void* _padding1[1];
#endif // FFL_NO_RENDER_TEXTURE
    // ^^ Unused when FFL_NO_DRAW_MASK_ALPHA_VALUES is set (FFLiDrawRawMask)

    void (*pDrawFunc)(void* pObj, const FFLDrawParam* drawParam);

    void (*pSetMatrixFunc)(void* pObj,
#if FFL_USE_RIO
                           const rio::BaseMtx44f* matrix);
#else
                           const float matrix[16]);
#endif // FFL_USE_RIO
}
FFLShaderCallback;
NN_STATIC_ASSERT32(sizeof(FFLShaderCallback) == 0x14);

void FFLSetShaderCallback(const FFLShaderCallback* pCallback);

#ifdef __cplusplus
}
#endif

#endif // FFL_SHADER_CALLBACK_H_
