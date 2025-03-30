#ifndef FFL_DATABASE_H_
#define FFL_DATABASE_H_

#include <nn/ffl/FFLDataSource.h>
#include <nn/ffl/FFLResult.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FFLAdditionalInfo FFLAdditionalInfo;
typedef struct FFLStoreData FFLStoreData;
typedef struct FFLiMiiDataOfficialRFL FFLiMiiDataOfficialRFL;

typedef struct FFLiCharInfo FFLiCharInfo;

#ifndef FFL_NO_DATABASE_RANDOM

#include <nn/ffl/FFLGender.h>
#include <nn/ffl/FFLAge.h>
#include <nn/ffl/FFLRace.h>

void FFLiGetRandomCharInfo(FFLiCharInfo* pCharInfo, FFLGender gender, FFLAge age, FFLRace race);
#endif // FFL_NO_DATABASE_RANDOM

#if !defined(FFL_NO_DATABASE_FILE) && !defined(FFL_NO_DATABASE_DEFAULT) && !defined(FFL_NO_DATABASE_RANDOM)

FFLResult FFLiGetStoreData(FFLStoreData* pStoreData, FFLDataSource dataSource, u16 index);

bool FFLIsAvailableOfficialData(u16 index);
bool FFLIsRegularOfficialData(u16 index);
#endif // !defined(FFL_NO_DATABASE_FILE) && !defined(FFL_NO_DATABASE_DEFAULT) && !defined(FFL_NO_DATABASE_RANDOM)

// FFLiGetRandomCharInfo
//FFLResult FFLiPickupCharInfo(FFLiCharInfo* pCharInfo, FFLDataSource dataSource, const void* pBuffer, u16 index); // Deleted in NSMBU

FFLResult FFLGetAdditionalInfo(FFLAdditionalInfo* pAdditionalInfo, FFLDataSource dataSource, const void* pBuffer, u16 index, bool checkFontRegion);
void FFLiEnableSpecialMii(u32 key);

// Not sure where these are supposed to go:
bool FFLpGetStoreDataFromCharInfo(FFLStoreData* pStoreData, const FFLiCharInfo* pCharInfo); // Deleted in NSMBU
bool FFLpGetCharInfoFromStoreData(FFLiCharInfo* pCharInfo, FFLStoreData* pStoreData); // Deleted in NSMBU
bool FFLpGetCharInfoFromMiiDataOfficialRFL(FFLiCharInfo* pCharInfo, FFLiMiiDataOfficialRFL* pMiiDataOfficial); // Custom

#ifdef __cplusplus
}
#endif

#endif // FFL_DATABASE_H_
