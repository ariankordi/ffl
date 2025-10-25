#include <nn/ffl/FFLiRawMaskParts.h>
#include <nn/ffl/FFLiShaderCallback.h>
#include <nn/ffl/FFLiUtil.h>

#include <nn/ffl/FFLiManager.h> // for g_TextureFlipY

#include <nn/ffl/detail/FFLiBug.h>

#include <math/rio_Matrix.h>
#include <misc/rio_MemUtil.h>

#if RIO_IS_CAFE
#include <gx2/mem.h>
#endif // RIO_IS_CAFE

namespace {

void CalcMVMatrix(rio::Matrix34f* pMVMatrix, const FFLiRawMaskPartsDesc* pDesc);

void InitPrimitive(FFLPrimitiveParam* pPrimitive);
void InitAttributes(FFLAttributeBufferParam* pAttributes, FFLiOriginPosition originPosition, const rio::BaseMtx44f* pMVPMatrix);

void DeletePrimitive(FFLPrimitiveParam* pPrimitive);
void DeleteAttributes(FFLAttributeBufferParam* pAttributes);

void InvalidatePrimitive(FFLPrimitiveParam* pPrimitive);
void InvalidateAttributes(FFLAttributeBufferParam* pAttributes);

void InitAttributesForFill(FFLAttributeBufferParam* pAttributes);

void DeleteAttributesForFill(FFLAttributeBufferParam* pAttributes);

}

void FFLiInitDrawParamRawMaskParts(FFLiRawMaskPartsDrawParam* pDrawParam, const FFLiRawMaskPartsDesc* pDesc, const rio::BaseMtx44f* pProjMatrix)
{
    rio::Matrix44f mvpMatrix;
    rio::Matrix34f mvMatrix;

    CalcMVMatrix(&mvMatrix, pDesc);

#ifdef FFL_USE_ADJUST_MTX_MASK
    rio::MemUtil::copy(&mvpMatrix, pProjMatrix, sizeof(rio::Matrix44f));
    pDrawParam->primitiveParam.pAdjustMatrix = new rio::Matrix34f(mvMatrix);
#else
    #ifdef FFL_USE_ADJUST_MTX
        pDrawParam->primitiveParam.pAdjustMatrix = 0;
    #else
        pDrawParam->primitiveParam._8 = 0;
    #endif
    mvpMatrix.fromMatrix34(mvMatrix);
    mvpMatrix.setMul(static_cast<const rio::Matrix44f&>(*pProjMatrix), mvpMatrix);
#endif // FFL_USE_ADJUST_MTX_MASK

    InitPrimitive(&pDrawParam->primitiveParam);
    InitAttributes(&pDrawParam->attributeBufferParam, pDesc->originPos, &mvpMatrix);
    pDrawParam->cullMode = FFL_CULL_MODE_MAX;
}

void FFLiDeleteDrawParamRawMaskParts(FFLiRawMaskPartsDrawParam* pDrawParam)
{
    DeletePrimitive(&pDrawParam->primitiveParam);
    DeleteAttributes(&pDrawParam->attributeBufferParam);
}

void FFLiInvalidateDrawParamRawMaskParts(FFLiRawMaskPartsDrawParam* pDrawParam)
{
    InvalidatePrimitive(&pDrawParam->primitiveParam);
    InvalidateAttributes(&pDrawParam->attributeBufferParam);
}

void FFLiInitDrawParamRawMaskPartsFill(FFLiRawMaskPartsDrawParam* pDrawParam)
{
    InitPrimitive(&pDrawParam->primitiveParam);
    InitAttributesForFill(&pDrawParam->attributeBufferParam);
    pDrawParam->cullMode = FFL_CULL_MODE_MAX;
}

void FFLiDeleteDrawParamRawMaskPartsFill(FFLiRawMaskPartsDrawParam* pDrawParam)
{
    DeletePrimitive(&pDrawParam->primitiveParam);
    DeleteAttributesForFill(&pDrawParam->attributeBufferParam);
}

void FFLiDrawRawMaskParts(const FFLiRawMaskPartsDrawParam* pDrawParam, const FFLiShaderCallback* pCallback)
{
    pCallback->CallDraw(pDrawParam);
}

namespace {

void CalcMVMatrix(rio::Matrix34f* pMVMatrix, const FFLiRawMaskPartsDesc* pDesc)
{
    static const f32 scaleAdjustX = 0.88961464f;
    static const f32 scaleAdjustY = 0.9276675f;

    //const rio::Vector3f scale =     { pDesc->scale.x * scaleAdjustX, pDesc->scale.y * scaleAdjustY, 1.0f };
    const rio::Vector3f rotate =    { 0.0f, 0.0f, rio::Mathf::deg2rad(pDesc->rot) };
    const rio::Vector3f translate = { pDesc->pos.x, pDesc->pos.y, 0.0f };

    //pMVMatrix->makeSRT(scale, rotate, translate);

    rio::Matrix34f scaleMatrix, rotateMatrix, scaleAdjustMatrix, translateMatrix;
    scaleMatrix.makeS({ pDesc->scale.x, pDesc->scale.y, 1.0f });
    pMVMatrix->setMul(scaleMatrix, *pMVMatrix);

    rotateMatrix.makeR(rotate);
    pMVMatrix->setMul(rotateMatrix, *pMVMatrix);

    scaleAdjustMatrix.makeS({ scaleAdjustX, scaleAdjustY, 1.0f });
    pMVMatrix->setMul(scaleAdjustMatrix, *pMVMatrix);

    translateMatrix.makeT(translate);
    pMVMatrix->setMul(translateMatrix, *pMVMatrix);

}

void* Allocate(u32 size, u32 alignment)
{
    return rio::MemUtil::alloc(size, alignment);
}

void Free(void* ptr)
{
    rio::MemUtil::free(ptr);
}

void EndianSwap(void* ptr, u32 size)
{
    FFLiBugEndianSwap(ptr, size);
}

void InitPrimitive(FFLPrimitiveParam* pPrimitive)
{
#ifndef FFL_USE_2D_TRIANGLE_STRIP
    static const u32 INDEX_COUNT = 6;
#else
    static const u32 INDEX_COUNT = 4;
#endif

    // IDX_ARRAY__36__N_20_FFLiRawMaskParts_cpp_7c9651f3..sugar_release_SDK2.2E07.2Exx.5Cprogram.5Cffl.5Ccafe_ffl.5Csystem.5Cobj.5Cghs.5Ccafe.5Clib.5Cffl.5CNDEBUG.5CFFLiRawMaskParts.
    static const u32 INDEX_BUFFER_SIZE = sizeof(u16) * INDEX_COUNT;

#ifndef FFL_USE_2D_TRIANGLE_STRIP
    pPrimitive->primitiveType = rio::Drawer::TRIANGLES;
    static const u16 INDEX_BUFFER[INDEX_COUNT] = { 2, 1, 3, 1, 3, 0 };
#else
    pPrimitive->primitiveType = rio::Drawer::TRIANGLE_STRIP;
    static const u16 INDEX_BUFFER[INDEX_COUNT] = { 2, 1, 3, 0 };
#endif
    NN_STATIC_ASSERT(sizeof(INDEX_BUFFER) == INDEX_BUFFER_SIZE);

    pPrimitive->indexCount = INDEX_COUNT;
    pPrimitive->pIndexBuffer = FFLiBugVgtFixedIndexPtr(Allocate(FFLiBugCanVgtFixedIndexSize(FFLiBugCanSwapSize(INDEX_BUFFER_SIZE)), rio::Drawer::cIdxAlignment));

    rio::MemUtil::copy(pPrimitive->pIndexBuffer, INDEX_BUFFER, INDEX_BUFFER_SIZE);
    EndianSwap(pPrimitive->pIndexBuffer, INDEX_BUFFER_SIZE);
}

void CalcAttribute(FFLVec4* pPosBuf, FFLVec2* pTexBuf, FFLiOriginPosition originPosition, const rio::BaseMtx44f* pMVPMatrix)
{
    f32 posXAdd = 0.0f;
    f32 texCoordX01 = 0.0f;
    f32 texCoordX23 = 0.0f;

    switch (originPosition)
    {
    case FFLI_ORIGIN_POSITION_CENTER:
        posXAdd = -0.5f;
        texCoordX01 = 1.0f;
        break;
    case FFLI_ORIGIN_POSITION_RIGHT:
        texCoordX23 = 1.0f;
        break;
    case FFLI_ORIGIN_POSITION_LEFT:
        posXAdd = -1.0f;
        texCoordX01 = 1.0f;
        break;
    }

    f32 posX[4] = { 1.0f, 1.0f, 0.0f, 0.0f };
    f32 posY[4] = { -0.5f, 0.5f, 0.5f, -0.5f };
    f32 texCoordY[4] = { 0.0f, 1.0f, 1.0f, 0.0f };

    for (u32 i = 0; i < 4; i++)
    {
        pPosBuf[i].x = posX[i] + posXAdd;
        pPosBuf[i].y = posY[i];
        pPosBuf[i].z = 0.0f;
        pPosBuf[i].w = 0.0f;

        pTexBuf[i].y = texCoordY[i];
    }

    pTexBuf[0].x = texCoordX01;
    pTexBuf[1].x = texCoordX01;
    pTexBuf[2].x = texCoordX23;
    pTexBuf[3].x = texCoordX23;

#ifndef FFL_USE_ADJUST_MTX_MASK
    for (u32 i = 0; i < 4; i++)
    {
        //const f32 w = pMVPMatrix->m[3][0] * pPosBuf[i].x + pMVPMatrix->m[3][1] * pPosBuf[i].y + pMVPMatrix->m[3][2] * pPosBuf[i].z + pMVPMatrix->m[3][3];
        //const f32 w_inv = 1 / w;
        const f32 w_inv = 1.0f;

        pPosBuf[i] = FFLVec4 {
            (pMVPMatrix->m[0][0] * pPosBuf[i].x + pMVPMatrix->m[0][1] * pPosBuf[i].y + pMVPMatrix->m[0][2] * pPosBuf[i].z + pMVPMatrix->m[0][3]) * w_inv,
            (pMVPMatrix->m[1][0] * pPosBuf[i].x + pMVPMatrix->m[1][1] * pPosBuf[i].y + pMVPMatrix->m[1][2] * pPosBuf[i].z + pMVPMatrix->m[1][3]) * w_inv,
            (pMVPMatrix->m[2][0] * pPosBuf[i].x + pMVPMatrix->m[2][1] * pPosBuf[i].y + pMVPMatrix->m[2][2] * pPosBuf[i].z + pMVPMatrix->m[2][3]) * w_inv,
            1.0f//pPosBuf[i].w
        };
    }
#endif // FFL_USE_ADJUST_MTX_MASK
}

void InitAttributes(FFLAttributeBufferParam* pAttributes, FFLiOriginPosition originPosition, const rio::BaseMtx44f* pMVPMatrix)
{
    const u32 POSITION_BUFFER_SIZE = sizeof(FFLVec4) * 4;
    const u32 TEXCOORD_BUFFER_SIZE = sizeof(FFLVec2) * 4;

    static const FFLAttributeBufferParam BASE_PARAM = { {
        // size                 stride          (ptr is unset)
        { POSITION_BUFFER_SIZE, sizeof(FFLVec4) },
        { TEXCOORD_BUFFER_SIZE, sizeof(FFLVec2) },
        { 0, 0 },
        { 0, 0 },
        { 0, 0 }
    } };

    rio::MemUtil::copy(pAttributes, &BASE_PARAM, sizeof(FFLAttributeBufferParam));

    pAttributes->attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_POSITION].ptr = Allocate(FFLiBugCanSwapSize(POSITION_BUFFER_SIZE), rio::Drawer::cVtxAlignment);
    pAttributes->attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_TEXCOORD].ptr = Allocate(FFLiBugCanSwapSize(TEXCOORD_BUFFER_SIZE), rio::Drawer::cVtxAlignment);

    CalcAttribute(
        static_cast<FFLVec4*>(pAttributes->attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_POSITION].ptr),
        static_cast<FFLVec2*>(pAttributes->attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_TEXCOORD].ptr),
        originPosition,
        pMVPMatrix
    );

    EndianSwap(pAttributes->attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_POSITION].ptr, POSITION_BUFFER_SIZE);
    EndianSwap(pAttributes->attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_TEXCOORD].ptr, TEXCOORD_BUFFER_SIZE);
}

void DeletePrimitive(FFLPrimitiveParam* pPrimitive)
{
    Free(FFLiBugVgtFixedIndexOriginalPtr(pPrimitive->pIndexBuffer));
}

void DeleteAttributes(FFLAttributeBufferParam* pAttributes)
{
    Free(pAttributes->attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_POSITION].ptr);
    Free(pAttributes->attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_TEXCOORD].ptr);
}

void InvalidatePrimitive(FFLPrimitiveParam* pPrimitive)
{
#if RIO_IS_CAFE
    GX2Invalidate(
        GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER,
        pPrimitive->pIndexBuffer,
        sizeof(u16) * pPrimitive->indexCount    // Apparently Nintendo forgot the index count is 4
    );
#endif // RIO_IS_CAFE
}

void InvalidateAttributes(FFLAttributeBufferParam* pAttributes)
{
#if RIO_IS_CAFE
    for (u32 i = 0; i < FFL_ATTRIBUTE_BUFFER_TYPE_MAX; i++)
    {
        void* ptr = pAttributes->attributeBuffers[i].ptr;
        if (ptr != NULL)
            GX2Invalidate(
                GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER,
                ptr,
                pAttributes->attributeBuffers[i].size
            );
    }
#endif // RIO_IS_CAFE
}

void InitAttributesForFill(FFLAttributeBufferParam* pAttributes)
{
    const u32 POSITION_BUFFER_SIZE = sizeof(FFLVec4) * 4;

    // POSITIONS.InitAttributesForFill__36__N_20_FFLiRawMaskParts_cpp_7c9651f3FP23FFLAttributeBufferParamP19FFLiBufferAllocator.sugar_release_SDK2.2E07.2Exx.5Cprogram.5Cffl.5Ccafe_ffl.5Csystem.5Cobj.5Cghs.5Ccafe.5Clib.5Cffl.5CNDEBUG.5CFFLiRawMaskParts..3

    static const FFLVec4 POSITIONS[4] = {
        {  1.0f, -1.0f,  0.0f,  0.0f },
        {  1.0f,  1.0f,  0.0f,  0.0f },
        { -1.0f,  1.0f,  0.0f,  0.0f },
        { -1.0f, -1.0f,  0.0f,  0.0f }
    };

    // NOTE: no texture, orientation does not matter, no Y flip needed.. I think
    static const FFLVec4 POSITIONS_FLIP[4] = {
        {  1.0f,  1.0f,  0.0f,  0.0f },
        {  1.0f, -1.0f,  0.0f,  0.0f },
        { -1.0f, -1.0f,  0.0f,  0.0f },
        { -1.0f,  1.0f,  0.0f,  0.0f }
    };

    NN_STATIC_ASSERT(sizeof(POSITIONS) == POSITION_BUFFER_SIZE);
    NN_STATIC_ASSERT(sizeof(POSITIONS_FLIP) == POSITION_BUFFER_SIZE);

    static const FFLAttributeBufferParam BASE_PARAM = { {
        { POSITION_BUFFER_SIZE, sizeof(FFLVec4) },
        { 0, 0 },
        { 0, 0 },
        { 0, 0 },
        { 0, 0 }
    } };

    rio::MemUtil::copy(pAttributes, &BASE_PARAM, sizeof(FFLAttributeBufferParam));

    pAttributes->attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_POSITION].ptr = Allocate(FFLiBugCanSwapSize(POSITION_BUFFER_SIZE), rio::Drawer::cVtxAlignment);

    if (g_TextureFlipY)
        rio::MemUtil::copy(pAttributes->attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_POSITION].ptr, POSITIONS_FLIP, POSITION_BUFFER_SIZE);
    else
        rio::MemUtil::copy(pAttributes->attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_POSITION].ptr, POSITIONS, POSITION_BUFFER_SIZE);

    EndianSwap(pAttributes->attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_POSITION].ptr, POSITION_BUFFER_SIZE);
}

void DeleteAttributesForFill(FFLAttributeBufferParam* pAttributes)
{
    Free(pAttributes->attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_POSITION].ptr);
}

}
