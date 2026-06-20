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

#include "NiPSysFieldModifier.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysFieldModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysFieldModifier::NiPSysFieldModifier(const char* pcName,
    NiAVObject* pkFieldObj, float fMagnitude, float fAttenuation,
    bool bUseMaxDistance, float fMaxDistance) : 
    NiPSysModifier(pcName, ORDER_FORCE), m_pkFieldObj(pkFieldObj), 
    m_fMagnitude(fMagnitude), m_fAttenuation(fAttenuation), 
    m_bUseMaxDistance(bUseMaxDistance), m_fMaxDistance(fMaxDistance)
{
    m_fMaxDistanceSqr = m_fMaxDistance * m_fMaxDistance;
}
//---------------------------------------------------------------------------
NiPSysFieldModifier::NiPSysFieldModifier() : 
    m_pkFieldObj(NULL), m_fMagnitude(0.0f), m_fAttenuation(0.0f), 
    m_bUseMaxDistance(false), m_fMaxDistance(0.0f)
{
    m_fMaxDistanceSqr = 0.0f;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSysFieldModifier::CopyMembers(NiPSysFieldModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);

    pkDest->m_pkFieldObj = m_pkFieldObj;
    pkDest->m_fMagnitude = m_fMagnitude;
    pkDest->m_fAttenuation = m_fAttenuation;;
    pkDest->m_bUseMaxDistance = m_bUseMaxDistance;
    pkDest->m_fMaxDistance = m_fMaxDistance;
    pkDest->m_fMaxDistanceSqr = m_fMaxDistanceSqr;
}
//---------------------------------------------------------------------------
void NiPSysFieldModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifier::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysFieldModifier* pkDest = (NiPSysFieldModifier*) pkClone;

    if (m_pkFieldObj)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_pkFieldObj, pkClone);
        if (bCloned)
        {
            NIASSERT(m_pkFieldObj->GetRTTI() == pkClone->GetRTTI());
            pkDest->m_pkFieldObj = (NiAVObject*) pkClone;
        }
        else
        {
            pkDest->m_pkFieldObj = m_pkFieldObj;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSysFieldModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkFieldObj
    NiStreamLoadBinary(kStream, m_fMagnitude);
    NiStreamLoadBinary(kStream, m_fAttenuation);

    NiBool kBool;
    NiStreamLoadBinary(kStream, kBool);
    m_bUseMaxDistance = (kBool != 0);

    NiStreamLoadBinary(kStream, m_fMaxDistance);
    m_fMaxDistanceSqr = m_fMaxDistance * m_fMaxDistance;
}
//---------------------------------------------------------------------------
void NiPSysFieldModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    m_pkFieldObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysFieldModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysFieldModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkFieldObj);
    NiStreamSaveBinary(kStream, m_fMagnitude);
    NiStreamSaveBinary(kStream, m_fAttenuation);

    NiBool kBool = m_bUseMaxDistance;
    NiStreamSaveBinary(kStream, kBool);

    NiStreamSaveBinary(kStream, m_fMaxDistance);
}
//---------------------------------------------------------------------------
bool NiPSysFieldModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysFieldModifier* pkDest = (NiPSysFieldModifier*) pkObject;

    if ((m_pkFieldObj && !pkDest->m_pkFieldObj) ||
        (!m_pkFieldObj && pkDest->m_pkFieldObj) ||
        (m_pkFieldObj && pkDest->m_pkFieldObj &&
            !m_pkFieldObj->IsEqual(pkDest->m_pkFieldObj)))
    {
        return false;
    }

    if (pkDest->m_fMagnitude != m_fMagnitude ||
        pkDest->m_fAttenuation != m_fAttenuation ||
        pkDest->m_bUseMaxDistance != m_bUseMaxDistance ||
        pkDest->m_fMaxDistance != m_fMaxDistance )
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysFieldModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysFieldModifier::ms_RTTI.GetName()));

    const char* pcFieldObj;
    if (m_pkFieldObj)
    {
        pcFieldObj = m_pkFieldObj->GetName();
    }
    else
    {
        pcFieldObj = "None";
    }
    pkStrings->Add(NiGetViewerString("Field Object", pcFieldObj));
    pkStrings->Add(NiGetViewerString("Magnitude", m_fMagnitude));
    pkStrings->Add(NiGetViewerString("Attenuation", m_fAttenuation));
    pkStrings->Add(NiGetViewerString("Use Max Distance", m_bUseMaxDistance));
    pkStrings->Add(NiGetViewerString("Max Distance", m_fMaxDistance));
}
//---------------------------------------------------------------------------
