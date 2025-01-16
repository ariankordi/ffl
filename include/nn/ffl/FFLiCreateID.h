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
// Bit 1: Set on DS // << Bit 1,3 Set: Wii U, Bit 1,3 Clear: Wii
// Bit 2: Temporary
// Bit 3: CTR       // << Bit 1,3 Set: Wii U, Bit 1,3 Clear: Wii

#define FFLI_CREATE_ID_FLAG_TEMPORARY (1 << 5) // 0b00100000
#define FFLI_CREATE_ID_FLAG_NORMAL    (1 << 7) // 0b10000000
#define FFLI_CREATE_ID_FLAG_SPECIAL   0        // 0b00000000

#define FFLI_CREATE_ID_TYPE_MASK      ((1 << 6) | (1 << 4))
                                      // 0b01010000

#define FFLI_CREATE_ID_TYPE_WII       0 // Bits 6 and 4 are 0.
#define FFLI_CREATE_ID_TYPE_CTR       (1 << 4) // 0b00010000
#define FFLI_CREATE_ID_TYPE_NTR       (1 << 6) // 0b01000000
#define FFLI_CREATE_ID_TYPE_WIIU      ((1 << 6) | (1 << 4))
                                      // 0b01010000

// For reference:
// #define RFLi_CREATE_ID_MASK_TEMPORARY 0x20000000   // 0b00100000
// #define RFLi_CREATE_ID_MASK_NOT_SPECIAL 0x80000000 // 0b10000000

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
