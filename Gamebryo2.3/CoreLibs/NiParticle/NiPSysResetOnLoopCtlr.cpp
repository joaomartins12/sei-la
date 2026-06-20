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
#include "NiParticlePCH.h"

#include "NiPSysResetOnLoopCtlr.h"
#include "NiParticleSystem.h"

NiImplementRTTI(NiPSysResetOnLoopCtlr, NiTimeController);

//---------------------------------------------------------------------------
NiPSysResetOnLoopCtlr::NiPSysResetOnLoopCtlr() :
    m_fLastScaledTime(-NI_INFINITY)
{
}
//---------------------------------------------------------------------------
void NiPSysResetOnLoopCtlr::Update(float fTime)
{
    SetCycleType(LOOP);

    if (!m_pkTarget || DontDoUpdate(fTime))
    {
        return;
    }

    if (m_fLastScaledTime == -NI_INFINITY)
    {
        m_fLastScaledTime = m_fScaledTime;
    }

    if (m_fLastScaledTime > m_fScaledTime)
    {
        // Controller has looped, so reset particle system.
        ((NiParticleSystem*) m_pkTarget)->ResetParticleSystem();
    }

    m_fLastScaledTime = m_fScaledTime;
}
//---------------------------------------------------------------------------
bool NiPSysResetOnLoopCtlr::TargetIsRequiredType() const
{
    return NiIsKindOf(NiParticleSystem, m_pkTarget);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysResetOnLoopCtlr);
//---------------------------------------------------------------------------
void NiPSysResetOnLoopCtlr::CopyMembers(NiPSysResetOnLoopCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiTimeController::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysResetOnLoopCtlr);
//---------------------------------------------------------------------------
void NiPSysResetOnLoopCtlr::LoadBinary(NiStream& kStream)
{
    NiTimeController::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysResetOnLoopCtlr::LinkObject(NiStream& kStream)
{
    NiTimeController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysResetOnLoopCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiTimeController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysResetOnLoopCtlr::SaveBinary(NiStream& kStream)
{
    NiTimeController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysResetOnLoopCtlr::IsEqual(NiObject* pkObject)
{
    return NiTimeController::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysResetOnLoopCtlr::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiTimeController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysResetOnLoopCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
