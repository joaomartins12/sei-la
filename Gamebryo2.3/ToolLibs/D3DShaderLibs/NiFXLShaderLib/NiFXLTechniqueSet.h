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

#ifndef NIFXLTECHNIQUESET_H
#define NIFXLTECHNIQUESET_H

#include "NiFXLShaderLibType.h"
#include "NiFXLTechnique.h"
#include <NiD3DRendererHeaders.h>

NiSmartPointer(NiShaderDesc);

class NiFXLAttributeTable;
class NiFXLShader;

class NIFXLSHADER_ENTRY NiFXLTechniqueSet : public NiRefObject
{
public:
    NiFXLTechniqueSet();
    ~NiFXLTechniqueSet();

    const char* GetName() const;
    unsigned int GetImplementationCount() const;

    // Get the D3D shader
    NiFXLShader* GetD3DShader(NiD3DRenderer* pkRenderer,
        unsigned int uiImplementation);
    NiFXLShader* GetBestD3DShader(NiD3DRenderer* pkRenderer);

    // *** begin Emergent internal use only ***
    NiShaderDesc* GetShaderDesc(NiFXLAttributeTable* pkAttribTable);
    NiFXLTechnique* GetTechnique(unsigned int uiImplementation);
    unsigned int GetTechniqueArrayCount() const;

    void AddImplementation(NiFXLTechnique* pkTechnique);

    void DestroyRendererData();

    // These functions are used during the creation
    void SetName(const char* pcName);

    // *** end Emergent internal use only ***

protected:
    char* m_pcName;

    unsigned int m_uiMinPixelShaderVersion;
    unsigned int m_uiMaxPixelShaderVersion;
    unsigned int m_uiMinVertexShaderVersion;
    unsigned int m_uiMaxVertexShaderVersion;
    unsigned int m_uiMinUserVersion;
    unsigned int m_uiMaxUserVersion;

    NiTObjectArray<NiFXLTechniquePtr> m_kImplementations;
    NiFXLTechnique* m_pkBestImplementation;

    NiShaderDescPtr m_spShaderDesc;
};

NiSmartPointer(NiFXLTechniqueSet);

#endif  //NIFXLTECHNIQUESET_H
