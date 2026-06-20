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

#include "NiScreenFillingRenderView.h"

NiImplementRTTI(NiScreenFillingRenderView, NiRenderView);

NiScreenElementsData* NiScreenFillingRenderView::ms_pkScreenFillingQuadData =
    NULL;
int NiScreenFillingRenderView::ms_iQuadDataRefCount = 0;
NiCriticalSection NiScreenFillingRenderView::ms_kQuadDataLock;

//---------------------------------------------------------------------------
NiScreenFillingRenderView::NiScreenFillingRenderView() :
    m_bPropertiesChanged(false), m_bEffectsChanged(false)
{
    // Create the screen-filling quad data object if it doesn't already exist.
    ms_kQuadDataLock.Lock();
    if (!ms_pkScreenFillingQuadData)
    {
        NIASSERT(ms_iQuadDataRefCount == 0);

        ms_pkScreenFillingQuadData = NiNew NiScreenElementsData(false, false,
            1, 1, 1, 4, 4, 2, 2);

        int iPolygon = ms_pkScreenFillingQuadData->Insert(4);
        ms_pkScreenFillingQuadData->SetRectangle(iPolygon, 0.0f, 0.0f, 1.0f,
            1.0f);

        NiPoint2 akTexCoords[4] = {NiPoint2(0.0f, 0.0f), NiPoint2(0.0f, 1.0f),
            NiPoint2(1.0f, 1.0f), NiPoint2(1.0f, 0.0f)};
        ms_pkScreenFillingQuadData->SetTextures(iPolygon, 0, akTexCoords);
    }
    ms_kQuadDataLock.Unlock();

    // Create the screen-filling quad using the shared geometry data.
    m_spScreenFillingQuad = NiNew NiScreenElements(ms_pkScreenFillingQuadData);
    NiAtomicIncrement(ms_iQuadDataRefCount);
    m_spScreenFillingQuad->Update(0.0f);

    // Create an effect state to use for dynamic effects.
    m_spEffectState = NiNew NiDynamicEffectState;
}
//---------------------------------------------------------------------------
NiScreenFillingRenderView::~NiScreenFillingRenderView()
{
    NiAtomicDecrement(ms_iQuadDataRefCount);

    ms_kQuadDataLock.Lock();
    if (ms_iQuadDataRefCount <= 0)
    {
        ms_pkScreenFillingQuadData = NULL;
        ms_iQuadDataRefCount = 0;
    }
    ms_kQuadDataLock.Unlock();
}
//---------------------------------------------------------------------------
void NiScreenFillingRenderView::SetCameraData(const NiRect<float>& kViewport)
{
    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Set screen-space camera data using viewport.
    pkRenderer->SetScreenSpaceCameraData(&kViewport);
}
//---------------------------------------------------------------------------
void NiScreenFillingRenderView::CalculatePVGeometry()
{
    NIASSERT(m_kCachedPVGeometry.GetCount() == 0);
    NIASSERT(m_spScreenFillingQuad);

    // Update properties, if necessary.
    if (m_bPropertiesChanged)
    {
        m_spScreenFillingQuad->UpdateProperties();
        m_bPropertiesChanged = false;
    }

    // Update effects, if necessary.
    if (m_bEffectsChanged)
    {
        NiDynamicEffectState* pkParentState = NULL;
        if (!m_kEffectList.IsEmpty())
        {
            NIASSERT(m_spEffectState);
            pkParentState = m_spEffectState;
            NiTListIterator kIter = m_kEffectList.GetHeadPos();
            while (kIter)
            {
                pkParentState->AddEffect(m_kEffectList.GetNext(kIter));
            }
        }
        m_spScreenFillingQuad->UpdateEffectsDownward(pkParentState);
        m_bEffectsChanged = false;
    }

    // Add screen-filling quad to PV geometry array.
    m_kCachedPVGeometry.Add(*m_spScreenFillingQuad);
}
//---------------------------------------------------------------------------
