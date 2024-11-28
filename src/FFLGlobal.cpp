#include <nn/ffl/FFLGlobal.h>

#include <nn/ffl/FFLiColor.h>
#include <nn/ffl/FFLiManager.h>

#ifdef FFL_ADD_GLAD_GL_IMPLEMENTATION
    #ifdef RIO_GLES
        #define gladLoadGLES2 FFLGladLoadGL

        #define GLAD_EGL_IMPLEMENTATION
        #define GLAD_GLES2_IMPLEMENTATION

        #include <glad/gles2.h>
    #else
        #define gladLoadGL FFLGladLoadGL

        #define GLAD_GL_IMPLEMENTATION

        #include <glad/gl.h>
    #endif // RIO_GLES
#endif // FFL_ADD_GLAD_GL_IMPLEMENTATION

// FFLiInitResEx(void *, FFLInitDesc const *, FFLResourceDesc const *)
FFLResult FFLInitResEx(const FFLInitDesc* pInitDesc, const FFLResourceDesc* pResDesc)
{
    return FFLiInitResEx(pInitDesc, pResDesc);
}

// NOTE: Does not match FFLInitRes, just provided for simplicity
// Original function: FFLiInitRes(void *, FFLInitDesc const *)
FFLResult FFLInitRes(FFLFontRegion fontRegion, const FFLResourceDesc* pResDesc)
{
    const FFLInitDesc initDesc = {
        .fontRegion = fontRegion,
        ._c = false,
        ._10 = true
    };
    return FFLiInitResEx(&initDesc, pResDesc);
}

void FFLInitResGPUStep(void)
{
    FFLiInitResGPUStep();
}

FFLResult FFLFlushQuota(void)
{
    return FFLiFlushQuota(false);
}

FFLResult FFLExit(void)
{
    return FFLiExit();
}

bool FFLIsAvailable(void)
{
    return FFLiIsAvailable();
}

FFLColor FFLGetFavoriteColor(s32 index)
{
    return FFLiGetFavoriteColor(index);
}

FFLColor FFLGetFacelineColor(s32 index)
{
    return FFLiGetFacelineColor(index);
}

void FFLSetTextureFlipY(bool textureFlipY) // not in FFL obviously
{
    FFLiSetTextureFlipY(textureFlipY);
}