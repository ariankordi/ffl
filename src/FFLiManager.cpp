#include <nn/ffl/FFLResourceDesc.h>

#include <nn/ffl/FFLiDatabaseFile.h>
#include <nn/ffl/FFLiManager.h>
#include <nn/ffl/FFLiResourceHeader.h>

#include <nn/ffl/detail/FFLiFileWriteBuffer.h>

#include <filedevice/rio_FileDevice.h>

#include <new>

// --------------------------------------------------------------------------

static void SetResultIfError(FFLResult* pDst, FFLResult src)
{
    if (*pDst == FFL_RESULT_OK || *pDst == FFL_RESULT_ODB_EMPTY)
        if (src != FFL_RESULT_OK)
            *pDst = src;
}

// --------------------------------------------------------------------------

namespace {

FFLiManager* g_FFLManager = NULL;

FFLResult InitResImpl(const FFLInitDesc* pInitDesc, const FFLResourceDesc* pResDesc);

}

FFLResult FFLiInitResEx(const FFLInitDesc* pInitDesc, const FFLResourceDesc* pResDesc)
{
    return InitResImpl(pInitDesc, pResDesc);
}

void FFLiInitResGPUStep()
{
    if (FFLiManager::IsConstruct())
        FFLiManager::GetInstance()->SetupGPU();
}

FFLResult FFLiFlushQuota(bool force)
{
    if (!FFLiManager::IsConstruct())
        return FFL_RESULT_MANAGER_NOT_CONSTRUCT;

    return FFLiManager::GetInstance()->FlushQuota(force);
}

FFLResult FFLiExit()
{
    return FFLiManager::Destroy();
}

bool FFLiIsAvailable()
{
    return FFLiManager::IsConstruct();
}

FFLResult FFLiManager::Create(const FFLInitDesc* pInitDesc, const FFLResourceDesc* pResDesc)
{
    if (IsConstruct())
        return FFL_RESULT_OK;

    if (pInitDesc == NULL)
        return FFL_RESULT_ERROR;

    g_FFLManager = new FFLiManager(pInitDesc);

    FFLResult result = g_FFLManager->AfterConstruct(pInitDesc, pResDesc);
    if (result != FFL_RESULT_OK && result != FFL_RESULT_ODB_EMPTY)
        Destroy();

    return result;
}

FFLResult FFLiManager::Destroy()
{
    if (!IsConstruct())
        return FFL_RESULT_MANAGER_NOT_CONSTRUCT;

    FFLiManager* pManager = GetInstance();

    FFLResult result = pManager->BeforeDestruct();

    delete pManager;
    g_FFLManager = NULL;

    return result;
}

bool FFLiManager::IsConstruct()
{
    return g_FFLManager != NULL;
}

FFLiManager* FFLiManager::GetInstance()
{
    return g_FFLManager;
}

FFLiManager::FFLiManager(const FFLInitDesc* pInitDesc)
    : m_SystemContext()

#ifndef FFL_NO_FS
    , m_pResourceMultiHeader(static_cast<FFLiResourceMultiHeader*>(
        rio::MemUtil::alloc(sizeof(FFLiResourceMultiHeader), rio::FileDevice::cBufferMinAlignment)))
#else
    , m_pResourceMultiHeader(nullptr)
#endif

#ifndef FFL_NO_DATABASE_FILE
    , m_pDatabaseFile(static_cast<FFLiDatabaseFile*>(
        rio::MemUtil::alloc(sizeof(FFLiDatabaseFile), rio::FileDevice::cBufferMinAlignment)))
    , m_pFileWriteBuffer(static_cast<FFLiFileWriteBuffer*>(
        rio::MemUtil::alloc(sizeof(FFLiFileWriteBuffer), rio::FileDevice::cBufferMinAlignment)))
#else
    , m_pDatabaseFile(nullptr)
    , m_pFileWriteBuffer(nullptr)
#endif

#ifndef FFL_NO_FS
    , m_ResourceManager(m_pResourceMultiHeader)
#else
    , m_ResourceManager(nullptr)
#endif

#ifndef FFL_NO_DATABASE_FILE
    , m_DatabaseManager(m_pDatabaseFile, m_pFileWriteBuffer, &m_SystemContext)
#else
    , m_DatabaseManager(nullptr, nullptr, &m_SystemContext)
#endif

    , m_CharModelCreateParam(&m_DatabaseManager, &m_ResourceManager, &m_ShaderCallback)

    , m_InitDesc(*pInitDesc)

#if RIO_IS_CAFE
    , m_CopySurface()
#endif // RIO_IS_CAFE

    , m_IsSetupGPU(false)
{
}

FFLiManager::~FFLiManager()
{
#ifndef FFL_NO_FS
    if (m_pResourceMultiHeader != nullptr)
    {
        for (u32 i = 0; i < FFL_RESOURCE_TYPE_MAX; i++)
        {
            if (m_pResourceMultiHeader->header[i] != nullptr)
            {
                void* pData = m_pResourceMultiHeader->header[i]->GetHeaderRaw();
                if (pData != nullptr)
                    rio::MemUtil::free(pData);
                //delete m_pResourceMultiHeader->header[i];
            }
        }
        rio::MemUtil::free(m_pResourceMultiHeader);
        m_pResourceMultiHeader = nullptr;
    }
#endif // FFL_NO_FS
#ifndef FFL_NO_DATABASE_FILE
    if (m_pDatabaseFile != nullptr)
    {
        rio::MemUtil::free(m_pDatabaseFile);
        m_pDatabaseFile = nullptr;
    }
    if (m_pFileWriteBuffer != nullptr)
    {
        rio::MemUtil::free(m_pFileWriteBuffer);
        m_pFileWriteBuffer = nullptr;
    }
#endif // FFL_NO_DATABASE_FILE
}

FFLResult FFLiManager::AfterConstruct(const FFLInitDesc* pInitDesc, const FFLResourceDesc* pResDesc)
{
    if (!m_SystemContext.AfterConstruct())
        return FFL_RESULT_UNKNOWN_17;

    FFLResult result = m_ResourceManager.AfterConstruct();
    if (result != FFL_RESULT_OK)
        return result;

    if (pResDesc == NULL)
    {
#ifndef FFL_NO_FS
        result = m_ResourceManager.LoadResourceHeader();
        if (result != FFL_RESULT_OK)
            return result;
#else
        RIO_ASSERT(false);
        return FFL_RESULT_ERROR;
#endif
    }
    else
    {
        bool noResourceEverLoaded = true;
        for (u32 i = 0; i < FFL_RESOURCE_TYPE_MAX; i++)
        {
            // skip resources with size of zero
            if (pResDesc->size[i] == 0) {
                RIO_LOG("FFL resource %i has size of zero, skipping\n", i);
                continue;
            }
            noResourceEverLoaded = false;
            result = m_ResourceManager.AttachCache(pResDesc->pData[i], pResDesc->size[i], FFLResourceType(i));
            if (result != FFL_RESULT_OK)
                return result;
        }
        // if no resource was loaded then do not go further
        if (noResourceEverLoaded)
            return FFL_RESULT_ERROR;
    }

    result = m_DatabaseManager.AfterConstruct();
    if (result != FFL_RESULT_OK)
        return result;

    return FFL_RESULT_OK;
}

FFLResult FFLiManager::BeforeDestruct()
{
    FFLResult result = FFL_RESULT_OK;
    SetResultIfError(&result, m_DatabaseManager.BeforeDestruct());
    return result;
}

bool FFLiManager::CanInitCharModel() const
{
    return m_IsSetupGPU;
}

void FFLiManager::SetupGPU()
{
    m_IsSetupGPU = true;

#if RIO_IS_CAFE
    m_CopySurface.SetupGPU();
#endif // RIO_IS_CAFE
}

FFLResult FFLiManager::FlushQuota(bool force)
{
    return m_DatabaseManager.FlushQuota(force);
}

namespace {

FFLResult InitResImpl(const FFLInitDesc* pInitDesc, const FFLResourceDesc* pResDesc)
{
    if (FFLiManager::IsConstruct())
        return FFL_RESULT_OK;

    return FFLiManager::Create(pInitDesc, pResDesc);
}

}
