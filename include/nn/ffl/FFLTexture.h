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


#endif // FFL_TEXTURE_H_