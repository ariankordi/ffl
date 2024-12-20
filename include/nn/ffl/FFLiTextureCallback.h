#ifndef FFLI_TEXTURE_CALLBACK_H_
#define FFLI_TEXTURE_CALLBACK_H_

#ifdef FFL_USE_TEXTURE_CALLBACK

#include <nn/ffl/types.h>
#include <nn/ffl/FFLTextureCallback.h>

struct FFLTextureInfo;
struct FFLTextureCallback;

class FFLiTextureCallback
{
public:
    FFLiTextureCallback();
    ~FFLiTextureCallback();

    bool IsExist() const;

    const FFLTextureCallback* Get() const
    {
        return m_pTextureCallback;
    }

    void Set(const FFLTextureCallback* pCallback);

    void CallCreate(const FFLTextureInfo* pTextureInfo, FFLTexture* pTexture) const;
    void CallDelete(FFLTexture* pTexture) const;

private:
    const FFLTextureCallback*    m_pTextureCallback;
};
NN_STATIC_ASSERT32(sizeof(FFLiTextureCallback) == 4);

#endif // FFLI_TEXTURE_CALLBACK_H_

#endif // FFL_USE_TEXTURE_CALLBACK
