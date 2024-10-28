#ifndef FFL_TYPES_H_
#define FFL_TYPES_H_

// Hard-coded for now
#if !defined(__BYTE_ORDER__) || !defined(__ORDER_LITTLE_ENDIAN__) || !defined(__ORDER_BIG_ENDIAN__)
    // Define these in MSVC
    #ifdef _MSC_VER
        // NOTE: assuming little endian
        // ... MSVC only targets x86, ARM anyway, all little endian
        #define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
        #define __ORDER_LITTLE_ENDIAN__ 1234
        #define __ORDER_BIG_ENDIAN__ 4321
    #else
        #error "Need __BYTE_ORDER__, __ORDER_LITTLE_ENDIAN__ and __ORDER_BIG_ENDIAN__ to be defined"
    #endif
#endif

#if (__BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ && __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__)
    #error "Host must be either big- or little-endian"
#endif


#ifdef __cplusplus
    #include <type_traits>

    #define NN_STATIC_ASSERT_IS_POD(T)  static_assert(std::is_trivial<T>::value)

    #ifdef NDEBUG
        #define NN_STATIC_ASSERT(condition) static_assert(true, "")
    #else
        #define NN_STATIC_ASSERT static_assert
    #endif

#else // __cplusplus
    #include <assert.h>

    #ifdef NDEBUG
        #define NN_STATIC_ASSERT(condition) _Static_assert(true, "")
    #else
        #define NN_STATIC_ASSERT _Static_assert
    #endif

    #define NN_STATIC_ASSERT_IS_POD(T)  ((void)0)
#endif

// NN_STATIC_ASSERT32 = NN_STATIC_ASSERT but only applies for 32 bit
#ifdef __cplusplus
    #include <type_traits>

    #if defined(NDEBUG) || (!defined(__WUT__) && INTPTR_MAX == INT64_MAX)
        #define NN_STATIC_ASSERT32(condition) static_assert(true, "")
    #else
        #define NN_STATIC_ASSERT32 NN_STATIC_ASSERT
    #endif
#else // __cplusplus
    #include <assert.h>

    #if defined(NDEBUG) || INTPTR_MAX == INT64_MAX
        #define NN_STATIC_ASSERT32(condition) _Static_assert(true, "")
    #else
        #define NN_STATIC_ASSERT32 NN_STATIC_ASSERT
    #endif
#endif


#ifdef __cplusplus
    #include <misc/rio_Types.h>
    // Define typedefs that are exported in public headers
    #include <gpu/rio_Texture.h>
    typedef rio::Texture2D FFLRIOTexture2D;
    #include <math/rio_Matrix.h>
    typedef rio::BaseMtx44f FFLRIOBaseMtx44f;
    #include <gfx/rio_Graphics.h>
    typedef rio::Graphics::CompareFunc FFLRIOCompareFunc;
    #include <gpu/rio_Drawer.h>
    typedef rio::Drawer::PrimitiveMode FFLRIOPrimitiveMode;

    #define FFL_GET_RIO_NATIVE_TEXTURE_HANDLE(texture2D) (texture2D)->getNativeTextureHandle()
#else
    // Typedefs that RIO would have otherwise imported

    #if defined(__WUT__)
        #define RIO_IS_WIN  0
        #define RIO_IS_CAFE 1
    #else
        #define RIO_IS_WIN  1
        #define RIO_IS_CAFE 0
    #endif

    #include <stdint.h>
    #include <stddef.h>
    #include <assert.h>
    #include <stdbool.h>
    typedef  int8_t s8;
    typedef uint8_t u8;

    typedef  int16_t s16;
    typedef uint16_t u16;

    typedef  int32_t s32;
    typedef uint32_t u32;

    typedef  int64_t s64;
    typedef uint64_t u64;

    typedef float  f32;
    typedef double f64;

    static_assert(sizeof(s8)  == sizeof(u8)  && sizeof(u8)  == sizeof(char) && sizeof(char) == 1);
    static_assert(sizeof(s16) == sizeof(u16) && sizeof(u16) == 2);
    static_assert(sizeof(s32) == sizeof(u32) && sizeof(u32) == 4);
    static_assert(sizeof(s64) == sizeof(u64) && sizeof(u64) == 8);
    static_assert(sizeof(f32) == 4);
    static_assert(sizeof(f64) == 8);
    // end of rio_Types.h typedefs

    // float 4x4 matrix
    #include <nn/ffl/FFLVec.h>
    typedef struct FFLRIOBaseMtx44f
    {
        union
        {
            // rio::BaseMtx44<float>
            f32         m[4][4];
            f32         a[4*4];
            FFLVec4     v[4];
        };
    }
    FFLRIOBaseMtx44f;

    typedef u32 FFLRIOCompareFunc; // TODO: not tested
#if RIO_IS_CAFE
    typedef GX2PrimitiveMode FFLRIOPrimitiveMode; // TODO: not tested
#elif RIO_IS_WIN
    typedef unsigned int FFLRIOPrimitiveMode; // should map directly..
#endif
    // only needed for below structure
#if RIO_IS_CAFE
    typedef const GX2Texture* FFLRIONativeTexture2DHandle;
    typedef GX2Texture FFLRIONativeTexture2D;
#elif RIO_IS_WIN
    typedef unsigned int FFLRIONativeTexture2DHandle;
    // below is SOLELY used as padding - rio::NativeTexture2D does NOT have pointers (only a u64?) so this constant size should be fine
    typedef u8 FFLRIONativeTexture2D[128]; // sizeof(rio::NativeTexture2D)
#endif

    // Substitutes for RIO structs that are used in public headers
    typedef struct FFLRIOTexture2D
    {
        FFLRIONativeTexture2D mTextureInner;
        // Only mHandle is needed to bind the texture on OpenGL
        FFLRIONativeTexture2DHandle mHandle; // OpenGL texture handle
        bool mSelfAllocated;
    } FFLRIOTexture2D;

    #define FFL_GET_RIO_NATIVE_TEXTURE_HANDLE(texture2D) (texture2D)->mHandle

#endif


#ifdef __cplusplus
extern "C" {
#endif

enum
{
    FFL_PATH_MAX_LEN = 256,

    FFL_FACE_TYPE_MAX       = 12,
    FFL_FACELINE_COLOR_MAX  = 6,
    FFL_FACE_LINE_MAX       = 12,
    FFL_FACE_MAKE_MAX       = 12,
    FFL_HAIR_TYPE_MAX       = 132,
    FFL_HAIR_COLOR_MAX      = 8,
    FFL_HAIR_DIR_MAX        = 2,
    FFL_EYE_TYPE_DATA_MAX   = 60,
    FFL_EYE_TYPE_TRUE_MAX   = 80,
    FFL_EYE_COLOR_MAX       = 6,
    FFL_EYE_SCALE_MAX       = 8,
    FFL_EYE_SCALE_Y_MAX     = 7,
    FFL_EYE_ROTATE_MAX      = 8,
    FFL_EYE_SPACING_MAX     = 13,
    FFL_EYE_POS_MAX         = 19,
    FFL_EYEBROW_TYPE_MAX    = 28,
    FFL_EYEBROW_COLOR_MAX   = 8,
    FFL_EYEBROW_SCALE_MAX   = 9,
    FFL_EYEBROW_SCALE_Y_MAX = 7,
    FFL_EYEBROW_ROTATE_MAX  = 12,
    FFL_EYEBROW_SPACING_MAX = 13,
    FFL_EYEBROW_POS_MIN     = 3,
    FFL_EYEBROW_POS_MAX     = 19,
    FFL_NOSE_TYPE_MAX       = 18,
    FFL_NOSE_SCALE_MAX      = 9,
    FFL_NOSE_POS_MAX        = 19,
    FFL_MOUTH_TYPE_DATA_MAX = 36,
    FFL_MOUTH_TYPE_TRUE_MAX = 52,
    FFL_MOUTH_COLOR_MAX     = 5,
    FFL_MOUTH_SCALE_MAX     = 9,
    FFL_MOUTH_SCALE_Y_MAX   = 7,
    FFL_MOUTH_POS_MAX       = 19,
    FFL_MUSTACHE_TYPE_MAX   = 6,
    FFL_BEARD_TYPE_MAX      = 6,
    FFL_BEARD_COLOR_MAX     = 8,
    FFL_MUSTACHE_SCALE_MAX  = 9,
    FFL_MUSTACHE_POS_MAX    = 17,
    FFL_GLASS_TYPE_MAX      = 9,
    FFL_GLASS_COLOR_MAX     = 6,
    FFL_GLASS_SCALE_MAX     = 8,
    FFL_GLASS_POS_MAX       = 21,
    FFL_MOLE_TYPE_MAX       = 2,
    FFL_MOLE_SCALE_MAX      = 9,
    FFL_MOLE_POS_X_MAX      = 17,
    FFL_MOLE_POS_Y_MAX      = 31,

    FFLI_FACELINE_COLOR_NUM = FFL_FACELINE_COLOR_MAX,
    FFLI_HAIR_COLOR_NUM     = FFL_HAIR_COLOR_MAX,
    FFLI_EYE_COLOR_R_NUM    = 3,
    FFLI_EYE_COLOR_B_NUM    = FFL_EYE_COLOR_MAX,
    FFLI_GLASS_COLOR_NUM    = FFL_GLASS_COLOR_MAX,
    FFLI_MOUTH_COLOR_R_NUM  = FFL_MOUTH_COLOR_MAX,
    FFLI_MOUTH_COLOR_G_NUM  = FFL_MOUTH_COLOR_MAX,
    FFLI_FAVORITE_COLOR_NUM = 12
};

// __int128 will only be defined on gnu, 64-bit
#if !RIO_IS_CAFE && __SIZEOF_INT128__
typedef __int128 FFLExpressionFlag;
#define FFL_EXPRESSION_LIMIT FFL_EXPRESSION_MAX
#else
// otherwise it is a u32 like normal
typedef u32 FFLExpressionFlag;
// FFL_EXPRESSION_LIMIT is what ExpressionTo*UseFlag use
// the purpose is that this is used in place of FFL_EXPRESSION_MAX
// so that they don't try to iterate and potentially overflow past 32
#define FFL_EXPRESSION_LIMIT 31
#endif

#ifdef __cplusplus
}
#endif

#endif // FFL_TYPES_H_
