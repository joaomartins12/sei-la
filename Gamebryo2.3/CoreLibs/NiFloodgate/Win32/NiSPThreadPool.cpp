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
#include "NiSPThreadPool.h"
#include "NiSPWorkflow.h"
#include "NiSPTask.h"
#define MAX_QUEUESIZE 128
//---------------------------------------------------------------------------
void NiSPThreadPool::Initialize()
{
    m_apkWorkerThreads = NiNew ThreadArray(m_uiMaxWorkerThreads);
    NIASSERT(m_apkWorkerThreads);

    m_akWorkerProcs = NiNew WorkerProc[m_uiMaxWorkerThreads];
    NIASSERT(m_akWorkerProcs);

    // Create update threads and resume them.
    for (unsigned int i = 0; i < m_uiMaxWorkerThreads; i++)
    {
        m_akWorkerProcs[i].SetPool(this);

        NiThread* pkThread = NiThread::Create(&m_akWorkerProcs[i]);
        pkThread->SetPriority(NiThread::HIGHEST);
        pkThread->Resume();
        m_apkWorkerThreads->Add(pkThread);
    }
}
//---------------------------------------------------------------------------
void NiSPThreadPool::Shutdown()
{
    for (unsigned int i = 0; i < m_uiMaxWorkerThreads; i++)
    {
        m_akWorkerProcs[i].SignalShutdown();
    }

    // All threads have shutdown signal. Signal the semaphore to wake them
    // up and have them exit.
    for (unsigned int i = 0; i < m_uiMaxWorkerThreads; i++)
    {
        m_kWorkQueueSemaphore.Signal();
    }
    
    // Send one more signal to the work queue semaphore. We're shutting down
    // so it won't matter if it's extraneous.
    m_kWorkQueueSemaphore.Signal();
}
//---------------------------------------------------------------------------
NiSPThreadPool::NiSPThreadPool(unsigned int uiMaxThreads) 
    : m_uiMaxWorkerThreads(uiMaxThreads)
    , m_kWorkQueue(MAX_QUEUESIZE)
    , m_kWorkQueueSemaphore(0,MAX_QUEUESIZE) 
{
}
//---------------------------------------------------------------------------
NiSPThreadPool::~NiSPThreadPool()
{
    // NiThread dtor waits for the completion we signaled in Shutdown.
    for (unsigned int i = 0; i < m_uiMaxWorkerThreads; i++)
    {
        NiDelete m_apkWorkerThreads->GetAt(i);
    }
    NiDelete m_apkWorkerThreads;
    NiDelete [] m_akWorkerProcs;

}
//---------------------------------------------------------------------------
void NiSPThreadPool::ExecuteWorkflow(NiSPWorkflow* pkWorkflow)
{
    NIASSERT(pkWorkflow->GetStatus() == NiSPWorkflow::RUNNING);

    // Verify that pending job lists are complete. Otherwise, return.
    if (pkWorkflow->GetNumPendingJobLists() > 0)
    {
        NiUInt32 uiJobs = pkWorkflow->GetNumPendingJobLists();
        for (NiUInt32 ui = 0; ui < uiJobs; ++ui)
        {
            if (!pkWorkflow->GetPendingJobList(ui)->IsFinished())
                return;
        }
        pkWorkflow->ClearPendingJobLists();
    }

    // If we reach here, all pending job lists have been processed by the
    // thread pool and we need to submit the next stage.
    NiUInt32 uiTaskCount = pkWorkflow->GetSize();
    NiUInt32 uiIndex = 0;

    // Find first task in the next stage. Set workflow's current stage and
    // interrupt the loop.
    for (; uiIndex < uiTaskCount; ++uiIndex)
    {
        NiSPTask* pkTask = pkWorkflow->GetAt(uiIndex);

        if (pkTask->GetStage() > pkWorkflow->GetCurrentStage())
        {
            pkWorkflow->SetCurrentStage(pkTask->GetStage());
            break;
        }
    }

    // Traverse the tasks until they are in a subsequent stage. Submit them
    // to the thread pool.
    for (; uiIndex < uiTaskCount; ++uiIndex)
    {
        NiSPTask* pkTask = pkWorkflow->GetAt(uiIndex);

        // Exit if we've submitted everything for the current stage.
        if (pkTask->GetStage() > pkWorkflow->GetCurrentStage())
            break;

        // Submit for the current stage.
        NiSPTaskImpl& kTaskImpl = pkTask->GetImpl();
        NiUInt32 uiListCount = kTaskImpl.GetSize();
        for (NiUInt32 i = 0; i < uiListCount; i++)
        {
            NiSPJobList* pkJobList = kTaskImpl.GetAt(i);
            pkJobList->SetIsFinished(false);
            Enqueue(pkJobList);
            pkWorkflow->AddPendingJobList(pkJobList);
        }
    }

    // If we've just fired off the signal NOP task, then mark this workflow
    // as completed.
    if (pkWorkflow->GetCurrentStage() == NiSPAnalyzer::MAX_STAGE)
    {
        pkWorkflow->ExecutionComplete();

        pkWorkflow->SetStatus(NiSPWorkflow::COMPLETED);
    }
}


//---------------------------------------------------------------------------
void NiSPThreadPool::Enqueue(NiSPJobList* pkJobList)
{
    // Spin yield if there's no room in the queue. Let the worker threads
    // peel some work off.
    while (m_kWorkQueue.GetSize() >= MAX_QUEUESIZE - 1)
    {
        NiYieldThread();
    }

    // Lock the manager when accessing the task queue.
    m_kLock.Lock();

    // Get queue and check for available space
    if (m_kWorkQueue.GetSize() < MAX_QUEUESIZE)
    {
        m_kWorkQueue.Add(pkJobList);
    }
    else
    {
        NILOG("Error: Queue full.");
    }
    // Unlock
    m_kLock.Unlock();

    // Signal the update semaphore to wake up the processing thread.
    m_kWorkQueueSemaphore.Signal();
}

//---------------------------------------------------------------------------
NiSPJobList* NiSPThreadPool::Dequeue()
{
    m_kWorkQueueSemaphore.Wait();

    m_kLock.Lock();
    NiSPJobList* pkJobList = m_kWorkQueue.Remove();
    m_kLock.Unlock();

    return pkJobList;
}


//---------------------------------------------------------------------------
// ManagerProc functions.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
unsigned int NiSPThreadPool::WorkerProc::ThreadProcedure(void* pvArg)
{
    m_bShutdown = false;
    while (!m_bShutdown)
    {
        // Block waiting for work.
        NiSPJobList* pkWork = m_pkPool->Dequeue();
        if (pkWork)
        {
            // Process work
            NiUInt32 uiJobCount = pkWork->GetJobCount();
            for (NiUInt32 j = 0; j < uiJobCount; j++)
            {
                NiSPWorkload& kWorkload = pkWork->GetWorkload(j);
                NiSPKernel* pkKernel = kWorkload.GetKernel();
                pkKernel->Execute(kWorkload);
            }
            pkWork->SetIsFinished(true);
        }
    }
    
    return 0;
}

