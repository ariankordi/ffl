#ifndef FFLI_SHAPE_H_
#define FFLI_SHAPE_H_

#include <nn/ffl/FFLResult.h>

#include <nn/ffl/FFLiShapePartsType.h>

extern bool g_FrontCullForFlipX;

void FFLiSetFrontCullForFlipX(bool enable);

struct FFLBoundingBox;
struct FFLDrawParam;
struct FFLVec3;

struct  FFLiCharModel;
class   FFLiCoordinate;
class   FFLiResourceLoader;

enum FFLiVertexLayoutType
{
    FFLI_VERTEX_LAYOUT_TYPE_DEFAULT,
    //FFLI_VERTEX_LAYOUT_TYPE_HALF_FLOAT = 1, // same as above but half float pos/texcoords
    FFLI_VERTEX_LAYOUT_TYPE_HALF_FLOAT_NORMAL_8888, // half float pos/texcoords AND 8888 normals
    FFLI_VERTEX_LAYOUT_TYPE_MAX
};

bool FFLiCanDrawShape(const FFLDrawParam* pDrawParam);

FFLResult FFLiLoadShape(void** ppShapeData, FFLDrawParam* pDrawParam, FFLBoundingBox* pBoundingBox, FFLiCharModel* pModel, FFLiShapePartsType partsType, u16 index, FFLiResourceLoader* pResLoader);
void FFLiDeleteShape(void** ppShapeData, FFLDrawParam* pDrawParam);
void FFLiAdjustShape(FFLDrawParam* pDrawParam, FFLBoundingBox* pBoundingBox, f32 scaleX, f32 scaleY, const FFLVec3* pTranslate, bool flipX, const FFLiCoordinate* pCoordinate, FFLiShapePartsType partsType, bool limitNoseScaleZ);

void FFLiInvalidateShape(FFLDrawParam* pDrawParam);

#endif // FFLI_SHAPE_H_
