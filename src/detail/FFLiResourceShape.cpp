#include <nn/ffl/FFLiSwapEndian.h>

#include <nn/ffl/detail/FFLiResourceShape.h>

namespace {

const void* GetElement(u32* pSize, const void* pShapeData, u32 pos, u32 size);

void SwapEndianAttribute(void* pShapeData, u32 pos, u32 size);
void SwapEndianIndex(void* pShapeData, u32 pos, u32 size);
void SwapEndianVec3(FFLVec3* pVec);
void SwapEndianHairTransform(void* pTransformData);
void SwapEndianFacelineTransform(void* pTransformData);

}

// Controls whether the normal attribute should be converted in-place
// to FFLiSnorm8_8_8_8 format. Needed for APIs not supporting
// 10_10_10_2 vertex format, including: GL(ES) 2.0, Veldrid?...
bool g_NormalIsSnorm8_8_8_8 = false;

void FFLiSetNormalIsSnorm8_8_8_8(bool enable)
{
    g_NormalIsSnorm8_8_8_8 = enable;
}

const void* FFLiGetResourceShapeElement(u32* pSize, const void* pShapeData, FFLiShapePartsType partsType, FFLiResourceShapeElementType elementType)
{
    const FFLiResourceShapeDataHeader* pShape = (const FFLiResourceShapeDataHeader*)pShapeData;

    switch (elementType)
    {
    case FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_POSITION:
        return GetElement(pSize, pShape, pShape->GetElementPos(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_POSITION), pShape->GetElementSize(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_POSITION));
    case FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_NORMAL:
    {
        if (g_NormalIsSnorm8_8_8_8)
        {
            // convert FFLiSnorm10_10_10_2 to FFLiSnorm8_8_8_8
            // note that this is done in AFLiGetResourceShapeElement
            *pSize = pShape->GetElementSize(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_NORMAL);

            if (*pSize == 0) // skip empty elements
                return NULL;
            u32* pElement = const_cast<u32*>(static_cast<const u32*>(GetElement(pSize, pShape, pShape->GetElementPos(FFLI_RESOURCE_SHAPE_ELEMENT_TYPE_NORMAL), *pSize)));
            if (*pSize >> 2 == 0)
                return pElement; // skip if there are less than 4 bytes

            for (u32 i = 0; i < (*pSize >> 2); i++) // for each u32
            {
                u32 curNormal = pElement[i];
                u32 topTwo = curNormal >> 30;  // top two bits (bits 30-31)

                // should be equivalent to AFLiGetResourceShapeElement
                pElement[i] =
                    // Extract and rearrange bits 8_8_8_8 format
                    ((curNormal >> 4) & 0xFF00) |         // Extract bits 4-11 and shift them to 8-15
                    ((curNormal >> 6) & 0xFF0000) |       // Extract bits 6-13 and shift them to 16-23
                    (((curNormal << 22) >> 24) & 0xFF) |  // Extract bits 0-7 (shifted to 0-7)
                    (((topTwo << 2 | curNormal >> 30 |    // Combine the two highest bits (30-31) and shift them
                        topTwo << 4 | topTwo << 6)        // with some additional shifting for alpha
                        << 24));                          // Shift it to the most significant byte (24-31)
            }
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
    SwapEndianVec3(&(_0[0]));
    SwapEndianVec3(&(_0[1]));
    SwapEndianVec3(&(_0[2]));
    SwapEndianVec3(&(_0[3]));
    SwapEndianVec3(&(_0[4]));
    SwapEndianVec3(&(_0[5]));
}

void FFLiResourceShapeFacelineTransform::SwapEndian()
{
    SwapEndianVec3(&m_HairPos);
    SwapEndianVec3(&m_FaceCenterPos);
    SwapEndianVec3(&m_BeardPos);
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

}
