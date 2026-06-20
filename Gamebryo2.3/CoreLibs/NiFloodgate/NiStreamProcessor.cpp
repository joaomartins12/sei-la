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
#include "NiStreamProcessor.h"
#include "NiFloodgateSDM.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NiStreamProcessor* NiStreamProcessor::ms_pkInstance = NULL;
NiTObjectPool<NiSPWorkflow>* NiStreamProcessor::ms_pkWorkflowPool = NULL; 
//---------------------------------------------------------------------------
NiStreamProcessor::NiStreamProcessor(NiUInt32 uiMaxQueueSize) :
    m_kWorkflows(256),
    m_eStatus(STOPPED),
    m_uiMaxQueueSize(uiMaxQueueSize),
    m_pkThreadPool(NULL),
    m_kSubmitSemaphore(0, uiMaxQueueSize),
    m_pkSubmitThread(NULL),
    m_pkSubmitThreadProc(NULL),
    m_pkPollThread(NULL),
    m_pkPollThreadProc(NULL),
    m_bIsActive(false)
{
}
//---------------------------------------------------------------------------
NiStreamProcessor::~NiStreamProcessor()
{
    NiDelete m_pkSubmitThread;
    NiDelete m_pkSubmitThreadProc;
    NiDelete m_pkPollThread;
    NiDelete m_pkPollThreadProc;
}
//---------------------------------------------------------------------------
void NiStreamProcessor::InitializePools()
{
    NIASSERT(ms_pkWorkflowPool == NULL);
    ms_pkWorkflowPool = NiNew NiTObjectPool<NiSPWorkflow>(128);

    // Initialize Pool
    NiSPTaskImpl::InitializePools(128);
    NiSPWorkflow::InitializePools(128, 128);
    
}
//---------------------------------------------------------------------------
void NiStreamProcessor::ShutdownPools()
{
    NIASSERT(ms_pkWorkflowPool);
    NiDelete ms_pkWorkflowPool;
    ms_pkWorkflowPool = NULL;

    NiSPTaskImpl::ShutdownPools();
    NiSPWorkflow::ShutdownPools();
}
//---------------------------------------------------------------------------
bool NiStreamProcessor::Initialize(NiUInt32 uiMaxQueueSize)
{
    // Create instance
    ms_pkInstance = NiNew NiStreamProcessor(uiMaxQueueSize);
    NIASSERT(ms_pkInstance);
    if (ms_pkInstance)
    {
        ms_pkInstance->Initialize();
        ms_pkInstance->Start();
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiStreamProcessor::Shutdown()
{
    // Stop the manager and wait for background thread to complete
    ms_pkInstance->Stop();

    // Perform internal shutdown
    ms_pkInstance->ShutdownInternal();

    // Delete instance
    NIASSERT(ms_pkInstance);
    NiDelete ms_pkInstance;
    ms_pkInstance = NULL;
    return true;
}
//---------------------------------------------------------------------------
bool NiStreamProcessor::Poll(const NiSPWorkflow* pkWorkflow)
{
    if (pkWorkflow)
    {
        NiSPWorkflow::Status eStatus = pkWorkflow->GetStatus();
        if (eStatus == NiSPWorkflow::COMPLETED 
            ||  eStatus == NiSPWorkflow::ABORTED)
        {

            // Memory synch on Xbox 360. Win32 handles it in the compiler via
            // the "volatile" keyword. PS3's memory architecture precludes
            // it from being a problem in this case.
#if defined(_XENON)
            // We read a volatile flag that indicated the workflow was
            // complete. We need a memory barrier to insure that any reads
            // of the results are at least as current as that flag.
            __lwsync();
#endif
            return true;
        }
        FinishOne(false);
    }

    return false;
}

//---------------------------------------------------------------------------
bool NiStreamProcessor::Wait(const NiSPWorkflow* pkWorkflow, 
    NiUInt64 uiTimeout)
{
    NiUInt64 uiTimoutMicroSecs = NiGetPerformanceCounter() + uiTimeout;
    
    // Wait until our workflow is done or timout has occurred
    while (uiTimeout == 0 || (NiGetPerformanceCounter() < uiTimoutMicroSecs))
    {
        if (Poll(pkWorkflow))
            return true;
    }
    return false;
}

//---------------------------------------------------------------------------
NiStreamProcessor::Status NiStreamProcessor::Start()
{
    if (m_eStatus == NiStreamProcessor::RUNNING)
        return m_eStatus;

    m_eStatus = NiStreamProcessor::RUNNING;

    m_pkSubmitThreadProc = NiNew NiStreamProcessor::ManagerProc(this);
    NIASSERT(m_pkSubmitThreadProc);

    m_pkSubmitThread = NiThread::Create(m_pkSubmitThreadProc);
    m_pkSubmitThread->SetPriority(eThreadPriority);
#if defined(_XENON)
    NiProcessorAffinity kAffinity(
        NiProcessorAffinity::PROCESSOR_XENON_CORE_0_THREAD_1, 
        NiProcessorAffinity::PROCESSOR_DONT_CARE);
        m_pkSubmitThread->SetThreadAffinity(kAffinity);
#endif
    NIASSERT(m_pkSubmitThread);
    m_pkSubmitThread->SetName("Floodgate: Submit Thread");
    m_pkSubmitThread->Resume();
    
    m_eStatus = StartInternal();

    return m_eStatus;
}
//---------------------------------------------------------------------------
NiStreamProcessor::Status NiStreamProcessor::Stop()
{
    m_kManagerLock.Lock();
    m_eStatus = NiStreamProcessor::STOPPING;
    m_kManagerLock.Unlock();
    
    Clear();

    // Signal the update and poll semaphore to allow the thread to complete
    m_kSubmitSemaphore.Signal();

    m_pkSubmitThread->WaitForCompletion();
    
    return StopInternal();
}
//---------------------------------------------------------------------------
void NiStreamProcessor::Clear()
{
    NIASSERT(m_eStatus == STOPPING);

    m_kManagerLock.Lock();
      
    for (NiInt32 iPriority = IMMEDIATE; iPriority >= 0; iPriority--)
    {
        NiSPWorkflow* pkWorkflow = m_akWorkflowQueues[iPriority].Remove();
        while (pkWorkflow)
        {
            pkWorkflow->SetStatus(NiSPWorkflow::ABORTED);
            pkWorkflow->Clear();
            pkWorkflow = m_akWorkflowQueues[iPriority].Remove();
        }
    }
    m_eStatus = STOPPED;

    m_kManagerLock.Unlock();
}
//---------------------------------------------------------------------------
void NiStreamProcessor::Clear(NiSPWorkflow* pkWorkflow)
{
    m_kManagerLock.Lock();

    pkWorkflow->Clear();
    pkWorkflow->SetStatus(NiSPWorkflow::IDLE);
    m_kManagerLock.Unlock();
  
}
//---------------------------------------------------------------------------
bool NiStreamProcessor::Submit(NiSPWorkflow* pkWorkflow, Priority ePriority)
{
    // Lock the manager when accessing the task queue.
    m_kManagerLock.Lock();
    
    // Reset the workflow in case it was used before
    NIASSERT(pkWorkflow);
    pkWorkflow->Reset();
    
    // Get queue and check for available space
    NiSPWorkflowQueue &kQueue = m_akWorkflowQueues[ePriority];
    if (kQueue.GetSize() >= m_uiMaxQueueSize)
    {
        NILOG("NiStreamProcessor::AddWorkflow: Failed.\n");
        m_kManagerLock.Unlock();
        return false;
    }
    
    // Add workflow to the proper queue
    kQueue.Add(pkWorkflow);

    // Update the status of the workflow to pending
    pkWorkflow->SetStatus(NiSPWorkflow::PENDING);

    // Release the lock
    m_kManagerLock.Unlock();
  

    // Signal the update semaphore to wake up the processing thread.
    m_kSubmitSemaphore.Signal();
    
    return true;
}
//---------------------------------------------------------------------------
NiSPWorkflow* NiStreamProcessor::GetFreeWorkflow()
{
    NIASSERT(ms_pkWorkflowPool && "ms_pkWorkflowPool was NULL!");
    NiSPWorkflow* pkWorkflow = ms_pkWorkflowPool->GetFreeObject();
    NIASSERT(pkWorkflow && "pkWorkflow was NULL!");
    if (pkWorkflow)
    {
        pkWorkflow->SetStatus(NiSPWorkflow::IDLE);
    }
    return pkWorkflow;
}
//---------------------------------------------------------------------------
void NiStreamProcessor::ReleaseWorkflow(NiSPWorkflow* pkWorkflow)
{
    NIASSERT(pkWorkflow && "pkWorkflow was NULL!");
    NIASSERT(ms_pkWorkflowPool && "ms_pkWorkflowPool was NULL!");
    if (pkWorkflow)
    {
        pkWorkflow->SetStatus(NiSPWorkflow::IDLE);
        pkWorkflow->Clear(true);
        ms_pkWorkflowPool->ReleaseObject(pkWorkflow);
    }
}
//---------------------------------------------------------------------------
// ManagerProc functions.
//---------------------------------------------------------------------------
NiStreamProcessor::ManagerProc::ManagerProc(NiStreamProcessor* pkManager) : 
    m_pkManager(pkManager)
{
}
//---------------------------------------------------------------------------
NiUInt32 NiStreamProcessor::ManagerProc::ThreadProcedure(void* pvArg)
{
    m_pkThread = (NiThread*)pvArg;

    // Loop on the manager until an external source sets it to stop.
    while (m_pkManager->IsRunning())
    {
        // Iterate over all queues in order of decreasing priority
        for (NiInt32 iPriority = IMMEDIATE; iPriority >= 0; iPriority--)
        {
            // If there are no tasks in the queues, then the ExecuteWorkflow
            // call will block on the update semaphore and we won't spend 
            // time iterating over empty queues.
            //
            // If the task is executed then break out of the for-loop and
            // start back at the beginning of the priorities
            if (m_pkManager->ExecuteOne(static_cast<Priority>(iPriority)))
                break;
        }
    }

    // Stop signal has been received. We'll return an exit code of 0.
    return 0;
}
