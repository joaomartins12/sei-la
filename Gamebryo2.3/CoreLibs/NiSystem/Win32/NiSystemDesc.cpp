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

#include "NiSystemDesc.h"
#include <Windows.h>

//---------------------------------------------------------------------------
NiSystemDesc* NiSystemDesc::ms_pkSystemDesc = NULL;
//---------------------------------------------------------------------------
void NiSystemDesc::InitSystemDesc()
{
    NIASSERT(ms_pkSystemDesc == NULL);
    ms_pkSystemDesc = NiNew NiSystemDesc();
}
//---------------------------------------------------------------------------
void NiSystemDesc::ShutdownSystemDesc()
{
    NiDelete ms_pkSystemDesc;
    ms_pkSystemDesc = NULL;
}
//---------------------------------------------------------------------------
NiSystemDesc::NiSystemDesc()
{
    SYSTEM_INFO kInfo;
    GetSystemInfo(&kInfo);
    m_uiNumLogicalProcessors = kInfo.dwNumberOfProcessors;

    LARGE_INTEGER ticksPerSec;
    QueryPerformanceFrequency(&ticksPerSec);
    m_fPCCyclesPerSecond = (float)ticksPerSec.QuadPart;
}
//---------------------------------------------------------------------------
NiSystemDesc::NiSystemDesc(const NiSystemDesc&)
{
    NIASSERT(!"This method should never be called");
}
//---------------------------------------------------------------------------


