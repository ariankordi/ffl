#include <nn/ffl/FFLiResourceHeader.h>
#include <nn/ffl/FFLiResourceUtil.h>

#include <nn/ffl/detail/FFLiResourceCache.h>

#include <misc/rio_MemUtil.h>

FFLiResourceCache::FFLiResourceCache()
{
    rio::MemUtil::set(m_Res, 0, sizeof(m_Res));
}

FFLiResourceCache::~FFLiResourceCache()
{
    for (u32 i = 0; i < FFL_RESOURCE_TYPE_MAX; i++)
    {
        if (m_Res[i].pHeader != nullptr)
            delete m_Res[i].pHeader;
    }
}

FFLResult FFLiResourceCache::Attach(void* pData, u32 size, FFLResourceType resourceType)
{
    RIO_ASSERT(pData != nullptr);
    RIO_ASSERT(size >= sizeof(FFLiResourceHeader));

    FFLiResourceHeader* pHeader;// = (FFLiResourceHeader*)pData;

    // will be freed by destructor if it is bound
    bool needsEndianSwap = false;
    //pHeader = new FFLiResourceHeaderAFL_2_3();
    pHeader = DetermineAndAllocateResourceHeaderType(pData, &needsEndianSwap);
    RIO_ASSERT(pHeader != nullptr);

    pHeader->SetHeader(pData);

    pHeader->m_NeedsEndianSwap = needsEndianSwap;
    if (pHeader->m_NeedsEndianSwap)
        pHeader->SwapEndian();

    FFLResult result = FFLiIsVaildResourceHeader(pHeader);
    if (result != FFL_RESULT_OK)
    {
        if(pHeader->m_NeedsEndianSwap)
        {
            reinterpret_cast<FFLiResourceHeaderDefaultData*>(pData)->SwapEndian();
            pHeader->SwapEndian();
        }

        delete pHeader;

        return result;
    }

    if (pHeader->m_NeedsEndianSwap)
    {
        for (u32 i = 0; i < FFLI_TEXTURE_PARTS_TYPE_MAX; i++)
        {
            u32 num;
            FFLiResourcePartsInfo* pPartsInfo = FFLiGetTextureResoucePartsInfos(&num, pHeader, FFLiTexturePartsType(i));

            for (u32 j = 0; j < num; j++)
            {
                const FFLiResourcePartsInfo& partsInfo = pPartsInfo[j];
                u32 size = partsInfo.dataSize;
                if (size == 0 || partsInfo.strategy != FFLI_RESOURCE_STRATEGY_UNCOMPRESSED)
                    continue;

                u8* pFooterData = (u8*)pData + partsInfo.dataPos;

                FFLiResourceTextureFooter& footer = FFLiResourceTextureFooter::GetFooterImpl(pFooterData, size);
                footer.SwapEndian();
            }
        }

        for (u32 i = 0; i < FFLI_SHAPE_PARTS_TYPE_MAX; i++)
        {
            u32 num;
            FFLiResourcePartsInfo* pPartsInfo = FFLiGetShapeResoucePartsInfos(&num, pHeader->GetShapeHeader(), FFLiShapePartsType(i));

            for (u32 j = 0; j < num; j++)
            {
                const FFLiResourcePartsInfo& partsInfo = pPartsInfo[j];
                u32 size = partsInfo.dataSize;
                if (size == 0 || partsInfo.strategy != FFLI_RESOURCE_STRATEGY_UNCOMPRESSED)
                    continue;

                u8* pFooterData = (u8*)pData + partsInfo.dataPos;

                FFLiSwapEndianResourceShapeElement(pFooterData, FFLiShapePartsType(i), false);
            }
        }
    }

    m_Res[resourceType].pHeader = pHeader;
    m_Res[resourceType].size = size;

    return FFL_RESULT_OK;
}

bool FFLiResourceCache::IsAttached() const
{
    for (u32 i = 0; i < FFL_RESOURCE_TYPE_MAX; i++)
        if (IsValid(FFLResourceType(i)))
            return true;

    return false;
}

bool FFLiResourceCache::IsValid(FFLResourceType resourceType) const
{
    return m_Res[resourceType].pHeader != NULL;
}

FFLiResourceHeader* FFLiResourceCache::Header(FFLResourceType resourceType) const
{
    return m_Res[resourceType].pHeader;
}
