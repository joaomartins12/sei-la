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

#include "NiPSysEmitterDeclinationVarCtlr.h"
#include "NiPSysEmitter.h"

NiImplementRTTI(NiPSysEmitterDeclinationVarCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysEmitterDeclinationVarCtlr::NiPSysEmitterDeclinationVarCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysEmitterDeclinationVarCtlr::NiPSysEmitterDeclinationVarCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysEmitterDeclinationVarCtlr::InterpTargetIsCorrectType(
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
void NiPSysEmitterDeclinationVarCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysEmitter*) m_pkModifier)->GetDeclinationVar();
}
//---------------------------------------------------------------------------
void NiPSysEmitterDeclinationVarCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysEmitter*) m_pkModifier)->SetDeclinationVar(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysEmitterDeclinationVarCtlr);
//---------------------------------------------------------------------------
void NiPSysEmitterDeclinationVarCtlr::CopyMembers(
    NiPSysEmitterDeclinationVarCtlr* pkDest, NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysEmitterDeclinationVarCtlr);
//---------------------------------------------------------------------------
void NiPSysEmitterDeclinationVarCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysEmitterDeclinationVarCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterDeclinationVarCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysEmitterDeclinationVarCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterDeclinationVarCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysEmitterDeclinationVarCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysEmitterDeclinationVarCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
