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

#include "NiPSysInitialRotSpeedCtlr.h"
#include "NiPSysRotationModifier.h"

NiImplementRTTI(NiPSysInitialRotSpeedCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysInitialRotSpeedCtlr::NiPSysInitialRotSpeedCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysInitialRotSpeedCtlr::NiPSysInitialRotSpeedCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysInitialRotSpeedCtlr::InterpTargetIsCorrectType(
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
void NiPSysInitialRotSpeedCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysRotationModifier*) m_pkModifier)->GetInitialRotSpeed();
}
//---------------------------------------------------------------------------
void NiPSysInitialRotSpeedCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysRotationModifier*) m_pkModifier)->SetInitialRotSpeed(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysInitialRotSpeedCtlr);
//---------------------------------------------------------------------------
void NiPSysInitialRotSpeedCtlr::CopyMembers(NiPSysInitialRotSpeedCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysInitialRotSpeedCtlr);
//---------------------------------------------------------------------------
void NiPSysInitialRotSpeedCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysInitialRotSpeedCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysInitialRotSpeedCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysInitialRotSpeedCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysInitialRotSpeedCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysInitialRotSpeedCtlr::GetViewerStrings(NiViewerStringsArray*
    pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysInitialRotSpeedCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
