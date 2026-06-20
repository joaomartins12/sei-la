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

#include "NiPSysModifierFloatCtlr.h"
#include <NiFloatInterpolator.h>
#include <NiBlendFloatInterpolator.h>
#include <NiFloatData.h>
#include <NiFloatInterpolator.h>

NiImplementRTTI(NiPSysModifierFloatCtlr, NiPSysModifierCtlr);

//---------------------------------------------------------------------------
NiPSysModifierFloatCtlr::NiPSysModifierFloatCtlr(const char* pcModifierName)
    : NiPSysModifierCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysModifierFloatCtlr::NiPSysModifierFloatCtlr()
{
}
//---------------------------------------------------------------------------
NiInterpolator* NiPSysModifierFloatCtlr::CreatePoseInterpolator(
    unsigned short usIndex)
{
    NIASSERT(usIndex == 0);
    float fValue;
    GetTargetFloatValue(fValue);
    return NiNew NiFloatInterpolator(fValue);
}
//---------------------------------------------------------------------------
void NiPSysModifierFloatCtlr::SynchronizePoseInterpolator(
    NiInterpolator* pkInterp, unsigned short usIndex)
{
    NiFloatInterpolator* pkFloatInterp = 
        NiDynamicCast(NiFloatInterpolator, pkInterp);

    NIASSERT(usIndex == 0);
    NIASSERT(pkFloatInterp);
    float fValue;
    GetTargetFloatValue(fValue);
    pkFloatInterp->SetPoseValue(fValue);
}
//---------------------------------------------------------------------------
NiBlendInterpolator* NiPSysModifierFloatCtlr::CreateBlendInterpolator(
    unsigned short usIndex, bool bManagerControlled,
    bool bAccumulateAnimations, float fWeightThreshold,
    unsigned char ucArraySize) const
{
    NIASSERT(usIndex == 0);
    return NiNew NiBlendFloatInterpolator(bManagerControlled, fWeightThreshold,
        ucArraySize);
}
//---------------------------------------------------------------------------
void NiPSysModifierFloatCtlr::Update(float fTime)
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
        float fValue;
        if (m_spInterpolator->Update(m_fScaledTime, m_pkTarget, fValue))
        {
            SetTargetFloatValue(fValue);
        }
    }
}
//---------------------------------------------------------------------------
bool NiPSysModifierFloatCtlr::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, unsigned short usIndex) const
{
    NIASSERT(usIndex == 0);
    return pkInterpolator->IsFloatValueSupported();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSysModifierFloatCtlr::CopyMembers(NiPSysModifierFloatCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifierCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSysModifierFloatCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierCtlr::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        kStream.ReadLinkID();   // m_spData
    }
}
//---------------------------------------------------------------------------
void NiPSysModifierFloatCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierCtlr::LinkObject(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        NiFloatData* pkFloatData = (NiFloatData*)
            kStream.GetObjectFromLinkID();
        m_spInterpolator = NiNew NiFloatInterpolator(pkFloatData);
    }
}
//---------------------------------------------------------------------------
bool NiPSysModifierFloatCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysModifierFloatCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysModifierFloatCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysModifierFloatCtlr::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifierCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPSysModifierFloatCtlr::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
