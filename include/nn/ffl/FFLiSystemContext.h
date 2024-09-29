#ifndef FFLI_SYSTEM_CONTEXT_H_
#define FFLI_SYSTEM_CONTEXT_H_

#include <nn/ffl/FFLiAuthorID.h>
#include <nn/ffl/FFLiCreateID.h>
#include <nn/ffl/FFLiRandomContext.h>

class FFLiSystemContext
{
public:
    FFLiSystemContext();
    ~FFLiSystemContext();

    void Init(u32 seed);

#ifndef FFL_NO_DATABASE_RANDOM
    FFLiRandomContext* RandomContext()
    {
        return &m_RandomContext;
    }
#endif

    const FFLiAuthorID* UserID() const;
    u64 TitleID() const;

    /*
    Deleted in NSMBU:
    const FFLiCreateIDBase* GetCreateIDBase() const;
    void SetCreateIDBase(const FFLiCreateIDBase* pCreateIDBase);
    */

    bool AfterConstruct();

private:
#ifndef FFL_NO_DATABASE_RANDOM
    FFLiRandomContext   m_RandomContext;
#endif
    FFLiAuthorID        m_AuthorID;
    FFLiCreateIDBase    m_CreateIDBase;
    u64                 m_TitleID;
};
//NN_STATIC_ASSERT(sizeof(FFLiSystemContext) == 0x28);

#endif // FFLI_SYSTEM_CONTEXT_H_
