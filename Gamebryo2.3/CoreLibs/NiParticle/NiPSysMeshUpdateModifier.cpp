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

#include "NiPSysMeshUpdateModifier.h"
#include "NiMeshPSysData.h"
#include "NiMeshParticleSystem.h"

NiImplementRTTI(NiPSysMeshUpdateModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysMeshUpdateModifier::NiPSysMeshUpdateModifier(const char* pcName) :
    NiPSysModifier(pcName, ORDER_POSTPOSUPDATE)
{
}
//---------------------------------------------------------------------------
NiPSysMeshUpdateModifier::NiPSysMeshUpdateModifier()
{
}
//---------------------------------------------------------------------------
void NiPSysMeshUpdateModifier::Update(float fTime, NiPSysData* pkData)
{
    NIASSERT(NiIsKindOf(NiMeshPSysData, pkData));
    NiMeshPSysData* pkMeshData = (NiMeshPSysData*) pkData;

    float* pfRotationAngles = pkData->GetRotationAngles();
    NiPoint3* pkRotationAxes = pkData->GetRotationAxes();

    for (unsigned short us = 0; us < pkMeshData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkMeshData
            ->GetParticleInfo()[us];

        NiAVObject* pkParticleMesh = pkMeshData->GetMeshAt(us);
        NIASSERT(pkParticleMesh);

        // Update the position, rotation and scale of the particle mesh
        pkParticleMesh->SetTranslate(pkData->GetVertices()[us]);
        if (pfRotationAngles && pkRotationAxes)
        {
            NiMatrix3 kRotate;
            kRotate.MakeRotation(pfRotationAngles[us], pkRotationAxes[us]);
            pkParticleMesh->SetRotate(kRotate);
        }
        pkParticleMesh->SetScale(pkData->GetSizes()[us] *
            pkData->GetRadii()[us]);

        // Update the particle mesh with it's age
        pkParticleMesh->UpdateDownwardPass(pkCurrentParticle->m_fAge, true);
    }
}
//---------------------------------------------------------------------------
void NiPSysMeshUpdateModifier::Initialize(NiPSysData* pkData,
    unsigned short usNewParticle)
{
    NIASSERT(NiIsKindOf(NiMeshPSysData, pkData));
    NiMeshPSysData* pkMeshData = (NiMeshPSysData*) pkData;

    NiParticleInfo* pkCurrentParticle = &pkMeshData->GetParticleInfo()
        [usNewParticle];

    unsigned short usGen = pkCurrentParticle->m_usGeneration;
    if (usGen >= m_kMeshes.GetSize())
    {
        usGen = m_kMeshes.GetSize() - 1;
    }

    // Try to pull from the pool
    NiAVObjectPtr spClonedMesh = pkMeshData->RemoveFromPool(
        (unsigned int)usGen);

    if (!spClonedMesh)
        spClonedMesh = (NiAVObject*) m_kMeshes.GetAt(usGen)->Clone();

    spClonedMesh->SetAppCulled(true);
    NiTimeController::StartAnimations(spClonedMesh, 0.0f);
    pkMeshData->SetMeshAt(usNewParticle, spClonedMesh);

    spClonedMesh->UpdatePropertiesDownward(m_pkTarget->GetPropertyState());
    spClonedMesh->UpdateEffectsDownward(m_pkTarget->GetEffectState());
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysMeshUpdateModifier);
//---------------------------------------------------------------------------
void NiPSysMeshUpdateModifier::CopyMembers(NiPSysMeshUpdateModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);

    for (unsigned int ui = 0; ui < m_kMeshes.GetSize(); ui++)
    {
        NiAVObject* pkMesh = m_kMeshes.GetAt(ui);
        if (pkMesh)
        {
            NiAVObject* pkClone = (NiAVObject*) pkMesh->CreateClone(
                kCloning);
            NIASSERT(pkClone);
            pkDest->SetMeshAt(ui, pkClone);
        }
    }
}
//---------------------------------------------------------------------------
void NiPSysMeshUpdateModifier::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifier::ProcessClone(kCloning);

    for (unsigned int ui = 0; ui < m_kMeshes.GetSize(); ui++)
    {
        NiAVObject* pkMesh = m_kMeshes.GetAt(ui);
        if (pkMesh)
        {
            pkMesh->ProcessClone(kCloning);
        }
    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysMeshUpdateModifier);
//---------------------------------------------------------------------------
void NiPSysMeshUpdateModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    // The Order in previous versions was incorrect
    m_uiOrder = ORDER_POSTPOSUPDATE;

    kStream.ReadMultipleLinkIDs();  // m_kMeshes
}
//---------------------------------------------------------------------------
void NiPSysMeshUpdateModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);

    unsigned int uiSize = kStream.GetNumberOfLinkIDs();
    m_kMeshes.SetSize(uiSize);
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiAVObject* pkMesh = (NiAVObject*) kStream.GetObjectFromLinkID();
        m_kMeshes.SetAt(ui, pkMesh);
    }
}
//---------------------------------------------------------------------------
bool NiPSysMeshUpdateModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysModifier::RegisterStreamables(kStream))
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_kMeshes.GetSize(); ui++)
    {
        NiAVObject* pkMesh = m_kMeshes.GetAt(ui);
        if (pkMesh)
        {
            pkMesh->RegisterStreamables(kStream);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSysMeshUpdateModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    unsigned int uiSize = m_kMeshes.GetSize();
    NiStreamSaveBinary(kStream, uiSize);
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiAVObject* pkMesh = m_kMeshes.GetAt(ui);
        kStream.SaveLinkID(pkMesh);
    }
}
//---------------------------------------------------------------------------
bool NiPSysMeshUpdateModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysMeshUpdateModifier* pkDest = (NiPSysMeshUpdateModifier*) pkObject;

    if (m_kMeshes.GetSize() != pkDest->m_kMeshes.GetSize())
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_kMeshes.GetSize(); ui++)
    {
        NiAVObject* pkMesh1 = m_kMeshes.GetAt(ui);
        NiAVObject* pkMesh2 = pkDest->GetMeshAt(ui);
        if ((pkMesh1 && !pkMesh2) || (!pkMesh1 && pkMesh2) ||
            (pkMesh1 && pkMesh2 && !pkMesh1->IsEqual(pkMesh2)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPSysMeshUpdateModifier::PostLinkObject(NiStream& kStream)
{
    NiPSysModifier::PostLinkObject(kStream);

    // It is possible to have no target if this modifier was attached to
    // a particle system that was skipped in the loading process.
    if (!m_pkTarget)
        return;

    // Specify the number of pools
    if(kStream.GetFileVersion() <= NiStream::GetVersion(10,1,0,0))
    {
        NIASSERT(NiIsKindOf(NiMeshParticleSystem, m_pkTarget));

        NiGeometryData* pkData = m_pkTarget->GetModelData();

        NIASSERT(pkData);
        NIASSERT(NiIsKindOf(NiMeshPSysData, pkData));

        ((NiMeshPSysData*)pkData)->SetNumGenerations(m_kMeshes.GetSize());
    }
    else
    {
        NIASSERT(NiIsKindOf(NiMeshParticleSystem, m_pkTarget));

        NiGeometryData* pkData = m_pkTarget->GetModelData();

        NIASSERT(pkData);
        NIASSERT(NiIsKindOf(NiMeshPSysData, pkData));

        NiMeshPSysData* pkMeshData = (NiMeshPSysData*)pkData;

        if (pkMeshData->GetFillPoolsOnLoad())
        {
            // Fill the Pools for each generation
            unsigned int uiGen;
            for (uiGen = 0; uiGen < pkMeshData->GetNumGenerations(); uiGen++)
            {
                // Make sure we have a mesh to put in that pool
                if ((uiGen < m_kMeshes.GetAllocatedSize()) &&
                    m_kMeshes.GetAt(uiGen))
                {
                    pkMeshData->FillPool(uiGen, m_kMeshes.GetAt(uiGen));
                }
            }
        }
    }
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysMeshUpdateModifier::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysMeshUpdateModifier::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
