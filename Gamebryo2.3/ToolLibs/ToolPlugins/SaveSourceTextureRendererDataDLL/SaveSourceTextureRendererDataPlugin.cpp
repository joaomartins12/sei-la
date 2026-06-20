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
#include "SaveSourceTextureRendererDataPlugin.h"

#include <windows.h>

#include <NiFlipController.h>
#include <NiPixelData.h>
#include <NiPixelFormat.h>
#include <NiNode.h>
#include <NiSourceTexture.h>
#include <NiTextureEffect.h>
#include <NiTexturingProperty.h>
#include <NiVersion.h>

const char* const SaveSourceTextureRendererDataPlugin::ms_pcImageTilerDLLName =
    "NiXbox360ImageTiler"
    NI_DLL_SUFFIX
    ".dll";

// RTTI implementation macro.
NiImplementRTTI(SaveSourceTextureRendererDataPlugin, NiPlugin);

//---------------------------------------------------------------------------
SaveSourceTextureRendererDataPlugin::SaveSourceTextureRendererDataPlugin()
    : NiPlugin(
    "Save Source Texture Renderer Data Plug-in",  // Name.
    "1.0",  // Version.
    "A plug-in to save persistent source texture renderer data.",
        // Short description.
    "A plug-in to save persistent source renderer data for internal textures.")
        // Long description.
{
    /* */
}
//---------------------------------------------------------------------------
SaveSourceTextureRendererDataPlugin::~SaveSourceTextureRendererDataPlugin()
{
    FreeLibrary((HINSTANCE)m_hImageTilerDLL);
}
//---------------------------------------------------------------------------
NiPluginInfo* SaveSourceTextureRendererDataPlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("SaveSourceTextureRendererDataPlugin");
    pkPluginInfo->SetType("PROCESS");
    
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool SaveSourceTextureRendererDataPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool SaveSourceTextureRendererDataPlugin::DoManagementDialog(
    NiPluginInfo* pkInitialInfo, NiWindowRef hWndParent)
{
    return false;
}
//---------------------------------------------------------------------------
bool SaveSourceTextureRendererDataPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    // Verify that the NiPluginInfo object can actually be executed by
    // this plug-in. If we were to support previous versions, this is where
    // we would agree to handle the NiPluginInfo. Conversion would come in the 
    // actual Execute call.

    // Other useful values to check would be:
    // - the name of the plug-in,
    // - the application name if your plug-in is application-dependent 
    //   (which you can get from NiFramework)
    // - if any required parameters exist in this NiPluginInfo

    if (pkInfo->GetClassName() == "SaveSourceTextureRendererDataPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr SaveSourceTextureRendererDataPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    // Coming into this method, CanExecute has already been called.
    // Therefore, we know that we can actually execute pkInfo.

    // We'll want to use the scene graph in this process plug-in,
    // so we'll go ahead and get the scene graph shared data.
    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
        GetSharedData(NiGetSharedDataType(NiSceneGraphSharedData));

    // Establish whether info, warnings, and errors go to a log file or to a
    // NiMessageBox.
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);
    NiExporterOptionsSharedData* pkExporterSharedData = 
        (NiExporterOptionsSharedData*) 
        pkDataList->Get(NiGetSharedDataType(NiExporterOptionsSharedData));
    bool bWriteResultsToLog = false;
    NiXMLLogger* pkLogger = NULL;
    if (pkExporterSharedData)
    {
        bWriteResultsToLog = pkExporterSharedData->GetWriteResultsToLog();
        pkLogger = pkExporterSharedData->GetXMLLogger();
    }

    // If we do not find scene graph shared data, we cannot complete the
    // processing of the scene graph. Return failure.
    if (!pkSGSharedData)
    {
        // Pop up an NiMessageBox (or write to the log file), giving a more
        // useful warning.
        if ((bWriteResultsToLog) && (pkLogger))
        {
            pkLogger->LogElement("SaveSourceTextureRendererDataError",
                "The Save Source Texture Renderer Data Process Plug-in "
                "could not find the NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The Save Source Texture Renderer Data Process "
                "Plug-in could not find the NiSceneGraphSharedData!",
                "Scene Graph Shared Data Missing");
        }

        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    bool bLibAvailable = LoadImageTilerLibrary();

    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui); 

        NiPluginToolkitRenderer eRenderer =
            pkSGSharedData->GetRootPlatform(spScene);

        // Warn if exporting for Xbox 360, without a Gamebryo Xenon SDK.
        if ((eRenderer == RENDER_XENON) && (!bLibAvailable))
        {
            if ((bWriteResultsToLog) && (pkLogger))
            {
                pkLogger->LogElement("SaveSourceTextureRendererDataWarning",
                    "Warning:  The Save Source Texture Renderer Data Process "
                    "Plug-in could not find\nthe NiXbox360ImageTiler DLL "
                    "(available only with Gamebryo Xenon SDKs).  No\nXbox 360 "
                    "texture tiling will be done, resulting in non-optimal "
                    "textures for\nXbox 360.\n\nSince a primary reason for "
                    "using persistent source texture renderer data on\nXbox "
                    "360 is optimal texture performance, the Xenon Renderer "
                    "will substitute\nerror replacement textures at run time "
                    "if the Xbox 360-specific persistent\nsource texture data "
                    "is not tiled.");
            }
            else
            {
                NiMessageBox("Warning:  The Save Source Texture Renderer Data "
                    "Process Plug-in could not find\nthe NiXbox360ImageTiler "
                    "DLL (available only with Gamebryo Xenon SDKs).  No\nXbox "
                    "360 texture tiling will be done, resulting in "
                    "non-optimal textures for\nXbox 360.\n\nSince a primary "
                    "reason for using persistent source texture renderer data "
                    "on\nXbox 360 is optimal texture performance, the Xenon "
                    "Renderer will substitute\nerror replacement textures at "
                    "run time if the Xbox 360-specific persistent\nsource "
                    "texture data is not tiled.",
                    "Warning:  NiXbox360ImageTiler DLL is missing");
            }
        }
    
        if ((eRenderer == RENDER_DX9) || (eRenderer == RENDER_XENON) ||
            (eRenderer == RENDER_PS3))
        {
            Traverse(spScene, eRenderer);
        }
    }

    // Presumably, we have correctly executed our actions on the scene
    // graph. We return success in this case.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
void SaveSourceTextureRendererDataPlugin::Traverse(NiAVObject* pkAVObject,
    const NiPluginToolkitRenderer eRenderer)
{
    if (!pkAVObject)
        return;

    NiTexturingProperty* pkTexProp =
        (NiTexturingProperty*)pkAVObject->GetProperty(NiProperty::TEXTURING);
    if (pkTexProp)
    {
        // Look for regular maps.
        const NiTexturingProperty::NiMapArray& kMaps = 
            pkTexProp->GetMaps();
        for (unsigned int i = 0; i < kMaps.GetSize(); i++)
        {
            NiTexturingProperty::Map* pkMap = kMaps.GetAt(i);
            if (pkMap)
            {
                NiTexture* pkTexture = pkMap->GetTexture();
                if (pkTexture)
                    ProcessTexture(pkTexture, eRenderer);
            }
        }

        // Look for shader maps.
        for (unsigned int i = 0; i < pkTexProp->GetShaderArrayCount(); i++)
        {
            NiTexturingProperty::ShaderMap* pkShaderMap =
                pkTexProp->GetShaderMap(i);

            if (pkShaderMap)
            {
                NiTexture* pkTexture = pkShaderMap->GetTexture();
                if (pkTexture)
                    ProcessTexture(pkTexture, eRenderer);
            }
        }
    }

    // Check for NiFlipController objects.
    const NiPropertyList& kPropList = pkAVObject->GetPropertyList();
    NiTListIterator kPos = kPropList.GetHeadPos();
    while (kPos)
    {
        NiProperty* pkProperty = kPropList.GetNext(kPos);
        if (pkProperty)
            ProcessFlipController(pkProperty->GetControllers(), eRenderer);
    }

    ProcessFlipController(pkAVObject->GetControllers(), eRenderer);

    NiNode* pkNode = NiDynamicCast(NiNode, pkAVObject);
    if (pkNode)
    {
        // Iterate over dynamic effects.
        const NiDynamicEffectList& kEffectList = pkNode->GetEffectList();
        NiTListIterator kPos = kEffectList.GetHeadPos();
        while (kPos)
        {
            NiDynamicEffect* pkEffect = kEffectList.GetNext(kPos);

            NiTextureEffect* pkTexEffect =
                NiDynamicCast(NiTextureEffect, pkEffect);
            if (pkTexEffect)
            {
                NiTexture* pkTexture = pkTexEffect->GetEffectTexture();
                if (pkTexture)
                    ProcessTexture(pkTexture, eRenderer);
            }
        }

        // Iterate over children.
        for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild)
                Traverse(pkChild, eRenderer);
        }
    }
}
//---------------------------------------------------------------------------
void SaveSourceTextureRendererDataPlugin::ProcessTexture(NiTexture* pkTexture,
    const NiPluginToolkitRenderer eRenderer)
{
    if (!NiIsKindOf(NiSourceTexture, pkTexture))
        return;

    NiSourceTexture* pkSrcTex = (NiSourceTexture*) pkTexture;

    if (pkSrcTex)
        CreatePersistentSrcRendererData(pkSrcTex, eRenderer);
}
//---------------------------------------------------------------------------
// Iterate through list of time controllers. If any NiFlipController is found,
// create PersistentSrcTextureRendererData for all textures referred to by the
// NiFlipController.
void SaveSourceTextureRendererDataPlugin::ProcessFlipController(
    NiTimeController* pkControl, const NiPluginToolkitRenderer eRenderer)
{
    for (/**/; pkControl != NULL; pkControl = pkControl->GetNext())
    {
        if (NiIsKindOf(NiFlipController, pkControl))
        {
            NiFlipController* pkFlip = (NiFlipController*) pkControl;

            unsigned int uiTextures = pkFlip->GetTextureArraySize();
            for (unsigned int i = 0; i < uiTextures; i++)
            {
                NiTexture* pkTexture = pkFlip->GetTextureAt(i);
                NIASSERT(pkTexture != NULL);
                ProcessTexture(pkTexture, eRenderer);
            }
        }
    }
}
//---------------------------------------------------------------------------
void SaveSourceTextureRendererDataPlugin::CreatePersistentSrcRendererData(
   NiSourceTexture* pkTexture, const NiPluginToolkitRenderer eRenderer)
{
    NiPersistentSrcTextureRendererData::PlatformID eTargetPlatform;
    switch (eRenderer)
    {
    case RENDER_DX9:
        eTargetPlatform = NiPersistentSrcTextureRendererData::NI_DX9;
        break;
    case RENDER_XENON:
        eTargetPlatform = NiPersistentSrcTextureRendererData::NI_XENON;
        break;
    case RENDER_PS3:
        eTargetPlatform = NiPersistentSrcTextureRendererData::NI_PS3;
        break;
    default:
        return;
        break;
    }

    // Create a persistent (streamable) copy of the data
    // (NiPersistentSourceTextureRendererData) using NiPixelData, and then
    // remove NiPixelData.  Also, conditionally, perform Xenon-specific texture
    // tiling, as appropriate.
    NiPersistentSrcTextureRendererData* pkPersistentSrcRendererData =
        NiPersistentSrcTextureRendererData::Create(pkTexture, eTargetPlatform);

    // Return if NULL, e.g., if persistent source texture renderer data already
    // exists.
    if (!pkPersistentSrcRendererData)
        return;

    if ((eRenderer == RENDER_XENON) && (m_pfnTileImageFunc))
        m_pfnTileImageFunc(pkPersistentSrcRendererData);

    // The streaming system will later stream out the newly created
    // NiPersistentSourceTextureRendererData objects.
}
//---------------------------------------------------------------------------
bool SaveSourceTextureRendererDataPlugin::LoadImageTilerLibrary()
{
    m_hImageTilerDLL = LoadLibrary(ms_pcImageTilerDLLName);
    if (!m_hImageTilerDLL)
    {
        m_pfnTileImageFunc = NULL;
        return false;
    }

    m_pfnTileImageFunc = (NiXbox360TileImageFunction)GetProcAddress(
        m_hImageTilerDLL, "TileImage");

    if (!m_pfnTileImageFunc)
        return false;

    return true;
}
//---------------------------------------------------------------------------
