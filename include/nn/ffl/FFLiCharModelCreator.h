#ifndef FFLI_CHAR_MODEL_CREATOR_H_
#define FFLI_CHAR_MODEL_CREATOR_H_

#include <nn/ffl/FFLResult.h>

struct FFLCharModelSource;
struct FFLCharModelDesc;
struct FFLShaderCallback;

struct  FFLiCharModel;
class   FFLiCharModelCreateParam;
class   FFLiManager;

#ifdef FFL_USE_TEXTURE_CALLBACK
#include <nn/ffl/FFLTextureCallback.h>
#endif

class FFLiCharModelCreator
{
public:
    FFLiCharModelCreator(FFLiCharModelCreateParam* pParam, FFLiManager* pManager);
    ~FFLiCharModelCreator();

    FFLResult ExecuteCPUStep(FFLiCharModel* pModel, const FFLCharModelSource* pSource, const FFLCharModelDesc* pDesc
#ifdef FFL_USE_TEXTURE_CALLBACK
    , const FFLTextureCallback* pCallback);
#else
    );
#endif

    void ExecuteGPUStep(FFLiCharModel* pModel, const FFLShaderCallback* pCallback);
    void AfterExecuteGPUStep(FFLiCharModel* pModel);

    static void Delete(FFLiCharModel* pModel);

    void InvalidateShapes(FFLiCharModel* pModel);

private:
    FFLiCharModelCreateParam*   m_pCharModelCreateParam;
    FFLiManager*                m_pManager;
};
NN_STATIC_ASSERT32(sizeof(FFLiCharModelCreator) == 8);

#endif // FFLI_CHAR_MODEL_CREATOR_H_
