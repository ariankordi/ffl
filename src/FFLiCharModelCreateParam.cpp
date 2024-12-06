#include <nn/ffl/FFLCharModelDesc.h>

#include <nn/ffl/FFLiCharModelCreateParam.h>
#include <nn/ffl/FFLiFacelineTexture.h>
#include <nn/ffl/FFLiMaskTextures.h>
#include <nn/ffl/FFLiResourceManager.h>
#include <nn/ffl/FFLiResourceUncompressBuffer.h>
#include <nn/ffl/FFLiTexture.h>
#include <nn/ffl/FFLiTextureTempObject.h>
#include <nn/ffl/FFLiUtil.h>

FFLiCharModelCreateParam::FFLiCharModelCreateParam(FFLiDatabaseManager* pDatabaseManager, FFLiResourceManager* pResourceManager, FFLiShaderCallback* pCallback)
    : m_pDatabaseManager(pDatabaseManager)
    , m_pResourceManager(pResourceManager)
    , m_pShaderCallback(pCallback)
    , _18(false) // ???
{
}

FFLiCharModelCreateParam::~FFLiCharModelCreateParam()
{
}

u32 FFLiCharModelCreateParam::GetResolution(FFLResolution resolution)
{
    return resolution & FFL_RESOLUTION_MASK;
}

bool FFLiCharModelCreateParam::IsEnabledMipMap(FFLResolution resolution)
{
    return resolution & FFL_RESOLUTION_MIP_MAP_ENABLE_MASK;
}

#define FFL_MAX_EXPRESSION_FLAG_MASK (((FFLExpressionFlag)1 << FFL_EXPRESSION_LIMIT) - 1)

bool FFLiCharModelCreateParam::CheckModelDesc(const FFLCharModelDesc* pDesc)
{
    RIO_ASSERT(pDesc != NULL);
    if (pDesc == NULL)
        return false;

    if (pDesc->resourceType >= FFL_RESOURCE_TYPE_MAX)
    {
        RIO_LOG("FFLiCharModelCreateParam::CheckModelDesc: pDesc->resourceType (= %d) < %d\n", pDesc->resourceType, FFL_RESOURCE_TYPE_MAX);
        return false;
    }

    if ((pDesc->expressionFlag & FFL_MAX_EXPRESSION_FLAG_MASK) == 0)
    {
        RIO_LOG("FFLiCharModelCreateParam::CheckModelDesc: pDesc->expressionFlag invalid: %d\n", pDesc->expressionFlag);
        return false;
    }

#ifdef FFL_ENABLE_NEW_MASK_ONLY_FLAG
    if ((pDesc->modelFlag & 63) == 0)
#else
    if ((pDesc->modelFlag & 7) == 0)
#endif
    {
        RIO_LOG("FFLiCharModelCreateParam::CheckModelDesc: pDesc->modelFlag invalid: %d\n", pDesc->modelFlag);
        return false;
    }

    return true;
}
