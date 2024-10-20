#include <nn/ffl/FFLiPath.h>
#include <nn/ffl/FFLiResourceHeader.h>
#include <nn/ffl/FFLiResourceManager.h>
#include <nn/ffl/FFLiResourceUtil.h>
#include <nn/ffl/FFLiUtil.h>

#include <filedevice/rio_FileDeviceMgr.h>


namespace {

const char* RESOUCE_FILE_NAME[2][FFL_RESOURCE_TYPE_MAX] = {
    {
        "FFLResMiddle.dat",
        "FFLResHigh.dat"
    },
    {
        "FFLResMiddleLG.dat",
        "FFLResHighLG.dat"
    }
};

}

FFLiResourceManager::FFLiResourceManager(FFLiResourceMultiHeader* pHeader)
#ifndef FFL_NO_FS
    : m_pResourceMultiHeader(pHeader)
#endif
{
#ifndef FFL_NO_FS
    rio::MemUtil::set(m_pResourceMultiHeader, 0, sizeof(FFLiResourceMultiHeader));
#endif
    rio::MemUtil::set(m_Path, 0, (s32)FFL_RESOURCE_TYPE_MAX * (s32)FFL_PATH_MAX_LEN);
}

FFLiResourceManager::~FFLiResourceManager()
{
    // NOTE: m_pResourceMultiHeader is freed by FFLiResourceManager
}


const char* FFLiResourceManager::GetRelativeResourcePath(FFLResourceType resourceType, bool LG)
{
    return RESOUCE_FILE_NAME[LG][resourceType];
}

FFLResult FFLiResourceManager::GetResourcePath(char* pDst, u32 size, FFLResourceType resourceType, bool LG)
{
    rio::RawErrorCode status = FFLiGetResourcePath(pDst, size, GetRelativeResourcePath(resourceType, LG));
    return FFLiConvertFSStatusToFFLResult(status);
}

FFLResult FFLiResourceManager::AfterConstruct()
{
    for (u32 i = 0; i < FFL_RESOURCE_TYPE_MAX; i++)
    {
        FFLResult result = GetResourcePath(m_Path[i], FFL_PATH_MAX_LEN, FFLResourceType(i), false);
        if (result != FFL_RESULT_OK)
            return result;
    }
    return FFL_RESULT_OK;
}

#ifndef FFL_NO_FS

FFLResult FFLiResourceManager::LoadResourceHeader()
{
    return LoadResourceHeaderImpl();
}

FFLResult FFLiResourceManager::LoadResourceHeaderImpl()
{
    rio::FileHandle fileHandle;
    rio::NativeFileDevice* device = rio::FileDeviceMgr::instance()->getNativeFileDevice();

    for (u32 i = 0; i < FFL_RESOURCE_TYPE_MAX; i++)
    {
        //FFLiResourceHeader* pHeader = &(m_pResourceMultiHeader->header[i]);
        // FIRST, we will make a small buffer, not reused, just to
        // read enough about the header to know which kind it is...
        char pHeaderPreData[sizeof(FFLiResourceHeaderDefaultData)];

        if (!device->tryOpen(&fileHandle, GetPath(FFLResourceType(i)), rio::FileDevice::FILE_OPEN_FLAG_READ))
            return FFL_RESULT_FILE_INVALID;

        u32 readSize = 0;
        // ... read into the small buffer.
        if (!(fileHandle.tryRead(&readSize, (u8*)pHeaderPreData, sizeof(FFLiResourceHeaderDefaultData)) && readSize == sizeof(FFLiResourceHeaderDefaultData)))
        {
            fileHandle.tryClose();
            return FFL_RESULT_FILE_INVALID;
        }

        // ... NOW, determine type and try to
        // read in the ENTIRE header into a NEW buffer
        bool needsEndianSwap;
        m_pResourceMultiHeader->header[i] = DetermineAndAllocateResourceHeaderType(pHeaderPreData, &needsEndianSwap);

        RIO_ASSERT(m_pResourceMultiHeader->header[i] != nullptr);

        m_pResourceMultiHeader->header[i]->m_NeedsEndianSwap = needsEndianSwap;
        // Read in the ENTIRE thing to a brand new buffer.
        u32 headerSize = m_pResourceMultiHeader->header[i]->GetHeaderSize();

        void* pData = rio::MemUtil::alloc(headerSize, rio::FileDevice::cBufferMinAlignment);
        rio::MemUtil::copy(pData, &pHeaderPreData, sizeof(FFLiResourceHeaderDefaultData));

        headerSize -= readSize;
        //readSize = 0;

        if (!(
            //fileHandle.trySeek(0, rio::FileDevice::SEEK_ORIGIN_BEGIN) &&
            fileHandle.tryRead(&readSize, (u8*)pData+readSize, headerSize)
            && readSize == headerSize
            ))
        {
            fileHandle.tryClose();
            rio::MemUtil::free(pData);
            delete m_pResourceMultiHeader->header[i];
            m_pResourceMultiHeader->header[i] = nullptr;
            return FFL_RESULT_FILE_INVALID;
        }


        // Use that buffer in the header class.
        m_pResourceMultiHeader->header[i]->SetHeader(pData);
        // (Will be freed by the destructor)

        if (!fileHandle.tryClose())
        {
            rio::MemUtil::free(pData);
            delete m_pResourceMultiHeader->header[i];
            m_pResourceMultiHeader->header[i] = nullptr;
            return FFL_RESULT_FILE_INVALID;
        }

        if (m_pResourceMultiHeader->header[i]->m_NeedsEndianSwap)
        {
            // If reading the entire thing from the beginning:
            // flip endianness for only the first part of the header
            //reinterpret_cast<FFLiResourceHeaderDefaultData*>(pData)->SwapEndian();
            m_pResourceMultiHeader->header[i]->SwapEndian();
        }

        FFLResult result = FFLiIsVaildResourceHeader(m_pResourceMultiHeader->header[i]);
        RIO_ASSERT(result == FFL_RESULT_OK);
        if (result != FFL_RESULT_OK)
        {
            rio::MemUtil::free(pData);
            delete m_pResourceMultiHeader->header[i];
            m_pResourceMultiHeader->header[i] = nullptr;
            return result;
        }
    }

    return FFL_RESULT_OK;
}

#endif // FFL_NO_FS

FFLResult FFLiResourceManager::AttachCache(void* pData, u32 size, FFLResourceType resourceType)
{
    return m_ResourceCache.Attach(pData, size, resourceType);
}

bool FFLiResourceManager::IsCached() const
{
    return m_ResourceCache.IsAttached();
}

FFLiResourceHeader* FFLiResourceManager::Header(FFLResourceType resourceType) const
{
    if (IsCached())
        return HeaderFromCache(resourceType);

    else
#ifndef FFL_NO_FS
        return HeaderFromFile(resourceType);
#else
        {
            RIO_ASSERT(false);
            return nullptr;
        }
#endif // FFL_NO_FS
}

FFLiResourceHeader* FFLiResourceManager::HeaderFromCache(FFLResourceType resourceType) const
{
    return m_ResourceCache.Header(resourceType);
}


#ifndef FFL_NO_FS
FFLiResourceHeader* FFLiResourceManager::HeaderFromFile(FFLResourceType resourceType) const
{
    return m_pResourceMultiHeader->header[resourceType];
}
#endif

u32 FFLiResourceManager::GetTextureAlignedMaxSize(FFLResourceType resourceType, FFLiTexturePartsType partsType) const
{
    return FFLiRoundUp(Header(resourceType)->GetTextureMaxSize(partsType), rio::FileDevice::cBufferMinAlignment);
}

u32 FFLiResourceManager::GetShapeAlignedMaxSize(FFLResourceType resourceType, FFLiShapePartsType partsType) const
{
    return FFLiRoundUp(Header(resourceType)->GetShapeMaxSize(partsType), rio::FileDevice::cBufferMinAlignment);
}

bool FFLiResourceManager::IsValid(FFLResourceType resourceType) const
{
    if (!IsCached())
        return true;

    return m_ResourceCache.IsValid(resourceType);
}

bool FFLiResourceManager::IsExpand(FFLResourceType resourceType) const
{
    if (!IsValid(resourceType) || !IsCached())
        return false;

    return Header(resourceType)->IsExpand();
}

u32 FFLiResourceManager::GetUncompressBufferSize(FFLResourceType resourceType) const
{
    return FFLiRoundUp(Header(resourceType)->GetUncompressBufferSize() + rio::FileDevice::cBufferMinAlignment, rio::FileDevice::cBufferMinAlignment);
}

const char* FFLiResourceManager::GetPath(FFLResourceType resourceType) const
{
    return m_Path[resourceType];
}
