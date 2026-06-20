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
#include "NiSystemPCH.h"

#include "NiInitOptions.h"
#include "NiMemTracker.h"
#include "NiStandardAllocator.h"

//---------------------------------------------------------------------------
NiInitOptions::NiInitOptions()
{
#ifdef NI_MEMORY_DEBUGGER
    m_pkAllocator = NiExternalNew NiMemTracker(
        NiExternalNew NiStandardAllocator());
#else
    m_pkAllocator = NiExternalNew NiStandardAllocator();
#endif
    m_bAllocatedInternally = true;

#if defined (_PS3)
    m_bInitSpuPrintServer = true;
    m_uiSpuCount = 2;
#endif

}
//---------------------------------------------------------------------------
NiInitOptions::NiInitOptions(NiAllocator* pkAllocator) : 
    m_pkAllocator(pkAllocator), m_bAllocatedInternally(false)
{
    NIASSERT(m_pkAllocator);
#if defined (_PS3)
    m_bInitSpuPrintServer = true;
    m_uiSpuCount = 2;
#endif

}
//---------------------------------------------------------------------------
NiInitOptions::~NiInitOptions()
{
    if (m_bAllocatedInternally)
        NiExternalDelete m_pkAllocator;
}
//---------------------------------------------------------------------------
NiAllocator* NiInitOptions::GetAllocator() const
{
    return m_pkAllocator;
}
//---------------------------------------------------------------------------
#if defined (_PS3)
bool NiInitOptions::GetInitSpuPrintServer() const
{
    return m_bInitSpuPrintServer;
}
//---------------------------------------------------------------------------
void NiInitOptions::SetInitSpuPrintServer(bool bInitSpuPrintServer)
{
    m_bInitSpuPrintServer = bInitSpuPrintServer;
}
//---------------------------------------------------------------------------
unsigned int NiInitOptions::GetSpuCount() const
{
    return m_uiSpuCount;
}
//---------------------------------------------------------------------------
void NiInitOptions::SetSpuCount(unsigned int uiSpuCount)
{
    m_uiSpuCount = uiSpuCount;
}

#endif
//---------------------------------------------------------------------------

