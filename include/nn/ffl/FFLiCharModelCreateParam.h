#ifndef FFLI_CHAR_MODEL_CREATE_PARAM_H_
#define FFLI_CHAR_MODEL_CREATE_PARAM_H_

#include <nn/ffl/FFLiCoordinate.h>

struct FFLCharModelDesc;

class FFLiDatabaseManager;
class FFLiResourceManager;
class FFLiShaderCallback;

class FFLiCharModelCreateParam
{
public:
    FFLiCharModelCreateParam(FFLiDatabaseManager* pDatabaseManager, FFLiResourceManager* pResourceManager, FFLiShaderCallback* pCallback, bool isShaderAvailable);
    ~FFLiCharModelCreateParam();

    u32 GetBufferSize(const FFLCharModelDesc* pDesc) const;
    u32 GetTempBufferSize(const FFLCharModelDesc* pDesc) const;

private:
    FFLiDatabaseManager*    mp_DatabaseManager;
    FFLiResourceManager*    mp_ResourceManager;
    FFLiShaderCallback*     mp_ShaderCallback;
    FFLiCoordinate          m_Coordinate;
    bool                    m_IsShaderAvailable;
};
NN_STATIC_ASSERT(sizeof(FFLiCharModelCreateParam) == 0x1C);

#endif // FFLI_CHAR_MODEL_CREATE_PARAM_H_
