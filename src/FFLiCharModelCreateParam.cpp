#include <nn/ffl/FFLCharModelDesc.h>

#include <nn/ffl/FFLiCharModelCreateParam.h>
#include <nn/ffl/FFLiFacelineTexture.h>
#include <nn/ffl/FFLiMaskTextures.h>
#include <nn/ffl/FFLiResourceManager.h>
#include <nn/ffl/FFLiResourceUncompressBuffer.h>
#include <nn/ffl/FFLiTexture.h>
#include <nn/ffl/FFLiTextureTempObject.h>
#include <nn/ffl/FFLiUtil.h>
#include <nn/ffl/FFLModelFlag.h>

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

#define FFL_MAX_EXPRESSION_FLAG_MASK (((u32)1 << (FFL_EXPRESSION_LIMIT > 31 ? 31 : FFL_EXPRESSION_LIMIT)) - 1)

namespace
{

    // Function to verify expression flag: if any bits are set and against the limit
    bool IsExpressionFlagValid(const FFLAllExpressionFlag* ef) {
        s32 anyBitsSet = 0;
#if FFL_EXPRESSION_LIMIT <= 96 // bit limit
        // Check the third u32
        if (ef->flags[2] & ~((1U << (FFL_EXPRESSION_LIMIT - 64)) - 1))
            return false; // Invalid: bits above limit are set in the third u32
        anyBitsSet = (ef->flags[0] & 0xFFFFFFFF) |
                     (ef->flags[1] & 0xFFFFFFFF) |
                     (ef->flags[2] & ((1U << (FFL_EXPRESSION_LIMIT - 64)) - 1));
#else // FFL_EXPRESSION_LIMIT <= 32
        // Check only the first u32
        u32 mask = ~((1U << FFL_EXPRESSION_LIMIT) - 1); // Bits above the limit
        if (ef->flags[0] & mask)
            return false; // Invalid: bits above limit are set
        anyBitsSet = ef->flags[0] & ((1U << FFL_EXPRESSION_LIMIT) - 1);
#endif

        // No bits over the limit have been set
        return anyBitsSet != 0; // Ensure at least one bit is set
    }

}

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

    if ((pDesc->modelFlag & 0x07) == 0) // Ensure one of first 3 bits are set.
    {
        RIO_LOG("FFLiCharModelCreateParam::CheckModelDesc: pDesc->modelFlag invalid: %d\n", pDesc->modelFlag);
        return false;
    }

    // Check expression flag depending on this flag:
    if (pDesc->modelFlag & FFL_MODEL_FLAG_NEW_EXPRESSIONS)
    {
        if (!IsExpressionFlagValid(&pDesc->allExpressionFlag))
        {
            RIO_LOG("FFLiCharModelCreateParam::CheckModelDesc: pDesc->allExpressionFlag invalid: %d/%d/%d\n",
                pDesc->allExpressionFlag.flags[0], pDesc->allExpressionFlag.flags[1], pDesc->allExpressionFlag.flags[2]);
            return false;
        }
    }
    else
    {
        if ((pDesc->expressionFlag & FFL_MAX_EXPRESSION_FLAG_MASK) == 0)
        {
            RIO_LOG("FFLiCharModelCreateParam::CheckModelDesc: pDesc->expressionFlag invalid: %d\n", pDesc->expressionFlag);
            return false;
        }
    }


    // note: model flag and expression flag check have been swapped in position

    return true;
}
