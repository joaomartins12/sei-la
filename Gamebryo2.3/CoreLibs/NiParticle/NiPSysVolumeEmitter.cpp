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

#include "NiPSysVolumeEmitter.h"
#include "NiParticleSystem.h"
#include <NiCloningProcess.h>

NiImplementRTTI(NiPSysVolumeEmitter, NiPSysEmitter);

//---------------------------------------------------------------------------
NiPSysVolumeEmitter::NiPSysVolumeEmitter(const char* pcName,
    NiAVObject* pkEmitterObj, float fSpeed, float fSpeedVar, 
    float fDeclination, float fDeclinationVar, float fPlanarAngle, 
    float fPlanarAngleVar, NiColorA kInitialColor, float fInitialRadius, 
    float fLifeSpan, float fLifeSpanVar, float fRadiusVar) :
    NiPSysEmitter(pcName, ORDER_EMITTER, fSpeed, fSpeedVar, fDeclination,
    fDeclinationVar, fPlanarAngle, fPlanarAngleVar, kInitialColor,
    fInitialRadius, fLifeSpan, fLifeSpanVar, fRadiusVar), 
    m_pkEmitterObj(pkEmitterObj)
{
}
//---------------------------------------------------------------------------
NiPSysVolumeEmitter::NiPSysVolumeEmitter() : m_pkEmitterObj(NULL)
{
}
//---------------------------------------------------------------------------
void NiPSysVolumeEmitter::ComputeInitialPositionAndVelocity(
    NiPoint3& kPosition, NiPoint3& kVelocity)
{
    // Modify velocity and position based on emitter object transforms.
    if (m_pkEmitterObj)
    {
        NiTransform kEmitter = m_pkEmitterObj->GetWorldTransform();
        NiTransform kPSys = m_pkTarget->GetWorldTransform();
        NiTransform kInvPSys;
        kPSys.Invert(kInvPSys);
        NiTransform kEmitterToPSys = kInvPSys * kEmitter;

        ComputeVolumeInitialPositionAndVelocity(kEmitterToPSys, 
            kPosition, kVelocity);
    }
    else
    {
        NiTransform kEmitterToPSys;
        kEmitterToPSys.MakeIdentity();
        ComputeVolumeInitialPositionAndVelocity(kEmitterToPSys, 
            kPosition, kVelocity);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSysVolumeEmitter::CopyMembers(NiPSysVolumeEmitter* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysEmitter::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
void NiPSysVolumeEmitter::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysEmitter::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysVolumeEmitter* pkDest = (NiPSysVolumeEmitter*) pkClone;

    if (m_pkEmitterObj)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_pkEmitterObj, pkClone);
        if (bCloned)
        {
            NIASSERT(m_pkEmitterObj->GetRTTI() == pkClone->GetRTTI());
            pkDest->m_pkEmitterObj = (NiAVObject*) pkClone;
        }
        else
        {
            pkDest->m_pkEmitterObj = m_pkEmitterObj;
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void NiPSysVolumeEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysEmitter::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_pkEmitterObj
}
//---------------------------------------------------------------------------
void NiPSysVolumeEmitter::LinkObject(NiStream& kStream)
{
    NiPSysEmitter::LinkObject(kStream);

    m_pkEmitterObj = (NiAVObject*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysVolumeEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysVolumeEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysEmitter::SaveBinary(kStream);

    kStream.SaveLinkID(m_pkEmitterObj);
}
//---------------------------------------------------------------------------
bool NiPSysVolumeEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysEmitter::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysVolumeEmitter* pkDest = (NiPSysVolumeEmitter*) pkObject;

    if ((m_pkEmitterObj && !pkDest->m_pkEmitterObj) ||
        (!m_pkEmitterObj && pkDest->m_pkEmitterObj))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysVolumeEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysEmitter::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysVolumeEmitter::ms_RTTI.GetName()));
    if (m_pkEmitterObj)
    {
        pkStrings->Add(NiGetViewerString("Emitter Object", m_pkEmitterObj
            ->GetName()));
    }
}
//---------------------------------------------------------------------------
