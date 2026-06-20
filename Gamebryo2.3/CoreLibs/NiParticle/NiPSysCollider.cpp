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

#include "NiPSysCollider.h"
#include "NiPSysData.h"
#include "NiPSysSpawnModifier.h"
#include "NiPSysColliderManager.h"
#include <NiCloningProcess.h>
#include <NiStream.h>
#include <NiBool.h>

NiImplementRTTI(NiPSysCollider, NiObject);

//---------------------------------------------------------------------------
NiPSysCollider::NiPSysCollider(float fBounce, bool bSpawnOnCollide,
    bool bDieOnCollide, NiPSysSpawnModifier* pkSpawnModifier) :
    m_fBounce(fBounce), m_bSpawnOnCollide(bSpawnOnCollide),
    m_bDieOnCollide(bDieOnCollide), m_pkSpawnModifier(pkSpawnModifier),
    m_kCollisionPoint(NiPoint3::ZERO), m_fCollisionTime(0.0f),
    m_pkManager(NULL), m_spNext(NULL)
{
}
//---------------------------------------------------------------------------
NiPSysCollider::NiPSysCollider() : m_fBounce(1.0f), m_bSpawnOnCollide(false),
    m_bDieOnCollide(false), m_pkSpawnModifier(NULL),
    m_kCollisionPoint(NiPoint3::ZERO), m_fCollisionTime(0.0f),
    m_pkManager(NULL), m_spNext(NULL)
{
}
//---------------------------------------------------------------------------
NiPSysCollider* NiPSysCollider::Resolve(float fInitialTime,
    float& fCollisionTime, NiPSysData* pkData, unsigned short usCurParticle)
{
    // Check other colliders.
    if (m_spNext)
    {
        // Test other collisions
        NiPSysCollider* pkResult = m_spNext->Resolve(fInitialTime,
            fCollisionTime, pkData, usCurParticle);
        if (pkResult)
        {
            // Other collision happened first
            return pkResult;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiPSysCollider::Update(float fTime, NiPSysData* pkData,
    unsigned short usCurParticle)
{
    // Update particle position.
    pkData->GetVertices()[usCurParticle] = m_kCollisionPoint;

    if (m_bSpawnOnCollide && m_pkSpawnModifier)
    {
        m_pkSpawnModifier->SpawnParticles(fTime, m_fCollisionTime,
            usCurParticle, m_pkManager->GetSystemPointer());
    }

    if (m_bDieOnCollide)
    {
        pkData->RemoveParticle(usCurParticle);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSysCollider::CopyMembers(NiPSysCollider* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_fBounce = m_fBounce;
    pkDest->m_bSpawnOnCollide = m_bSpawnOnCollide;
    pkDest->m_bDieOnCollide = m_bDieOnCollide;

    if (m_spNext)
    {
        pkDest->m_spNext = (NiPSysCollider*) m_spNext->CreateClone(kCloning);
    }
}
//---------------------------------------------------------------------------
void NiPSysCollider::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(this, pkClone);
    NIASSERT(bCloned);
    NiPSysCollider* pkDest = (NiPSysCollider*) pkClone;

    if (m_pkSpawnModifier)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_pkSpawnModifier, pkClone);
        if (bCloned)
        {
            pkDest->m_pkSpawnModifier = (NiPSysSpawnModifier*) pkClone;
        }
        else
        {
            pkDest->m_pkSpawnModifier = m_pkSpawnModifier;
        }
    }

    if (m_pkManager)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_pkManager, pkClone);
        if (bCloned)
        {
            pkDest->m_pkManager = (NiPSysColliderManager*) pkClone;
        }
        else
        {
            pkDest->m_pkManager = m_pkManager;
        }
    }

    if (m_spNext)
    {
        m_spNext->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSysCollider::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fBounce);
    NiBool bSpawnOnCollide;
    NiStreamLoadBinary(kStream, bSpawnOnCollide);
    m_bSpawnOnCollide = (bSpawnOnCollide == 0) ? false : true;
    NiBool bDieOnCollide;
    NiStreamLoadBinary(kStream, bDieOnCollide);
    m_bDieOnCollide = (bDieOnCollide == 0) ? false : true;
    kStream.ReadLinkID();   // m_pkSpawnModifier

    kStream.ReadLinkID();   // m_pkManager
    kStream.ReadLinkID();   // m_spNext
}
//---------------------------------------------------------------------------
void NiPSysCollider::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);

    m_pkSpawnModifier = (NiPSysSpawnModifier*) kStream.GetObjectFromLinkID();

    m_pkManager = (NiPSysColliderManager*) kStream.GetObjectFromLinkID();
    m_spNext = (NiPSysCollider*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysCollider::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spNext)
    {
        m_spNext->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSysCollider::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fBounce);
    NiStreamSaveBinary(kStream, NiBool(m_bSpawnOnCollide));
    NiStreamSaveBinary(kStream, NiBool(m_bDieOnCollide));
    kStream.SaveLinkID(m_pkSpawnModifier);

    kStream.SaveLinkID(m_pkManager);
    kStream.SaveLinkID(m_spNext);
}
//---------------------------------------------------------------------------
bool NiPSysCollider::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysCollider* pkDest = (NiPSysCollider*) pkObject;

    if (m_fBounce != pkDest->m_fBounce ||
        m_bSpawnOnCollide != pkDest->m_bSpawnOnCollide ||
        m_bDieOnCollide != pkDest->m_bDieOnCollide)
    {
        return false;
    }

    if ((m_pkSpawnModifier && !pkDest->m_pkSpawnModifier) ||
        (!m_pkSpawnModifier && pkDest->m_pkSpawnModifier) ||
        (m_pkSpawnModifier && pkDest->m_pkSpawnModifier &&
            !m_pkSpawnModifier->IsEqual(pkDest->m_pkSpawnModifier)))
    {
        return false;
    }

    if ((m_pkManager && !pkDest->m_pkManager) ||
        (!m_pkManager && pkDest->m_pkManager))
    {
        return false;
    }

    if ((m_spNext && !pkDest->m_spNext) ||
        (!m_spNext && pkDest->m_spNext) ||
        (m_spNext && pkDest->m_spNext &&
            !m_spNext->IsEqual(pkDest->m_spNext)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysCollider::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysCollider::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Bounce", m_fBounce));
    pkStrings->Add(NiGetViewerString("Spawn on Collide", m_bSpawnOnCollide));
    pkStrings->Add(NiGetViewerString("Die on Collide", m_bDieOnCollide));
    const char* pcSpawnModifier;
    if (m_pkSpawnModifier)
    {
        pcSpawnModifier = m_pkSpawnModifier->GetName();
    }
    else
    {
        pcSpawnModifier = "None";
    }
    pkStrings->Add(NiGetViewerString("Spawn Modifier", pcSpawnModifier));

    pkStrings->Add(NiGetViewerString("Manager", m_pkManager));

    if (m_spNext)
    {
        m_spNext->GetViewerStrings(pkStrings);
    }
}
//---------------------------------------------------------------------------
