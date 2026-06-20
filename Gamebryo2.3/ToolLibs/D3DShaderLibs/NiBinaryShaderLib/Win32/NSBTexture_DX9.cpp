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
#include "NiBinaryShaderLibPCH.h"

// Only include this file if it's a WIN32 build!
#if defined(WIN32)
//---------------------------------------------------------------------------
#include "NSBTexture.h"
#include "NSBUtility.h"

#include <NiD3DUtility.h>
#include <NiD3DTextureStage.h>

//---------------------------------------------------------------------------
bool NSBTexture::SetupD3DTextureStage_DX9(NiD3DTextureStage& kD3DStage)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, NiRenderer::GetRenderer()));
    NIASSERT(kD3DStage.GetStage() == m_uiStage);

    // Override all texture flags except for 
    // NiD3DTextureStage::TSTF_MAP_USE_INDEX
    unsigned int uiCurrentTextureFlags = kD3DStage.GetTextureFlags();
    kD3DStage.SetTextureFlags(
        (uiCurrentTextureFlags & NiD3DTextureStage::TSTF_MAP_USE_INDEX) |
        (m_uiTextureFlags & ~NiD3DTextureStage::TSTF_MAP_USE_INDEX));

    // Override object texture flags
    kD3DStage.SetObjTextureFlags(m_usObjTextureFlags);

    return true;
}
//---------------------------------------------------------------------------
bool NSBTexture::SetupD3DTextureStage_Xenon(NiD3DTextureStage& kD3DStage)
{
    NIASSERT(!"NSBTexture::SetupD3DTextureStage_Xenon> Win32 BUILD!");
    return false;
}
//---------------------------------------------------------------------------
#endif  //#if defined(WIN32)
//---------------------------------------------------------------------------
