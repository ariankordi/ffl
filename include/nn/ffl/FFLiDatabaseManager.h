#ifndef FFLI_DATABASE_MANAGER_H_
#define FFLI_DATABASE_MANAGER_H_

#include <nn/ffl/FFLDataSource.h>
#include <nn/ffl/FFLResult.h>

#include <nn/ffl/FFLiDatabaseDefault.h>
#include <nn/ffl/FFLiDatabaseFileAccessor.h>
#include <nn/ffl/FFLiDatabaseRandom.h>

struct  FFLiCharInfo;
class   FFLiMiddleDB;
class   FFLiSystemContext;

class FFLiDatabaseManager
{
public:
    FFLiDatabaseManager(FFLiDatabaseFile* pFile, FFLiFileWriteBuffer* pWriteBuffer, FFLiSystemContext* pContext);
    ~FFLiDatabaseManager();

#ifndef FFL_NO_DATABASE_FILE
    FFLiDatabaseFileAccessor& GetDatabaseFileAccessor()
    {
        return m_DatabaseFileAccessor;
    }
#endif
#ifndef FFL_NO_DATABASE_RANDOM
    FFLiDatabaseRandom& GetDatabaseRandom() // may not be in FFL
    {
        return m_DatabaseRandom;
    }
#endif

    FFLResult AfterConstruct();
    FFLResult BeforeDestruct();
    FFLResult FlushQuota(bool force);

    void EnableSpecialMii(u32 key);
    bool IsEnabledSpecialMii() const;

    FFLResult GetCharInfoFromOfficial(FFLiCharInfo* pCharInfo, u16 index);
    FFLResult GetCharInfoFromDefault(FFLiCharInfo* pCharInfo, u16 index);
    FFLResult GetCharInfoFromStoreData(FFLiCharInfo* pCharInfo, FFLDataSource dataSource, const FFLiStoreDataCFL* pStoreDataCFL, u16 index);
    FFLResult PickupCharInfo(FFLiCharInfo* pCharInfo, FFLDataSource dataSource, const void* pBuffer, u16 index);
    FFLResult GetStoreData(FFLiStoreDataCFL* pStoreDataCFL, FFLDataSource dataSource, u16 index);

    FFLResult UpdateMiddleDB(FFLiMiddleDB* pMiddleDB);

private:
    FFLiSystemContext*          m_pSystemContext;
    // These are excluded from here because their initializers
    // will be missing, the whole file would be excluded.
#ifndef FFL_NO_DATABASE_FILE
    FFLiDatabaseFileAccessor    m_DatabaseFileAccessor;
#endif
#ifndef FFL_NO_DATABASE_DEFAULT
    FFLiDatabaseDefault         m_DatabaseDefault;
#endif
#ifndef FFL_NO_DATABASE_RANDOM
    FFLiDatabaseRandom          m_DatabaseRandom;
#endif
    // Only thing that may be somewhat worth pulling from here
    // anyway is this, which is borderline useless and can probably be seen by other means
    bool                        m_IsEnabledSpecialMii;
    void*                       _fd4;   // Deleted
};
#if !defined(FFL_NO_DATABASE_FILE) && !defined(FFL_NO_DATABASE_DEFAULT) && !defined(FFL_NO_DATABASE_RANDOM)
NN_STATIC_ASSERT32(sizeof(FFLiDatabaseManager) == 0xFD8); // NOTE: will not be consistent if you disable features
#endif

#endif // FFLI_DATABASE_MANAGER_H_
