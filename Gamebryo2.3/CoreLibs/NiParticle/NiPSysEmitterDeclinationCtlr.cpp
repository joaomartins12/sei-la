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

#include "NiPSysEmitterDeclinationCtlr.h"
#include "NiPSysEmitter.h"

NiImplementRTTI(NiPSysEmitterDeclinationCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysEmitterDeclinationCtlr::NiPSysEmitterDeclinationCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysEmitterDeclinationCtlr::NiPSysEmitterDeclinationCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysEmitterDeclinationCtlr::InterpTargetIsCorrectType(
    NiObjectNET* pkTarget) const
{
    if (!NiPSysModifierCtlr::InterpTargetIsCorrectType(pkTarget))
    {
        return false;
    }

    NiPSysModifier* pkModifier = ((NiParticleSystem*) pkTarget)
        ->GetModifierByName(m_kModifierName);
    NIASSERT(pkModifier);

    return NiIsKindOf(NiPSysEmitter, pkModifier);
}
//---------------------------------------------------------------------------
void NiPSysEmitterDeclinationCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysEmitter*) m_pkModifier)->GetDeclination();
}
//---------------------------------------------------------------------------
void NiPSysEmitterDeclinationCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysEmitter*) m_pkModifier)->SetDeclination(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysEmitterDeclinationCtlr);
//---------------------------------------------------------------------------
void NiPSysEmitterDeclinationCtlr::CopyMembers(
    NiPSysEmitterDeclinationCtlr* pkDest, NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysEmitterDeclinationCtlr);
//---------------------------------------------------------------------------
void NiPSysEmitterDeclinationCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysEmitterDeclinationCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterDeclinationCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysEmitterDeclinationCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterDeclinationCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysEmitterDeclinationCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysEmitterDeclinationCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
