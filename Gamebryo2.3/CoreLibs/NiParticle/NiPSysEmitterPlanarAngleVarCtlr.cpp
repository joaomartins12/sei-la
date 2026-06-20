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

#include "NiPSysEmitterPlanarAngleVarCtlr.h"
#include "NiPSysEmitter.h"

NiImplementRTTI(NiPSysEmitterPlanarAngleVarCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysEmitterPlanarAngleVarCtlr::NiPSysEmitterPlanarAngleVarCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysEmitterPlanarAngleVarCtlr::NiPSysEmitterPlanarAngleVarCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysEmitterPlanarAngleVarCtlr::InterpTargetIsCorrectType(
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
void NiPSysEmitterPlanarAngleVarCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysEmitter*) m_pkModifier)->GetPlanarAngleVar();
}
//---------------------------------------------------------------------------
void NiPSysEmitterPlanarAngleVarCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysEmitter*) m_pkModifier)->SetPlanarAngleVar(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysEmitterPlanarAngleVarCtlr);
//---------------------------------------------------------------------------
void NiPSysEmitterPlanarAngleVarCtlr::CopyMembers(
    NiPSysEmitterPlanarAngleVarCtlr* pkDest, NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysEmitterPlanarAngleVarCtlr);
//---------------------------------------------------------------------------
void NiPSysEmitterPlanarAngleVarCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysEmitterPlanarAngleVarCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterPlanarAngleVarCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysEmitterPlanarAngleVarCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterPlanarAngleVarCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysEmitterPlanarAngleVarCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysEmitterPlanarAngleVarCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
