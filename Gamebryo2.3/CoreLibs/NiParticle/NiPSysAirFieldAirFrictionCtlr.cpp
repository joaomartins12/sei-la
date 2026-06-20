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

#include "NiPSysAirFieldAirFrictionCtlr.h"
#include "NiPSysAirFieldModifier.h"
#include "NiPSysFieldModifier.h"

NiImplementRTTI(NiPSysAirFieldAirFrictionCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysAirFieldAirFrictionCtlr::NiPSysAirFieldAirFrictionCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysAirFieldAirFrictionCtlr::NiPSysAirFieldAirFrictionCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysAirFieldAirFrictionCtlr::InterpTargetIsCorrectType(
    NiObjectNET* pkTarget) const
{
    if (!NiPSysModifierCtlr::InterpTargetIsCorrectType(pkTarget))
    {
        return false;
    }

    NiPSysModifier* pkModifier = ((NiParticleSystem*) pkTarget)
        ->GetModifierByName(m_kModifierName);
    NIASSERT(pkModifier);

    return NiIsKindOf(NiPSysAirFieldModifier, pkModifier);
}
//---------------------------------------------------------------------------
void NiPSysAirFieldAirFrictionCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysAirFieldModifier*) m_pkModifier)->GetAirFriction();
}
//---------------------------------------------------------------------------
void NiPSysAirFieldAirFrictionCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysAirFieldModifier*) m_pkModifier)->SetAirFriction(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysAirFieldAirFrictionCtlr);
//---------------------------------------------------------------------------
void NiPSysAirFieldAirFrictionCtlr::CopyMembers(
    NiPSysAirFieldAirFrictionCtlr* pkDest, NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysAirFieldAirFrictionCtlr);
//---------------------------------------------------------------------------
void NiPSysAirFieldAirFrictionCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysAirFieldAirFrictionCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysAirFieldAirFrictionCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysAirFieldAirFrictionCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysAirFieldAirFrictionCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysAirFieldAirFrictionCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPSysAirFieldAirFrictionCtlr::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
