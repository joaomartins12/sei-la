// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2007 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef NIFXLTECHNIQUE_H
#define NIFXLTECHNIQUE_H

#include "NiFXLShaderLibType.h"
#include "NiFXLDefines.h"
//#include <NiD3DRendererHeaders.h>
#include <NiShaderRequirementDesc.h>
#include "NiFXLShader.h"
#include "NiFXLParameter.h"

NiSmartPointer(NiFXLShader);

class NiFXLFile;
class NiFXLParameter;
class NiFXLTechniqueDesc;
class NiShaderDesc;

class NIFXLSHADER_ENTRY NiFXLTechnique : public NiRefObject
{
public:
    NiFXLTechnique();
    ~NiFXLTechnique();

    void Initialize(NiFXLTechniqueDesc* pkTechDesc);

    const char* GetName() const;
    const char* GetDescription() const;
    const char* GetTechniqueName() const;
    unsigned int GetImplementation() const;
    unsigned int GetNumPasses() const;
    FXLTechniquePtr GetTechniquePtr() const;
    NiFXLFile* GetFile() const;
    
    // Get the D3D shader
    NiFXLShader* GetD3DShader(NiD3DRenderer* pkRenderer);
    bool ReleaseShader();

    // *** begin Emergent internal use only ***

    // These functions are used during the creation
    void SetTechniquePtr(FXLTechniquePtr pkTechnique);
    void SetFile(NiFXLFile* pkFile);

    void AddParameter(NiFXLParameter* pkParam);
    NiFXLParameter* GetFirstParameter();
    NiFXLParameter* GetNextParameter();
    NiFXLParameter* GetParameter(const char* pcParamName);
    bool GetTimeParameterPresent() const;

    NiShaderRequirementDesc* GetRequirements();

    void LinkHandle(FXLEffect* pkEffect);

    void DestroyRendererData();

    // *** end Emergent internal use only ***

protected:
    char* m_pcTechniqueName;
    unsigned int m_uiImplementation;

    FXLTechniquePtr m_pkTechnique;
    NiFXLFile* m_pkFile;

    NiFXLShaderPtr m_spShader;

    NiTPointerList<NiFXLParameter*> m_kParameters;
    NiTListIterator m_kParamIter;

    unsigned int m_uiNumPasses;

    bool m_bTimeParameter;

    bool m_bUsesNiRenderState;
    bool m_bUsesNiLightState;

    NiShaderRequirementDesc* m_pkReqDesc;
};

NiSmartPointer(NiFXLTechnique);

#include "NiFXLTechnique.inl"

#endif  //NIFXLTECHNIQUE_H
