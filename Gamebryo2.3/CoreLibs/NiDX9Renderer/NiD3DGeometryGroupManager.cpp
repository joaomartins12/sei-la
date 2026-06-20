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
#include "NiD3DRendererPCH.h"

#include "NiD3DGeometryGroupManager.h"
#include "NiGeometryGroup.h"
#include "NiStaticGeometryGroup.h"
#include "NiUnsharedGeometryGroup.h"
#include "NiDynamicGeometryGroup.h"
#include "NiGeometryBufferData.h"
#include "NiVBSet.h"

#include <NiTriBasedGeomData.h>
#include <NiScreenTexture.h>
#include <NiSkinInstance.h>

//---------------------------------------------------------------------------
NiD3DGeometryGroupManager::NiD3DGeometryGroupManager() :
    m_pkD3DDevice(NULL),
    m_pkVBManager(NULL),
    m_bHWSkinning(false)
{ /* */ }
//---------------------------------------------------------------------------
NiD3DGeometryGroupManager::~NiD3DGeometryGroupManager()
{
    unsigned int uiSize = m_kGroups.GetSize();

    for (unsigned int i = 0; i < uiSize; i++)
    {
        NiDelete m_kGroups.GetAt(i);
    }

    NiVBSet::DeleteBlocks();

    if (m_pkD3DDevice)
    {
        NiD3DRenderer::ReleaseDevice(m_pkD3DDevice);
    }
}
//---------------------------------------------------------------------------
NiD3DGeometryGroupManager* NiD3DGeometryGroupManager::Create(
    D3DDevicePtr pkD3DDevice, NiD3DVBManager* pkVBManager)
{
    NiD3DGeometryGroupManager* pkManager = NiNew NiD3DGeometryGroupManager;
    NIASSERT(pkManager);

    pkManager->m_pkD3DDevice = pkD3DDevice;
    D3D_POINTER_REFERENCE(pkManager->m_pkD3DDevice);
    pkManager->m_pkVBManager = pkVBManager;

    return pkManager;
}
//---------------------------------------------------------------------------
NiGeometryGroup* NiD3DGeometryGroupManager::CreateGroup(
    RepackingBehavior eBehavior)
{
    NiGeometryGroup* pkGroup = NULL;

    switch (eBehavior)
    {
    case STATIC:
        pkGroup = NiStaticGeometryGroup::Create();
        break;
    case UNSHARED:
        pkGroup = NiUnsharedGeometryGroup::Create();
        break;
    case DYNAMIC:
        pkGroup = NiDynamicGeometryGroup::Create();
        break;
    default:
        NIASSERT(false);
        break;
    }
    
    if (pkGroup && m_kGroups.Find(pkGroup) < 0)
    {
        m_kGroups.Add(pkGroup);

        NIASSERT(m_pkD3DDevice);
        pkGroup->m_pkD3DDevice = m_pkD3DDevice;
        D3D_POINTER_REFERENCE(pkGroup->m_pkD3DDevice);
    }

    return pkGroup;
}
//---------------------------------------------------------------------------
bool NiD3DGeometryGroupManager::DestroyGroup(NiGeometryGroup* pkGroup)
{
    if (!pkGroup || !IsGroupEmpty(pkGroup))
        return false;

    unsigned int uiIndex = m_kGroups.Find(pkGroup);
    m_kGroups.RemoveAt(uiIndex);

    NiDelete pkGroup;

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DGeometryGroupManager::IsGroupEmpty(NiGeometryGroup* pkGroup) const
{
    return pkGroup->IsEmpty();
}
//---------------------------------------------------------------------------
bool NiD3DGeometryGroupManager::AddObjectToGroup(NiGeometryGroup* pkGroup, 
    NiGeometryData* pkData, NiSkinInstance* pkSkinInstance)
{
    return AddObjectToGroup(pkGroup, pkData, pkSkinInstance,
        m_bHWSkinning, 0, 0);
}
//---------------------------------------------------------------------------
bool NiD3DGeometryGroupManager::AddObjectToGroup(
    NiGeometryGroup* pkGroup, 
    NiGeometryData* pkData, 
    NiSkinInstance* pkSkinInstance, 
    bool bHWSkinning, 
    unsigned int, 
    unsigned int)
{
    // Check for skinned object!
    if (pkSkinInstance && bHWSkinning)
    {
        // Check for partitions, but don't generate them if they are not there.
        NiSkinPartition* pkSkinPartition = pkSkinInstance->GetSkinPartition();
        if (pkSkinPartition)
        {
            // Check to see if object is already in a group
            if (pkSkinPartition->GetPartitions()->m_pkBuffData)
                return false;

            NiSkinPartition::Partition* pkPartitionArray = 
                pkSkinPartition->GetPartitions();

            unsigned int uiNumPartitions = 
                pkSkinPartition->GetPartitionCount();

            // Add each of Partition to group and pack
            for (unsigned int i = 0; i < uiNumPartitions; i++)
            {
                pkGroup->AddObject(pkData, pkSkinInstance, 
                    &pkPartitionArray[i]);
            }

            return true;
        }
    }

    // Check to see if object is already in a group
    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkData->GetRendererData();
    if (pkBuffData)
        return false;

    pkGroup->AddObject(pkData, pkSkinInstance, NULL);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DGeometryGroupManager::RemoveObjectFromGroup(NiGeometryData* pkData, 
    NiSkinInstance* pkSkinInstance)
{
    NiSkinPartition* pkSkinPartition = NULL;
    if (pkSkinInstance)
        pkSkinPartition = pkSkinInstance->GetSkinPartition();

    if (pkSkinPartition == NULL)
    {
        NiGeometryBufferData* pkBuffData = 
            (NiGeometryBufferData*)pkData->GetRendererData();
        if (!pkBuffData)
            return false;

        NiGeometryGroup* pkGroup = pkBuffData->GetGeometryGroup();
        NIASSERT(pkGroup);
        pkGroup->RemoveObject(pkData);

        if (pkGroup->IsEmpty())
            pkGroup->Purge();

        return true;
    }
    else
    {
        return RemoveSkinPartitionFromGroup(pkSkinPartition);
    }
}
//---------------------------------------------------------------------------
bool NiD3DGeometryGroupManager::AddObjectToGroup(
    NiGeometryGroup* pkGroup, 
    NiScreenTexture* pkScreenTexture) 
{
    // Check to see if object is already in a group
    NiGeometryBufferData* pkBuffData = (NiGeometryBufferData*)
        (NiGeometryBufferData*)pkScreenTexture->GetRendererData();
    if (pkBuffData)
        return false;

    pkGroup->AddObject(pkScreenTexture);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DGeometryGroupManager::RemoveObjectFromGroup(
    NiScreenTexture* pkScreenTexture) 
{
    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkScreenTexture->GetRendererData();
    if (!pkBuffData)
        return false;

    NiGeometryGroup* pkGroup = pkBuffData->GetGeometryGroup();
    NIASSERT(pkGroup);
    pkGroup->RemoveObject(pkScreenTexture);

    if (pkGroup->IsEmpty())
        pkGroup->Purge();

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DGeometryGroupManager::RemoveSkinPartitionFromGroup(
    NiSkinPartition* pkSkinPartition)
{
    unsigned int uiPartitionCount = pkSkinPartition->GetPartitionCount();
    NiSkinPartition::Partition* pkPartitions = 
        pkSkinPartition->GetPartitions();

    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkPartitions->m_pkBuffData;
    NiGeometryGroup* pkGroup = pkBuffData->GetGeometryGroup();
    NIASSERT(pkGroup);

    bool bReturn = false;

    for (unsigned int ui = 0; ui < uiPartitionCount; ui++)
    {
        NiSkinPartition::Partition& kPartition = pkPartitions[ui];
        NiGeometryBufferData* pkBuffData = 
            (NiGeometryBufferData*)kPartition.m_pkBuffData;
        if (pkBuffData)
        {
            NiGeometryGroup* pkPartitionGroup = 
                pkBuffData->GetGeometryGroup();
            NIASSERT(pkPartitionGroup);
            pkPartitionGroup->RemoveObject(&kPartition);
            bReturn = true;
        }
    }

    if (bReturn && pkGroup->IsEmpty())
        pkGroup->Purge();

    return bReturn;
}
//---------------------------------------------------------------------------
void NiD3DGeometryGroupManager::SetHWSkinning(bool bHWSkinning)
{
    m_bHWSkinning = bHWSkinning;
}
//---------------------------------------------------------------------------
