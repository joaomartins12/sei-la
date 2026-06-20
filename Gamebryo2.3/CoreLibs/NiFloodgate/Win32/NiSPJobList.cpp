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
#include "NiSPJob.h"
#include "NiSPTask.h"
#include "NiSPKernel.h"
#include "NiSPAlgorithms.h"
#include "NiSPStreamPartitioner.h"
#include "NiStreamProcessor.h"
//---------------------------------------------------------------------------
NiSPJobList::NiSPJobList() 
    : m_uiJobCount(0)
    , m_bIsFinished(false)
{
}
//---------------------------------------------------------------------------
NiSPJobList::~NiSPJobList()
{
}
//---------------------------------------------------------------------------
bool NiSPJobList::Initialize(NiSPTask* pkTask)
{
    memset(m_akWorkload, 0, sizeof(m_akWorkload));
    m_uiJobCount = 0;
    m_bIsFinished = false;

    NiSPStreamPartitioner* pkPartitioner = 
        pkTask->GetImpl().GetStreamPartitioner();
     
#if _DEBUG
    if (pkTask->IsSync() && m_akWorkload[0].GetBlockCount() != 0)
    {
        NIDEBUGBREAK();
    }
#endif

    if (pkTask->IsSync())
    {
        // Create a single stream-less job
        Build(m_uiJobCount++, pkTask);
    }
    else
    {
        // Create a job for each group of blocks
        while (pkPartitioner->HasMoreBlocks() && m_uiJobCount < MAX_JOBS)
        {
            Build(m_uiJobCount++, pkTask);
        }
    }



    if (pkTask->IsSync() && m_uiJobCount != 1)
    {
        NIDEBUGBREAK();
    }

    // If we have a partitioner then return true only if there is more 
    // data that needs to be scheduled for execution in an additional list
    if (pkPartitioner)
        return pkPartitioner->HasMoreBlocks();
    
    // All data has been processed, so no more iterations required
    return false;
}


//---------------------------------------------------------------------------
void NiSPJobList::Build(NiUInt32 uiJobIndex, NiSPTask* pkTask)
{
    NIASSERT(uiJobIndex < MAX_JOBS);

    // If there is a partitioner, use it to partition the data
    NiUInt32 uiInputSize = 0;
    NiUInt32 uiOutputSize = 0;
    
    NiSPWorkload& kWorkload = GetWorkload(uiJobIndex);
    kWorkload.Clear();

    if (!pkTask->IsSync())
    {
        NiSPStreamPartitioner* pkPartitioner = 
            pkTask->GetImpl().GetStreamPartitioner();
        pkPartitioner->Partition(this, uiJobIndex, uiInputSize, uiOutputSize);
    }
    else
    {
        // Do nothing for sync tasks for now
    }

    // Set kernel
    NiSPKernel *pkKernel = pkTask->GetKernel();
    NIASSERT(pkKernel && "Invalid Kernel");
    kWorkload.SetKernel(pkKernel);
}
