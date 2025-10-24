#ifndef FFLI_CREATE_ID_H_
#define FFLI_CREATE_ID_H_

#include <nn/ffl/FFLCreateID.h>
#include <nn/ffl/FFLResult.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FFLI_CREATE_ID_BASE_SIZE  (6)

typedef struct FFLiCreateIDBase
{
    // Unsure if this is correct
    union
    {
        u8 data[FFLI_CREATE_ID_BASE_SIZE];
        u16 value16[FFLI_CREATE_ID_BASE_SIZE / sizeof(u16)];
    };
}
FFLiCreateIDBase;
NN_STATIC_ASSERT(sizeof(FFLiCreateIDBase) == FFLI_CREATE_ID_BASE_SIZE);

// Bit 0: Normal
// Bit 1: NTR (Set on DS  / Wii U, cleared on Wii / 3DS)
// Bit 2: Temporary
// Bit 3: CTR (Set on 3DS / Wii U, cleared on Wii / DS)

#define FFLI_CREATE_ID_BIT_NORMAL    0x80 // (1 << 7) // 0b10000000
#define FFLI_CREATE_ID_BIT_NTR       0x40 // (1 << 6) // 0b01000000
#define FFLI_CREATE_ID_BIT_TEMPORARY 0x20 // (1 << 5) // 0b00100000
#define FFLI_CREATE_ID_BIT_CTR       0x10 // (1 << 4) // 0b00010000

#define FFLI_CREATE_ID_TYPE_WIIU     (FFLI_CREATE_ID_BIT_NTR | FFLI_CREATE_ID_BIT_CTR)

typedef struct FFLiCreateID
{
    u8                  flags;
    u8                  _1;
    u8                  _2;
    u8                  databaseIndex;
    FFLiCreateIDBase    base;
}
FFLiCreateID;
NN_STATIC_ASSERT(sizeof(FFLiCreateID) == FFL_CREATE_ID_SIZE);

bool FFLiIsNullMiiID(const FFLCreateID* pCreateID);

void FFLiGetTemporaryMiiID(FFLCreateID* pCreateID);

bool FFLiIsNormalMiiID(const FFLCreateID* pCreateID);
bool FFLiIsSpecialMiiID(const FFLCreateID* pCreateID);

bool FFLiIsWiiMiiID(const FFLCreateID* pCreateID);
bool FFLiIsNTRMiiID(const FFLCreateID* pCreateID);
bool FFLiIsCTRMiiID(const FFLCreateID* pCreateID);
bool FFLiIsWiiUMiiID(const FFLCreateID* pCreateID);

bool FFLiIsTemporaryMiiID(const FFLCreateID* pCreateID);

bool FFLiIsValidMiiID(const FFLCreateID* pCreateID);
bool FFLiIsSameMiiID(const FFLCreateID* a, const FFLCreateID* b);

void FFLiGetDefaultCreateIDonCTR(FFLCreateID* pCreateID, s32 index);
//void FFLiCreateMiiIDonWiiU(FFLCreateID* pCreateID, const FFLiCreateIDBase* pCreateIDBase, u32 date, bool isSpecial); // Deleted in NSMBU
//void FFLiiSetCreateID(FFLCreateID* pCreateID, bool isSpecial); // Deleted in NSMBU
//void FFLiSetCreateID(FFLCreateID* pCreateID, bool isSpecial); // Deleted in NSMBU
//void FFLiIncrementCreateIDTime(FFLiCreateID* pCreateID); // Deleted in NSMBU

//FFLResult FFLiGetCreateIDBaseBySystem(FFLiCreateIDBase* pCreateIDBase); // Deleted in NSMBU
//FFLiCreateIDBase* FFLiGetCreateIDBase(); // Deleted in NSMBU
//FFLResult FFLpSetupCreateIDBase(); // FFLp = private?

#ifdef __cplusplus
}
#endif

#endif // FFLI_CREATE_ID_H_
