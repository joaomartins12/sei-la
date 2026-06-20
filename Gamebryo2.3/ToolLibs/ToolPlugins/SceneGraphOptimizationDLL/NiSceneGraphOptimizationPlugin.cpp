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

#include <stdafx.h>
#include "NiSceneGraphOptimizationPlugin.h"
#include "NiSceneGraphOptimizationDialog.h"
#include "NiSceneGraphOptimizationDefines.h"
#include <NiBoneLODController.h>

NiImplementRTTI(NiSceneGraphOptimizationPlugin, NiPlugin);

//---------------------------------------------------------------------------
NiSceneGraphOptimizationPlugin::NiSceneGraphOptimizationPlugin() :
    NiPlugin("Scene Graph Optimization", "2.0", "Optimize scene graph for "
    "speed and size.", "Optimizes scene, removes childless nodes, duplicates "
    "properties, and much, much more.")
{
    m_bShowProgress = true;
}
//---------------------------------------------------------------------------
bool NiSceneGraphOptimizationPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "NiSceneGraphOptimizationPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NiSceneGraphOptimizationPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    NiSharedDataList* pkDataList = NiSharedDataList::GetInstance();
    NIASSERT(pkDataList);

    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*)
        pkDataList->Get(NiGetSharedDataType(NiSceneGraphSharedData));

    if (!pkSGSharedData)
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }


    // Check the PluginInfo to double check the type
    if(pkInfo->GetType() != "PROCESS")
    {
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    NiString strTrue = NiString::FromBool(true);

    // Get script version.
    unsigned int uiScriptVersion = NiPluginHelpers::GetVersionFromString(
        pkInfo->GetVersion());
    
    // Read the Parameters
    m_bRemoveDupProperties = pkInfo->GetBool(OPT_REMOVE_DUP_PROPERTIES);
    m_bRemoveChildlessNodes = pkInfo->GetBool(OPT_REMOVE_CHILDLESS_NODES);
    m_bRemoveMultiMtlNodes = pkInfo->GetBool(OPT_REMOVE_MULTIMTL_NODES);
    m_bRemoveSingleChildNodes = pkInfo->GetBool(OPT_REMOVE_SINGLE_CHILD_NODES);
    m_bMergeSiblingNodes = pkInfo->GetBool(OPT_MERGE_SIBLING_NODES);
    m_bOptimizeTriShapes = pkInfo->GetBool(OPT_OPTIMIZE_TRISHAPES);
    m_bMergeSiblingTriShapes =pkInfo->GetBool(OPT_MERGE_SIBLING_TRISHAPES);
    m_bNoMergeOnAlpha = pkInfo->GetBool(OPT_NO_MERGE_ON_ALPHA);
    m_bRemoveExtraUVSets = pkInfo->GetBool(OPT_REMOVE_EXTRA_UVSETS);
    m_bRemoveUnnecessaryNormals = pkInfo->GetBool(
        OPT_REMOVE_UNNECESSARY_NORMALS);
    m_bRemoveUnnecessaryVisControllers = pkInfo->GetBool(
        OPT_REMOVE_UNNECESSARY_VISCONTROLLERS);
    m_bRemoveNames = pkInfo->GetBool(OPT_REMOVE_NAMES);
    m_bNameUnnamedObjects = pkInfo->GetBool(OPT_NAME_UNNAMED_OBJECTS);
    m_bRemoveBadTexturingProps = pkInfo->GetBool(
        OPT_REMOVE_BAD_TEXTURING_PROPS);
    m_bRemoveUnnecessaryExtraData = pkInfo->GetBool(
        OPT_REMOVE_UNNECESSARY_EXTRA_DATA);
    m_bRemoveSgoKeepTags = pkInfo->GetBool(OPT_REMOVE_SGOKEEP_TAGS);
    m_bRemoveUnnecessaryLookAtControllers = 
        pkInfo->GetBool(OPT_REMOVE_UNNECESSARY_LOOKATCONTROLLERS);
    m_bRemoveHiddenBoneGeometry = 
        pkInfo->GetBool(OPT_REMOVE_HIDDEN_BONE_GEOMETRY);
    m_bReduceAnimationKeys = 
        pkInfo->GetBool(OPT_REDUCE_ANIMATION_KEYS);

    if(!pkInfo->GetValue(OPT_ANIMATION_KEY_REDUCTION_TOLERANCE).ToFloat(
        m_fAnimationKeyReductionTolerance))
    {
        // This case should give a warning
        m_bReduceAnimationKeys = false;
    }

    m_bSortByTexturingProp = pkInfo->GetBool(OPT_SORT_BY_TEXTURING_PROP);

    if (CompareVersions(uiScriptVersion, "1.2"))
    {
        m_bWeldSkin = true;
    }
    else
    {
        m_bWeldSkin = pkInfo->GetBool(OPT_WELD_SKIN);
    }

    int iBones = 4;
    if(!pkInfo->GetValue(OPT_BONES_PER_PARTITION).ToInt(iBones))
    {
        // This case should give a warning
    }

    NiNodePtr spRoot = pkSGSharedData->GetFullSceneGraph();
    for (unsigned int ui = 0; ui < spRoot->GetArrayCount(); ui++)
    {
        NiNodePtr spScene = pkSGSharedData->GetRootAt(ui);
        m_uiBonesPerPartition = (unsigned int)iBones;

        NiPluginToolkitRenderer eRenderer = pkSGSharedData->GetRootPlatform(
            spScene);

        m_bExportSoftwareSkin = pkInfo->GetBool(OPT_EXPORT_SOFTWARE_SKIN);
        m_bExportHardwareSkin = pkInfo->GetBool(OPT_EXPORT_HARDWARE_SKIN);

        if (eRenderer == RENDER_XENON)
        {
            // Force bones per partition to 30 on Xenon since that's what the
            // default pipeline supports. Shaders can override during the
            // optimize pass. This value must be kept in synch with the 
            // value in NDL_VSFragments::TransformSkinnedPosition and the
            // value in NiXenonRenderer::m_uiHWBones.
            m_uiBonesPerPartition = 30;
            m_bExportHardwareSkin = true;
        }

        Optimize(spScene);
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
NiPluginInfo* NiSceneGraphOptimizationPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NiSceneGraphOptimizationPlugin");
    pkPluginInfo->SetType("PROCESS");

    pkPluginInfo->SetFloat(OPT_ANIMATION_KEY_REDUCTION_TOLERANCE, 0.0001f);
    pkPluginInfo->SetBool(OPT_REMOVE_DUP_PROPERTIES, true);
    pkPluginInfo->SetBool(OPT_REMOVE_CHILDLESS_NODES, true);
    pkPluginInfo->SetBool(OPT_REMOVE_MULTIMTL_NODES, true);
    pkPluginInfo->SetBool(OPT_REMOVE_SINGLE_CHILD_NODES, true);
    pkPluginInfo->SetBool(OPT_MERGE_SIBLING_NODES, true);
    pkPluginInfo->SetBool(OPT_OPTIMIZE_TRISHAPES,true);
    pkPluginInfo->SetBool(OPT_MERGE_SIBLING_TRISHAPES, true);
    pkPluginInfo->SetBool(OPT_NO_MERGE_ON_ALPHA,true);
    pkPluginInfo->SetBool(OPT_REMOVE_EXTRA_UVSETS, true);
    pkPluginInfo->SetBool(OPT_REMOVE_UNNECESSARY_NORMALS, true);
    pkPluginInfo->SetBool(OPT_REMOVE_UNNECESSARY_VISCONTROLLERS, true);
    pkPluginInfo->SetBool(OPT_REMOVE_NAMES, false);
    pkPluginInfo->SetBool(OPT_NAME_UNNAMED_OBJECTS, false);
    pkPluginInfo->SetBool(OPT_REMOVE_BAD_TEXTURING_PROPS, true);
    pkPluginInfo->SetBool(OPT_REMOVE_UNNECESSARY_EXTRA_DATA, true);
    pkPluginInfo->SetBool(OPT_REMOVE_SGOKEEP_TAGS, true);
    pkPluginInfo->SetBool(OPT_REMOVE_UNNECESSARY_LOOKATCONTROLLERS, true);
    pkPluginInfo->SetBool(OPT_REMOVE_HIDDEN_BONE_GEOMETRY,true);
    pkPluginInfo->SetBool(OPT_REDUCE_ANIMATION_KEYS,true);
    pkPluginInfo->SetBool(OPT_SORT_BY_TEXTURING_PROP,false);
    pkPluginInfo->SetBool(OPT_WELD_SKIN, true);
    pkPluginInfo->SetBool(OPT_EXPORT_SOFTWARE_SKIN, true);
    pkPluginInfo->SetBool(OPT_EXPORT_HARDWARE_SKIN, true);
    pkPluginInfo->SetInt(OPT_BONES_PER_PARTITION,4);

    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NiSceneGraphOptimizationPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiSceneGraphOptimizationPlugin::DoManagementDialog(
    NiPluginInfo* pkInitialInfo, NiWindowRef hWndParent)
{
    // Convert old version script, if necessary.
    bool bConvertedOldScript = false;
    unsigned int uiScriptVersion = NiPluginHelpers::GetVersionFromString(
        pkInitialInfo->GetVersion());
    if (CompareVersions(uiScriptVersion, m_strVersion))
    {
        if (CompareVersions(uiScriptVersion, "1.2"))
        {
            pkInitialInfo->SetBool(OPT_WELD_SKIN, true);
        }

        pkInitialInfo->SetVersion(m_strVersion);
        bConvertedOldScript = true;
    }

    NiSceneGraphOptimizationDialog kDialog(m_hModule, hWndParent,
        pkInitialInfo);

    switch(kDialog.DoModal())
    {
    case IDOK:
        // Copy over the results
        pkInitialInfo->RemoveAllParameters();
        kDialog.GetResults()->Clone(pkInitialInfo);
        return true;
        break;
    case IDCANCEL:
        break;
    default:
        NIASSERT(false);
    }

    return bConvertedOldScript;
}


//---------------------------------------------------------------------------
// Progress bar.
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationPlugin::InitProgressBar()
{
    if(m_bShowProgress)
    {
        m_pkProgressBar = NiNew NiProgressDialog("Optimizing Scene Graph");
        m_pkProgressBar->Create();
        m_pkProgressBar->SetRangeSpan(21);
        m_pkProgressBar->SetPosition(0);
        m_pkProgressBar->SetLineOne("Optimization Setting:");
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationPlugin::UpdateProgressBar(const char* pcLine, 
    bool bDoOption)
{

    if(m_bShowProgress && m_pkProgressBar != NULL)
    {
        m_pkProgressBar->StepIt();
        const unsigned int uiStrLen = 512;
        char acString[uiStrLen];
        if (bDoOption)
        {
            NiSprintf(acString, uiStrLen, "%s ...", pcLine);
        }
        else
        {
            NiSprintf(acString, uiStrLen, "%s (Skipping)", pcLine);
        }
        m_pkProgressBar->SetLineTwo(acString);
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationPlugin::CloseProgressBar()
{
    if(m_bShowProgress)
    {
        m_pkProgressBar->Destroy();
        NiDelete m_pkProgressBar;
        m_pkProgressBar = NULL;
    }
}
//---------------------------------------------------------------------------
// General functions.
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationPlugin::Optimize(NiNode* pkScene)
{
    InitProgressBar();

    pkScene->Update(0.0f);
    pkScene->UpdateProperties();
    pkScene->UpdateEffects();
    
    UpdateProgressBar("Removing extra UV sets", m_bRemoveExtraUVSets); 
    if (m_bRemoveExtraUVSets)
    {
        NiOptimize::RemoveExtraUVSets(pkScene);
    }

    UpdateProgressBar("Removing duplicate properties",
        m_bRemoveDupProperties);
    if (m_bRemoveDupProperties)
    {
        NiOptimize::RemoveDupProperties(pkScene);
    }

    UpdateProgressBar("Removing bad texturing properties", 
        m_bRemoveBadTexturingProps);
    if (m_bRemoveBadTexturingProps)
    {
        NiOptimize::RemoveBadTexturingProps(pkScene);
    }

    UpdateProgressBar("Removing multi-material nodes",
        m_bRemoveMultiMtlNodes);
    if (m_bRemoveMultiMtlNodes)
    {
        NiOptimize::RemoveMultiMtlNodes(pkScene);
    }

    UpdateProgressBar("Removing childless nodes", m_bRemoveChildlessNodes);
    if (m_bRemoveChildlessNodes)
    {
        NiOptimize::RemoveChildlessNodes(pkScene);
    }

    UpdateProgressBar("Removing single child nodes", 
        m_bRemoveSingleChildNodes);
    if (m_bRemoveSingleChildNodes)
    {
        NiOptimize::RemoveSingleChildNodes(pkScene);
    }

    UpdateProgressBar("Merging sibling nodes", m_bMergeSiblingNodes);
    if (m_bMergeSiblingNodes)
    {
        NiOptimize::MergeSiblingNodes(pkScene);
    }

    UpdateProgressBar("Merging sibling NiTriShapes",
        m_bMergeSiblingTriShapes);
    if (m_bMergeSiblingTriShapes)
    {
        NiOptimize::MergeSiblingTriShapes(pkScene, m_bNoMergeOnAlpha);
    }

    UpdateProgressBar("Removing unnecessary normals", 
        m_bRemoveUnnecessaryNormals);
    if (m_bRemoveUnnecessaryNormals)
    {
        NiOptimize::RemoveUnnecessaryNormals(pkScene);
    }

    UpdateProgressBar("Removing unnecessary NiVisControllers", 
        m_bRemoveUnnecessaryVisControllers);
    if (m_bRemoveUnnecessaryVisControllers)
    {
        NiOptimize::RemoveUnnecessaryVisControllers(pkScene);
    }

    UpdateProgressBar("Removing unneccessary NiLookAtControllers",
        m_bRemoveUnnecessaryLookAtControllers);
    if (m_bRemoveUnnecessaryLookAtControllers)
    {
        NiOptimize::RemoveUnnecessaryLookAtControllers(pkScene);
    }

    UpdateProgressBar("Removing hidden bone geometry", 
        m_bRemoveHiddenBoneGeometry);
    if (m_bRemoveHiddenBoneGeometry)
    {
        NiOptimize::RemoveHiddenBoneGeometry(pkScene);
    }

    UpdateProgressBar("Reducing animation keys", m_bReduceAnimationKeys);
    if (m_bReduceAnimationKeys)
    {
        NiOptimize::ReduceAnimationKeys(pkScene, 
            m_fAnimationKeyReductionTolerance);
    }

    UpdateProgressBar("Cleaning up animation keys", true);
    NiOptimize::CleanUpAnimationKeys(pkScene);

    UpdateProgressBar("Sorting siblings by texturing property", 
        m_bSortByTexturingProp);
    if (m_bSortByTexturingProp)
    {
        NiOptimize::SortByTexturingProp(pkScene);
    }

    // should be done before optimizing skin data
    UpdateProgressBar("Optimizing NiTriShapes", m_bOptimizeTriShapes);
    if (m_bOptimizeTriShapes)
    {
        NiOptimize::OptimizeTriShapes(pkScene);
    }

    // should be done after optimizing trishapes but before optimizing
    // skin data
    UpdateProgressBar("Creating bone LOD controllers", true);
    NiBoneLODController::ProcessScene(pkScene);

    // should be done before stripify
    // The fifth argument to OptimizeSkinData used to be console-specific,
    // but the cosoles that needed it to be false are no longer supported.
    UpdateProgressBar("Optimizing skin data", true);
    NiOptimize::OptimizeSkinData(pkScene, m_bExportHardwareSkin, 
        m_bExportSoftwareSkin, m_uiBonesPerPartition, true);

    // must be done after creation of skin partitions
    UpdateProgressBar("Welding skinned objects", m_bWeldSkin);
    if (m_bWeldSkin)
    {
        NiOptimize::WeldSkin(pkScene);
    }

    UpdateProgressBar("Naming unnamed objects", m_bNameUnnamedObjects);
    if (m_bNameUnnamedObjects)
    {
        NiOptimize::NameUnnamedObjects(pkScene);
    }

    UpdateProgressBar("Removing names", m_bRemoveNames);
    if (m_bRemoveNames)
    {
        NiOptimize::RemoveNames(pkScene);
    }

    UpdateProgressBar("Removing unnecessary extra data",
        m_bRemoveUnnecessaryExtraData );
    if (m_bRemoveUnnecessaryExtraData)
    {
        NiOptimize::RemoveUnnecessaryExtraData(pkScene, m_bRemoveSgoKeepTags);
    }

    CloseProgressBar();
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationPlugin::ResetDefaultOptions()
{
    m_bRemoveDupProperties = true;
    m_bRemoveChildlessNodes = true;
    m_bRemoveMultiMtlNodes = true;
    m_bRemoveSingleChildNodes = true;
    m_bMergeSiblingNodes = true;
    m_bOptimizeTriShapes = true;
    m_bMergeSiblingTriShapes = true;
    m_bNoMergeOnAlpha = true;
    m_bRemoveExtraUVSets = true;
    m_bRemoveUnnecessaryNormals = true;
    m_bRemoveUnnecessaryVisControllers = true;
    m_bRemoveNames = false;
    m_bNameUnnamedObjects = false;
    m_bRemoveBadTexturingProps = true;
    m_bRemoveUnnecessaryExtraData = true;
    m_bRemoveSgoKeepTags = true;
    m_bRemoveUnnecessaryLookAtControllers = true;
    m_bRemoveHiddenBoneGeometry = true;
    m_bReduceAnimationKeys = true;
    m_fAnimationKeyReductionTolerance =
        NiOptimize::ms_fEpsilon;
    m_bSortByTexturingProp = false;
    m_bWeldSkin = true;
    m_bExportSoftwareSkin = true;
    m_bExportHardwareSkin = true;
    m_uiBonesPerPartition = 4;
}
