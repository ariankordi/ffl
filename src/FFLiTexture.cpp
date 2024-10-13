#include <nn/ffl/FFLiResourceHeader.h>
#include <nn/ffl/FFLiResourceLoader.h>
#include <nn/ffl/FFLiResourceManager.h>
#include <nn/ffl/FFLiTexture.h>

#if RIO_IS_WIN
    #ifndef FFL_NO_NINTEXUTILS
        #include <ninTexUtils/gx2/gx2Surface.h>
    #endif
    #include <gpu/win/rio_Texture2DUtilWin.h>
#elif RIO_IS_CAFE
#define numMips mipLevels
#define imagePtr image
#define mipPtr mipmaps
#define mipOffset mipLevelOffset
#define GX2_SURFACE_DIM_2D GX2_SURFACE_DIM_TEXTURE_2D
#define GX2_AA_MODE_1X GX2_AA_MODE1X
#endif // RIO_IS_CAFE

#define TEXTURE_DATA_MAX_ALIGNMENT   (0x800)

FFLResult FFLiLoadTextureWithAllocate(rio::Texture2D** ppTexture2D, FFLiTexturePartsType partsType, u32 index, FFLiResourceLoader* pResLoader)
{
    u32 size = pResLoader->GetTextureAlignedMaxSize(partsType);
    void* pData;

    if (!pResLoader->IsExpand())
    {
        pData = rio::MemUtil::alloc(size, TEXTURE_DATA_MAX_ALIGNMENT);
        FFLResult result = pResLoader->LoadTexture(pData, &size, partsType, index);
        if (result != FFL_RESULT_OK)
        {
            rio::MemUtil::free(pData);
            return result;
        }
    }
    else
    {
        pData = NULL;
        FFLResult result = pResLoader->GetPointerTextureByExpandCache(&pData, &size, partsType, index);
        if (result != FFL_RESULT_OK)
            return result;
    }

    const FFLiResourceTextureFooter& footer = FFLiResourceTextureFooter::GetFooterImpl(pData, size);

    rio::NativeTexture2D texture;

    FFLiResourceHeader* pHeader = pResLoader->Header();
    bool textureFormatIsLinear = pHeader->TextureFormatIsLinear();

#if RIO_IS_WIN
    #ifndef FFL_NO_NINTEXUTILS
        GX2Surface surface;
    #else
        // with no ninTexUtils, we cannot deswizzle
        // the texture at all so this guarantees it is linear
        if (!textureFormatIsLinear)
        {
            rio::MemUtil::free(pData);
            // not sure what to do here
            RIO_ASSERT(false && "This was built with FFL_NO_NINTEXUTILS enabled, so you can only use resources with linear textures, such as AFLResHigh.dat, etc.");
            return FFL_RESULT_ERROR;
        }
    #endif // FFL_NO_NINTEXUTILS
#else
    GX2Surface& surface = texture.surface;
#endif // RIO_IS_WIN

    #if RIO_IS_WIN

        // if the texture format is linear then...
        // ... just make a rio texture directly
        // this is the path that should always be taken
        // if there is no ninTexUtils
        #ifndef FFL_NO_NINTEXUTILS
        if (textureFormatIsLinear)
        {
        #endif
            texture.surface.width = footer.Width();
            texture.surface.height = footer.Height();
            if (pHeader->IgnoreMipMaps())
                texture.surface.mipLevels = 0;
            else
                texture.surface.mipLevels = footer.NumMips();
            texture.surface.format = static_cast<rio::TextureFormat>(footer.SurfaceFormat());

            void* imagePtr = footer.GetImagePtrImpl(size);
            RIO_ASSERT(imagePtr == pData);

            texture.surface.image = imagePtr;
            // TODO: mipmaps not supported for linear textures right now
            texture.surface.mipmaps = nullptr;

            texture.compMap = rio::TextureFormatUtil::getDefaultCompMap(texture.surface.format);

            // Initialize nativeFormat based on rio::TextureFormat
            bool success = rio::TextureFormatUtil::getNativeTextureFormat(texture.surface.nativeFormat, texture.surface.format);
            RIO_ASSERT(success);

            texture.surface.imageSize = rio::Texture2DUtil::calcImageSize(texture.surface.format, texture.surface.width, texture.surface.height);
            texture.surface.mipmapSize = 0; //rio::Texture2DUtil::calcMipmapSize(texture.surface.format, texture.surface.width, texture.surface.height, texture.surface.mipLevels, reinterpret_cast<u32*>(texture.surface.mipmaps));

            texture.surface.mipLevelOffset[0] = 0; // NOTE: no mipmaps

            {
                texture._footer.magic = 0x5101382D;
                texture._footer.version = 0x01000000;
            }
        #ifndef FFL_NO_NINTEXUTILS
        } else
        {
            surface.dim = GX2_SURFACE_DIM_2D;
            surface.width = footer.Width();
            surface.height = footer.Height();
            surface.depth = 1;
            if (pHeader->IgnoreMipMaps())
                surface.numMips = 1;
            else
                surface.numMips = footer.NumMips();
            surface.format = footer.SurfaceFormat();
            surface.aa = GX2_AA_MODE_1X;
            surface.use = GX2_SURFACE_USE_TEXTURE;
            surface.tileMode = GX2_TILE_MODE_DEFAULT;
            surface.swizzle = 0;

            GX2CalcSurfaceSizeAndAlignment(&surface);

            void* imagePtr = footer.GetImagePtrImpl(size);
            RIO_ASSERT(imagePtr == pData);

            void* mipPtr = footer.GetMipPtrImpl(size);
            if (mipPtr == nullptr && surface.numMips > 1)
                mipPtr = (void*)((uintptr_t)imagePtr + surface.mipOffset[0]);

            surface.imagePtr = imagePtr;
            surface.mipPtr = mipPtr;

            GX2Surface linearSurface;
            linearSurface.dim = GX2_SURFACE_DIM_2D;
            linearSurface.width = footer.Width();
            linearSurface.height = footer.Height();
            linearSurface.depth = 1;
            linearSurface.numMips = surface.numMips;
            linearSurface.format = footer.SurfaceFormat();
            linearSurface.aa = GX2_AA_MODE_1X;
            linearSurface.use = GX2_SURFACE_USE_TEXTURE;
            linearSurface.tileMode = GX2_TILE_MODE_LINEAR_SPECIAL;
            linearSurface.swizzle = 0;

            texture.surface.format = rio::TextureFormat(linearSurface.format);

            GX2CalcSurfaceSizeAndAlignment(&linearSurface);

            linearSurface.imagePtr = new u8[linearSurface.imageSize];
            if (linearSurface.mipSize > 0)
                linearSurface.mipPtr = new u8[linearSurface.mipSize];
            else
                linearSurface.mipPtr = nullptr;

            GX2CopySurface(&surface, 0, 0, &linearSurface, 0, 0);

            for (u32 i = 1; i < linearSurface.numMips; i++)
                GX2CopySurface(&surface, i, 0, &linearSurface, i, 0);

            if (!pResLoader->IsExpand())
                rio::MemUtil::free(pData);

            texture.surface.width = linearSurface.width;
            texture.surface.height = linearSurface.height;
            texture.surface.mipLevels = linearSurface.numMips;

            [[maybe_unused]] bool success = rio::TextureFormatUtil::getNativeTextureFormat(texture.surface.nativeFormat, texture.surface.format);
            RIO_ASSERT(success);

            texture.surface.imageSize = linearSurface.imageSize;
            texture.surface.mipmapSize = linearSurface.mipSize;
            texture.surface.mipLevelOffset[0] = 0;
            rio::MemUtil::copy(&texture.surface.mipLevelOffset[1], &linearSurface.mipOffset[1], sizeof(u32) * (13 - 1));
            texture.surface.image = linearSurface.imagePtr;
            texture.surface.mipmaps = linearSurface.mipPtr;

            texture.compMap = rio::TextureFormatUtil::getDefaultCompMap(texture.surface.format);

            {
                texture._footer.magic = 0x5101382D;
                texture._footer.version = 0x01000000;
            }
        }
        #endif // FFL_NO_NINTEXUTILS
    #else
        surface.dim = GX2_SURFACE_DIM_2D;
        surface.width = footer.Width();
        surface.height = footer.Height();
        surface.depth = 1;
        if (pHeader->IgnoreMipMaps())
            surface.numMips = 1;
        else
            surface.numMips = footer.NumMips();
        surface.format = footer.SurfaceFormat();
        surface.aa = GX2_AA_MODE_1X;
        surface.use = GX2_SURFACE_USE_TEXTURE;
        surface.tileMode = GX2_TILE_MODE_DEFAULT;
        if (textureFormatIsLinear)
            surface.tileMode = GX2_TILE_MODE_LINEAR_SPECIAL;
        surface.swizzle = 0;

        GX2CalcSurfaceSizeAndAlignment(&surface);

        void* imagePtr = footer.GetImagePtrImpl(size);
        RIO_ASSERT(imagePtr == pData);

        void* mipPtr = footer.GetMipPtrImpl(size);
        if (mipPtr == nullptr && surface.numMips > 1)
            mipPtr = (void*)((uintptr_t)imagePtr + surface.mipOffset[0]);

        surface.imagePtr = imagePtr;
        surface.mipPtr = mipPtr;

        texture.viewFirstMip = 0;
        texture.viewNumMips = surface.numMips;
        texture.viewFirstSlice = 0;
        texture.viewNumSlices = surface.depth;
        texture.compMap = rio::TextureFormatUtil::getDefaultCompMap(static_cast<rio::TextureFormat>(surface.format));
        GX2InitTextureRegs(&texture);
    #endif // RIO_IS_WIN

    *ppTexture2D = new rio::Texture2D(texture);

    return FFL_RESULT_OK;
}


void FFLiDeleteTexture(rio::Texture2D** ppTexture2D, bool isExpand)
{
    rio::Texture2D*& pTexture2D = *ppTexture2D;

#if RIO_IS_WIN
    u8* imagePtr = (u8*)pTexture2D->getNativeTexture().surface.image;
    u8* mipPtr = (u8*)pTexture2D->getNativeTexture().surface.mipmaps;

    delete[] imagePtr;
    if (pTexture2D->getNativeTexture().surface.mipmapSize > 0)
        delete[] mipPtr;
#else
    if (!isExpand)
    {
        void* pData = pTexture2D->getNativeTexture().surface.image;
        rio::MemUtil::free(pData);
    }
#endif

    delete pTexture2D;
    pTexture2D = nullptr;
}
