#ifndef FFL_CHAR_MODEL_DESC_H_
#define FFL_CHAR_MODEL_DESC_H_

#include <nn/ffl/FFLResolution.h>
#include <nn/ffl/FFLResourceType.h>

#include <nn/ffl/FFLExpressionFlag.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FFLCharModelDesc
{
    FFLResolution     resolution;
    //u32             expressionFlag; // Previous version
    union {
        // Union to allow storage of an expanded
        // expression flag for up to 96 expressions.
        u32  expressionFlag; // Standard usage
        FFLAllExpressionFlag allExpressionFlag;
    };
    u32               modelFlag;
    FFLResourceType   resourceType;
    //int               _10;    // Deleted
}
FFLCharModelDesc;
//NN_STATIC_ASSERT(sizeof(FFLCharModelDesc) == 0x20);

#ifdef __cplusplus
}
#endif

#endif // FFL_CHAR_MODEL_DESC_H_
