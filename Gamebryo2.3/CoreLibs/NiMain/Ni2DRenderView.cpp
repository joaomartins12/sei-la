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

#include "Ni2DRenderView.h"

NiImplementRTTI(Ni2DRenderView, NiRenderView);

//---------------------------------------------------------------------------
void Ni2DRenderView::SetCameraData(const NiRect<float>& kViewport)
{
    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Set screen-space camera data using viewport.
    pkRenderer->SetScreenSpaceCameraData(&kViewport);
}
//---------------------------------------------------------------------------
void Ni2DRenderView::CalculatePVGeometry()
{
    NIASSERT(m_kCachedPVGeometry.GetCount() == 0);

    // Add all screen elements to the PV geometry array.
    NiTListIterator kIter = m_kScreenElements.GetHeadPos();
    while (kIter)
    {
        NiScreenElements* pkScreenElement = m_kScreenElements.GetNext(kIter);
        NIASSERT(pkScreenElement);
        m_kCachedPVGeometry.Add(*pkScreenElement);
    }
}
//---------------------------------------------------------------------------
