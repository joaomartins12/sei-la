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

#include "NiScreenTextureRenderClick.h"

NiImplementRTTI(NiScreenTextureRenderClick, NiRenderClick);

//---------------------------------------------------------------------------
unsigned int NiScreenTextureRenderClick::GetNumObjectsDrawn() const
{
    return m_uiNumObjectsDrawn;
}
//---------------------------------------------------------------------------
float NiScreenTextureRenderClick::GetCullTime() const
{
    // This render click does not perform any culling, so the cull time is
    // always zero.
    return 0.0f;
}
//---------------------------------------------------------------------------
float NiScreenTextureRenderClick::GetRenderTime() const
{
    return m_fRenderTime;
}
//---------------------------------------------------------------------------
void NiScreenTextureRenderClick::PerformRendering(unsigned int uiFrameID)
{
    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Set up the renderer's camera data. Not strictly necessary for the
    // view transform or frustum, but needed for the viewport.
    pkRenderer->SetScreenSpaceCameraData(&m_kViewport);

    // Draw the NiScreenTexture objects.
    float fBeginTime = NiGetCurrentTimeInSec();
    NiTListIterator kIter = m_kScreenTextures.GetHeadPos();
    while (kIter)
    {
        NiScreenTexture* pkScreenTexture = m_kScreenTextures.GetNext(kIter);
        NIASSERT(pkScreenTexture);
        pkScreenTexture->Draw(pkRenderer);
    }

    // Update rendering statistics.
    m_fRenderTime = NiGetCurrentTimeInSec() - fBeginTime;
    m_uiNumObjectsDrawn = m_kScreenTextures.GetSize();
}
//---------------------------------------------------------------------------
