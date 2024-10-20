#include <nn/ffl/FFLiRandomContext.h>

#ifndef FFL_NO_DATABASE_RANDOM // iirc FFLiDatabaseRandom.cpp is the only thing that uses this

void FFLiRandomContext::Init(u32 seed)
{
    m_RndX = seed;
    m_RndY = 1566083941;
    m_RndZ = 2531011;
}

u16 FFLiRandomContext::Random(u16 max)
{
    m_RndX = m_RndY * m_RndX + m_RndZ;

    u32 ret = m_RndX >> 16;
    if (max != 0)
        ret = ret * u32(max) >> 16;

    return ret;
}

#endif // FFL_NO_DATABASE_RANDOM
