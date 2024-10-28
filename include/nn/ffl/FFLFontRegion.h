#ifndef FFL_FONT_REGION_H_
#define FFL_FONT_REGION_H_

#include <nn/ffl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum FFLFontRegion
{
    FFL_FONT_REGION_JP_US_EU = 0,
    FFL_FONT_REGION_CHINA    = 1,
    FFL_FONT_REGION_KOREA    = 2,
    FFL_FONT_REGION_TAIWAN   = 3
}
FFLFontRegion;

#ifdef __cplusplus
}
#endif

#endif // FFL_FONT_REGION_H_
