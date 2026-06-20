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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiSingleShaderMaterial.h"
#include "NiShaderFactory.h"

NiImplementRTTI(NiSingleShaderMaterial, NiMaterial);

//---------------------------------------------------------------------------
NiSingleShaderMaterial::NiSingleShaderMaterial(const NiFixedString& kName) :
    NiMaterial(kName), m_bManualShaderManagement(false)
{
    /* */
}
//---------------------------------------------------------------------------
NiSingleShaderMaterial::NiSingleShaderMaterial(const NiFixedString& kName, 
    NiShader* pkShader, bool bManualShaderManagement) :
    NiMaterial(kName)
{
    NIASSERT(pkShader);
    m_kShaderName = pkShader->GetName();
    m_spCachedShader = pkShader;
    m_bManualShaderManagement = bManualShaderManagement;
    if (!pkShader->IsInitialized())
        pkShader->Initialize();
}
//---------------------------------------------------------------------------
NiSingleShaderMaterial::NiSingleShaderMaterial(const NiFixedString& kName, 
    const char* pcName, bool bPreloadShader) :
    NiMaterial(kName), m_kShaderName(pcName), m_bManualShaderManagement(false)
{
    if (bPreloadShader)
    {
        // Attempt to preload at least one implementation of the shader,
        // but don't generate an error yet if one can't be found.
        NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
        NiShaderPtr spShader = pkFactory->RetrieveShader(pcName, 
            NiShader::DEFAULT_IMPLEMENTATION, false);
        spShader = NULL;
    }
}
//---------------------------------------------------------------------------
NiSingleShaderMaterial::~NiSingleShaderMaterial()
{
    /* */
}
//---------------------------------------------------------------------------
NiSingleShaderMaterial* NiSingleShaderMaterial::Create(NiShader* pkShader,
    bool bManualShaderManagement)
{
    if (pkShader == NULL)
        return NULL;

    NiFixedString kMaterialName = pkShader->GetName();

    NiMaterial* pkMaterial = GetMaterial(kMaterialName);
    if (pkMaterial)
    {
        NIASSERT(NiIsKindOf(NiSingleShaderMaterial, pkMaterial));
        return (NiSingleShaderMaterial*)pkMaterial;
    }
    else
    {
        return NiNew NiSingleShaderMaterial(kMaterialName, pkShader,
            bManualShaderManagement);
    }
}
//---------------------------------------------------------------------------
NiSingleShaderMaterial* NiSingleShaderMaterial::Create(const char* pcName,
    bool bPreloadShader)
{
    NiMaterial* pkMaterial = GetMaterial(pcName);

    if (pkMaterial)
    {
        NIASSERT(NiIsKindOf(NiSingleShaderMaterial, pkMaterial));
        return (NiSingleShaderMaterial*)pkMaterial;
    }
    else
    {
        return NiNew NiSingleShaderMaterial(pcName, pcName, bPreloadShader);
    }
}
//---------------------------------------------------------------------------
bool NiSingleShaderMaterial::IsShaderCurrent(NiShader* pkShader, 
    const NiGeometry*, const NiSkinInstance*, const NiPropertyState*, 
    const NiDynamicEffectState*, unsigned int uiMaterialExtraData)
{
    if (pkShader && pkShader->GetName() == m_kShaderName)
        return true;

    return false;
}
//---------------------------------------------------------------------------
NiShader* NiSingleShaderMaterial::GetCurrentShader(const NiGeometry* pkGeom, 
    const NiSkinInstance* pkSkin, const NiPropertyState* pkPropState, 
    const NiDynamicEffectState* pkEffectState,unsigned int uiMaterialExtraData)
{
    NiShaderFactory* pkShaderFactory = NiShaderFactory::GetInstance();
    if (m_spCachedShader)
    {
        if (m_spCachedShader->GetImplementation() == uiMaterialExtraData)
        {
            return m_spCachedShader;
        }
        else if (uiMaterialExtraData == NiShader::DEFAULT_IMPLEMENTATION &&
            pkShaderFactory && pkShaderFactory->IsDefaultImplementation(
            m_spCachedShader))
        {
            return m_spCachedShader;
        }
    }

    if (pkShaderFactory == NULL)
        return NULL;

    NiShader* pkShader = 
        pkShaderFactory->RetrieveShader(m_kShaderName, uiMaterialExtraData);

    if (pkShader)
    {
        if (!pkShader->IsInitialized() && !pkShader->Initialize())
            return NULL;
            
        return pkShader;
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiSingleShaderMaterial::UnloadShaders()
{
    if (!m_bManualShaderManagement)
       m_spCachedShader = NULL; 
}
//---------------------------------------------------------------------------
void NiSingleShaderMaterial::SetCachedShader(NiShader* pkShader)
{
   NIASSERT(!pkShader || m_kShaderName == pkShader->GetName());
   m_spCachedShader = pkShader; 
}
//---------------------------------------------------------------------------
bool NiSingleShaderMaterial::GetManualShaderManagement()
{
    return m_bManualShaderManagement;
}
//---------------------------------------------------------------------------
bool NiSingleShaderMaterial::GetVertexInputSemantics(NiGeometry* pkGeometry, 
    unsigned int uiMaterialExtraData,
    NiShaderDeclaration::ShaderRegisterEntry* pakSemantics)
{
    NiShaderPtr spShader;
    NiShaderFactory* pkShaderFactory = NiShaderFactory::GetInstance();
    if (m_spCachedShader)
    {
        if (m_spCachedShader->GetImplementation() == uiMaterialExtraData)
        {
            spShader = m_spCachedShader;
        }
        else if (uiMaterialExtraData == NiShader::DEFAULT_IMPLEMENTATION &&
            pkShaderFactory && pkShaderFactory->IsDefaultImplementation(
            m_spCachedShader))
        {
            spShader = m_spCachedShader;
        }
    }

    if (spShader == NULL)
    {
        spShader = pkShaderFactory->RetrieveShader(
            m_kShaderName, uiMaterialExtraData);
    }

    if (spShader == NULL)
    {
        pakSemantics[0].m_uiUsageIndex = NiMaterial::VS_INPUTS_TERMINATE_ARRAY;
        return false;
    }

    // Copy the data over.
    return spShader->GetVertexInputSemantics(pakSemantics);
}
