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

#include "NiCommonMaterialLibPCH.h"

#include "NiFlatWireframeMaterial.h"
#include <NiShaderFactory.h>

NiImplementRTTI(NiFlatWireframeMaterial, NiMaterial);

//---------------------------------------------------------------------------
NiFlatWireframeMaterial::NiFlatWireframeMaterial() : 
    NiMaterial("NiFlatWireframeMaterial")
{
}
//---------------------------------------------------------------------------
NiFlatWireframeMaterial::~NiFlatWireframeMaterial()
{
}
//---------------------------------------------------------------------------
bool NiFlatWireframeMaterial::IsShaderCurrent(NiShader* pkShader, 
    const NiGeometry* pkGeometry, const NiSkinInstance* pkSkin, 
    const NiPropertyState* pkState, const NiDynamicEffectState* pkEffects,
    unsigned int uiMaterialExtraData)
{
    if (pkShader == m_spSkinnedShader || pkShader == m_spUnskinnedShader)
        return true;

    return false;
}
//---------------------------------------------------------------------------
NiShader* NiFlatWireframeMaterial::GetCurrentShader(
    const NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, const NiPropertyState* pkState, 
    const NiDynamicEffectState* pkEffects,
    unsigned int uiMaterialExtraData)
{
    return GetCurrentShader(pkSkin);
}
//---------------------------------------------------------------------------
NiShader* NiFlatWireframeMaterial::GetCurrentShader(
    const NiSkinInstance* pkSkin)
{
    if (pkSkin && pkSkin->GetSkinPartition() != NULL)
    {
        if (m_spSkinnedShader == NULL)
        {
            NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
            if (pkFactory)
            {
                m_spSkinnedShader = pkFactory->RetrieveShader(
                    "FlatWireframeSkinned", NiShader::DEFAULT_IMPLEMENTATION);
            }
        }
        return m_spSkinnedShader;
    }
    else
    {
        if (m_spUnskinnedShader == NULL)
        {
            NiShaderFactory* pkFactory = NiShaderFactory::GetInstance();
            if (pkFactory)
            {
                m_spUnskinnedShader = pkFactory->RetrieveShader(
                    "FlatWireframe", NiShader::DEFAULT_IMPLEMENTATION);
            }
        }
        return m_spUnskinnedShader;
    }
}
//---------------------------------------------------------------------------
void NiFlatWireframeMaterial::UnloadShaders()
{
    m_spSkinnedShader = NULL;
    m_spUnskinnedShader = NULL;
}
//---------------------------------------------------------------------------
bool NiFlatWireframeMaterial::GetVertexInputSemantics(NiGeometry* pkGeometry, 
    unsigned int uiMaterialExtraData,
    NiShaderDeclaration::ShaderRegisterEntry* pakSemantics)
{
    NIASSERT(pkGeometry);
    NiShader* pkShader = GetCurrentShader(pkGeometry->GetSkinInstance());
    if (!pkShader)
    {
        pakSemantics[0].m_uiUsageIndex = NiMaterial::VS_INPUTS_TERMINATE_ARRAY;
        return false;
    }

    return pkShader->GetVertexInputSemantics(pakSemantics);
}
//---------------------------------------------------------------------------
