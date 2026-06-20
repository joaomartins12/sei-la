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

#include "NiPSysAirFieldInheritVelocityCtlr.h"
#include "NiPSysAirFieldModifier.h"
#include "NiPSysFieldModifier.h"

NiImplementRTTI(NiPSysAirFieldInheritVelocityCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysAirFieldInheritVelocityCtlr::NiPSysAirFieldInheritVelocityCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysAirFieldInheritVelocityCtlr::NiPSysAirFieldInheritVelocityCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysAirFieldInheritVelocityCtlr::InterpTargetIsCorrectType(
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
void NiPSysAirFieldInheritVelocityCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysAirFieldModifier*) m_pkModifier)->GetInheritVelocity();
}
//---------------------------------------------------------------------------
void NiPSysAirFieldInheritVelocityCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysAirFieldModifier*) m_pkModifier)->SetInheritVelocity(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysAirFieldInheritVelocityCtlr);
//---------------------------------------------------------------------------
void NiPSysAirFieldInheritVelocityCtlr::CopyMembers(
    NiPSysAirFieldInheritVelocityCtlr* pkDest, NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysAirFieldInheritVelocityCtlr);
//---------------------------------------------------------------------------
void NiPSysAirFieldInheritVelocityCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysAirFieldInheritVelocityCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysAirFieldInheritVelocityCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysAirFieldInheritVelocityCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysAirFieldInheritVelocityCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysAirFieldInheritVelocityCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiPSysAirFieldInheritVelocityCtlr::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
