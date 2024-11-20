#ifndef FFL_RIO_INTEROP_H_
#define FFL_RIO_INTEROP_H_

#include <nn/ffl/types.h>
#include <nn/ffl/FFLVec.h>

#ifdef __cplusplus
extern "C" {
#endif

// Substitutes for RIO structs that are used in public headers

// float 4x4 matrix
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

typedef struct FFLRIOTexture2D
{
	FFLRIONativeTexture2D mTextureInner;
	// Only mHandle is needed to bind the texture on OpenGL
	FFLRIONativeTexture2DHandle mHandle; // OpenGL texture handle
	bool mSelfAllocated;
}
FFLRIOTexture2D;

#define FFL_GET_RIO_NATIVE_TEXTURE_HANDLE(texture2D) (texture2D)->mHandle

#ifdef __cplusplus
}
#endif

#endif // FFL_RIO_INTEROP_H_
