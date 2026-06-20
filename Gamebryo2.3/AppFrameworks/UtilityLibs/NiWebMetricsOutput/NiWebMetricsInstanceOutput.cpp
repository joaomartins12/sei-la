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

#include "NiWebMetricsInstanceOutput.h"
#include "NiMetricsTimer.h"
#include "NiMetricsProbe.h"
#include "NiCriticalSection.h"
#include <exception>

// The metrics probe library does not respect VC80's secure functions
// and so to disable the warnings, we will utilize a VC80-specific
// define for this file.
#if defined(WIN32) && (_MSC_VER == 1400)
#undef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

NiMetricsProbe* NiWebMetricsInstanceOutput::ms_pkInstance = NULL;

//---------------------------------------------------------------------------
NiWebMetricsInstanceOutput::NiWebMetricsInstanceOutput(const char* pcLogURI, 
    const char* pcHostName,
    const char* pcAppName, const char* pcModuleName, 
    const char* pcReferenceName,
    int iFlushInterval, bool bStrictTime, 
    cIEmeMetricsProbeListener* pkListener)
{
    try
    {
        // Only one instance should ever be created.
        static NiFastCriticalSection kSection;
        kSection.Lock();
        {
            NIASSERT(!ms_pkInstance);
            ms_pkInstance = NiNew NiMetricsProbe;
        }
        kSection.Unlock();

        cEmeMetricsProbe::setInstance(ms_pkInstance);
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in cEmeMetricsProbe::Instance()");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in cEmeMetricsProbe::Instance()");
    }

    Init(pcLogURI, pcHostName, pcAppName, pcModuleName, pcReferenceName,
        iFlushInterval, bStrictTime, pkListener);
}
//---------------------------------------------------------------------------
NiWebMetricsInstanceOutput::~NiWebMetricsInstanceOutput()
{
    Deinit();

    try
    {
        NIASSERT(ms_pkInstance);
        // No need for a critical section as we've been assured by
        // the constructor that this is only created once.
        NiDelete ms_pkInstance;
        cEmeMetricsProbe::setInstance(NULL);
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in cEmeMetricsProbe::setInstance(NULL)");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in cEmeMetricsProbe::setInstance(NULL)");
    }

    ms_pkInstance = NULL;
}
//---------------------------------------------------------------------------
void NiWebMetricsInstanceOutput::SetUploaderPath(const char* pcUploaderPath)
{
    try
    {
        ms_pkInstance->GetMetricsLog().setUploaderPath(pcUploaderPath);
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in SetUploaderPath");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in SetUploaderPath");
    }
}
//---------------------------------------------------------------------------
void NiWebMetricsInstanceOutput::SetUploaderHost(const char* pcUploaderHost)
{
    try
    {
        ms_pkInstance->GetMetricsLog().setUploaderHost(pcUploaderHost);
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in SetUploaderHost");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in SetUploaderHost");
    }
}
//---------------------------------------------------------------------------
void NiWebMetricsInstanceOutput::SetUploaderPort(int iUploaderPort)
{
    try
    {
        ms_pkInstance->GetMetricsLog().setUploaderPort(iUploaderPort);
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in SetUploaderPort");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in SetUploaderPort");
    }
}
//---------------------------------------------------------------------------
void NiWebMetricsInstanceOutput::SetUploaderUri(const char* pcUploaderUri)
{
    try
    {
        ms_pkInstance->GetMetricsLog().setUploaderUri(pcUploaderUri);
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in SetUploaderUri");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in SetUploaderUri");
    }
}
//---------------------------------------------------------------------------
void NiWebMetricsInstanceOutput::SetUploaderIni(const char* pcUploaderIni)
{
    try
    {
        ms_pkInstance->GetMetricsLog().setUploaderIni(pcUploaderIni);
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in SetUploaderIni");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in SetUploaderIni");
    }
}
//---------------------------------------------------------------------------
void NiWebMetricsInstanceOutput::SetUploaderSrcId(const char* pcUploaderSrcId)
{
    try
    {
        ms_pkInstance->GetMetricsLog().setUploaderSrcId(pcUploaderSrcId);
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in SetUploaderSrcId");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in SetUploaderSrcId");
    }
}
//---------------------------------------------------------------------------
void NiWebMetricsInstanceOutput::SetUploadOnClose(bool bUploadOnClose)
{
    try
    {
        ms_pkInstance->GetMetricsLog().setUploadOnClose(bUploadOnClose);
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in SetUploadOnClose");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in SetUploadOnClose");
    }
}
//---------------------------------------------------------------------------
const char* NiWebMetricsInstanceOutput::GetUploaderPath() const
{
    const char* pcRet = NULL;

    try
    {
        pcRet = ms_pkInstance->GetMetricsLog().getUploaderPath().c_str();
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in GetUploaderPath");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in GetUploaderPath");
    }

    return pcRet;
}
//---------------------------------------------------------------------------
const char* NiWebMetricsInstanceOutput::GetUploaderHost() const
{
    const char* pcRet = NULL;

    try
    {
        pcRet = ms_pkInstance->GetMetricsLog().getUploaderHost().c_str();
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in GetUploaderHost");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in GetUploaderHost");
    }

    return pcRet;
}
//---------------------------------------------------------------------------
int NiWebMetricsInstanceOutput::GetUploaderPort() const
{
    int iRet = ~0;

    try
    {
        iRet = ms_pkInstance->GetMetricsLog().getUploaderPort();
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in GetUploaderPort");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in GetUploaderPort");
    }

    return iRet;
}
//---------------------------------------------------------------------------
const char* NiWebMetricsInstanceOutput::GetUploaderUri() const
{
    const char* pcRet = NULL;

    try
    {
        pcRet = ms_pkInstance->GetMetricsLog().getUploaderUri().c_str();
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in getUploaderUri");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in getUploaderUri");
    }

    return pcRet;
}
//---------------------------------------------------------------------------
const char* NiWebMetricsInstanceOutput::GetUploaderIni() const
{
    const char* pcRet = NULL;

    try
    {
        pcRet = ms_pkInstance->GetMetricsLog().getUploaderIni().c_str();
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in GetUploaderIni");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in GetUploaderIni");
    }

    return pcRet;
}
//---------------------------------------------------------------------------
const char* NiWebMetricsInstanceOutput::GetUploaderSrcId() const
{
    const char* pcRet = NULL;

    try
    {
        pcRet = ms_pkInstance->GetMetricsLog().getUploaderSrcId().c_str();
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in GetUploaderSrcId");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in GetUploaderSrcId");
    }

    return pcRet;
}
//---------------------------------------------------------------------------
bool NiWebMetricsInstanceOutput::GetUploadOnClose() const
{
    bool bUploadOnClose = true;

    try
    {
        bUploadOnClose = ms_pkInstance->GetMetricsLog().getUploadOnClose();
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in GetUploaderPath");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in GetUploaderPath");
    }

    return bUploadOnClose;
}
//---------------------------------------------------------------------------
