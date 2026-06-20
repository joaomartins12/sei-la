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

#include "NiPSysFieldMaxDistanceCtlr.h"
#include "NiPSysFieldModifier.h"

NiImplementRTTI(NiPSysFieldMaxDistanceCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysFieldMaxDistanceCtlr::NiPSysFieldMaxDistanceCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysFieldMaxDistanceCtlr::NiPSysFieldMaxDistanceCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysFieldMaxDistanceCtlr::InterpTargetIsCorrectType(
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
void NiPSysFieldMaxDistanceCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysFieldModifier*) m_pkModifier)->GetMaxDistance();
}
//---------------------------------------------------------------------------
void NiPSysFieldMaxDistanceCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysFieldModifier*) m_pkModifier)->SetMaxDistance(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysFieldMaxDistanceCtlr);
//---------------------------------------------------------------------------
void NiPSysFieldMaxDistanceCtlr::CopyMembers(
    NiPSysFieldMaxDistanceCtlr* pkDest, NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysFieldMaxDistanceCtlr);
//---------------------------------------------------------------------------
void NiPSysFieldMaxDistanceCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysFieldMaxDistanceCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysFieldMaxDistanceCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysFieldMaxDistanceCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysFieldMaxDistanceCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysFieldMaxDistanceCtlr::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysFieldMaxDistanceCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
