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
#include "NiAnimationPCH.h"

#include "NiGeomMorpherUpdateTask.h"

NiImplementRTTI(NiGeomMorpherUpdateTask, NiTask);

NiCriticalSection NiGeomMorpherUpdateTask::ms_kPoolCriticalSection;
NiTObjectPool<NiGeomMorpherUpdateTask>* NiGeomMorpherUpdateTask::ms_pkPool;
//----------------------------------------------------------------------------
NiGeomMorpherUpdateTask::NiGeomMorpherUpdateTask()
{
}
//----------------------------------------------------------------------------
void NiGeomMorpherUpdateTask::_SDMInit()
{
    ms_pkPool = NiNew NiTObjectPool<NiGeomMorpherUpdateTask>;
}
//----------------------------------------------------------------------------
void NiGeomMorpherUpdateTask::_SDMShutdown()
{
    ms_pkPool->PurgeAllObjects();
    NiDelete ms_pkPool;
}
//----------------------------------------------------------------------------
NiGeomMorpherUpdateTask* NiGeomMorpherUpdateTask::GetFreeObject()
{
    ms_kPoolCriticalSection.Lock();
    NiGeomMorpherUpdateTask* pkTask = ms_pkPool->GetFreeObject();
    ms_kPoolCriticalSection.Unlock();
    return pkTask;
}
//---------------------------------------------------------------------------
void NiGeomMorpherUpdateTask::ReleaseObject(NiGeomMorpherUpdateTask* pkTask)
{
    ms_kPoolCriticalSection.Lock();
    ms_pkPool->ReleaseObject(pkTask);
    ms_kPoolCriticalSection.Unlock();
}
//----------------------------------------------------------------------------
void NiGeomMorpherUpdateTask::DoTask()
{
    //Only morph if something other than this task is referencing the object.
    if (m_spTarget->GetRefCount() > 1)
    {
        m_spMorphController->OnPreDisplay();
    }

    Clear();
}
//----------------------------------------------------------------------------
void NiGeomMorpherUpdateTask::SetController(
    NiGeomMorpherController* pkController)
{
    m_spMorphController = pkController;
    m_spTarget = ((NiTimeController*)pkController)->GetTarget();
}
//----------------------------------------------------------------------------
bool NiGeomMorpherUpdateTask::Clear()
{
    m_spMorphController = 0;
    m_spTarget = 0;
    ReleaseObject(this);
    return true;
}
