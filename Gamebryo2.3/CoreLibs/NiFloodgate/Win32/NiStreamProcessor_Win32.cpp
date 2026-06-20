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
#include <NiSystemDesc.h>
#include "NiStreamProcessor.h"
#include "NiSPWorkflow.h"

//---------------------------------------------------------------------------
bool NiStreamProcessor::Initialize()
{
    const NiSystemDesc& kSystemDesc = NiSystemDesc::GetSystemDesc();
    NIASSERT(kSystemDesc.GetPlatformID() == NiSystemDesc::NI_WIN32);
    unsigned int uiProcCount = kSystemDesc.GetLogicalProcessorCount();
    
    unsigned int uiTaskThreads = NiMax(1, uiProcCount - 1);
    
    m_pkThreadPool = NiNew NiSPThreadPool(uiTaskThreads);
    NIASSERT(m_pkThreadPool);
    m_pkThreadPool->Initialize();
    return true;
}
//---------------------------------------------------------------------------
bool NiStreamProcessor::ShutdownInternal()
{
    m_pkThreadPool->Shutdown();
    NiDelete m_pkThreadPool;
    m_pkThreadPool = NULL;
    return true;
}

//---------------------------------------------------------------------------
void NiStreamProcessor::Receive()
{
    // Noop for win32
}
//---------------------------------------------------------------------------
NiStreamProcessor::Status NiStreamProcessor::StartInternal()
{
    return NiStreamProcessor::RUNNING;
}
NiStreamProcessor::Status NiStreamProcessor::StopInternal()
{
    return NiStreamProcessor::STOPPED;
}
//---------------------------------------------------------------------------
void NiStreamProcessor::FinishOne(bool bIsBlocking, NiUInt64 uiTimeout)
{
    (void)bIsBlocking;
    (void)uiTimeout;

    m_kManagerLock.Lock();
        
    // Find next completed workflow
    NiTMapIterator kIter = m_kWorkflows.GetFirstPos();
    while(kIter)
    {
        NiUInt32 uiKey = 0;
        NiSPWorkflow* pkWorkflow = NULL;
        m_kWorkflows.GetNext(kIter, uiKey, pkWorkflow);
        
        // Mark as complete and remove from lookup table
        if (pkWorkflow->GetStatus() == NiSPWorkflow::COMPLETED)
        {
            m_kWorkflows.RemoveAt(uiKey);
            m_kManagerLock.Unlock();
 
            return;
        }
    }
    m_kManagerLock.Unlock();
    
}
//---------------------------------------------------------------------------
bool NiStreamProcessor::ExecuteOne(Priority ePriority)
{
    // Block on the update semaphore until at least one workflow is added
    m_kSubmitSemaphore.Wait();
    
    m_kManagerLock.Lock();
    
    // When we get here, a workflow was in -some- queue, but not necessarily
    // the one with the priority we are looking for...
    // We might not get a workflow from the queue with the priority specified.
    NiSPWorkflow* pkWorkflow = m_akWorkflowQueues[ePriority].IsEmpty() ?
        NULL : m_akWorkflowQueues[ePriority].Head();
    
    // Workflow could be NULL if the current queue is empty
    if (pkWorkflow)
    {
        if (pkWorkflow->GetStatus() == NiSPWorkflow::PENDING)
        {
            // Attempt to prepare the task
            if (pkWorkflow->Prepare())
            {
                // Determine dependencies and 
                // execution order of contained tasks
                m_kAnalyzer.Analyze(pkWorkflow);
                   
                // Set workflow id on manager hash table
                NIASSERT(pkWorkflow->GetId() != 0)
                
                // Add the workflow to the manager's hash table for fast lookup
                m_kWorkflows.SetAt(pkWorkflow->GetId(), pkWorkflow);

                // Update status and execute
                pkWorkflow->SetStatus(NiSPWorkflow::RUNNING);

                // Remove the workflow that was just processed from the queue
                m_akWorkflowQueues[ePriority].Remove();

                pkWorkflow->Execute();
    
                // If the workflow is still running, add it to the back of 
                // the queue for polling later.
                if (pkWorkflow->GetStatus() == NiSPWorkflow::RUNNING)
                {
                    m_akWorkflowQueues[ePriority].Add(pkWorkflow);
                    m_kSubmitSemaphore.Signal();
                }

                m_kManagerLock.Unlock();
                return true;
            }
            else
            {
                NiSleep(1);
            }
        }
        else if (pkWorkflow->GetStatus() == NiSPWorkflow::RUNNING)
        {
            // Remove the workflow to be processed from the queue
            m_akWorkflowQueues[ePriority].Remove();

            pkWorkflow->Execute();

            // If the workflow is still running, add it to the back of 
            // the queue for polling later.
            if (pkWorkflow->GetStatus() == NiSPWorkflow::RUNNING)
            {
                m_akWorkflowQueues[ePriority].Add(pkWorkflow);
                m_kSubmitSemaphore.Signal();
            }

            m_kManagerLock.Unlock();
            return true;
        }
    }

    m_kManagerLock.Unlock();
    
    
    // If we didn't remove a workflow, signal the semaphore 
    // so that it still holds the correct count of tasks.
    m_kSubmitSemaphore.Signal();
    return false;
}
//---------------------------------------------------------------------------
