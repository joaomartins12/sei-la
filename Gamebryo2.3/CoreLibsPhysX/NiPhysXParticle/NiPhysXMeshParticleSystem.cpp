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

#include <NiPSysPositionModifier.h>

NiImplementRTTI(NiPhysXMeshParticleSystem, NiMeshParticleSystem);

//---------------------------------------------------------------------------
NiPhysXMeshParticleSystem::NiPhysXMeshParticleSystem(
    unsigned short usMaxNumParticles, bool bHasColors, bool bHasRotations,
    bool bWorldSpace, unsigned int uiNumGenerations,
    unsigned int uiMeshPoolSize, bool bFillMeshPoolsOnLoad,
    NiPhysXMeshPSysProp* pkProp, unsigned int uiCompartmentID,
    bool bPhysXSpace, bool bPhysXRotations, unsigned int uiActorPoolSize,
    bool bFillActorPoolsOnLoad, bool bKeepsMeshes)
    : NiMeshParticleSystem(NiNew NiPhysXMeshPSysData(usMaxNumParticles,
    bHasColors, bHasRotations, uiNumGenerations, uiMeshPoolSize,
    bFillMeshPoolsOnLoad, pkProp ? pkProp->GetScene() : 0, uiCompartmentID,
    uiActorPoolSize, bFillActorPoolsOnLoad,
    bKeepsMeshes), bWorldSpace)
{
    m_bPhysXRotations = bPhysXRotations;
    m_bPhysXSpace = bPhysXSpace;

    m_spProp = pkProp;
    if (pkProp)
    {
        // Set up sources and destinations to get data into and out of PhysX
        pkProp->AddSystem(this);
        m_spDestination = NiNew NiPhysXPSysDest(this);
        pkProp->AddDestination(m_spDestination);
        m_spSource = NiNew NiPhysXMeshPSysSrc(this);
        pkProp->AddSource(m_spSource);
    }
    else
    {
        m_spDestination = 0;
        m_spSource = 0;
    }
}
//---------------------------------------------------------------------------
NiPhysXMeshParticleSystem::NiPhysXMeshParticleSystem(
    NiMeshParticleSystem* pkPSys, NiPhysXMeshPSysProp* pkProp,
    unsigned int uiCompartmentID, bool bPhysXSpace, bool bPhysXRotations,
    unsigned int uiActorPoolSize, bool bFillActorPoolsOnLoad,
    bool bKeepsMeshes) : NiMeshParticleSystem()
{
    // Clone to copy all the modifiers, controllers, shaders, etc.
    NiCloningProcess kCloning;
    kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
    kCloning.m_cAppendChar = '%';

    pkPSys->CopyMembers(this, kCloning);
    pkPSys->ProcessClone(kCloning);
    
    // Cloning misses setting the target on the controllers attached to the
    // particle system object. Go through and set them now.
    NiTimeController* pkControllers = GetControllers();
    while (pkControllers)
    {
        pkControllers->SetTarget(this);
        pkControllers = pkControllers->GetNext();
    }
    
    // We need to get the number of meshes from the update modifier
    unsigned int uiNumGenerations = 0;
    
    // Remove the excess modifiers and add some more.
    NiPSysModifier* pkPosnModifier = 0;
    NiTListIterator kListIter = m_kModifierList.GetHeadPos();
    while (kListIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kListIter);
        if (NiIsKindOf(NiPSysPositionModifier, pkModifier))
        {
            pkPosnModifier = pkModifier;
        }
        else if (NiIsKindOf(NiPSysMeshUpdateModifier, pkModifier))
        {
            NiPSysMeshUpdateModifier* pkUpdateMod =
                (NiPSysMeshUpdateModifier*)pkModifier;
            uiNumGenerations = pkUpdateMod->GetMeshCount();
        }
    }
    if (pkPosnModifier)
    {
        RemoveModifier(pkPosnModifier);
    }
    AddModifier(NiNew NiPhysXPSysUpdateModifier("PhysXUpdate"));
    
    // Set the stuff the clone didn't catch
    m_bPhysXRotations = bPhysXRotations;
    m_bPhysXSpace = bPhysXSpace;

    // Replace the model data.
    NiPhysXScene* pkScene = 0;
    if (pkProp)
        pkScene = pkProp->GetScene();
    m_spModelData = NiNew NiPhysXMeshPSysData(
        NiDynamicCast(NiMeshPSysData, m_spModelData),
        pkScene, uiCompartmentID, uiNumGenerations, uiActorPoolSize,
        bFillActorPoolsOnLoad, bKeepsMeshes);

    m_spProp = pkProp;
    if (pkProp)
    {
        // Set up sources and destinations to get data into and out of PhysX
        pkProp->AddSystem(this);
        m_spDestination = NiNew NiPhysXPSysDest(this);
        pkProp->AddDestination(m_spDestination);
        m_spSource = NiNew NiPhysXMeshPSysSrc(this);
        pkProp->AddSource(m_spSource);
    }
    else
    {
        m_spDestination = 0;
        m_spSource = 0;
    }
}
//---------------------------------------------------------------------------
NiPhysXMeshParticleSystem::NiPhysXMeshParticleSystem()
{
    m_spProp = 0;
    m_spDestination = 0;
    m_spSource = 0;

    m_bPhysXRotations = false;
    m_bPhysXSpace = false;
}
//---------------------------------------------------------------------------
NiPhysXMeshParticleSystem::~NiPhysXMeshParticleSystem()
{
    DetachFromProp();
    m_spDestination = 0;
    m_spSource = 0;
}
//---------------------------------------------------------------------------
void NiPhysXMeshParticleSystem::SetModelData(NiGeometryData* pkModelData)
{
    // This function might be called due to cloning, in which case the
    // pkModelData is just a MeshPSysData object. If so, ignore it.
    if (!NiIsKindOf(NiPhysXMeshPSysData, pkModelData))
        return;
    
    // Remove sources and destinations from prop so they get removed
    // from old scene
    if (GetModelData())
    {
        (NiPhysXMeshPSysData*)GetModelData();

        if (m_spProp)
        {
            m_spProp->DeleteDestination(m_spDestination);
            m_spProp->DeleteSource(m_spSource);
        }
    }
    
    // The model data must be of type NiPhysXMeshPSysData.
    NiParticles::SetModelData(pkModelData);
    
    // Re-add sources and destinations so they will be in the right scene
    if (m_spProp)
    {
        NIASSERT(((NiPhysXMeshPSysData*)pkModelData)->GetScene() ==
            m_spProp->GetScene());

        m_spProp->AddDestination(m_spDestination);
        m_spProp->AddSource(m_spSource);
    }
}
//---------------------------------------------------------------------------
void NiPhysXMeshParticleSystem::AttachToProp(NiPhysXMeshPSysProp* pkProp)
{
    DetachFromProp();
    
    AttachToScene(pkProp->GetScene());
    
    m_spProp = pkProp;
    pkProp->AddSystem(this);
    m_spDestination = NiNew NiPhysXPSysDest(this);
    pkProp->AddDestination(m_spDestination);
    m_spSource = NiNew NiPhysXMeshPSysSrc(this);
    pkProp->AddSource(m_spSource);
}
//---------------------------------------------------------------------------
void NiPhysXMeshParticleSystem::DetachFromProp()
{
    if (!m_spProp)
        return;

    DetachFromScene();
    
    m_spProp->RemoveSystem(this);
    m_spProp->DeleteDestination(m_spDestination);
    m_spProp->DeleteSource(m_spSource);
    
    m_spProp = 0;
    m_spDestination = 0;
    m_spSource = 0;
}
//---------------------------------------------------------------------------
void NiPhysXMeshParticleSystem::AttachToScene(NiPhysXScene* pkScene)
{
    NiPhysXMeshPSysData* pkPhysXData = (NiPhysXMeshPSysData*)GetModelData();

    if (pkPhysXData->GetScene())
        DetachFromScene();
        
    pkPhysXData->SetScene(pkScene);
}
//---------------------------------------------------------------------------
void NiPhysXMeshParticleSystem::DetachFromScene(void)
{
    NiPhysXMeshPSysData* pkPhysXData = (NiPhysXMeshPSysData*)GetModelData();
    
    // Important to use a smart pointer here. We need the call to
    // pkPhysXData->SetScene(0); to NOT cause the PhysX scene to
    // be deleted before the pointer in the NiPhysXPSysData object
    // is setto 0. Otherwise we infinite loop through this function,
    // repeatedly trying to delete the scene.
    NiPhysXScenePtr spScene = pkPhysXData->GetScene();
    
    if (!spScene)
        return;
    
    pkPhysXData->SetScene(0);
    
    spScene = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXMeshParticleSystem);
//---------------------------------------------------------------------------
void NiPhysXMeshParticleSystem::CopyMembers(NiPhysXMeshParticleSystem* pkDest,
    NiCloningProcess& kCloning)
{
    NiMeshParticleSystem::CopyMembers(pkDest, kCloning);
    
    pkDest->m_spProp = 0;
    pkDest->m_spDestination = 0;
    pkDest->m_spSource = 0;
    pkDest->m_bPhysXRotations = m_bPhysXRotations;
    pkDest->m_bPhysXSpace = m_bPhysXSpace;
}
//---------------------------------------------------------------------------
void NiPhysXMeshParticleSystem::ProcessClone(NiCloningProcess& kCloning)
{
    NiMeshParticleSystem::ProcessClone(kCloning);
    
    NiObject* pkClone = 0;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkClone));
    
    NiPhysXMeshParticleSystem* pkDest = (NiPhysXMeshParticleSystem*)pkClone;
    NiPhysXMeshPSysData* pkPhysXData =
        (NiPhysXMeshPSysData*)pkDest->GetModelData();
    
    pkPhysXData->SetScene(0);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXMeshParticleSystem);
//---------------------------------------------------------------------------
bool NiPhysXMeshParticleSystem::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshParticleSystem::LoadBinary(NiStream& kStream)
{
    NiMeshParticleSystem::LoadBinary(kStream);

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 2))
    {
        m_spProp = (NiPhysXMeshPSysProp*)kStream.ResolveLinkID();
    }

    m_spDestination = (NiPhysXPSysDest*)kStream.ResolveLinkID();
    m_spSource = (NiPhysXMeshPSysSrc*)kStream.ResolveLinkID();

    NiBool bVal;
    NiStreamLoadBinary(kStream, bVal);
    m_bPhysXRotations = ( bVal != 0 );
    NiStreamLoadBinary(kStream, bVal);
    m_bPhysXSpace = ( bVal != 0 );
}
//---------------------------------------------------------------------------
void NiPhysXMeshParticleSystem::LinkObject(NiStream& kStream)
{
    NiMeshParticleSystem::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXMeshParticleSystem::PostLinkObject(NiStream& kStream)
{
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 2))
    {
        // Create a prop and add it to the scene, if it exists
        NiPhysXMeshPSysData* pkPhysXData =
            (NiPhysXMeshPSysData*)GetModelData();
        if (!pkPhysXData)
            return;

        NiPhysXScene* pkScene = pkPhysXData->GetScene();
        if (!pkScene)
            return;
            
        // Remove the sources and destinations from their current prop, or
        // from the scene. It depends on the order in which these post-link
        // functions get called.
        NiPhysXProp* pkRBProp = 0;
        for (unsigned int ui = 0; ui < pkScene->GetPropCount(); ui++)
        {
            NiPhysXProp* pkProp = pkScene->GetPropAt(ui);
            if (NiIsExactKindOf(NiPhysXProp, pkProp))
            {
                pkRBProp = pkProp;
                break;
            }
        }
        if (pkRBProp)
        {
            // Look for the regular prop
            pkRBProp->DeleteDestination(m_spDestination);
            pkRBProp->DeleteSource(m_spSource);
        }
        else
        {
            pkScene->DeleteDestination(m_spDestination);
            pkScene->DeleteSource(m_spSource);
        }
        
        m_spProp = 0;
        for (unsigned int ui = 0; ui < pkScene->GetPropCount(); ui++)
        {
            NiPhysXProp* pkProp = pkScene->GetPropAt(ui);
            if (NiIsKindOf(NiPhysXMeshPSysProp, pkProp))
            {
                m_spProp = (NiPhysXMeshPSysProp*)pkProp;
                break;
            }
        }
        if (!m_spProp)
        {
            m_spProp = NiNew NiPhysXMeshPSysProp();
            pkScene->AddProp(m_spProp);
        }
        m_spProp->AddSystem(this);
        m_spProp->AddDestination(m_spDestination);
        m_spProp->AddSource(m_spSource);
    }
}
//---------------------------------------------------------------------------
bool NiPhysXMeshParticleSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiMeshParticleSystem::RegisterStreamables(kStream))
    {
        return false;
    }
    
    if (m_spProp)
        m_spProp->RegisterStreamables(kStream);
    if (m_spDestination)
        m_spDestination->RegisterStreamables(kStream);
    if (m_spSource)
        m_spSource->RegisterStreamables(kStream);
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshParticleSystem::SaveBinary(NiStream& kStream)
{
    NiMeshParticleSystem::SaveBinary(kStream);

    kStream.SaveLinkID(m_spProp);
    kStream.SaveLinkID(m_spDestination);
    kStream.SaveLinkID(m_spSource);

    NiStreamSaveBinary(kStream, NiBool(m_bPhysXRotations));
    NiStreamSaveBinary(kStream, NiBool(m_bPhysXSpace));
}
//---------------------------------------------------------------------------
bool NiPhysXMeshParticleSystem::IsEqual(NiObject* pkObject)
{
    if (!NiMeshParticleSystem::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXMeshParticleSystem* pkNewObject =
        (NiPhysXMeshParticleSystem*)pkObject;
    
    if (m_bPhysXRotations != pkNewObject->m_bPhysXRotations ||
        m_bPhysXSpace != pkNewObject->m_bPhysXSpace)
        return false;

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXMeshParticleSystem::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiMeshParticleSystem::GetViewerStrings(pkStrings);
    
    pkStrings->Add(NiGetViewerString(
        NiPhysXMeshParticleSystem::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("PhysXRotations", m_bPhysXRotations));
    pkStrings->Add(NiGetViewerString("PhysXSpace", m_bPhysXSpace));
        
    if (m_spDestination)
    {
        m_spDestination->GetViewerStrings(pkStrings);
    }
    else
    {
        pkStrings->Add(NiGetViewerString("m_spDestination", 0));
    }
    if (m_spSource)
    {
        m_spSource->GetViewerStrings(pkStrings);
    }
    else
    {
        pkStrings->Add(NiGetViewerString("m_spSource", 0));
    }
}
//---------------------------------------------------------------------------
