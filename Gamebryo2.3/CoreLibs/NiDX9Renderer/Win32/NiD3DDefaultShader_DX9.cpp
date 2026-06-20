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
#include "NiD3DRendererPCH.h"

#include "NiD3DDefaultShader.h"
#include "NiDX9Renderer.h"
//---------------------------------------------------------------------------
void NiD3DDefaultShader::InitializeDeviceCaps()
{
    const D3DCAPS9* pkCaps = m_pkD3DRenderer->GetDeviceCaps();

    m_bBlendTextureAlpha = 
        ((pkCaps->TextureOpCaps & D3DTEXOPCAPS_BLENDTEXTUREALPHA) != 0);

    if (pkCaps->TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAPLUMINANCE)
        m_eEnvBumpOp = D3DTOP_BUMPENVMAPLUMINANCE;
    else if (pkCaps->TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP)
        m_eEnvBumpOp = D3DTOP_BUMPENVMAP;
}
//---------------------------------------------------------------------------


