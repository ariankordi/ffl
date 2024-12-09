#include <nn/ffl/FFLGlobal.h>

#include <nn/ffl/FFLiColor.h>
#include <nn/ffl/FFLiManager.h>

#include <nn/ffl/detail/FFLiResourceShape.h> // FFLiSetNormalIsSnorm8_8_8_8
#include <nn/ffl/FFLiShape.h> // FFLiSetFrontCullForFlipX

#include <nn/ffl/FFLExpression.h> // FFL_EXPRESSION_LIMIT

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
#else
    void FFLGladLoadGL(void)
    {
        // no-op just so that this resolves
        RIO_LOG("FFLGladLoadGL called but FFL_ADD_GLAD_GL_IMPLEMENTATION not defined, ignoring");
    }
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

// All functions below are not originally in FFL:

FFLColor FFLGetFacelineColor(s32 index)
{
    return FFLiGetFacelineColor(index);
}

void FFLSetExpressionFlagIndex(FFLAllExpressionFlag* ef, u32 index, bool set)
{
    if (index < 0 || index >= FFL_EXPRESSION_LIMIT) {
        RIO_LOG("FFLSetExpressionFlagIndex: input out of range: %d\n", index);
        return; // Do not set anything.
    }

    s32 part = index / 32;       // Determine which 32-bit block
    s32 bitIndex = index % 32;   // Determine which bit within the block

    if (set)
        ef->flags[part] |= (1 << bitIndex);  // Set the bit
    else
        ef->flags[part] &= ~(1 << bitIndex); // Clear the bit
}

void FFLSetTextureFlipY(bool textureFlipY)
{
    FFLiSetTextureFlipY(textureFlipY);
}
void FFLSetNormalIsSnorm8_8_8_8(bool enable)
{
    FFLiSetNormalIsSnorm8_8_8_8(enable);
}
void FFLSetFrontCullForFlipX(bool enable)
{
    FFLiSetFrontCullForFlipX(enable);
}
