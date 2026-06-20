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

#include "NiPSysUpdateTask.h"
#include "NiParticleSystem.h"

NiImplementRTTI(NiPSysUpdateTask, NiTask);

NiCriticalSection NiPSysUpdateTask::ms_kPoolCriticalSection;
NiTObjectPool<NiPSysUpdateTask>* NiPSysUpdateTask::ms_pkPool;
//----------------------------------------------------------------------------
NiPSysUpdateTask::NiPSysUpdateTask() :
    m_fTime(0.0f)
{
}
//----------------------------------------------------------------------------
NiPSysUpdateTask::~NiPSysUpdateTask()
{
    m_spSystem = NULL;
}
//---------------------------------------------------------------------------
void NiPSysUpdateTask::_SDMInit()
{
    ms_pkPool = NiNew NiTObjectPool<NiPSysUpdateTask>;
}
//---------------------------------------------------------------------------
void NiPSysUpdateTask::_SDMShutdown()
{
    ms_pkPool->PurgeAllObjects();
    NiDelete ms_pkPool;
}
//---------------------------------------------------------------------------
NiPSysUpdateTask* NiPSysUpdateTask::GetFreeObject()
{
    ms_kPoolCriticalSection.Lock();
    NiPSysUpdateTask* pkTask = ms_pkPool->GetFreeObject();
    ms_kPoolCriticalSection.Unlock();
    return pkTask;
}
//---------------------------------------------------------------------------
void NiPSysUpdateTask::ReleaseObject(NiPSysUpdateTask* pkTask)
{
    ms_kPoolCriticalSection.Lock();
    ms_pkPool->ReleaseObject(pkTask);
    ms_kPoolCriticalSection.Unlock();
}
//----------------------------------------------------------------------------
void NiPSysUpdateTask::Init(NiParticleSystem* pkSystem, float fTime)
{
    m_spSystem = pkSystem;
    m_fTime = fTime;
}
//----------------------------------------------------------------------------
void NiPSysUpdateTask::DoTask()
{
    if ( m_spSystem != NULL )
    {
        m_spSystem->Do_UpdateSystem(m_fTime);
        m_spSystem = NULL;
    }
    Clear();
}
//----------------------------------------------------------------------------
bool NiPSysUpdateTask::Clear()
{
    m_spSystem = NULL;
    ReleaseObject(this);
    return true;
}
