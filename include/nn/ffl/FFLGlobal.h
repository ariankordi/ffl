#ifndef FFL_GLOBAL_H_
#define FFL_GLOBAL_H_

#include <nn/ffl/FFLColor.h>
#include <nn/ffl/FFLFontRegion.h>
#include <nn/ffl/FFLResult.h>

#include <nn/ffl/FFLExpressionFlag.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FFLInitDesc FFLInitDesc;
typedef struct FFLResourceDesc FFLResourceDesc;

FFLResult FFLInitResEx(const FFLInitDesc* pInitDesc, const FFLResourceDesc* pResDesc);
FFLResult FFLInitRes(FFLFontRegion fontRegion, const FFLResourceDesc* pResDesc);
void FFLInitResGPUStep(void);
FFLResult FFLFlushQuota(void);
FFLResult FFLExit(void);
bool FFLIsAvailable(void);

FFLColor FFLGetFavoriteColor(s32 index);
void FFLSetLinearGammaMode(bool isLinear); // Deleted in NSMBU

FFLColor FFLGetFacelineColor(s32 index); // NOTE: this and below are not in FFL
void FFLSetTextureFlipY(bool textureFlipY);
void FFLSetNormalIsSnorm8_8_8_8(bool enable);
void FFLSetFrontCullForFlipX(bool enable);

// Helper method to set a specific index within an expression flag.
void FFLSetExpressionFlagIndex(FFLAllExpressionFlag* ef, u32 index, bool set);

#ifdef __cplusplus
}
#endif

#endif // FFL_GLOBAL_H_
