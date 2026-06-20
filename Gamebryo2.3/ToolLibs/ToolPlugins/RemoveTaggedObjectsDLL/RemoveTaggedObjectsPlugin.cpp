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
#include "RemoveTaggedObjectsPlugin.h"
#include <NiParticle.h>

#define SAMPLE_FRAMES_PER_SECOND 30.0f

//NiAllocatorDeclareStatics(unsigned int, 2048)

// RTTI implementation macro.
NiImplementRTTI(RemoveTaggedObjectsPlugin, NiPlugin);

#define REMOVE_OBJECT_STRING "NiRemoveObject"

//---------------------------------------------------------------------------
RemoveTaggedObjectsPlugin::RemoveTaggedObjectsPlugin() : NiPlugin(
    "Remove Tagged Objects Plug-in",
    "2.0",
    "Removes objects from the scene graph",
    "Removes objects that are tagged with \"NiRemoveObject\" from the scene "
    "graph.\nWill not remove skinned geometry, bones, or objects that are "
    "referenced by other objects in the scene graph.\nIf an NiNode is tagged "
    "to be removed, it and all of its children that can be removed will be.")
{
}
//---------------------------------------------------------------------------
NiPluginInfo* RemoveTaggedObjectsPlugin::GetDefaultPluginInfo()
{
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("RemoveTaggedObjectsPlugin");
    pkPluginInfo->SetType("PROCESS");
    
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool RemoveTaggedObjectsPlugin::HasManagementDialog()
{
    return false;
}
//---------------------------------------------------------------------------
bool RemoveTaggedObjectsPlugin::DoManagementDialog(
    NiPluginInfo* pkInitialInfo, NiWindowRef hWndParent)
{
    return false;
}
//---------------------------------------------------------------------------
bool RemoveTaggedObjectsPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (pkInfo->GetClassName() == "RemoveTaggedObjectsPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr RemoveTaggedObjectsPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    NiSceneGraphSharedData* pkSGSharedData = (NiSceneGraphSharedData*) 
        GetSharedData(NiGetSharedDataType(NiSceneGraphSharedData));
    if (!pkSGSharedData)
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
            pkLogger->LogElement("RemoveTaggedObjectsError", "The Remove "
                "Tagged Objects Process Plug-in could not find the "
                "NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The Remove Tagged Objects Process Plug-in could "
                "not find the NiSceneGraphSharedData!", 
                "Scene Graph Shared Data Missing");
        }
        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }

    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        RemoveTaggedObjects(pkSGSharedData->GetRootAt(ui));
    }

    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
void RemoveTaggedObjectsPlugin::RemoveTaggedObjects(NiAVObject* pkObject)
{
    NiTPointerMap<NiAVObject*, bool> kMap;
    BuildHashTable(pkObject, kMap);
    NiTPointerList<NiAVObject*> kList;
    BuildRemoveTaggedObjectsList(pkObject, kList, kMap);
    if (NiIsKindOf(NiNode, pkObject))
        RemoveTaggedObjects((NiNode*) pkObject, kList);

}
//---------------------------------------------------------------------------
bool RemoveTaggedObjectsPlugin::RemoveTaggedObjects(NiNode* pkSceneRoot, 
    NiTPointerList<NiAVObject*>& kList)
{
    while (!kList.IsEmpty())
    {
        NiAVObject* pkObj = kList.RemoveHead();
        if (pkObj)
        {
            NiNode* pkNode = NiDynamicCast(NiNode, pkObj);     
            NiNode* pkParentNode = pkObj->GetParent();
        
            if (pkNode != NULL && pkNode->GetChildCount() > 0)
            {
                for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
                {
                    NiAVObjectPtr spChild = pkNode->GetAt(ui);
                    if (pkParentNode && spChild)
                    {
                        if (pkSceneRoot)
                            BakeKeys(pkSceneRoot, pkNode, pkParentNode,
                            spChild);
                        pkNode->DetachChild(spChild);
                        pkParentNode->AttachChild(spChild);
                    }
                }
            }
            
            if (pkParentNode)
            {
                pkParentNode->DetachChild(pkObj);
            }
        
        }
    }

    return true;
}

//---------------------------------------------------------------------------
void RemoveTaggedObjectsPlugin::BakeKeys(NiNode* pkSceneRoot, 
    NiNode* pkCurrentParent, NiNode* pkNewParent, NiAVObject* pkChild)
{
    if (pkSceneRoot && pkCurrentParent && pkNewParent && pkChild)
    {
        float fBegin = NI_INFINITY;
        float fEnd = -NI_INFINITY;

        NiTransformController* pkCurrentParentCtrl = NULL; 
        NiTransformController* pkNewParentCtrl = NULL;
        NiTransformController* pkChildCtrl = NULL;

        pkCurrentParentCtrl = NiGetController(NiTransformController, 
            pkCurrentParent);

        pkNewParentCtrl = NiGetController(NiTransformController, 
            pkNewParent);

        pkChildCtrl = NiGetController(NiTransformController, 
            pkChild);

        if (pkCurrentParentCtrl)
        {
            float fTempBegin = pkCurrentParentCtrl->GetBeginKeyTime();
            float fTempEnd = pkCurrentParentCtrl->GetEndKeyTime();
            if (fBegin > fTempBegin)
                fBegin = fTempBegin;
            if (fEnd < fTempEnd)
                fEnd = fTempEnd;
        }

        /*if (pkNewParentCtrl)
        {
            float fTempBegin = pkNewParentCtrl->GetBeginKeyTime();
            float fTempEnd = pkNewParentCtrl->GetEndKeyTime();
            if (fBegin > fTempBegin)
                fBegin = fTempBegin;
            if (fEnd < fTempEnd)
                fEnd = fTempEnd;
        }*/

        if (pkChildCtrl)
        {
            float fTempBegin = pkChildCtrl->GetBeginKeyTime();
            float fTempEnd = pkChildCtrl->GetEndKeyTime();
            if (fBegin > fTempBegin)
                fBegin = fTempBegin;
            if (fEnd < fTempEnd)
                fEnd = fTempEnd;
        }

        float fTime = fBegin;
        float fIncTime = 1.0f / SAMPLE_FRAMES_PER_SECOND;
        float fNumKeys = (fEnd - fBegin)/fIncTime;

        if (fBegin == NI_INFINITY && fEnd == -NI_INFINITY || fBegin == fEnd)
        {
            NiTransform kChildTransform = pkChild->GetWorldTransform();
            NiTransform kNewParentTransform =
                pkNewParent->GetWorldTransform();
            NiTransform kInvNewParentTransform;
            kNewParentTransform.Invert(kInvNewParentTransform);

            NiTransform kResultTransform =
                kInvNewParentTransform * kChildTransform;

            pkChild->SetTranslate(kResultTransform.m_Translate);
            pkChild->SetRotate(kResultTransform.m_Rotate);
            
        }
        else
        {
            unsigned int uiNumKeys = (unsigned int)(fNumKeys + 1.5f);
            NiLinPosKey* pkPosKeys = NiNew NiLinPosKey[uiNumKeys];
            NiLinRotKey* pkRotKeys = NiNew NiLinRotKey[uiNumKeys];

            NiTransform kChildTransform = pkChild->GetWorldTransform();
            NiTransform kNewParentTransform =
                pkNewParent->GetWorldTransform();
            NiTransform kInvNewParentTransform;
            unsigned int uiIndex = 0;

            NiQuaternion kQuat;

            for (unsigned int ui = 0; ui < uiNumKeys; ui++)
            {
                pkSceneRoot->Update(fTime);
                kChildTransform = pkChild->GetWorldTransform();
                kNewParentTransform = pkNewParent->GetWorldTransform();
                kNewParentTransform.Invert(kInvNewParentTransform);

                NiTransform kResultTransform = kInvNewParentTransform * 
                    kChildTransform;

                // Bake Pos
                pkPosKeys[ui].SetTime(fTime);
                pkPosKeys[ui].SetPos(kResultTransform.m_Translate);
                
                
                // Bake Rots
                kQuat.FromRotation(kResultTransform.m_Rotate);
                pkRotKeys[ui].SetTime(fTime);
                pkRotKeys[ui].SetQuaternion(kQuat);

                fTime += fIncTime;
            }

            if (pkChildCtrl == NULL)
            {
                pkChildCtrl = NiNew NiTransformController;
                pkChildCtrl->SetTarget(pkChild);
               
            }

            if (pkCurrentParentCtrl)
            {
                pkChildCtrl->SetCycleType(
                    pkCurrentParentCtrl->GetCycleType());
            }

            NiTransformInterpolator* pkInterp = NiNew NiTransformInterpolator;
            pkInterp->ReplacePosData(pkPosKeys, uiNumKeys, 
                NiAnimationKey::LINKEY);
            pkInterp->ReplaceRotData(pkRotKeys, uiNumKeys, 
                NiAnimationKey::LINKEY);
            pkChildCtrl->SetInterpolator(pkInterp);
            pkChildCtrl->ResetTimeExtrema();

        }
    }
}

//---------------------------------------------------------------------------
void RemoveTaggedObjectsPlugin::BuildHashTable(NiAVObject* pkObject,
    NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Builds a hash table containing the pointers of objects that should
    // not be removed by removal or merge functions. The following objects
    // are added to kMap:
    // - LookAt targets of NiLookAtControllers
    // - objects referenced by particle systems
    // - affected nodes of dynamic effects
    // - all bones
    // - Biped Footsteps
    // - Objects with ABV's
    // - Objects used with the NiPortal system

    // Add LookAt target.
    NiLookAtController* pkLookAtCtlr = NiGetController(NiLookAtController,
        pkObject);
    if (pkLookAtCtlr)
    {
        NiAVObject* pkLookAt = pkLookAtCtlr->GetLookAt();
        if (pkLookAt)
            kMap.SetAt(pkLookAt, true);
    }

    // Add LookAt target.
    NiTransformController* pkTransformCtlr = NiGetController(
        NiTransformController, pkObject);
    if (pkTransformCtlr)
    {
        NiInterpolator* pkInterp = pkTransformCtlr->GetInterpolator(0);
        if (pkInterp && NiIsKindOf(NiLookAtInterpolator, pkInterp))
        {
            NiLookAtInterpolator* pkLookAtInterp = (NiLookAtInterpolator*)
                pkInterp;
            NiAVObject* pkLookAt = pkLookAtInterp->GetLookAt();
            if (pkLookAt)
                kMap.SetAt(pkLookAt, true);
        }
    }

    // Add objects referenced by particle systems.
    if (NiIsKindOf(NiParticleSystem, pkObject))
    {
        NiParticleSystem* pkPSystem = (NiParticleSystem*) pkObject;
        for (unsigned int ui = 0; ui < pkPSystem->GetModifierCount(); ui++)
        {
            NiPSysModifier* pkModifier = pkPSystem->GetModifierAt(ui);
            if (pkModifier)
            {
                if (NiIsKindOf(NiPSysBombModifier, pkModifier))
                {
                    kMap.SetAt(((NiPSysBombModifier*) pkModifier)
                        ->GetBombObj(), true);
                }
                else if (NiIsKindOf(NiPSysColliderManager, pkModifier))
                {
                    NiPSysCollider* pkCollider = ((NiPSysColliderManager*)
                        pkModifier)->GetColliders();
                    while (pkCollider)
                    {
                        if (NiIsKindOf(NiPSysPlanarCollider, pkCollider))
                        {
                            kMap.SetAt(((NiPSysPlanarCollider*) pkCollider)
                                ->GetColliderObj(), true);
                        }
                        else if (NiIsKindOf(NiPSysSphericalCollider,
                            pkCollider))
                        {
                            kMap.SetAt(((NiPSysSphericalCollider*) pkCollider)
                                ->GetColliderObj(), true);
                        }
                        pkCollider = pkCollider->GetNext();
                    }
                }
                else if (NiIsKindOf(NiPSysGravityModifier, pkModifier))
                {
                    kMap.SetAt(((NiPSysGravityModifier*) pkModifier)
                        ->GetGravityObj(), true);
                }
                else if (NiIsKindOf(NiPSysMeshEmitter, pkModifier))
                {
                    NiPSysMeshEmitter* pkMeshEmitter = (NiPSysMeshEmitter*)
                        pkModifier;
                    for (unsigned int uj = 0;
                        uj < pkMeshEmitter->GetGeometryEmitterCount(); uj++)
                    {
                        NiGeometry* pkGeom = pkMeshEmitter
                            ->GetGeometryEmitter(uj);
                        if (pkGeom)
                        {
                            kMap.SetAt(pkGeom, true);
                        }
                    }
                }
                else if (NiIsKindOf(NiPSysVolumeEmitter, pkModifier))
                {
                    kMap.SetAt(((NiPSysVolumeEmitter*) pkModifier)
                        ->GetEmitterObj(), true);
                }
            }
        }
    }

    // Add NiAVObjects with Collision Object's
    if (pkObject->GetCollisionObject())
    {
        kMap.SetAt(pkObject, true);
    }

    // Add affected nodes of dynamic effects.
    if (NiIsKindOf(NiDynamicEffect, pkObject))
    {
        NiTListIterator kPos = ((NiDynamicEffect*)
            pkObject)->GetAffectedNodeList().GetHeadPos();
        while (kPos)
        {
            NiNode* pkAffectedNode = ((NiDynamicEffect*)
                pkObject)->GetAffectedNodeList().GetNext(kPos);
            if (pkAffectedNode)
                kMap.SetAt(pkAffectedNode, true);
        }

    
        // Add unaffected nodes of dynamic effects.
        kPos = ((NiDynamicEffect*)
            pkObject)->GetUnaffectedNodeList().GetHeadPos();
        while (kPos)
        {
            NiNode* pkUnaffectedNode = ((NiDynamicEffect*)
                pkObject)->GetUnaffectedNodeList().GetNext(kPos);
            if (pkUnaffectedNode)
                kMap.SetAt(pkUnaffectedNode, true);
        }

    }
    // Add bone nodes.
    NiSkinInstance* pkInstance = NULL;
    NiGeometry* pkGeometry = NiDynamicCast(NiGeometry, pkObject);
    if (pkGeometry)
        pkInstance = pkGeometry->GetSkinInstance();

    if (pkInstance)
    {
        // Add object with skin instance.
        kMap.SetAt(pkGeometry, true);

        NiAVObject* pkRootParent = pkInstance->GetRootParent();
        if (pkRootParent)
            kMap.SetAt(pkRootParent, true);
        
        unsigned int uiBones = pkInstance->GetSkinData()->GetBoneCount();
        NiAVObject*const* ppkBones = pkInstance->GetBones();
        
        for (unsigned int ui = 0; ui < uiBones; ui++)
        {
            NiAVObject* pkBone = ppkBones[ui];
            if (pkBone)
                kMap.SetAt(pkBone, true);
        }
    }

/* ///////
    // Add Havok associated nodes.
    NiHavokInfoSim* pkSim = NiDynamicCast(NiHavokInfoSim, pkObject);
    if(pkSim)
    {
        NiTObjectArray<NiHavokRigidCollectionPtr>* pkCols =
            pkSim->GetCollection();
        for (unsigned int uiCol = 0; uiCol < pkCols->GetSize(); uiCol++)
        {
            NiHavokRigidCollection* pkC = pkCols->GetAt(uiCol);
            for(unsigned int uiRB = 0; uiRB < pkC->GetNumOfRigidBodies();
                uiRB++)
            {
                NiHavokRigidBody* pkRBInfo = pkC->GetRigidBodyAt(uiRB);

                NiNodePtr spAssociated = pkRBInfo->GetAssociatedNode();
                kMap.SetAt(spAssociated, true);
                for (unsigned int uiPrim = 0;
                    uiPrim < pkRBInfo->GetNumOfPrimitives(); uiPrim++)
                {
                    NiHavokPrimitive* pkPrimInfo =
                        pkRBInfo->GetPrimitive(uiPrim);
                    NiNodePtr spPrimAssociated =
                        pkPrimInfo->GetAssociatedNode();
                    kMap.SetAt(spPrimAssociated, true);

                    for (unsigned int uiChild = 0;
                        uiChild < spPrimAssociated->GetChildCount();
                        uiChild++)
                    {
                        NiAVObject* pkObj = spPrimAssociated->GetAt(uiChild);
                        kMap.SetAt(pkObj, true);
                    }
                }
            }
        }
    }
*/

    // Add objects used with the NiPortal system.
    const char* pcName = pkObject->GetName();
    if (pcName && (strstr(pcName, "NDLPR") || strstr(pcName, "NDLRG") ||
        strstr(pcName, "NDLRM") || strstr(pcName, "NDLWL")))
    {
        kMap.SetAt(pkObject, true);
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        // Add biped footsteps node.
        NiNode* pkParent = pkNode->GetParent();
        if (pkParent && pkParent->GetName())
        {
            const char* pcParent = pkParent->GetName();

            unsigned int uiLen = strlen(pcParent) + 11;
            char* pcFootsteps = NiAlloc(char, uiLen);
            NiSprintf(pcFootsteps, uiLen, "%s Footsteps", pcParent);
            if (pkNode->GetName() &&
                strcmp(pcFootsteps, pkNode->GetName()) == 0)
            {
                kMap.SetAt(pkNode, true);
            }
            NiFree(pcFootsteps);
        }

        // Recurse over children.
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                BuildHashTable(pkChild, kMap);
        }
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Helper functions.
//---------------------------------------------------------------------------
bool RemoveTaggedObjectsPlugin::CheckForExtraDataTags(NiAVObject* pkObject)
{
    // Checks for extra data strings that indicate an object should not
    // be removed.

    // Returns: true, if the specified extra data tags exist.
    //          false, otherwise.

    const char* ppcTags[3] = {"fiducial", "sgoKeep", "NiOptimizeKeep"};

    return CheckForExtraDataTags(pkObject, ppcTags, 3);
}
//---------------------------------------------------------------------------
bool RemoveTaggedObjectsPlugin::CheckForExtraDataTags(NiAVObject* pkObject, 
    const char* ppcTags[], unsigned int uiNumTags)
{
    // Checks for the string extra data tags in ppcTags

    // Returns: true, if the specified extra data tags exist.
    //          false, otherwise.

    for (unsigned int i=0; i < pkObject->GetExtraDataSize(); i++)
    {
        NiExtraData* pkExtraData = pkObject->GetExtraDataAt(i);

        NiStringExtraData* pkStrExData = NiDynamicCast(NiStringExtraData,
                pkExtraData);
        if (pkStrExData)
        {
            const char* pcString = pkStrExData->GetValue();
            if (pcString)
            {
                for(unsigned int uiLoop = 0; uiLoop < uiNumTags; uiLoop++)
                {
                    if (strstr(pcString, ppcTags[uiLoop]))
                    {
                        return true;   
                    }
                }
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void RemoveTaggedObjectsPlugin::BuildRemoveTaggedObjectsList(
    NiAVObject* pkObject, NiTPointerList<NiAVObject*>& kList,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    if (pkObject == NULL)
        return;

    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode =  (NiNode*) pkObject;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            BuildRemoveTaggedObjectsList(pkNode->GetAt(ui), kList, kMap);
        }
    }

    
    bool bKeep;
    if (kMap.GetAt(pkObject, bKeep))
        return;

    bool bRemoveObject = false;
    NiAVObject* pkParent = pkObject;
    while (pkParent)
    {
        for (unsigned int ui = 0; ui < pkParent->GetExtraDataSize(); ui++)
        {
            NiStringExtraData* pkStringExtraData = NiDynamicCast(
                NiStringExtraData, pkParent->GetExtraDataAt(ui));
            if (pkStringExtraData)
            {
                const char* pcString = pkStringExtraData->GetValue();
                if (pcString && strstr(pcString, REMOVE_OBJECT_STRING))
                {
                    bRemoveObject = true;
                    break;
                }
            }
        }

        if (bRemoveObject)
        {
            break;
        }

        pkParent = pkParent->GetParent();
    }

    if (bRemoveObject)
        kList.AddTail(pkObject);

}
//---------------------------------------------------------------------------
