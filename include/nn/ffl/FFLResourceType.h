#ifndef FFL_RESOURCE_TYPE_H_
#define FFL_RESOURCE_TYPE_H_

#include <nn/ffl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum FFLResourceType
{
    FFL_RESOURCE_TYPE_MIDDLE    = 0,
    FFL_RESOURCE_TYPE_HIGH      = 1,
#ifdef FFL_USE_EXTRA_RESOURCES
    // Add more resource types.
    FFL_RESOURCE_TYPE_VERY_HIGH = 2, // FFLResPoster.dat
    FFL_RESOURCE_TYPE_LOW       = 3,
    FFL_RESOURCE_TYPE_MAX       = 4
#else
    FFL_RESOURCE_TYPE_MAX       = 2
#endif // FFL_USE_EXTRA_RESOURCES
}
FFLResourceType;

#ifdef __cplusplus
}
#endif

#endif // FFL_RESOURCE_TYPE_H_
