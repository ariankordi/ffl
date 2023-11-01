#include <nn/ffl/FFLiPath.h>

#if RIO_IS_CAFE

#include <cafe/sysapp.h>

#include <nn/save/save_Types.h>

extern "C" {

// Thanks for deprecating these, Nintendo... :/

typedef enum SysSystemApplicationID
{
    SYS_ID_MII_MAKER    = 4
} SysSystemApplicationID;

u64 _SYSGetSystemApplicationTitleId(SysSystemApplicationID id);

SAVEStatus SAVEGetSharedDataTitlePath(u64 titleId, const char* path, char* dst, u32 size);
SAVEStatus SAVEGetSharedSaveDataPath(u64 titleId, const char* path, char* dst, u32 size);

}

#else

#include <cstdio>

#endif // RIO_IS_CAFE

u64 FFLiGetMiiStudioTitleID()
{
#if RIO_IS_CAFE
    return _SYSGetSystemApplicationTitleId(SYS_ID_MII_MAKER);
#else
    return 0;
#endif // RIO_IS_CAFE
}

rio::RawErrorCode FFLiGetResourcePath(char* pDst, u32 size, const char* filename)
{
#if RIO_IS_CAFE
    return rio::RawErrorCode(SAVEGetSharedDataTitlePath(0x0005001B10056000ULL, filename, pDst, size));
#else
    int ret = std::snprintf(pDst, size, "C:/Users/USR/Desktop/cemu_1.7.5/mlc01/sys/title/0005001b/10056000/content/%s", filename);
    if (ret > 0 && (u32)ret < size)
        return rio::RAW_ERROR_OK;
    return rio::RAW_ERROR_FATAL_ERROR;
#endif // RIO_IS_CAFE
}

rio::RawErrorCode FFLiGetDatabasePath(char* pDst, u32 size, u64 titleID, const char* filename)
{
#if RIO_IS_CAFE
    return rio::RawErrorCode(SAVEGetSharedSaveDataPath(titleID, filename, pDst, size));
#else
    RIO_ASSERT(titleID == 0);
    int ret = std::snprintf(pDst, size, "C:/Users/USR/Desktop/cemu_1.7.5/mlc01/usr/save/00050010/1004a100/user/common/%s", filename);
    if (ret > 0 && (u32)ret < size)
        return rio::RAW_ERROR_OK;
    return rio::RAW_ERROR_FATAL_ERROR;
#endif
}

FFLResult FFLiConvertFSStatusToFFLResult(rio::RawErrorCode status)
{
    if (status == rio::RAW_ERROR_OK)
        return FFL_RESULT_OK;

    return FFL_RESULT_FS_ERROR;
}
