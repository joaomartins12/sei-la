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

#include "NiMeshParticleSystem.h"
#include "NiMeshPSysData.h"
#include "NiPSysMeshEmitter.h"
#include "NiPSysMeshUpdateModifier.h"
#include "NiPSysUpdateCtlr.h"

NiImplementRTTI(NiMeshParticleSystem, NiParticleSystem);

//---------------------------------------------------------------------------
NiMeshParticleSystem::NiMeshParticleSystem(unsigned short usMaxNumParticles,
    bool bHasColors, bool bHasRotations, bool bWorldSpace,
    unsigned int uiPoolSize, unsigned int uiNumGenerations,
    bool bFillPoolsOnLoad) : NiParticleSystem(NiNew NiMeshPSysData(
    usMaxNumParticles, bHasColors, bHasRotations, uiPoolSize,
    uiNumGenerations, bFillPoolsOnLoad), bWorldSpace),
    m_fTime(0.0f), m_bUpdateControllers(true)
{
}
//---------------------------------------------------------------------------
NiMeshParticleSystem::NiMeshParticleSystem(NiMeshPSysData* pkData,
    bool bWorldSpace) : NiParticleSystem(pkData, bWorldSpace), m_fTime(0.0f),
    m_bUpdateControllers(true)
{
}
//---------------------------------------------------------------------------
NiMeshParticleSystem::NiMeshParticleSystem() : m_fTime(0.0f),
    m_bUpdateControllers(true)
{
}
//---------------------------------------------------------------------------
void NiMeshParticleSystem::SetModelData(NiGeometryData* pkModelData)
{
    // The model data must be of type NiMeshPSysData.
    NIASSERT(NiIsKindOf(NiMeshPSysData, pkModelData));
    NiParticleSystem::SetModelData(pkModelData);
}
//---------------------------------------------------------------------------
void NiMeshParticleSystem::UpdateDownwardPass(float fTime,
    bool bUpdateControllers)
{
    NiPSysUpdateCtlr* pkUpdateController = NULL;

    if (bUpdateControllers)
    {
        // Update the Proprety Controllers
        NiTListIterator kPos = m_kPropertyList.GetHeadPos();
        while (kPos)
        {
            NiProperty* pkProperty = m_kPropertyList.GetNext(kPos);
            if (pkProperty && pkProperty->GetControllers())
                pkProperty->Update(fTime);
        }

        // Update all of the controllers except the update controller
        NiTimeController* pkControl = GetControllers();
        for (/**/; pkControl; pkControl = pkControl->GetNext())
        {
            if (NiIsKindOf(NiPSysUpdateCtlr, pkControl))
                pkUpdateController = (NiPSysUpdateCtlr*)pkControl;
            else
                pkControl->Update(fTime);
        }


        // Only Update the World Location of the Particle System. It is going
        // to be used by the modifiers etc
        UpdateWorldData();

        // Call the Update Controller for the Particle System
        if (pkUpdateController)
        {
            pkUpdateController->Update(fTime);
        }
    }
    else
    {
        // Only Update the World Location of the Particle System. It is going
        // to be used by the modifiers etc
        UpdateWorldData();
    }

    // Update the World Bounds
    UpdateWorldBound();

    m_fTime = fTime;
    m_bUpdateControllers = bUpdateControllers;
}
//---------------------------------------------------------------------------
void NiMeshParticleSystem::UpdateSelectedDownwardPass(float fTime)
{
    NiParticleSystem::UpdateSelectedDownwardPass(fTime);

    m_fTime = fTime;
}
//---------------------------------------------------------------------------
void NiMeshParticleSystem::UpdateRigidDownwardPass(float fTime)
{
    NiParticleSystem::UpdateRigidDownwardPass(fTime);

    m_fTime = fTime;
}
//---------------------------------------------------------------------------
void NiMeshParticleSystem::UpdatePropertiesDownward(
    NiPropertyState* pkParentState)
{
    NiParticleSystem::UpdatePropertiesDownward(pkParentState);

    NIASSERT(NiIsKindOf(NiMeshPSysData, m_spModelData));
    NiMeshPSysData* pkData = NiSmartPointerCast(NiMeshPSysData,
        m_spModelData);

    pkData->GetContainerNode()->UpdatePropertiesDownward(
        m_spPropertyState);
}
//---------------------------------------------------------------------------
void NiMeshParticleSystem::UpdateEffectsDownward(
    NiDynamicEffectState* pkParentState)
{
    NiParticleSystem::UpdateEffectsDownward(pkParentState);

    NIASSERT(NiIsKindOf(NiMeshPSysData, m_spModelData));
    NiMeshPSysData* pkData = NiSmartPointerCast(NiMeshPSysData,
        m_spModelData);

    pkData->GetContainerNode()->UpdateEffectsDownward(m_spEffectState);
}
//---------------------------------------------------------------------------
void NiMeshParticleSystem::OnVisible(NiCullingProcess& kCuller)
{
    NIASSERT(NiIsKindOf(NiMeshPSysData, m_spModelData));
    NiMeshPSysData* pkData = NiSmartPointerCast(NiMeshPSysData,
        m_spModelData);

    pkData->GetContainerNode()->OnVisible(kCuller);
}
//---------------------------------------------------------------------------
void NiMeshParticleSystem::SetSelectiveUpdateFlags(bool& bSelectiveUpdate, 
    bool bSelectiveUpdateTransforms, bool& bRigid)
{
    NiParticleSystem::SetSelectiveUpdateFlags(bSelectiveUpdate, true,
        bRigid);

    NIASSERT(NiIsKindOf(NiMeshPSysData, m_spModelData));
    NiMeshPSysData* pkData = NiSmartPointerCast(NiMeshPSysData,
        m_spModelData);

    pkData->GetContainerNode()->SetSelectiveUpdateFlags(bSelectiveUpdate,
        true, bRigid);

    bRigid = false;
    pkData->GetContainerNode()->SetSelectiveUpdateRigid(bRigid);
}
//---------------------------------------------------------------------------
void NiMeshParticleSystem::UpdateWorldData()
{
    NiParticleSystem::UpdateWorldData();

    NIASSERT(NiIsKindOf(NiMeshPSysData, m_spModelData));
    NiMeshPSysData* pkData = NiSmartPointerCast(NiMeshPSysData,
        m_spModelData);

    pkData->GetContainerNode()->SetRotate(GetWorldRotate());
    pkData->GetContainerNode()->SetTranslate(GetWorldTranslate());
    pkData->GetContainerNode()->SetScale(GetWorldScale());
    pkData->GetContainerNode()->UpdateWorldData();
}
//---------------------------------------------------------------------------
void NiMeshParticleSystem::UpdateWorldBound()
{
    NiParticleSystem::UpdateWorldBound();

    NIASSERT(NiIsKindOf(NiMeshPSysData, m_spModelData));
    NiMeshPSysData* pkData = NiSmartPointerCast(NiMeshPSysData,
        m_spModelData);

    m_kWorldBound = pkData->GetContainerNode()->GetWorldBound();
}
//---------------------------------------------------------------------------
void NiMeshParticleSystem::PurgeRendererData(NiRenderer* pkRenderer)
{
    NiParticleSystem::PurgeRendererData(pkRenderer);

    NIASSERT(NiIsKindOf(NiMeshPSysData, m_spModelData));
    NiMeshPSysData* pkData = NiSmartPointerCast(NiMeshPSysData,
        m_spModelData);

    if (pkData && pkData->GetContainerNode())
        pkData->GetContainerNode()->PurgeRendererData(pkRenderer);
    
    if (pkData)
        pkData->PurgeRendererDataForPools(pkRenderer);

    // Purge any meshes on mesh-related modifiers
    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    while (kIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kIter);

        // This should be in a "virtual NiPSysModifier::PurgeRendererData"
        if (NiIsKindOf(NiPSysMeshEmitter, pkModifier))
        {
            NiPSysMeshEmitter* pkPSME = (NiPSysMeshEmitter*)pkModifier;
                NiGeometry* GetGeometryEmitter(unsigned int uiWhichEmitter);
            unsigned int uiNumGeoms = pkPSME->GetGeometryEmitterCount();
            for (unsigned int i = 0; i < uiNumGeoms; i++)
            {
                NiGeometry* pkGeom = pkPSME->GetGeometryEmitter(i);
                if (pkGeom)
                    pkGeom->PurgeRendererData(pkRenderer);
            }
        }
        else if (NiIsKindOf(NiPSysMeshUpdateModifier, pkModifier))
        {
            NiPSysMeshUpdateModifier* pkPSMUM = 
                (NiPSysMeshUpdateModifier*)pkModifier;
            unsigned int uiNumGeoms = pkPSMUM->GetMeshCount();
            for (unsigned int i = 0; i < uiNumGeoms; i++)
            {
                NiAVObject* pkMesh = pkPSMUM->GetMeshAt(i);
                if (pkMesh)
                    pkMesh->PurgeRendererData(pkRenderer);
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiMeshParticleSystem);
//---------------------------------------------------------------------------
void NiMeshParticleSystem::CopyMembers(NiMeshParticleSystem* pkDest,
    NiCloningProcess& kCloning)
{
    NiParticleSystem::CopyMembers(pkDest, kCloning);

    pkDest->m_fTime = m_fTime;
    pkDest->m_bUpdateControllers = m_bUpdateControllers;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiMeshParticleSystem);
//---------------------------------------------------------------------------
void NiMeshParticleSystem::LoadBinary(NiStream& kStream)
{
    NiParticleSystem::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiMeshParticleSystem::LinkObject(NiStream& kStream)
{
    NiParticleSystem::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiMeshParticleSystem::RegisterStreamables(NiStream& kStream)
{
    return NiParticleSystem::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiMeshParticleSystem::SaveBinary(NiStream& kStream)
{
    NiParticleSystem::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiMeshParticleSystem::IsEqual(NiObject* pkObject)
{
    return NiParticleSystem::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiMeshParticleSystem::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiParticleSystem::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiMeshParticleSystem::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
