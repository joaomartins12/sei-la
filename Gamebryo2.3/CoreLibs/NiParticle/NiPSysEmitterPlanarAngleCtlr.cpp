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

#include "NiPSysEmitterPlanarAngleCtlr.h"
#include "NiPSysEmitter.h"

NiImplementRTTI(NiPSysEmitterPlanarAngleCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysEmitterPlanarAngleCtlr::NiPSysEmitterPlanarAngleCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysEmitterPlanarAngleCtlr::NiPSysEmitterPlanarAngleCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysEmitterPlanarAngleCtlr::InterpTargetIsCorrectType(
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
void NiPSysEmitterPlanarAngleCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysEmitter*) m_pkModifier)->GetPlanarAngle();
}
//---------------------------------------------------------------------------
void NiPSysEmitterPlanarAngleCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysEmitter*) m_pkModifier)->SetPlanarAngle(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysEmitterPlanarAngleCtlr);
//---------------------------------------------------------------------------
void NiPSysEmitterPlanarAngleCtlr::CopyMembers(
    NiPSysEmitterPlanarAngleCtlr* pkDest, NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysEmitterPlanarAngleCtlr);
//---------------------------------------------------------------------------
void NiPSysEmitterPlanarAngleCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysEmitterPlanarAngleCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterPlanarAngleCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysEmitterPlanarAngleCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterPlanarAngleCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysEmitterPlanarAngleCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysEmitterPlanarAngleCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
