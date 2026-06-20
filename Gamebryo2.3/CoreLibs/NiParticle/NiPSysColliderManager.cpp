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

#include "NiPSysColliderManager.h"
#include "NiParticleSystem.h"
#include <NiStream.h>

NiImplementRTTI(NiPSysColliderManager, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysColliderManager::NiPSysColliderManager(const char* pcName,
    NiPSysCollider* pkColliders) : NiPSysModifier(pcName, ORDER_COLLIDER),
    m_spColliders(pkColliders)
{
}
//---------------------------------------------------------------------------
NiPSysColliderManager::NiPSysColliderManager() : m_spColliders(NULL)
{
}
//---------------------------------------------------------------------------
void NiPSysColliderManager::Update(float fTime, NiPSysData* pkData)
{
    // Set the resolve constants for all of the colliders
    if (m_spColliders && (pkData->GetNumParticles() > 0))
        m_spColliders->SetResolveConstants();

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        // Determine first collider, if it exists.
        NiPSysCollider* pkFirstCollider = NULL;
        float fTimeStepStart = m_pkTarget->GetLastTime();
        float fCollisionTime = fTime;
        if (m_spColliders)
        {
            pkFirstCollider = m_spColliders->Resolve(fTimeStepStart,
                fCollisionTime, pkData, us);
        }

        // Update first collider.
        if (pkFirstCollider)
        {
            pkFirstCollider->Update(fTime, pkData, us);
        }
        else
        {
            fCollisionTime = fTimeStepStart;
        }

        // Set the last update time of the particle to the collision time.
        pkData->GetParticleInfo()[us].m_fLastUpdate = fCollisionTime;
    }
}
//---------------------------------------------------------------------------
void NiPSysColliderManager::RemoveCollider(NiPSysCollider* pkCollider)
{
    NiPSysCollider* pkPrevCollider = NULL;
    NiPSysCollider* pkCurCollider = m_spColliders;
    while (pkCurCollider)
    {
        if (pkCurCollider == pkCollider)
        {
            pkCurCollider->SetManager(NULL);
            if (pkPrevCollider)
            {
                pkPrevCollider->SetNext(pkCurCollider->GetNext());
            }
            else
            {
                m_spColliders = pkCurCollider->GetNext();
            }
            break;
        }

        pkPrevCollider = pkCurCollider;
        pkCurCollider = pkCurCollider->GetNext();
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysColliderManager);
//---------------------------------------------------------------------------
void NiPSysColliderManager::CopyMembers(NiPSysColliderManager* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);

    if (m_spColliders)
    {
        pkDest->m_spColliders = (NiPSysCollider*) m_spColliders->CreateClone(
            kCloning);
    }
}
//---------------------------------------------------------------------------
void NiPSysColliderManager::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifier::ProcessClone(kCloning);

    if (m_spColliders)
    {
        m_spColliders->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysColliderManager);
//---------------------------------------------------------------------------
void NiPSysColliderManager::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_spColliders
}
//---------------------------------------------------------------------------
void NiPSysColliderManager::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    m_spColliders = (NiPSysCollider*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysColliderManager::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysModifier::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spColliders)
    {
        m_spColliders->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSysColliderManager::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    kStream.SaveLinkID(m_spColliders);
}
//---------------------------------------------------------------------------
bool NiPSysColliderManager::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysColliderManager* pkDest = (NiPSysColliderManager*) pkObject;

    if ((m_spColliders && !pkDest->m_spColliders) ||
        (!m_spColliders && pkDest->m_spColliders) ||
        (m_spColliders && pkDest->m_spColliders &&
            !m_spColliders->IsEqual(pkDest->m_spColliders)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysColliderManager::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysColliderManager::ms_RTTI
        .GetName()));

    m_spColliders->GetViewerStrings(pkStrings);
}
//---------------------------------------------------------------------------
