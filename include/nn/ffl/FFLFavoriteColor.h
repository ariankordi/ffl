#ifndef FFL_FAVORITE_COLOR_H_
#define FFL_FAVORITE_COLOR_H_

#include <nn/ffl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum FFLFavoriteColor
{
    FFL_FAVORITE_COLOR_RED         = 0,
    FFL_FAVORITE_COLOR_ORANGE      = 1,
    FFL_FAVORITE_COLOR_YELLOW      = 2,
    FFL_FAVORITE_COLOR_YELLOWGREEN = 3,
    FFL_FAVORITE_COLOR_GREEN       = 4,
    FFL_FAVORITE_COLOR_BLUE        = 5,
    FFL_FAVORITE_COLOR_SKYBLUE     = 6,
    FFL_FAVORITE_COLOR_PINK        = 7,
    FFL_FAVORITE_COLOR_PURPLE      = 8,
    FFL_FAVORITE_COLOR_BROWN       = 9,
    FFL_FAVORITE_COLOR_WHITE       = 10,
    FFL_FAVORITE_COLOR_BLACK       = 11,
    FFL_FAVORITE_COLOR_MAX         = 12
}
FFLFavoriteColor;

#ifdef __cplusplus
}
#endif

#endif // FFL_FAVORITE_COLOR_H_
