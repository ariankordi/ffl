#ifndef FFL_MODEL_FLAG_H_
#define FFL_MODEL_FLAG_H_

#include <nn/ffl/types.h>
#include <nn/ffl/FFLModelType.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum FFLModelFlag
{
    FFL_MODEL_FLAG_NORMAL          = 1 << (int)FFL_MODEL_TYPE_NORMAL,
    // Uses a variant of hair designed for a hat.
    FFL_MODEL_FLAG_HAT             = 1 << (int)FFL_MODEL_TYPE_HAT,
    // Discards hair from the model for helmets, etc.
    FFL_MODEL_FLAG_FACE_ONLY       = 1 << (int)FFL_MODEL_TYPE_FACE_ONLY,
    // Limits the Z depth on a nose for helmets, etc.
    FFL_MODEL_FLAG_FLATTEN_NOSE    = 1 << 3,
    // Enables expression flag to express beyond 32 expressions.
    FFL_MODEL_FLAG_NEW_EXPRESSIONS = 1 << 4,
#ifdef FFL_ENABLE_NEW_MASK_ONLY_FLAG
    // This flag will only make new textures
    // when initializing a CharModel and not
    // initialize shapes. Note that this means
    // you cannot DrawOpa/Xlu when this is set.
    FFL_MODEL_FLAG_NEW_MASK_ONLY = 1 << 5,
#endif
} FFLModelFlag;

#ifdef __cplusplus
}
#endif

#endif // FFL_MODEL_FLAG_H_
