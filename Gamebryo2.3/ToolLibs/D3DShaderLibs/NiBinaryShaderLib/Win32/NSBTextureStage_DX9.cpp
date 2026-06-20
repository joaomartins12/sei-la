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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiBinaryShaderLibPCH.h"

// Only include this file if it's a WIN32 build!
#if defined(WIN32)
//---------------------------------------------------------------------------
#include "NSBTextureStage.h"
#include "NSBUtility.h"

#include <NiD3DUtility.h>
#include <NiD3DTextureStage.h>

//---------------------------------------------------------------------------
bool NSBTextureStage::SetupD3DTextureStage_DX9(NiD3DTextureStage& kD3DStage)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    kD3DStage.SetStage(m_uiStage);
    kD3DStage.SetTextureFlags(m_uiTextureFlags);
    kD3DStage.SetTextureTransformFlags(m_uiTextureTransformFlags,
        m_pcGlobalEntry);
    kD3DStage.SetObjTextureFlags(m_usObjTextureFlags);

    NiD3DTextureStageGroup* pkStageGroup = kD3DStage.GetTextureStageGroup();
    if (m_pkTextureStageGroup)
    {
        if (m_pkTextureStageGroup->GetStateCount())
        {
            NIASSERT(pkStageGroup);
            if (!m_pkTextureStageGroup->SetupTextureStageGroup(
                *pkStageGroup))
            {
                NiDelete pkStageGroup;
                return false;
            }
        }
    }

    if (m_pkSamplerStageGroup)
    {
        if (m_pkSamplerStageGroup->GetStateCount())
        {
            NIASSERT(pkStageGroup);
            if (!m_pkSamplerStageGroup->SetupTextureSamplerGroup(
                *pkStageGroup))
            {
                NiDelete pkStageGroup;
                return false;
            }
        }
    }

    kD3DStage.SetTextureTransformation(m_kTextureTransformation);

    return true;
}
//---------------------------------------------------------------------------
bool NSBTextureStage::SetupD3DTextureStage_Xenon(NiD3DTextureStage& kD3DStage)
{
    NIASSERT(!"NSBTextureStage::SetupD3DTextureStage_Xenon> DX9 BUILD!");
    return false;
}
//---------------------------------------------------------------------------
#endif  //#if defined(WIN32)
//---------------------------------------------------------------------------
