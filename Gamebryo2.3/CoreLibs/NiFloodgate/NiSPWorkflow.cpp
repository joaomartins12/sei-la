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

//---------------------------------------------------------------------------
#include "NiFloodgatePCH.h"
#include "NiSPWorkflow.h"
#include "NiSPTask.h"
//---------------------------------------------------------------------------
NiTObjectPool<NiSPTask>* NiSPWorkflow::ms_pkTaskPool = NULL;
NiTObjectPool<NiSPWorkflowImpl>* NiSPWorkflow::ms_pkImplPool = NULL;
//---------------------------------------------------------------------------
void NiSPWorkflow::InitializePools(NiUInt32 uiWorkflowPoolSize,
    NiUInt32 uiTaskPoolSize)
{
    NIASSERT(ms_pkTaskPool == NULL);
    ms_pkTaskPool = NiNew NiTObjectPool<NiSPTask>(uiTaskPoolSize);

    NIASSERT(ms_pkImplPool == NULL);
    ms_pkImplPool = NiNew NiTObjectPool<NiSPWorkflowImpl>(uiWorkflowPoolSize);
}
//---------------------------------------------------------------------------
void NiSPWorkflow::ShutdownPools()
{
    NIASSERT(ms_pkTaskPool != NULL);
    NiDelete ms_pkTaskPool;
    ms_pkTaskPool = NULL;

    NIASSERT(ms_pkImplPool != NULL);
    NiDelete ms_pkImplPool;
    ms_pkImplPool = NULL;
}
//---------------------------------------------------------------------------
NiSPWorkflow::NiSPWorkflow() :
    m_pkWorkflowImpl(NULL),
    m_uiId(0),
    m_iCurrentStage(-1),
    m_eStatus(IDLE)
{
}
//---------------------------------------------------------------------------
NiSPWorkflow::~NiSPWorkflow()
{
    Clear();
}
//---------------------------------------------------------------------------
bool NiSPWorkflow::Prepare()
{
    // Return workflow implementation to pool
    NIASSERT(ms_pkImplPool && "ms_pkImplPool was NULL!");
    if (!m_pkWorkflowImpl)
    {
        // Get an implementation object from the pool
        m_pkWorkflowImpl = ms_pkImplPool->GetFreeObject();
        NIASSERT(m_pkWorkflowImpl && "m_pkWorkflowImpl was NULL!");
        if (!m_pkWorkflowImpl)
        {
            return false;
        }
    }

    // Attempt to initialize it
    if (!m_pkWorkflowImpl->Initialize())
        return false;
    
    // Find sync task
    NiUInt32 uiTaskCount = GetSize();
    for (NiUInt32 uiIndex = 0; uiIndex < uiTaskCount; ++uiIndex)
    {
        // If this is a sync task then set this workflow's id to 
        // that of its signal task so that the manager can detect 
        // its completion
        NiSPTask* pkTask = GetAt(uiIndex);
        if (pkTask->GetTotalCount() == 0)
        {
            m_uiId = pkTask->GetId();
        }
    }

    // Prepare contained tasks
    for (NiUInt32 uiIndex = 0; uiIndex < uiTaskCount; ++uiIndex)
    {
        NiSPTask* pkTask = GetAt(uiIndex);
        pkTask->SetWorkflowId(m_uiId);
        pkTask->Prepare();
    }
    // Ensure that the workflow has been given an id
    NIASSERT(m_uiId > 0 && "Workflow signal id is invalid!");

   
    return m_uiId > 0;
}
//---------------------------------------------------------------------------
void NiSPWorkflow::Reset()
{
    // Clear all tasks...
    NIASSERT(ms_pkTaskPool && "ms_pkTaskPool was NULL!");
    NiInt32 uiIndex = GetSize();
    while (--uiIndex >= 0)
    {
        NiSPTask* pkTask = GetAt(uiIndex);
        pkTask->Reset();
    }

    // Reset status
    m_iCurrentStage = -1;
    ClearPendingJobLists();
    SetStatus(NiSPWorkflow::IDLE);
 
}
//---------------------------------------------------------------------------
void NiSPWorkflow::Clear(bool bIgnoreCaching)
{
    // Clear all tasks...
    
    NIASSERT(ms_pkTaskPool && "ms_pkTaskPool was NULL!");
    NiInt32 uiIndex = GetSize();
    while (--uiIndex >= 0)
    {
        NiSPTask* pkTask = GetAt(uiIndex);
        
        pkTask->Clear(bIgnoreCaching);
        if (bIgnoreCaching)
        {
            RemoveAt(uiIndex);
            ms_pkTaskPool->ReleaseObject(pkTask);
        }
    }

    // Release impl
    if (m_pkWorkflowImpl)
    {
        ms_pkImplPool->ReleaseObject(m_pkWorkflowImpl);
        m_pkWorkflowImpl = NULL;
    }

    m_iCurrentStage = -1;
    ClearPendingJobLists();
    m_uiId = 0;

}
//---------------------------------------------------------------------------
void NiSPWorkflow::HandleSignal(NiUInt32 uiSignal)
{
}
//---------------------------------------------------------------------------
