#include <nn/ffl/FFLiCreateID.h>
#include <nn/ffl/FFLiRFLCreateID.h>

#include <nn/ffl/detail/FFLiCrc.h>

#include <misc/rio_MemUtil.h>

static inline
FFLiCreateID* GetCreateID(FFLCreateID* pCreateID)
{
    return reinterpret_cast<FFLiCreateID*>(pCreateID);
}

static inline
const FFLiCreateID* GetCreateID(const FFLCreateID* pCreateID)
{
    return reinterpret_cast<const FFLiCreateID*>(pCreateID);
}

bool FFLiIsNullMiiID(const FFLCreateID* pCreateID)
{
    if (pCreateID != NULL)
    {
        [[maybe_unused]] const u32 count = FFL_CREATE_ID_SIZE / sizeof(u16);
        NN_STATIC_ASSERT(count == 5);

        if (pCreateID->value16[0] == 0 &&
            pCreateID->value16[1] == 0 &&
            pCreateID->value16[2] == 0 &&
            pCreateID->value16[3] == 0 &&
            pCreateID->value16[4] == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}

void FFLiGetTemporaryMiiID(FFLCreateID* pCreateID)
{
    static const FFLiCreateID temporaryMiiID = {
        FFLI_CREATE_ID_BIT_TEMPORARY,
        0,
        0,
        0,
        { 0 }
    };

    *GetCreateID(pCreateID) = temporaryMiiID;
}

bool FFLiIsNormalMiiID(const FFLCreateID* pCreateID)
{
    u8 flags = GetCreateID(pCreateID)->flags;
    return (flags & FFLI_CREATE_ID_BIT_NORMAL) != 0;
}

bool FFLiIsSpecialMiiID(const FFLCreateID* pCreateID)
{
    return !FFLiIsNormalMiiID(pCreateID);
}

bool FFLiIsWiiMiiID(const FFLCreateID* pCreateID)
{
    u8 flags = GetCreateID(pCreateID)->flags;
    return (flags & FFLI_CREATE_ID_BIT_CTR) == 0 && (flags & FFLI_CREATE_ID_BIT_NTR) == 0;
}

bool FFLiIsNTRMiiID(const FFLCreateID* pCreateID)
{
    u8 flags = GetCreateID(pCreateID)->flags;
    return (flags & FFLI_CREATE_ID_BIT_CTR) == 0 && (flags & FFLI_CREATE_ID_BIT_NTR) != 0;
}

bool FFLiIsCTRMiiID(const FFLCreateID* pCreateID)
{
    u8 flags = GetCreateID(pCreateID)->flags;
    return (flags & FFLI_CREATE_ID_BIT_CTR) != 0 && (flags & FFLI_CREATE_ID_BIT_NTR) == 0;
}

bool FFLiIsWiiUMiiID(const FFLCreateID* pCreateID)
{
    u8 flags = GetCreateID(pCreateID)->flags;
    return (flags & FFLI_CREATE_ID_BIT_CTR) != 0 && (flags & FFLI_CREATE_ID_BIT_NTR) != 0;
}


bool FFLiIsTemporaryMiiID(const FFLCreateID* pCreateID)
{
    u8 flags = GetCreateID(pCreateID)->flags;
    return (flags & FFLI_CREATE_ID_BIT_TEMPORARY) != 0;
}

bool FFLiIsValidMiiID(const FFLCreateID* pCreateID)
{
    return !FFLiIsNullMiiID(pCreateID) && !FFLiIsTemporaryMiiID(pCreateID);
}

bool FFLiIsSameMiiID(const FFLCreateID* a, const FFLCreateID* b)
{
    if (FFLiIsValidMiiID(a) && FFLiIsValidMiiID(b))
    {
        [[maybe_unused]] const u32 count = FFL_CREATE_ID_SIZE / sizeof(u16);
        NN_STATIC_ASSERT(count == 5);

        if (a->value16[0] == b->value16[0] &&
            a->value16[1] == b->value16[1] &&
            a->value16[2] == b->value16[2] &&
            a->value16[3] == b->value16[3] &&
            a->value16[4] == b->value16[4])
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}

void FFLiGetDefaultCreateIDonCTR(FFLCreateID* pCreateID, s32 index)
{
    const u8 DEFAULT_CREATE_ID_ON_CTR[FFL_CREATE_ID_SIZE] = {
        0x80, 0x00, 0x00, 0x00, 0xec,
        0xff, 0x82, 0xd2, 0x00, 0x00
        // CFLi_GetDefaultCreateIDonCTR just sets 0x80, index, and:
        // *(undefined4 *)((int)&createID->field0_0x0 + 4) = 0xd282ffec;
    };

    if (index < 6)
    {
        rio::MemUtil::copy(pCreateID, &DEFAULT_CREATE_ID_ON_CTR, sizeof(DEFAULT_CREATE_ID_ON_CTR));
        reinterpret_cast<FFLiCreateID*>(&pCreateID)->databaseIndex = index;
    }
}



bool FFLiRFLCreateID::Convert(FFLCreateID* pCreateID, const FFLiAuthorID* pAuthorID) const
{
    if (!IsValidOnNand())
        return false;

    if (pCreateID == NULL || pAuthorID == NULL)
        return false;

    if (FFLiIsNullAuthorID(pAuthorID))
        return false;

    u16 crc = FFLiGetCRC16(0x7f03, &pAuthorID, FFLI_RFL_CREATE_ID_SIZE);

    rio::MemUtil::copy(pCreateID, this, 8);

    // Put CRC here..???
    pCreateID->value16[4] = crc;
    //pCreateID->data[8] = (u8)(crc >> 8);
    //pCreateID->data[9] = (u8)crc;

    // Clear temporary flag if it is there (0b11101111 / 0xef)
    pCreateID->data[0] &= ~FFLI_CREATE_ID_BIT_TEMPORARY;
    return true;

}

bool FFLiRFLCreateID::IsValid() const
{
    // Basically checks if any byte is non-null
    for (s32 i = 0; i < FFLI_RFL_CREATE_ID_SIZE; i++)
        if (this->data[i] != '\0')
            return true;

    return false;
}

bool FFLiRFLCreateID::IsValidOnNand() const
{
    if (IsValid() && !IsTemporary())
        return true;

    return false;
}

bool FFLiRFLCreateID::IsTemporary() const
{
    RIO_ASSERT(IsValid());
    return (this->data[0] & FFLI_CREATE_ID_BIT_TEMPORARY) != 0;
    //return this->data[0] >> 5 & 1;
}

bool FFLiRFLCreateID::IsNormal() const
{
    RIO_ASSERT(IsValid());
    return (this->data[0] & FFLI_CREATE_ID_BIT_NORMAL) != 0;
    //return this->data[0] >> 7;
}

bool FFLiRFLCreateID::IsSpecial() const
{
    return !IsNormal();
}

bool FFLiRFLCreateID::IsDs() const
{
    RIO_ASSERT(IsValid());
    return (this->data[0] & FFLI_CREATE_ID_BIT_CTR) == 0 && (this->data[0] & FFLI_CREATE_ID_BIT_NTR) != 0;
    //return this->data[0] >> 6 & 1;
}

bool FFLiRFLCreateID::IsWii() const
{
    return !IsDs();
}
