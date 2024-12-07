#ifndef FFL_MIDDLE_DB_H_
#define FFL_MIDDLE_DB_H_

#include <nn/ffl/FFLAge.h>
#include <nn/ffl/FFLGender.h>
#include <nn/ffl/FFLMiddleDBType.h>
#include <nn/ffl/FFLRace.h>
#include <nn/ffl/FFLResult.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <nn/ffl/FFLiMiddleDB.h>

#define FFL_MIDDLE_DB_SIZE (int)sizeof(FFLiMiddleDB)

typedef struct FFLMiddleDB
{
    union
    {
        u8 data[FFL_MIDDLE_DB_SIZE];
        u32 data32[FFL_MIDDLE_DB_SIZE / sizeof(u32)];
    };
}
FFLMiddleDB;
NN_STATIC_ASSERT(sizeof(FFLMiddleDB) == FFL_MIDDLE_DB_SIZE);

u32 FFLGetMiddleDBBufferSize(u16 miiDataNum);

void FFLInitMiddleDB(FFLMiddleDB* pMiddleDB, FFLMiddleDBType type, void* pMiiData, u16 miiDataNum);
FFLResult FFLUpdateMiddleDB(FFLMiddleDB* pMiddleDB);

FFLMiddleDBType FFLGetMiddleDBType(const FFLMiddleDB* pMiddleDB);   // Deleted in NSMBU

s32 FFLGetMiddleDBSize(const FFLMiddleDB* pMiddleDB);               // Deleted in NSMBU
s32 FFLGetMiddleDBStoredSize(const FFLMiddleDB* pMiddleDB);

void FFLSetMiddleDBRandomMask(FFLMiddleDB* pMiddleDB, FFLGender gender, FFLAge age, FFLRace race);
void FFLSetMiddleDBHiddenMask(FFLMiddleDB* pMiddleDB, FFLGender gender);

#ifdef __cplusplus
}
#endif

#endif // FFL_MIDDLE_DB_H_