#include <nn/ffl/FFLVec.h>

#include <nn/ffl/FFLiCoordinate.h>

FFLiCoordinate::FFLiCoordinate()
    : m_SwizzleY(FFLI_AXIS_TYPE_Y)
    , m_SwizzleZ(FFLI_AXIS_TYPE_Z)
    , m_SwizzleX(FFLI_AXIS_TYPE_X)
    , m_FlipY(false)
    , m_FlipZ(false)
    , m_FlipX(false)
    , m_Scale(1.0f)
{
}

FFLiCoordinate::~FFLiCoordinate()
{
}

namespace
{

FFLCoordinateType GetRightCoordinateType(FFLCoordinateType upType, FFLCoordinateType frontType)
{
    // First 3 bytes/24 bits:
    s32 up24    = (upType    << 0x10) >> 0x18;
    s32 front24 = (frontType << 0x10) >> 0x18;
    // First 2 bytes/16 bits:
    s32 up16    = (upType    << 8)    >> 0x18;
    s32 front16 = (frontType << 8)    >> 0x18;

    u32 upCrossFrontX = (up24 * front16 - up16 * front24);
    u32 upCrossFrontY = (up16 * frontType - upType * front16) * 0x100;                // << 8
    u32 upFrontCross = ((upType * front24 - up24 * frontType) * 0x10000 & 0xff0000) | // << 0x10, idk what the and is
        (upCrossFrontY & 0xff00) | (upCrossFrontX & 0xff);

    // in nn::mii::CoordinateType __cdecl nn::mii::detail::Coordinate::GetRightCoordinateType(enum nn::mii::CoordinateType,enum nn::mii::CoordinateType):

    //u32 result = (front24 * up24 - frontHigh * up16) & FFL_COORDINATE_TYPE_X_MINUS |
    //   (frontType * up16 - front24 * upType & 0xff) << 8 |
    //   (frontHigh * upType - frontType * up24 & 0xff) << 0x10;

    return static_cast<FFLCoordinateType>(upFrontCross);
}

FFLiAxisType GetAxisType(FFLCoordinateType type)
{
    FFLiAxisType axisType = FFLI_AXIS_TYPE_X;

    if ((type & FFL_COORDINATE_TYPE_X_PLUS) == 0)
    {
        if ((type & FFL_COORDINATE_TYPE_DEFAULT_UP) == 0)
        {
            if ((type & FFL_COORDINATE_TYPE_DEFAULT_FRONT) == 0)
                RIO_ASSERT(false);
                // Should not reach here.
            else
                axisType = FFLI_AXIS_TYPE_Z;
        }
        else
            axisType = FFLI_AXIS_TYPE_Y;
    }
    else
        axisType = FFLI_AXIS_TYPE_X;

    return axisType;
}

bool GetAxisInv(FFLCoordinateType type)
{
    return (type & 0x101010) != 0;
}

}

void FFLiCoordinate::Set(FFLCoordinateType upType, FFLCoordinateType frontType)
{
    RIO_ASSERT((upType & frontType) == 0);
    // up and front are not orthogonal(up = %x,front = %x).

    // in nn::mii::detail::Coordinate::Coordinate(enum nn::mii::CoordinateType,enum nn::mii::CoordinateType,float):
    FFLCoordinateType rightType =
        GetRightCoordinateType(upType, frontType);

    this->m_SwizzleX = GetAxisType(rightType);
    this->m_SwizzleY = GetAxisType(upType);
    this->m_SwizzleZ = GetAxisType(frontType);
    this->m_FlipX = GetAxisInv(rightType);
    this->m_FlipY = GetAxisInv(upType);
    this->m_FlipZ = GetAxisInv(frontType);
}

void FFLiCoordinate::Transform(FFLVec3* pVec) const
{
    TransformCommon<FFLVec3, f32>(pVec);
    pVec->x *= m_Scale;
    pVec->y *= m_Scale;
    pVec->z *= m_Scale;
}

void FFLiCoordinate::Transform(FFLVec4* pVec) const
{
    Transform((FFLVec3*)pVec);
}

f32 FFLiCoordinate::FlipValue(FFLVec3*, f32 value)
{
    return -value;
};

f32 FFLiCoordinate::FlipValue(FFLVec4*, f32 value)
{
    return -value;
};

void FFLiCoordinate::TransformWithoutScale(FFLVec3* pVec) const
{
    TransformCommon<FFLVec3, f32>(pVec);
}

void FFLiCoordinate::TransformWithoutScale(FFLVec4* pVec) const
{
    TransformWithoutScale((FFLVec3*)pVec);
}

void FFLiCoordinate::TransformWithoutScale(FFLiSnorm10_10_10_2* pVec) const
{
    TransformCommon<FFLiSnorm10_10_10_2, u32>(pVec);
}

void FFLiCoordinate::TransformWithoutScale(FFLiSnorm8_8_8_8* pVec) const
{
    TransformCommon<FFLiSnorm8_8_8_8, s8>(pVec);
}

u32 FFLiCoordinate::FlipValue(FFLiSnorm10_10_10_2* pVec, u32 value)
{
    if (value == 0x200)
        return 0x1FF;

    else if (value == 0x1FF)
        return 0x200;

    else
        return (0x400 - value) & 0x3ff;

}

s8 FFLiCoordinate::FlipValue(FFLiSnorm8_8_8_8* pVec, s8 value)
{
    if (value == -128)
        value = 127;

    return -value;
}

bool FFLiCoordinate::IsDefault() const
{
    if (m_FlipY)
        return false;

    if (m_FlipZ)
        return false;

    if (m_FlipX)
        return false;

    if (m_SwizzleY != FFLI_AXIS_TYPE_Y)
        return false;

    if (m_SwizzleZ != FFLI_AXIS_TYPE_Z)
        return false;

    if (m_SwizzleX != FFLI_AXIS_TYPE_X)
        return false;

    if (m_Scale != 1.0f)
        return false;

    return true;
}

void FFLiCoordinate::SetScale(f32 scale)
{
    m_Scale = scale;
};
