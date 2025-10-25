#include <nn/ffl/FFLiCharModel.h>
#include <nn/ffl/FFLiCoordinate.h>
#include <nn/ffl/FFLiResourceLoader.h>
#include <nn/ffl/FFLiResourceHeader.h>
#include <nn/ffl/FFLiShape.h>

#include <nn/ffl/FFLiUtil.h>
#include <nn/ffl/detail/FFLiBug.h>
#include <nn/ffl/detail/FFLiResourceShape.h>

#include <gpu/rio_VertexStream.h>
#ifdef FFL_USE_ADJUST_MTX
    #include <math/rio_Matrix.h>
#endif // FFL_USE_ADJUST_MTX

#if RIO_IS_CAFE
#include <gx2/mem.h>
#endif // RIO_IS_CAFE

union F32BitCast
{
    f32 f;
    u32 u;
    struct
    {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        u32 sign        : 1;    // (MSB)
        u32 exponent    : 8;
        u32 mantissa    : 23;   // (LSB)
#else
        u32 mantissa    : 23;   // (LSB)
        u32 exponent    : 8;
        u32 sign        : 1;    // (MSB)
#endif // __BYTE_ORDER__
    };
};
NN_STATIC_ASSERT(sizeof(F32BitCast) == 4);

[[maybe_unused]] static bool IsNaN(f32 value)
{
    F32BitCast x = { value };
    // Basically:
    // return x.exponent == 0xff && x.mantissa > 0;
    return (x.u << 1) > 0xff000000;
}

// Controls whether to set front face culling
// when flipping X on shapes (flipped hair).
// If this is false, the index buffer will be
// adjusted to reverse triangle winding.
#ifndef FFL_USE_ADJUST_MTX
bool g_FrontCullForFlipX = true; // True by default in FFL.
#else
bool g_FrontCullForFlipX = false; // Not necessary with matrix.
#endif

void FFLiSetFrontCullForFlipX(bool enable)
{
    g_FrontCullForFlipX = enable;
}

namespace {

FFLiResourceShapeElementType GetElementType(FFLAttributeBufferType type);

u32 GetStride(FFLAttributeBufferType type, u32 size, FFLiVertexLayoutType layoutType);

//void EndianSwap(void* ptr, u32 size);

template <typename T>
void AdjustAttribute(T* pVec, u32 num, f32 scaleX, f32 scaleY, f32 scaleZ, const FFLVec3* pTranslate, bool flipX, const FFLiCoordinate* pCoordinate);

template <typename T>
void AdjustAttributeWithoutScale(T* pVec, u32 num, bool flipX, const FFLiCoordinate* pCoordinate);

void AdjustIndexBuffer(void* pIndexPtr, u32 indexCount);

}

bool FFLiCanDrawShape(const FFLDrawParam* pDrawParam)
{
    return pDrawParam->primitiveParam.indexCount != 0;
}

FFLResult FFLiLoadShape(void** ppShapeData, FFLDrawParam* pDrawParam, FFLBoundingBox* pBoundingBox, FFLiCharModel* pModel, FFLiShapePartsType partsType, u16 index, FFLiResourceLoader* pResLoader)
{
    void*& pData = *ppShapeData;

    u32 size = pResLoader->GetShapeAlignedMaxSize(partsType);

    // Sanity check. Feel free to remove this if you do have resources this large.
    RIO_ASSERT(size < 200000000 && "Are you sure this shape is supposed to be over 200 MB large?");

    pData = rio::MemUtil::alloc(
#ifdef FFL_USE_ADJUST_MTX
        // Let's allocate the matrix inside of the buffer.
        size + sizeof(rio::Matrix34f),
#else
        size,
#endif // FFL_USE_ADJUST_MTX
        rio::FileDevice::cBufferMinAlignment);

    FFLResult result = pResLoader->LoadShape(pData, &size, partsType, index);
    if (result != FFL_RESULT_OK)
    {
        rio::MemUtil::free(pData);
        pData = nullptr;
        return result;
    }

    if (size == 0)
    {
        rio::MemUtil::free(pData);
        pData = nullptr;

        rio::MemUtil::set(pDrawParam, 0, sizeof(FFLDrawParam));

        const u32 NaN = 0x7FC00000;
        static const u32 INVALID_BOUNDING_BOX[2][3] = {
            { NaN, NaN, NaN },
            { NaN, NaN, NaN }
        };
        NN_STATIC_ASSERT(sizeof(INVALID_BOUNDING_BOX) == sizeof(FFLBoundingBox));
        rio::MemUtil::copy(pBoundingBox, INVALID_BOUNDING_BOX, sizeof(FFLBoundingBox));

        return FFL_RESULT_OK;
    }

    const FFLiVertexLayoutType layoutType = pResLoader->Header()->VertexLayoutType();
    for (u32 i = 0; i < FFL_ATTRIBUTE_BUFFER_TYPE_MAX; i++)
    {
        FFLiResourceShapeElementType elementType = GetElementType(FFLAttributeBufferType(i));
        FFLAttributeBuffer& attribute = pDrawParam->attributeBufferParam.attributeBuffers[i];
        attribute.ptr = const_cast<void*>(FFLiGetResourceShapeElement(&attribute.size, pData, partsType, elementType, layoutType));
        attribute.stride = GetStride(FFLAttributeBufferType(i), attribute.size, layoutType);
    }

    {
        FFLPrimitiveParam& primitive = pDrawParam->primitiveParam;
        primitive.pIndexBuffer = const_cast<void*>(FFLiGetResourceShapeElement(&primitive.indexCount, pData, partsType, FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_INDEX, layoutType));
        primitive.primitiveType = rio::Drawer::TRIANGLES;
    }

    if (partsType == FFLI_SHAPE_PARTS_TYPE_HAIR_NORMAL)
    {
        const FFLiResourceShapeHairTransform* pTransform = static_cast<const FFLiResourceShapeHairTransform*>(FFLiGetResourceShapeElement(&size, pData, partsType, FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TRANSFORM_HAIR, layoutType));

        pModel->partsTransform.headFrontTranslate = pTransform->GetFrontTranslate();
        pModel->partsTransform.headFrontRotate  = pTransform->GetFrontRotate();
        pModel->partsTransform.headSideTranslate = pTransform->GetSideTranslate();
        pModel->partsTransform.headSideRotate = pTransform->GetSideRotate();
        pModel->partsTransform.headTopTranslate = pTransform->GetTopTranslate();
        pModel->partsTransform.headTopRotate = pTransform->GetTopRotate();
    }
    else if (partsType == FFLI_SHAPE_PARTS_TYPE_FACELINE)
    {
        const FFLiResourceShapeFacelineTransform* pTransform = static_cast<const FFLiResourceShapeFacelineTransform*>(FFLiGetResourceShapeElement(&size, pData, partsType, FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TRANSFORM_FACELINE, layoutType));

        pModel->beardPos        = pTransform->GetBeardTranslate();
        pModel->hairPos         = pTransform->GetHairTranslate();
        pModel->faceCenterPos   = pTransform->GetNoseTranslate();

        pModel->partsTransform.hatTranslate = pTransform->GetHairTranslate();
    }

    rio::MemUtil::copy(pBoundingBox, FFLiGetResourceShapeElement(&size, pData, partsType, FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_BOUNDING_BOX, layoutType), sizeof(FFLBoundingBox));

#ifdef FFL_USE_ADJUST_MTX
    pDrawParam->primitiveParam.pAdjustMatrix =
        reinterpret_cast<rio::Matrix34f*>(static_cast<u8*>(pData) + size);
#endif // FFL_USE_ADJUST_MTX

    return FFL_RESULT_OK;
}

void FFLiDeleteShape(void** ppShapeData, FFLDrawParam* pDrawParam)
{
    void*& pData = *ppShapeData;
    if (pData != nullptr)
    {
        rio::MemUtil::free(pData);
        pData = nullptr;

        rio::MemUtil::set(pDrawParam, 0, sizeof(FFLDrawParam));
    }
}

void FFLiAdjustShape(FFLDrawParam* pDrawParam, FFLBoundingBox* pBoundingBox, f32 scaleX, f32 scaleY, const FFLVec3* pTranslate, bool flipX, const FFLiCoordinate* pCoordinate, FFLiShapePartsType partsType, bool limitNoseScaleZ)
{
    f32 scaleZ = (scaleX + scaleY) * 0.5f;
    if (limitNoseScaleZ &&
        (partsType == FFLI_SHAPE_PARTS_TYPE_NOSE || partsType == FFLI_SHAPE_PARTS_TYPE_NOSELINE) &&
        scaleZ > 1.1f)
    {
        scaleZ = 1.1f;
    }

#ifdef FFL_USE_ADJUST_MTX
    // Skip for an empty shape
    if (pDrawParam->primitiveParam.pAdjustMatrix == NULL)
        return;
    // pAdjustMatrix is already allocated, let's assign to it
    rio::Vector3f translate = { 0.0f, 0.0f, 0.0f };
    // Set translation, if present.
    if (pTranslate != NULL)
        rio::MemUtil::copy(&translate, pTranslate, sizeof(translate));

    // Create scale/translate matrix.
    pDrawParam->primitiveParam.pAdjustMatrix->makeST({
        flipX ? -scaleX : scaleX,
        scaleY, scaleZ
    }, translate);

    // NOTE: does not use FFLiCoordinate, no scale/swizzle/flip
    // bounding box expected to be maintained by caller

#else

    AdjustAttribute<FFLVec4>(
        static_cast<FFLVec4*>(pDrawParam->attributeBufferParam.attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_POSITION].ptr),
        pDrawParam->attributeBufferParam.attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_POSITION].size / sizeof(FFLVec4),
        scaleX, scaleY, scaleZ,
        pTranslate,
        flipX,
        pCoordinate
    );


    if (g_NormalIsSnorm8_8_8_8)
    {
        AdjustAttributeWithoutScale<FFLiSnorm8_8_8_8>(
            static_cast<FFLiSnorm8_8_8_8*>(pDrawParam->attributeBufferParam.attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_NORMAL].ptr),
            pDrawParam->attributeBufferParam.attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_NORMAL].size / sizeof(FFLiSnorm8_8_8_8),
            flipX,
            pCoordinate
        );
    }
    else
    {
        AdjustAttributeWithoutScale<FFLiSnorm10_10_10_2>(
            static_cast<FFLiSnorm10_10_10_2*>(pDrawParam->attributeBufferParam.attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_NORMAL].ptr),
            pDrawParam->attributeBufferParam.attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_NORMAL].size / sizeof(FFLiSnorm10_10_10_2),
            flipX,
            pCoordinate
        );
    }


    AdjustAttributeWithoutScale<FFLiSnorm8_8_8_8>(
        static_cast<FFLiSnorm8_8_8_8*>(pDrawParam->attributeBufferParam.attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_TANGENT].ptr),
        pDrawParam->attributeBufferParam.attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_TANGENT].size / sizeof(FFLiSnorm8_8_8_8),
        flipX,
        pCoordinate
    );

    if (!IsNaN(pBoundingBox->min.x))
        AdjustAttribute<FFLVec3>(
            &pBoundingBox->min,
            2,
            scaleX, scaleY, scaleZ,
            pTranslate,
            flipX,
            pCoordinate
        );

    if (flipX && !g_FrontCullForFlipX)
        AdjustIndexBuffer(
            pDrawParam->primitiveParam.pIndexBuffer,
            pDrawParam->primitiveParam.indexCount
        );

/*
    for (u32 i = 0; i < FFL_ATTRIBUTE_BUFFER_TYPE_MAX; i++)
    {
        FFLAttributeBuffer& attribute = pDrawParam->attributeBufferParam.attributeBuffers[i];
        if (attribute.ptr != NULL)
            EndianSwap(attribute.ptr, attribute.size);
    }

    FFLPrimitiveParam& primitive = pDrawParam->primitiveParam;
    if (primitive.pIndexBuffer != NULL)
        EndianSwap(
            primitive.pIndexBuffer,
            sizeof(u16) * primitive.indexCount  // Apparently Nintendo forgot the index count is 4
        );
*/

#endif // FFL_USE_ADJUST_MTX

}

void FFLiInvalidateShape(FFLDrawParam* pDrawParam)
{
#if RIO_IS_CAFE
    for (u32 i = 0; i < FFL_ATTRIBUTE_BUFFER_TYPE_MAX; i++)
    {
        FFLAttributeBuffer& attribute = pDrawParam->attributeBufferParam.attributeBuffers[i];
        if (attribute.ptr != NULL)
            GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, attribute.ptr, attribute.size);
    }

    FFLPrimitiveParam& primitive = pDrawParam->primitiveParam;
    if (primitive.pIndexBuffer != NULL)
        GX2Invalidate(
            GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER,
            primitive.pIndexBuffer,
            sizeof(u16) * primitive.indexCount  // Apparently Nintendo forgot the index count is 4
        );
#endif // RIO_IS_CAFE
}

namespace {

FFLiResourceShapeElementType GetElementType(FFLAttributeBufferType type)
{
    switch (type)
    {
    case FFL_ATTRIBUTE_BUFFER_TYPE_POSITION:
        return FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_POSITION;
    case FFL_ATTRIBUTE_BUFFER_TYPE_TEXCOORD:
        return FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TEXCOORD;
    case FFL_ATTRIBUTE_BUFFER_TYPE_NORMAL:
        return FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_NORMAL;
    case FFL_ATTRIBUTE_BUFFER_TYPE_TANGENT:
        return FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TANGENT;
    case FFL_ATTRIBUTE_BUFFER_TYPE_COLOR:
        return FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_COLOR;
    default:
        return FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_POSITION;
    }
}

static const rio::VertexStream::Format VERTEX_FORMATS[FFLI_VERTEX_LAYOUT_TYPE_MAX][FFL_ATTRIBUTE_BUFFER_TYPE_MAX] =
{
    // Default FFL formats
    {
        rio::VertexStream::FORMAT_32_32_32_FLOAT,   // POSITION
        rio::VertexStream::FORMAT_32_32_FLOAT,      // TEXCOORD
        rio::VertexStream::FORMAT_10_10_10_2_SNORM, // NORMAL
        rio::VertexStream::FORMAT_8_8_8_8_SNORM,    // TANGENT
        rio::VertexStream::FORMAT_8_8_8_8_UNORM     // COLOR
    },
    {
        rio::VertexStream::FORMAT_16_16_16_16_FLOAT, // POSITION
        rio::VertexStream::FORMAT_16_16_FLOAT,       // TEXCOORD
        rio::VertexStream::FORMAT_8_8_8_8_SNORM,     // NORMAL
        rio::VertexStream::FORMAT_8_8_8_8_SNORM,     // TANGENT
        rio::VertexStream::FORMAT_8_8_8_8_UNORM      // COLOR
    }
};

u32 FormatToStride(rio::VertexStream::Format format)
{
    switch (format)
    {
    case rio::VertexStream::FORMAT_32_32_FLOAT:
        return 8;
    case rio::VertexStream::FORMAT_32_32_32_FLOAT:
    case rio::VertexStream::FORMAT_32_32_32_32_FLOAT:
        // Four extra bytes for padding.
        return 16;
    case rio::VertexStream::FORMAT_8_8_8_8_UNORM:
    case rio::VertexStream::FORMAT_8_8_8_8_SNORM:
    case rio::VertexStream::FORMAT_10_10_10_2_SNORM:
        return 4;

    case rio::VertexStream::FORMAT_16_16_16_16_FLOAT:
        return 6;//8; // HACK: actually 16_16_16
    case rio::VertexStream::FORMAT_16_16_FLOAT:
        return 4;
    default:
        return 0;
    }
}

u32 GetStride(FFLAttributeBufferType type, u32 size, FFLiVertexLayoutType layoutType)
{
    u32 stride;

    RIO_ASSERT(type < FFL_ATTRIBUTE_BUFFER_TYPE_MAX);
    stride = FormatToStride(VERTEX_FORMATS[layoutType][type]);

    if (stride >= size)
        stride = 0;

    return stride;
}

// NOTE: This does literally nothing
/*
void EndianSwap(void* ptr, u32 size)
{
    FFLiBugEndianSwap(ptr, size);
}
*/

template <typename T>
void AdjustAttribute(T* pVec, u32 num, f32 scaleX, f32 scaleY, f32 scaleZ, const FFLVec3* pTranslate, bool flipX, const FFLiCoordinate* pCoordinate)
{
    bool isDefault = pCoordinate->IsDefault();

    for (u32 i = 0; i < num; i++)
    {
        if (flipX)
            pVec->x = FFLiCoordinate::FlipValue(pVec, pVec->x);

        if (scaleX != 1.0f)
            pVec->x *= scaleX;

        if (scaleY != 1.0f)
            pVec->y *= scaleY;

        if (scaleZ != 1.0f)
            pVec->z *= scaleZ;

        if (pTranslate != NULL)
        {
            pVec->x += pTranslate->x;
            pVec->y += pTranslate->y;
            pVec->z += pTranslate->z;
        }

        if (!isDefault)
            pCoordinate->Transform(pVec);

        pVec++;
    }
}

template <typename T>
void AdjustAttributeWithoutScale(T* pVec, u32 num, bool flipX, const FFLiCoordinate* pCoordinate)
{
    bool isDefault = pCoordinate->IsDefault();

    for (u32 i = 0; i < num; i++)
    {
        T& vec = pVec[i];

        if (flipX)
            vec.x = FFLiCoordinate::FlipValue(&vec, vec.x);

        if (!isDefault)
            pCoordinate->TransformWithoutScale(&vec);
    }
}

// Reverses winding, should be called when flipping X
// as an alternative to using front face culling.
[[maybe_unused]] void AdjustIndexBuffer(void* pIndexPtr, u32 indexCount)
{
    u16* pIndexBuffer = static_cast<u16*>(pIndexPtr);
    u32 halfCount = indexCount / 2;

    for (u32 i = 0; i < halfCount; ++i)
    {
        u16 curIdx = pIndexBuffer[i];
        pIndexBuffer[i] = pIndexBuffer[indexCount - 1 - i];
        pIndexBuffer[indexCount - 1 - i] = curIdx;
    }
}

}
