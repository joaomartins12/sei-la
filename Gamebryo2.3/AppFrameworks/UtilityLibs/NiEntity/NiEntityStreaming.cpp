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
#include "NiEntityPCH.h"

#include "NiEntityStreaming.h"
#include "NiEntityPropertyInterface.h"

#define PT_ENTITYPOINTER    NiEntityPropertyInterface::PT_ENTITYPOINTER
#define PT_NIOBJECTPOINTER  NiEntityPropertyInterface::PT_NIOBJECTPOINTER

//---------------------------------------------------------------------------
// top level Scene list
//---------------------------------------------------------------------------
NiEntityStreaming::NiEntityStreaming()
{
    m_pkErrorHandler = NULL;
}
//---------------------------------------------------------------------------
NiEntityStreaming::~NiEntityStreaming()
{
    m_kTopScenes.RemoveAll();
}
//---------------------------------------------------------------------------
void NiEntityStreaming::InsertScene(NiScene* pkScene)
{
#ifdef _DEBUG
    NIASSERT(pkScene);

    for (unsigned int i = 0; i < m_kTopScenes.GetSize(); i++)
    {
        NIASSERT(pkScene != m_kTopScenes.GetAt(i));
    }
#endif

    m_kTopScenes.Add(pkScene);
}
//---------------------------------------------------------------------------
void NiEntityStreaming::RemoveScene(NiScene* pkScene)
{
    for (unsigned int i = 0; i < m_kTopScenes.GetSize(); i++)
    {
        if (pkScene == m_kTopScenes.GetAt(i))
        {
            m_kTopScenes.RemoveAt(i);
            return;
        }
    }
}
//---------------------------------------------------------------------------
void NiEntityStreaming::RemoveAllScenes()
{
    m_kTopScenes.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int NiEntityStreaming::GetSceneCount() const
{
    return m_kTopScenes.GetSize();
}
//---------------------------------------------------------------------------
unsigned int NiEntityStreaming::GetEffectiveSceneCount() const
{
    return m_kTopScenes.GetEffectiveSize();
}
//---------------------------------------------------------------------------
NiScene* NiEntityStreaming::GetSceneAt(unsigned int i) const
{
    return m_kTopScenes.GetAt(i);
}
//---------------------------------------------------------------------------
void NiEntityStreaming::ReportError(const NiFixedString& kErrorMessage,
    const NiFixedString& kErrorDescription, const NiFixedString& kEntityName,
    const NiFixedString& kPropertyName)
{
    if (m_pkErrorHandler == NULL)
        return;

    m_pkErrorHandler->ReportError(kErrorMessage, kErrorDescription,
        kEntityName, kPropertyName);
}
//---------------------------------------------------------------------------
void NiEntityStreaming::SetErrorHandler(NiEntityErrorInterface* pkErrorIntf)
{
    m_pkErrorHandler = pkErrorIntf;
}
//---------------------------------------------------------------------------
void NiEntityStreaming::MapToID(IDLinkMaps& kMaps, 
    NiEntityPropertyInterface* pkPropIntf)
{
    NIASSERT(pkPropIntf);
    kMaps.AssignID(pkPropIntf);
}
//---------------------------------------------------------------------------
void NiEntityStreaming::RecurseEntityPointerProperties(IDLinkMaps& kMaps,
    NiEntityPropertyInterface* pkPropIntf)
{
    NiTObjectSet<NiFixedString> kPropertyNames;
    pkPropIntf->GetPropertyNames(kPropertyNames);
    unsigned int uiNameCount = kPropertyNames.GetSize();
    for (unsigned int uiProp = 0; uiProp < uiNameCount; uiProp++)
    {
        const NiFixedString& kPropertyName = kPropertyNames.GetAt(uiProp);

        NiFixedString kPrimitiveType;
        if (pkPropIntf->GetPrimitiveType(kPropertyName, kPrimitiveType) 
            == false)
        {
            continue;
        }

        if (kPrimitiveType != PT_ENTITYPOINTER)
        {
            continue;
        }

        bool bIsSerializable;
        if (!pkPropIntf->IsPropertySerializable(kPropertyName,
            bIsSerializable) || !bIsSerializable)
        {
            continue;
        }

        unsigned int uiSubEntityCount;
        NIVERIFY(pkPropIntf->GetElementCount(kPropertyName,
            uiSubEntityCount));

        for(unsigned int uiEntity=0; uiEntity < uiSubEntityCount; uiEntity++)
        {
            NiEntityInterface* pkSubEntity = NULL;
            pkPropIntf->GetPropertyData(kPropertyName, pkSubEntity, uiEntity);

            if (pkSubEntity == NULL)
            {
                continue;
            }

            RecursiveMapToID(kMaps, pkSubEntity);
        }
    }
}
//---------------------------------------------------------------------------
void NiEntityStreaming::RecursiveMapToID(IDLinkMaps& kMaps,
    NiEntityComponentInterface* pkComp)
{
    // If we are already mapped... return out...
    unsigned int uiThisID;
    if (kMaps.GetID(pkComp, uiThisID))
        return;

    MapToID(kMaps, pkComp);   

    // First, check to see if there is a master component
    NiEntityComponentInterface* pkMasterComp = 
        pkComp->GetMasterComponent();
    if (pkMasterComp)
    {
        RecursiveMapToID(kMaps, pkMasterComp);
    }

    // Next, check to see if there are any property interface pointers
    RecurseEntityPointerProperties(kMaps, pkComp);
}
//---------------------------------------------------------------------------
void NiEntityStreaming::RecursiveMapToID(IDLinkMaps& kMaps,
    NiEntityInterface* pkEntity)
{
    // If we are already mapped... return out...
    unsigned int uiThisID;
    if (kMaps.GetID(pkEntity, uiThisID))
        return;

    MapToID(kMaps, pkEntity);

    // First, check to see if there is a master entity
    NiEntityInterface* pkMasterEntity = pkEntity->GetMasterEntity();
    if (pkMasterEntity)
    {
        RecursiveMapToID(kMaps, pkMasterEntity);
    }

    // Next, check to see if there are any components
    unsigned int uiComponentCount = pkEntity->GetComponentCount();
    for (unsigned int uiComp = 0; uiComp < uiComponentCount; uiComp++)
    {
        NiEntityComponentInterface* pkComp = pkEntity->GetComponentAt(uiComp);

        if (pkComp == NULL)
            continue;

        RecursiveMapToID(kMaps, pkComp);
    }

    // Third, check to see if there are any property interface pointers
    RecurseEntityPointerProperties(kMaps, pkEntity);

    return;
}
//---------------------------------------------------------------------------
void NiEntityStreaming::AssignUniqueLinkIDs(IDLinkMaps& kMaps)
{
    // For each Scene
    unsigned int uiSceneCount = GetSceneCount();
    for (unsigned int uiScene=0; uiScene < uiSceneCount; uiScene++)
    {
        NiScene* pkScene = GetSceneAt(uiScene);
        if (pkScene == NULL)
            continue;

        // First Map selection set entities.
        // In theory, selection sets would only include entities that exist
        // in the entity list. But if they are not, this code will handle
        // it.
        unsigned int uiSelectionSetCount = pkScene->GetSelectionSetCount();
        for (unsigned int uiSet = 0; uiSet < uiSelectionSetCount; uiSet++)
        {
            NiEntitySelectionSet* pkSet = pkScene->GetSelectionSetAt(uiSet);
            NIASSERT(pkSet);

            unsigned int uiEntityCount = pkSet->GetEntityCount();
        
            for(unsigned int uiEntity=0; uiEntity < uiEntityCount; uiEntity++)
            {
                NiEntityInterface* pkEntity = pkSet->GetEntityAt(uiEntity);
                NIASSERT(pkEntity);

                RecursiveMapToID(kMaps, pkEntity);           
            }
        }

        // MapToID and gather asset paths
        unsigned int uiEntityCount = pkScene->GetEntityCount();
        for (unsigned int uiEntity = 0; uiEntity < uiEntityCount; uiEntity++)
        {
            NiEntityInterface* pkEntity = pkScene->GetEntityAt(uiEntity);

            if (pkEntity == NULL)
                continue;
            
            RecursiveMapToID(kMaps, pkEntity);           
        }
    }
}
//---------------------------------------------------------------------------
void NiEntityStreaming::IDLinkMaps::RemoveLinks() 
{
    m_kIDMap.RemoveAll();
    m_kPropToIDMap.RemoveAll();
};
//---------------------------------------------------------------------------
NiBool NiEntityStreaming::IDLinkMaps::GetID(
    NiEntityPropertyInterface* pkPropIntf, unsigned int& uiID)
{
    return m_kPropToIDMap.GetAt(pkPropIntf, uiID);
}
//---------------------------------------------------------------------------
void NiEntityStreaming::IDLinkMaps::AssignID(
    NiEntityPropertyInterface* pkPropIntf, unsigned int uiID)
{
    m_kIDMap.SetAt(uiID, pkPropIntf);
    m_kPropToIDMap.SetAt(pkPropIntf, uiID);
}
//---------------------------------------------------------------------------
void NiEntityStreaming::IDLinkMaps::AssignID(
    NiEntityPropertyInterface* pkPropIntf)
{
    unsigned int uiID = m_kIDMap.GetCount();
    m_kIDMap.SetAt(uiID, pkPropIntf);
    m_kPropToIDMap.SetAt(pkPropIntf, uiID);
}
//---------------------------------------------------------------------------
NiEntityPropertyInterface* NiEntityStreaming::IDLinkMaps::GetAt(
    unsigned int uiID)
{
    NiEntityPropertyInterfaceIPtr spPropIntf;
    if (m_kIDMap.GetAt(uiID, spPropIntf))
    {
        return spPropIntf;
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
unsigned int NiEntityStreaming::IDLinkMaps::GetIDCount()
{
    return m_kIDMap.GetCount();
}
//---------------------------------------------------------------------------

