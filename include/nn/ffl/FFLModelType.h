#ifndef FFL_MODEL_TYPE_H_
#define FFL_MODEL_TYPE_H_

#include <nn/ffl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum FFLModelType
{
    FFL_MODEL_TYPE_NORMAL    = 0,
    FFL_MODEL_TYPE_HAT       = 1,
    FFL_MODEL_TYPE_FACE_ONLY = 2,
    FFL_MODEL_TYPE_MAX       = 3
}
FFLModelType;

// should this be in its own file?
typedef enum FFLModelFlag
{
    FFL_MODEL_FLAG_NORMAL        = 1 << FFL_MODEL_TYPE_NORMAL,
    FFL_MODEL_FLAG_HAT           = 1 << FFL_MODEL_TYPE_HAT,
    FFL_MODEL_FLAG_FACE_ONLY     = 1 << FFL_MODEL_TYPE_FACE_ONLY,
    FFL_MODEL_FLAG_FLATTEN_NOSE  = 1 << 3,
} FFLModelFlag;

#ifdef __cplusplus
}
#endif

#endif // FFL_MODEL_TYPE_H_
