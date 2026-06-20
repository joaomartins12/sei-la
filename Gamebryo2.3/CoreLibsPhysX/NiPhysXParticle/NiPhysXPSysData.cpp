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
#include "NiPhysXParticlePCH.h"

#include "NiPhysXPSysData.h"
#include <NiBool.h>

NiImplementRTTI(NiPhysXPSysData, NiPSysData);

//---------------------------------------------------------------------------
NiPhysXPSysData::NiPhysXPSysData(unsigned short usMaxNumParticles,
    bool bHasColors, bool bHasRotations, NiPhysXScene* pkScene,
    unsigned int uiCompartmentID, unsigned int uiActorPoolSize,
    bool bFillActorPoolOnLoad, bool bKeepsMeshes)
    : NiPSysData(usMaxNumParticles, bHasColors, bHasRotations)
{
    SetOnConstruction(usMaxNumParticles, pkScene, uiCompartmentID,
        uiActorPoolSize, bFillActorPoolOnLoad, bKeepsMeshes);
}
//---------------------------------------------------------------------------
NiPhysXPSysData::NiPhysXPSysData(NiPSysData* pkPSysData,
    NiPhysXScene* pkScene, unsigned int uiCompartmentID,
    unsigned int uiActorPoolSize, bool bFillActorPoolOnLoad, bool bKeepsMeshes)
    : NiPSysData(pkPSysData->GetMaxNumParticles(),
    pkPSysData->GetColors() != NULL, pkPSysData->GetRotationAngles() != NULL)
{
    SetOnConstruction(pkPSysData->GetMaxNumParticles(), pkScene,
        uiCompartmentID, uiActorPoolSize, bFillActorPoolOnLoad, bKeepsMeshes);
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::SetOnConstruction(unsigned short usMaxNumParticles,
    NiPhysXScene* pkScene, unsigned int uiCompartmentID,
    unsigned int uiActorPoolSize, bool bFillActorPoolOnLoad, bool bKeepsMeshes)
{
    NIASSERT(usMaxNumParticles > 0);

    // Set the scene in which it resides to 0 for now
    m_spScene = 0;
    m_uiCompartmentID = uiCompartmentID;

    // Create PhysX specific particle data
    m_pkPhysXParticleInfo = NiNew NiPhysXParticleInfo[usMaxNumParticles];
    
    m_bFillActorPoolOnLoad = bFillActorPoolOnLoad;
    m_bKeepsActorMeshes = bKeepsMeshes;

    // Initialize the size of the pool
    m_uiDefaultActorPoolSize = uiActorPoolSize;
    if (m_uiDefaultActorPoolSize ==
        (unsigned int)NiMeshPSysData::DEFAULT_POOL_SIZE)
    {
        m_uiDefaultActorPoolSize = usMaxNumParticles;
    }

    m_kActorPool.SetSize(0);
    m_kActorPoolCenter = NiPoint3::ZERO;
    m_kActorPoolDim = NiPoint3(100.0f, 100.0f, 100.0f);

    // Attach the scene
    if (pkScene)
        SetScene(pkScene);
}
//---------------------------------------------------------------------------
NiPhysXPSysData::NiPhysXPSysData()
{
}
//---------------------------------------------------------------------------
NiPhysXPSysData::~NiPhysXPSysData()
{
    NiDelete[] m_pkPhysXParticleInfo;
    EmptyActorPool();
    m_spScene = 0;
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::RemoveParticle(unsigned short usParticle)
{
    unsigned short usLastParticle = GetActiveVertexCount() - 1;

    NiPSysData::RemoveParticle(usParticle);

    NxActor* pkRemovedActor;
    
    if (usParticle == usLastParticle)
    {
        pkRemovedActor = m_pkPhysXParticleInfo[usLastParticle].m_pkActor;
    }
    else
    {
        pkRemovedActor = m_pkPhysXParticleInfo[usParticle].m_pkActor;
        m_pkPhysXParticleInfo[usParticle] =
            m_pkPhysXParticleInfo[usLastParticle];
    }

    if (pkRemovedActor)
    {
        // Add the actor to the appropriate pool. It will be deleted if the
        // pool is full
        AddActorToPool(pkRemovedActor);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::SetScene(NiPhysXScene* pkScene)
{
    if (m_spScene)
    {
        // Clear out all particles.
        SetActiveVertexCount(0);
        
        // Clear the actor pool
        EmptyActorPool();
    }
    
    m_spScene = pkScene;
    
    if(!m_spScene)
        return;
    
    m_kActorPool.SetSize(m_uiDefaultActorPoolSize);

    if (m_bFillActorPoolOnLoad && m_uiDefaultActorPoolSize > 0)
    {
        FillActorPool(m_spActorDescriptor);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::SetActiveVertexCount(unsigned short usActive)
{
    // Makes sure that any added particle's, which will have
    // actors associated with then, are included in the removal.
    ResolveAddedParticles();
    
    if (m_spScene && m_spScene->GetPhysXScene())
    {
        // Release all the actors
        NiPhysXManager::GetPhysXManager()->WaitSDKLock();
        NIASSERT(!m_spScene->GetInSimFetch());
        for (unsigned short us = usActive; us < GetActiveVertexCount(); us++)
        {
            if (m_pkPhysXParticleInfo[us].m_pkActor)
            {
                m_spScene->GetPhysXScene()->releaseActor(
                    *m_pkPhysXParticleInfo[us].m_pkActor);
            }
        }
        NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();
    }
    
    NiPSysData::SetActiveVertexCount(usActive);
}
//---------------------------------------------------------------------------
inline void NiPhysXPSysData::FillActorPool(NiPhysXActorDesc* pkOriginal)
{
    if (!m_spScene || !m_spScene->GetPhysXScene())
        return;
        
    // Get any compartment
    NxCompartment* pkCompartment = 0;
    if (m_uiCompartmentID)
    {
        pkCompartment = m_spScene->GetCompartmentAt(m_uiCompartmentID);
    }

    NIASSERT(pkOriginal != NULL);

    NiPhysXManager::GetPhysXManager()->WaitSDKLock();
    NIASSERT(!m_spScene->GetInSimFetch());
    for(unsigned int ui = 0; ui < m_kActorPool.GetAllocatedSize(); ui++)
    {
        NxActor* pkActor = pkOriginal->CreateActor(m_spScene->GetPhysXScene(),
            pkCompartment, 0, NiPhysXTypes::NXMAT34_ID, m_bKeepsActorMeshes);
        if (!pkActor)
            continue;
        pkActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);

        NxVec3 kPosn(
            (NiUnitRandom() * 2.0f - 1.0f) * m_kActorPoolDim.x
            + m_kActorPoolCenter.x,
            (NiUnitRandom() * 2.0f - 1.0f) * m_kActorPoolDim.y
            + m_kActorPoolCenter.y,
            (NiUnitRandom() * 2.0f - 1.0f) * m_kActorPoolDim.z
            + m_kActorPoolCenter.z);
        pkActor->setGlobalPosition(kPosn);

        m_kActorPool.SetAt(ui, pkActor);
    }
    NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::AddActorToPool(NxActor* pkActor)
{
    if (!pkActor)
        return;
        
    if (m_kActorPool.GetSize() < m_kActorPool.GetAllocatedSize())
    {
        m_kActorPool.Add(pkActor);
        NiPhysXManager::GetPhysXManager()->WaitSDKLock();
        NIASSERT(!m_spScene->GetInSimFetch());
        pkActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
        NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();
    }
    else
    {
        NiPhysXManager::GetPhysXManager()->WaitSDKLock();
        NIASSERT(!m_spScene->GetInSimFetch());
        m_spScene->GetPhysXScene()->releaseActor(*pkActor);
        NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::EmptyActorPool()
{
    // It could be that the PhysX scene has already been deleted, in which
    // case the actors are all gone.
    if (m_spScene && m_spScene->GetPhysXScene())
    {
        NiPhysXManager::GetPhysXManager()->WaitSDKLock();
        for (unsigned int ui = 0; ui < m_kActorPool.GetSize(); ui++)
        {
            NxActor* pkActor = m_kActorPool.GetAt(ui);
            if (pkActor)
                m_spScene->GetPhysXScene()->releaseActor(*pkActor);
        }
        NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();
    }
    m_kActorPool.RemoveAll();
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::MapActorPool(NiTMap<NxActor*, bool>& kMap)
{
    for (unsigned int uj = 0; uj < m_kActorPool.GetSize(); uj++)
    {
        NxActor* pkActor = m_kActorPool.GetAt(uj);
        if (pkActor)
            kMap.SetAt(pkActor, true);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXPSysData);
//---------------------------------------------------------------------------
void NiPhysXPSysData::CopyMembers(NiPhysXPSysData* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysData::CopyMembers(pkDest, kCloning);
    
    // The cloned particle system must be attached to a prop and scene
    pkDest->m_spScene = 0;
    
    // Copy particle data. This also copies the actor pointers, which we want
    // to zero so that new actors are created later.
    pkDest->m_pkPhysXParticleInfo = NiNew NiPhysXParticleInfo[m_usVertices];
    for (unsigned short us = 0; us < GetActiveVertexCount(); us++)
    {
        // Make sure the old actor is gone
        pkDest->m_pkPhysXParticleInfo[us].m_pkActor = 0;
    }

    pkDest->m_bFillActorPoolOnLoad = m_bFillActorPoolOnLoad;
    pkDest->m_bKeepsActorMeshes = m_bKeepsActorMeshes;
    pkDest->m_uiDefaultActorPoolSize = m_uiDefaultActorPoolSize;
    pkDest->m_kActorPoolCenter = m_kActorPoolCenter;
    pkDest->m_kActorPoolDim = m_kActorPoolDim;
    
    // We do not clone the pool - systems do not share pools
    pkDest->m_kActorPool.SetSize(m_uiDefaultActorPoolSize);
    
    pkDest->m_spActorDescriptor =
        (NiPhysXActorDesc*)m_spActorDescriptor->CreateSharedClone(kCloning);
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysData::ProcessClone(kCloning);

    m_spActorDescriptor->ProcessClone(kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSysData);
//---------------------------------------------------------------------------
bool NiPhysXPSysData::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::LoadBinary(NiStream& kStream)
{
    NiPSysData::LoadBinary(kStream);

    m_pkPhysXParticleInfo = NiNew NiPhysXParticleInfo[m_usVertices];
    // Don't have to load anything for PhysX particle info

    m_spScene = (NiPhysXScene*)kStream.ResolveLinkID();

    NiBool bVal;
    
    // Load Fill Pools on Load
    NiStreamLoadBinary(kStream, bVal);
    m_bFillActorPoolOnLoad = (bVal != 0);

    // Load actor meshes on load
    NiStreamLoadBinary(kStream, bVal);
    m_bKeepsActorMeshes = (bVal != 0);

    // Load the Default Pool Size
    NiStreamLoadBinary(kStream, m_uiDefaultActorPoolSize);

    unsigned int uiGenerationPoolSize;
    NiStreamLoadBinary(kStream, uiGenerationPoolSize);
    m_kActorPoolCenter.LoadBinary(kStream);
    m_kActorPoolDim.LoadBinary(kStream);

    if (uiGenerationPoolSize > 0)
    {
        m_kActorPool.SetSize(uiGenerationPoolSize);
    }

    m_spActorDescriptor =(NiPhysXActorDesc*)kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::LinkObject(NiStream& kStream)
{
    NiPSysData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSysData::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysData::RegisterStreamables(kStream))
        return false;
    
    if (m_spScene) 
        m_spScene->RegisterStreamables(kStream);
    
    if (m_spActorDescriptor)
        m_spActorDescriptor->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::SaveBinary(NiStream& kStream)
{
    NiPSysData::SaveBinary(kStream);

    // Nothing in PhysX PSys info gets saved
    
    kStream.SaveLinkID(m_spScene);
    
    NiStreamSaveBinary(kStream, NiBool(m_bFillActorPoolOnLoad));
    NiStreamSaveBinary(kStream, NiBool(m_bKeepsActorMeshes));
    NiStreamSaveBinary(kStream, m_uiDefaultActorPoolSize);

    NiStreamSaveBinary(kStream, m_kActorPool.GetAllocatedSize());
    m_kActorPoolCenter.SaveBinary(kStream);
    m_kActorPoolDim.SaveBinary(kStream);

    kStream.SaveLinkID(m_spActorDescriptor);
}
//---------------------------------------------------------------------------
bool NiPhysXPSysData::IsEqual(NiObject* pkObject)
{
    if (!NiPSysData::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXPSysData* pkDest = (NiPhysXPSysData*) pkObject;

    if (m_bFillActorPoolOnLoad != pkDest->m_bFillActorPoolOnLoad ||
        m_bKeepsActorMeshes != pkDest->m_bKeepsActorMeshes ||
        m_uiDefaultActorPoolSize != pkDest->m_uiDefaultActorPoolSize ||
        m_kActorPoolCenter != pkDest->m_kActorPoolCenter ||
        m_kActorPoolDim != pkDest->m_kActorPoolDim)
    {
        return false;
    }
                    
    if (m_kActorPool.GetAllocatedSize() !=
        pkDest->m_kActorPool.GetAllocatedSize())
        return false;

    if (!m_spActorDescriptor->IsEqual(pkDest->m_spActorDescriptor))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysData::PostLinkObject(NiStream& kStream)
{
    NiPSysData::PostLinkObject(kStream);

    if (m_bFillActorPoolOnLoad)
        FillActorPool(m_spActorDescriptor);
    
    // Can't stream  a particle system with active particles, otherwise we'd
    // need to initialize them here, or in a post-update for
    // NiPhysXParticleSystem.
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXPSysData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysData::GetViewerStrings(pkStrings);
    
    pkStrings->Add(NiGetViewerString(NiPhysXPSysData::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("m_pkPhysXParticleInfo",
        m_pkPhysXParticleInfo));
    pkStrings->Add(NiGetViewerString("m_spScene", m_spScene));

    pkStrings->Add(NiGetViewerString("FillActorPoolOnLoad",
        m_bFillActorPoolOnLoad));
    pkStrings->Add(NiGetViewerString("KeepsActorMeshes",
        m_bKeepsActorMeshes));
    pkStrings->Add(NiGetViewerString("DefaultActorPoolSize",
        m_usAddedParticlesBase));
        
    if (m_spActorDescriptor)
    {
        m_spActorDescriptor->GetViewerStrings(pkStrings);
    }
    else
    {
        pkStrings->Add(NiGetViewerString("m_spActorDescriptor", 0));
    }
}
//---------------------------------------------------------------------------
