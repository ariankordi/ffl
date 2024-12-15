#ifndef FFLI_SHAPE_PARTS_TYPE_H_
#define FFLI_SHAPE_PARTS_TYPE_H_

#include <nn/ffl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum FFLiShapePartsType
{
    FFLI_SHAPE_PARTS_TYPE_BEARD           =  0,
    FFLI_SHAPE_PARTS_TYPE_HAT_NORMAL      =  1,
    FFLI_SHAPE_PARTS_TYPE_HAT_CAP         =  2,
    FFLI_SHAPE_PARTS_TYPE_FACELINE        =  3,
    FFLI_SHAPE_PARTS_TYPE_GLASS           =  4,
    FFLI_SHAPE_PARTS_TYPE_MASK            =  5,
    FFLI_SHAPE_PARTS_TYPE_NOSELINE        =  6,
    FFLI_SHAPE_PARTS_TYPE_NOSE            =  7,
    FFLI_SHAPE_PARTS_TYPE_HAIR_NORMAL     =  8,
    FFLI_SHAPE_PARTS_TYPE_HAIR_CAP        =  9,
    FFLI_SHAPE_PARTS_TYPE_FOREHEAD_NORMAL = 10,
    FFLI_SHAPE_PARTS_TYPE_FOREHEAD_CAP    = 11,
    FFLI_SHAPE_PARTS_TYPE_MAX             = 12
}
FFLiShapePartsType;

#ifdef __cplusplus
}
#endif

#endif // FFLI_SHAPE_PARTS_TYPE_H_
