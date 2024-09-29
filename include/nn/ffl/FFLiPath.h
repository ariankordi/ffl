#ifndef FFLI_PATH_H_
#define FFLI_PATH_H_

#include <nn/ffl/FFLResult.h>

#include <filedevice/rio_FileDevice.h>

#if defined(FFL_NO_FS) && !defined(FFL_NO_DATABASE_FILE)
#define FFL_NO_DATABASE_FILE
#endif // defined(FFL_NO_FS) && !defined(FFL_NO_DATABASE_FILE)
#if defined(FFL_NO_FS) && !defined(FFL_MLC_PATH)
#define FFL_MLC_PATH "."
#endif // defined(FFL_NO_FS) && !defined(FFL_MLC_PATH)

u64 FFLiGetMiiStudioTitleID();

rio::RawErrorCode FFLiGetResourcePath(char* pDst, u32 size, const char* filename);
rio::RawErrorCode FFLiGetDatabasePath(char* pDst, u32 size, u64 titleID, const char* filename);

FFLResult FFLiConvertFSStatusToFFLResult(rio::RawErrorCode status);

#endif // FFLI_PATH_H_
