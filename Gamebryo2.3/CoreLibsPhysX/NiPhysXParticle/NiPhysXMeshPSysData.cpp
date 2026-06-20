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

#include "NiPhysXParticle.h"
#include <NiBool.h>

NiImplementRTTI(NiPhysXMeshPSysData, NiMeshPSysData);

//---------------------------------------------------------------------------
NiPhysXMeshPSysData::NiPhysXMeshPSysData(unsigned short usMaxNumParticles,
    bool bHasColors, bool bHasRotations, unsigned int uiNumGenerations,
    unsigned int uiMeshPoolSize, bool bFillMeshPoolsOnLoad,
    NiPhysXScene* pkScene, unsigned int uiCompartmentID,
    unsigned int uiActorPoolSize, bool bFillActorPoolsOnLoad,
    bool bKeepsMeshes) :
    NiMeshPSysData(usMaxNumParticles, bHasColors, bHasRotations,
    uiMeshPoolSize, uiNumGenerations, bFillMeshPoolsOnLoad)
{
    SetOnConstruction(usMaxNumParticles, uiNumGenerations, pkScene,
        uiCompartmentID, uiActorPoolSize, bFillActorPoolsOnLoad, bKeepsMeshes);
}
//---------------------------------------------------------------------------
NiPhysXMeshPSysData::NiPhysXMeshPSysData(NiMeshPSysData* pkPSysData,
    NiPhysXScene* pkScene, unsigned int uiCompartmentID,
    unsigned int uiNumGenerations, unsigned int uiActorPoolSize,
    bool bFillActorPoolsOnLoad, bool bKeepsMeshes) :
    NiMeshPSysData(pkPSysData->GetMaxNumParticles(),
    pkPSysData->GetColors() != NULL, pkPSysData->GetRotationAngles() != NULL,
    pkPSysData->GetDefaultPoolsSize(), pkPSysData->GetNumGenerations(),
    pkPSysData->GetFillPoolsOnLoad())
{
    SetOnConstruction(pkPSysData->GetMaxNumParticles(),
        uiNumGenerations, pkScene, uiCompartmentID,
        uiActorPoolSize, bFillActorPoolsOnLoad, bKeepsMeshes);
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::SetOnConstruction(unsigned short usMaxNumParticles,
    unsigned int uiNumGenerations, NiPhysXScene* pkScene,
    unsigned int uiCompartmentID, unsigned int uiActorPoolSize,
    bool bFillActorPoolsOnLoad, bool bKeepsMeshes)
{
    NIASSERT(usMaxNumParticles > 0);

    // Set the scene in which it resides to 0 for now.
    m_spScene = 0;
    m_uiCompartmentID = uiCompartmentID;

    // Create PhysX specific particle data
    m_pkPhysXParticleInfo = NiNew NiPhysXParticleInfo[usMaxNumParticles];
    
    // Set up space for the actor descriptors
    m_kActorDescriptors.SetSize(uiNumGenerations);
    
    m_bFillActorPoolsOnLoad = bFillActorPoolsOnLoad;
    m_bKeepsActorMeshes = bKeepsMeshes;

    // Initialize the size of the pool. This may be ignored later.
    m_uiDefaultActorPoolSize = uiActorPoolSize;
    if (m_uiDefaultActorPoolSize ==
        (unsigned int)NiMeshPSysData::DEFAULT_POOL_SIZE)
        m_uiDefaultActorPoolSize = usMaxNumParticles;

    // Set the number of pools, one for each generation
    m_kActorPools.SetSize(uiNumGenerations);
    
    // Create the individual pools for each generation, but set to zero size
    for(unsigned int ui = 0; ui < uiNumGenerations; ui++)
    {
        NiPhysXMeshPSysData::ActorArray* pkPool =
            NiNew NiPhysXMeshPSysData::ActorArray;
        pkPool->SetSize(0);

        m_kActorPools.SetAt(ui, pkPool);
    }
    m_kActorPoolCenter = NiPoint3::ZERO;
    m_kActorPoolDim = NiPoint3(100.0f, 100.0f, 100.0f);

    // Attach the scene, which will fill actor pools if necessary
    if (pkScene)
        SetScene(pkScene);
}
//---------------------------------------------------------------------------
NiPhysXMeshPSysData::NiPhysXMeshPSysData()
{
}
//---------------------------------------------------------------------------
NiPhysXMeshPSysData::~NiPhysXMeshPSysData()
{
    NiDelete[] m_pkPhysXParticleInfo;
    unsigned int uiNumGenerations = m_kActorPools.GetSize();
    for (unsigned int ui = uiNumGenerations; ui > 0; ui--)
    {
        EmptyActorPool(ui - 1);
        NiDelete m_kActorPools.RemoveEnd();
    }
    m_spScene = 0;
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::RemoveParticle(unsigned short usParticle)
{
    unsigned short usLastParticle = GetActiveVertexCount() - 1;
    unsigned short usGeneration = m_pkParticleInfo[usParticle].m_usGeneration;

    NiMeshPSysData::RemoveParticle(usParticle);

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
        AddActorToPool(usGeneration, pkRemovedActor);
    }
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::SetScene(NiPhysXScene* pkScene)
{
    if (m_spScene)
    {
        // Clear out all particles.
        SetActiveVertexCount(0);
        
        // Clear the actor pools
        for (unsigned int ui = m_kActorPools.GetSize(); ui > 0; ui--)
        {
            EmptyActorPool(ui - 1);
        }
    }

    m_spScene = pkScene;
    
    if (!m_spScene)
        return;
    
    // Create the individual pools for each generation
    for(unsigned int ui = 0; ui < GetNumGenerations(); ui++)
    {
        NiPhysXMeshPSysData::ActorArray* pkPool = m_kActorPools.GetAt(ui);
        pkPool->SetSize(m_uiDefaultActorPoolSize);
    }

    if (m_bFillActorPoolsOnLoad && m_uiDefaultActorPoolSize > 0)
    {
        // Fill the Pools for each generation
        unsigned int uiGen;
        for (uiGen = 0; uiGen < GetNumGenerations(); uiGen++)
        {
            // Make sure we have a mesh to put in that pool
            if ((uiGen < m_kActorDescriptors.GetAllocatedSize()) &&
                m_kActorDescriptors.GetAt(uiGen))
            {
                FillActorPool(uiGen, m_kActorDescriptors.GetAt(uiGen));
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::SetActiveVertexCount(unsigned short usActive)
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
    
    NiMeshPSysData::SetActiveVertexCount(usActive);
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::SetNumGenerations(unsigned int uiNumGenerations)
{
    // Removing Pools
    unsigned int uiOldSize = m_kActorPools.GetSize();
    if (uiNumGenerations < uiOldSize)
    {
        for (unsigned int ui = uiOldSize; ui > uiNumGenerations; ui--)
        {
            EmptyActorPool(ui - 1);
            NiDelete m_kActorPools.RemoveEnd();
        }
    } // Adding Pools
    else if (uiNumGenerations > uiOldSize)
    {
        m_kActorPools.SetSize(uiNumGenerations);

        // Allocate the New Pools.
        unsigned int ui;
        for (ui = uiOldSize; ui < uiNumGenerations; ui++)
        {
           // Create the Pool
            NiPhysXMeshPSysData::ActorArray* pkPool =
                NiNew NiPhysXMeshPSysData::ActorArray;
            pkPool->SetSize(m_uiDefaultActorPoolSize);

            m_kActorPools.SetAt(ui, pkPool);
        }

    }
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::FillActorPool(unsigned int uiGeneration,
    NiPhysXActorDesc* pkOriginal)
{
    if (!m_spScene || !m_spScene->GetPhysXScene())
        return;
        
    NIASSERT(pkOriginal != NULL);

    // Make sure we have something to fill
    if (uiGeneration >= m_kActorPools.GetSize() ||
        !m_kActorPools.GetAt(uiGeneration))
        return;

    ActorArray* pkPool = m_kActorPools.GetAt(uiGeneration);
    
    // Get any compartment
    NxCompartment* pkCompartment = 0;
    if (m_uiCompartmentID)
    {
        pkCompartment = m_spScene->GetCompartmentAt(m_uiCompartmentID);
    }

    NiPhysXManager::GetPhysXManager()->WaitSDKLock();
    NIASSERT(!m_spScene->GetInSimFetch());
    for(unsigned int ui = 0; ui < pkPool->GetAllocatedSize(); ui++)
    {
        NxActor* pkActor = pkOriginal->CreateActor(m_spScene->GetPhysXScene(),
            pkCompartment, 0, NiPhysXTypes::NXMAT34_ID, m_bKeepsActorMeshes);
        if (!pkActor)
            continue;
        NxVec3 kPosn(
            (NiUnitRandom() * 2.0f - 1.0f) * m_kActorPoolDim.x
            + m_kActorPoolCenter.x,
            (NiUnitRandom() * 2.0f - 1.0f) * m_kActorPoolDim.y
            + m_kActorPoolCenter.y,
            (NiUnitRandom() * 2.0f - 1.0f) * m_kActorPoolDim.z
            + m_kActorPoolCenter.z);
        pkActor->setGlobalPosition(kPosn);
        pkActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
        
        pkPool->SetAt(ui, pkActor);
    }
    NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();
}
//---------------------------------------------------------------------------
NxActor* NiPhysXMeshPSysData::RemoveActorFromPool(unsigned int uiGeneration)
{
    // Check if the generation has a pool
    if (uiGeneration >= m_kActorPools.GetSize())
    {
        return NULL;
    }

    // Check if the generation has a pool allocated
    if (!m_kActorPools.GetAt(uiGeneration))
        return NULL;

    return m_kActorPools.GetAt(uiGeneration)->RemoveEnd();
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::AddActorToPool(unsigned int uiGeneration, 
    NxActor* pkActor)
{
    if (!pkActor)
        return;
        
    // Check if the generation has a pool
    if (uiGeneration >= m_kActorPools.GetSize())
        return;

    // Check there is a pool to add into
    if (!m_kActorPools.GetAt(uiGeneration))
        return;

    if (m_kActorPools.GetAt(uiGeneration)->GetSize() <
        m_kActorPools.GetAt(uiGeneration)->GetAllocatedSize())
    {
        m_kActorPools.GetAt(uiGeneration)->Add(pkActor);
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
void NiPhysXMeshPSysData::EmptyActorPool(unsigned int uiGeneration)
{
    NIASSERT(uiGeneration < m_kActorPools.GetSize());

    // It could be that the PhysX scene has already been deleted, in which
    // case the actors are all gone.
    if (m_spScene && m_spScene->GetPhysXScene())
    {
        ActorArray* pkActorArray = m_kActorPools.GetAt(uiGeneration);
        
        NiPhysXManager::GetPhysXManager()->WaitSDKLock();
        for (unsigned int ui = 0; ui < pkActorArray->GetSize(); ui++)
        {
            NxActor* pkActor = pkActorArray->GetAt(ui);
            if (pkActor)
                m_spScene->GetPhysXScene()->releaseActor(*pkActor);
        }
        NiPhysXManager::GetPhysXManager()->ReleaseSDKLock();
    }
    m_kActorPools.GetAt(uiGeneration)->RemoveAll();
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::MapActorPools(NiTMap<NxActor*, bool>& kMap)
{
    for (unsigned int ui = 0; ui < GetNumGenerations(); ui++)
    {
        ActorArray* pkActorArray = m_kActorPools.GetAt(ui);
        for (unsigned int uj = 0; uj < pkActorArray->GetSize(); uj++)
        {
            NxActor* pkActor = pkActorArray->GetAt(uj);
            if (pkActor)
                kMap.SetAt(pkActor, true);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXMeshPSysData);
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::CopyMembers(NiPhysXMeshPSysData* pkDest,
    NiCloningProcess& kCloning)
{
    NiMeshPSysData::CopyMembers(pkDest, kCloning);
    
    // The cloned particle system must be attached to a prop and scene.
    pkDest->m_spScene = 0;
    
    // Copy particle data. This also copies the actor pointers, which we want
    // to zero so that new actors are created later.
    pkDest->m_pkPhysXParticleInfo = NiNew NiPhysXParticleInfo[m_usVertices];
    for (unsigned short us = 0; us < GetActiveVertexCount(); us++)
    {
        // Make sure the old actor is gone
        pkDest->m_pkPhysXParticleInfo[us].m_pkActor = 0;
    }

    pkDest->m_bFillActorPoolsOnLoad = m_bFillActorPoolsOnLoad;
    pkDest->m_bKeepsActorMeshes = m_bKeepsActorMeshes;
    pkDest->m_uiDefaultActorPoolSize = m_uiDefaultActorPoolSize;
    
    // We do not clone the pools - systems do not share pools
    unsigned int uiNumGenerations = GetNumGenerations();
    pkDest->m_kActorPools.SetSize(uiNumGenerations);

    // Create the individual pools for each generation
    for(unsigned int ui = 0; ui < uiNumGenerations; ui++)
    {
        NiPhysXMeshPSysData::ActorArray* pkPool =
            NiNew NiPhysXMeshPSysData::ActorArray;
        pkPool->SetSize(m_uiDefaultActorPoolSize);

        pkDest->m_kActorPools.SetAt(ui, pkPool);
    }
    pkDest->m_kActorPoolCenter = m_kActorPoolCenter;
    pkDest->m_kActorPoolDim = m_kActorPoolDim;

    pkDest->m_kActorDescriptors.SetSize(m_kActorDescriptors.GetSize());
    for (unsigned int ui = 0; ui < m_kActorDescriptors.GetSize(); ui++)
    {
        NiPhysXActorDesc* pkActorDesc = m_kActorDescriptors.GetAt(ui);
        if (pkActorDesc)
        {
            pkDest->m_kActorDescriptors.SetAt(ui, 
                (NiPhysXActorDesc*)pkActorDesc->CreateSharedClone(kCloning));
        }
        else
        {
            pkDest->m_kActorDescriptors.SetAt(ui, 0);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::ProcessClone(NiCloningProcess& kCloning)
{
    NiMeshPSysData::ProcessClone(kCloning);
    
    NiObject* pkClone;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkClone));
    
    for (unsigned int ui = 0; ui < m_kActorDescriptors.GetSize(); ui++)
    {
        NiPhysXActorDesc* pkActorDesc = m_kActorDescriptors.GetAt(ui);
        if (pkActorDesc)
        {
            pkActorDesc->ProcessClone(kCloning);
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXMeshPSysData);
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysData::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::LoadBinary(NiStream& kStream)
{
    NiMeshPSysData::LoadBinary(kStream);

    m_pkPhysXParticleInfo = NiNew NiPhysXParticleInfo[m_usVertices];
    // Don't have to load anything for PhysX particle info

    m_spScene = (NiPhysXScene*)kStream.ResolveLinkID();

    NiBool bVal;
    
    // Load Fill Pools on Load
    NiStreamLoadBinary(kStream, bVal);
    m_bFillActorPoolsOnLoad = (bVal != 0);

    // Load Fill Pools on Load
    NiStreamLoadBinary(kStream, bVal);
    m_bKeepsActorMeshes = (bVal != 0);

    // Load the Default Pool Size
    NiStreamLoadBinary(kStream, m_uiDefaultActorPoolSize);

    unsigned int uiNumGenerations;
    unsigned int uiGenerationPoolSize;

    NiStreamLoadBinary(kStream, uiNumGenerations);

    // Load prototypical actors and set pools up
    m_kActorPools.SetSize(uiNumGenerations);
    for (unsigned int ui = 0; ui < uiNumGenerations; ui++)
    {
        NiStreamLoadBinary(kStream, uiGenerationPoolSize);

        if (uiGenerationPoolSize > 0)
        {
            // Create the Pool
            NiPhysXMeshPSysData::ActorArray* pkPool = NiNew 
                NiPhysXMeshPSysData::ActorArray;
            pkPool->SetSize(uiGenerationPoolSize);

            m_kActorPools.SetAt(ui, pkPool);
        }
    }
    m_kActorPoolCenter.LoadBinary(kStream);
    m_kActorPoolDim.LoadBinary(kStream);

    unsigned int uiSize;
    NiStreamLoadBinary(kStream, uiSize);
    m_kActorDescriptors.SetSize(uiSize);
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXActorDesc* pkActor = (NiPhysXActorDesc*)
            kStream.ResolveLinkID();
        m_kActorDescriptors.SetAt(ui, pkActor);
    }
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::LinkObject(NiStream& kStream)
{
    NiMeshPSysData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysData::RegisterStreamables(NiStream& kStream)
{
    if (!NiMeshPSysData::RegisterStreamables(kStream))
        return false;
    
    if (m_spScene) 
        m_spScene->RegisterStreamables(kStream);
    
    for (unsigned int ui = 0; ui < m_kActorDescriptors.GetSize(); ui++)
    {
        NiPhysXActorDesc* pkActor = m_kActorDescriptors.GetAt(ui);
        if (pkActor)
        {
            pkActor->RegisterStreamables(kStream);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::SaveBinary(NiStream& kStream)
{
    NiMeshPSysData::SaveBinary(kStream);

    // Nothing in PhysX PSys info gets saved
    
    kStream.SaveLinkID(m_spScene);
    
    NiStreamSaveBinary(kStream, NiBool(m_bFillActorPoolsOnLoad));
    NiStreamSaveBinary(kStream, NiBool(m_bKeepsActorMeshes));
    NiStreamSaveBinary(kStream, m_uiDefaultActorPoolSize);

    unsigned int uiNumGenerations = m_kActorPools.GetSize();
    NiStreamSaveBinary(kStream, uiNumGenerations);

    for (unsigned int ui = 0; ui < uiNumGenerations; ui++)
    {
        if (m_kActorPools.GetAt(ui))
        {
            NiStreamSaveBinary(kStream, 
                m_kActorPools.GetAt(ui)->GetAllocatedSize());
        }
        else
        {
            NiStreamSaveBinary(kStream, 0);
        }
    }
    m_kActorPoolCenter.SaveBinary(kStream);
    m_kActorPoolDim.SaveBinary(kStream);

    unsigned int uiSize = m_kActorDescriptors.GetSize();
    NiStreamSaveBinary(kStream, uiSize);
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXActorDesc* pkActor = m_kActorDescriptors.GetAt(ui);
        kStream.SaveLinkID(pkActor);
    }
}
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysData::IsEqual(NiObject* pkObject)
{
    if (!NiMeshPSysData::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXMeshPSysData* pkDest = (NiPhysXMeshPSysData*) pkObject;

    if (m_bFillActorPoolsOnLoad != pkDest->m_bFillActorPoolsOnLoad ||
        m_bKeepsActorMeshes != pkDest->m_bKeepsActorMeshes ||
        m_uiDefaultActorPoolSize != pkDest->m_uiDefaultActorPoolSize ||
        m_kActorPoolCenter != pkDest->m_kActorPoolCenter ||
        m_kActorPoolDim != pkDest->m_kActorPoolDim)
    {
        return false;
    }
    
    for (unsigned int ui = 0; ui < m_kActorPools.GetSize(); ui++)
    {
        if (m_kActorPools.GetAt(ui))
        {
            if (!pkDest->m_kActorPools.GetAt(ui))
                return false;
                
            if (m_kActorPools.GetAt(ui)->GetAllocatedSize() !=
                pkDest->m_kActorPools.GetAt(ui)->GetAllocatedSize())
                return false;
        }
        else if (pkDest->m_kActorPools.GetAt(ui))
        {
            return false;
        }
    }

    if (m_kActorDescriptors.GetSize() != pkDest->m_kActorDescriptors.GetSize())
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_kActorDescriptors.GetSize(); ui++)
    {
        NiPhysXActorDesc* pkActor1 = m_kActorDescriptors.GetAt(ui);
        NiPhysXActorDesc* pkActor2 = pkDest->m_kActorDescriptors.GetAt(ui);
        if ((pkActor1 && !pkActor2) || (!pkActor1 && pkActor2) ||
            (pkActor1 && pkActor2 && !pkActor1->IsEqual(pkActor2)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::PostLinkObject(NiStream& kStream)
{
    NiMeshPSysData::PostLinkObject(kStream);

    if (m_bFillActorPoolsOnLoad)
    {
        // Fill the Pools for each generation
        unsigned int uiGen;
        for (uiGen = 0; uiGen < GetNumGenerations(); uiGen++)
        {
            // Make sure we have a mesh to put in that pool
            if ((uiGen < m_kActorDescriptors.GetAllocatedSize()) &&
                m_kActorDescriptors.GetAt(uiGen))
            {
                FillActorPool(uiGen, m_kActorDescriptors.GetAt(uiGen));
            }
        }
    }
    
    // Can't stream  a particle system with active particles, otherwise we'd
    // need to initialize them here, or in a post-update for
    // NiPhysXMeshParticleSystem.
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXMeshPSysData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiMeshPSysData::GetViewerStrings(pkStrings);
    
    pkStrings->Add(NiGetViewerString(NiPhysXMeshPSysData::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("m_pkPhysXParticleInfo",
        m_pkPhysXParticleInfo));
    pkStrings->Add(NiGetViewerString("m_spScene", m_spScene));

    pkStrings->Add(NiGetViewerString("FillActorPoolsOnLoad",
        m_bFillActorPoolsOnLoad));
    pkStrings->Add(NiGetViewerString("KeepsActorMeshes",
        m_bKeepsActorMeshes));
    pkStrings->Add(NiGetViewerString("DefaultActorPoolSize",
        m_usAddedParticlesBase));

    for (unsigned int ui = 0; ui < m_kActorDescriptors.GetSize(); ui++)
    {
        NiPhysXActorDesc* pkActorDesc = m_kActorDescriptors.GetAt(ui);
        if (pkActorDesc)
        {
            pkActorDesc->GetViewerStrings(pkStrings);
        }
        else
        {
            pkStrings->Add(NiGetViewerString("Missing actor", 0));
        }
    }
}
//---------------------------------------------------------------------------
