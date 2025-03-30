#include <nn/ffl/FFLDatabase.h>

#include <nn/ffl/FFLiAdditionalInfo.h>
#include <nn/ffl/FFLiMiiData.h>
#include <nn/ffl/FFLiDatabaseFile.h>
#include <nn/ffl/FFLiManager.h>

#include <nn/ffl/detail/FFLiCharInfo.h>

#ifndef FFL_NO_DATABASE_RANDOM
void FFLiGetRandomCharInfo(FFLiCharInfo* pCharInfo, FFLGender gender, FFLAge age, FFLRace race)
{
    if (!FFLiManager::IsConstruct())
        return;

    FFLiManager* pManager = FFLiManager::GetInstance();

    return pManager->GetDatabaseManager().GetDatabaseRandom().Get(pCharInfo, gender, age, race);
}
#endif

#if !defined(FFL_NO_DATABASE_FILE) && !defined(FFL_NO_DATABASE_DEFAULT) && !defined(FFL_NO_DATABASE_RANDOM) // then what is the point to any of these functions even being here huh

FFLResult FFLiGetStoreData(FFLStoreData* pStoreData, FFLDataSource dataSource, u16 index)
{
    if (!FFLiManager::IsConstruct())
        return FFL_RESULT_MANAGER_NOT_CONSTRUCT;

    return FFLiManager::GetInstance()->GetDatabaseManager().GetStoreData(static_cast<FFLiStoreDataCFL*>(pStoreData), dataSource, index);
}

bool FFLIsAvailableOfficialData(u16 index)
{
#ifndef FFL_NO_DATABASE_FILE
    if (FFLiManager::IsConstruct())
    {
        FFLiManager* pManager = FFLiManager::GetInstance();
        FFLiDatabaseManager& databaseManager = pManager->GetDatabaseManager();
        FFLiDatabaseFile* pDatabaseFile = databaseManager.GetDatabaseFileAccessor().GetDatabaseFile();
        return pDatabaseFile->official.IsAvailable(index, true, databaseManager.IsEnabledSpecialMii());
    }
#else
    RIO_ASSERT(false);
#endif
    return false;
}

bool FFLIsRegularOfficialData(u16 index)
{
#ifndef FFL_NO_DATABASE_FILE
    if (FFLiManager::IsConstruct())
    {
        FFLiManager* pManager = FFLiManager::GetInstance();
        FFLiDatabaseManager& databaseManager = pManager->GetDatabaseManager();
        FFLiDatabaseFile* pDatabaseFile = databaseManager.GetDatabaseFileAccessor().GetDatabaseFile();
        return pDatabaseFile->official.IsRegular(index, true, databaseManager.IsEnabledSpecialMii());
    }
#else
    RIO_ASSERT(false);
#endif
    return false;
}

void FFLiEnableSpecialMii(u32 key)
{
    if (!FFLiManager::IsConstruct())
        return;

    FFLiManager::GetInstance()->GetDatabaseManager().EnableSpecialMii(key);
}

#else
void FFLiEnableSpecialMii(u32 key)
{
    // No-op since FFL_NO_DATABASE_FILE is defined.
}

#endif // !defined(FFL_NO_DATABASE_FILE) && !defined(FFL_NO_DATABASE_DEFAULT) && !defined(FFL_NO_DATABASE_RANDOM)

bool FFLpGetStoreDataFromCharInfo(FFLStoreData* pStoreData, const FFLiCharInfo* pCharInfo)
{
    if (!FFLiiVerifyCharInfo(pCharInfo, true))
        return false;

    if (FFLiCharInfoToStoreDataCFL(static_cast<FFLiStoreDataCFL*>(pStoreData), pCharInfo) == FFL_RESULT_OK)
        return true;

    return false;
}

bool FFLpGetCharInfoFromStoreData(FFLiCharInfo* pCharInfo, FFLStoreData* pStoreData)
{
    if (FFLiStoreDataCFLToCharInfo(pCharInfo, *static_cast<FFLiStoreDataCFL*>(pStoreData)) == FFL_RESULT_OK)
        return FFLiiVerifyCharInfo(pCharInfo, true);

    return false;
}

bool FFLpGetCharInfoFromMiiDataOfficialRFL(FFLiCharInfo* pCharInfo, FFLiMiiDataOfficialRFL* pMiiDataOfficial)
{
    if (FFLiMiiDataOfficialRFL2CharInfo(pCharInfo, *static_cast<FFLiMiiDataOfficialRFL*>(pMiiDataOfficial), false) == FFL_RESULT_OK)
        return FFLiiVerifyCharInfo(pCharInfo, true);

    return false;
}

FFLResult FFLGetAdditionalInfo(FFLAdditionalInfo* pAdditionalInfo, FFLDataSource dataSource, const void* pBuffer, u16 index, bool checkFontRegion)
{
    if (!FFLiManager::IsConstruct())
        return FFL_RESULT_MANAGER_NOT_CONSTRUCT;

    FFLiManager* pManager = FFLiManager::GetInstance();

    if (dataSource == FFL_DATA_SOURCE_DIRECT_POINTER)
    {
        FFLiGetAdditionalInfo(pAdditionalInfo, reinterpret_cast<const FFLiCharInfo*>(pBuffer), checkFontRegion, pManager->GetInitDesc().fontRegion);
        return FFL_RESULT_OK;
    }
    else
    {
        FFLiCharInfo charInfo;
        FFLResult result = pManager->GetDatabaseManager().PickupCharInfo(&charInfo, dataSource, pBuffer, index);
        if (result == FFL_RESULT_OK)
            FFLiGetAdditionalInfo(pAdditionalInfo, &charInfo, checkFontRegion, pManager->GetInitDesc().fontRegion);
        return result;
    }
}

