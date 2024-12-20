#include <nn/ffl/FFLTextureCallback.h>

#include <nn/ffl/FFLiManager.h>

#ifdef FFL_USE_TEXTURE_CALLBACK

void FFLSetTextureCallback(const FFLTextureCallback* pCallback)
{
    RIO_ASSERT(pCallback != NULL);
    if (FFLiManager::IsConstruct())
        FFLiManager::GetInstance()->GetTextureCallback().Set(pCallback);
}

#endif
