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

#include "NiPSysFieldMagnitudeCtlr.h"
#include "NiPSysFieldModifier.h"

NiImplementRTTI(NiPSysFieldMagnitudeCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysFieldMagnitudeCtlr::NiPSysFieldMagnitudeCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysFieldMagnitudeCtlr::NiPSysFieldMagnitudeCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysFieldMagnitudeCtlr::InterpTargetIsCorrectType(
    NiObjectNET* pkTarget) const
{
    if (!NiPSysModifierCtlr::InterpTargetIsCorrectType(pkTarget))
    {
        return false;
    }

    NiPSysModifier* pkModifier = ((NiParticleSystem*) pkTarget)
        ->GetModifierByName(m_kModifierName);
    NIASSERT(pkModifier);

    return NiIsKindOf(NiPSysFieldModifier, pkModifier);
}
//---------------------------------------------------------------------------
void NiPSysFieldMagnitudeCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysFieldModifier*) m_pkModifier)->GetMagnitude();
}
//---------------------------------------------------------------------------
void NiPSysFieldMagnitudeCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysFieldModifier*) m_pkModifier)->SetMagnitude(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysFieldMagnitudeCtlr);
//---------------------------------------------------------------------------
void NiPSysFieldMagnitudeCtlr::CopyMembers(NiPSysFieldMagnitudeCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysFieldMagnitudeCtlr);
//---------------------------------------------------------------------------
void NiPSysFieldMagnitudeCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysFieldMagnitudeCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysFieldMagnitudeCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysFieldMagnitudeCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysFieldMagnitudeCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysFieldMagnitudeCtlr::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysFieldMagnitudeCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
