#ifndef FFL_TEXTURE_H_
#define FFL_TEXTURE_H_


#if FFL_USE_RIO
	#include <gpu/rio_Texture.h>
	typedef rio::Texture2D FFLTexture;
	#define FFL_GET_RIO_NATIVE_TEXTURE_HANDLE(texture2D) (texture2D)->getNativeTextureHandle()
#else
	#include <nn/ffl/FFLRIOInterop.h>
	typedef FFLRIOTexture2D FFLTexture;
#endif

// Marks that a faceline texture is supposed
// to be bound but render textures are not supported.
#define FFL_TEXTURE_PLACEHOLDER (FFLTexture*)0x01

#endif // FFL_TEXTURE_H_
