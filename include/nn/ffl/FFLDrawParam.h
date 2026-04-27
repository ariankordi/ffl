#ifndef FFL_DRAW_PARAM_H_
#define FFL_DRAW_PARAM_H_

#include <nn/ffl/FFLModulateParam.h>

#if FFL_USE_RIO
    #include <gpu/rio_Drawer.h>
    #if FFL_USE_ADJUST_MTX
        #include <math/rio_Matrix.h>
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum FFLAttributeBufferType
{
    FFL_ATTRIBUTE_BUFFER_TYPE_POSITION  = 0,
    FFL_ATTRIBUTE_BUFFER_TYPE_TEXCOORD  = 1,
    FFL_ATTRIBUTE_BUFFER_TYPE_NORMAL    = 2,
    FFL_ATTRIBUTE_BUFFER_TYPE_TANGENT   = 3,
    FFL_ATTRIBUTE_BUFFER_TYPE_COLOR     = 4,
    FFL_ATTRIBUTE_BUFFER_TYPE_MAX       = 5
}
FFLAttributeBufferType;

typedef struct FFLAttributeBuffer
{
    u32     size;
    u32     stride;
    void*   ptr;
}
FFLAttributeBuffer;
NN_STATIC_ASSERT32(sizeof(FFLAttributeBuffer) == 0xC);

typedef struct FFLAttributeBufferParam
{
    FFLAttributeBuffer  attributeBuffers[FFL_ATTRIBUTE_BUFFER_TYPE_MAX];
}
FFLAttributeBufferParam;
NN_STATIC_ASSERT32(sizeof(FFLAttributeBufferParam) == 0x3C);

typedef struct FFLPrimitiveParam
{
#if FFL_USE_RIO
    // Shapes are triangles, but for faceline/mask
    // texture 2D planes it is triangle strip
    rio::Drawer::PrimitiveMode  primitiveType;
#else
    unsigned int                primitiveType;
#endif // endif

    u32                         indexCount;

#ifdef FFL_USE_ADJUST_MTX
    #if FFL_USE_RIO
    rio::Matrix34f*             pAdjustMatrix;
    #else
    void*                       pAdjustMatrix;
    #endif
#else
    u32                         _8; // Deleted
#endif // FFL_USE_ADJUST_MTX

    void*                       pIndexBuffer;
}
FFLPrimitiveParam;
NN_STATIC_ASSERT32(sizeof(FFLPrimitiveParam) == 0x10);

typedef enum FFLCullMode
{
    FFL_CULL_MODE_NONE  = 0,
    FFL_CULL_MODE_BACK  = 1,
    FFL_CULL_MODE_FRONT = 2,
    FFL_CULL_MODE_MAX   = 3
}
FFLCullMode;

typedef struct FFLDrawParam
{
    FFLAttributeBufferParam attributeBufferParam;
    FFLModulateParam        modulateParam;
    FFLCullMode             cullMode;
    FFLPrimitiveParam       primitiveParam;
}
FFLDrawParam;
NN_STATIC_ASSERT32(sizeof(FFLDrawParam) == 0x68);

#ifdef __cplusplus
}
#endif

#endif // FFL_DRAW_PARAM_H_
