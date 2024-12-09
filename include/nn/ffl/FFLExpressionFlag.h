#ifndef FFL_EXPRESSION_FLAG_H_
#define FFL_EXPRESSION_FLAG_H_

#include <nn/ffl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Structure of 96 bits in total to represent
// more expression flags than the default of 31.
typedef union FFLAllExpressionFlag
{
    struct {
        u32 low;  // 0-31
        // NOTE: The higher flags are only used if
        // the model flag is set: FFL_MODEL_FLAG_NEW_EXPRESSIONS
        u32 mid;  // 32-63
        u32 high; // 63-69
    } flag;
    u32 flags[3]; // 0-96
}
FFLAllExpressionFlag;

// NOTE: RFL contains the "RFLExpressionFlag" type to represent
// a flag to enable every expression and I'm pretty sure nn::mii
// has the same thing so if that's what you expected in this file, sorry

#ifdef __cplusplus
}
#endif

#endif // FFL_EXPRESSION_FLAG_H_
