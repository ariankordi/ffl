#ifndef FFLI_RAW_MASK_PARTS_H_
#define FFLI_RAW_MASK_PARTS_H_

#include <nn/ffl/FFLDrawParam.h>
#include <nn/ffl/FFLVec.h>

#ifdef __cplusplus
struct FFLiRawMaskPartsDrawParam : FFLDrawParam
{
};
#else
typedef FFLDrawParam FFLiRawMaskPartsDrawParam;
#endif
NN_STATIC_ASSERT32(sizeof(FFLiRawMaskPartsDrawParam) == 0x68);

#ifdef __cplusplus

enum FFLiOriginPosition
{
    FFLI_ORIGIN_POSITION_CENTER = 0,
    FFLI_ORIGIN_POSITION_RIGHT  = 1,
    FFLI_ORIGIN_POSITION_LEFT   = 2
};

struct FFLiRawMaskPartsDesc
{
    FFLVec2             pos;
    FFLVec2             scale;
    f32                 rot;
    FFLiOriginPosition  originPos;
};
NN_STATIC_ASSERT(sizeof(FFLiRawMaskPartsDesc) == 0x18);


#include <math/rio_MathTypes.h>

class FFLiShaderCallback;

void FFLiInitDrawParamRawMaskParts(FFLiRawMaskPartsDrawParam* pDrawParam, const FFLiRawMaskPartsDesc* pDesc, const rio::BaseMtx44f* pProjMatrix);
void FFLiDeleteDrawParamRawMaskParts(FFLiRawMaskPartsDrawParam* pDrawParam);
void FFLiInvalidateDrawParamRawMaskParts(FFLiRawMaskPartsDrawParam* pDrawParam);
void FFLiInitDrawParamRawMaskPartsFill(FFLiRawMaskPartsDrawParam* pDrawParam);
void FFLiDeleteDrawParamRawMaskPartsFill(FFLiRawMaskPartsDrawParam* pDrawParam);
void FFLiDrawRawMaskParts(const FFLiRawMaskPartsDrawParam* pDrawParam, const FFLiShaderCallback* pCallback);

#endif // __cplusplus

#endif // FFLI_RAW_MASK_PARTS_H_
