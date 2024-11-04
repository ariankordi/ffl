#include <nn/ffl/FFLGlobal.h>

#include <nn/ffl/FFLiColor.h>
#include <nn/ffl/FFLiManager.h>

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
