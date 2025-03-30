#include <nn/ffl/FFLiSwapEndian.h>

#include <nn/ffl/detail/FFLiResourceShape.h>

namespace {

const void* GetElement(u32* pSize, const void* pShapeData, u32 pos, u32 size);

void SwapEndianAttribute(void* pShapeData, u32 pos, u32 size);
void SwapEndianIndex(void* pShapeData, u32 pos, u32 size);
void SwapEndianVec3(FFLVec3* pVec);
void SwapEndianHairTransform(void* pTransformData);
void SwapEndianFacelineTransform(void* pTransformData);

void ConvertNormalTo8_8_8_8(u32* pData); // custom

}

// Controls whether the normal attribute should be converted in-place
// to FFLiSnorm8_8_8_8 format. Needed for APIs not supporting
// 10_10_10_2 vertex format, including: GL(ES) 2.0, Veldrid?...
bool g_NormalIsSnorm8_8_8_8 = false;

void FFLiSetNormalIsSnorm8_8_8_8(bool enable)
{
    g_NormalIsSnorm8_8_8_8 = enable;
}

const void* FFLiGetResourceShapeElement(u32* pSize, const void* pShapeData, FFLiShapePartsType partsType, FFLiResourceShapeElementType elementType, FFLiVertexLayoutType layoutType)
{
    const FFLiResourceShapeDataHeader* pShape = (const FFLiResourceShapeDataHeader*)pShapeData;

    switch (elementType)
    {
    case FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_POSITION:
        return GetElement(pSize, pShape, pShape->GetElementPos(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_POSITION), pShape->GetElementSize(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_POSITION));
    case FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_NORMAL:
    {
        if (g_NormalIsSnorm8_8_8_8 && layoutType != FFLI_VERTEX_LAYOUT_TYPE_HALF_FLOAT_NORMAL_8888)
        {
            // convert FFLiSnorm10_10_10_2 to FFLiSnorm8_8_8_8
            // note that this is done in AFLiGetResourceShapeElement
            *pSize = pShape->GetElementSize(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_NORMAL);

            if (*pSize == 0) // skip empty elements
                return NULL;
            u32* pElement = const_cast<u32*>(static_cast<const u32*>(GetElement(pSize, pShape, pShape->GetElementPos(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_NORMAL), *pSize)));
            if (*pSize >> 2 == 0)
                return pElement; // skip if there are less than 4 bytes

            for (u32 i = 0; i < (*pSize >> 2); i++)
                ConvertNormalTo8_8_8_8(&pElement[i]);

            return pElement;
        }
        return GetElement(pSize, pShape, pShape->GetElementPos(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_NORMAL), pShape->GetElementSize(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_NORMAL));
    }
    case FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TEXCOORD:
        return GetElement(pSize, pShape, pShape->GetElementPos(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TEXCOORD), pShape->GetElementSize(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TEXCOORD));
    case FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TANGENT:
        return GetElement(pSize, pShape, pShape->GetElementPos(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TANGENT), pShape->GetElementSize(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TANGENT));
    case FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_COLOR:
        return GetElement(pSize, pShape, pShape->GetElementPos(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_COLOR), pShape->GetElementSize(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_COLOR));
    case FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_INDEX:
        return GetElement(pSize, pShape, pShape->GetElementPos(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_INDEX), pShape->GetElementSize(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_INDEX));
    case FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TRANSFORM_HAIR:
        if (partsType == FFLI_SHAPE_PARTS_TYPE_HAIR_NORMAL)
        {
            *pSize = sizeof(FFLiResourceShapeHairTransform);
            return pShape->GetTransform();
        }
        break;
    case FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TRANSFORM_FACELINE:
        if (partsType == FFLI_SHAPE_PARTS_TYPE_FACELINE)
        {
            *pSize = sizeof(FFLiResourceShapeFacelineTransform);
            return pShape->GetTransform();
        }
        break;
    case FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_BOUNDING_BOX:
        *pSize = sizeof(FFLBoundingBox);
        return &pShape->GetBoundingBox();
    }

    *pSize = 0;
    return NULL;
}

void FFLiSwapEndianResourceShapeElement(void* pShapeData, bool save, FFLiShapePartsType partsType)
{
    // This function is deleted in NSMBU.
    // Therefore, its implementation is only theoretical.

    // In AFL, Second argument = u32 shapeAlignedMaxSize (still unused)

    FFLiResourceShapeDataHeader* pShape = static_cast<FFLiResourceShapeDataHeader*>(pShapeData);

    // save argument is not in FFL
    if (!save)
        pShape->SwapEndian(); // FFL calls this first unconditionally

    SwapEndianAttribute(pShape, pShape->GetElementPos(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_POSITION), pShape->GetElementSize(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_POSITION));
    SwapEndianAttribute(pShape, pShape->GetElementPos(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_NORMAL), pShape->GetElementSize(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_NORMAL));
    SwapEndianAttribute(pShape, pShape->GetElementPos(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TEXCOORD), pShape->GetElementSize(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_TEXCOORD));
    // Tangent and color (8 bit) don't need endian swap
    // (FFL calls the func to swap them anyway)

    SwapEndianIndex(pShape, pShape->GetElementPos(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_INDEX), pShape->GetElementSize(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_INDEX));

    if (partsType == FFLI_SHAPE_PARTS_TYPE_HAIR_NORMAL)
        SwapEndianHairTransform(pShape->GetTransform());

    else if (partsType == FFLI_SHAPE_PARTS_TYPE_FACELINE)
        SwapEndianFacelineTransform(pShape->GetTransform());

    if (save)
        pShape->SwapEndian();
}

void FFLiResourceShapeDataHeader::SwapEndian()
{
    FFLiSwapEndianArrayImpl<u32>(m_ElementPos, FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_BUFFER_MAX);
    FFLiSwapEndianArrayImpl<u32>(m_ElementSize, FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_BUFFER_MAX);
    // real function calls FFLiSwapEndianImpl<f32> x 6
    SwapEndianVec3(&m_BoundingBox.min);
    SwapEndianVec3(&m_BoundingBox.max);
}

void FFLiResourceShapeHairTransform::SwapEndian()
{
    SwapEndianVec3(&m_FrontTranslate);
    SwapEndianVec3(&m_FrontRotate);
    SwapEndianVec3(&m_SideTranslate);
    SwapEndianVec3(&m_SideRotate);
    SwapEndianVec3(&m_TopTranslate);
    SwapEndianVec3(&m_TopRotate);
}

void FFLiResourceShapeFacelineTransform::SwapEndian()
{
    SwapEndianVec3(&m_HairTranslate);
    SwapEndianVec3(&m_NoseTranslate);
    SwapEndianVec3(&m_BeardTranslate);
}

namespace {

const void* GetElement(u32* pSize, const void* pShapeData, u32 pos, u32 size)
{
    *pSize = size;
    return size != 0 ? ((const u8*)pShapeData + pos) : NULL;
}

// These functions are all deleted in NSMBU.
// Therefore, their implementation is only theoretical.

void SwapEndianAttribute(void* pShapeData, u32 pos, u32 size)
{
    if (size == 0)
        return;
    RIO_ASSERT((size % sizeof(float)) == 0);
    // real function calls FFLiSwapEndianImpl<f32>

    FFLiSwapEndianArrayImpl<u32>((u32*)((const u8*)pShapeData + pos), size / sizeof(u32));
}

void SwapEndianIndex(void* pShapeData, u32 pos, u32 size)
{
    if (size == 0)
        return;

    FFLiSwapEndianArrayImpl<u16>((u16*)((const u8*)pShapeData + pos), size);
}

void SwapEndianVec3(FFLVec3* pVec)
{
    // real function calls FFLiSwapEndianImpl<f32> three times
    FFLiSwapEndianArrayImpl<f32>(&pVec->x, sizeof(FFLVec3) / sizeof(f32));
}

void SwapEndianHairTransform(void* pTransformData)
{
    static_cast<FFLiResourceShapeHairTransform*>(pTransformData)->SwapEndian();
}

void SwapEndianFacelineTransform(void* pTransformData)
{
    static_cast<FFLiResourceShapeFacelineTransform*>(pTransformData)->SwapEndian();
}

void ConvertNormalTo8_8_8_8(u32* pData)
{
    u32 val = *pData;

    // Extract the 10-bit fields (Red, Green, Blue) and the 2-bit alpha.
    int r = (int)((val >> 20) & 0x3FF); // red: bits 20-29
    int g = (int)((val >> 10) & 0x3FF); // green: bits 10-19
    int b = (int)(val & 0x3FF);         // blue: bits 0-9
    int a = (int)((val >> 30) & 0x3);     // alpha: bits 30-31

    // Sign-extend the 10-bit values (if bit 9 is set, fill upper bits with 1's)
    if (r & 0x200) r |= ~0x3FF;
    if (g & 0x200) g |= ~0x3FF;
    if (b & 0x200) b |= ~0x3FF;

    // Convert the 10-bit SNORM range (-512..511) to 8-bit SNORM (-128..127)
    r = (r * 127) / 511;
    g = (g * 127) / 511;
    b = (b * 127) / 511;

    // Convert the 2-bit SNORM alpha (-2..1) to 8-bit (0..255).
    // Mapping: -2 -> 0, -1 -> 85, 0 -> 170, 1 -> 255.
    a = ((a + 2) * 85);

    // Pack the channels into an 8_8_8_8 integer.
    *pData = ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}

}
