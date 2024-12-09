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

#ifdef __cplusplus
}
#endif

#endif // FFL_MODEL_TYPE_H_
