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
#include "NiSPTask.h"
#include "NiSPStream.h"
#include "NiSPKernel.h"
//---------------------------------------------------------------------------
NiUInt32 NiSPTask::ms_uiNextTaskId = 1;
//---------------------------------------------------------------------------
NiSPTask::NiSPTask() : 
    m_uiTaskId(0),
    m_uiWorkflowId(0),
    m_pkKernel(NULL),
    m_uiSliceSize(0),
    m_eStatus(IDLE),
    m_uiSyncData(0),
    m_uiOptimalBlockCount(1024),
    m_usStage(0),
    m_bIsMarked(false),
    m_bIsRoot(false),
    m_bIsLeaf(false),
    m_bIsAligned(false),
    m_bIsCached(false)
{
    m_uiTaskId = ms_uiNextTaskId++;
    NIASSERT(m_uiTaskId);
}
//---------------------------------------------------------------------------
NiSPTask::~NiSPTask()
{
}
//---------------------------------------------------------------------------
void NiSPTask::Prepare()
{
    if (m_bIsCached)
        return;

    // If there are no streams then this is a sync task
    NiUInt32 uiCount = GetTotalCount();
    m_bIsSync = (uiCount == 0);

    // Track how many streams are aligned
    NiUInt32 uiMisalignedStreamCount = uiCount;
    
    // Prepare input streams tracking alignment, and slice size
    NiUInt32 uiInputStreamCount = m_kInputStreams.GetSize();
    NiUInt32 uiRootStreamCount = uiInputStreamCount;
    for (NiUInt32 uiIndex = 0; uiIndex < uiInputStreamCount; ++uiIndex)
    {
        NiSPStream* pStream = m_kInputStreams.GetAt(uiIndex);
        pStream->Prepare();
        if (pStream->IsDataAligned())
        {
            uiMisalignedStreamCount--;
        }
        
        if (pStream->GetOutputSize() == 0)
        {
            uiRootStreamCount--;
        }
        
        m_uiSliceSize += pStream->GetStride();
    }
    m_bIsRoot = !uiRootStreamCount;

    // Prepare output streams tracking alignment, and slice size
    NiUInt32 uiOutputStreamCount = m_kOutputStreams.GetSize();
    NiUInt32 uiLeafStreamCount = uiOutputStreamCount;
    for (NiUInt32 uiIndex = 0; uiIndex < uiOutputStreamCount; uiIndex++)
    {
        NiSPStream* pStream = m_kOutputStreams.GetAt(uiIndex);
        pStream->Prepare();
        if (pStream->IsDataAligned())
        {
            uiMisalignedStreamCount--;
        }

        if (pStream->GetInputSize() == 0)
        {
            uiLeafStreamCount--;
        }
        m_uiSliceSize += pStream->GetStride();
    }
    m_bIsLeaf = !uiLeafStreamCount;

    // Save alignment
    m_bIsAligned = !uiMisalignedStreamCount;
    
    // Initialize the task implementation
    m_kImpl.Initialize(this);

    // Set status to pending
    SetStatus(PENDING); 
}
//---------------------------------------------------------------------------
