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

#include "NiMeshPSysData.h"
#include <NiBool.h>

NiImplementRTTI(NiMeshPSysData, NiPSysData);

//---------------------------------------------------------------------------
NiMeshPSysData::NiMeshPSysData(unsigned short usMaxNumParticles,
    bool bHasColors, bool bHasRotations, unsigned int uiPoolSize,
    unsigned int uiNumGenerations, bool bFillPoolsOnLoad) : NiPSysData(
    usMaxNumParticles, bHasColors, bHasRotations)
{
    NIASSERT(usMaxNumParticles > 0);

    if (bHasRotations)
    {
        m_pkRotationAxes = NiNew NiPoint3[usMaxNumParticles];
    }

    m_spParticleMeshes = NiNew NiNode(usMaxNumParticles);

    m_bFillPoolsOnLoad = bFillPoolsOnLoad;

    // Initialize the size of the pool
    m_uiDefaultPoolSize = uiPoolSize;

    if (m_uiDefaultPoolSize == (unsigned int) -1)
        m_uiDefaultPoolSize = usMaxNumParticles;

    // Set the Number of Pools one for each generation
    m_kPools.SetSize(uiNumGenerations);

    // Create the Individual Pools for each generation
    for(unsigned int uiLoop = 0; uiLoop < uiNumGenerations; uiLoop++)
    {
        NiMeshPSysData::NiAVObjectArray* pkPool = NiNew 
            NiMeshPSysData::NiAVObjectArray;
        pkPool->SetSize(m_uiDefaultPoolSize);

        m_kPools.SetAt(uiLoop, pkPool);
    }
}
//---------------------------------------------------------------------------
NiMeshPSysData::NiMeshPSysData() : m_spParticleMeshes(NULL)
{

}
//---------------------------------------------------------------------------
NiMeshPSysData::~NiMeshPSysData()
{
    // Delete Each of the pools
    while(m_kPools.GetSize() > 0)
    {
        NiDelete m_kPools.RemoveEnd();
    }
}
//---------------------------------------------------------------------------
void NiMeshPSysData::RemoveParticle(unsigned short usParticle)
{
    unsigned short usLastParticle = GetNumParticles() - 1;

    // Get this particles generation
    unsigned short usGeneration = m_pkParticleInfo[usParticle].m_usGeneration;

    NiPSysData::RemoveParticle(usParticle);

    NiAVObjectPtr spRemovedParticle;

    if (usParticle == usLastParticle)
    {
        spRemovedParticle = m_spParticleMeshes->DetachChildAt(usLastParticle);
    }
    else
    {
        NiAVObjectPtr spLastMesh = m_spParticleMeshes->DetachChildAt(
            usLastParticle);

        spRemovedParticle = m_spParticleMeshes->DetachChildAt(usParticle);

        m_spParticleMeshes->SetAt(usParticle, spLastMesh);
        spLastMesh = NULL;
    }

    // Add the particle to the appropriate pool it will be deleted if the
    // pool is full
    AddToPool(usGeneration, spRemovedParticle);
}
//---------------------------------------------------------------------------
void NiMeshPSysData::ResolveAddedParticles()
{
    for (unsigned short us = m_usAddedParticlesBase;
        us < m_usAddedParticlesBase + m_usNumAddedParticles; us++)
    {
        NiAVObject* pkMesh = m_spParticleMeshes->GetAt(us);
        if (pkMesh)
        {
            pkMesh->SetAppCulled(false);
        }
    }

    NiPSysData::ResolveAddedParticles();
}
//---------------------------------------------------------------------------
void NiMeshPSysData::SetActiveVertexCount(unsigned short usActive)
{
    for (unsigned short us = usActive; us < GetNumParticles(); us++)
    {
        m_spParticleMeshes->DetachChildAt(us);
    }

    NiPSysData::SetActiveVertexCount(usActive);
}
//---------------------------------------------------------------------------
void NiMeshPSysData::SetNumGenerations(unsigned int uiNumGenerations)
{
    // Removing Pools
    if (uiNumGenerations < m_kPools.GetSize())
    {
        while (m_kPools.GetSize() > uiNumGenerations)
        {
            NiDelete m_kPools.RemoveEnd();
        }
    } // Adding Pools
    else if (uiNumGenerations > m_kPools.GetSize())
    {
        unsigned int uiOldSize = m_kPools.GetSize();

        m_kPools.SetSize(uiNumGenerations);

        // Allocate the New Pools.
        unsigned int uiLoop;
        for (uiLoop = uiOldSize; uiLoop < uiNumGenerations; uiLoop++)
        {
           // Create the Pool
            NiMeshPSysData::NiAVObjectArray* pkPool = NiNew 
                NiMeshPSysData::NiAVObjectArray;
            pkPool->SetSize(m_uiDefaultPoolSize);

            m_kPools.SetAt(uiLoop, pkPool);
        }

    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiMeshPSysData);
//---------------------------------------------------------------------------
void NiMeshPSysData::CopyMembers(NiMeshPSysData* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysData::CopyMembers(pkDest, kCloning);

    pkDest->m_spParticleMeshes = (NiNode*) m_spParticleMeshes->CreateClone(
        kCloning);

    pkDest->SetFillPoolsOnLoad(m_bFillPoolsOnLoad);
}
//---------------------------------------------------------------------------
void NiMeshPSysData::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysData::ProcessClone(kCloning);

    m_spParticleMeshes->ProcessClone(kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiMeshPSysData);
//---------------------------------------------------------------------------
void NiMeshPSysData::LoadBinary(NiStream& kStream)
{
    NiPSysData::LoadBinary(kStream);

    if(kStream.GetFileVersion() <= NiStream::GetVersion(10,1,0,0))
    {
        // Assume a pool is equal to the max number of particles
        m_uiDefaultPoolSize = GetMaxNumParticles();

        // Assume one generation
        SetNumGenerations(1);
    }
    else
    {
        // Load the Default Pool Size
        NiStreamLoadBinary(kStream, m_uiDefaultPoolSize);

        // Load Fill Pools on Load
        NiBool bFill;
        NiStreamLoadBinary(kStream, bFill);
        m_bFillPoolsOnLoad = (bFill != 0);


        unsigned int uiNumGenerations;
        unsigned int uiGenerationPoolSize;

        NiStreamLoadBinary(kStream, uiNumGenerations);

        m_kPools.SetSize(uiNumGenerations);

        for (unsigned int uiLoop = 0; uiLoop < uiNumGenerations; uiLoop++)
        {
            NiStreamLoadBinary(kStream, uiGenerationPoolSize);

            if (uiGenerationPoolSize > 0)
            {
                // Create the Pool
                NiMeshPSysData::NiAVObjectArray* pkPool = NiNew 
                    NiMeshPSysData::NiAVObjectArray;
                pkPool->SetSize(uiGenerationPoolSize);

                m_kPools.SetAt(uiLoop, pkPool);
            }
        }
    }

    kStream.ReadLinkID();   // m_spParticleMeshes
}
//---------------------------------------------------------------------------
void NiMeshPSysData::LinkObject(NiStream& kStream)
{
    NiPSysData::LinkObject(kStream);

    m_spParticleMeshes = (NiNode*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiMeshPSysData::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysData::RegisterStreamables(kStream))
    {
        return false;
    }

    m_spParticleMeshes->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiMeshPSysData::SaveBinary(NiStream& kStream)
{
    NiPSysData::SaveBinary(kStream);

    // Save the Default Pool Size
    NiStreamSaveBinary(kStream, m_uiDefaultPoolSize);

    // Save Fill Pools on Load
    NiStreamSaveBinary(kStream, (NiBool)m_bFillPoolsOnLoad);

    // Save the number of pools
    NiStreamSaveBinary(kStream, m_kPools.GetSize());

    // Save the size of each pool
    for (unsigned int uiLoop = 0; uiLoop < m_kPools.GetSize(); uiLoop++)
    {
        if (m_kPools.GetAt(uiLoop))
            NiStreamSaveBinary(kStream, 
                m_kPools.GetAt(uiLoop)->GetAllocatedSize());
        else
            NiStreamSaveBinary(kStream, 0);
    }

    //

    kStream.SaveLinkID(m_spParticleMeshes);
}
//---------------------------------------------------------------------------
bool NiMeshPSysData::IsEqual(NiObject* pkObject)
{
    if (!NiPSysData::IsEqual(pkObject))
    {
        return false;
    }

    NiMeshPSysData* pkDest = (NiMeshPSysData*) pkObject;

    if (!m_spParticleMeshes->IsEqual(pkDest->m_spParticleMeshes))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiMeshPSysData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysData::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiMeshPSysData::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------

