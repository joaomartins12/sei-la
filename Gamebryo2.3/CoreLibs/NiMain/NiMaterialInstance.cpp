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

#include "NiGeometry.h"
#include "NiMaterialInstance.h"

//---------------------------------------------------------------------------
NiShader* NiMaterialInstance::GetCachedShader(const NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, const NiPropertyState* pkState, 
    const NiDynamicEffectState* pkEffects) const
{
    if (m_spCachedShader)
    {
        if (m_eNeedsUpdate == DIRTY || (m_eNeedsUpdate == UNKNOWN && 
            pkGeometry->GetDefaultMaterialNeedsUpdateFlag()))
        {
            return NULL;
        }

        if (!m_spMaterial || m_spMaterial->IsShaderCurrent(m_spCachedShader, 
            pkGeometry, pkSkin, pkState, pkEffects, m_uiMaterialExtraData))
        {
            return m_spCachedShader;
        }
    }    

    return NULL;
}
//---------------------------------------------------------------------------
NiShader* NiMaterialInstance::GetCurrentShader(NiGeometry* pkGeometry, 
    const NiSkinInstance* pkSkin, const NiPropertyState* pkState, 
    const NiDynamicEffectState* pkEffects)
{
    if (m_spMaterial)
    {
        bool bGetNewShader = (m_spCachedShader == NULL);
        if (m_eNeedsUpdate == DIRTY || (m_eNeedsUpdate == UNKNOWN && 
            pkGeometry->GetDefaultMaterialNeedsUpdateFlag()))
        {
            bGetNewShader = true;
        }

        if (bGetNewShader && m_spCachedShader)
        {
            bGetNewShader = !m_spMaterial->IsShaderCurrent(m_spCachedShader, 
                pkGeometry, pkSkin, pkState, pkEffects, m_uiMaterialExtraData);
        }

        if (bGetNewShader)
        {
            NiShader* pkNewShader = m_spMaterial->GetCurrentShader(
                pkGeometry, pkSkin, pkState, pkEffects, m_uiMaterialExtraData);


            if (pkNewShader == NULL)
                return NULL;

            NIASSERT(m_spCachedShader != pkNewShader);

            m_spCachedShader = pkNewShader;
            pkNewShader->SetupGeometry(pkGeometry);
        }

        m_eNeedsUpdate = UNKNOWN;
    }

    return m_spCachedShader;
}
//---------------------------------------------------------------------------
void NiMaterialInstance::SetMaterialExtraData(unsigned int uiExtraData)
{
    m_uiMaterialExtraData = uiExtraData;
}
//---------------------------------------------------------------------------
unsigned int NiMaterialInstance::GetMaterialExtraData() const
{
    return m_uiMaterialExtraData;
}
//---------------------------------------------------------------------------
