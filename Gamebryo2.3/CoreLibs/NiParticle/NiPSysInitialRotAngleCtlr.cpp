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

#include "NiPSysInitialRotAngleCtlr.h"
#include "NiPSysRotationModifier.h"

NiImplementRTTI(NiPSysInitialRotAngleCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysInitialRotAngleCtlr::NiPSysInitialRotAngleCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysInitialRotAngleCtlr::NiPSysInitialRotAngleCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysInitialRotAngleCtlr::InterpTargetIsCorrectType(
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
void NiPSysInitialRotAngleCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysRotationModifier*) m_pkModifier)->GetInitialRotAngle();
}
//---------------------------------------------------------------------------
void NiPSysInitialRotAngleCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysRotationModifier*) m_pkModifier)->SetInitialRotAngle(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysInitialRotAngleCtlr);
//---------------------------------------------------------------------------
void NiPSysInitialRotAngleCtlr::CopyMembers(NiPSysInitialRotAngleCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysInitialRotAngleCtlr);
//---------------------------------------------------------------------------
void NiPSysInitialRotAngleCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysInitialRotAngleCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysInitialRotAngleCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysInitialRotAngleCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysInitialRotAngleCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysInitialRotAngleCtlr::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysInitialRotAngleCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
