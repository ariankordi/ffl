#ifndef FFL_PARTS_TRANSFORM_H_
#define FFL_PARTS_TRANSFORM_H_

#include <nn/ffl/FFLVec.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FFLPartsTransform
{
    FFLVec3 hatTranslate;
    FFLVec3 headFrontRotate;
    FFLVec3 headFrontTranslate;
    FFLVec3 headSideRotate;
    FFLVec3 headSideTranslate;
    FFLVec3 headTopRotate;
    FFLVec3 headTopTranslate;
}
FFLPartsTransform;
NN_STATIC_ASSERT(sizeof(FFLPartsTransform) == 0x54);

#ifdef __cplusplus
}
#endif

#endif // FFL_PARTS_TRANSFORM_H_
