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

#include "NiPSysAirFieldSpreadCtlr.h"
#include "NiPSysAirFieldModifier.h"
#include "NiPSysFieldModifier.h"

NiImplementRTTI(NiPSysAirFieldSpreadCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysAirFieldSpreadCtlr::NiPSysAirFieldSpreadCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysAirFieldSpreadCtlr::NiPSysAirFieldSpreadCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysAirFieldSpreadCtlr::InterpTargetIsCorrectType(
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
void NiPSysAirFieldSpreadCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysAirFieldModifier*) m_pkModifier)->GetSpread();
}
//---------------------------------------------------------------------------
void NiPSysAirFieldSpreadCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysAirFieldModifier*) m_pkModifier)->SetSpread(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysAirFieldSpreadCtlr);
//---------------------------------------------------------------------------
void NiPSysAirFieldSpreadCtlr::CopyMembers(NiPSysAirFieldSpreadCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysAirFieldSpreadCtlr);
//---------------------------------------------------------------------------
void NiPSysAirFieldSpreadCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysAirFieldSpreadCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysAirFieldSpreadCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysAirFieldSpreadCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysAirFieldSpreadCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysAirFieldSpreadCtlr::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysAirFieldSpreadCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
