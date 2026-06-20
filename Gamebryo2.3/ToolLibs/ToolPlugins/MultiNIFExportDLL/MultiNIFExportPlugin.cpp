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
#include "MultiNIFExportPlugin.h"
#include "MultiNIFExportDialog.h"
#include "MultiNIFExportPluginDefines.h"
#include "NiLookAtController.h"
#include "NiStringExtraData.h"
#include "NiRoomGroup.h"
#include "NiRoom.h"

// RTTI implementation macro.
NiImplementRTTI(MultiNIFExportPlugin, NiExportPlugin);

//---------------------------------------------------------------------------
MultiNIFExportPlugin::MultiNIFExportPlugin() : NiExportPlugin(
    "MultiNIF Export Plug-in",                        // name
    "1.0",                                          // version
    "A MultiNIF export plug-in",                      // short description
    "A MultiNIF export plug-in that uses NiExtraData added by the art "
    "packages to determine which pieces of the scene graph to export as "
    "a NIF")
    // long description
{
    // If any required internal variables need to be set, set them here.

    m_bGlobalUseObjectNameAsFileName = true;
    m_bGlobalDeleteAfterExport = true;
    m_iGlobalTranslateType = MULTI_NIF_EXPORT_CLEAR_WORLD_TRANFORM;
    m_iGlobalRotateType = MULTI_NIF_EXPORT_KEEP_WORLD_TRANFORM;
    m_iGlobalScaleType = MULTI_NIF_EXPORT_KEEP_WORLD_TRANFORM;
    m_bGlobalRepairEmitterWithoutParticles = true;

}
//---------------------------------------------------------------------------
NiPluginInfo* MultiNIFExportPlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiExportPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("MultiNIFExportPlugin");
    pkPluginInfo->SetType("EXPORT");

    // The MultiNIF_KEY_XXXX keys are defined in MultiNIFExportPlugin.h.
    pkPluginInfo->SetBool(MULTI_NIF_EXPROT_USE_OBJECT_NAME_AS_FILE_NAME, 
        true);
    pkPluginInfo->SetBool(MULTI_NIF_EXPORT_DELETE_AFTER_EXPORT, true);

    pkPluginInfo->SetInt(MULTI_NIF_EXPORT_TRANSLATE_TYPE, 
        MULTI_NIF_EXPORT_CLEAR_WORLD_TRANFORM);

    pkPluginInfo->SetInt(MULTI_NIF_EXPORT_ROTATE_TYPE, 
        MULTI_NIF_EXPORT_KEEP_WORLD_TRANFORM);

    pkPluginInfo->SetInt(MULTI_NIF_EXPORT_SCALE_TYPE, 
        MULTI_NIF_EXPORT_KEEP_WORLD_TRANFORM);

    pkPluginInfo->SetBool(MULTI_NIF_EXPORT_REPAIR_EMITTER_WITHOUT_PARTICLES,
        true);

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool MultiNIFExportPlugin::HasManagementDialog()
{
    // Since we have parameters that can be changed, we provide a management
    // dialog, otherwise we would return false.
    return true;
}
//---------------------------------------------------------------------------
bool MultiNIFExportPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    // Handle the user interface for manipulating the execution of this
    // NiPlugin.
    // - If the user clicks 'OK', make the input pkInitialInfo match the new
    //   set of parameters and return true.
    // - If the user clicks 'Cancel', then leave the input pkInitialInfo as
    //   it is and return false.

    MultiNIFExportDialog kDialog(m_hModule, hWndParent, pkInitialInfo);

    switch (kDialog.DoModal())
    {
        case IDOK:
            // Copy over the results.
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
bool MultiNIFExportPlugin::CanExport(NiExportPluginInfo* pkInfo)
{
    // Verify that the NiPluginInfo object can actually be executed by
    // this plug-in. If we were to support previous versions, this is where
    // we would agree to handle the NiPluginInfo. Conversion would come in the
    // actual Export call.

    // Other useful values to check would be:
    // - the name of the plug-in,
    // - the application name if your plug-in is application-dependent 
    //   (which you can get from NiFramework)
    // - if any required parameters exist in this NiPluginInfo

    if (pkInfo->GetClassName() == "MultiNIFExportPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr MultiNIFExportPlugin::Export(
    const NiExportPluginInfo* pkInfo)
{
    // Coming into this method, CanExport has already been called.
    // Therefore, we know that we can actually execute pkInfo.

    // We'll likely want to use the scene graph in this export plug-in,
    // so we'll go ahead and get the scene graph shared data.
    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
        GetSharedData(NiGetSharedDataType(NiSceneGraphSharedData));
 
    // If we do not find scene graph shared data, we cannot complete the
    // processing of the scene graph. Return failure.
    if (!pkSGSharedData)
    {
        // If we want to, we can pop up an NiMessageBox giving a more
        // useful warning if not running in silent mode.
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
            pkLogger->LogElement("MultiNIFExportError", "The MultiNIF Export "
                "Plug-in could not find the NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The MultiNIF Export Plug-in could not find the "
                "NiSceneGraphSharedData!","Scene Graph Shared Data Missing");
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    // Set the endianness of the exported NIFs.
    // Priority 1 - Exporter Options (art exporter)
    // Priority 2 - Scene Graph Options (importer)
    // Fallback - use platform endianness

    NiExporterOptionsSharedData* pkEOSharedData = NULL;
    pkEOSharedData = (NiExporterOptionsSharedData*)GetSharedData(
        NiGetSharedDataType(NiExporterOptionsSharedData));
    m_bGlobalExportLittleEndian = NiSystemDesc::GetSystemDesc()
        .IsLittleEndian();
    if (pkEOSharedData)
    {
        m_bGlobalExportLittleEndian = pkEOSharedData
            ->GetTargetPlatformLittleEndian();
    }
    else if (pkSGSharedData->GetPreserveEndianness())
    {
        m_bGlobalExportLittleEndian = pkSGSharedData->GetSourceLittleEndian();
    }

    // Get the actual parameters for the NiPluginInfo parameter map.
    // These functions will assert in debug mode if the key doesn't exist or
    // if the value is not convertable to the correct primitive type.
    m_bGlobalUseObjectNameAsFileName = pkInfo->GetBool(
        MULTI_NIF_EXPROT_USE_OBJECT_NAME_AS_FILE_NAME);

    m_bGlobalDeleteAfterExport = pkInfo->GetBool(
        MULTI_NIF_EXPORT_DELETE_AFTER_EXPORT);
    m_iGlobalTranslateType = pkInfo->GetInt(MULTI_NIF_EXPORT_TRANSLATE_TYPE);
    m_iGlobalRotateType = pkInfo->GetInt(MULTI_NIF_EXPORT_ROTATE_TYPE);
    m_iGlobalScaleType = pkInfo->GetInt(MULTI_NIF_EXPORT_SCALE_TYPE);
    m_bGlobalRepairEmitterWithoutParticles = pkInfo->GetBool(
        MULTI_NIF_EXPORT_REPAIR_EMITTER_WITHOUT_PARTICLES);

    // You will also likely want to use the file and path specified by
    // the user.
    m_kDirectory = pkInfo->GetDirectory();
    m_kFile = pkInfo->GetFile();

    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNodePtr spRoot = pkSGSharedData->GetRootAt(ui);

        // Save the current scene root
        m_pkCurrentSceneRoot = spRoot;
        RecursiveFindExportNodes(spRoot);
    }

    // Presumably, we have correctly executed our actions on the scene
    // graph. We return success in this case.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
const char* MultiNIFExportPlugin::GetFileDescriptorString()
{
    // Return the name of this file type. This is used by file selection
    // dialogs.
    return "NIF Files (*.nif)";
}
//---------------------------------------------------------------------------
const char* MultiNIFExportPlugin::GetFileTypeString()
{
    // Return the extension used by this export plug-in.
    return "*.nif";
}
//---------------------------------------------------------------------------
void MultiNIFExportPlugin::RecursiveFindExportNodes(NiAVObject* pkObject)
{
    // Depth First processing where we process on the way back up

    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                RecursiveFindExportNodes(pkChild);
            }
        }
    }

    // Setup the defaults
    NiString kFilename;
    m_bLocalUseObjectNameAsFileName = m_bGlobalUseObjectNameAsFileName;
    m_bLocalDeleteAfterExport = m_bGlobalDeleteAfterExport;
    m_iLocalTranslateType = m_iGlobalTranslateType;
    m_iLocalRotateType = m_iGlobalRotateType;
    m_iLocalScaleType = m_iGlobalScaleType;
    m_bLocalRepairEmitterWithoutParticles = 
        m_bGlobalRepairEmitterWithoutParticles;
    

    if (IsExportNode(pkObject, kFilename))
    {
        // Check for Repairable particle Systems
        NiTPointerList<NiParticles*> kParticles;

        if (ValidExportNode(pkObject, kParticles))
        {
            ExportNode(pkObject, kFilename, kParticles);
        }
    }
}
//---------------------------------------------------------------------------
bool MultiNIFExportPlugin::IsExportNode(NiAVObject* pkObject,
    NiString& kFilename)
{
    // Check for the correct extra Data
    if ((pkObject->GetExtraData(
            MULTI_NIF_EXPORT_EXTRA_DATA_USE_GLOBAL_SETTINGS) == NULL) &&
        (pkObject->GetExtraData(
            MULTI_NIF_EXPORT_EXTRA_DATA_FILE_NAME) == NULL) &&
        (pkObject->GetExtraData(
            MULTI_NIF_EXPORT_EXTRA_DATA_USE_OBJECT_NAME) == NULL) &&
        (pkObject->GetExtraData(
            MULTI_NIF_EXPORT_EXTRA_DATA_TRANSLATE_TYPE) == NULL) &&
        (pkObject->GetExtraData(
            MULTI_NIF_EXPORT_EXTRA_DATA_ROTATE_TYPE) == NULL) &&
        (pkObject->GetExtraData(
            MULTI_NIF_EXPORT_EXTRA_DATA_SCALE_TYPE) == NULL) &&
        (pkObject->GetExtraData(
            MULTI_NIF_EXPORT_EXTRA_DATA_DELETE_AFTER_EXPORT) == NULL) &&
        (pkObject->GetExtraData(
            MULTI_NIF_EXPORT_EXTRA_DATA_REPAIR_EMITTER_WITHOUT_PARTICLES) 
            == NULL))
    {
        return false;
    }

    NiBooleanExtraData* pkUseGlobalSettings = 
        (NiBooleanExtraData*)pkObject->GetExtraData(
        MULTI_NIF_EXPORT_EXTRA_DATA_USE_GLOBAL_SETTINGS);

    NiStringExtraData* pkFileName = 
        (NiStringExtraData*)pkObject->GetExtraData(
        MULTI_NIF_EXPORT_EXTRA_DATA_FILE_NAME);

    NiBooleanExtraData* pkUseObjectName = 
        (NiBooleanExtraData*)pkObject->GetExtraData(
        MULTI_NIF_EXPORT_EXTRA_DATA_USE_OBJECT_NAME);

    NiIntegerExtraData* pkTranslateType = 
        (NiIntegerExtraData*)pkObject->GetExtraData(
        MULTI_NIF_EXPORT_EXTRA_DATA_TRANSLATE_TYPE);

    NiIntegerExtraData* pkRotateType =
        (NiIntegerExtraData*)pkObject->GetExtraData(
        MULTI_NIF_EXPORT_EXTRA_DATA_ROTATE_TYPE);

    NiIntegerExtraData* pkScaleType =
        (NiIntegerExtraData*)pkObject->GetExtraData(
        MULTI_NIF_EXPORT_EXTRA_DATA_SCALE_TYPE);

    NiBooleanExtraData* pkDeleteAfterExport =
        (NiBooleanExtraData*)pkObject->GetExtraData(
        MULTI_NIF_EXPORT_EXTRA_DATA_DELETE_AFTER_EXPORT);
    
    NiBooleanExtraData* pkRepairParticles =
        (NiBooleanExtraData*)pkObject->GetExtraData(
        MULTI_NIF_EXPORT_EXTRA_DATA_REPAIR_EMITTER_WITHOUT_PARTICLES);

    // In the case of any error messages, figure out if we display a dialog
    // or write them to the log
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

    // Read in the file name
    if (pkFileName != NULL)
    {
        kFilename = pkFileName->GetValue();
    }

    // Check for using the global settings
    if ((pkUseGlobalSettings != NULL) &&
        !pkUseGlobalSettings->GetValue())
    {
        if (pkUseObjectName != NULL)
        {
            m_bLocalUseObjectNameAsFileName = pkUseObjectName->GetValue();
        }
        
        if (pkTranslateType != NULL)
        {
            m_iLocalTranslateType = pkTranslateType->GetValue();
        }
        
        if (pkRotateType != NULL)
        {
            m_iLocalRotateType = pkRotateType->GetValue();
        }
        
        if (pkScaleType != NULL)
        {
            m_iLocalScaleType = pkScaleType->GetValue();
        }
        
        if (pkDeleteAfterExport != NULL)
        {
            m_bLocalDeleteAfterExport = pkDeleteAfterExport->GetValue();
        }

        if (pkRepairParticles != NULL)
        {
            m_bLocalRepairEmitterWithoutParticles =
                pkRepairParticles->GetValue();            
        }
    }

    // Check for a valid file name
    if ((m_bLocalUseObjectNameAsFileName == false) &&
        !IsValidFilename(kFilename))
    {
        // Show Error Message Invalid File Name provided
        char acBuffer[512];
        NiSprintf(acBuffer, 512, "Invalid File Name::%s   On Object::%s", 
            kFilename, pkObject->GetName());

        if ((bWriteResultsToLog) && (pkLogger))
        {
            pkLogger->LogElement("MultiNIFExportError", acBuffer);
        }
        else
        {
            NiMessageBox(acBuffer, "Multi NIF Export Plug-in");
        }
        return false;
    }

    // Make sure the object name isn't NULL if we are using the object name
    if ((!pkObject->GetName().Exists()) &&
        (m_bLocalUseObjectNameAsFileName == true))
    {
        // Show Error Message Object Has No Name
        char acBuffer[512];
        NiSprintf(acBuffer, 512, "Object Has No Name. Can't convert to File "
            "Name");

        if ((bWriteResultsToLog) && (pkLogger))
        {
            pkLogger->LogElement("MultiNIFExportError", acBuffer);
        }
        else
        {
            NiMessageBox(acBuffer, "Multi NIF Export Plug-in");
        }
        return false;
    }

    // Make sure the object name is a valid file name if we are using the
    // object name
    if ((pkObject->GetName().Exists()) &&
        (m_bLocalUseObjectNameAsFileName == true) &&
        !IsValidFilename(pkObject->GetName()))
    {
        // Show Error Message Invalid File Name provided
        char acBuffer[512];
        NiSprintf(acBuffer, 512, "Object Name is an Invalid File Name::%s",
            pkObject->GetName());

        if ((bWriteResultsToLog) && (pkLogger))
        {
            pkLogger->LogElement("MultiNIFExportError", acBuffer);
        }
        else
        {
            NiMessageBox(acBuffer, "Multi NIF Export Plug-in");
        }
        return false;
    }

    // Set the file using teh Object name if we are using the object name
    if (m_bLocalUseObjectNameAsFileName)
    {
        kFilename = pkObject->GetName();
    }

    
    // append .nif if it doesn't already exist
    if (kFilename.Length() > strlen(".nif"))
    {
        NiString kEnd = kFilename.GetSubstring(
            kFilename.Length() - strlen(".nif"), kFilename.Length());
        
        if (!kEnd.EqualsNoCase(".nif"))
        {
            kFilename += ".nif";
        }
    }
    else
    {
        kFilename += ".nif";
    }

    // Remove the Extra Data
    pkObject->RemoveExtraData(
        MULTI_NIF_EXPORT_EXTRA_DATA_USE_GLOBAL_SETTINGS);
    pkObject->RemoveExtraData(MULTI_NIF_EXPORT_EXTRA_DATA_FILE_NAME);
    pkObject->RemoveExtraData(MULTI_NIF_EXPORT_EXTRA_DATA_USE_OBJECT_NAME);
    pkObject->RemoveExtraData(MULTI_NIF_EXPORT_EXTRA_DATA_TRANSLATE_TYPE);
    pkObject->RemoveExtraData(MULTI_NIF_EXPORT_EXTRA_DATA_ROTATE_TYPE);
    pkObject->RemoveExtraData(MULTI_NIF_EXPORT_EXTRA_DATA_SCALE_TYPE);
    pkObject->RemoveExtraData(
        MULTI_NIF_EXPORT_EXTRA_DATA_DELETE_AFTER_EXPORT);
    pkObject->RemoveExtraData(
        MULTI_NIF_EXPORT_EXTRA_DATA_REPAIR_EMITTER_WITHOUT_PARTICLES);

    return true;
}
//---------------------------------------------------------------------------
bool MultiNIFExportPlugin::ValidExportNode(NiAVObject* pkObject,
    NiTPointerList<NiParticles*>& kParticles)
{
    // Check for error conditions

    // Look At controller without a target
    if (CheckForLookAtController(m_pkCurrentSceneRoot, pkObject))
        return false;

    // Skin without bones
    if (CheckForSkinAndBones(m_pkCurrentSceneRoot, pkObject))
        return false;

    // Room groups without rooms or rooms without fixtures or portals
    if (CheckForPortalDependencies(m_pkCurrentSceneRoot, pkObject))
        return false;

    // New Particle System Particles without Modifiers
    if (CheckForNewParticleSystemWithoutModifiers(pkObject, pkObject))
        return false;

    // Particle System Modifiers without Particles
    if (CheckForModifiersWithoutNewParticleSystem(m_pkCurrentSceneRoot, 
        pkObject))
    {
        return false;
    }


    return true;
}
//---------------------------------------------------------------------------
void MultiNIFExportPlugin::ExportNode(NiAVObject* pkObject, 
    NiString& kFilename, NiTPointerList<NiParticles*>& kParticles)
{
    // Save the Transform
    NiPoint3 kTranslate = pkObject->GetTranslate();
    NiMatrix3 kRotate = pkObject->GetRotate();
    float fScale = pkObject->GetScale();

    // Set the Translate
    switch(m_iLocalTranslateType)
    {
    case MULTI_NIF_EXPORT_KEEP_WORLD_TRANFORM:
        pkObject->SetTranslate(pkObject->GetWorldTranslate());
        break;
    case MULTI_NIF_EXPORT_KEEP_LOCAL_TRANFORM:
        break;
    case MULTI_NIF_EXPORT_CLEAR_WORLD_TRANFORM:
        pkObject->SetTranslate(NiPoint3::ZERO);
        break;
    }


    // Set the Rotate
    switch(m_iLocalRotateType)
    {
    case MULTI_NIF_EXPORT_KEEP_WORLD_TRANFORM:
        pkObject->SetRotate(pkObject->GetWorldRotate());
        break;
    case MULTI_NIF_EXPORT_KEEP_LOCAL_TRANFORM:
        break;
    case MULTI_NIF_EXPORT_CLEAR_WORLD_TRANFORM:
        pkObject->SetRotate(NiMatrix3::IDENTITY);
        break;
    }


    // Set the Scale
    switch(m_iLocalScaleType)
    {
    case MULTI_NIF_EXPORT_KEEP_WORLD_TRANFORM:
        pkObject->SetScale(pkObject->GetWorldScale());
        break;
    case MULTI_NIF_EXPORT_KEEP_LOCAL_TRANFORM:
        break;
    case MULTI_NIF_EXPORT_CLEAR_WORLD_TRANFORM:
        pkObject->SetScale(1.0f);
        break;
    }


    // Clone Any Particle Systems and attach them to the Root
    NiTPointerList<NiParticles*> kRepaired;


    if (NiIsKindOf(NiNode, pkObject) &&
        !kParticles.IsEmpty())
    {
        NiCloningProcess kCloning;
        kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;

        NiTListIterator kIter = kParticles.GetHeadPos();
        while (kIter != NULL)
        {
            NiParticles* pkNewParticles = 
                (NiParticles*)kParticles.GetNext(kIter)->Clone(kCloning);
            
            ((NiNode*)pkObject)->AttachChild(pkNewParticles);
            kParticles.AddTail(pkNewParticles);
        }
    }

    // Duplicate any scene data if it exists
    DuplicateSceneData(pkObject);

    // Add to the stream
    NiStream kStream;
    kStream.SetSaveAsLittleEndian(m_bGlobalExportLittleEndian);
    kStream.InsertObject(pkObject);

    // Append the directory to the file name
    NiString kFullPath = m_kDirectory + kFilename;

    // Save to stream
    if (!kStream.Save(kFullPath))
    {
        return;
    }

    // Check for deleting the Scene graph
    if (m_bLocalDeleteAfterExport)
    {
        pkObject->GetParent()->DetachChild(pkObject);

        // Delete Any Particle Systems
        while (!kParticles.IsEmpty())
        {
            NiParticles* pkPart = kParticles.RemoveHead();
            
            pkPart->GetParent()->DetachChild(pkPart);
        }

        return;
    }
    else if (!kRepaired.IsEmpty())      
    {
        // Cleanup any created Particle systems
        while (!kRepaired.IsEmpty())
        {
            ((NiNode*)pkObject)->DetachChild(kRepaired.RemoveHead());
        }
    }

    // Reset the Transforms
    pkObject->SetTranslate(kTranslate);
    pkObject->SetRotate(kRotate);
    pkObject->SetScale(fScale);
}
//---------------------------------------------------------------------------
bool MultiNIFExportPlugin::IsValidFilename(const char* pcFilename)
{
    if (pcFilename == NULL)
        return false;

    const char* pcCurrent = pcFilename;

    while(*pcCurrent != '\0')
    {
        // Only allow letters, numbers, _, : and 
        if (((*pcCurrent < 'a') || (*pcCurrent > 'z')) &&
            ((*pcCurrent < 'A') || (*pcCurrent > 'Z')) &&
            ((*pcCurrent < '0') || (*pcCurrent > '9')) &&
            (*pcCurrent != '_') && (*pcCurrent != ' ') &&
            (*pcCurrent != ':') && (*pcCurrent != '.') &&
            (*pcCurrent != '\\') && (*pcCurrent != '/') &&
            (*pcCurrent != '-'))
        {
            return false;
        }

        pcCurrent++;
    }
    return true;
}
//---------------------------------------------------------------------------
bool MultiNIFExportPlugin::IsDescendentOf(NiAVObject* pkObject, 
    NiAVObject* pkRoot)
{
    // Determine if Object Is a descendent of the Root
    while(pkObject != NULL)
    {
        if (pkObject == pkRoot)
        {
            return true;
        }
        pkObject = pkObject->GetParent();
    }

    return false;
}
//---------------------------------------------------------------------------
bool  MultiNIFExportPlugin::CheckForLookAtController(NiAVObject* pkObject, 
    NiAVObject* pkRoot)
{
    NiLookAtController* pkController = (NiLookAtController*)pkObject->
        GetController(&NiLookAtController::ms_RTTI);

    if ((pkController != NULL) &&
        (pkController->GetLookAt() != NULL))
    {
        // Check to see if the controller and look at are NOT part of the Root
        // Tree
        //
        // If the Object is part of the root AND the Look At is NOT part of
        // the root then the exported NIF will be broken
        // 
        // If the Object is NOT part of the root AND the Look At is part of
        // the root AND we are deleting after export then the resulting Scene
        // graph will be broken.
        if ((IsDescendentOf(pkObject, pkRoot) &&
             !IsDescendentOf(pkController->GetLookAt(), pkRoot)) ||
            (!IsDescendentOf(pkObject, pkRoot) &&
             IsDescendentOf(pkController->GetLookAt(), pkRoot) &&
             m_bLocalDeleteAfterExport))
        {

            ShowDependencyError(pkRoot->GetName(), "Look at Controller", 
                pkObject->GetName(), "Looks At", 
                pkController->GetLookAt()->GetName());

            return true;
        }
    }

    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                if (CheckForLookAtController(pkChild, pkRoot))
                    return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool  MultiNIFExportPlugin::CheckForSkinAndBones(NiAVObject* pkObject, 
    NiAVObject* pkRoot)
{

    // Check for Geometry
    if (NiIsKindOf(NiGeometry, pkObject))
    {

        NiSkinInstance* pkInstance =
            ((NiGeometry*)pkObject)->GetSkinInstance();

        // Check for a skin instance
        if ((pkInstance != NULL) &&
            (pkInstance->GetSkinData() != NULL))
        {
            unsigned int uiNumBones =
                pkInstance->GetSkinData()->GetBoneCount();
            NiAVObject*const* ppkBones = pkInstance->GetBones();
            
            for(unsigned int uiLoop = 0; uiLoop < uiNumBones; uiLoop++)
            {


                // Check to see if the controller and look at are NOT part of
                // the Root Tree
                //
                // If the Skin is part of the root AND the bones is NOT part
                // of the root then the exported NIF will be broken
                // 
                // If the Skin is NOT part of the root AND the Bones is part
                // of the root AND we are deleting after export then the
                // resulting Scene graph will be broken.
                if ((IsDescendentOf(pkObject, pkRoot) &&
                    !IsDescendentOf(ppkBones[uiLoop], pkRoot)) ||
                    (!IsDescendentOf(pkObject, pkRoot) &&
                    IsDescendentOf(ppkBones[uiLoop], pkRoot) &&
                    m_bLocalDeleteAfterExport))
                {

                    ShowDependencyError(pkRoot->GetName(), "Skin Instance", 
                        pkObject->GetName(), "Bone", 
                        ppkBones[uiLoop]->GetName());
                    return true;
                }
            }

        }
    }


    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                if (CheckForSkinAndBones(pkChild, pkRoot))
                    return true;
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool  MultiNIFExportPlugin::CheckForNewParticleSystemWithoutModifiers(
    NiAVObject* pkObject, NiAVObject* pkRoot)
{
    NiParticleSystem* pkPSystem = NiDynamicCast(NiParticleSystem, pkObject);

    // Check for a particle system controller
    if (pkPSystem != NULL)
    {
        bool bError = false;
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < pkPSystem->GetModifierCount(); uiLoop++)
        {
            NiPSysModifier* pkModifier = pkPSystem->GetModifierAt(uiLoop);

            // Emitter Modifier
            if ((NiIsKindOf(NiPSysVolumeEmitter, pkModifier)) &&
                !IsDescendentOf(
                ((NiPSysVolumeEmitter*)pkModifier)->GetEmitterObj(), pkRoot))
            {
                bError = true;
                ShowDependencyError(pkRoot->GetName(), "Particle System", 
                    pkObject->GetName(), "Volume Emitter",
                    ((NiPSysVolumeEmitter*)pkModifier)->
                    GetEmitterObj()->GetName());
            }

            // NiPSysMesh Emitter
            if (NiIsKindOf(NiPSysMeshEmitter, pkModifier))
            {
                unsigned int uiMesh;
                NiPSysMeshEmitter* pkMeshEmitter =
                    (NiPSysMeshEmitter*)pkModifier;

                for (uiMesh = 0; uiMesh < 
                    pkMeshEmitter->GetGeometryEmitterArrayCount(); uiMesh++)
                {
                    if ((pkMeshEmitter->GetGeometryEmitter(uiMesh) != NULL) &&
                        !IsDescendentOf(
                        pkMeshEmitter->GetGeometryEmitter(uiMesh), pkRoot))
                    {
                        bError = true;
                        ShowDependencyError(pkRoot->GetName(), 
                            "Particle System", pkObject->GetName(), 
                            "Mesh Emitter",
                            pkMeshEmitter->GetGeometryEmitter(
                                uiMesh)->GetName());
                    }
                }
                
            }



            // Bomb Modifier
            if ((NiIsKindOf(NiPSysBombModifier, pkModifier)) &&
                !IsDescendentOf(
                    ((NiPSysBombModifier*)pkModifier)->GetBombObj(), pkRoot))
            {
                bError = true;
                ShowDependencyError(pkRoot->GetName(), "Particle System", 
                    pkObject->GetName(), "Bomb Modifier",
                    ((NiPSysBombModifier*)pkModifier)->
                        GetBombObj()->GetName());
            }

            // Gravity Modifier
            if ((NiIsKindOf(NiPSysGravityModifier, pkModifier)) &&
                !IsDescendentOf(
                ((NiPSysGravityModifier*)pkModifier)->
                    GetGravityObj(), pkRoot))
            {
                bError = true;
                ShowDependencyError(pkRoot->GetName(), "Particle System", 
                    pkObject->GetName(), "Gravity Modifier",
                    ((NiPSysGravityModifier*)pkModifier)->
                        GetGravityObj()->GetName());
            }

            // Field Modifier
            if ((NiIsKindOf(NiPSysFieldModifier, pkModifier)) &&
                !IsDescendentOf(
                ((NiPSysFieldModifier*)pkModifier)->GetFieldObj(), pkRoot))
            {
                bError = true;
                ShowDependencyError(pkRoot->GetName(), "Particle System", 
                    pkObject->GetName(), "Field Modifier",
                    ((NiPSysFieldModifier*)pkModifier)->
                        GetFieldObj()->GetName());
            }

            // NiPSysDragModifier Modifier
            if ((NiIsKindOf(NiPSysDragModifier, pkModifier)) &&
                !IsDescendentOf(
                ((NiPSysDragModifier*)pkModifier)->GetDragObj(), pkRoot))
            {
                bError = true;
                ShowDependencyError(pkRoot->GetName(), "Particle System", 
                    pkObject->GetName(), "Drag Modifier",
                    ((NiPSysDragModifier*)pkModifier)->
                        GetDragObj()->GetName());
            }
        }

        if (bError)
            return true;
    }


    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                if (CheckForNewParticleSystemWithoutModifiers(pkChild,
                    pkRoot))
                {
                    return true;
                }
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool  MultiNIFExportPlugin::CheckForModifiersWithoutNewParticleSystem(
    NiAVObject* pkObject, NiAVObject* pkRoot)
{
    NiParticleSystem* pkPSystem = NiDynamicCast(NiParticleSystem, pkObject);

    // Check for a particle system controller
    if ((pkPSystem != NULL) && !IsDescendentOf(pkObject, pkRoot))
    {
        bool bError = false;
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < pkPSystem->GetModifierCount(); uiLoop++)
        {
            NiPSysModifier* pkModifier = pkPSystem->GetModifierAt(uiLoop);

            // Emitter Modifier
            if ((NiIsKindOf(NiPSysVolumeEmitter, pkModifier)) &&
                IsDescendentOf(
                ((NiPSysVolumeEmitter*)pkModifier)->GetEmitterObj(), pkRoot))
            {
                bError = true;
                ShowDependencyError(pkRoot->GetName(), "Particle System", 
                    pkObject->GetName(), "Volume Emitter",
                    ((NiPSysVolumeEmitter*)pkModifier)->
                        GetEmitterObj()->GetName());
            }

            // NiPSysMesh Emitter
            if (NiIsKindOf(NiPSysMeshEmitter, pkModifier))
            {
                unsigned int uiMesh;
                NiPSysMeshEmitter* pkMeshEmitter =
                    (NiPSysMeshEmitter*)pkModifier;

                for (uiMesh = 0; uiMesh < 
                    pkMeshEmitter->GetGeometryEmitterArrayCount(); uiMesh++)
                {
                    if ((pkMeshEmitter->GetGeometryEmitter(uiMesh) != NULL) &&
                        IsDescendentOf(
                        pkMeshEmitter->GetGeometryEmitter(uiMesh), pkRoot))
                    {
                        bError = true;
                        ShowDependencyError(pkRoot->GetName(), 
                            "Particle System", pkObject->GetName(), 
                            "Mesh Emitter",
                            pkMeshEmitter->GetGeometryEmitter(
                                uiMesh)->GetName());
                    }
                }
                
            }


            // Bomb Modifier
            if ((NiIsKindOf(NiPSysBombModifier, pkModifier)) &&
                IsDescendentOf(
                ((NiPSysBombModifier*)pkModifier)->GetBombObj(), pkRoot))
            {
                bError = true;
                ShowDependencyError(pkRoot->GetName(), "Particle System", 
                    pkObject->GetName(), "Bomb Modifier",
                    ((NiPSysBombModifier*)pkModifier)->
                        GetBombObj()->GetName());
            }

            // Gravity Modifier
            if ((NiIsKindOf(NiPSysGravityModifier, pkModifier)) &&
                IsDescendentOf(
                ((NiPSysGravityModifier*)pkModifier)->GetGravityObj(),
                pkRoot))
            {
                bError = true;
                ShowDependencyError(pkRoot->GetName(), "Particle System", 
                    pkObject->GetName(), "Bomb Modifier",
                    ((NiPSysGravityModifier*)pkModifier)->
                        GetGravityObj()->GetName());
            }

            // Field Modifier
            if ((NiIsKindOf(NiPSysFieldModifier, pkModifier)) &&
                IsDescendentOf(
                ((NiPSysFieldModifier*)pkModifier)->GetFieldObj(), pkRoot))
            {
                bError = true;
                ShowDependencyError(pkRoot->GetName(), "Particle System", 
                    pkObject->GetName(), "Field Modifier",
                    ((NiPSysFieldModifier*)pkModifier)->
                        GetFieldObj()->GetName());
            }

            // NiPSysDragModifier Modifier
            if ((NiIsKindOf(NiPSysDragModifier, pkModifier)) &&
                IsDescendentOf(
                ((NiPSysDragModifier*)pkModifier)->GetDragObj(), pkRoot))
            {
                bError = true;
                ShowDependencyError(pkRoot->GetName(), "Particle System", 
                    pkObject->GetName(), "Drag Modifier",
                    ((NiPSysDragModifier*)pkModifier)->
                        GetDragObj()->GetName());
            }
        }

        if (bError)
            return true;
    }


    // Loop over all children 
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);

    if (pkNode != NULL)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount();
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                if (CheckForModifiersWithoutNewParticleSystem(pkChild,
                    pkRoot))
                {
                    return true;
                }
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool MultiNIFExportPlugin::CheckForPortalDependencies(NiAVObject* pkObject,
    NiAVObject* pkRoot)
{
    // if it is kind of room group and not under multinif root, make sure
    // no children are under the multinif root
    //
    // if it is kind of room and not under multinif root, make sure all 
    // fixtures and portals are not under multinif root

    NiNode* pkNode = NULL;
    if (NiIsKindOf(NiRoomGroup, pkObject) || NiIsKindOf(NiRoom, pkObject))
    {
        pkNode = (NiNode*)pkObject;
    }
    if (pkNode)
    {
        for (unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);
            if (!IsDescendentOf(pkObject, pkRoot) &&
                IsDescendentOf(pkChild, pkRoot))
            {
                ShowDependencyError(pkRoot->GetName(), "Room", 
                    pkObject->GetName(), "AVObject", 
                    pkChild->GetName());
                return true;
            }
        }
    }

    pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for(unsigned int uiLoop = 0; uiLoop < pkNode->GetArrayCount(); 
            uiLoop++)
        {
            NiAVObject* pkChild = pkNode->GetAt(uiLoop);

            if (pkChild != NULL)
            {
                if (CheckForPortalDependencies(pkChild, pkRoot))
                    return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void MultiNIFExportPlugin::DuplicateSceneData(NiAVObject* pkNewRoot)
{
    // Check for Scene Export Info
    NiStringExtraData* pkExportInfo = 
        (NiStringExtraData*)m_pkCurrentSceneRoot->GetExtraData(
            "Scene Name and Version");

    if (pkExportInfo == NULL)
        return;


    // Duplicate the Scene Export Info
    NiStringExtraData* pkNewInfo = NiNew NiStringExtraData(
        pkExportInfo->GetValue());

    pkNewInfo->SetName("Scene Name and Version");

    pkNewRoot->InsertExtraData(pkNewInfo);
}
//---------------------------------------------------------------------------
void MultiNIFExportPlugin::ShowDependencyError(const char* pcRootName,
   const char* pcObjectType, const char* pcObjectName, 
   const char* pcDependencyType, const char* pcDependentObject)
{
    // Show Error Message 
    char acBuffer[1024] = "Export Failed for Root(";


    if (pcRootName != NULL)
    {
        NiStrcat(acBuffer, 1024, pcRootName);
    }
    else
    {
        NiStrcat(acBuffer, 1024, "<<No Name Found>>");
    }

    NiStrcat(acBuffer, 1024, ").  The ");
    NiStrcat(acBuffer, 1024, pcObjectType);
    NiStrcat(acBuffer, 1024, " on Object(");

    if (pcObjectName)
    {
        NiStrcat(acBuffer, 1024, pcObjectName);
    }
    else
    {
        NiStrcat(acBuffer, 1024, "<<No Name Found>>");
    }

    NiStrcat(acBuffer, 1024, ") References the ");
    NiStrcat(acBuffer, 1024, pcDependencyType);
    NiStrcat(acBuffer, 1024, "(");

    if (pcDependentObject != NULL)
    {
        NiStrcat(acBuffer, 1024, pcDependentObject);
    }
    else
    {
        NiStrcat(acBuffer, 1024, "<<No Name Found>>");
    }

    NiStrcat(acBuffer, 1024, ") both must be descendants of the Root.  "
        "Exporting will break the scene or NIF. ");

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
        pkLogger->LogElement("MultiNIFExportError", acBuffer);
    }
    else
    {
        NiMessageBox(acBuffer, "Multi NIF Export Plug-in");
    }
}
