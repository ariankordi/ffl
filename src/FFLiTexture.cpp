#include <nn/ffl/FFLiResourceHeader.h>
#include <nn/ffl/FFLiResourceLoader.h>
#include <nn/ffl/FFLiTexture.h>

#include <nn/ffl/FFLiTextureInfo.h>

#if RIO_IS_WIN
    #ifndef FFL_NO_NINTEXUTILS
        #include <ninTexUtils/gx2/gx2Surface.h>
    #endif
    #include <gpu/win/rio_Texture2DUtilWin.h>
#endif // RIO_IS_WIN

#ifdef FFL_USE_TEXTURE_CALLBACK
#include <nn/ffl/FFLTextureCallback.h>
#include <nn/ffl/FFLiManager.h>
#include <nn/ffl/FFLiTextureCallback.h>
#endif // FFL_USE_TEXTURE_CALLBACK

namespace
{
#ifndef FFL_NO_NINTEXUTILS
    static GX2Surface CreateGX2Surface(FFLTextureInfo textureInfo);
    #if !RIO_IS_CAFE
        static void ConvertGX2TiledToLinear(void** outImagePtr, void** outMipPtr, FFLTextureInfo textureInfo);
    #endif // RIO_IS_CAFE
#endif // FFL_NO_NINTEXUTILS
}

FFLResult FFLiLoadTexture(rio::Texture2D** ppTexture2D, FFLiTexturePartsType partsType, u32 index, FFLiResourceLoader* pResLoader);

// overload for FFLiLoadTextureWithAllocate just using the FFLTexture typedef
// that will either use RIO or the texture callback to load in the texture
FFLResult FFLiLoadTextureWithAllocate(FFLTexture** ppTexture, FFLiTexturePartsType partsType, u32 index, FFLiResourceLoader* pResLoader)
{
#ifdef FFL_USE_TEXTURE_CALLBACK
    const FFLTextureCallback* pCallback = pResLoader->GetTextureCallback();
    // is there a texture callback? in which case, create with that
    if (pCallback && pCallback->pCreateFunc)
    {
        void* pData;
        FFLTextureInfo info;
        FFLResult result = FFLiLoadTextureInfo(&info, partsType, index, pResLoader, &pData);
        if (result != FFL_RESULT_OK)
            return result;

        RIO_ASSERT(info.imagePtr == pData);

        // Convert the texture unless the callback needs the original tile mode.
        bool convertTileMode = info.isGX2Tiled && !pCallback->useOriginalTileMode;
        if (convertTileMode)
        {
            // Convert texture in place.
            ConvertGX2TiledToLinear(&info.imagePtr, &info.mipPtr, info);

            // Free data now.
            if (!pResLoader->IsExpand())
                rio::MemUtil::free(pData);
        }

        // Call the texture callback, giving the callback the texture data.
        pCallback->pCreateFunc(pCallback->pObj, &info, reinterpret_cast<FFLTexture*>(ppTexture));
        // Assuming that it is finished with the texture pointers after this point.

        // Free texture pointers.
        if (convertTileMode)
            // If tile mode conversion happened...
            // ... assume the pointer is in imagePtr.
            rio::MemUtil::free(info.imagePtr);
        else
            if (!pResLoader->IsExpand())
                rio::MemUtil::free(pData);

        return FFL_RESULT_OK; // if this returned by now we're probably good
    }
    // no texture callback was set? needed for creation
    RIO_ASSERT(false && "you set FFL_USE_TEXTURE_CALLBACK but did not set a texture callback....");
    return FFL_RESULT_ERROR;
// #endif // FFL_USE_TEXTURE_CALLBACK
#else
    // call rio::Texture2D overload
    return FFLiLoadTexture(reinterpret_cast<rio::Texture2D**>(ppTexture), partsType, index, pResLoader);
#endif
}

#ifndef FFL_USE_TEXTURE_CALLBACK

// Path to natively upload texture via RIO/GX2
FFLResult FFLiLoadTexture(rio::Texture2D** ppTexture2D, FFLiTexturePartsType partsType, u32 index, FFLiResourceLoader* pResLoader)
{
    FFLTextureInfo textureInfo;
    [[maybe_unused]] void* pData = NULL; // unused
    FFLResult result = FFLiLoadTextureInfo(&textureInfo, partsType, index, pResLoader, &pData);
    if (result != FFL_RESULT_OK)
        return result;

    rio::NativeTexture2D texture;

#if RIO_IS_WIN
#ifdef FFL_NO_NINTEXUTILS
    // with no ninTexUtils, we cannot deswizzle
    // the texture at all so this guarantees it is linear
    if (textureInfo.isGX2Tiled)
    {
        rio::MemUtil::free(pData);
        // not susurfacere what to do here
        RIO_ASSERT(false && "FFL was built with FFL_NO_NINTEXUTILS enabled, and ninTexUtils is needed to load Wii U resources "
                            "such as FFLResHigh.dat, etc. Try locating and loading AFLResHigh(_2_3).dat, or rebuild without FFL_NO_NINTEXUTILS.");
        return FFL_RESULT_ERROR;
    }
#endif // FFL_NO_NINTEXUTILS
#else
    GX2Surface& surface = texture.surface; // Cafe Path
#endif // RIO_IS_WIN

    /* TODO: NEEDS REFACTORING!!!!
     * Win ninTexUtils Path
     * Win Linear/No ninTexUtils Path
     * Cafe Path
     */

#if RIO_IS_WIN

    /*********************************************************
     *                Win Linear/no ninTexUtils              *
     *********************************************************/

    // Initialize the RIO texture.
    texture.surface.width = textureInfo.width;
    texture.surface.height = textureInfo.height;
    texture.surface.mipLevels = textureInfo.mipCount;

    // Get the surface format.
    const FFLTextureFormat textureFormat = static_cast<FFLTextureFormat>(textureInfo.format);
    const FFLiSurfaceFormat surfaceFormat = FFLiGetResourceSurfaceFormat(textureFormat);
    texture.surface.format = static_cast<rio::TextureFormat>(surfaceFormat);

// if the texture format is linear then...
// ... just make a rio texture directly
// this is the path that should always be taken
// if there is no ninTexUtils
#ifndef FFL_NO_NINTEXUTILS
    if (!textureInfo.isGX2Tiled)
    {
#endif

        texture.surface.image = textureInfo.imagePtr;
        texture.surface.mipmaps = textureInfo.mipPtr;

#ifndef FFL_NO_NINTEXUTILS
        /*********************************************************
         *                Win ninTexUtils Deswizzle              *
         *********************************************************/
    }
    else
    {
        // Convert texture from GX2 tile mode, putting the
        // result in image (imagePtr) and mipmaps (mipPtr).
        ConvertGX2TiledToLinear(&texture.surface.image, &texture.surface.mipmaps, textureInfo);

        // Free data now.
        if (!pResLoader->IsExpand())
            rio::MemUtil::free(pData);

    }
#endif // FFL_NO_NINTEXUTILS

    // Run for linear and tiled textures:


    // Initialize rio::NativeTextureFormat based on rio::TextureFormat.
    [[maybe_unused]] bool success = rio::TextureFormatUtil::getNativeTextureFormat(
    texture.surface.nativeFormat, texture.surface.format);
    RIO_ASSERT(success);

    texture.surface.imageSize = textureInfo.imageSize;
    texture.surface.mipmapSize = textureInfo.mipSize;

    /// Copy mipmap offsets.
    texture.surface.mipLevelOffset[0] = 0;
    rio::MemUtil::copy(&texture.surface.mipLevelOffset[1], &textureInfo.mipLevelOffset[1], sizeof(u32) * (13 - 1));

    texture.compMap = rio::TextureFormatUtil::getDefaultCompMap(texture.surface.format);
    {
        texture._footer.magic = 0x5101382D;
        texture._footer.version = 0x01000000;
    }

#else // RIO_IS_CAFE
    /*********************************************************
     *                  Cafe/GX2Surface Init                 *
     *********************************************************/
    // Initialize the surface.
    surface = CreateGX2Surface(textureInfo);

    const void* imagePtr = textureInfo.imagePtr;
    RIO_ASSERT(imagePtr == pData);

    const void* mipPtr = textureInfo.mipPtr;

    surface.image = (void*)imagePtr;
    surface.mipmaps = (void*)mipPtr;

    texture.viewFirstMip = 0;
    texture.viewNumMips = surface.mipLevels;
    texture.viewFirstSlice = 0;
    texture.viewNumSlices = surface.depth;
    texture.compMap = rio::TextureFormatUtil::getDefaultCompMap(static_cast<rio::TextureFormat>(surface.format));
    GX2InitTextureRegs(&texture);
#endif // RIO_IS_WIN

    *ppTexture2D = new rio::Texture2D(texture);

    return FFL_RESULT_OK;
}

#endif // FFL_USE_TEXTURE_CALLBACK

// Delete texture via either texture callback or RIO
void FFLiDeleteTexture(FFLTexture** ppTexture, bool isExpand)
{
#ifdef FFL_USE_TEXTURE_CALLBACK
    // the calling external code can set this on DrawParams by the way
    if (*ppTexture == nullptr)
        return; // operation can be safely skipped

    const FFLTextureCallback* pCallback = FFLiManager::GetInstance()->GetTextureCallback().Get();
    if (pCallback && pCallback->pDeleteFunc)
    {
        pCallback->pDeleteFunc(pCallback->pObj, reinterpret_cast<FFLTexture*>(ppTexture));
        *ppTexture = nullptr;
        return;
    }
    // no draw callback? that is fine
    // because you can explicitly specify one in FFLInitCharModelCPUStepWithCallback
    // ... and then the caller can delete the textures on their own

// #endif // FFL_USE_TEXTURE_CALLBACK
#else

    // RIO path
    rio::Texture2D* pTexture2D = reinterpret_cast<rio::Texture2D*>(*ppTexture);

#if RIO_IS_WIN
    u8* imagePtr = static_cast<u8*>(pTexture2D->getNativeTexture().surface.image);

    delete[] imagePtr;
    //if (pTexture2D->getNativeTexture().surface.mipmapSize > 0)
    //    delete[] (u8*)pTexture2D->getNativeTexture().surface.mipmaps;imagePtr
#else // RIO_IS_CAFE
    if (!isExpand)
    {
        void* pData = pTexture2D->getNativeTexture().surface.image;
        rio::MemUtil::free(pData);
    }
#endif // RIO_IS_WIN

    delete pTexture2D;
    pTexture2D = nullptr;
#endif
}

namespace
{
#ifndef FFL_NO_NINTEXUTILS

    // Creates a GX2Surface from the textureInfo and calls GX2CalcSurfaceSizeAndAlignment.
    // Note that this does not actually set imagePtr or mipPtr
    static GX2Surface CreateGX2Surface(FFLTextureInfo textureInfo)
    {
        GX2Surface surface = {
#if RIO_IS_CAFE
            .dim = GX2_SURFACE_DIM_TEXTURE_2D,
#else
            .dim = GX2_SURFACE_DIM_2D,
#endif // RIO_IS_CAFE

            .width = textureInfo.width,
            .height = textureInfo.height,

            .depth = 1,

#if RIO_IS_CAFE
            .mipLevels = textureInfo.mipCount,
#else
            .numMips = textureInfo.mipCount,
#endif // RIO_IS_CAFE

            // Convert FFL format to FFLiSurfaceFormat -> GX2SurfaceFormat.
            .format = FFLiGetResourceSurfaceFormat(static_cast<FFLTextureFormat>(textureInfo.format)),

#if RIO_IS_CAFE
            .aa = GX2_AA_MODE1X,
#else
            .aa = GX2_AA_MODE_1X,
#endif // RIO_IS_CAFE
            .use = GX2_SURFACE_USE_TEXTURE,

            .tileMode = textureInfo.isGX2Tiled
            ? GX2_TILE_MODE_DEFAULT : GX2_TILE_MODE_LINEAR_SPECIAL,

            .swizzle = 0,

            // Not defined: imagePtr, mipSize, mipPtr,
            // tileMode, alignment, pitch, mipOffset
        };

        GX2CalcSurfaceSizeAndAlignment(&surface);

        return surface;
    }

#if !RIO_IS_CAFE
    // imagePtr, mipPtr: Output pointers. pData: Input image data pointer (may be freed).
    // Return bool: true = you must free the pData.
    static void ConvertGX2TiledToLinear(void** outImagePtr, void** outMipPtr, FFLTextureInfo textureInfo)
    {
        // Copy imagePtr and mipPtr.
        void* imagePtr = textureInfo.imagePtr;
        void* mipPtr = textureInfo.mipPtr;

        // Initialize the source surface.
        GX2Surface surface = CreateGX2Surface(textureInfo);
        // Set source surface's imagePtr and mipPtr.
        surface.imagePtr = imagePtr;
        surface.mipPtr = mipPtr;

        // Copy the source surface.
        GX2Surface linearSurface = surface; // Target surface.
        // Set the target surface tile mode to linear.
        linearSurface.tileMode = GX2_TILE_MODE_LINEAR_SPECIAL;

        GX2CalcSurfaceSizeAndAlignment(&linearSurface);

        // Allocate pointer for image and mipmaps.
        linearSurface.imagePtr = new u8[linearSurface.imageSize
                                       + linearSurface.mipSize];
        if (linearSurface.mipSize > 0)
        {
            linearSurface.mipPtr = (void*)((uintptr_t)(linearSurface.imagePtr)
                                + linearSurface.imageSize);
            //new u8[linearSurface.mipSize];
        }
        else
            linearSurface.mipPtr = nullptr;

        // Perform deswizzling in ninTexUtils.
        GX2CopySurface(&surface, 0, 0, &linearSurface, 0, 0);

        // Walk through deswizzling every mipmap level.
        for (u32 i = 1; i < linearSurface.numMips; i++)
            GX2CopySurface(&surface, i, 0, &linearSurface, i, 0);

        // Set output image and mipmap pointers.
        *outImagePtr = linearSurface.imagePtr;
        *outMipPtr = linearSurface.mipPtr;

        // Caller should free original imagePtr/pData.
    }
#endif // RIO_IS_CAFE

#endif // FFL_NO_NINTEXUTILS

}
