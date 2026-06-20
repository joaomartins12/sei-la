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

#include "NiPSysInitialRotSpeedVarCtlr.h"
#include "NiPSysRotationModifier.h"

NiImplementRTTI(NiPSysInitialRotSpeedVarCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysInitialRotSpeedVarCtlr::NiPSysInitialRotSpeedVarCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysInitialRotSpeedVarCtlr::NiPSysInitialRotSpeedVarCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysInitialRotSpeedVarCtlr::InterpTargetIsCorrectType(
    NiObjectNET* pkTarget) const
{
    if (!NiPSysModifierCtlr::InterpTargetIsCorrectType(pkTarget))
    {
        return false;
    }

    NiPSysModifier* pkModifier = ((NiParticleSystem*) pkTarget)
        ->GetModifierByName(m_kModifierName);
    NIASSERT(pkModifier);

    return NiIsKindOf(NiPSysRotationModifier, pkModifier);
}
//---------------------------------------------------------------------------
void NiPSysInitialRotSpeedVarCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysRotationModifier*) m_pkModifier)
        ->GetInitialRotSpeedVar();
}
//---------------------------------------------------------------------------
void NiPSysInitialRotSpeedVarCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysRotationModifier*) m_pkModifier)->SetInitialRotSpeedVar(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysInitialRotSpeedVarCtlr);
//---------------------------------------------------------------------------
void NiPSysInitialRotSpeedVarCtlr::CopyMembers(
    NiPSysInitialRotSpeedVarCtlr* pkDest, NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysInitialRotSpeedVarCtlr);
//---------------------------------------------------------------------------
void NiPSysInitialRotSpeedVarCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysInitialRotSpeedVarCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysInitialRotSpeedVarCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysInitialRotSpeedVarCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysInitialRotSpeedVarCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysInitialRotSpeedVarCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysInitialRotSpeedVarCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
