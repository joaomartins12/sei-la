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
#include "NiCollisionPCH.h" // Precompiled header.

#include "NiCollisionWorkflow.h"

//---------------------------------------------------------------------------
#if defined(ENABLE_PARALLEL_PICK)
//---------------------------------------------------------------------------

#include <NiSPWorkflow.h>
#include <NiSPTask.h>
#include <NiPoint3.h>
#include "NiCollisionTraversals.h"
#include <vectormath/cpp/vectormath_aos.h>


using namespace Vectormath::Aos;
namespace NiCollisionUtils
{

//---------------------------------------------------------------------------
NiCollisionWorkflow* NiCollisionWorkflow::ms_kWorkflows = NULL;
NiUInt32 NiCollisionWorkflow::ms_uiCurrentWorkflow = 0;
//---------------------------------------------------------------------------
void NiCollisionWorkflow::InitializePools()
{
    assert(!ms_kWorkflows);
    ms_kWorkflows = NiNew NiCollisionWorkflow[MAX_WORKFLOWS];
}
//---------------------------------------------------------------------------
void NiCollisionWorkflow::ShutdownPools()
{
    assert(ms_kWorkflows);
    NiDelete [] ms_kWorkflows;
    ms_kWorkflows = NULL;
}
//---------------------------------------------------------------------------
NiCollisionWorkflow& NiCollisionWorkflow::GetCurrentWorkflow()
{
    NextWorkflow();
    return ms_kWorkflows[ms_uiCurrentWorkflow];
}
//---------------------------------------------------------------------------
void NiCollisionWorkflow::NextWorkflow()
{
    // Find the next available workflow
    NiSPWorkflow::Status eStatus = IDLE;
    for (;;)
    {
        ms_uiCurrentWorkflow++;
        ms_uiCurrentWorkflow %= MAX_WORKFLOWS;
        NiCollisionWorkflow& kFlow = ms_kWorkflows[ms_uiCurrentWorkflow];
        eStatus = kFlow.GetStatus();
        if (eStatus == IDLE)
            break;
        
        if (eStatus == COMPLETED)
        {
            NiStreamProcessor::Get()->Clear(&kFlow);
            break;
        }
        
        NiStreamProcessor::Get()->Poll(&kFlow);
    } 
}

//---------------------------------------------------------------------------
void NiCollisionWorkflow::FinishWorkflows(NiPick* pkPick)
{
    NiInt32 iIndex = MAX_WORKFLOWS - 1;
    do
    {
        NiCollisionWorkflow& kFlow = ms_kWorkflows[iIndex];
        if (pkPick == kFlow.m_pkPick)
        {
            NiSPWorkflow::Status eStatus = kFlow.GetStatus();
            if (eStatus == RUNNING || eStatus == PENDING) 
            {
                NiStreamProcessor::Get()->Wait(&kFlow);
                NiStreamProcessor::Get()->Clear(&kFlow);
            }
        }
    }
    while (--iIndex >= 0);
}
//---------------------------------------------------------------------------
NiCollisionWorkflow::NiCollisionWorkflow() 
    : m_pkTriShape(NULL)
    , m_kAllowBack_b(false)
    , m_kVertStartAddress_i(0)
    , m_usTriCount(0)
    , m_pkPick(NULL)
    , m_pkSyncTask(NULL)
    , m_kIndicies(NULL, 1)
    , m_kModelOrigin(&m_kModelOrigin_v, 1)
    , m_kModelDir(&m_kModelDir_v, 1)
    , m_kAllowBack(&m_kAllowBack_b, 1)
    , m_kFirstHitOnly(&m_kFirstHitOnly_b, 1)
    , m_kVertStartAddress(&m_kVertStartAddress_i, 1)
{
    // Setup workflow
    NiSPTask* pkTask = NULL;
    pkTask = AddNewTask();
    pkTask->SetIsCacheable(true);
    pkTask->SetKernel(&m_kIntersectTriangleKernel);
    
    // Add fixed inputs
    pkTask->AddInput(&m_kModelOrigin);
    pkTask->AddInput(&m_kModelDir);
    pkTask->AddInput(&m_kAllowBack);
    pkTask->AddInput(&m_kFirstHitOnly);
    pkTask->AddInput(&m_kVertStartAddress);
    
    // Add streaming inputs (indicies)
    m_kIndicies.SetStride(sizeof(unsigned short) * 3);
    pkTask->AddInput(&m_kIndicies);
    
    // Setup signal task to let us know when processing is complete
    m_pkSyncTask = AddNewTask();
    m_pkSyncTask->SetIsCacheable(true);
    m_pkSyncTask->SetKernel(&m_kSignalKernel);
    
}
//---------------------------------------------------------------------------
NiCollisionWorkflow::~NiCollisionWorkflow()
{
}
//---------------------------------------------------------------------------
void NiCollisionWorkflow::Initialize( 
    NiPoint3& kModelOrigin, NiPoint3& kModelDir, 
    bool bCull, bool bFirstHitOnly, unsigned short* pIndicies, 
    unsigned short usTriCount, NiUInt64 uiVertStartAddress, 
    NiTriShape* pkTriShape, NiPick* pkPick)
{
    m_pkPick = pkPick;
    m_pkTriShape = pkTriShape;
    m_usTriCount = usTriCount;

    // Set fixed origin
    m_kModelOrigin_v.setX(kModelOrigin.x); 
    m_kModelOrigin_v.setY(kModelOrigin.y); 
    m_kModelOrigin_v.setZ(kModelOrigin.z);
    
    // Set fixed direction
    m_kModelDir_v.setX(kModelDir.x); 
    m_kModelDir_v.setY(kModelDir.y); 
    m_kModelDir_v.setZ(kModelDir.z);

    // Set backface flag
    m_kAllowBack_b = !bCull;
    m_kFirstHitOnly_b = bFirstHitOnly;

    m_kVertStartAddress_i = uiVertStartAddress;

    // Set indicies
    m_kIndicies.SetData(pIndicies);
    m_kIndicies.SetBlockCount(usTriCount);
}
//---------------------------------------------------------------------------
void NiCollisionWorkflow::HandleSignal(NiUInt32 uiIndex)
{
    NiCollisionTraversals::FindIntersections_TriShapeFastPathEnd(
        *this, uiIndex);
}

}


//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
