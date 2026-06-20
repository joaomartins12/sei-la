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

#ifndef NIFXLFILE_H
#define NIFXLFILE_H

#include "NiFXLAttributeTable.h"
#include "NiFXLShaderLibType.h"
#include "NiFXLTechniqueSet.h"

#include <Fxl.h>

class NiShaderDesc;
class NiFXLShader;

class NIFXLSHADER_ENTRY NiFXLFile : public NiMemObject
{

public:
    NiFXLFile();
    ~NiFXLFile();

    bool Initialize(FXLEffect* pkFXL);

    // Name
    const char* GetName() const;
    
    // Get the D3D shader
    NiFXLShader* GetD3DShader(NiD3DRenderer* pkRenderer, 
        const char* pcName, unsigned int uiImplementation);
    bool ReleaseShader(const char* pcName, unsigned int uiImplementation);

    // *** begin Emergent internal use only ***
    FXLEffect* GetEffect() const;
    FXLEffect* GetRenderableEffect() const;

    NiFXLTechnique* GetFirstTechnique();
    NiFXLTechnique* GetNextTechnique();

    NiFXLTechnique* GetTechnique(const char* pcName, 
        unsigned int uiImplementation) const;

    NiShaderDesc* GetFirstShaderDesc();
    NiShaderDesc* GetNextShaderDesc();

    bool ParamIsUsed(NiFXLParameter* pkParam, NiFXLTechnique* pkTechnique);

    // These functions are used during the creation
    void SetName(const char* pszName);
    void SetEffect(FXLEffect* pkEffect);
    void SetDXVersion(unsigned int uiDXVersion);

    void DestroyRendererData();

    // *** end Emergent internal use only ***

protected:
    bool ParseEffect();
    void AddTechnique(NiFXLTechnique* pkTechnique);
    bool LinkHandles();

    char* m_pcName;
    unsigned int m_uiDXVersion;

    NiTStringMap<NiFXLTechniqueSetPtr> m_kTechniques;
    NiTPointerList<NiFXLParameter*> m_kParameters;
    NiTMapIterator m_kTechniqueIter;
    unsigned int m_uiIndex;
    NiFXLTechniqueSet* m_pkCurrTechniqueSet;

    NiFXLAttributeTable m_kAttribTable;

    FXLEffect* m_pkFXLEffect;
    bool m_bRenderableEffect;
    bool m_bParsed;
};

#endif  //NIFXLFILE_H
