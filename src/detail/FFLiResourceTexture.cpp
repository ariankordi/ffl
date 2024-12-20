#include <nn/ffl/FFLiSwapEndian.h>

#include <nn/ffl/detail/FFLiResourceTexture.h>

#if RIO_IS_CAFE
#define GX2_SURFACE_FORMAT_TC_R8_UNORM              GX2_SURFACE_FORMAT_UNORM_R8
#define GX2_SURFACE_FORMAT_TC_R8_G8_UNORM           GX2_SURFACE_FORMAT_UNORM_R8_G8
#define GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM    GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8
#elif RIO_IS_WIN
#define GX2_SURFACE_FORMAT_TC_R8_UNORM              GX2_SURFACE_FORMAT_UNORM_R8
#define GX2_SURFACE_FORMAT_TC_R8_G8_UNORM           GX2_SURFACE_FORMAT_UNORM_RG8
#define GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM    GX2_SURFACE_FORMAT_UNORM_RGBA8
#endif

FFLiSurfaceFormat FFLiGetResourceSurfaceFormat(FFLTextureFormat textureFormat)
{
    switch (textureFormat)
    {
#ifdef FFL_NO_NINTEXUTILS
    // use rio texture formats here
    case FFL_TEXTURE_FORMAT_R8_UNORM:
        return rio::TEXTURE_FORMAT_R8_UNORM;
    case FFL_TEXTURE_FORMAT_R8_G8_UNORM:
        return rio::TEXTURE_FORMAT_R8_G8_UNORM;
    case FFL_TEXTURE_FORMAT_R8_G8_B8_A8_UNORM:
        return rio::TEXTURE_FORMAT_R8_G8_B8_A8_UNORM;
    default:
        RIO_ASSERT(false);
        return rio::TEXTURE_FORMAT_R8_G8_B8_A8_UNORM;
    // compressed texture formats can go here later maybe
    }
    return rio::TEXTURE_FORMAT_R8_UNORM;
#else
    case FFL_TEXTURE_FORMAT_R8_UNORM:
        return GX2_SURFACE_FORMAT_TC_R8_UNORM;
    case FFL_TEXTURE_FORMAT_R8_G8_UNORM:
        return GX2_SURFACE_FORMAT_TC_R8_G8_UNORM;
    case FFL_TEXTURE_FORMAT_R8_G8_B8_A8_UNORM:
        return GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM;
    default:
        RIO_ASSERT(false);
        return GX2_SURFACE_FORMAT_TCS_R8_G8_B8_A8_UNORM;
    }
    return GX2_SURFACE_FORMAT_TC_R8_UNORM;
#endif
}

FFLiSurfaceFormat FFLiResourceTextureFooter::SurfaceFormat() const
{
    return FFLiGetResourceSurfaceFormat(FFLTextureFormat(m_TextureFormat));
}

FFLiResourceTextureFooter& FFLiResourceTextureFooter::GetFooterImpl(const void* pData, u32 size)
{
    return ((FFLiResourceTextureFooter*)((u8*)pData + size))[-1];
}

void* FFLiResourceTextureFooter::GetImagePtrImpl(u32 size) const
{
    return (u8*)(this + 1) - size;
}

void* FFLiResourceTextureFooter::GetMipPtrImpl(u32 size) const
{
    return m_MipOffset != 0 ? ((u8*)(this + 1) - size + m_MipOffset) : NULL;
}

void FFLiResourceTextureFooter::SwapEndian()
{
    // This function is deleted in NSMBU.
    // Therefore, its implementation is only theoretical.

    m_MipOffset = FFLiSwapEndianImpl<u32>(m_MipOffset);
    m_Width = FFLiSwapEndianImpl<u16>(m_Width);
    m_Height = FFLiSwapEndianImpl<u16>(m_Height);
}
