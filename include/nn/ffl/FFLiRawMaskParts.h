#ifndef FFLI_RAW_MASK_PARTS_H_
#define FFLI_RAW_MASK_PARTS_H_

#include <nn/ffl/FFLDrawParam.h>
#include <nn/ffl/FFLVec.h>

#include <math/rio_MathTypes.h>

struct FFLiRawMaskPartsDrawParam : FFLDrawParam
{
};
NN_STATIC_ASSERT32(sizeof(FFLiRawMaskPartsDrawParam) == 0x68);

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

class FFLiShaderCallback;

void FFLiInitDrawParamRawMaskParts(FFLiRawMaskPartsDrawParam* pDrawParam, const FFLiRawMaskPartsDesc* pDesc, const rio::BaseMtx44f* pProjMatrix);
void FFLiDeleteDrawParamRawMaskParts(FFLiRawMaskPartsDrawParam* pDrawParam);
void FFLiInvalidateDrawParamRawMaskParts(FFLiRawMaskPartsDrawParam* pDrawParam);
void FFLiInitDrawParamRawMaskPartsFill(FFLiRawMaskPartsDrawParam* pDrawParam);
void FFLiDeleteDrawParamRawMaskPartsFill(FFLiRawMaskPartsDrawParam* pDrawParam);
void FFLiDrawRawMaskParts(const FFLiRawMaskPartsDrawParam* pDrawParam, const FFLiShaderCallback* pCallback);

#endif // FFLI_RAW_MASK_PARTS_H_
