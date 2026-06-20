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

#include "FLTImportPlugin.h"
#include <NiPluginToolkit.h>
#include <NiStandardSharedData.h>
#include <NiOpenFlight.h>

// Include Gamebryo headers.
#include <NiMain.h>
#include <NiSystem.h>

NiImplementRTTI(FLTImportPlugin, NiImportPlugin);

//---------------------------------------------------------------------------
FLTImportPlugin::FLTImportPlugin() :
    NiImportPlugin("FLT Importer", "1.0", "Imports FLT files.", "Imports "
    "FLT files and stores the result in an NiSceneGraphSharedData object.")
{
}
//---------------------------------------------------------------------------
NiPluginInfo* FLTImportPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiImportPluginInfo;
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("FLTImportPlugin");
    pkPluginInfo->SetType("IMPORT");

    pkPluginInfo->SetBool(USE_INTERNAL_TEXTURES, true);
    pkPluginInfo->SetBool(ADD_DEFAULT_LIGHTS, true);
    pkPluginInfo->SetBool(USE_TRUE_DOUBLESIDED, true);
    pkPluginInfo->SetBool(FORCE_MERGE_SIBLING_SWITCHNODES, false);
    pkPluginInfo->SetBool(FORCE_MERGE_SIBLING_LODNODES, true);
    
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool FLTImportPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool FLTImportPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    return false;
}
//---------------------------------------------------------------------------
bool FLTImportPlugin::CanImport(NiImportPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "FLTImportPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr FLTImportPlugin::Import(const NiImportPluginInfo* pkInfo)
{
    // Get file path.
    NiString strFilePath = pkInfo->GetFullFilePath();
    if (strFilePath.IsEmpty())
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Get the NiSceneGraphSharedData, creating it if it does not exist.
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);
    NiSceneGraphSharedData* pkSGSharedData = NULL;
    pkSGSharedData =(NiSceneGraphSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        pkSGSharedData = NiNew NiSceneGraphSharedData;
        pkDataList->Insert(pkSGSharedData);
    }
 

    NiImageConverterPtr spConverter = NiImageConverter::GetImageConverter();
    NiImageConverter::SetImageConverter(NiNew NiDevImageConverter);
    bool bPreload = NiSourceTexture::GetUsePreloading();
    NiSourceTexture::SetUsePreloading(false);

    // Load the NIF file.
    NiOpenFlight kOpenFlight;

    NiOpenFlight::SetUseInternalTextures(
        pkInfo->GetBool(USE_INTERNAL_TEXTURES));
    NiOpenFlight::SetAddDefaultLights(
        pkInfo->GetBool(ADD_DEFAULT_LIGHTS));
    NiOpenFlight::SetUseTrueDoubleSided(
        pkInfo->GetBool(USE_TRUE_DOUBLESIDED));
    NiOpenFlight::SetForceMergeSiblingSwitchNodes(
        pkInfo->GetBool(FORCE_MERGE_SIBLING_SWITCHNODES));
    NiOpenFlight::SetForceMergeSiblingLODNodes(
        pkInfo->GetBool(FORCE_MERGE_SIBLING_LODNODES));

    NiNode* pkNode = kOpenFlight.CreateScene(strFilePath);
    if (!pkNode)
    {
        NiImageConverter::SetImageConverter(spConverter);
        NiSourceTexture::SetUsePreloading(bPreload);

        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    NiImageConverter::SetImageConverter(spConverter);
    NiSourceTexture::SetUsePreloading(bPreload);

    if (pkInfo->GetImportType() == NiImportPluginInfo::ADD)
    {
        pkSGSharedData->AddRoot(pkNode, strFilePath, true, RENDER_GENERIC);
    }
    else if (pkInfo->GetImportType() == NiImportPluginInfo::REPLACE)
    {
        pkSGSharedData->RemoveAllRoots();
        pkSGSharedData->AddRoot(pkNode, strFilePath, true, RENDER_GENERIC);
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
const char* FLTImportPlugin::GetFileDescriptorString()
{
    return "MultiGen FLT Files (*.flt)";
}
//---------------------------------------------------------------------------
const char* FLTImportPlugin::GetFileTypeString()
{
    return "*.flt";
}
//---------------------------------------------------------------------------
      