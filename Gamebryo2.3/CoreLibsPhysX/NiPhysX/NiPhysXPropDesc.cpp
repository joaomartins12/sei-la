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

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

#include <NxPhysics.h>

NiImplementRTTI(NiPhysXPropDesc, NiObject);

//---------------------------------------------------------------------------
NiPhysXPropDesc::NiPhysXPropDesc(unsigned int uiMaterialHashSize)
    : m_kActors(0,5), m_kJoints(0,5), m_kClothes(0,5),
    m_kMaterials(uiMaterialHashSize)
{
    m_uiRBCompartmentID = 0;
    m_uiClothCompartmentID = 0;
    m_uiFluidCompartmentID = 0;
    m_uiNumStates = 0;
}
//---------------------------------------------------------------------------
NiPhysXPropDesc::~NiPhysXPropDesc()
{
    m_kActors.RemoveAll();
    m_kJoints.RemoveAll();
    m_kClothes.RemoveAll();
    m_kMaterials.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiPhysXPropDesc::CreateInScene(NxScene* pkScene, const NxMat34& kXform,
    NxCompartment* pkRBCompartment, NxCompartment* pkClothCompartment,
    NxCompartment* pkFluidCompartment, const bool bKeepMeshes,
    const unsigned int uiIndex)
{
    NIASSERT(uiIndex < m_uiNumStates);
    
    // Add all the materials. First find the highest index we have.
    unsigned short usMaxIndex = 0;
    unsigned short usKey;
    NiPhysXMaterialDescPtr spMaterial;
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        m_kMaterials.GetNext(iter, usKey, spMaterial);
        if (spMaterial->GetIndex() > usMaxIndex)
            usMaxIndex = spMaterial->GetIndex();
    }
    
    // Now create that many materials
    NxMaterialDesc kMaterialDesc;
    for (unsigned short i = 0; i < usMaxIndex; i++)
        pkScene->createMaterial(kMaterialDesc);    
    
    // Now set them all.
    iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        m_kMaterials.GetNext(iter, usKey, spMaterial);
        NxMaterial* pkMaterial = pkScene->getMaterialFromIndex(usKey);
        spMaterial->ToMaterialDesc(kMaterialDesc, uiIndex);
        pkMaterial->loadFromDesc(kMaterialDesc);
    }
    
    // Add all the actors
    for (unsigned int i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        spActor->CreateActor(pkScene, pkRBCompartment, uiIndex, kXform,
            bKeepMeshes);
        if (!spActor->GetActor())
        {
            RemoveFromScene(pkScene);
            return false;
        }
    }    
    
    // Add all the joints
    for (unsigned int i = 0; i < m_kJoints.GetSize(); i++)
    {
        NiPhysXJointDescPtr spJoint = m_kJoints.GetAt(i);
        if (spJoint)
        {
            NiPhysXD6JointDescPtr spD6Joint =
                NiDynamicCast(NiPhysXD6JointDesc, spJoint);
            spD6Joint->CreateJoint(pkScene, kXform);
            if (!spD6Joint->GetJoint())
            {
                RemoveFromScene(pkScene);
                return false;
            }
        }
    }
    
    // Add all the cloth
    for (unsigned int i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        spCloth->CreateCloth(pkScene, pkClothCompartment, kXform, bKeepMeshes);
        if (!spCloth->GetCloth())
        {
            RemoveFromScene(pkScene);
            return false;
        }
    }
        
    PushToSrcDest();

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::RemoveFromScene(NxScene* pkScene)
{
    // We don't free materials because they are shared and someone else
    // may be using them.
    
    // Remove all the cloth, which may reference actors
    for (unsigned int i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        spCloth->RemoveCloth(pkScene);
    }    

    // Remove all joints. They reference actors.
    for (unsigned int i = 0; i < m_kJoints.GetSize(); i++)
    {
        NiPhysXJointDescPtr spJoint = m_kJoints.GetAt(i);
        if (spJoint)
        {
            NiPhysXD6JointDescPtr spD6Joint =
                NiDynamicCast(NiPhysXD6JointDesc, spJoint);
            spD6Joint->RemoveJoint(pkScene);
        }
    }

    // Remove all the actors
    for (unsigned int i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        spActor->RemoveActor(pkScene);
    }    
}
//---------------------------------------------------------------------------
unsigned int NiPhysXPropDesc::AddState(NxScene* pkPhysXScene,
    const NxMat34& kXform)
{
    for (unsigned int i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        spActor->AddState(kXform);
    }
    
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NxMaterialIndex usKey;
        NiPhysXMaterialDescPtr spMaterialDesc;
        m_kMaterials.GetNext(iter, usKey, spMaterialDesc);
        NIASSERT(spMaterialDesc != 0);
        NxMaterial* pkNxMaterial = pkPhysXScene->getMaterialFromIndex(usKey);
        spMaterialDesc->FromMaterial(pkNxMaterial, m_uiNumStates);
    }
    
    m_uiNumStates++;
    
    return m_uiNumStates;
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::UpdateState(NxScene* pkPhysXScene,
    const unsigned int uiIndex, const NxMat34& kXform)
{
    // Update the actors
    for (unsigned int i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        spActor->UpdateState(uiIndex, kXform);
    }
    
    // Update materials
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NxMaterialIndex usKey;
        NiPhysXMaterialDescPtr spMaterialDesc;
        m_kMaterials.GetNext(iter, usKey, spMaterialDesc);
        NIASSERT(spMaterialDesc != 0);
        NxMaterial* pkNxMaterial = pkPhysXScene->getMaterialFromIndex(usKey);
        spMaterialDesc->FromMaterial(pkNxMaterial, uiIndex);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::RestoreState(NxScene* pkScene,
    const unsigned int uiIndex, const NxMat34& kXform, const bool bKeepMeshes)
{
    // Remove all the cloth. Cloth does not offer state save and restore
    // functionality, so our solution is to remove the cloth and re-add it
    NxCompartment* pkClothCompartment = 0;
    for (unsigned int i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        pkClothCompartment = spCloth->GetCloth()->getCompartment();
        spCloth->RemoveCloth(pkScene);
    }

    for (unsigned int i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        spActor->RestoreState(uiIndex, kXform);
    }
    
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NxMaterialIndex usKey;
        NiPhysXMaterialDescPtr spMaterialDesc;
        m_kMaterials.GetNext(iter, usKey, spMaterialDesc);
        NIASSERT(spMaterialDesc != 0);
        NxMaterial* pkNxMaterial = pkScene->getMaterialFromIndex(usKey);
        spMaterialDesc->ToMaterial(pkNxMaterial, uiIndex);
    }
    
    // Add all the cloth back in
    for (unsigned int i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        spCloth->CreateCloth(pkScene, pkClothCompartment, kXform, bKeepMeshes);
    }

    // Here we could restore broken joints
}
//---------------------------------------------------------------------------
NxMaterialIndex NiPhysXPropDesc::AddMaterial(NxMaterialDesc& kMaterial,
    const NxMaterialIndex usBaseIndex)
{
    unsigned short usMaxIndex = 0;
    
    // Check if it's already there
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        unsigned short usIndex;
        NiPhysXMaterialDescPtr spMaterial;
        m_kMaterials.GetNext(iter, usIndex, spMaterial);
        
        if (spMaterial->IsDataEqual(kMaterial))
            return usIndex;
            
        if (usIndex > usMaxIndex)
            usMaxIndex = usIndex;
    }
    
    usMaxIndex++;
    if (usMaxIndex < usBaseIndex)
        usMaxIndex = usBaseIndex;
    
    NiPhysXMaterialDesc* pkNewMaterial = NiNew NiPhysXMaterialDesc(usMaxIndex);
    pkNewMaterial->FromMaterialDesc(kMaterial);
    
    m_kMaterials.SetAt(usMaxIndex, pkNewMaterial);
    
    return usMaxIndex;
}
//---------------------------------------------------------------------------
NiPhysXMaterialDescPtr NiPhysXPropDesc::SetMaterialAt(
    NxMaterialDesc& kMaterial, const NxMaterialIndex usIndex)
{
    NiPhysXMaterialDescPtr spExisting;
    
    if (!m_kMaterials.GetAt(usIndex, spExisting))
        spExisting = 0;
    
    NiPhysXMaterialDesc* pkNewMaterial = NiNew NiPhysXMaterialDesc(usIndex);
    pkNewMaterial->FromMaterialDesc(kMaterial);
    m_kMaterials.SetAt(usIndex, pkNewMaterial);
    
    return spExisting;
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::PushToSrcDest() const
{
    for (unsigned int i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        if (spActor->GetSource())
            spActor->GetSource()->SetTarget(spActor->GetActor());
        if (spActor->GetDest())
        {
            spActor->GetDest()->SetActor(spActor->GetActor());
            if (spActor->GetActorParent())
            {
                spActor->GetDest()->SetActorParent(
                    spActor->GetActorParent()->GetActor());
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXPropDesc);
//---------------------------------------------------------------------------
void NiPhysXPropDesc::CopyMembers(NiPhysXPropDesc* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    // Clone the actors
    pkDest->m_kActors.SetSize(m_kActors.GetSize());
    for (unsigned int ui = 0; ui < m_kActors.GetSize(); ui++)
    {
        NiPhysXActorDescPtr spActorClone = (NiPhysXActorDesc*)
            m_kActors.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kActors.SetAt(ui, spActorClone);
    }

    // Clone the joints
    pkDest->m_kJoints.SetSize(m_kJoints.GetSize());
    for (unsigned int ui = 0; ui < m_kJoints.GetSize(); ui++)
    {
        NiPhysXJointDescPtr spJointClone = (NiPhysXJointDesc*)
            m_kJoints.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kJoints.SetAt(ui, spJointClone);
    }
    
    // Clone the clothes
    pkDest->m_kClothes.SetSize(m_kClothes.GetSize());
    for (unsigned int ui = 0; ui < m_kClothes.GetSize(); ui++)
    {
        NiPhysXClothDescPtr spClothClone = (NiPhysXClothDesc*)
            m_kClothes.GetAt(ui)->CreateSharedClone(kCloning);
        pkDest->m_kClothes.SetAt(ui, spClothClone);
    }

    // Clone materials. Clones use the same index, which is what we want.
    // If two clones are merged in the same scene, their indexes will be
    // the same, and so they continue to have the same material.
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NxMaterialIndex usKey;
        NiPhysXMaterialDescPtr spMaterialDesc;
        m_kMaterials.GetNext(iter, usKey, spMaterialDesc);
        
        NiPhysXMaterialDesc* spMaterialClone = (NiPhysXMaterialDesc*)
            spMaterialDesc->CreateSharedClone(kCloning);
        pkDest->m_kMaterials.SetAt(usKey, spMaterialClone);
    }
    
    pkDest->m_uiNumStates = m_uiNumStates;
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::ProcessClone(NiCloningProcess& kCloning)
{
    NiObject::ProcessClone(kCloning);
    
    // Process the actors
    for (unsigned int ui = 0; ui < m_kActors.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXActorDesc* pkActor = m_kActors.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkActor, bJunk))
            pkActor->ProcessClone(kCloning);
    }

    // Process the joints
    for (unsigned int ui = 0; ui < m_kJoints.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXJointDesc* pkJoint = m_kJoints.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkJoint, bJunk))
            pkJoint->ProcessClone(kCloning);
    }
    
    // Process the clothes
    for (unsigned int ui = 0; ui < m_kClothes.GetSize(); ui++)
    {
        bool bJunk;
        NiPhysXClothDesc* pkCloth = m_kClothes.GetAt(ui);
        if (!kCloning.m_pkProcessMap->GetAt(pkCloth, bJunk))
            pkCloth->ProcessClone(kCloning);
    }

    // Process materials.
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        NxMaterialIndex usKey;
        NiPhysXMaterialDescPtr spMaterialDesc;
        m_kMaterials.GetNext(iter, usKey, spMaterialDesc);
        
        bool bJunk;
        if (!kCloning.m_pkProcessMap->GetAt(spMaterialDesc, bJunk))
            spMaterialDesc->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXPropDesc::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    for (unsigned int i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        spActor->RegisterStreamables(kStream);
    }
    for (unsigned int i = 0; i < m_kJoints.GetSize(); i++)
    {
        NiPhysXJointDescPtr spJoint = m_kJoints.GetAt(i);
        NIASSERT(spJoint != 0);
        spJoint->RegisterStreamables(kStream);
    }
    for (unsigned int i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        spCloth->RegisterStreamables(kStream);
    }
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        unsigned short usKey;
        NiPhysXMaterialDescPtr spMaterial;
        m_kMaterials.GetNext(iter, usKey, spMaterial);
        spMaterial->RegisterStreamables(kStream);
    }
     
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    
    NiStreamSaveBinary(kStream, m_kActors.GetEffectiveSize());
    for (unsigned int i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NIASSERT(spActor != 0);
        kStream.SaveLinkID(spActor);
    }
    NiStreamSaveBinary(kStream, m_kJoints.GetEffectiveSize());
    for (unsigned int i = 0; i < m_kJoints.GetSize(); i++)
    {
        NiPhysXJointDescPtr spJoint = m_kJoints.GetAt(i);
        NIASSERT(spJoint != 0);
        kStream.SaveLinkID(spJoint);
    }
    NiStreamSaveBinary(kStream, m_kClothes.GetEffectiveSize());
    for (unsigned int i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NIASSERT(spCloth != 0);
        kStream.SaveLinkID(spCloth);
    }
    NiStreamSaveBinary(kStream, m_kMaterials.GetCount());
    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        unsigned short usKey;
        NiPhysXMaterialDescPtr spMaterial;
        m_kMaterials.GetNext(iter, usKey, spMaterial);
        NiStreamSaveBinary(kStream, usKey);
        kStream.SaveLinkID(spMaterial);
    }

    NiStreamSaveBinary(kStream, m_uiNumStates);
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPropDesc);
//---------------------------------------------------------------------------
void NiPhysXPropDesc::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
    
    unsigned int uiSize;

    NiStreamLoadBinary(kStream, uiSize);
    m_kActors.SetSize(uiSize);
    for (unsigned int i = 0; i < uiSize; i++)
    {
        m_kActors.SetAt(i, (NiPhysXActorDesc*)kStream.ResolveLinkID());
    }

    NiStreamLoadBinary(kStream, uiSize);
    m_kJoints.SetSize(uiSize);
    for (unsigned int i = 0; i < uiSize; i++)
    {
        m_kJoints.SetAt(i, (NiPhysXJointDesc*)kStream.ResolveLinkID());
    }
    
    if (kStream.GetFileVersion() >= kStream.GetVersion(20, 3, 0, 5))
    {
        NiStreamLoadBinary(kStream, uiSize);
        m_kClothes.SetSize(uiSize);
        for (unsigned int i = 0; i < uiSize; i++)
        {
            m_kClothes.SetAt(i, (NiPhysXClothDesc*)kStream.ResolveLinkID());
        }
    }

    NiStreamLoadBinary(kStream, uiSize);
    for (unsigned int i = 0; i < uiSize; i++)
    {
        unsigned short usKey;
        NiStreamLoadBinary(kStream, usKey);
        NiPhysXMaterialDescPtr spMat =
            (NiPhysXMaterialDesc*)kStream.ResolveLinkID();
        m_kMaterials.SetAt(usKey, spMat);
    }
    
    NiStreamLoadBinary(kStream, m_uiNumStates);
}
//---------------------------------------------------------------------------
void NiPhysXPropDesc::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPropDesc::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXPropDesc, pkObject));
    if(!NiObject::IsEqual(pkObject))
        return false;
        
    NiPhysXPropDesc* pkNewObject = (NiPhysXPropDesc*)pkObject;

    for (unsigned int i = 0; i < m_kActors.GetSize(); i++)
    {
        NiPhysXActorDescPtr spActor = m_kActors.GetAt(i);
        NiPhysXActorDescPtr spNewActor = pkNewObject->m_kActors.GetAt(i);
        if (spActor)
        {
            if (!spNewActor)
                return false;
            if (!spActor->IsEqual(spNewActor))
                return false;
        }
        else
        {
            if (spNewActor)
                return false;
        }
    }

    for (unsigned int i = 0; i < m_kJoints.GetSize(); i++)
    {
        NiPhysXJointDescPtr spJoint = m_kJoints.GetAt(i);
        NiPhysXJointDescPtr spNewJoint = pkNewObject->m_kJoints.GetAt(i);
        if (spJoint)
        {
            if (!spNewJoint)
                return false;
            if (!spJoint->IsEqual(spNewJoint))
                return false;
        }
        else
        {
            if (spNewJoint)
                return false;
        }
    }

    for (unsigned int i = 0; i < m_kClothes.GetSize(); i++)
    {
        NiPhysXClothDescPtr spCloth = m_kClothes.GetAt(i);
        NiPhysXClothDescPtr spNewCloth = pkNewObject->m_kClothes.GetAt(i);
        if (spCloth)
        {
            if (!spNewCloth)
                return false;
            if (!spCloth->IsEqual(spNewCloth))
                return false;
        }
        else
        {
            if (spNewCloth)
                return false;
        }
    }

    NiTMapIterator iter = m_kMaterials.GetFirstPos();
    while (iter)
    {
        unsigned short usKey;
        NiPhysXMaterialDescPtr spMaterial;
        m_kMaterials.GetNext(iter, usKey, spMaterial);
        
        NiPhysXMaterialDescPtr spNewMat;
        if (!pkNewObject->m_kMaterials.GetAt(usKey, spNewMat))
            return false;
        if (!spMaterial->IsEqual(spNewMat))
            return false;
    }

    if (m_uiNumStates != pkNewObject->m_uiNumStates)
        return false;

    return true;
}
//---------------------------------------------------------------------------
