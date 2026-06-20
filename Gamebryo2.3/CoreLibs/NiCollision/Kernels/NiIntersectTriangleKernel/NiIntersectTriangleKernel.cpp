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
#if defined (_PS3)
//---------------------------------------------------------------------------
#include "NiIntersectTriangleKernel.h"
#include <vectormath/cpp/vectormath_aos.h>
#include <NiUniversalTypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <spu_intrinsics.h> 
using namespace Vectormath::Aos;
//---------------------------------------------------------------------------
struct NiPoint3 { float x, y, z; };     // Mirrors PPU version of NiPoint3
static const uint32_t CACHE_SIZE = 128;
static const uint32_t BUFFER_SIZE = 128;
struct Cache
{
    NiInt32 iIndex[CACHE_SIZE];
    Vector3  kVert[CACHE_SIZE];
};

volatile static char aBuffer1a[BUFFER_SIZE]  __attribute__ ((aligned(128)));
volatile static char aBuffer2a[BUFFER_SIZE]  __attribute__ ((aligned(128)));
volatile static char aBuffer3a[BUFFER_SIZE]  __attribute__ ((aligned(128)));

volatile static char aBuffer1b[BUFFER_SIZE]  __attribute__ ((aligned(128)));
volatile static char aBuffer2b[BUFFER_SIZE]  __attribute__ ((aligned(128)));
volatile static char aBuffer3b[BUFFER_SIZE]  __attribute__ ((aligned(128)));

volatile static char aBuffer1c[BUFFER_SIZE]  __attribute__ ((aligned(128)));
volatile static char aBuffer2c[BUFFER_SIZE]  __attribute__ ((aligned(128)));
volatile static char aBuffer3c[BUFFER_SIZE]  __attribute__ ((aligned(128)));

volatile static char aCacheBuffer[512] __attribute__ ((aligned(128)));
volatile static char aIndexBuffer[512] __attribute__ ((aligned(128)));
 
static float* fp1[3] __attribute__ ((aligned(128)));
static float* fp2[3] __attribute__ ((aligned(128)));
static float* fp3[3] __attribute__ ((aligned(128)));
static uint32_t e1[3] __attribute__ ((aligned(128)));
static uint32_t e2[3] __attribute__ ((aligned(128)));
static uint32_t e3[3] __attribute__ ((aligned(128)));
static uint32_t Tag[3] __attribute__ ((aligned(128))); 
static bool bNeedWait[3] __attribute__ ((aligned(128)));


static inline void  StartTriangle(uint32_t i, Cache& cache, NiInt32 i0, 
    NiInt32 i1, NiInt32 i2, NiPoint3* pVertEA)
{
    fp1[i] = 0;
    fp2[i] = 0;
    fp3[i] = 0;

    e1[i] = i0 % CACHE_SIZE;
    e2[i] = i1 % CACHE_SIZE;
    e3[i] = i2 % CACHE_SIZE;
    bNeedWait[i] = false;

    if (cache.iIndex[e1[i]] != i0)
    {
        // Item not found in cache so load it
        volatile char *pBuffer = 
            i == 0 ? aBuffer1a : i == 1 ? aBuffer1b : aBuffer1c;
        uint64_t uiVertAddress = (uint64_t)&pVertEA[i0];
        uint64_t eaAligned = (uiVertAddress & ~0xF);
        cellDmaGet((void*)pBuffer, eaAligned, BUFFER_SIZE, Tag[i], 0, 0);
        
        uint32_t extra = uiVertAddress - eaAligned;
        fp1[i] = (float*)(pBuffer + extra);
        cache.iIndex[e1[i]] = i0;
        bNeedWait[i] = true;
    }
    
    if (cache.iIndex[e2[i]] != i1)
    {
        // Item not found in cache so load it
        volatile char *pBuffer = 
            i == 0 ? aBuffer2a : i == 1 ? aBuffer2b : aBuffer2c;
        uint64_t uiVertAddress = (uint64_t)&pVertEA[i1];
        uint64_t eaAligned = (uiVertAddress & ~0xF);
        cellDmaGet((void*)pBuffer, eaAligned, BUFFER_SIZE, Tag[i], 0, 0);
    
        uint32_t extra = uiVertAddress - eaAligned;
        fp2[i] = (float*)(pBuffer + extra);
        cache.iIndex[e2[i]] = i1;
        bNeedWait[i] = true;
    }
    
    if (cache.iIndex[e3[i]] != i2)
    {
        // Item not found in cache so load it
        volatile char *pBuffer = 
            i == 0 ? aBuffer3a : i == 1 ? aBuffer3b : aBuffer3c;
        uint64_t uiVertAddress = (uint64_t)&pVertEA[i2];
        uint64_t eaAligned = (uiVertAddress & ~0xF);
        cellDmaGet((void*)pBuffer, eaAligned, BUFFER_SIZE, Tag[i], 0, 0);
    
        uint32_t extra = uiVertAddress - eaAligned;
        fp3[i] = (float*)(pBuffer + extra);
        cache.iIndex[e3[i]] = i2;
        bNeedWait[i] = true;
    }    
 
}
static inline void FinishTriangle(uint32_t i, Cache& cache)
{
    if (bNeedWait[i])
    {
        cellDmaWaitTagStatusAll(1<<(Tag[i]));
    
        if (fp1[i])
        {
            float *fp = fp1[i];
            uint32_t k = e1[i];
            cache.kVert[k].setX(*fp++);
            cache.kVert[k].setY(*fp++);
            cache.kVert[k].setZ(*fp++);
        }
        if (fp2[i])
        {
            float *fp = fp2[i];
            uint32_t k = e2[i];
            cache.kVert[k].setX(*fp++);
            cache.kVert[k].setY(*fp++);
            cache.kVert[k].setZ(*fp++);
        }
        if (fp3[i])
        {
            float *fp = fp3[i];
            uint32_t k = e3[i];
            cache.kVert[k].setX(*fp++);
            cache.kVert[k].setY(*fp++);
            cache.kVert[k].setZ(*fp++);
        }
    }
}
//---------------------------------------------------------------------------
NiSPBeginKernelImpl(NiIntersectTriangleKernel)
{
    // Constants
    const float fTolerance = 1e-05f;
    Tag[0] = 26;
    Tag[1] = 27;
    Tag[2] = 28;

    // Inputs
    Vector3 kOrigin = *kWorkload.GetInput<Vector3>(0);
    Vector3 kDir = *kWorkload.GetInput<Vector3>(1);
    bool bAllowBack = *kWorkload.GetInput<bool>(2);
    bool bFirstHitOnly = *kWorkload.GetInput<bool>(3);
    NiUInt64 uiVertEA = *kWorkload.GetInput<NiUInt64>(4);
    unsigned short* pusIndicies = kWorkload.GetInput<unsigned short>(5);
    
    // Outputs
    uintptr_t uiArgsAddress=(uintptr_t) &m_pkJob->workArea;
    uiArgsAddress += m_pkJob->header.sizeDmaList;
    uiArgsAddress=(uiArgsAddress + 0xf) & ~0xf;
    
    uint32_t* puiArgs = reinterpret_cast<uint32_t*>(uiArgsAddress);
    //uint32_t uiSignalType = puiArgs[0];
    uint32_t uiPort = puiArgs[1];
    uint32_t uiId = puiArgs[2];

    // Block Counts
    unsigned int uiBlockCount = kWorkload.GetBlockCount();

    // Load cache
    NiPoint3* pVertEA = (NiPoint3*)uiVertEA;
    Cache cache;
    memset(&cache.iIndex, -1, CACHE_SIZE * sizeof(NiInt32));
    memset(&cache.kVert, 0, CACHE_SIZE * sizeof(Vector3));

    uint32_t uiBufferIndex = 0;
    
    unsigned short i0, i1, i2;
    i0 = pusIndicies[0];
    i1 = pusIndicies[1];
    i2 = pusIndicies[2];
    StartTriangle(0, cache, i0, i1, i2, pVertEA);

    if (uiBlockCount > 1)
    {
        i0 = pusIndicies[3];
        i1 = pusIndicies[4];
        i2 = pusIndicies[5];
        StartTriangle(1, cache, i0, i1, i2, pVertEA);
    }

    if (uiBlockCount > 2)
    {
        i0 = pusIndicies[6];
        i1 = pusIndicies[7];
        i2 = pusIndicies[8];
        StartTriangle(2, cache, i0, i1, i2, pVertEA);
    }

    // Process each triangle
    unsigned int i = 0;
    do
    {
        // DMA over verts
        FinishTriangle(uiBufferIndex, cache);
        Vector3 &kV1 = cache.kVert[e1[uiBufferIndex]];
        Vector3 &kV2 = cache.kVert[e2[uiBufferIndex]];
        Vector3 &kV3 = cache.kVert[e3[uiBufferIndex]];

        bool last = (i >= (uiBlockCount - 3));
        if (!last)
        {
            unsigned int j0 = 3 * (i + 3);
            unsigned int j1 = j0 + 1;
            unsigned int j2 = j1 + 1;
            i0 = pusIndicies[j0];
            i1 = pusIndicies[j1];
            i2 = pusIndicies[j2];
            StartTriangle(uiBufferIndex , cache, i0, i1, i2, pVertEA);
        }
        uiBufferIndex = (uiBufferIndex + 1) % 3;

        bool hit = false;
        Vector3 kEdge1 = kV2 - kV1;
        Vector3 kEdge2 = kV3 - kV1;
        Vector3 kPt = cross(kDir, kEdge2);

        float s = 0.0f;
        float t = 0.0f;
        float r = 0.0f;
        float fDet = dot(kEdge1, kPt);

        bool b1 = fDet >= fTolerance;
        bool b5 = fDet <= -fTolerance && bAllowBack;
        
        if (b1)
        {
            Vector3 kS = kOrigin - kV1;
            s = dot(kS,  kPt);
            
            bool b2 = s < 0.0f || s > fDet;
            if (!b2)
            {
                Vector3 q = cross(kS, kEdge1);
                t = dot(kDir, q);
                bool b3 = t < 0.0f || s + t > fDet;
                if (!b3)
                {
                    r = dot(kEdge2, q);
                    bool b4 = r < 0.0f;
                    hit = !b4;
                }
            }
        }
        else if (b5)
        {
            Vector3 kS = kOrigin - kV1;
            s = dot(kS,  kPt);
            bool b6 = s > 0.0f || s < fDet;
            if (!b6)
            {
                Vector3 q = cross(kS, kEdge1);
                t = dot(kDir, q);
                bool b7 = t > 0.0f || s + t < fDet;
                if (!b7)
                {
                    r = dot(kEdge2, q);
                    bool b8 = r > 0.0f;
                    hit = !b8;
                }
            }
        }
        if (hit)
        {
           NiUInt32 uiHitIndex = m_pkWorkload->m_uiRangeStart + i;
           
           // avoid live lock situation if it occurs
           int z = 10000;
           while (sys_spu_thread_send_event(uiPort, uiId, 
               uiHitIndex) != CELL_OK)
           {
                while(z--){}
            }
            if (bFirstHitOnly)
                break;
        }
        i++;
    }
    while (i < uiBlockCount);
}
#else
NiSPBeginKernelImpl(NiIntersectTriangleKernel)
{
}
#endif

NiSPEndKernelImpl(NiIntersectTriangleKernel)
