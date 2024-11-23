#ifndef FFL_DATABASE_H_
#define FFL_DATABASE_H_

#include <nn/ffl/FFLDataSource.h>
#include <nn/ffl/FFLResult.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FFLAdditionalInfo FFLAdditionalInfo;
typedef struct FFLStoreData FFLStoreData;

#ifndef FFL_NO_DATABASE_RANDOM
#include <nn/ffl/FFLiDatabaseRandom.h>

void FFLiGetRandomCharInfo(FFLiCharInfo* pCharInfo, FFLGender gender, FFLAge age, FFLRace race);
#endif // FFL_NO_DATABASE_RANDOM

#if !defined(FFL_NO_DATABASE_FILE) && !defined(FFL_NO_DATABASE_DEFAULT) && !defined(FFL_NO_DATABASE_RANDOM)

FFLResult FFLiGetStoreData(FFLStoreData* pStoreData, FFLDataSource dataSource, u16 index);

bool FFLIsAvailableOfficialData(u16 index);
bool FFLIsRegularOfficialData(u16 index);

void FFLiEnableSpecialMii(u32 key);
#endif // !defined(FFL_NO_DATABASE_FILE) && !defined(FFL_NO_DATABASE_DEFAULT) && !defined(FFL_NO_DATABASE_RANDOM)

FFLResult FFLGetAdditionalInfo(FFLAdditionalInfo* pAdditionalInfo, FFLDataSource dataSource, const void* pBuffer, u16 index, bool checkFontRegion);

#ifdef __cplusplus
}
#endif

#endif // FFL_DATABASE_H_
