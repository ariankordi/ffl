#ifndef FFL_GLOBAL_H_
#define FFL_GLOBAL_H_

#include <nn/ffl/FFLColor.h>
#include <nn/ffl/FFLResult.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FFLInitDesc FFLInitDesc;
typedef struct FFLResourceDesc FFLResourceDesc;

FFLResult FFLInitResEx(const FFLInitDesc* pInitDesc, const FFLResourceDesc* pResDesc);
void FFLInitResGPUStep(void);
FFLResult FFLFlushQuota(void);
FFLResult FFLExit(void);
bool FFLIsAvailable(void);
FFLColor FFLGetFavoriteColor(s32 index);

#ifdef __cplusplus
}
#endif

#endif // FFL_GLOBAL_H_
