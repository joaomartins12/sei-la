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

#include "NiPSysModifierActiveCtlr.h"

NiImplementRTTI(NiPSysModifierActiveCtlr, NiPSysModifierBoolCtlr);

//---------------------------------------------------------------------------
NiPSysModifierActiveCtlr::NiPSysModifierActiveCtlr(const char* pcModifierName)
    : NiPSysModifierBoolCtlr(pcModifierName)
{
}
//---------------------------------------------------------------------------
NiPSysModifierActiveCtlr::NiPSysModifierActiveCtlr()
{
}
//---------------------------------------------------------------------------
bool NiPSysModifierActiveCtlr::InterpTargetIsCorrectType(NiObjectNET* 
    pkTarget) const
{
    if (!NiPSysModifierCtlr::InterpTargetIsCorrectType(pkTarget))
    {
        return false;
    }

    NiPSysModifier* pkModifier = ((NiParticleSystem*) pkTarget)
        ->GetModifierByName(m_kModifierName);

    if (pkModifier)
        return true;
    else
        return false;
}
//---------------------------------------------------------------------------
void NiPSysModifierActiveCtlr::GetTargetBoolValue(bool& bValue)
{
    NIASSERT(m_pkModifier);
    bValue = m_pkModifier->GetActive();
}
//---------------------------------------------------------------------------
void NiPSysModifierActiveCtlr::SetTargetBoolValue(bool bValue)
{
    NIASSERT(m_pkModifier);
    if (m_pkModifier->GetActive() != bValue)
    {
        m_pkModifier->SetActive(bValue);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysModifierActiveCtlr);
//---------------------------------------------------------------------------
void NiPSysModifierActiveCtlr::CopyMembers(NiPSysModifierActiveCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifierBoolCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysModifierActiveCtlr);
//---------------------------------------------------------------------------
void NiPSysModifierActiveCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierBoolCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysModifierActiveCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierBoolCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysModifierActiveCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifierBoolCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysModifierActiveCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierBoolCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysModifierActiveCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSysModifierBoolCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSysModifierActiveCtlr::GetViewerStrings(NiViewerStringsArray*
    pkStrings)
{
    NiPSysModifierBoolCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysModifierActiveCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
