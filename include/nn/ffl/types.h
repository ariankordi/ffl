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

#include <misc/rio_Types.h>

/*
#ifdef __cplusplus
    #include <type_traits>

    #define NN_STATIC_ASSERT static_assert
    #define NN_STATIC_ASSERT_IS_POD(T)  static_assert(std::is_trivial<T>::value)
#else // __cplusplus
    #include <assert.h>

    #define NN_STATIC_ASSERT _Static_assert
    #define NN_STATIC_ASSERT_IS_POD(T)  ((void)0)
#endif
*/


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

    #if defined(NDEBUG) || INTPTR_MAX == INT64_MAX
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

    FFL_FAVORITE_COLOR_MAX  = 12,

    FFLI_FACELINE_COLOR_NUM = FFL_FACELINE_COLOR_MAX,
    FFLI_HAIR_COLOR_NUM     = FFL_HAIR_COLOR_MAX,
    FFLI_EYE_COLOR_R_NUM    = 3,
    FFLI_EYE_COLOR_B_NUM    = FFL_EYE_COLOR_MAX,
    FFLI_GLASS_COLOR_NUM    = FFL_GLASS_COLOR_MAX,
    FFLI_MOUTH_COLOR_R_NUM  = FFL_MOUTH_COLOR_MAX,
    FFLI_MOUTH_COLOR_G_NUM  = FFL_MOUTH_COLOR_MAX,
    FFLI_FAVORITE_COLOR_NUM = FFL_FAVORITE_COLOR_MAX
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
