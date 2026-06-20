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
#include "NiSPJob.h"
#include "NiSPKernel.h"
//---------------------------------------------------------------------------
NiTObjectPool<NiSPStreamPartitioner>* NiSPTaskImpl::ms_pkStreamPartPool = NULL;
//---------------------------------------------------------------------------
void NiSPTaskImpl::InitializePools(NiUInt32 uiPartitionPoolSize)
{
    NIASSERT(ms_pkStreamPartPool == NULL);
    ms_pkStreamPartPool = NiNew 
        NiTObjectPool<NiSPStreamPartitioner>(uiPartitionPoolSize);
    
    // Initialize JobList Pool
    NiSPJob::InitializePools(128);
}
//---------------------------------------------------------------------------
void NiSPTaskImpl::ShutdownPools()
{
    NiSPJob::ShutdownPools();

    NIASSERT(ms_pkStreamPartPool != NULL);
    NiDelete ms_pkStreamPartPool;
    ms_pkStreamPartPool = NULL;
}
//---------------------------------------------------------------------------
NiSPTaskImpl::NiSPTaskImpl() 
    : m_pkPartitioner(NULL)
{
}
//---------------------------------------------------------------------------
NiSPTaskImpl::~NiSPTaskImpl()
{
}
//---------------------------------------------------------------------------
void NiSPTaskImpl::Initialize(NiSPTask* pkTask)
{
    // If there are no misaligned streams then use the aligned stream 
    // partitioner; otherwise use the misaligned stream partitioner
    m_pkTask = pkTask;
    if (!m_pkTask->IsSync())
    {
        // Initialize the stream partitioner with this task
        m_pkPartitioner = ms_pkStreamPartPool->GetFreeObject();
        m_pkPartitioner->Initialize(m_pkTask);
    }

    // Initialize the job
    NiSPJob::Initialize(m_pkTask);
}
//---------------------------------------------------------------------------
void NiSPTaskImpl::Clear()
{
    NiSPJob::Clear();

    if (!m_pkPartitioner)
        return;

    ms_pkStreamPartPool->ReleaseObject(m_pkPartitioner);
    m_pkPartitioner = NULL;
}
//---------------------------------------------------------------------------
