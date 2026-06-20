#include "MayaPluginPCH.h"
#include "GetDefaultShadowBias.h"
#include "maya/MArgList.h"
#include "NiShadowManager.h"

MStatus GetDefaultShadowBias::doIt( const MArgList& args )
{
    MStatus kStat = MStatus::kSuccess;

    double fBias = 0.0f;

    NiShadowManager::Initialize();

    int iTechniqueIndex = args.asInt(0, &kStat);
    
    if(kStat != MStatus::kSuccess)
    {
        NIASSERT(0);

        displayError("Could not parse technique index argument");

        return MStatus::kFailure;
    }

    if(iTechniqueIndex >= NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES)
    {
        NIASSERT(0);

        displayError("Shadow Technique Index Out of Range!");

        return MStatus::kFailure;
    }

    int iLightType = args.asInt(1, &kStat);

    if(kStat != MStatus::kSuccess)
    {
        NIASSERT(0);

        displayError("Could not parse light type argument");

        return MStatus::kFailure;
    }

    if(iLightType >= NiStandardMaterial::LightType::LIGHT_MAX)
    {
        NIASSERT(0);

        displayError("Invalid Light Type");

        return MStatus::kFailure;
    }

    NiShadowTechnique* pkShadowTechnique = 
        NiShadowManager::GetKnownShadowTechnique(iTechniqueIndex);
        
    if (pkShadowTechnique)
    {
         fBias = pkShadowTechnique->GetDefaultDepthBias(
             (NiStandardMaterial::LightType)iLightType, true);
    }
    else
    {
        NIASSERT(0);

        displayError("Could not load shadow technique");

        return MStatus::kFailure;
    }
    

    NiShadowManager::Shutdown();

    clearResult(); 
    setResult(fBias);  

    return MStatus::kSuccess;
}

void* GetDefaultShadowBias::creator()
{
    return new GetDefaultShadowBias();
}