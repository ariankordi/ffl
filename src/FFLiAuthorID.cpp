#include <nn/ffl/FFLiAuthorID.h>
#include <nn/ffl/FFLiSystem.h>

bool FFLiIsHomeAuthorID(const FFLiAuthorID* pAuthorID)
{
    return FFLiIsSameAuthorID(pAuthorID, FFLiGetUserID());
}

bool FFLiIsSameAuthorID(const FFLiAuthorID* a, const FFLiAuthorID* b)
{
    [[maybe_unused]] const u32 count = FFLI_AUTHOR_ID_SIZE / sizeof(u16);
    NN_STATIC_ASSERT(count == 4);

    if (a->value16[0] != b->value16[0])
        return false;

    if (a->value16[1] != b->value16[1])
        return false;

    if (a->value16[2] != b->value16[2])
        return false;

    if (a->value16[3] != b->value16[3])
        return false;

    return true;
}

bool FFLiIsNullAuthorID(const FFLiAuthorID* pAuthorID)
{
    if (pAuthorID != NULL)
    {
        [[maybe_unused]] const u32 count = FFLI_AUTHOR_ID_SIZE / sizeof(u16);
        NN_STATIC_ASSERT(count == 4);

        if (pAuthorID->value16[0] == 0 &&
            pAuthorID->value16[1] == 0 &&
            pAuthorID->value16[2] == 0 &&
            pAuthorID->value16[3] == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
}
