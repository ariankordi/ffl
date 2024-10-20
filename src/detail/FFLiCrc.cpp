#include <nn/ffl/FFLiSwapEndian.h>

#include <nn/ffl/detail/FFLiCrc.h>

u16 FFLiGetCRC16(const void* pData, u32 size)
{
    u16 crc = 0;
    for (u32 i = 0; i < size; i++)
    {
        for (s32 j = 0; j < 8; j++)
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        }
        crc = crc ^ *(u8*)pData;
        pData = (u8*)pData + 1;
    }
    return crc;
}

void FFLiPutCRC16(void* pData, u32 size)
{
    ((u16*)((u8*)pData + size))[-1] = 0;
    u16 crc = FFLiGetCRC16(pData, size);
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    crc = FFLiSwapEndianImpl<u16>(crc);
#endif // __BYTE_ORDER__
    ((u16*)((u8*)pData + size))[-1] = crc;
}

bool FFLiIsValidCRC16(const void* pData, u32 size)
{
    return FFLiGetCRC16(pData, size) == 0;
}
