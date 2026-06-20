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

#include "NiExporterOptionsSharedData.h"
#include "NiScriptInfo.h"
NiImplementRTTI(NiExporterOptionsSharedData, NiSharedData);

//---------------------------------------------------------------------------
NiExporterOptionsSharedData::NiExporterOptionsSharedData()
{
    m_spCurrentScript = NULL;
    m_eTargetRenderer = RENDER_UNKNOWN;
    m_bViewerRun = false;
    m_bViewerStripify = false;
    m_bWriteResultsToLog = false;
}
//---------------------------------------------------------------------------
NiExporterOptionsSharedData::~NiExporterOptionsSharedData()
{
}
//---------------------------------------------------------------------------
NiScriptInfo* NiExporterOptionsSharedData::GetCurrentScript()
{
    return m_spCurrentScript;
}
//---------------------------------------------------------------------------
void NiExporterOptionsSharedData::SetCurrentScript(NiScriptInfo* pkInfo)
{
    if (pkInfo != NULL)
        m_spCurrentScript = pkInfo;
}
//---------------------------------------------------------------------------
NiPluginToolkitRenderer NiExporterOptionsSharedData::GetTargetPlatform()
{
    return m_eTargetRenderer;
}
//---------------------------------------------------------------------------
void NiExporterOptionsSharedData::SetTargetPlatform(
    NiPluginToolkitRenderer eTargetRenderer)
{
    m_eTargetRenderer = eTargetRenderer;
}
//---------------------------------------------------------------------------
bool NiExporterOptionsSharedData::GetTargetPlatformLittleEndian()
{
    // Note: this should be filled in as the list of platforms changes
    switch(m_eTargetRenderer)
    {
    case RENDER_XENON:
    case RENDER_PS3:
    case RENDER_GAMECUBE:
        return false;
    case RENDER_DX8:
    case RENDER_PS2:
    case RENDER_XBOX:
    case RENDER_HURRICANE:
    case RENDER_DX9:
    case RENDER_GENERIC:
    case RENDER_D3D10:
        return true;
    default:
        NIASSERT(!"NiPluginToolkitRenderer endianness undefined.");
        return true;
    }
}
//---------------------------------------------------------------------------
bool NiExporterOptionsSharedData::GetViewerRun()
{
    return m_bViewerRun;
}
//---------------------------------------------------------------------------
void NiExporterOptionsSharedData::SetViewerRun(bool bViewerRun)
{
    m_bViewerRun = bViewerRun;
}
//---------------------------------------------------------------------------
bool NiExporterOptionsSharedData::GetStripifyForViewer()
{
    return m_bViewerStripify;
}
//---------------------------------------------------------------------------
void NiExporterOptionsSharedData::SetStripifyForViewer(bool bViewerStrip)
{
    m_bViewerStripify = bViewerStrip;
}
//---------------------------------------------------------------------------
bool NiExporterOptionsSharedData::GetWriteResultsToLog()
{
    return m_bWriteResultsToLog;
}
//---------------------------------------------------------------------------
void NiExporterOptionsSharedData::SetWriteResultsToLog(
    bool bWriteResultsToLog)
{
    m_bWriteResultsToLog = bWriteResultsToLog;
}
//---------------------------------------------------------------------------
NiXMLLogger* NiExporterOptionsSharedData::GetXMLLogger()
{
    return m_pkLogger;
}
//---------------------------------------------------------------------------
void NiExporterOptionsSharedData::SetXMLLogger(NiXMLLogger* pkLogger)
{
    m_pkLogger = pkLogger;
}
//---------------------------------------------------------------------------
