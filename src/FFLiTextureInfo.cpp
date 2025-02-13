#include <nn/ffl/FFLiUtil.h>
#include <nn/ffl/FFLiTextureInfo.h>

#include <nn/ffl/FFLiResourceHeader.h>
#include <nn/ffl/FFLiResourceLoader.h>

struct TextureFormatParam
{
    // Bytes per pixel, or block size in bytes for compressed
    u32 size;
    // 0 = uncompressed, block size (usually 4) for compressed
    u32 block;
};

namespace {
    // Table of supported texture formats.
    static const TextureFormatParam TextureFormatTable[FFL_TEXTURE_FORMAT_MAX] = {
        {  1, 0 }, // FFL_TEXTURE_FORMAT_R8_UNORM
        {  2, 0 }, // FFL_TEXTURE_FORMAT_R8_G8_UNORM
        {  4, 0 }  // FFL_TEXTURE_FORMAT_R8_G8_B8_A8_UNORM
    };
    NN_STATIC_ASSERT(sizeof(TextureFormatTable) == sizeof(TextureFormatParam) * FFL_TEXTURE_FORMAT_MAX);

    // replacement for rio::Texture2DUtil::calcImageSize
    static u32 CalcImageSize(FFLTextureFormat format, u32 width, u32 height)
    {
        const TextureFormatParam param = TextureFormatTable[format]; // resolve from table

        const u32 size = param.size;
        if (param.block > 0) // Compressed format
        {
            width  = (width  + param.block - 1) / param.block;
            height = (height + param.block - 1) / param.block;
        }

        return width * height * size;
    }

    // replacement for rio::Texture2DUtil::calcMipmapSize
    static u32 CalcMipmapSize(FFLTextureFormat format, u32 width, u32 height,
                                    u32 mipLevels, u32* mipLevelOffset)
    {
        if (mipLevelOffset != nullptr)
            // Initialize mipLevelOffset with zeroes.
            rio::MemUtil::set(mipLevelOffset, 0, sizeof(u32) * 13);

        // Maximum mipmaps of 14.
        mipLevels = std::min(std::max(mipLevels, 1u), 14u);

        const TextureFormatParam param = TextureFormatTable[format]; // resolve from table

        const u32 size = param.size;
        u32 mipmapSize = 0;

        for (u32 i = 1; i < mipLevels; i++)
        {
            if (mipLevelOffset)
                mipLevelOffset[i - 1] = mipmapSize;

            u32 mipWidth  = std::max(width  >> i, 1u);
            u32 mipHeight = std::max(height >> i, 1u);

            if (param.block > 0) // Compressed format
            {
                mipWidth  = (mipWidth  + param.block - 1) / param.block;
                mipHeight = (mipHeight + param.block - 1) / param.block;
            }

            mipmapSize += mipWidth * mipHeight * size;
        }

        return mipmapSize;
    }
}

#define FFLI_RESOUCE_MAX_ALIGNMENT_TEXTURE (0x800)

// new function to get FFLTextureInfo which is both used to
// load texture with RIO/GX2 and passed to texture callback
FFLResult FFLiLoadTextureInfo(FFLTextureInfo* textureInfo, FFLiTexturePartsType partsType, u32 index,
    FFLiResourceLoader* pResLoader, void** pData)
{
    u32 size = pResLoader->GetTextureAlignedMaxSize(partsType);
    // void* pData;

    // Sanity check. Feel free to remove this if you do have resources this large.
    RIO_ASSERT(size < 200000000 && "Are you sure this texture is supposed to be over 200 MB large?");

    if (!pResLoader->IsExpand())
    {
        *pData = rio::MemUtil::alloc(size, FFLI_RESOUCE_MAX_ALIGNMENT_TEXTURE);
        FFLResult result = pResLoader->LoadTexture(*pData, &size, partsType, index);
        if (result != FFL_RESULT_OK)
        {
            rio::MemUtil::free(*pData);
            return result;
        }
    }
    else
    {
        *pData = NULL;
        FFLResult result = pResLoader->GetPointerTextureByExpandCache(pData, &size, partsType, index);
        if (result != FFL_RESULT_OK)
            return result;
    }

    // Make sure that the texture size is aligned
    // because GetFooterImpl() uses the size to
    // make a pointer that needs to be aligned
    RIO_ASSERT(FFLiCheckAlign(size, 4));

    const FFLiResourceTextureFooter& footer = FFLiResourceTextureFooter::GetFooterImpl(*pData, size);

    const u32 width = footer.Width();
    const u32 height = footer.Height();
    // const rio::TextureFormat textureFormat = static_cast<rio::TextureFormat>(footer.SurfaceFormat()); // for calcImageSize
    const FFLTextureFormat textureFormat = footer.TextureFormat();

    // determine whether this header's textures are linear
    FFLiResourceHeader* pHeader = pResLoader->Header();
    bool textureFormatIsLinear = pHeader->TextureFormatIsLinear();

    textureInfo->width = width;
    textureInfo->height = height;
    textureInfo->format = static_cast<u8>(textureFormat);

    textureInfo->imageSize = CalcImageSize(textureFormat, width, height);
    // textureInfo->mipCount = footer.NumMips();

    textureInfo->imagePtr = footer.GetImagePtrImpl(size);
#if RIO_IS_CAFE
    RIO_ASSERT(FFLiCheckAlignPtr(textureInfo->imagePtr, FFLI_RESOUCE_MAX_ALIGNMENT_TEXTURE));
#endif // RIO_IS_CAFE

    // Determine whether or not to ignore footer's mipmaps.
    if (pHeader->IgnoreMipMaps())
    {
        textureInfo->mipCount = 1;
        textureInfo->mipPtr = nullptr;
    }
    else
    {
        textureInfo->mipCount = footer.NumMips();
        textureInfo->mipPtr = footer.GetMipPtrImpl(size);
#if RIO_IS_CAFE
        RIO_ASSERT(FFLiCheckAlignPtr(textureInfo->mipPtr, FFLI_RESOUCE_MAX_ALIGNMENT_TEXTURE));
#endif // RIO_IS_CAFE
    }
    // Calculate mipSize, mipLevelOffset array / zero it out.
    textureInfo->mipSize = CalcMipmapSize(textureFormat, width, height,
                    textureInfo->mipCount, textureInfo->mipLevelOffset);

/*
    if (textureInfo->mipPtr == nullptr && textureInfo->mipCount > 1)
        // If there are mipmaps, the mipPtr will
        // begin after the image data.
        textureInfo->mipPtr = (void*)((uintptr_t)textureInfo->imagePtr
                                    + textureInfo->mipLevelOffset[0]);
*/

    textureInfo->isGX2Tiled = !textureFormatIsLinear;

    return FFL_RESULT_OK;

    // caller needs to free pData when they are done uploading to the gpu.
    // note: may want to have a dedicated method for that or put it into FFLiDeleteTexture?
}
