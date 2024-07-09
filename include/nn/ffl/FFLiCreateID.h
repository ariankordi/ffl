#ifndef FFLI_CREATE_ID_H_
#define FFLI_CREATE_ID_H_

#include <nn/ffl/FFLCreateID.h>

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

#define FFLI_CREATE_ID_FLAG_UNKNOWN_0   (1 << 4)
#define FFLI_CREATE_ID_FLAG_TEMPORARY   (1 << 5)
#define FFLI_CREATE_ID_FLAG_UNKNOWN_2   (1 << 6)
#define FFLI_CREATE_ID_FLAG_NORMAL      (1 << 7)

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
bool FFLiIsNTRMiiID(const FFLCreateID* pCreateID);
bool FFLiIsTemporaryMiiID(const FFLCreateID* pCreateID);

bool FFLiIsValidMiiID(const FFLCreateID* pCreateID);
bool FFLiIsSameMiiID(const FFLCreateID* a, const FFLCreateID* b);

#ifdef __cplusplus
}
#endif

#endif // FFLI_CREATE_ID_H_
