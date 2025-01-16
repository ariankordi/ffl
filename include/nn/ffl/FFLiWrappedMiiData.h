#ifndef FFLI_WRAPPED_MIIDATA_H_
#define FFLI_WRAPPED_MIIDATA_H_

// NOTE that this entire file is unfinished
// and doesn't actually declare anything.

#include <nn/ffl/FFLiMiiDataCore.h>
#include <nn/ffl/FFLStandard.h>

//FFLResult FFLiWrapStoreData(FFLiWrappedStoreData* pWrappedStoreData, const FFLiStoreDataCFL* pStoreDataCFL);

// need this if you add these funcs:
#if RIO_IS_CAFE
//#include <nn/acp.h> // TODO uncomment
#endif // RIO_IS_CAFE

/*
FFLResult FFLiWrapStoreData(FFLiWrappedStoreData* pWrappedStoreData, const FFLiStoreDataCFL* pStoreDataCFL)
{
    if (pWrappedStoreData == NULL || pStoreDataCFL == NULL)
        return FFL_RESULT_ERROR;
#if RIO_IS_CAFE
    int iVar1 = ACPMiiWrap(pWrappedStoreData, FFLI_WRAPPEDSTOREDATA_SIZE, pWrappedStoreData, FFL_MIIDATA_PACKET_SIZE);
    if (iVar1 != 0)
        iVar1 = 16; // FFL_RESULT_FAILED_SYSTEM0

    return iVar1;
#else
    RIO_ASSERT(false);
    return FFL_RESULT_ERROR;
#endif // RIO_IS_CAFE
}
*/

//FFLResult FFLiUnwrapStoreData(FFLiStoreDataCFL* pStoreDataCFL, const FFLiWrappedStoreData* pWrappedStoreData);

/*
FFLResult FFLiUnwrapStoreData(FFLiStoreDataCFL* pStoreDataCFL, const FFLiWrappedStoreData* pWrappedStoreData)
{
    if (pStoreDataCFL == NULL || pWrappedStoreData == NULL)
        return FFL_RESULT_ERROR;
#if RIO_IS_CAFE
    int iVar1 = ACPMiiUnwrap(pStoreDataCFL, FFL_MIIDATA_PACKET_SIZE, pWrappedStoreData, FFLI_WRAPPEDSTOREDATA_SIZE);
    if (iVar1 == -0x899)
        return FFL_RESULT_FILE_INVALID;

    if (iVar1 != 0)
        iVar1 = 16; // FFL_RESULT_FAILED_SYSTEM0

    return iVar1;
#else
    RIO_ASSERT(false);
    return FFL_RESULT_ERROR;
#endif // RIO_IS_CAFE
}
*/

#define FFLI_WRAPPEDSTOREDATA_SIZE  (0x70)

// CFLiWrappedMiiData, only references are in: 000500001010C700, 0005000010128F00
// __N_15_FFLiNetwork_cpp_5a76681b::UnwrapStoreData(FFLiStoreDataCFL *, FFLiWrappedStoreData const *, bool)
// __N_15_FFLiNetwork_cpp_5a76681b::WrapStoreData(FFLiWrappedStoreData *, FFLiStoreDataCFL const *, bool)
// , FFLiUnwrapStoreData
// Used for Mii QR codes and networking.

struct FFLiWrappedStoreData
{
    union
    {
        u8 data[FFLI_WRAPPEDSTOREDATA_SIZE];
        u32 value32[FFLI_WRAPPEDSTOREDATA_SIZE / sizeof(u32)];
    };
};
//NN_STATIC_ASSERT_IS_POD(FFLiWrappedStoreData);
NN_STATIC_ASSERT(sizeof(FFLiWrappedStoreData) == FFLI_WRAPPEDSTOREDATA_SIZE);

#endif // FFLI_WRAPPED_MIIDATA_H_
