#include <nn/ffl/detail/FFLiBufferAllocator.h>
#include <nn/ffl/detail/FFLiCopySurfaceDrawer.h>

#include <cafe/gx2.h>

#include <cstring>

FFLiCopySurfaceDrawer::FFLiCopySurfaceDrawer()
    : m_pPositionBuffer(NULL)
    , m_pTexCoordBuffer(NULL)
{
}

FFLiCopySurfaceDrawer::~FFLiCopySurfaceDrawer()
{
}

void FFLiCopySurfaceDrawer::SetupCPU(FFLiBufferAllocator* pAllocator)
{
    const u32 POSITION_BUFFER_SIZE = sizeof(FFLVec2) * 4;
    const u32 TEXCOORD_BUFFER_SIZE = sizeof(FFLVec2) * 4;

    static const FFLVec2 POSITION_BUFFER[4] = {
        { -1.0f,  1.0f },
        { -1.0f, -1.0f },
        {  1.0f,  1.0f },
        {  1.0f, -1.0f }
    };
    NN_STATIC_ASSERT(sizeof(POSITION_BUFFER) == POSITION_BUFFER_SIZE);

    static const FFLVec2 TEXCOORD_BUFFER[4] = {
        { 0.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 0.0f },
        { 1.0f, 1.0f }
    };
    NN_STATIC_ASSERT(sizeof(TEXCOORD_BUFFER) == TEXCOORD_BUFFER_SIZE);

    m_pPositionBuffer = static_cast<FFLVec2*>(pAllocator->Allocate(POSITION_BUFFER_SIZE, GX2_VERTEX_BUFFER_ALIGNMENT));
    m_pTexCoordBuffer = static_cast<FFLVec2*>(pAllocator->Allocate(TEXCOORD_BUFFER_SIZE, GX2_VERTEX_BUFFER_ALIGNMENT));

    std::memcpy(m_pPositionBuffer, POSITION_BUFFER, POSITION_BUFFER_SIZE);
    std::memcpy(m_pTexCoordBuffer, TEXCOORD_BUFFER, TEXCOORD_BUFFER_SIZE);
}

void FFLiCopySurfaceDrawer::SetupGPU()
{
    const u32 POSITION_BUFFER_SIZE = sizeof(FFLVec2) * 4;
    const u32 TEXCOORD_BUFFER_SIZE = sizeof(FFLVec2) * 4;

    GX2Invalidate(GX2_INVALIDATE_CPU_ATTRIB_BUFFER, m_pPositionBuffer, POSITION_BUFFER_SIZE);
    GX2Invalidate(GX2_INVALIDATE_CPU_ATTRIB_BUFFER, m_pTexCoordBuffer, TEXCOORD_BUFFER_SIZE);
}

void FFLiCopySurfaceDrawer::SetAttributeBuffer(u32 positionBufferIndex, u32 texCoordBufferIndex)
{
    const u32 POSITION_BUFFER_SIZE = sizeof(FFLVec2) * 4;
    const u32 TEXCOORD_BUFFER_SIZE = sizeof(FFLVec2) * 4;

    GX2SetAttribBuffer(positionBufferIndex, POSITION_BUFFER_SIZE, sizeof(FFLVec2), m_pPositionBuffer);
    GX2SetAttribBuffer(texCoordBufferIndex, TEXCOORD_BUFFER_SIZE, sizeof(FFLVec2), m_pTexCoordBuffer);
}

void FFLiCopySurfaceDrawer::Draw()
{
    GX2Draw(GX2_PRIMITIVE_TRIANGLE_STRIP, 4);
}

u32 FFLiCopySurfaceDrawer::GetBufferSize()
{
    const u32 POSITION_BUFFER_SIZE = sizeof(FFLVec2) * 4;
    const u32 TEXCOORD_BUFFER_SIZE = sizeof(FFLVec2) * 4;

    u32 ret  = GX2_VERTEX_BUFFER_ALIGNMENT;
    ret     += POSITION_BUFFER_SIZE;

    ret     += GX2_VERTEX_BUFFER_ALIGNMENT;
    ret     += TEXCOORD_BUFFER_SIZE;

    return ret;
}
