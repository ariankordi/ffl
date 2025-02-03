#ifndef FFLI_RFL_CREATE_ID_H_
#define FFLI_RFL_CREATE_ID_H_

#include <nn/ffl/FFLCreateID.h>
#include <nn/ffl/FFLiAuthorID.h>
#include <nn/ffl/FFLResult.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FFLI_RFL_CREATE_ID_SIZE  (8) // 2 bytes smaller than FFLCreateID

class FFLiRFLCreateID
{
public:
    bool Convert(FFLCreateID* pCreateID, const FFLiAuthorID* pAuthorID) const;

    bool IsValid() const;
    bool IsValidOnNand() const;

    bool IsTemporary() const;
    bool IsNormal() const;
    bool IsSpecial() const;
    bool IsDs() const;
    bool IsWii() const;
private:
    union
    {
        u8 data[FFLI_RFL_CREATE_ID_SIZE];
        u16 value16[FFLI_RFL_CREATE_ID_SIZE / sizeof(u16)];
    };
};
NN_STATIC_ASSERT(sizeof(FFLiRFLCreateID) == FFLI_RFL_CREATE_ID_SIZE);

#ifdef __cplusplus
}
#endif

#endif // FFLI_RFL_CREATE_ID_H_
