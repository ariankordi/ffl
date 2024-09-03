#ifndef FFLI_RESOURCE_HEADER_H_
#define FFLI_RESOURCE_HEADER_H_

#include <nn/ffl/detail/FFLiResourceTexture.h>
#include <nn/ffl/FFLResourceType.h>
#include <nn/ffl/FFLResult.h>

#include <nn/ffl/FFLiSwapEndian.h>

#include <nn/ffl/FFLiShapePartsType.h>
#include <nn/ffl/FFLiTexturePartsType.h>

#include <nn/ffl/detail/FFLiResourceShape.h>
#include <nn/ffl/detail/FFLiResourceTexture.h>

struct  FFLiResourcePartsInfo;
struct  FFLiResourceShapeHeader;
class   FFLiResourceTextureHeader;
enum    FFLiResourceWindowBits;

u32 FFLiGetTextureResourceNum(FFLiTexturePartsType partsType);
u32 FFLiGetShapeResourceNum(FFLiShapePartsType partsType);

class FFLiResourceHeader;

FFLiResourcePartsInfo* FFLiGetTextureResoucePartsInfos(u32* pNum, FFLiResourceHeader* pHeader, FFLiTexturePartsType partsType);
FFLiResourcePartsInfo* FFLiGetShapeResoucePartsInfos(u32* pNum, FFLiResourceShapeHeader* pHeader, FFLiShapePartsType partsType);

s32 FFLiResourceWindowBitsToZlibWindowBits(FFLiResourceWindowBits windowBits);


template <typename T>
void HeaderSwapEndianImpl(T* pHeader);

// --------------------------------------------------------------------------

#define FFLI_RESOURCE_HEADER_VERSION    (0x00070000)

// Abstract type
class FFLiResourceHeader
{
public:
    virtual FFLResult GetResult() const;
    virtual u32 GetMagic() const = 0;
    virtual u32 GetVersion() const = 0;
    virtual u32 GetUncompressBufferSize() const = 0;
    virtual bool IsExpand() const = 0;
    //virtual FFLiResourceTextureHeader* GetTextureHeader() = 0;
    virtual u32 GetTextureMaxSize(FFLiTexturePartsType partsType) const = 0;

    virtual u32 GetTextureResourceNum(FFLiTexturePartsType partsType) const = 0;
    virtual FFLiResourcePartsInfo* GetTextureResourcePartsInfos(u32* pNum, FFLiTexturePartsType partsType) = 0;

    virtual FFLiResourceShapeHeader* GetShapeHeader() = 0;
    virtual u32 GetShapeMaxSize(FFLiShapePartsType partsType) const = 0;
    virtual void SwapEndian() = 0;
    //virtual void SwapEndian() { return; }

    virtual void SetHeader(void* ptr) = 0;
    virtual char* GetHeaderRaw() const = 0;
    virtual u32 GetHeaderSize() const = 0;

    // NOTE: these are here bc miitomo format
    // has linear textures and its mipmaps are specifically unused
    virtual bool TextureFormatIsLinear() const = 0;
    virtual bool IgnoreMipMaps() const = 0;

    /* TODO:
     * add ABSTRACT TEXTURE HEADER type here
     * delete ABSTRACT HEADER (if it exists?) ALSO HERE
     */
    virtual ~FFLiResourceHeader() {}

    bool m_NeedsEndianSwap;
};

// casted to this to identify header type
class FFLiResourceHeaderDefaultData
{
public:
    // NOTE: ONLY SWAPS ENDIAN PRE-TEXTURE/SHAPE HEADERS
    void SwapEndian()
    {
        m_Magic = FFLiSwapEndianImpl<u32>(m_Magic);
        m_Version = FFLiSwapEndianImpl<u32>(m_Version);
        m_UncompressBufferSize = FFLiSwapEndianImpl<u32>(m_UncompressBufferSize);
        m_TotalUncompressSize = FFLiSwapEndianImpl<u32>(m_TotalUncompressSize);
        m_IsExpand = FFLiSwapEndianImpl<u32>(m_IsExpand);
    }

    u32                         m_Magic;
    u32                         m_Version;                  // I think? Always 0x00070000
    u32                         m_UncompressBufferSize;
    u32                         m_TotalUncompressSize;      // _c, unused
    int                         m_IsExpand;                 // Must be 1 specifically to be true, not just non-zero
    FFLiResourceTextureHeader   m_TextureHeader;
    FFLiResourceShapeHeader     m_ShapeHeader;
    u32                         _49d0[48 / sizeof(u32)];    // Unused AND zero in files
};

class FFLiResourceHeaderDefault : public FFLiResourceHeader
{
public:
    u32 GetMagic() const override { return m_Header->m_Magic; }
    u32 GetVersion() const override { return m_Header->m_Version; }
    u32 GetUncompressBufferSize() const override
    {
        return m_Header->m_UncompressBufferSize;
    }
    bool IsExpand() const override
    {
        return m_Header->m_IsExpand == 1;
    }
    /*FFLiResourceTextureHeader* GetTextureHeader() override
    {
        return &m_Header->m_TextureHeader;
    }*/
    u32 GetTextureMaxSize(FFLiTexturePartsType partsType) const override
    {
        return m_Header->m_TextureHeader.partsMaxSize[partsType];
    }

    u32 GetTextureResourceNum(FFLiTexturePartsType partsType) const override;

    FFLiResourcePartsInfo* GetTextureResourcePartsInfos(u32* pNum, FFLiTexturePartsType partsType) override
    {
        *pNum = GetTextureResourceNum(partsType);
        return m_Header->m_TextureHeader.GetPartsInfos(partsType);
    }

    FFLiResourceShapeHeader* GetShapeHeader() override
    {
        return &m_Header->m_ShapeHeader;
    }
    u32 GetShapeMaxSize(FFLiShapePartsType partsType) const override
    {
        return m_Header->m_ShapeHeader.partsMaxSize[partsType];
    }

    void SwapEndian() override;

    void SetHeader(void* ptr) override
    {
        m_Header = reinterpret_cast<decltype(m_Header)>(ptr);
    }
    char* GetHeaderRaw() const override
    {
        return reinterpret_cast<char*>(m_Header);
    }
    u32 GetHeaderSize() const override
    {
        return sizeof(decltype(*m_Header));
    }

    bool TextureFormatIsLinear() const override { return false; }
    bool IgnoreMipMaps() const override { return false; }

private:
    // header will be, hot loaded?? into this
    FFLiResourceHeaderDefaultData* m_Header;

    template <typename T>
    friend void HeaderSwapEndianImpl(T* header);
};

//NN_STATIC_ASSERT(sizeof(BOOL) == 4);
//NN_STATIC_ASSERT_IS_POD(FFLiResourceHeader);
//NN_STATIC_ASSERT(sizeof(FFLiResourceHeader) == 0x4A00);

struct FFLiResourceMultiHeader
{
    FFLiResourceHeader*  header[FFL_RESOURCE_TYPE_MAX];
};
//NN_STATIC_ASSERT(sizeof(FFLiResourceMultiHeader) == 0x9400);


// more header types
class FFLiResourceHeaderAFL_2_3 : public FFLiResourceHeader
{
public:
    u32 GetMagic() const override { return m_Header->m_Magic; }
    u32 GetVersion() const override { return m_Header->m_Version; }
    u32 GetUncompressBufferSize() const override
    {
        return m_Header->m_UncompressBufferSize;
    }
    bool IsExpand() const override
    {
        return m_Header->m_IsExpand == 1;
    }
    /*FFLiResourceTextureHeader* GetTextureHeader() override
    {
        return &m_Header->m_TextureHeader;
    }*/
    u32 GetTextureMaxSize(FFLiTexturePartsType partsType) const override
    {
        return m_Header->m_TextureHeader.partsMaxSize[partsType];
    }

    u32 GetTextureResourceNum(FFLiTexturePartsType partsType) const override;

    FFLiResourcePartsInfo* GetTextureResourcePartsInfos(u32* pNum, FFLiTexturePartsType partsType) override
    {
        *pNum = GetTextureResourceNum(partsType);
        return m_Header->m_TextureHeader.GetPartsInfos(partsType);
    }

    FFLiResourceShapeHeader* GetShapeHeader() override
    {
        return &m_Header->m_ShapeHeader;
    }
    u32 GetShapeMaxSize(FFLiShapePartsType partsType) const override
    {
        return m_Header->m_ShapeHeader.partsMaxSize[partsType];
    }

    void SetHeader(void* ptr) override
    {
        m_Header = reinterpret_cast<decltype(m_Header)>(ptr);
    }
    char* GetHeaderRaw() const override
    {
        return reinterpret_cast<char*>(m_Header);
    }
    u32 GetHeaderSize() const override
    {
        return sizeof(decltype(*m_Header));
    }

    bool TextureFormatIsLinear() const override { return true; }
    bool IgnoreMipMaps() const override { return true; }

    void SwapEndian() override;

private:
    struct {
        u32                              m_Magic;
        u32                              m_Version; // STILL 0x00070000
        u32                              m_UncompressBufferSize;
        u32                              _c[4 / sizeof(u32)];
        int                              m_IsExpand;
        FFLiResourceTextureHeaderAFL_2_3 m_TextureHeader;
        FFLiResourceShapeHeader          m_ShapeHeader;
        u32                              _49d0[48 / sizeof(u32)];
    } *m_Header;

    template <typename T>
    friend void HeaderSwapEndianImpl(T* header);
};

class FFLiResourceHeaderAFL : public FFLiResourceHeader
{
public:
    u32 GetMagic() const override { return m_Header->m_Magic; }
    u32 GetVersion() const override { return m_Header->m_Version; }
    u32 GetUncompressBufferSize() const override
    {
        return m_Header->m_UncompressBufferSize;
    }
    bool IsExpand() const override
    {
        return m_Header->m_IsExpand == 1;
    }
    u32 GetTextureMaxSize(FFLiTexturePartsType partsType) const override
    {
        return m_Header->m_TextureHeader.partsMaxSize[partsType];
    }

    u32 GetTextureResourceNum(FFLiTexturePartsType partsType) const override;

    FFLiResourcePartsInfo* GetTextureResourcePartsInfos(u32* pNum, FFLiTexturePartsType partsType) override
    {
        *pNum = GetTextureResourceNum(partsType);
        return m_Header->m_TextureHeader.GetPartsInfos(partsType);
    }

    FFLiResourceShapeHeader* GetShapeHeader() override
    {
        return &m_Header->m_ShapeHeader;
    }
    u32 GetShapeMaxSize(FFLiShapePartsType partsType) const override
    {
        return m_Header->m_ShapeHeader.partsMaxSize[partsType];
    }

    void SetHeader(void* ptr) override
    {
        m_Header = reinterpret_cast<decltype(m_Header)>(ptr);
    }
    char* GetHeaderRaw() const override
    {
        return reinterpret_cast<char*>(m_Header);
    }
    u32 GetHeaderSize() const override
    {
        return sizeof(decltype(*m_Header));
    }

    bool TextureFormatIsLinear() const override { return true; }
    bool IgnoreMipMaps() const override { return true; }

    void SwapEndian() override;

private:
    struct {
        u32                           m_Magic;
        u32                           m_Version; // STILL 0x00070000
        u32                           m_UncompressBufferSize;
        u32                           _c[4 / sizeof(u32)];
        int                           m_IsExpand;
        FFLiResourceTextureHeaderAFL  m_TextureHeader;
        FFLiResourceShapeHeader       m_ShapeHeader;
        u32                           _49d0[48 / sizeof(u32)];
    } *m_Header;

    template <typename T>
    friend void HeaderSwapEndianImpl(T* header);
};
#endif // FFLI_RESOURCE_HEADER_H_
