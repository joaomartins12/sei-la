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

#include "NiPSysModifierBoolCtlr.h"
#include <NiBoolInterpolator.h>
#include <NiBlendBoolInterpolator.h>
#include <NiBoolTimelineInterpolator.h>

NiImplementRTTI(NiPSysModifierBoolCtlr, NiPSysModifierCtlr);

//---------------------------------------------------------------------------
NiPSysModifierBoolCtlr::NiPSysModifierBoolCtlr(const char* pcModifierName) :
    NiPSysModifierCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysModifierBoolCtlr::NiPSysModifierBoolCtlr()
{
}
//---------------------------------------------------------------------------
NiInterpolator* NiPSysModifierBoolCtlr::CreatePoseInterpolator(
    unsigned short usIndex)
{
    NIASSERT(usIndex == 0);
    bool bValue;
    GetTargetBoolValue(bValue);
    return NiNew NiBoolInterpolator(bValue);
}
//---------------------------------------------------------------------------
void NiPSysModifierBoolCtlr::SynchronizePoseInterpolator(NiInterpolator* 
    pkInterp, unsigned short usIndex)
{
    NiBoolInterpolator* pkBoolInterp = 
        NiDynamicCast(NiBoolInterpolator, pkInterp);

    NIASSERT(usIndex == 0);
    NIASSERT(pkBoolInterp);
    bool bValue;
    GetTargetBoolValue(bValue);
    pkBoolInterp->SetPoseValue(bValue);
}
//---------------------------------------------------------------------------
NiBlendInterpolator* NiPSysModifierBoolCtlr::CreateBlendInterpolator(
    unsigned short usIndex, bool bManagerControlled,
    bool bAccumulateAnimations, float fWeightThreshold,
    unsigned char ucArraySize) const
{
    NIASSERT(usIndex == 0);
    return NiNew NiBlendBoolInterpolator(bManagerControlled, fWeightThreshold,
        ucArraySize);
}
//---------------------------------------------------------------------------
void NiPSysModifierBoolCtlr::Update(float fTime)
{
    if (GetManagerControlled())
    {
        m_fScaledTime = INVALID_TIME;
    }
    else if (DontDoUpdate(fTime) &&
        (!m_spInterpolator || !m_spInterpolator->AlwaysUpdate()))
    {
        return;
    }

    if (m_spInterpolator)
    {
        bool bValue;
        if (m_spInterpolator->Update(m_fScaledTime, m_pkTarget, bValue))
        {
            SetTargetBoolValue(bValue);
        }
    }
}
//---------------------------------------------------------------------------
bool NiPSysModifierBoolCtlr::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, unsigned short usIndex) const
{
    NIASSERT(usIndex == 0);
    return pkInterpolator->IsBoolValueSupported();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSysModifierBoolCtlr::CopyMembers(NiPSysModifierBoolCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifierCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSysModifierBoolCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierCtlr::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        kStream.ReadLinkID();   // m_spBoolData
    }
}
//---------------------------------------------------------------------------
void NiPSysModifierBoolCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierCtlr::LinkObject(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        NiBoolData* pkBoolData = (NiBoolData*) kStream.GetObjectFromLinkID();
        m_spInterpolator = NiNew NiBoolTimelineInterpolator(pkBoolData);
    }
}
//---------------------------------------------------------------------------
bool NiPSysModifierBoolCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysModifierBoolCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysModifierBoolCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysModifierBoolCtlr::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifierCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPSysModifierBoolCtlr::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
