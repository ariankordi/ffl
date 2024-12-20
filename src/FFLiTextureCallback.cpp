#include <nn/ffl/FFLTextureCallback.h>

#include <nn/ffl/FFLiTextureCallback.h>

#ifdef FFL_USE_TEXTURE_CALLBACK

FFLiTextureCallback::FFLiTextureCallback()
    : m_pTextureCallback(NULL)
{
}

FFLiTextureCallback::~FFLiTextureCallback()
{
}

bool FFLiTextureCallback::IsExist() const
{
    return m_pTextureCallback != NULL;
}

void FFLiTextureCallback::Set(const FFLTextureCallback* pCallback)
{
    m_pTextureCallback = pCallback;
}

void FFLiTextureCallback::CallCreate(const FFLTextureInfo* pTextureInfo, FFLTexture* pTexture) const
{
    if (IsExist())
        (*m_pTextureCallback->pCreateFunc)(m_pTextureCallback->pObj, pTextureInfo, pTexture);
}

void FFLiTextureCallback::CallDelete(FFLTexture* pTexture) const
{
    if (IsExist())
        (*m_pTextureCallback->pDeleteFunc)(m_pTextureCallback->pObj, pTexture);
}

#endif // FFL_USE_TEXTURE_CALLBACK
