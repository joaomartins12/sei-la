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

#include "stdafx.h"
#include "NiStripifyGeometryPlugin.h"
#include "NiStripifyGeometryDialog.h"
#include "NiStripifyDLLDefines.h"
#include <NiExporterOptionsSharedData.h>
#include <NiStripifyInterface.h>

NiImplementRTTI(NiStripifyGeometryPlugin, NiPlugin);
NiStripifyGeometryPlugin* NiStripifyGeometryPlugin::ms_pkThis = NULL;

//---------------------------------------------------------------------------
NiStripifyGeometryPlugin::NiStripifyGeometryPlugin() :
    NiPlugin("Stripify Geometry", "1.0", "turn geometry into tri strips",
    "Turn the Geometry into Tri Strips"), m_pkProgressBar(NULL)
{
}
//---------------------------------------------------------------------------
bool NiStripifyGeometryPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiStripifyGeometryPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiStripifyGeometryPlugin::PerObjectCallback(NiAVObject* pkObject)
{
    if (ms_pkThis != NULL && ms_pkThis->m_pkProgressBar != NULL)
    {
        ms_pkThis->m_pkProgressBar->SetLineTwo(pkObject->GetName());
        ms_pkThis->m_pkProgressBar->StepIt();
    }
    return true;
}
//---------------------------------------------------------------------------
void NiStripifyGeometryPlugin::ErrorCallback(unsigned int uiCode, 
    NiObject* pkObj, const char* pcMessage)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
    bool bWriteResultsToLog;
    NiXMLLogger* pkLogger;

    if (pkExporterSharedData)
    {
        bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
        pkLogger = pkExporterSharedData->GetXMLLogger();
    }

    if ((bWriteResultsToLog) && (pkLogger))
    {
        pkLogger->LogElement("StripificationWarning", pcMessage);
    }
    else
    {
        NiMessageBox(pcMessage, "Stripification Warning");
    }
}
//---------------------------------------------------------------------------
unsigned int NiStripifyGeometryPlugin::CountTriShapes(NiAVObject* pkObject)
{
    if (pkObject == NULL)
        return 0;

    if (NiIsKindOf(NiNode, pkObject))
    {
        unsigned int uiSum = 0;
        NiNode* pkNode = (NiNode*)pkObject;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            uiSum += CountTriShapes(pkChild);
        }
        return uiSum;
    }
    else if (NiIsKindOf(NiTriShape, pkObject))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiStripifyGeometryPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    ms_pkThis = this;
    // Get the Scene graph from the shared data
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));
    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    bool bViewerStrip = false;
    bool bViewerRun = false;
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));

    if (pkExporterSharedData)
    {
        bViewerRun = pkExporterSharedData->GetViewerRun();
        bViewerStrip = pkExporterSharedData->GetStripifyForViewer();
    }
    
    // Check the PluginInfo to double check the type
    if(pkInfo->GetType() != "PROCESS")
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    m_pkProgressBar = NiNew NiProgressDialog("Stripifying Scene Graph");
    m_pkProgressBar->Create();
    m_pkProgressBar->SetPosition(0);
    m_pkProgressBar->SetLineOne("Stripifying Geometry...");

    NiStripify::Init();
    NiStripify::SetFinishedCallback(PerObjectCallback);
    NiStripify::SetErrorCallback(ErrorCallback);
    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        m_pkProgressBar->StepIt();

        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);

        unsigned int uiSize = CountTriShapes(spScene);
        m_pkProgressBar->SetRangeSpan(uiSize + 1);
    
        NiPluginToolkitRenderer eRenderer = 
            pkSGSharedData->GetRootPlatform(spScene);

        NiString strStripifyType = STRIP_DO_NOT_STRIPIFY_VALUE;

        switch (eRenderer)
        {
            case RENDER_GENERIC:
            case RENDER_DX8:
            case RENDER_DX9:
                strStripifyType = pkInfo->GetValue(STRIP_PC_SETTINGS);
                break;
            case RENDER_XENON:
                strStripifyType = pkInfo->GetValue(
                    STRIP_NEXTGENXBOX_SETTINGS);
                break;
            default:
                strStripifyType = STRIP_DO_NOT_STRIPIFY_VALUE;
                break;
        }

        if (bViewerRun && !bViewerStrip)
        {
            strStripifyType = STRIP_DO_NOT_STRIPIFY_VALUE;
        }

        NiStripifyInterface::Platform eStripifyPlatform =
            NiStripifyInterface::PLATFORM_WIN32;
        NiStripifyInterface::Algorithm eStripifyAlgorithm =
            NiStripifyInterface::ALGORITHM_NG;

        if (strStripifyType == STRIP_PC_UNACCELERATED_VALUE)
        {
            eStripifyPlatform = NiStripifyInterface::PLATFORM_WIN32;
            eStripifyAlgorithm = NiStripifyInterface::ALGORITHM_NI;
        }
        else if (strStripifyType == STRIP_PC_FIRSTGEN_VALUE)
        {
            eStripifyPlatform = NiStripifyInterface::PLATFORM_WIN32;
            eStripifyAlgorithm = NiStripifyInterface::ALGORITHM_NG;
        }
        else if (strStripifyType == STRIP_PC_NEXTGEN_VALUE)
        {
            eStripifyPlatform = NiStripifyInterface::PLATFORM_WIN32;
            eStripifyAlgorithm = NiStripifyInterface::ALGORITHM_NG;
        }
        else if (strStripifyType == STRIP_NEXTGENXBOX_VALUE)
        {
            eStripifyPlatform = NiStripifyInterface::PLATFORM_XBOX;
            eStripifyAlgorithm = NiStripifyInterface::ALGORITHM_NG;
        }

        if (strStripifyType != STRIP_DO_NOT_STRIPIFY_VALUE)
        {
            NiStripifyInterface kSI(eStripifyPlatform);
            kSI.SetAlgorithm(eStripifyAlgorithm);
            kSI.Stripify_Object(spScene);
        }
    }

    m_pkProgressBar->Destroy();
    NiDelete m_pkProgressBar;
    m_pkProgressBar = NULL;

    NiStripify::Shutdown();

    ms_pkThis = NULL;

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiStripifyGeometryPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiStripifyGeometryPlugin");
    pkPluginInfo->SetType("PROCESS");
    pkPluginInfo->SetValue(STRIP_PC_SETTINGS, STRIP_PC_NEXTGEN_VALUE);
    pkPluginInfo->SetValue(STRIP_NEXTGENXBOX_SETTINGS,
        STRIP_NEXTGENXBOX_VALUE);
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiStripifyGeometryPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiStripifyGeometryPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
        NiWindowRef hWndParent)
{
    NiStripifyGeometryDialog kDialog(m_hModule, hWndParent, pkInitialInfo);

    switch(kDialog.DoModal())
    {
    case IDOK:
        // Copy over the results
        pkInitialInfo->RemoveAllParameters();
        kDialog.GetResults()->Clone(pkInitialInfo);
        break;
    case IDCANCEL:
        return false;
        break;
    default:
        NIASSERT(false);
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
