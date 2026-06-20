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
#include "NiSPStreamPartitioner.h"
#include "NiSPJobList.h"
#include "NiSPTask.h"
#include "NiSPWorkload.h"
#include "NiSPStream.h"
#include "NiStreamProcessor.h"
//---------------------------------------------------------------------------
NiSPStreamPartitioner::NiSPStreamPartitioner() :
    m_uiOptimalBlockCount(0), 
    m_uiCurrentBlockIndex(0), 
    m_uiTotalBlockCount(0),
    m_uiTransferSize(0),
    m_pkTask(NULL)
{
}
//---------------------------------------------------------------------------
void NiSPStreamPartitioner::Initialize(NiSPTask* pkTask)
{
    NIASSERT(pkTask != NULL);
    m_pkTask = pkTask;
    m_uiTotalBlockCount = 0;
    m_uiOptimalBlockCount = 0; 
    m_uiCurrentBlockIndex = 0; 
    m_uiTotalBlockCount = 0;
    m_uiTransferSize = 0;

    // Iterate through inputs streams looking for 
    // first non fixed stream to determine block count
    NiUInt32 uiInputStreamCount = pkTask->GetInputCount();
    for (NiUInt32 uiIndex = 0; uiIndex < uiInputStreamCount; ++uiIndex)
    {
        NiSPStream* pStream = pkTask->GetInputAt(uiIndex);
        if (pStream->IsPartitioningEnabled())
        {
            m_uiTotalBlockCount = pStream->GetBlockCount();
            break;
        }
    }

    // If block count couldn't be determined from input streams
    // then look through the output streams
    if (m_uiTotalBlockCount == 0)
    {
        NiUInt32 uiOutputStreamCount = pkTask->GetOutputCount();
        for (NiUInt32 uiIndex = 0; uiIndex < uiOutputStreamCount; uiIndex++)
        {
            NiSPStream* pStream = pkTask->GetOutputAt(uiIndex);
            if (pStream->IsPartitioningEnabled())
            {
                m_uiTotalBlockCount = pStream->GetBlockCount();
                break;
            }
        }
    }
    NIASSERT(m_uiTotalBlockCount != 0 && "TotalBlockCount was 0");

    m_uiOptimalBlockCount = pkTask->GetOptimalBlockCount();
}
//---------------------------------------------------------------------------
void NiSPStreamPartitioner::Partition(NiSPJobList* pkJob, 
    NiUInt32 uiJobIndex, NiUInt32& uiInputSize, NiUInt32& uiOutputSize)
{
    uiInputSize = 0;
    uiOutputSize = 0;

    // Initialize workload with task data
    NiSPWorkload& kWorkload = pkJob->GetWorkload(uiJobIndex);
    InitializeWorkload(kWorkload);

    // Partition input streams
    NiUInt32 uiInputCount = m_pkTask->GetInputCount();
    for (NiUInt32 uiIndex = 0; uiIndex < uiInputCount ; ++uiIndex)
    {
        NiSPStream* pkStream = m_pkTask->GetInputAt(uiIndex);
        if (pkStream->GetData())
        {
            uiInputSize += PartitionInput(pkJob, uiJobIndex, pkStream, 
                kWorkload.m_kInputs[uiIndex]);
        }
    }

    // Partition output streams
    NiUInt32 uiOutputCount = m_pkTask->GetOutputCount();
    for (NiUInt32 uiIndex = 0; uiIndex < uiOutputCount ; ++uiIndex)
    {
        NiSPStream* pkStream = m_pkTask->GetOutputAt(uiIndex);
        if (pkStream->GetData())
        {
            uiOutputSize += PartitionOutput(pkJob, uiJobIndex, pkStream, 
                kWorkload.m_kOutputs[uiIndex]);
        }
    }

    // Move to next set of blocks
    AdvanceBlockIndex();
}
//---------------------------------------------------------------------------
void NiSPStreamPartitioner::InitializeWorkload(NiSPWorkload& kWorkload)
{
    kWorkload.m_uiInputCount = m_pkTask->GetInputCount();
    kWorkload.m_uiOutputCount = m_pkTask->GetOutputCount();
    kWorkload.m_uiBlockCount = GetTransferBlockCount();
    kWorkload.m_uiRangeStart = GetBlockIndex();
    
    // Add input streams to workload
    NiUInt32 uiInputStreamCount = kWorkload.m_uiInputCount;
    for (NiUInt32 uiIndex = 0; uiIndex < uiInputStreamCount; ++uiIndex)
    {
        NiSPStream* pStream = m_pkTask->GetInputAt(uiIndex);
        NiSPWorkload::Header& kHeader = kWorkload.m_kInputs[uiIndex];
        kHeader.m_pvEffectiveAddress = 0;
        kHeader.m_uiBlockCount = 0;
        kHeader.m_uiStride = pStream->GetStride();
        kHeader.m_uiDataOffset = 0;
        kHeader.m_uiReserved = 0;
    }

    // Add output streams to workload
    NiUInt32 uiOutputCount = kWorkload.m_uiOutputCount;
    for (NiUInt32 uiIndex = 0; uiIndex < uiOutputCount; ++uiIndex)
    {
        NiSPStream* pStream = m_pkTask->GetOutputAt(uiIndex);
        NiSPWorkload::Header& kHeader = kWorkload.m_kOutputs[uiIndex];
        kHeader.m_pvEffectiveAddress = 0;
        kHeader.m_uiBlockCount = 0;
        kHeader.m_uiStride = pStream->GetStride();
        kHeader.m_uiDataOffset = 0;
        kHeader.m_uiReserved = 0;
    }
}
//---------------------------------------------------------------------------
NiUInt32 NiSPStreamPartitioner::PartitionInput(NiSPJobList* pkJob, 
    NiUInt32 uiJobIndex, NiSPStream* pkStream, 
    NiSPWorkload::Header& kWorkloadHeader)
{
    // If partitioning enabled then partition the stream.
    // Otherwise, send the entire stream contents
    NiUInt32 uiBytes = 0;
    if (pkStream->IsPartitioningEnabled())
    {
        kWorkloadHeader.m_uiBlockCount = GetTransferBlockCount();
        kWorkloadHeader.m_pvEffectiveAddress = 
            pkStream->GetBlockStartPointer(m_uiCurrentBlockIndex);
        uiBytes = pkStream->GetStride() * kWorkloadHeader.m_uiBlockCount;
    }
    else
    {
        kWorkloadHeader.m_uiBlockCount = pkStream->GetBlockCount();
        kWorkloadHeader.m_pvEffectiveAddress = pkStream->GetData();
        uiBytes = pkStream->GetDataSize();
    }
    return uiBytes;
}
//---------------------------------------------------------------------------
NiUInt32 NiSPStreamPartitioner::PartitionOutput(NiSPJobList* pkJob, 
   NiUInt32 uiJobIndex, NiSPStream* pStream, 
   NiSPWorkload::Header& kWorkloadHeader)
{
    kWorkloadHeader.m_pvEffectiveAddress = 
        pStream->GetBlockStartPointer(m_uiCurrentBlockIndex);
    kWorkloadHeader.m_uiBlockCount = GetTransferBlockCount();
    return pStream->GetStride() * kWorkloadHeader.m_uiBlockCount;
}
