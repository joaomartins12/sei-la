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

// Precompiled Header
#include "NiD3DRendererPCH.h"

#include "NiGeometryGroup.h"
#include "NiGeometryBufferData.h"
#include "NiVBSet.h"

//---------------------------------------------------------------------------
#if !defined(WIN32) && !defined(_XENON)
NiCriticalSection NiGeometryGroup::ms_kCritSec;
#endif //#if !defined(WIN32) && !defined(_XENON)
//---------------------------------------------------------------------------
NiGeometryGroup::NiGeometryGroup() 
{ 
#if !defined(WIN32) && !defined(_XENON)
    ms_kCritSec.Lock();
    m_uiRefCount = 0;
    ms_kCritSec.Unlock();
#else //#if !defined(WIN32) && !defined(_XENON)
    InterlockedExchange((LONG*)&m_uiRefCount, 0);
#endif //#if !defined(WIN32) && !defined(_XENON)

    m_pkD3DDevice = NULL;
}
//---------------------------------------------------------------------------
NiGeometryGroup::~NiGeometryGroup() 
{
    // Failing this assertion is almost _always_ caused by leaving objects
    // (that contain references to DX data) around when trying to destroy
    // the NiD3DRenderer.  If you are hitting this assertion, make sure
    // the entire scene, all NiGeometry objects, all NiScreenTexture objects,
    // and any other objects with references to DX data are destroyed before
    // attempting to delete the NiD3DRenderer.
    NIASSERT(m_uiRefCount == 0);

    if (m_pkD3DDevice)
    {
        NiD3DRenderer::ReleaseDevice(m_pkD3DDevice);
    }
}
//---------------------------------------------------------------------------
void NiGeometryGroup::AddObject(NiGeometryBufferData* pkBuffData)
{
#if !defined(WIN32) && !defined(_XENON)
    ms_kCritSec.Lock();
    m_uiRefCount++;
    ms_kCritSec.Unlock();
#else //#if !defined(WIN32) && !defined(_XENON)
    InterlockedIncrement((LONG*)&m_uiRefCount);
#endif //#if !defined(WIN32) && !defined(_XENON)
    pkBuffData->SetGeometryGroup(this);
}
//---------------------------------------------------------------------------
void NiGeometryGroup::RemoveObject(NiGeometryBufferData* pkBuffData)
{
    NIASSERT(pkBuffData);
    NIASSERT(m_uiRefCount > 0);
#if !defined(WIN32) && !defined(_XENON)
    ms_kCritSec.Lock();
    m_uiRefCount--;
    ms_kCritSec.Unlock();
#else //#if !defined(WIN32) && !defined(_XENON)
    InterlockedDecrement((LONG*)&m_uiRefCount);
#endif //#if !defined(WIN32) && !defined(_XENON)
    pkBuffData->SetGeometryGroup(NULL);

    unsigned int uiStreamCount = pkBuffData->GetStreamCount();
    for (unsigned int i = 0; i < uiStreamCount; i++)
    {
        ReleaseChip(pkBuffData, i);
    }

    pkBuffData->RemoveIB();
}
//---------------------------------------------------------------------------

