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

#include "NiShadowClickValidator.h"
#include "NiShadowRenderClick.h"
#include "NiShadowGenerator.h"

NiImplementRTTI(NiShadowClickValidator, NiRenderClickValidator);

//---------------------------------------------------------------------------
bool NiShadowClickValidator::ValidateClick(NiRenderClick* pkRenderClick,
    unsigned int uiFrameID)
{
    // Cast render click to an NiShadowRenderClick.
    NiShadowRenderClick* pkShadowClick = NiDynamicCast(NiShadowRenderClick,
        pkRenderClick);
    if (!pkShadowClick)
    {
        // If the render click is not a shadow click, allow rendering.
        return true;
    }
    else if (pkShadowClick->GetForceRender())
    {
        // If the shadow click is set to force render, allow rendering.
        pkShadowClick->MarkAsDirty();
        return true;
    }

    // If no camera or culling process have been set, allow rendering.
    if (!m_spCamera || !m_spCullingProcess)
    {
        pkShadowClick->MarkAsDirty();
        return true;
    }

    m_kVisibleSet.RemoveAll();

    // Get shadow generator.
    NiShadowGenerator* pkGenerator = pkShadowClick->GetGenerator();
    NIASSERT(pkGenerator);

    // Loop over shadow receivers (affected nodes list), culling each to
    // determine its visibility. If any are visible, allow rendering.
    NiDynamicEffect* pkDynEffect = pkGenerator->GetAssignedDynamicEffect();
    NIASSERT(pkDynEffect);
    const NiNodeList& kReceivers = pkDynEffect->GetAffectedNodeList();
    NiTListIterator kIter = kReceivers.GetHeadPos();
    while (kIter)
    {
        NiNode* pkReceiver = kReceivers.GetNext(kIter);

        // Cull the shadow receiver against the camera using the culling
        // process.
        m_spCullingProcess->Process(
            m_spCamera, (NiAVObject*)pkReceiver, &m_kVisibleSet);

        // Determine whether or not the receiver is visible. If so, return
        // immediately allowing rendering.
        if (m_kVisibleSet.GetCount() > 0)
        {
            pkShadowClick->MarkAsDirty();
            return true;
        }
    }

    // No shadow receivers are visible, so rendering should not be performed.
    return false;
}
//---------------------------------------------------------------------------
