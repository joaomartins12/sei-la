#include "MayaPluginPCH.h"
#include "GetShadowTechniquesCommand.h"
#include "NiShadowManager.h"

MStatus GetShadowTechniques::doIt( const MArgList& args )
{
    MStringArray kTechniqueNames;

    NiShadowManager::Initialize();

    for (int iTechniqueIndex = 0;
         iTechniqueIndex < NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES; 
         iTechniqueIndex++)
    {
         NiShadowTechnique* pkShadowTechnique =
             NiShadowManager::GetKnownShadowTechnique(iTechniqueIndex);
         if (pkShadowTechnique)
         {
            MString kName(pkShadowTechnique->GetName());
            kTechniqueNames.append(kName);   
         }
    }

    NiShadowManager::Shutdown();

    clearResult(); 
    setResult(kTechniqueNames);  

    return MStatus::kSuccess;
}

void* GetShadowTechniques::creator()
{
    return new GetShadowTechniques();
}