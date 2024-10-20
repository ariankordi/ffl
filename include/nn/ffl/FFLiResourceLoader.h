#ifndef FFLI_RESOURCE_LOADER_H_
#define FFLI_RESOURCE_LOADER_H_

#include <nn/ffl/FFLResourceType.h>
#include <nn/ffl/FFLResult.h>

#include <nn/ffl/FFLiShapePartsType.h>
#include <nn/ffl/FFLiTexturePartsType.h>

#include <filedevice/rio_FileDevice.h>

class   FFLiResourceHeader;
class   FFLiResourceLoaderBuffer;
class   FFLiResourceManager;
struct  FFLiResourcePartsInfo;

class FFLiResourceLoader
{
public:
    FFLiResourceLoader(FFLiResourceManager* pResourceManager, FFLiResourceLoaderBuffer* pResLoaderBuffer, FFLResourceType resourceType);
    ~FFLiResourceLoader();

    bool IsExpand() const;

    FFLiResourceHeader* Header() const;

    u32 GetTextureAlignedMaxSize(FFLiTexturePartsType partsType) const;
    u32 GetShapeAlignedMaxSize(FFLiShapePartsType partsType) const;

    FFLResult LoadTexture(void* pData, u32* pSize, FFLiTexturePartsType partsType, u32 index);
    FFLResult LoadShape(void* pData, u32* pSize, FFLiShapePartsType partsType, u32 index);

    FFLResult GetPointerTextureByExpandCache(void** ppPtr, u32* pSize, FFLiTexturePartsType partsType, u32 index);

private:
    FFLResult Load(void* pData, u32* pSize, const FFLiResourcePartsInfo& partsInfo);

    FFLResult LoadFromCache(void* pData, const FFLiResourcePartsInfo& partsInfo);
    FFLResult LoadFromFile(void* pData, const FFLiResourcePartsInfo& partsInfo);

    bool IsCached() const;
    FFLResult GetPointerFromCache(void** ppPtr, const FFLiResourcePartsInfo& partsInfo);

    rio::RawErrorCode OpenIfClosed();
    s32 ReadWithPos(void* pDst, u32 pos, u32 size);
    rio::RawErrorCode Close();

private:
    FFLiResourceManager*        m_pResourceManager;
    FFLiResourceLoaderBuffer*   m_pBuffer;
#ifndef FFL_NO_FS
    rio::FileHandle             m_FileHandle;
#endif
    FFLResourceType             m_ResourceType;
};
//NN_STATIC_ASSERT(sizeof(FFLiResourceLoader) == 0x18);

#endif // FFLI_RESOURCE_LOADER_H_
