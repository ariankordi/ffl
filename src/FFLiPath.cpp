#include <nn/ffl/FFLiPath.h>

#if RIO_IS_CAFE
#include <sysapp/title.h>
#include <nn/save/save.h>
#else
#ifndef FFL_MLC_PATH
#error "Must define FFL_MLC_PATH macro"
#endif // FFL_MLC_PATH
#include <cstdio>
#endif // RIO_IS_CAFE

u64 FFLiGetMiiStudioTitleID()
{
#if RIO_IS_CAFE
    return _SYSGetSystemApplicationTitleId(SYSTEM_APP_ID_MII_MAKER);
#else
    return 0;
#endif // RIO_IS_CAFE
}

rio::RawErrorCode FFLiGetResourcePath(char* pDst, u32 size, const char* filename)
{
#if RIO_IS_CAFE
    return rio::RawErrorCode(SAVEGetSharedDataTitlePath(0x0005001B10056000ULL, filename, pDst, size));
#else
    static const char BASE_PATH[] = FFL_MLC_PATH "/sys/title/0005001b/10056000/content/";
    const size_t base_len = std::strlen(BASE_PATH);
    const size_t file_len = std::strlen(filename);

    // Ensure buffer is large enough for base + file + null terminator.
    if (base_len + file_len + 1 > size)
        return rio::RAW_ERROR_FATAL_ERROR;
    std::strncpy(pDst, BASE_PATH, size);
    pDst[size - 1] = '\0'; // Force null termination.
    std::strncat(pDst, filename, size - base_len - 1);
    pDst[size - 1] = '\0';

    return rio::RAW_ERROR_OK;
#endif // RIO_IS_CAFE
}

rio::RawErrorCode FFLiGetDatabasePath(char* pDst, u32 size, u64 titleID, const char* filename)
{
#if RIO_IS_CAFE
    return rio::RawErrorCode(SAVEGetSharedSaveDataPath(titleID, filename, pDst, size));
#else
    RIO_ASSERT(titleID == 0);

    static const char BASE_PATH[] = FFL_MLC_PATH "/usr/save/00050010/1004a100/user/common/";
    const size_t base_len = std::strlen(BASE_PATH);
    const size_t file_len = std::strlen(filename);

    if (base_len + file_len + 1 > size)
        return rio::RAW_ERROR_FATAL_ERROR;
    std::strncpy(pDst, BASE_PATH, size);
    pDst[size - 1] = '\0';
    std::strncat(pDst, filename, size - base_len - 1);
    pDst[size - 1] = '\0';

    return rio::RAW_ERROR_OK;
#endif
}

FFLResult FFLiConvertFSStatusToFFLResult(rio::RawErrorCode status)
{
    if (status == rio::RAW_ERROR_OK)
        return FFL_RESULT_OK;

    return FFL_RESULT_FS_ERROR;
}
