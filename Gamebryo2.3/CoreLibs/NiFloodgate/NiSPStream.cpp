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
#include "NiSPStream.h"
#include "NiSPTask.h"
#include "NiSPAlgorithms.h"

//---------------------------------------------------------------------------
NiSPStream::NiSPStream(void* pvData, NiUInt32 uiStride, 
    NiUInt32 uiBlockCount, bool bPartitioningEnabled) :
        m_pvData(pvData), 
        m_pvAlignedData(0),
        m_uiDataSize(0), 
        m_uiBlockCount(uiBlockCount),
        m_uiStride(uiStride),
        m_bPartitioningEnabled(bPartitioningEnabled)
{
}
//---------------------------------------------------------------------------
NiSPStream::~NiSPStream()
{
}
//---------------------------------------------------------------------------
void NiSPStream::Prepare()
{
    NIASSERT(m_uiStride > 0 && "Error: Stride was zero. Stride must be > 0");
    if (m_uiStride <= 0)
    {
        NILOG("Error: Stride was zero. Stride must be > 0");
        return;
    }

    m_pvAlignedData = 
        reinterpret_cast<void*>(NiSPAlgorithms::PreAlign128(m_pvData));

    // Compute size of buffer
    m_uiDataSize = m_uiBlockCount * m_uiStride;

    // Determine the number of blocks that fit in a 128 aligned region
    m_uiSegmentBlockCount = ComputeLCM(128, m_uiStride) / m_uiStride;
}
//---------------------------------------------------------------------------
void* NiSPStream::GetBlockStartPointer(NiUInt32 uiBlockIndex)
{
    if (uiBlockIndex >= m_uiBlockCount)
        return 0;

    if (m_pvData == NULL)
        return NULL;

    return ((unsigned char*)(m_pvData)) + uiBlockIndex * m_uiStride;
}
//---------------------------------------------------------------------------
void* NiSPStream::GetBlockStartPointerAligned(NiUInt32 uiBlockIndex)
{
    if (uiBlockIndex >= m_uiBlockCount)
        return 0;

    if (m_pvData == NULL)
        return NULL;

    // Convert to unsigned char for pointer math
    return ((unsigned char*)(m_pvAlignedData)) + uiBlockIndex * m_uiStride;
}

//---------------------------------------------------------------------------
inline NiUInt32 NiSPStream::ComputeLCM(NiUInt32 uiInputA, NiUInt32 uiInputB)
{
    return (uiInputA * uiInputB) / ComputeGCD(uiInputA, uiInputB);
}
//---------------------------------------------------------------------------
void NiSPStream::NotifyDependentTasks()
{
    NiUInt32 uiInputCount = GetInputSize();
    for (NiUInt32 i = 0; i < uiInputCount; i++)
    {
        NiSPTask* pkTask = GetInputAt(i);
        pkTask->SetIsCached(false);
    }
    NiUInt32 uiOutputCount = GetOutputSize();
    for (NiUInt32 i = 0; i < uiOutputCount; i++)
    {
        NiSPTask* pkTask = GetOutputAt(i);
        pkTask->SetIsCached(false);
    }
}
//---------------------------------------------------------------------------
#define IS_EVEN(x) ((x & 1) == 0)
inline NiUInt32 NiSPStream::ComputeGCD(NiUInt32 uiInputA, NiUInt32 uiInputB)
{
    // Binary GCD: 
    // See http://www.nist.gov/dads/HTML/binaryGCD.html for algorithm.

    // Early out if either input is 0
    if (uiInputA == 0)
        return uiInputB;
    if (uiInputB == 0)
        return uiInputA;
    
    // Remove common factors of 2, saving number of factors
    NiUInt32 uiPowersOfTwo = 0;
    while (IS_EVEN(uiInputA) && IS_EVEN(uiInputB)) 
    { 
        uiInputA >>= 1;  
        uiInputB >>= 1;  
        uiPowersOfTwo++;             
    }
    
    do 
    {
        if (IS_EVEN(uiInputA))
            uiInputA >>= 1;
        else if (IS_EVEN(uiInputB))
            uiInputB >>= 1;
        else if (uiInputA >= uiInputB)
            uiInputA = (uiInputA-uiInputB) >> 1;
        else
            uiInputB = (uiInputB-uiInputA) >> 1;
    } 
    while (uiInputA > 0);
    return uiInputB << uiPowersOfTwo;  
}
