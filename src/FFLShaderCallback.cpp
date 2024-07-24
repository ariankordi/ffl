#include <nn/ffl/FFLShaderCallback.h>

#include <nn/ffl/FFLiManager.h>

void FFLSetShaderCallback(const FFLShaderCallback* pCallback)
{
    // HACK: initialize this to false in case the shader class does not initialize it
    const_cast<FFLShaderCallback*>(pCallback)->facelineColorIsTransparent = false;
    if (FFLiManager::IsConstruct())
        FFLiManager::GetInstance()->GetShaderCallback().Set(pCallback);
}
