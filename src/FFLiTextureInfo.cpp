#include <nn/ffl/FFLiTextureInfo.h>

#include <nn/ffl/FFLiResourceHeader.h>
#include <nn/ffl/FFLiResourceLoader.h>

namespace {
    // replacement for rio::TextureFormatUtil::getPixelByteSize
    static const u32 GetPixelByteSize(FFLTextureFormat format)
    {
        static const u32
            bytesPerPixelMapping[FFL_TEXTURE_FORMAT_MAX] = {
            1, // FFL_TEXTURE_FORMAT_R8_UNORM
            2, // FFL_TEXTURE_FORMAT_R8_G8_UNORM
            4, // FFL_TEXTURE_FORMAT_R8_G8_B8_A8_UNORM
        };

        RIO_ASSERT(format < FFL_TEXTURE_FORMAT_MAX && "undefined texture format?");

        return bytesPerPixelMapping[format]; // resolve from table
    }

    // replacement for rio::Texture2DUtil::calcImageSize
    static const u32 CalcImageSize(FFLTextureFormat format, u32 width, u32 height)
    {
        u32 bytesPerPixel = GetPixelByteSize(format);

        return width * height * bytesPerPixel;
    }

    // rio::Texture2DUtil::calcMipmapSize
    static const u32 CalcMipmapSize(FFLTextureFormat format, u32 width, u32 height,
                                    u32 mipLevels, u32* mipLevelOffset)
    {
        if (mipLevelOffset != nullptr)
            // Initialize mipLevelOffset with zeroes.
            rio::MemUtil::set(mipLevelOffset, 0, sizeof(u32) * 13);

        // Maximum mipmaps of 14.
        mipLevels = std::min(std::max(mipLevels, 1u), 14u);

        u32 bytesPerPixel = GetPixelByteSize(format);

        u32 mipmapSize = 0;

        for (u32 i = 1; i < mipLevels; i++)
        {
            if (mipLevelOffset)
                mipLevelOffset[i - 1] = mipmapSize;

            u32 mipWidth  = std::max(width  >> i, 1u);
            u32 mipHeight = std::max(height >> i, 1u);

            // NOTE: Does not account for compressed textures at all.

            mipmapSize += mipWidth * mipHeight * bytesPerPixel;
        }

        return mipmapSize;
    }
}

#define TEXTURE_DATA_MAX_ALIGNMENT (0x800)

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
        *pData = rio::MemUtil::alloc(size, TEXTURE_DATA_MAX_ALIGNMENT);
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
