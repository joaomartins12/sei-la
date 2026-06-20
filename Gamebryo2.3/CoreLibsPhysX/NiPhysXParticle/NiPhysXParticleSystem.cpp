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

#include <NiPSysPositionModifier.h>

#include "NiPhysXParticle.h"

NiImplementRTTI(NiPhysXParticleSystem, NiParticleSystem);

//---------------------------------------------------------------------------
NiPhysXParticleSystem::NiPhysXParticleSystem(unsigned short usMaxNumParticles,
    bool bHasColors, bool bHasRotations, bool bWorldSpace,
    NiPhysXPSysProp* pkProp, unsigned int uiCompartmentID,
    bool bPhysXSpace, unsigned int uiActorPoolSize,
    bool bFillActorPoolsOnLoad, bool bKeepsMeshes) :
    NiParticleSystem(NiNew NiPhysXPSysData(usMaxNumParticles, bHasColors,
    bHasRotations, pkProp ? pkProp->GetScene() : 0, uiCompartmentID,
    uiActorPoolSize, bFillActorPoolsOnLoad,
    bKeepsMeshes), bWorldSpace)
{
    m_bPhysXSpace = bPhysXSpace;

    m_spProp = pkProp;
    if (pkProp)
    {
        // Set up sources and destinations to get data into and out of PhysX
        pkProp->AddSystem(this);
        m_spDestination = NiNew NiPhysXPSysDest(this);
        pkProp->AddDestination(m_spDestination);
        m_spSource = NiNew NiPhysXPSysSrc(this);
        pkProp->AddSource(m_spSource);
    }
    else
    {
        m_spDestination = 0;
        m_spSource = 0;
    }
}
//---------------------------------------------------------------------------
NiPhysXParticleSystem::NiPhysXParticleSystem(NiParticleSystem* pkPSys,
    NiPhysXPSysProp* pkProp, unsigned int uiCompartmentID, bool bPhysXSpace,
    unsigned int uiActorPoolSize, bool bFillActorPoolsOnLoad,
    bool bKeepsMeshes) : NiParticleSystem()
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
    }
    if (pkPosnModifier)
    {
        RemoveModifier(pkPosnModifier);
    }
    AddModifier(NiNew NiPhysXPSysUpdateModifier("PhysXUpdate"));
    
    // Set the stuff the clone didn't catch
    m_bPhysXSpace = bPhysXSpace;

    // Replace the model data.
    NiPhysXScene* pkScene = 0;
    if (pkProp)
        pkScene = pkProp->GetScene();
    m_spModelData = NiNew NiPhysXPSysData(
        NiDynamicCast(NiPSysData, m_spModelData), pkScene, uiCompartmentID,
        uiActorPoolSize, bFillActorPoolsOnLoad, bKeepsMeshes);

    m_spProp = pkProp;
    if (pkProp)
    {
        // Set up sources and destinations to get data into and out of PhysX
        pkProp->AddSystem(this);
        m_spDestination = NiNew NiPhysXPSysDest(this);
        pkProp->AddDestination(m_spDestination);
        m_spSource = NiNew NiPhysXPSysSrc(this);
        pkProp->AddSource(m_spSource);
    }
    else
    {
        m_spDestination = 0;
        m_spSource = 0;
    }
}
//---------------------------------------------------------------------------
NiPhysXParticleSystem::NiPhysXParticleSystem()
{
    m_spProp = 0;
    m_spDestination = 0;
    m_spSource = 0;
    m_bPhysXSpace = false;
}
//---------------------------------------------------------------------------
NiPhysXParticleSystem::~NiPhysXParticleSystem()
{
    DetachFromProp();
    m_spDestination = 0;
    m_spSource = 0;
}
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::SetModelData(NiGeometryData* pkModelData)
{
    // This function might be called due to cloning, in which case the
    // pkModelData is just a PSysData object. If so, ignore it.
    if (!NiIsKindOf(NiPhysXPSysData, pkModelData))
        return;
    
    // Remove sources and desatinations from prop so they get removed
    // from old scene
    if (GetModelData())
    {
        if (m_spProp)
        {
            m_spProp->DeleteDestination(m_spDestination);
            m_spProp->DeleteSource(m_spSource);
        }
    }
    
    // The model data must be of type NiPhysXPSysData.
    NiParticles::SetModelData(pkModelData);
    
    // Create sources and destinations.
    if (m_spProp)
    {
        NIASSERT(((NiPhysXPSysData*)pkModelData)->GetScene() ==
            m_spProp->GetScene());
        m_spProp->AddDestination(m_spDestination);
        m_spProp->AddSource(m_spSource);
    }
}
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::AttachToProp(NiPhysXPSysProp* pkProp)
{
    DetachFromProp();
    
    AttachToScene(pkProp->GetScene());

    m_spProp = pkProp;    
    pkProp->AddSystem(this);
    m_spDestination = NiNew NiPhysXPSysDest(this);
    pkProp->AddDestination(m_spDestination);
    m_spSource = NiNew NiPhysXPSysSrc(this);
    pkProp->AddSource(m_spSource);
}
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::DetachFromProp()
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
void NiPhysXParticleSystem::AttachToScene(NiPhysXScene* pkScene)
{
    NiPhysXPSysData* pkPhysXData = (NiPhysXPSysData*)GetModelData();

    if (pkPhysXData->GetScene())
        DetachFromScene();
        
    pkPhysXData->SetScene(pkScene);
}
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::DetachFromScene(void)
{
    NiPhysXPSysData* pkPhysXData = (NiPhysXPSysData*)GetModelData();
    
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
NiImplementCreateClone(NiPhysXParticleSystem);
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::CopyMembers(NiPhysXParticleSystem* pkDest,
    NiCloningProcess& kCloning)
{
    NiParticleSystem::CopyMembers(pkDest, kCloning);
    
    pkDest->m_spProp = 0;
    pkDest->m_spDestination = 0;
    pkDest->m_spSource = 0;
    pkDest->m_bPhysXSpace = m_bPhysXSpace;
}
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::ProcessClone(NiCloningProcess& kCloning)
{
    NiParticleSystem::ProcessClone(kCloning);
    
    NiObject* pkClone = 0;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkClone));
    
    NiPhysXParticleSystem* pkDest = (NiPhysXParticleSystem*)pkClone;
    NiPhysXPSysData* pkPhysXData = (NiPhysXPSysData*)pkDest->GetModelData();
    
    pkPhysXData->SetScene(0);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXParticleSystem);
//---------------------------------------------------------------------------
bool NiPhysXParticleSystem::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::LoadBinary(NiStream& kStream)
{
    NiParticleSystem::LoadBinary(kStream);

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 3, 0, 2))
    {
        m_spProp = (NiPhysXPSysProp*)kStream.ResolveLinkID();
    }

    m_spDestination = (NiPhysXPSysDest*)kStream.ResolveLinkID();
    m_spSource = (NiPhysXPSysSrc*)kStream.ResolveLinkID();

    NiBool bVal;
    NiStreamLoadBinary(kStream, bVal);
    m_bPhysXSpace = ( bVal != 0 );
}
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::LinkObject(NiStream& kStream)
{
    NiParticleSystem::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::PostLinkObject(NiStream& kStream)
{
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 3, 0, 2))
    {
        // Create a prop and add it to the scene, if it exists
        NiPhysXPSysData* pkPhysXData = (NiPhysXPSysData*)GetModelData();
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
            if (NiIsKindOf(NiPhysXPSysProp, pkProp))
            {
                m_spProp = (NiPhysXPSysProp*)pkProp;
                break;
            }
        }
        if (!m_spProp)
        {
            m_spProp = NiNew NiPhysXPSysProp();
            pkScene->AddProp(m_spProp);
        }
        m_spProp->AddSystem(this);
        m_spProp->AddDestination(m_spDestination);
        m_spProp->AddSource(m_spSource);
    }
}
//---------------------------------------------------------------------------
bool NiPhysXParticleSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiParticleSystem::RegisterStreamables(kStream))
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
void NiPhysXParticleSystem::SaveBinary(NiStream& kStream)
{
    NiParticleSystem::SaveBinary(kStream);

    kStream.SaveLinkID(m_spProp);
    kStream.SaveLinkID(m_spDestination);
    kStream.SaveLinkID(m_spSource);

    NiStreamSaveBinary(kStream, NiBool(m_bPhysXSpace));
}
//---------------------------------------------------------------------------
bool NiPhysXParticleSystem::IsEqual(NiObject* pkObject)
{
    if (!NiParticleSystem::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXParticleSystem* pkNewObject = (NiPhysXParticleSystem*)pkObject;
    
    if (m_bPhysXSpace != pkNewObject->m_bPhysXSpace)
        return false;

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXParticleSystem::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiParticleSystem::GetViewerStrings(pkStrings);
    
    pkStrings->Add(NiGetViewerString(
        NiPhysXParticleSystem::ms_RTTI.GetName()));

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
