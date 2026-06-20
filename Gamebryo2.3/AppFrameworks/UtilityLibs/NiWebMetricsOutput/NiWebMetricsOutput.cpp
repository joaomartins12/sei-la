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

#if NIMETRICS
    // Metrics Probe Library-specific macro
    #define METRICS_ENABLED
#endif // #if NIMETRICS

// The metrics probe library does not respect VC80's secure functions
// and so to disable the warnings, we will utilize a VC80-specific
// define for this file.
#if defined(WIN32) && (_MSC_VER == 1400)
    #undef _CRT_SECURE_NO_DEPRECATE
    #define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include "NiWebMetricsOutput.h"
#include "NiMetricsTimer.h"
#include "NiMetricsProbe.h"
#include <exception>

// A note about exceptions, because the metrics probe library utilizes them:
//
// We will catch both std::exception and everything else separately
// so that in the case that a std::exception is thrown while debugging,
// kEx can be inspected.  In either case, make sure no exceptions
// propagate out of any function, given that throwing an exception
// to non-exception handling code causes that program to immediately
// exit.

//---------------------------------------------------------------------------
void NiWebMetricsOutput::Init(const char* pcLogURI, 
    const char* pcHostName,
    const char* pcAppName, const char* pcModuleName, 
    const char* pcReferenceName,
    int iFlushInterval, bool bStrictTime, 
    cIEmeMetricsProbeListener* pkListener)
{
    cEmeMetricsProbeSettings kInit;

    // Add logic to allow const char* parameters to be NULL for convenience.
    if (pcLogURI)
    {
        NiStrcpy(kInit.logUri, METRICS_PROBE_SETTINGS_BUFFER_LEN, pcLogURI);
    }
    else
    {
        NiStrcpy(kInit.logUri, METRICS_PROBE_SETTINGS_BUFFER_LEN, "");
    }

    if (pcHostName)
    {
        NiStrcpy(kInit.hostName, METRICS_PROBE_SETTINGS_BUFFER_LEN, 
            pcHostName);
    }
    else
    {
        NiStrcpy(kInit.hostName, METRICS_PROBE_SETTINGS_BUFFER_LEN, "");
    }

    if (pcAppName)
    {
        NiStrcpy(kInit.appName, METRICS_PROBE_SETTINGS_BUFFER_LEN, pcAppName);
    }
    else
    {
        NiStrcpy(kInit.appName, METRICS_PROBE_SETTINGS_BUFFER_LEN, "");
    }

    if (pcModuleName)
    {
        NiStrcpy(kInit.moduleName, METRICS_PROBE_SETTINGS_BUFFER_LEN, 
            pcModuleName);
    }
    else
    {
        NiStrcpy(kInit.moduleName, METRICS_PROBE_SETTINGS_BUFFER_LEN, "");
    }

    if (pcReferenceName)
    {
        NiStrcpy(kInit.referenceName, METRICS_PROBE_SETTINGS_BUFFER_LEN,
            pcReferenceName);
    }
    else
    {
        NiStrcpy(kInit.referenceName, METRICS_PROBE_SETTINGS_BUFFER_LEN, "");
    }

    kInit.flushInterval = iFlushInterval;
    kInit.strictTime = bStrictTime;
    kInit.listener = pkListener;

    try
    {
        METRICS_PROBE_INIT(kInit);
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in METRICS_PROBE_INIT");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT("!Exception in METRICS_PROBE_INIT");
    }
}
//---------------------------------------------------------------------------
void NiWebMetricsOutput::Deinit()
{
    try
    {
        METRICS_PROBE_DEINIT;
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in METRICS_PROBE_DEINIT");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in METRICS_PROBE_DEINIT");
    }
}
//---------------------------------------------------------------------------
void NiWebMetricsOutput::AddValue(const char* pcName, float fValue)
{
    try
    {
        METRICS_PROBE_METRIC(pcName, fValue);
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in METRICS_PROBE_METRIC");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in METRICS_PROBE_METRIC");
    }
}
//---------------------------------------------------------------------------
void NiWebMetricsOutput::EndTimer(NiMetricsTimer& kTimer)
{
    try
    {
        METRICS_PROBE_METRIC(kTimer.GetName(), 
            kTimer.GetElapsed());
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in METRICS_PROBE_METRIC");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in METRICS_PROBE_METRIC");
    }
}
//---------------------------------------------------------------------------
void NiWebMetricsOutput::AddImmediateEvent(const char* pcName, float fValue)
{
    try
    {
        METRICS_PROBE_IMMEDIATE(pcName, fValue);
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in METRICS_PROBE_IMMEDIATE");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in METRICS_PROBE_IMMEDIATE");
    }
}
//---------------------------------------------------------------------------
void NiWebMetricsOutput::Update()
{
    try
    {
        METRICS_PROBE_TICK;
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in METRICS_PROBE_TICK");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in METRICS_PROBE_TICK");
    }
}
//---------------------------------------------------------------------------
void NiWebMetricsOutput::Flush()
{
    try
    {
        METRICS_PROBE_FLUSH;
    }
    catch(std::exception kEx)
    {
        NIASSERT("!Exception in METRICS_PROBE_FLUSH");
        NiOutputDebugString(kEx.what());
    }
    catch(...)
    {
        NIASSERT(!"Exception in METRICS_PROBE_FLUSH");
    }
}
//---------------------------------------------------------------------------
