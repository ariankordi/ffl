#ifndef FFL_RACE_H_
#define FFL_RACE_H_

#include <nn/ffl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum FFLRace
{
    FFL_RACE_BLACK  = 0,
    FFL_RACE_WHITE  = 1,
    FFL_RACE_ASIAN  = 2,
    FFL_RACE_MAX    = 3
}
FFLRace;

#ifdef __cplusplus
}
#endif

#endif // FFL_RACE_H_
