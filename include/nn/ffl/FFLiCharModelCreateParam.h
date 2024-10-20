#ifndef FFLI_CHAR_MODEL_CREATE_PARAM_H_
#define FFLI_CHAR_MODEL_CREATE_PARAM_H_

#include <nn/ffl/FFLResolution.h>

#include <nn/ffl/FFLiCoordinate.h>

struct FFLCharModelDesc;

class FFLiDatabaseManager;
class FFLiResourceManager;
class FFLiShaderCallback;

class FFLiCharModelCreateParam
{
public:
    FFLiCharModelCreateParam(FFLiDatabaseManager* pDatabaseManager, FFLiResourceManager* pResourceManager, FFLiShaderCallback* pCallback);
    ~FFLiCharModelCreateParam();

    static u32 GetResolution(FFLResolution resolution);
    static bool IsEnabledMipMap(FFLResolution resolution);

    static bool CheckModelDesc(const FFLCharModelDesc* pDesc);

    FFLiDatabaseManager* GetDatabaseManager() const
    {
        return m_pDatabaseManager;
    }

    FFLiResourceManager* GetResourceManager() const
    {
        return m_pResourceManager;
    }

    const FFLiCoordinate& GetCoordinate() const
    {
        return m_Coordinate;
    }

private:
    FFLiDatabaseManager*    m_pDatabaseManager;
    FFLiResourceManager*    m_pResourceManager;
    FFLiShaderCallback*     m_pShaderCallback;
    FFLiCoordinate          m_Coordinate;
    bool                    _18;    // Deleted
};
NN_STATIC_ASSERT32(sizeof(FFLiCharModelCreateParam) == 0x1C);

#endif // FFLI_CHAR_MODEL_CREATE_PARAM_H_
