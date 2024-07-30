#include <nn/ffl/FFLiResourceHeader.h>
#include <nn/ffl/FFLiResourceLoader.h>
#include <nn/ffl/FFLiResourceLoaderBuffer.h>
#include <nn/ffl/FFLiResourceManager.h>
#include <nn/ffl/FFLiUtil.h>
#include <nn/ffl/FFLiZlibInflator.h>

#include <filedevice/rio_FileDeviceMgr.h>

namespace {

bool Uncompress(void* pDst, const void* pSrc, FFLiResourceUncompressBuffer* pBuffer, const FFLiResourcePartsInfo& partsInfo);

}

FFLiResourceLoader::FFLiResourceLoader(FFLiResourceManager* pResourceManager, FFLiResourceLoaderBuffer* pResLoaderBuffer, FFLResourceType resourceType)
    : m_pResourceManager(pResourceManager)
    , m_pBuffer(pResLoaderBuffer)
    , m_ResourceType(resourceType)
{
}

FFLiResourceLoader::~FFLiResourceLoader()
{
    Close();
}

bool FFLiResourceLoader::IsExpand() const
{
    return m_pResourceManager->IsExpand(m_ResourceType);
}

FFLiResourceHeader* FFLiResourceLoader::Header() const
{
    return m_pResourceManager->Header(m_ResourceType);
}

u32 FFLiResourceLoader::GetTextureAlignedMaxSize(FFLiTexturePartsType partsType) const
{
    return FFLiRoundUp(Header()->GetTextureMaxSize(partsType), rio::FileDevice::cBufferMinAlignment);
}

u32 FFLiResourceLoader::GetShapeAlignedMaxSize(FFLiShapePartsType partsType) const
{
    return FFLiRoundUp(Header()->GetShapeMaxSize(partsType), rio::FileDevice::cBufferMinAlignment);
}

FFLResult FFLiResourceLoader::LoadTexture(void* pData, u32* pSize, FFLiTexturePartsType partsType, u32 index)
{
    u32 num;
    FFLiResourcePartsInfo* pPartsInfo = FFLiGetTextureResoucePartsInfos(&num, Header(), partsType);

    // NOTE: if this is a glass type, we are going to check...
    // ... if the currently loaded resource has the new glass types
    // if it does NOT then we will map the index to the ver3 table
    // effectively loads new glasses if they are in the resource
    // while loading the old glasses if they are not

    // glass type threshold for FFLResHigh, FFLResMiddle
    // this is what num would be if you were using those resources
    // new switch glasses to ver3 table
    // taken from MiiPort/include/convert_mii.h: https://github.com/Genwald/MiiPort/blob/4ee38bbb8aa68a2365e9c48d59d7709f760f9b5d/include/convert_mii.h
    static const u8 ToVer3GlassTypeTable[20] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 1, 3, 7, 7, 6, 7, 8, 7, 7};

    if (partsType == FFLI_TEXTURE_PARTS_TYPE_GLASS
        // is this a higher glass type than what is in the default resource?
        && index > FFL_GLASS_TYPE_MAX - 1
        // well, then does this resource have more than that many?
        && num < index
        // finally, check the glass type is not too large for th table
        && index < sizeof(ToVer3GlassTypeTable)
    )
        // ... in this case, your resource does not have that glass type
        // assuming this is a new glass type, we will map it to ver3
        index = ToVer3GlassTypeTable[index];

    if (pPartsInfo == NULL || index >= num)
        return FFL_RESULT_ERROR;

    const FFLiResourcePartsInfo& partsInfo = pPartsInfo[index];

    FFLResult result = Load(pData, pSize, partsInfo);
    if (result != FFL_RESULT_OK)
        return result;

#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    u32 size = *pSize;
    if (size && ((IsCached() && partsInfo.strategy != FFLI_RESOURCE_STRATEGY_UNCOMPRESSED) || !IsCached()))
    {
        FFLiResourceTextureFooter& footer = FFLiResourceTextureFooter::GetFooterImpl(pData, size);
        footer.SwapEndian();
    }
#endif // __BYTE_ORDER__

    return FFL_RESULT_OK;
}

FFLResult FFLiResourceLoader::LoadShape(void* pData, u32* pSize, FFLiShapePartsType partsType, u32 index)
{
    u32 num;
    FFLiResourcePartsInfo* pPartsInfo = FFLiGetShapeResoucePartsInfos(&num, Header()->GetShapeHeader(), partsType);
    if (pPartsInfo == NULL || index >= num)
        return FFL_RESULT_ERROR;

    const FFLiResourcePartsInfo& partsInfo = pPartsInfo[index];

    FFLResult result = Load(pData, pSize, partsInfo);
    if (result != FFL_RESULT_OK)
        return result;

#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    u32 size = *pSize;
    if (size && ((IsCached() && partsInfo.strategy != FFLI_RESOURCE_STRATEGY_UNCOMPRESSED) || !IsCached()))
    {
        FFLiSwapEndianResourceShapeElement(pData, partsType, false);
    }
#endif // __BYTE_ORDER__

    return FFL_RESULT_OK;
}

FFLResult FFLiResourceLoader::GetPointerTextureByExpandCache(void** ppPtr, u32* pSize, FFLiTexturePartsType partsType, u32 index)
{
    if (!IsExpand())
        return FFL_RESULT_FILE_LOAD_ERROR;

    u32 num;
    FFLiResourcePartsInfo* pPartsInfo = FFLiGetTextureResoucePartsInfos(&num, Header(), partsType);
    if (pPartsInfo == NULL || index >= num)
        return FFL_RESULT_ERROR;

    *pSize = pPartsInfo[index].dataSize;
    return GetPointerFromCache(ppPtr, pPartsInfo[index]);
}

FFLResult FFLiResourceLoader::Load(void* pData, u32* pSize, const FFLiResourcePartsInfo& partsInfo)
{
    *pSize = partsInfo.dataSize;

    if (partsInfo.dataSize != 0)
    {
        if (IsCached())
        {
            FFLResult result = LoadFromCache(pData, partsInfo);
            if (result != FFL_RESULT_OK)
                return result;
        }
        else
        {
            FFLResult result = LoadFromFile(pData, partsInfo);
            if (result != FFL_RESULT_OK)
                return result;
        }
    }

    return FFL_RESULT_OK;
}

FFLResult FFLiResourceLoader::LoadFromCache(void* pData, const FFLiResourcePartsInfo& partsInfo)
{
    void* ptr = NULL;
    FFLResult result = GetPointerFromCache(&ptr, partsInfo);
    if (result != FFL_RESULT_OK)
        return result;

    if (partsInfo.strategy == FFLI_RESOURCE_STRATEGY_UNCOMPRESSED)
    {
        rio::MemUtil::copy(pData, ptr, partsInfo.dataSize);
    }
    else
    {
        if (!Uncompress(pData, ptr, &m_pBuffer->GetUncompressBuffer(), partsInfo))
            return FFL_RESULT_FILE_LOAD_ERROR;
    }

    return FFL_RESULT_OK;
}

FFLResult FFLiResourceLoader::LoadFromFile(void* pData, const FFLiResourcePartsInfo& partsInfo)
{
    if (OpenIfClosed() != rio::RAW_ERROR_OK)
        return FFL_RESULT_RES_FS_ERROR;

    if (partsInfo.strategy == FFLI_RESOURCE_STRATEGY_UNCOMPRESSED)
    {
        if (ReadWithPos(pData, partsInfo.dataPos, partsInfo.dataSize) != 1)
            return FFL_RESULT_RES_FS_ERROR;
    }
    else
    {
        if (ReadWithPos(m_pBuffer->GetUncompressBuffer().Buffer(), partsInfo.dataPos, partsInfo.compressedSize) != 1)
            return FFL_RESULT_RES_FS_ERROR;

        if (!Uncompress(pData, m_pBuffer->GetUncompressBuffer().Buffer(), &m_pBuffer->GetUncompressBuffer(), partsInfo))
            return FFL_RESULT_FILE_LOAD_ERROR;
    }

    return FFL_RESULT_OK;
}

bool FFLiResourceLoader::IsCached() const
{
    return m_pResourceManager->IsCached();
}

FFLResult FFLiResourceLoader::GetPointerFromCache(void** ppPtr, const FFLiResourcePartsInfo& partsInfo)
{
    if (!IsCached())
        return FFL_RESULT_FILE_LOAD_ERROR;

    *ppPtr = (u8*)m_pResourceManager->GetResourceCache().Header(m_ResourceType) + partsInfo.dataPos;
    return FFL_RESULT_OK;
}

rio::RawErrorCode FFLiResourceLoader::OpenIfClosed()
{
    if (m_FileHandle.getDevice() != nullptr)
        return rio::RAW_ERROR_OK;

    rio::NativeFileDevice* device = rio::FileDeviceMgr::instance()->getNativeFileDevice();
    if (!device->tryOpen(&m_FileHandle, m_pResourceManager->GetPath(m_ResourceType), rio::FileDevice::FILE_OPEN_FLAG_READ))
    {
        rio::RawErrorCode status = m_FileHandle.getDevice()->getLastRawError();
        RIO_ASSERT(status != rio::RAW_ERROR_OK);
        return status;
    }

    return rio::RAW_ERROR_OK;
}

rio::RawErrorCode FFLiResourceLoader::ReadWithPos(void* pDst, u32 pos, u32 size)
{
    if (!m_FileHandle.trySeek(pos, rio::FileDevice::SEEK_ORIGIN_BEGIN))
    {
        rio::RawErrorCode status = m_FileHandle.getDevice()->getLastRawError();
        RIO_ASSERT(status != rio::RAW_ERROR_OK);
        return status;
    }

    u32 readSize = 0;
    if (!m_FileHandle.tryRead(&readSize, (u8*)pDst, FFLiRoundUp(size, rio::FileDevice::cBufferMinAlignment)))
    {
        rio::RawErrorCode status = m_FileHandle.getDevice()->getLastRawError();
        RIO_ASSERT(status != rio::RAW_ERROR_OK);
        return status;
    }

    return rio::RAW_ERROR_OK;
}

rio::RawErrorCode FFLiResourceLoader::Close()
{
    if (m_FileHandle.getDevice() == nullptr)
        return rio::RAW_ERROR_OK;

    if (!m_FileHandle.tryClose())
    {
        rio::RawErrorCode status = m_FileHandle.getDevice()->getLastRawError();
        RIO_ASSERT(status != rio::RAW_ERROR_OK);
        return status;
    }

    return rio::RAW_ERROR_OK;
}

namespace {

bool Uncompress(void* pDst, const void* pSrc, FFLiResourceUncompressBuffer* pBuffer, const FFLiResourcePartsInfo& partsInfo)
{
    FFLiZlibInflator inflator(FFLiResourceWindowBitsToZlibWindowBits(FFLiResourceWindowBits(partsInfo.windowBits)));

    void* dst = pDst;
    u32 dstSize = partsInfo.dataSize;

    const void* src = pSrc;
    u32 srcSize = partsInfo.compressedSize;

    return inflator.Process(&dst, &dstSize, &src, &srcSize, Z_FINISH) == Z_STREAM_END;
}

}
