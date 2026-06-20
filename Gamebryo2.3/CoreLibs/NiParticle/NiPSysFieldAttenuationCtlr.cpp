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

#include "NiPSysFieldAttenuationCtlr.h"
#include "NiPSysFieldModifier.h"

NiImplementRTTI(NiPSysFieldAttenuationCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysFieldAttenuationCtlr::NiPSysFieldAttenuationCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysFieldAttenuationCtlr::NiPSysFieldAttenuationCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysFieldAttenuationCtlr::InterpTargetIsCorrectType(
    NiObjectNET* pkTarget) const
{
    if (!NiPSysModifierCtlr::InterpTargetIsCorrectType(pkTarget))
    {
        return false;
    }

    NiPSysModifier* pkModifier = ((NiParticleSystem*) pkTarget)
        ->GetModifierByName(m_kModifierName);
    NIASSERT(pkModifier);

    return NiIsKindOf(NiPSysFieldModifier, pkModifier);
}
//---------------------------------------------------------------------------
void NiPSysFieldAttenuationCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysFieldModifier*) m_pkModifier)->GetAttenuation();
}
//---------------------------------------------------------------------------
void NiPSysFieldAttenuationCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysFieldModifier*) m_pkModifier)->SetAttenuation(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysFieldAttenuationCtlr);
//---------------------------------------------------------------------------
void NiPSysFieldAttenuationCtlr::CopyMembers(
    NiPSysFieldAttenuationCtlr* pkDest, NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysFieldAttenuationCtlr);
//---------------------------------------------------------------------------
void NiPSysFieldAttenuationCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysFieldAttenuationCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysFieldAttenuationCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysFieldAttenuationCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysFieldAttenuationCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysFieldAttenuationCtlr::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysFieldAttenuationCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
