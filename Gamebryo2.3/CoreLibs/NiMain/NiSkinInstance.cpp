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
#include "NiMainPCH.h"

#include "NiCloningProcess.h"
#include "NiSkinInstance.h"
#include "NiAVObject.h"

NiImplementRTTI(NiSkinInstance, NiObject);

#include "NiSystem.h"

//---------------------------------------------------------------------------
void NiSkinInstance::UpdateModelBound(NiBound& kBound)
{
    NiBound kWorldBound;

    NiAVObject* pBone = m_ppkBones[0];
    const NiSkinData::BoneData* pkBoneData = m_spSkinData->GetBoneData();
    kWorldBound.Update(pkBoneData[0].m_kBound, pBone->GetWorldTransform());
    unsigned int uiBones = m_spSkinData->GetBoneCount();

    for (unsigned int i = 1; i < uiBones; i++)
    {
        NiBound kBoneBound;
        pBone = m_ppkBones[i];

        if (pBone->GetSelectiveUpdate())
        {
            kBoneBound.Update(pkBoneData[i].m_kBound, 
                pBone->GetWorldTransform());
            kWorldBound.Merge(&kBoneBound);
        }
    }

    NiTransform kWorldToSkin, kWorldToRootParent;
    m_pkRootParent->GetWorldTransform().Invert(kWorldToRootParent);
    kWorldToSkin = m_spSkinData->GetRootParentToSkin() * kWorldToRootParent;
    kBound.Update(kWorldBound, kWorldToSkin);
}
//---------------------------------------------------------------------------
bool NiSkinInstance::ContainsVertexData(NiShaderDeclaration::ShaderParameter 
    eParameter) const
{
    switch (eParameter)
    {
    case NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES:
        return m_spSkinPartition != NULL && 
            m_spSkinPartition->GetPartitions() != NULL && 
            m_spSkinPartition->GetPartitions()[0].m_usBones > 4;
    case NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT:
        return m_spSkinPartition != NULL;
    default:
        if (m_spSkinPartition && m_spSkinPartition->GetPartitionCount() != 0)
        {
            NiGeometryData::RendererData* pkRendData = 
                m_spSkinPartition->GetPartitions()[0].m_pkBuffData;
            if (pkRendData)
                return pkRendData->ContainsVertexData(eParameter);
        }
        return false;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiObject* NiSkinInstance::CreateClone(
    NiCloningProcess& kCloning)
{
    NiSkinInstance* pkSkinInstance = NiNew NiSkinInstance;
    CopyMembers(pkSkinInstance, kCloning);

    NiObject* pkClone = NULL;
    bool bCloned = kCloning.m_pkCloneMap->GetAt(m_pkRootParent, pkClone);
    pkSkinInstance->m_pkRootParent = bCloned ? 
        (NiAVObject*) pkClone : (NiAVObject*) m_pkRootParent;
    
    unsigned int uiBoneCount = m_spSkinData->GetBoneCount();
    
    pkSkinInstance->m_ppkBones = NiAlloc(NiAVObject*, uiBoneCount);
    
    for (unsigned int i = 0; i < uiBoneCount; i++)
    {
        bCloned = kCloning.m_pkCloneMap->GetAt(m_ppkBones[i], pkClone);
        pkSkinInstance->m_ppkBones[i] = bCloned ?
            (NiAVObject*) pkClone : (NiAVObject*) m_ppkBones[i];
    }
    return pkSkinInstance;
}

//---------------------------------------------------------------------------
void NiSkinInstance::CopyMembers(NiSkinInstance* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);
    pkDest->m_spSkinData = m_spSkinData;
    pkDest->m_spSkinPartition = m_spSkinPartition;
}

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSkinInstance);
//---------------------------------------------------------------------------
void NiSkinInstance::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    kStream.ReadLinkID();   // m_spSkinData
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 101))
    {
        kStream.ReadLinkID(); // m_spSkinPartition;
    }
    kStream.ReadLinkID();   // m_spRootParent

    kStream.ReadMultipleLinkIDs();  // m_ppkBones
}

//---------------------------------------------------------------------------
void NiSkinInstance::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);

    m_spSkinData = (NiSkinData*) kStream.GetObjectFromLinkID();
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 101))
    {
        m_spSkinPartition = (NiSkinPartition*) kStream.GetObjectFromLinkID();
    }
    m_pkRootParent = (NiAVObject*) kStream.GetObjectFromLinkID();
    
    unsigned int uiBoneCount = kStream.GetNumberOfLinkIDs();
    m_ppkBones = NiAlloc(NiAVObject*,uiBoneCount);

    for (unsigned int i = 0; i < uiBoneCount; i++)
    {
        m_ppkBones[i] = (NiAVObject*) kStream.GetObjectFromLinkID();
    }
}

//---------------------------------------------------------------------------
void NiSkinInstance::PostLinkObject(NiStream& kStream)
{
    // m_spSkinData's skin partition may not be valid in LinkObject
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 101))
    {
        if (m_spSkinData)
        {
            // Move skin partition to NiSkinInstance from NiSkinData, in
            // older NIF files.
            m_spSkinPartition = m_spSkinData->GetSkinPartition(true);
            m_spSkinData->SetSkinPartition(NULL, true);
        }
    }
}
//---------------------------------------------------------------------------
bool NiSkinInstance::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    m_spSkinData->RegisterStreamables(kStream);
    if (m_spSkinPartition)
        m_spSkinPartition->RegisterStreamables(kStream);

    m_pkRootParent->RegisterStreamables(kStream);

    unsigned int i, uiBoneCount = m_spSkinData->GetBoneCount();

    for (i = 0; i < uiBoneCount; i++)
    {
        m_ppkBones[i]->RegisterStreamables(kStream);
    }

    return true;
}

//---------------------------------------------------------------------------
void NiSkinInstance::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    kStream.SaveLinkID(m_spSkinData);
    kStream.SaveLinkID(m_spSkinPartition);
    kStream.SaveLinkID(m_pkRootParent);

    unsigned int i, uiBoneCount = m_spSkinData->GetBoneCount();
    
    NiStreamSaveBinary(kStream, uiBoneCount);

    for (i = 0; i < uiBoneCount; i++)
    {
        kStream.SaveLinkID(m_ppkBones[i]);
    }
}

//---------------------------------------------------------------------------
bool NiSkinInstance::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiSkinInstance* pkSkinInstance = (NiSkinInstance*) pkObject;

    if (!m_spSkinData->IsEqual(pkSkinInstance->m_spSkinData))
        return false;
    
    if (m_spSkinPartition &&
        !m_spSkinPartition->IsEqual(pkSkinInstance->m_spSkinPartition))
    {
        return false;
    }
    else if (!m_spSkinPartition && pkSkinInstance->m_spSkinPartition)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
int CompareBoneNames(const void* pvName0, const void* pvName1)
{
    if (pvName0 == NULL)
        return 1;
    if (pvName1 == NULL)
        return -1;
    return strcmp(*((const char**) pvName0), *((const char**) pvName1));
}
//---------------------------------------------------------------------------
void NiSkinInstance::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(ms_RTTI.GetName()));

    NiAVObject* pkParent = GetRootParent(); 
    if (pkParent != NULL)
    {
        pkStrings->Add(NiGetViewerString("Root Parent", pkParent->GetName()));
    }

    NiSkinData* pkSkinData = GetSkinData(); 
    if (pkSkinData != NULL)
    {
        const NiSkinPartition* pkPartition = m_spSkinPartition;
        unsigned int uiPartitions = (pkPartition == NULL) ?
            0 : pkPartition->GetPartitionCount();

        pkStrings->Add(NiGetViewerString("Hardware partitions", uiPartitions));

        unsigned int uiBones = pkSkinData->GetBoneCount();
        pkStrings->Add(NiGetViewerString("Bone Count", uiBones));

        const char** ppcNames = NiAlloc(const char*,uiBones);
        unsigned int b;

        for (b = 0; b < uiBones; b++)
        {
            ppcNames[b] = m_ppkBones[b]->GetName();
        }

        qsort(ppcNames, uiBones, sizeof(ppcNames[0]), CompareBoneNames);

        for (b = 0; b < uiBones; b++)
        {
            char acBone[10];
            NiSprintf(acBone, 10, "  %3d", b);
            pkStrings->Add(NiGetViewerString(acBone,
                (ppcNames[b] == NULL) ? "<noname>" : ppcNames[b]));
        }

        NiFree(ppcNames);
    }
}
