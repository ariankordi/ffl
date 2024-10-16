#ifndef FFL_INIT_DESC_H_
#define FFL_INIT_DESC_H_

#include <nn/ffl/FFLFontRegion.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FFLInitDesc
{
    const void*             _0; // Deleted
    void*                   _4; // Deleted
    FFLFontRegion           fontRegion;
    int                     _c;
    int                     _10;
}
FFLInitDesc;
NN_STATIC_ASSERT32(sizeof(FFLInitDesc) == 0x14);

#ifdef __cplusplus
}
#endif

#endif // FFL_INIT_DESC_H_
