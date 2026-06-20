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

#include "NiParticleMetrics.h"
#include <NiCloningProcess.h>
#include <NiNode.h>
#include <NiVersion.h>

#include "NiPSysModifierCtlr.h"
#include "NiPSysUpdateTask.h"
#include "NiParallelUpdateTaskManager.h"
#include "NiParticleSystem.h"


NiImplementRTTI(NiParticleSystem, NiParticles);

bool NiParticleSystem::ms_bParallelUpdateEnabled = false;
NiTaskManager::TaskPriority 
    NiParticleSystem::ms_eParallelUpdatePriority = NiTaskManager::MEDIUM;

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED =
    "Copyright 2007 Emergent Game Technologies";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED =
    GAMEBRYO_MODULE_VERSION_STRING(NiParticle);
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
NiParticleSystem::NiParticleSystem(unsigned short usMaxNumParticles,
    bool bHasColors, bool bHasRotations, bool bWorldSpace) :
    NiParticles(NiNew NiPSysData(usMaxNumParticles, bHasColors,
    bHasRotations)), m_bWorldSpace(bWorldSpace),
    m_fLastTime(-NI_INFINITY), m_bResetSystem(false), m_bDynamicBounds(false)
{
}
//---------------------------------------------------------------------------
NiParticleSystem::NiParticleSystem(NiPSysData* pkData,
    bool bWorldSpace) : NiParticles(pkData), m_bWorldSpace(bWorldSpace),
    m_fLastTime(-NI_INFINITY), m_bResetSystem(false), m_bDynamicBounds(false)
{
}
//---------------------------------------------------------------------------
NiParticleSystem::NiParticleSystem() : m_bWorldSpace(true),
    m_fLastTime(-NI_INFINITY), m_bResetSystem(false), m_bDynamicBounds(false)
{
}
//---------------------------------------------------------------------------
void NiParticleSystem::SetModelData(NiGeometryData* pkModelData)
{
    // The model data must be of type NiPSysData.
    NIASSERT(NiIsKindOf(NiPSysData, pkModelData));
    NiParticles::SetModelData(pkModelData);
}
//---------------------------------------------------------------------------
void NiParticleSystem::UpdateSystem(float fTime)
{
    // Only update in the background if we have a bound from a previous frame
    // and the update manager is active. This allows the bounds of the
    // particles to start in a valid state for the system.
    if (GetParallelUpdateEnabled() &&
        NiParallelUpdateTaskManager::IsActive() &&
        m_spModelData->GetBound().GetRadius() > 0.0f)
    {

        NiPSysUpdateTask* pkTask = NiPSysUpdateTask::GetFreeObject();
        if (pkTask)
        {
            pkTask->Init(this, fTime);
            if (NiParallelUpdateTaskManager::Get()->AddTask(pkTask,
                ms_eParallelUpdatePriority))
            {
                return;
            }

            // Failed to add task, clean up task and fall through
            pkTask->Clear();
        }
    }

    Do_UpdateSystem(fTime);
}
//---------------------------------------------------------------------------
void NiParticleSystem::Do_UpdateSystem(float fTime)
{
    NIMETRICS_PARTICLE_SCOPETIMER(UPDATE_PSYS_TIME);

    // Get particle data.
    NiPSysData* pkData = NiSmartPointerCast(NiPSysData, m_spModelData);

    // Actually add any particles that were created by modifiers last frame.
    pkData->ResolveAddedParticles();

    // Reset particle system if the last time is greater than the current
    // time.
    if (m_bResetSystem || m_fLastTime > fTime)
    {
        PerformSystemReset();
        m_bResetSystem = false;
    }

    // Initialize the last time.
    if (m_fLastTime == -NI_INFINITY)
    {
        m_fLastTime = fTime;
    }

    // Update modifiers.
    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    while (kIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kIter);
        pkModifier->UpdateModifier(fTime, pkData);

        NIMETRICS_PARTICLE_ADDVALUE(UPDATED_MODIFIERS, 
            pkData->GetActiveVertexCount());
    }

    // Mark geometry as changed.
    pkData->MarkAsChanged(NiGeometryData::VERTEX_MASK |
        NiGeometryData::NORMAL_MASK | NiGeometryData::COLOR_MASK);

    m_fLastTime = fTime;

    NIMETRICS_PARTICLE_ADDVALUE(UPDATED_PARTICLES, 
        pkData->GetActiveVertexCount());
}
//---------------------------------------------------------------------------
void NiParticleSystem::PerformSystemReset()
{
    // Remove all particles in system and reset the last time.
    NiPSysData* pkData = NiSmartPointerCast(NiPSysData, m_spModelData);
    pkData->ClearAllParticles();
    m_fLastTime = -NI_INFINITY;

    // Notify modifiers of reset.
    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    while (kIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kIter);
        pkModifier->HandleReset();
    }

    // Notify modifier controllers of reset.
    NiTimeController* pkCtlr = m_spControllers;
    while (pkCtlr)
    {
        NiPSysModifierCtlr* pkModifierCtlr = NiDynamicCast(NiPSysModifierCtlr,
            pkCtlr);
        if (pkModifierCtlr)
        {
            pkModifierCtlr->HandleReset();
        }

        pkCtlr = pkCtlr->GetNext();
    }
}
//---------------------------------------------------------------------------
void NiParticleSystem::AddModifier(NiPSysModifier* pkModifier)
{
    NIASSERT(pkModifier && pkModifier->GetName());

    // Insert the modifier into the list in the proper order.
    bool bInserted = false;
    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    if (!kIter)
    {
        m_kModifierList.AddHead(pkModifier);
        bInserted = true;
    }
    while (kIter)
    {
        NiPSysModifier* pkCurModifier = m_kModifierList.Get(kIter);
        if (pkCurModifier->GetOrder() > pkModifier->GetOrder())
        {
            m_kModifierList.InsertBefore(kIter, pkModifier);
            bInserted = true;
            break;
        }
        kIter = m_kModifierList.GetNextPos(kIter);
    }
    if (!bInserted)
    {
        m_kModifierList.AddTail(pkModifier);
    }

    // Set target of modifier.
    pkModifier->SetSystemPointer(this);
}
//---------------------------------------------------------------------------
void NiParticleSystem::InitializeNewParticle(unsigned short usNewParticle)
{
    // Get the data object.
    NIASSERT(NiIsKindOf(NiPSysData, m_spModelData));
    NiPSysData* pkData = NiSmartPointerCast(NiPSysData, m_spModelData);

    // Initialize new particle with modifiers.
    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    while (kIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kIter);
        pkModifier->Initialize(pkData, usNewParticle);
    }
}
//---------------------------------------------------------------------------
void NiParticleSystem::UpdateDownwardPass(float fTime,
    bool bUpdateControllers)
{
    UpdateWorldData();

    NiParticles::UpdateDownwardPass(fTime, bUpdateControllers);
}
//---------------------------------------------------------------------------
void NiParticleSystem::UpdateSelectedDownwardPass(float fTime)
{
    if (GetSelectiveUpdateTransforms())
    {
        UpdateWorldData();
    }

    NiParticles::UpdateSelectedDownwardPass(fTime);
}
//---------------------------------------------------------------------------
void NiParticleSystem::UpdateRigidDownwardPass(float fTime)
{
    if (GetSelectiveUpdateTransforms())
    {
        UpdateWorldData();
    }

    NiParticles::UpdateRigidDownwardPass(fTime);
}
//---------------------------------------------------------------------------
void NiParticleSystem::UpdateWorldData()
{
    NiParticles::UpdateWorldData();

    m_kUnmodifiedWorld = m_kWorld;

    if (m_bWorldSpace)
    {
        m_kWorld.m_Translate = NiPoint3::ZERO;
        m_kWorld.m_Rotate = NiMatrix3::IDENTITY;
    }
}
//---------------------------------------------------------------------------
void NiParticleSystem::UpdateWorldBound()
{
    NiParticles::UpdateWorldBound();

    if (m_bWorldSpace)
    {
        if (m_bDynamicBounds)
        {
            m_kWorldBound = m_spModelData->GetBound();
            m_kWorldBound.SetCenterAndRadius(m_kWorld.m_fScale *
                m_kWorldBound.GetCenter(), m_kWorld.m_fScale *
                m_kWorldBound.GetRadius());
        }
        else
        {
            m_kWorldBound.Update(m_spModelData->GetBound(),
                m_kUnmodifiedWorld);
        }
    }
}
//---------------------------------------------------------------------------
void NiParticleSystem::SetSelectiveUpdateFlags(bool& bSelectiveUpdate, 
    bool bSelectiveUpdateTransforms, bool& bRigid)
{
    bSelectiveUpdate = true;
    bSelectiveUpdateTransforms = true;
    bRigid = false;
    SetSelectiveUpdate(bSelectiveUpdate);
    SetSelectiveUpdateTransforms(bSelectiveUpdateTransforms);
    SetSelectiveUpdatePropertyControllers(true);
    SetSelectiveUpdateRigid(bRigid);
}
//---------------------------------------------------------------------------
void NiParticleSystem::FindMinBeginKeyTimeRecursive(NiAVObject* pkObject,
    float& fMinTime)
{
    if (NiIsKindOf(NiParticleSystem, pkObject))
    {
        NiTimeController* pkCtlr = pkObject->GetControllers();
        while(pkCtlr)
        {
            if (NiIsKindOf(NiPSysModifierCtlr, pkCtlr) &&
                pkCtlr->GetBeginKeyTime() < fMinTime)
            {
                fMinTime = pkCtlr->GetBeginKeyTime();
            }

            pkCtlr = pkCtlr->GetNext();
        }
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*) pkObject;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                FindMinBeginKeyTimeRecursive(pkChild, fMinTime);
            }
        }
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiParticleSystem);
//---------------------------------------------------------------------------
void NiParticleSystem::CopyMembers(NiParticleSystem* pkDest,
    NiCloningProcess& kCloning)
{
    NiParticles::CopyMembers(pkDest, kCloning);

    pkDest->m_bWorldSpace = m_bWorldSpace;

    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    while (kIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kIter);
        pkDest->m_kModifierList.AddTail((NiPSysModifier*)
            pkModifier->CreateClone(kCloning));
    }

    pkDest->m_fLastTime = m_fLastTime;

    pkDest->m_bDynamicBounds = m_bDynamicBounds;

    pkDest->m_spModelData = (NiPSysData*) m_spModelData->CreateClone(
        kCloning);
}
//---------------------------------------------------------------------------
void NiParticleSystem::ProcessClone(NiCloningProcess& kCloning)
{
    NiParticles::ProcessClone(kCloning);

    NiObject* pkClone = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkClone));
    //NiParticleSystem* pkDest = (NiParticleSystem*) pkClone;

    NiTListIterator kListIter = m_kModifierList.GetHeadPos();
    while (kListIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kListIter);
        pkModifier->ProcessClone(kCloning);
    }

    m_spModelData->ProcessClone(kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiParticleSystem);
//---------------------------------------------------------------------------
void NiParticleSystem::LoadBinary(NiStream& kStream)
{
    NiParticles::LoadBinary(kStream);

    NiBool bWorldSpace;
    NiStreamLoadBinary(kStream, bWorldSpace);
    m_bWorldSpace = (bWorldSpace != 0);

    kStream.ReadMultipleLinkIDs();  // m_kModifierList
}
//---------------------------------------------------------------------------
void NiParticleSystem::LinkObject(NiStream& kStream)
{
    NiParticles::LinkObject(kStream);

    unsigned int uiNumModifiers = kStream.GetNumberOfLinkIDs();
    for (unsigned int ui = 0; ui < uiNumModifiers; ui++)
    {
        NiPSysModifier* pkModifier = (NiPSysModifier*)
            kStream.GetObjectFromLinkID();
        AddModifier(pkModifier);
    }
}
//---------------------------------------------------------------------------
bool NiParticleSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiParticles::RegisterStreamables(kStream))
    {
        return false;
    }

    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    while (kIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kIter);
        pkModifier->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiParticleSystem::SaveBinary(NiStream& kStream)
{
    NiParticles::SaveBinary(kStream);

    NiBool bWorldSpace = m_bWorldSpace;
    NiStreamSaveBinary(kStream, bWorldSpace);

    NiStreamSaveBinary(kStream, m_kModifierList.GetSize());
    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    while (kIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kIter);
        kStream.SaveLinkID(pkModifier);
    }
}
//---------------------------------------------------------------------------
bool NiParticleSystem::IsEqual(NiObject* pkObject)
{
    if (!NiParticles::IsEqual(pkObject))
    {
        return false;
    }

    NiParticleSystem* pkDest = (NiParticleSystem*) pkObject;

    if (m_bWorldSpace != pkDest->m_bWorldSpace ||
        m_bDynamicBounds != pkDest->m_bDynamicBounds)
    {
        return false;
    }

    NiTListIterator kListIter = m_kModifierList.GetHeadPos();
    while (kListIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kListIter);
        NiPSysModifier* pkDestModifier = pkDest->GetModifierByName(
            pkModifier->GetName());
        if (!pkModifier->IsEqual(pkDestModifier))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiParticleSystem::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiParticles::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiParticleSystem::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("World Space", m_bWorldSpace));

    NiTListIterator kIter = m_kModifierList.GetHeadPos();
    while (kIter)
    {
        NiPSysModifier* pkModifier = m_kModifierList.GetNext(kIter);
        pkModifier->GetViewerStrings(pkStrings);
    }
}
//---------------------------------------------------------------------------
