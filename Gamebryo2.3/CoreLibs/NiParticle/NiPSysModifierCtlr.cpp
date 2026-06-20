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

#include "NiPSysModifierCtlr.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysModifierCtlr, NiSingleInterpController);

//---------------------------------------------------------------------------
NiPSysModifierCtlr::NiPSysModifierCtlr(const NiFixedString& kModifierName) :
    m_pkModifier(NULL)
{
    SetModifierName(kModifierName);
}
//---------------------------------------------------------------------------
NiPSysModifierCtlr::NiPSysModifierCtlr() : m_pkModifier(NULL)
{
}
//---------------------------------------------------------------------------
NiPSysModifierCtlr::~NiPSysModifierCtlr()
{
}
//---------------------------------------------------------------------------
void NiPSysModifierCtlr::SetTarget(NiObjectNET* pkTarget)
{
    // Clear the modifier pointer.
    m_pkModifier = NULL;

    // Call the base class SetTarget.
    NiSingleInterpController::SetTarget(pkTarget);

    // If the target was set to something other than NULL, retreive the
    // modifier pointer using the name.
    if (m_pkTarget)
    {
        GetModifierPointerFromName();
    }
}
//---------------------------------------------------------------------------
void NiPSysModifierCtlr::HandleReset()
{
    // Default implementation does nothing.
}
//---------------------------------------------------------------------------
const char* NiPSysModifierCtlr::GetCtlrID() 
{
    return (const char*)m_kModifierName;
}
//---------------------------------------------------------------------------
bool NiPSysModifierCtlr::InterpTargetIsCorrectType(NiObjectNET* pkTarget) 
    const
{  
    if (!NiIsKindOf(NiParticleSystem, pkTarget) || !m_kModifierName.Exists())
    {
        return false;
    }

    if (m_pkTarget)
    {
        if (((NiParticleSystem*) pkTarget)->GetModifierByName(
            m_kModifierName))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSysModifierCtlr::CopyMembers(NiPSysModifierCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiSingleInterpController::CopyMembers(pkDest, kCloning);

    pkDest->SetModifierName(m_kModifierName);
}
//---------------------------------------------------------------------------
void NiPSysModifierCtlr::ProcessClone(NiCloningProcess& kCloning)
{
    NiSingleInterpController::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkClone));
    NiPSysModifierCtlr* pkDest = (NiPSysModifierCtlr*) pkClone;

    if (m_pkTarget && pkDest->m_pkTarget)
    {
        NIASSERT(m_pkModifier);

        NIVERIFY(kCloning.m_pkCloneMap->GetAt(m_pkModifier, pkClone));

        pkDest->m_pkModifier = (NiPSysModifier*) pkClone;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSysModifierCtlr::LoadBinary(NiStream& kStream)
{
    NiSingleInterpController::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        kStream.LoadCStringAsFixedString(m_kModifierName);
    }
    else
    {
        kStream.LoadFixedString(m_kModifierName);
    }
}
//---------------------------------------------------------------------------
void NiPSysModifierCtlr::LinkObject(NiStream& kStream)
{
    NiSingleInterpController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPSysModifierCtlr::PostLinkObject(NiStream& kStream)
{
    NiSingleInterpController::PostLinkObject(kStream);

    if (m_pkTarget)
    {
        GetModifierPointerFromName();
    }
}
//---------------------------------------------------------------------------
bool NiPSysModifierCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiSingleInterpController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysModifierCtlr::SaveBinary(NiStream& kStream)
{
    NiSingleInterpController::SaveBinary(kStream);

    NIASSERT(m_kModifierName.Exists());
    kStream.SaveFixedString(m_kModifierName);
}
//---------------------------------------------------------------------------
bool NiPSysModifierCtlr::IsEqual(NiObject* pkObject)
{
    if (!NiSingleInterpController::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysModifierCtlr* pkDest = (NiPSysModifierCtlr*) pkObject;

    if ((m_pkModifier && !pkDest->m_pkModifier) ||
        (!m_pkModifier && pkDest->m_pkModifier) ||
        (m_pkModifier && pkDest->m_pkModifier && 
            !m_pkModifier->IsEqual(pkDest->m_pkModifier)))
    {
        return false;
    }

    if (m_kModifierName != pkDest->m_kModifierName)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysModifierCtlr::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiSingleInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysModifierCtlr::ms_RTTI.GetName()));

    NIASSERT(m_kModifierName);
    pkStrings->Add(NiGetViewerString("Modifier Name", 
        (const char*)m_kModifierName));
}
//---------------------------------------------------------------------------
