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

#include "NiPSysEmitterInitialRadiusCtlr.h"
#include "NiPSysEmitter.h"

NiImplementRTTI(NiPSysEmitterInitialRadiusCtlr, NiPSysModifierFloatCtlr);

//---------------------------------------------------------------------------
NiPSysEmitterInitialRadiusCtlr::NiPSysEmitterInitialRadiusCtlr(
    const char* pcModifierName) : NiPSysModifierFloatCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysEmitterInitialRadiusCtlr::NiPSysEmitterInitialRadiusCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSysEmitterInitialRadiusCtlr::InterpTargetIsCorrectType(
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
void NiPSysEmitterInitialRadiusCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSysEmitter*) m_pkModifier)->GetInitialRadius();
}
//---------------------------------------------------------------------------
void NiPSysEmitterInitialRadiusCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSysEmitter*) m_pkModifier)->SetInitialRadius(fValue);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysEmitterInitialRadiusCtlr);
//---------------------------------------------------------------------------
void NiPSysEmitterInitialRadiusCtlr::CopyMembers(
    NiPSysEmitterInitialRadiusCtlr* pkDest, NiCloningProcess& kCloning)
{
    NiPSysModifierFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysEmitterInitialRadiusCtlr);
//---------------------------------------------------------------------------
void NiPSysEmitterInitialRadiusCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysEmitterInitialRadiusCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterInitialRadiusCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysEmitterInitialRadiusCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterInitialRadiusCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysEmitterInitialRadiusCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSysModifierFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysEmitterInitialRadiusCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
