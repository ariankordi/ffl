#include <nn/ffl/FFLiCreateID.h>
#include <nn/ffl/FFLiRFLCreateID.h>

#include <nn/ffl/detail/FFLiCrc.h>

#include <misc/rio_MemUtil.h>

#if RIO_IS_CAFE
#include <nn/act.h>
#endif // RIO_IS_CAFE

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


/*
void FFLiCreateMiiIDonWiiU(FFLCreateID* pCreateID, const FFLiCreateIDBase* pCreateIDBase, u32 date, bool isSpecial)
{
    u8 flag = FFLI_CREATE_ID_BIT_NORMAL; // initialize flag
    u32 dateTmp = date / 2; // date gets divided by two (>> 1)
    if (!isSpecial) // set flag to special
        flag = ~FFLI_CREATE_ID_BIT_NORMAL;

                    // why 25 what... what........
                    // is index even date or like the create id in // 32 bits or smth
    (pCreateID->_).databaseIndex = (u8)dateTmp; // 0-8 bits
    (pCreateID->_)._1 = (u8)(dateTmp >> 16); // 8-16 bits
    //(pCreateID->_).flags = flag | 0b01010000 | (u8)(date >> 25) & 0xf;

    (pCreateID->_).flags = flag | FFLI_CREATE_ID_BIT_NORMAL | FFLI_CREATE_ID_TYPE_WIIU;

                                      // vv 7 bits
    (pCreateID->_).flags |= (u8)(date >> 25) & 0xf; // 0b00001111

    (pCreateID->_)._2 = (u8)(dateTmp >> 8); // 24 bits

    // copy create id base into create id
    rio::MemUtil::copy(&(pCreateID->_).base, pCreateIDBase, sizeof(FFLiCreateIDBase));
}

// NOTE: an exact copy of this function "FFLiSetCreateID" exists too?????
void FFLiiSetCreateID(FFLCreateID* pCreateID, bool isSpecial)
{
    FFLiCreateIDBase* pCreateIDBase = FFLiGetCreateIDBase();

    s32 year, month, day, hour, minute, second;

    FFLiGetNowDateTime(&year, &month, &day, &hour, &minute, &second);
    if (year < 2010)
        year = 2010;

    s32 daysToday = FFLiDateToDays(year, month, day);
    s32 daysMin = FFLiDateToDays(2010, 1, 1);

    FFLiCreateMiiIDonWiiU
            (pCreateID,pCreateIDBase,
                (daysToday - daysMin) * 86400 + (hour * 60 + minute) * 60 + second, isSpecial);
}

void FFLiSetCreateID(FFLCreateID* pCreateID, bool isSpecial)
{ // just a thunk not a copy
    return FFLiiSetCreateID(pCreateID, isSpecial);
}

void FFLiIncrementCreateIDTime(FFLiCreateID* pCreateID)

{
    u32 all = ((u32)pCreateID->_2 << 8 |
            (u32)pCreateID->_1 << 0x10 | (pCreateID->flags & 0xf) << 0x18 |
            (u32)pCreateID->databaseIndex) + 1;
    pCreateID->databaseIndex = (u8)all;
    pCreateID->flags = (u8)((u32)all >> 0x18) & 0xf | pCreateID->flags & 0xf0;
    pCreateID->_1 = (u8)((u32)all >> 0x10);
    pCreateID->_2 = (u8)((u32)all >> 8);
}

*/
/*
FFLResult FFLiGetCreateIDBaseBySystem(FFLiCreateIDBase* pCreateIDBase)
{
    RIO_ASSERT(pCreateIDBase != NULL);

    // TODO NOT TESTED PROBABLY WONT WORK vv
#if RIO_IS_CAFE
    u8 deviceHash[12];
    nn::Result result = nn::act::GetDeviceHash(&deviceHash);
    if (result.IsFailure())
        return FFL_RESULT_FILE_LOAD_ERROR;

    rio::MemUtil::copy(pCreateIDBase, &deviceHash, sizeof(FFLiCreateIDBase));
#else
    rio::MemUtil::set(pCreateIDBase, 0, sizeof(FFLiCreateIDBase));
#endif // RIO_IS_CAFE

    return FFL_RESULT_OK;
}


FFLiCreateIDBase* FFLiGetCreateIDBase()
{
    if (!FFLiManager::IsConstruct())
        return NULL;

    FFLiManager* pManager = FFLiManager::GetInstance();
    return pManager->GetSystemContext()->GetCreateIDBase();
}

FFLResult FFLpSetupCreateIDBase() // FFLp = private?
{
    FFLiCreateIDBase createIDBase;
    FFLResult result = FFLiGetCreateIDBaseBySystem(&createIDBase);
    if (result == FFL_RESULT_OK)
    {
        if (!FFLiManager::IsConstruct())
            return FFL_RESULT_MANAGER_NOT_CONSTRUCT;

        FFLiManager* pManager = FFLiManager::GetInstance();
        pManager->GetSystemContext()->SetCreateIDBase(&createIDBase);
        result = FFL_RESULT_OK;
    }

    return result;
}
*/


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
