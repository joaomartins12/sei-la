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

#include "NiOptimize.h"
#include <NiStringExtraData.h>
#include <NiLookAtController.h>
#include <NiLookAtInterpolator.h>
#include <NiTransformController.h>
#include <NiPathController.h>
#include <NiLinRotKey.h>
#include <NiLinPosKey.h>
#include <NiLinFloatKey.h>
#include <NiEulerRotKey.h>
#include <NiBezRotKey.h>
#include <NiBezPosKey.h>
#include <NiBezFloatKey.h>
#include <NiIntegerExtraData.h>
#include <NiTriShape.h>
#include <NiParticle.h>
#include <NiShadowGenerator.h>

//---------------------------------------------------------------------------
// Helper functions.
//---------------------------------------------------------------------------
bool NiOptimize::CheckForExtraDataTags(NiAVObject* pkObject)
{
    // Checks for extra data strings that indicate an object should not
    // be removed.

    // Returns: true, if the specified extra data tags exist.
    //          false, otherwise.

    const char* ppcTags[4] = {"fiducial", "sgoKeep", "NiOptimizeKeep",
        "NiBoneLOD#"};

    return CheckForExtraDataTags(pkObject, ppcTags, 4);
}
//---------------------------------------------------------------------------
bool NiOptimize::CheckForExtraDataTags(NiAVObject* pkObject, 
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
void NiOptimize::BuildHashTable(NiAVObject* pkObject,
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
                else if (NiIsKindOf(NiPSysFieldModifier, pkModifier))
                {
                    kMap.SetAt(((NiPSysFieldModifier*) pkModifier)
                        ->GetFieldObj(), true);
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

        // Add targets of shadow generator node lists
        NiShadowGenerator* pkShadow = 
            ((NiDynamicEffect*)pkObject)->GetShadowGenerator();
        if (pkShadow != NULL)
        {
            // Add unaffected casters
            const NiNodeList& pkCasterNodeList = 
                pkShadow->GetUnaffectedCasterNodeList();
            kPos = pkCasterNodeList.GetHeadPos();
            while (kPos)
            {
                NiNode* pkUnaffectedNode = pkCasterNodeList.GetNext(kPos);
                if (pkUnaffectedNode != NULL)
                    kMap.SetAt(pkUnaffectedNode, true);
            }

            // Add unaffected receivers
            const NiNodeList& pkReceiverNodeList = 
                pkShadow->GetUnaffectedReceiverNodeList();
            kPos = pkReceiverNodeList.GetHeadPos();
            while (kPos)
            {
                NiNode* pkUnaffectedNode = pkReceiverNodeList.GetNext(kPos);
                if (pkUnaffectedNode != NULL)
                    kMap.SetAt(pkUnaffectedNode, true);
            }
        }
    }

    // Add bone nodes.
    NiSkinInstance* pkInstance = NULL;
    NiGeometry* pkGeometry = NiDynamicCast(NiGeometry, pkObject);
    if (pkGeometry)
        pkInstance = pkGeometry->GetSkinInstance();

    if (pkInstance)
    {
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
                        uiChild < spPrimAssociated->GetChildCount(); uiChild++)
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

    // Add Objects used with ABV collision
    if (pcName && (strstr(pcName, "NDLCD ")))
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
void NiOptimize::CollectProperties(NiAVObject* pkObject,
    NiPropertyArray& apkProps, unsigned int uiType)
{
    // Recursively collects all properties of the specified type and stores
    // pointers to them in apkProps.

    // Add this object's property to apkMaterials.
    NiProperty* pkProp = pkObject->GetProperty(uiType);
    if (pkProp)
        apkProps.Add(pkProp);

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if(pkChild)
                CollectProperties(pkChild, apkProps, uiType);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::PropagateProperties(NiAVObject* pkObj1, NiAVObject* pkObj2)
{
    // Propagates properties from pkObj1 to pkObj2.

    // Loop through the properties in the two objects.  If pkObj1 has
    // a property that pkObj2 doesn't, attach that property to pkObj2.
    for (unsigned int ui = 0; ui < NiProperty::MAX_TYPES; ui++)
    {
        NiProperty* pkProp1 = pkObj1->GetProperty(ui);
        NiProperty* pkProp2 = pkObj2->GetProperty(ui);
        if (pkProp1 && !pkProp2)
        {
            // Attach the property to the pkObj2.
            pkObj2->AttachProperty(pkProp1);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::PropagateTransforms(NiAVObject* pkObj1, NiAVObject* pkObj2)
{
    // Propagates transforms from pkObj1 to pkObj2.

    // Get the transforms from the two objects.
    NiPoint3 kTrans1 = pkObj1->GetTranslate();
    NiPoint3 kTrans2 = pkObj2->GetTranslate();
    NiMatrix3 kRot1 = pkObj1->GetRotate();
    NiMatrix3 kRot2 = pkObj2->GetRotate();
    float fScale1 = pkObj1->GetScale();
    float fScale2 = pkObj2->GetScale();

    // Combine the transforms to get the new transforms.
    NiMatrix3 kNewRot = kRot1 * kRot2;
    float fNewScale = fScale1 * fScale2;
    NiPoint3 kNewTrans = kTrans1 + fScale1 * (kRot1 * kTrans2);

    // Set the new transforms.
    pkObj2->SetTranslate(kNewTrans);
    pkObj2->SetRotate(kNewRot);
    pkObj2->SetScale(fNewScale);
}
//---------------------------------------------------------------------------
bool NiOptimize::CompareProperties(NiAVObject* pkObj1, NiAVObject* pkObj2)
{
    // Checks whether or not two objects have the same properties.

    // Returns: true, the properties are the same.
    //          false, the properties are not the same.

    // Loop through all the properties, testing for equality with each one.
    for (unsigned int ui = 0; ui < NiProperty::MAX_TYPES; ui++)
    {
        NiProperty* pkProp1 = pkObj1->GetProperty(ui);
        NiProperty* pkProp2 = pkObj2->GetProperty(ui);

        if (pkProp1 && pkProp2)
        {
            // This may return false even if the two properties are
            // functionally equivalent.  It will always return false if the
            // properties are not the same, though.
            if (!pkProp1->IsEqual(pkProp2))
            {
                return false;
            }
        }
        else if (!pkProp1 && !pkProp2)
        {
            // If neither node has the property, do nothing.
        }
        else
        {
            // If one of the properties is NULL and the other is not,
            // the properties cannot be the same, so return false.
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiOptimize::CompareTransforms(NiAVObject* pkObj1, NiAVObject* pkObj2)
{
    // Checks whether or not two objects have the same transforms.

    // Returns: true, the transforms are the same.
    //          false, the transforms are not the same.

    // Get the transforms from the two nodes.
    NiPoint3 kTrans1 = pkObj1->GetTranslate();
    NiPoint3 kTrans2 = pkObj2->GetTranslate();
    NiMatrix3 kRot1 = pkObj1->GetRotate();
    NiMatrix3 kRot2 = pkObj2->GetRotate();
    float fScale1 = pkObj1->GetScale();
    float fScale2 = pkObj2->GetScale();

    // Return true if all of the transforms are equal.
    if (kTrans1 == kTrans2 &&
        kRot1 == kRot2 &&
        fScale1 == fScale2)
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiOptimize::CanMergeTriShape(NiGeometry* pkObj1, NiGeometry* pkObj2)
{
    // Tests whether or not the two geometry objects can be merged. They can
    // be merged if the following is true:
    // They have:
    //     the same set of materials,
    //     the same number of properties,
    //     the same set of property pointers,
    //     the same number of texture sets,
    //     different NiGeometryData pointers,
    //     the same consistency flags,
    //     the same keep flags,
    //     the same compression flags,
    //     the same set of extra data objects,
    // and they both/neither have:
    //     vertices,
    //     normals,
    //     textures,
    //     colors.
    // and they don't have:
    //     ABV's 
    //     "ABV" in the object name
    //     "NiWalkabout" in the extra data tags
    //     "NiOptimizeDontMerge" in the extra data tags

    // Returns: true, the two objects can be merged.
    //          false, the two objects cannot be merged.

    // Must have the same set of materials.
    bool bMaterialsDifferent = false;
    for (unsigned int uiIndex1 = 0; !bMaterialsDifferent &&
        uiIndex1 < pkObj1->GetMaterialCount(); uiIndex1++)
    {
        const NiMaterialInstance* pkInstance1 = pkObj1->GetMaterialInstance(
            uiIndex1);
        NIASSERT(pkInstance1);
        const unsigned int uiMaterialCount2 = pkObj2->GetMaterialCount();
        unsigned int uiIndex2;
        for (uiIndex2 = 0; uiIndex2 < uiMaterialCount2; uiIndex2++)
        {
            const NiMaterialInstance* pkInstance2 =
                pkObj2->GetMaterialInstance(uiIndex2);
            NIASSERT(pkInstance2);
            if (pkInstance1->GetMaterial() == pkInstance2->GetMaterial() &&
                pkInstance1->GetMaterialExtraData() ==
                pkInstance2->GetMaterialExtraData())
            {
                break;
            }
        }
        if (uiIndex2 == uiMaterialCount2)
        {
            bMaterialsDifferent = true;
        }
    }
    for (unsigned int uiIndex2 = 0; !bMaterialsDifferent &&
        uiIndex2 < pkObj2->GetMaterialCount(); uiIndex2++)
    {
        const NiMaterialInstance* pkInstance2 = pkObj2->GetMaterialInstance(
            uiIndex2);
        NIASSERT(pkInstance2);
        const unsigned int uiMaterialCount1 = pkObj1->GetMaterialCount();
        unsigned int uiIndex1;
        for (uiIndex1 = 0; uiIndex1 < uiMaterialCount1; uiIndex1++)
        {
            const NiMaterialInstance* pkInstance1 =
                pkObj1->GetMaterialInstance(uiIndex1);
            NIASSERT(pkInstance1);
            if (pkInstance2->GetMaterial() == pkInstance1->GetMaterial() &&
                pkInstance2->GetMaterialExtraData() ==
                pkInstance1->GetMaterialExtraData())
            {
                break;
            }
        }
        if (uiIndex1 == uiMaterialCount1)
        {
            bMaterialsDifferent = true;
        }
    }
    if (bMaterialsDifferent)
    {
        return false;
    }

    // Must have the same set of extra data objects.
    bool bExtraDataDifferent = false;
    for (unsigned short usIndex1 = 0; !bExtraDataDifferent &&
        usIndex1 < pkObj1->GetExtraDataSize(); usIndex1++)
    {
        NiExtraData* pkExtraData1 = pkObj1->GetExtraDataAt(usIndex1);
        const unsigned short usExtraDataSize2 = pkObj2->GetExtraDataSize();
        unsigned short usIndex2;
        for (usIndex2 = 0; usIndex2 < usExtraDataSize2; usIndex2++)
        {
            if (pkExtraData1->IsEqual(pkObj2->GetExtraDataAt(usIndex2)))
            {
                break;
            }
        }
        if (usIndex2 == usExtraDataSize2)
        {
            bExtraDataDifferent = true;
        }
    }
    for (unsigned short usIndex2 = 0; !bExtraDataDifferent &&
        usIndex2 < pkObj2->GetExtraDataSize(); usIndex2++)
    {
        NiExtraData* pkExtraData2 = pkObj2->GetExtraDataAt(usIndex2);
        const unsigned short usExtraDataSize1 = pkObj1->GetExtraDataSize();
        unsigned short usIndex1;
        for (usIndex1 = 0; usIndex1 < usExtraDataSize1; usIndex1++)
        {
            if (pkExtraData2->IsEqual(pkObj1->GetExtraDataAt(usIndex1)))
            {
                break;
            }
        }
        if (usIndex1 == usExtraDataSize1)
        {
            bExtraDataDifferent = true;
        }
    }
    if (bExtraDataDifferent)
    {
        return false;
    }

    // Must have the same consistency, keep, and compression flags.
    NiGeometryData* pkData1 = pkObj1->GetModelData();
    NiGeometryData* pkData2 = pkObj2->GetModelData();
    if (pkData1->GetConsistency() != pkData2->GetConsistency())
    {
        return false;
    }
    else if (pkData1->GetConsistency() == NiGeometryData::STATIC &&
        pkData1->GetKeepFlags() != pkData2->GetKeepFlags())
    {
        return false;
    }
    if (pkData1->GetCompressFlags() != pkData2->GetCompressFlags())
    {
        return false;
    }

    // Property lists must have same number of elements.
    if (pkObj1->GetPropertyList().GetSize() !=
        pkObj2->GetPropertyList().GetSize())
    {
        return false;
    }

    // Two AVObjects can merge if they have the same set of property
    // *pointers*.
    NiTListIterator kPos1 = pkObj1->GetPropertyList().GetHeadPos();
    while (kPos1)
    {
        NiPropertyPtr spP1 = pkObj1->GetPropertyList().GetNext(kPos1);
        NiPropertyPtr spP2;

        NiTListIterator kPos2 = pkObj2->GetPropertyList().GetHeadPos();
        while (kPos2)
        {
            spP2 = pkObj2->GetPropertyList().GetNext(kPos2);
            
            if (spP1 && (spP1 == spP2))
                break;
        }
        
        if (spP1 != spP2)
            return false;
    }

    // Objects must both have no Collision Objects
    if(pkObj1->GetCollisionObject() != NULL || 
       pkObj2->GetCollisionObject() != NULL)
        return false;

    // Objects must both (neither) have vertices.
    if ((pkObj1->GetVertices() == 0) != (pkObj2->GetVertices() == 0))
    {
        return false;
    }

    // Objects must both (neither) have normals.
    if ((pkObj1->GetNormals() == 0) != (pkObj2->GetNormals() == 0))
    {
        return false;
    }

    // Objects must have same NormalBinormalTangent generation method.
    if (pkObj1->GetNormalBinormalTangentMethod() != 
        pkObj2->GetNormalBinormalTangentMethod())
    {
        return false;
    }

    // Objects must both (neither) have textures.
    if ((pkObj1->GetTextures() == 0) != (pkObj2->GetTextures() == 0))
    {
        return false;
    }

    // Objects must have same number of texture sets.
    if (pkObj1->GetTextureSets() != pkObj2->GetTextureSets())
    {
        return false;
    }

    // Objects must both (neither) have colors.
    if ((pkObj1->GetColors() == 0) != (pkObj2->GetColors() == 0))
    {
        return false;
    }

    const char* pcName = pkObj1->GetName();
    if (pcName && strncmp(pcName, "ABV", 3) == 0)
        return false;
    pcName = pkObj2->GetName();
    if (pcName && strncmp(pcName, "ABV", 3) == 0)
        return false;

    const char* ppcTags[2] = { "NiWalkabout" , 
        "NiOptimizeDontMerge" };

    if (CheckForExtraDataTags(pkObj1, ppcTags, 2) ||
        CheckForExtraDataTags(pkObj2, ppcTags, 2))
    {
        return false;
    }

    // If the above tests are all passed, the two objects can be merged.
    return true;
}
//---------------------------------------------------------------------------
NiTriShapePtr NiOptimize::MergeTriShapes(
    const NiTriShapePtrArray& aspMergeShapes)
{
    // Merges all the NiTriShape objects in aspMergeShapes and returns the
    // first NiTriShape in aspMergeShapes containing all the merged data.
    // Transforms on all NiTriShape objects are pushed into the vertex and
    // normal data.

    // Return without merging if only one TriShape is in the array.
    if (aspMergeShapes.GetEffectiveSize() == 1)
    {
        return aspMergeShapes.GetAt(0);
    }

    NiGeometryData::Consistency eMergedConsistency = NiGeometryData::STATIC;
    unsigned int uiCompressFlags = NiGeometryData::COMPRESS_ALL;
    unsigned int uiKeepFlags = 0;

    unsigned int uiTotalVertices = 0;
    unsigned int uiTotalTriangles = 0;
    unsigned int uiTotalTriListLength = 0;

    unsigned int uiSize = 0;
    
    unsigned int ui;
    for (ui = 0; ui < aspMergeShapes.GetEffectiveSize(); ui++)
    {
        NiTriShape* pkShape = aspMergeShapes.GetAt(ui);
        NiTriShapeData* pkData = (NiTriShapeData*) pkShape->GetModelData();
        NIASSERT(NiIsKindOf(NiTriShapeData, pkData));

        NiGeometryData::Consistency eConsistency = pkData->GetConsistency();
        if (eConsistency == NiGeometryData::VOLATILE)
        {
            eMergedConsistency = NiGeometryData::VOLATILE;
        }
        else if (eConsistency == NiGeometryData::MUTABLE &&
            eMergedConsistency != NiGeometryData::VOLATILE)
        {
            eMergedConsistency = NiGeometryData::MUTABLE;
        }

        uiCompressFlags &= pkData->GetCompressFlags();
        uiKeepFlags |= pkData->GetKeepFlags();

        uiTotalVertices += pkData->GetVertexCount();
        uiTotalTriangles += pkData->GetTriangleCount();
        uiTotalTriListLength += pkData->GetTriListLength();
    }

    // Assert if the TriShapes are empty.
    NIASSERT(uiTotalVertices != 0 && uiTotalTriangles != 0);

    // Assert if any of the quantities is larger than an unsigned short.
    unsigned int uiMaxUnsignedShort = (unsigned int) ((unsigned short) (~0));
    NIASSERT(uiTotalVertices < uiMaxUnsignedShort &&
        uiTotalTriangles < uiMaxUnsignedShort);

    unsigned short usTotalVertices = (unsigned short) uiTotalVertices;
    unsigned short usTotalTriangles = (unsigned short) uiTotalTriangles;

    // Allocate space for the new connectivity list.
    unsigned short* pusTotalTriList =
        NiAlloc(unsigned short, uiTotalTriListLength);
    NIASSERT(pusTotalTriList);

    NiTriShapePtr spShape0 = aspMergeShapes.GetAt(0);

    // Allocate the new shape components.
    NiPoint3* pkTotalVertex = 0;
    if (spShape0->GetVertices())
    {
        pkTotalVertex = NiNew NiPoint3[usTotalVertices];
        NIASSERT(pkTotalVertex);
    }
    NiPoint3* pkTotalNormal = 0;
    NiGeometryData::DataFlags eNBTMethod = 
        spShape0->GetNormalBinormalTangentMethod();
    unsigned int uiNormalFactor = 
        (eNBTMethod == NiGeometryData::NBT_METHOD_NONE ? 1 : 3);
    if (spShape0->GetNormals())
    {
        pkTotalNormal = NiNew NiPoint3[usTotalVertices * uiNormalFactor];
        NIASSERT(pkTotalNormal);
    }
    NiPoint2* pkTotalTexture = 0;
    if (spShape0->GetTextures())
    {
        pkTotalTexture =
            NiNew NiPoint2[spShape0->GetTextureSets() * usTotalVertices];
        NIASSERT(pkTotalTexture);
    }
    NiColorA* pkTotalColor = 0;
    if (spShape0->GetColors())
    {
        pkTotalColor = NiNew NiColorA[usTotalVertices];
        NIASSERT(pkTotalColor);
    }

    // Merge all shape components.
    // Note:  The memory copies here work because of the 
    // 'struct'-like nature of NiPoint[23] and NiColorA.
    // The more object-oriented way is to loop through the
    // arrays of objects and do an assignment (catches any
    // side effects due object copies, although there are
    // none for NiPoint[23] and NiColorA).

    unsigned short usVertexIndex = 0;
    unsigned int uiListIndex = 0;

    for (ui = 0; ui < aspMergeShapes.GetEffectiveSize(); ui++)
    {
        NiTriShapePtr spShape = aspMergeShapes.GetAt(ui);
        unsigned short usVertexCount = spShape->GetVertexCount();
        unsigned short usNumTexSets = spShape->GetTextureSets();

        if (pkTotalVertex)
        {
            // Move transforms for spShape into vertex data.
            NiTransform kTransform;
            kTransform.m_Translate = spShape->GetTranslate();
            kTransform.m_Rotate = spShape->GetRotate();
            kTransform.m_fScale = spShape->GetScale();
            NiPoint3* pkVertices = NULL;
            bool bDeleteArray = false;
            if (kTransform.m_Translate == NiPoint3::ZERO &&
                kTransform.m_Rotate == NiMatrix3::IDENTITY &&
                kTransform.m_fScale == 1.0f)
            {
                pkVertices = spShape->GetVertices();
            }
            else
            {
                pkVertices = NiNew NiPoint3[usVertexCount];
                NiProcessorSpecificCode::TransformPoints(usVertexCount,
                    (const float*) spShape->GetVertices(),
                    (float*) pkVertices, &kTransform);
                bDeleteArray = true;
            }
            NIASSERT(pkVertices);

            // Copy spShape vertex data.
            uiSize = usVertexCount * sizeof(pkTotalVertex[0]);
            NiMemcpy(pkTotalVertex + usVertexIndex, pkVertices, uiSize);
            if (bDeleteArray)
            {
                NiDelete[] pkVertices;
            }
        }
        if (pkTotalNormal)
        {
            // Move transforms for spShape into normal data.
            NiMatrix3 kRotate = spShape->GetRotate();
            NiPoint3* pkNormals = NULL;
            bool bDeleteArray = false;
            if (kRotate == NiMatrix3::IDENTITY)
            {
                pkNormals = spShape->GetNormals();
            }
            else
            {
                pkNormals = NiNew NiPoint3[usVertexCount * uiNormalFactor];
                NiProcessorSpecificCode::TransformVectors(
                    usVertexCount * uiNormalFactor,
                    (const float*) spShape->GetNormals(),
                    (float*) pkNormals, &kRotate);
                bDeleteArray = true;
            }
            NIASSERT(pkNormals);

            // Copy spShape normal data.
            uiSize = usVertexCount * sizeof(pkTotalNormal[0]);
            NiMemcpy(pkTotalNormal + usVertexIndex, pkNormals, uiSize);
            if (uiNormalFactor == 3)
            {
                uiSize = usVertexCount * sizeof(pkTotalNormal[0]);
                NiMemcpy(pkTotalNormal + usTotalVertices + usVertexIndex,
                    pkNormals + usVertexCount, uiSize);

                NiMemcpy(pkTotalNormal + 2 * usTotalVertices + usVertexIndex, 
                    pkNormals + 2 * usVertexCount, uiSize);
            }
            if (bDeleteArray)
            {
                NiDelete[] pkNormals;
            }
        }
        if (pkTotalColor)
        {
            // Copy spShape color data.
            NiColorA* pkColors = spShape->GetColors();
            NIASSERT(pkColors);
            uiSize = usVertexCount * sizeof(pkTotalColor[0]);
            NiMemcpy(pkTotalColor + usVertexIndex, pkColors, uiSize);
        }
        if (pkTotalTexture)
        {
            unsigned short usTex;
            for (usTex = 0; usTex < usNumTexSets; usTex++)
            {
                // Copy spShape texture data.
                NiPoint2* pkUVs = spShape->GetTextureSet(usTex);
                NIASSERT(pkUVs);
                uiSize = usVertexCount * sizeof(pkTotalTexture[0]);
                NiMemcpy(pkTotalTexture + usVertexIndex + (usTotalVertices *
                    usTex), pkUVs, uiSize);
            }
        }

        // Copy spShape tri list data.
        const unsigned short* pusTriList = spShape->GetTriList();
        NIASSERT(pusTriList);
        for (unsigned int uiCount = 0; uiCount < spShape->GetTriListLength();
            uiCount++)
        {
            pusTotalTriList[uiListIndex++] = pusTriList[uiCount] +
                usVertexIndex;
        }


        usVertexIndex += usVertexCount;
    }

    // Reset transforms on spShape0.
    spShape0->SetTranslate(NiPoint3::ZERO);
    spShape0->SetRotate(NiMatrix3::IDENTITY);
    spShape0->SetScale(1.0f);

    // Update spShape0 model data.
    NiTriShapeData* pkNewData = NiNew NiTriShapeData(usTotalVertices,
        pkTotalVertex, pkTotalNormal, pkTotalColor, pkTotalTexture,
        spShape0->GetTextureSets(), eNBTMethod, usTotalTriangles, 
        pusTotalTriList);
    NIASSERT(pkNewData);
/*    pkNewData->MarkAsChanged(NiGeometryData::VERTEX_MASK |
        NiGeometryData::NORMAL_MASK | NiGeometryData::COLOR_MASK |
        NiGeometryData::TEXTURE_MASK |
        NiTriBasedGeomData::TRIANGLE_INDEX_MASK |
        NiTriBasedGeomData::TRIANGLE_COUNT_MASK);
*/
    pkNewData->SetConsistency(eMergedConsistency);
    pkNewData->SetCompressFlags(uiCompressFlags);
    pkNewData->SetKeepFlags(uiKeepFlags);
    spShape0->SetModelData(pkNewData);

    RemoveDegenerateTriangles(spShape0);
    CollapseVertices(spShape0);

    return spShape0;
}
//---------------------------------------------------------------------------
NiTexturingProperty* NiOptimize::FindTexturingProperty(NiAVObject* pkObject)
{
    // Finds the NiTexturingProperty attached to the object or one
    // of its ancestors.

    // Returns: a pointer to the NiTexturingProperty found or NULL if one
    //          was not found.

    NiProperty* pkProp = pkObject->GetProperty(NiProperty::TEXTURING);
    if (!pkProp)
    {
        NiNode* pkParent = pkObject->GetParent();
        if (pkParent)
            return FindTexturingProperty(pkParent);
        else
            return NULL;
    }

    return (NiTexturingProperty*) pkProp;
}
//---------------------------------------------------------------------------
void NiOptimize::FindInstances(const NiGeometryData* pkGeometryDataToFind,
    const NiAVObject* pkObjectToSearchUnder,
    NiTPrimitiveArray<NiGeometry*>* pkInstances)
{
    NIASSERT(pkGeometryDataToFind);
    NIASSERT(pkObjectToSearchUnder);
    
    //check for geometry
    if(NiIsKindOf(NiGeometry, pkObjectToSearchUnder) && 
        !NiIsKindOf(NiParticles, pkObjectToSearchUnder))
    {
        NiGeometry* pkGeometry = NiDynamicCast(NiGeometry, 
    pkObjectToSearchUnder);
        NIASSERT(pkGeometry);

        NiGeometryData* pkCurrentGeometryData = pkGeometry->GetModelData();
        //is this Geometry data the same as the one we are testing for
        if(pkCurrentGeometryData == pkGeometryDataToFind)
        {
            //update instance pointer and to instance count
            pkInstances->Add(pkGeometry);
        }
    }
    
    NiNode* pkNode = NiDynamicCast(NiNode, pkObjectToSearchUnder);
    if(pkNode)
    {
        // Recurse over children.
        unsigned int uiChildCount = pkNode->GetArrayCount();
        NiAVObject* pkChild = NULL;

        for(unsigned int uiChildIndex = 0; uiChildIndex < uiChildCount; 
            uiChildIndex++)
        {
            pkChild = pkNode->GetAt(uiChildIndex);
            if (pkChild)
            {
                FindInstances(pkGeometryDataToFind, pkChild, pkInstances);
            }
        }
    }
}

//---------------------------------------------------------------------------
NiAVObject* NiOptimize::FindRoot(const NiAVObject* pkNode)
{
    NIASSERT(pkNode);

    while(pkNode->GetParent())
    {
        pkNode = pkNode->GetParent();
    }

    return const_cast<NiAVObject*>(pkNode);
}

//---------------------------------------------------------------------------
bool NiOptimize::CheckForTransformControllers(NiAVObject* pkObject,
    bool bCheckUpTree)
{
    // Searches for keyframe controllers or path controller on the current
    // object and, if bCheckUpTree is true, on its ancestors.

    // Returns: true, if keyframe controllers or path controllers exist on
    //              this object or anywhere above it if bCheckUpTree is
    //              specified.
    //          false, otherwise.

    NiTransformController* pkKFCtlr =
        NiGetController(NiTransformController, pkObject);
    NiPathController* pkPathCtlr = NiGetController(NiPathController,
        pkObject);

    if (pkKFCtlr || pkPathCtlr)
        return true;

    // Search up the scene graph if desired.
    if (bCheckUpTree)
    {
        NiNode* pkParent = pkObject->GetParent();
        if (pkParent)
        {
            if (CheckForTransformControllers(pkParent, true))
                return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceLinRotKeys(unsigned int uiNumKeys, NiRotKey* pkKeys,
    unsigned int& uiNumNewKeys, NiRotKey*& pkNewKeys, float fRotationTolerance,
    NiAVObject* pkObject)
{
    // Removes redundant NiLinRotKeys.

    // Rotation keys featuring quaternions can only be checked for exact
    // redundancies. A tolerance cannot be specified.

    NiTPrimitiveSet<NiLinRotKey*>  apkNewKeys;
    NiLinRotKey* pkPrevKey = NULL;
    NiLinRotKey* pkNextKey = NULL;
    unsigned char ucSize = NiRotKey::GetKeySize(NiRotKey::LINKEY);
    
    // Keep the first key.
    NiLinRotKey* pkKey = (NiLinRotKey*) pkKeys->GetKeyAt(0, ucSize);
    pkPrevKey = pkKey;
    apkNewKeys.Add(pkKey);

    unsigned int ui;
    for (ui = 1; ui < uiNumKeys - 1; ui++)
    {
        pkKey = (NiLinRotKey*) pkKeys->GetKeyAt(ui, ucSize);
        pkNextKey = (NiLinRotKey*) pkKeys->GetKeyAt(ui + 1, ucSize);

        // Check for exact duplicates.
        if (QuatCloseTo(pkPrevKey->GetQuaternion(), pkKey->GetQuaternion(), 
            fRotationTolerance) &&
            QuatCloseTo(pkKey->GetQuaternion(), pkNextKey->GetQuaternion(), 
            fRotationTolerance))
        {
            // Duplicate key; don't keep.
            continue;
        }

        // Check against interpolated rotation.
        float fTimeDiff31 = pkNextKey->GetTime() - pkPrevKey->GetTime();
        float fRatio = (pkKey->GetTime() - pkPrevKey->GetTime()) /
            fTimeDiff31;

        // if the angle between the opposing quaternions is obtuse, we cannot
        // remove this key
        bool bAcute;
        bAcute = (NiQuaternion::Dot(pkPrevKey->GetQuaternion(), 
            pkNextKey->GetQuaternion()) > 0.0f);

        NiQuaternion kInterpQuat = NiQuaternion::Slerp(fRatio,
            pkPrevKey->GetQuaternion(), pkNextKey->GetQuaternion());
        if (QuatCloseTo(pkKey->GetQuaternion(), kInterpQuat, 
            fRotationTolerance) && bAcute)
        {
            // Unnecessary key in interpolation; don't keep.
            continue;
        }

        pkPrevKey = pkKey;
        apkNewKeys.Add(pkKey);
    }

    if (uiNumKeys > 1)
    {
        // Keep the last key.
        pkKey = (NiLinRotKey*) pkKeys->GetKeyAt(uiNumKeys - 1, ucSize);
        apkNewKeys.Add(pkKey);
    }

    // Reduce keys if only two and identical.
    if (apkNewKeys.GetSize() == 2)
    {
        if (QuatCloseTo(apkNewKeys.GetAt(0)->GetQuaternion(),
            apkNewKeys.GetAt(1)->GetQuaternion(), 
            fRotationTolerance))
        {
            apkNewKeys.RemoveAt(1);
        }
    }

    // Create the new key array.
    uiNumNewKeys = apkNewKeys.GetSize();
    pkNewKeys = NULL;
    if (uiNumNewKeys > 0)
    {
        pkNewKeys = NiNew NiLinRotKey[uiNumNewKeys];
        NiAnimationKey::CopyFunction pfnCopyFunc =
            NiAnimationKey::GetCopyFunction(NiAnimationKey::ROTKEY,
            NiAnimationKey::LINKEY);
        NIASSERT(pfnCopyFunc);
        for (ui = 0; ui < apkNewKeys.GetSize(); ui++)
        {
            pfnCopyFunc(pkNewKeys->GetKeyAt(ui, ucSize), 
                apkNewKeys.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceLinPosKeys(unsigned int uiNumKeys, NiPosKey* pkKeys,
    unsigned int& uiNumNewKeys, NiPosKey*& pkNewKeys, float fTolerance,
    NiAVObject* pkObject)
{
    // Removes redundant NiLinPosKeys.

    NiTPrimitiveSet<NiLinPosKey*>  apkNewKeys;
    NiLinPosKey* pkPrevKey = NULL;
    NiLinPosKey* pkNextKey = NULL;

    unsigned char ucSize = NiPosKey::GetKeySize(NiPosKey::LINKEY);

    // Keep the first key.
    NiLinPosKey* pkKey = (NiLinPosKey*) pkKeys->GetKeyAt(0, ucSize);
    pkPrevKey = pkKey;
    apkNewKeys.Add(pkKey);


    unsigned int ui;
    for (ui = 1; ui < uiNumKeys - 1; ui++)
    {
        pkKey = (NiLinPosKey*) pkKeys->GetKeyAt(ui, ucSize);
        pkNextKey = (NiLinPosKey*) pkKeys->GetKeyAt(ui + 1, ucSize);

        // Check for exact duplicates.
        if (Point3CloseTo(pkPrevKey->GetPos(), pkKey->GetPos()) &&
            Point3CloseTo(pkKey->GetPos(), pkNextKey->GetPos()))
        {
            // Duplicate key; don't keep.
            continue;
        }

        // Check against interpolated position.
        float fTimeDiff31 = pkNextKey->GetTime() - pkPrevKey->GetTime();
        float fRatio = (pkKey->GetTime() - pkPrevKey->GetTime()) /
            fTimeDiff31;
        NiPoint3 kPosDiff31 = pkNextKey->GetPos() - pkPrevKey->GetPos();
        NiPoint3 kInterpPos = pkPrevKey->GetPos() + fRatio * kPosDiff31;
        if ((pkKey->GetPos() - kInterpPos).Length() <=
            fTolerance * kPosDiff31.Length())
        {
            // Unnecessary key in interpolation; don't keep.
            continue;
        }

        pkPrevKey = pkKey;
        apkNewKeys.Add(pkKey);
    }

    if (uiNumKeys > 1)
    {
        // Keep the last key.
        pkKey = (NiLinPosKey*) pkKeys->GetKeyAt(uiNumKeys - 1, ucSize);
        apkNewKeys.Add(pkKey);
    }

    // Reduce keys if only two and identical.
    if (apkNewKeys.GetSize() == 2)
    {
        if (Point3CloseTo(apkNewKeys.GetAt(0)->GetPos(),
            apkNewKeys.GetAt(1)->GetPos()))
        {
            apkNewKeys.RemoveAt(1);
        }
    }

    // Create the new key array.
    uiNumNewKeys = apkNewKeys.GetSize();
    pkNewKeys = NULL;
    if (uiNumNewKeys > 0)
    {
        pkNewKeys = NiNew NiLinPosKey[uiNumNewKeys];
        NiAnimationKey::CopyFunction pfnCopyFunc =
            NiAnimationKey::GetCopyFunction(NiAnimationKey::POSKEY,
            NiAnimationKey::LINKEY);
        NIASSERT(pfnCopyFunc);
        for (ui = 0; ui < apkNewKeys.GetSize(); ui++)
        {
            pfnCopyFunc(pkNewKeys->GetKeyAt(ui, ucSize), 
                apkNewKeys.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceLinFloatKeys(unsigned int uiNumKeys,
    NiFloatKey* pkKeys, unsigned int& uiNumNewKeys,
    NiFloatKey*& pkNewKeys, float fTolerance)
{
    // Removes redundant NiLinFloatKeys.

    NiTPrimitiveSet<NiLinFloatKey*> apkNewKeys;
    NiLinFloatKey* pkPrevKey = NULL;
    NiLinFloatKey* pkNextKey = NULL;

    unsigned char ucSize = NiFloatKey::GetKeySize(NiFloatKey::LINKEY);

    // Keep the first key.
    NiLinFloatKey* pkKey = (NiLinFloatKey*) pkKeys->GetKeyAt(0, ucSize);
    pkPrevKey = pkKey;
    apkNewKeys.Add(pkKey);

    unsigned int ui;
    for (ui = 1; ui < uiNumKeys - 1; ui++)
    {
        pkKey = (NiLinFloatKey*) pkKeys->GetKeyAt(ui, ucSize);
        pkNextKey = (NiLinFloatKey*) pkKeys->GetKeyAt(ui + 1, ucSize);

        // Check for exact duplicates.
        if (CloseTo(pkPrevKey->GetValue(), pkKey->GetValue()) &&
            CloseTo(pkKey->GetValue(), pkNextKey->GetValue()))
        {
            // Duplicate key; don't keep.
            continue;
        }

        // Check against interpolated value.
        float fTimeDiff31 = pkNextKey->GetTime() - pkPrevKey->GetTime();
        float fRatio = (pkKey->GetTime() - pkPrevKey->GetTime()) /
            fTimeDiff31;
        float fValueDiff31 = pkNextKey->GetValue() - pkPrevKey->GetValue();
        float fInterpValue = pkPrevKey->GetValue() + fRatio * fValueDiff31;
        if (CloseTo(pkKey->GetValue(), fInterpValue, fTolerance *
            (float) fabs(fValueDiff31)))
        {
            // Unnecessary key in interpolation; don't keep.
            continue;
        }

        pkPrevKey = pkKey;
        apkNewKeys.Add(pkKey);
    }

    if (uiNumKeys > 1)
    {
        // Keep the last key.
        pkKey = (NiLinFloatKey*) pkKeys->GetKeyAt(uiNumKeys - 1, ucSize);
        apkNewKeys.Add(pkKey);
    }

    // Reduce keys if only two and identical.
    if (apkNewKeys.GetSize() == 2)
    {
        if (CloseTo(apkNewKeys.GetAt(0)->GetValue(),
            apkNewKeys.GetAt(1)->GetValue()))
        {
            apkNewKeys.RemoveAt(1);
        }
    }

    // Create the new key array.
    uiNumNewKeys = apkNewKeys.GetSize();
    pkNewKeys = NULL;
    if (uiNumNewKeys > 0)
    {
        pkNewKeys = NiNew NiLinFloatKey[uiNumNewKeys];
        NiAnimationKey::CopyFunction pfnCopyFunc =
            NiAnimationKey::GetCopyFunction(NiAnimationKey::FLOATKEY,
            NiAnimationKey::LINKEY);
        NIASSERT(pfnCopyFunc);
        for (ui = 0; ui < apkNewKeys.GetSize(); ui++)
        {
            pfnCopyFunc(pkNewKeys->GetKeyAt(ui, ucSize),
                apkNewKeys.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceEulerRotKeys(unsigned int uiNumKeys,
    NiRotKey* pkKeys, float fTolerance)
{
    unsigned char ucSize = NiRotKey::GetKeySize(NiRotKey::EULERKEY);

    // Removes redundant NiEulerRotKeys.
    NIASSERT(uiNumKeys == 1);
    NiEulerRotKey* pkKey = (NiEulerRotKey*) pkKeys->GetKeyAt(0, ucSize);

    for (unsigned char ucIndex = 0; ucIndex < 3; ucIndex++)
    {
        bool bKeysReduced = false;
        unsigned int uiNumFloatKeys = pkKey->GetNumKeys(ucIndex);
        NiFloatKey* pkFloatKeys = pkKey->GetKeys(ucIndex);
        NiFloatKey::KeyType eFloatKeyType = pkKey->GetType(ucIndex);
        unsigned int uiNumNewFloatKeys = 0;
        NiFloatKey* pkNewFloatKeys = NULL;

        if (uiNumFloatKeys > 1)
        {
            switch(eFloatKeyType)
            {
                case NiFloatKey::LINKEY:
                    ReduceLinFloatKeys(uiNumFloatKeys, pkFloatKeys,
                        uiNumNewFloatKeys, pkNewFloatKeys, fTolerance);
                    bKeysReduced = true;
                    break;
                case NiFloatKey::BEZKEY:
                    ReduceBezFloatKeys(uiNumFloatKeys, pkFloatKeys,
                        uiNumNewFloatKeys, pkNewFloatKeys, fTolerance);
                    bKeysReduced = true;
                    break;
                default:
                    break;
            }
        }
        if (bKeysReduced)
        {
            pkKey->ReplaceKeys(ucIndex, uiNumNewFloatKeys, pkNewFloatKeys);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceBezRotKeys(unsigned int uiNumKeys, NiRotKey* pkKeys,
    unsigned int& uiNumNewKeys, NiRotKey*& pkNewKeys, float fRotationTolerance,
    NiAVObject* pkObject)    
{
    // Removes redundant NiBezRotKeys.

    // Rotation keys featuring quaternions can only be checked for exact
    // redundancies. A tolerance cannot be specified.

    NiTPrimitiveSet<NiBezRotKey*> apkNewKeys;
    NiBezRotKey* pkPrevKey = NULL;
    NiBezRotKey* pkNextKey = NULL;

    // Get fill derived values function.
    NiRotKey::FillDerivedValsFunction pfnFillDerived =
        NiRotKey::GetFillDerivedFunction(NiAnimationKey::BEZKEY);
    NIASSERT(pfnFillDerived);

    unsigned char ucSize = NiRotKey::GetKeySize(NiRotKey::BEZKEY);

    // Keep the first key.
    NiBezRotKey* pkKey = (NiBezRotKey*) pkKeys->GetKeyAt(0, ucSize);
    pkPrevKey = pkKey;
    apkNewKeys.Add(pkKey);

    unsigned int ui;
    for (ui = 1; ui < uiNumKeys - 1; ui++)
    {
        pkKey = (NiBezRotKey*) pkKeys->GetKeyAt(ui, ucSize);
        pkNextKey = (NiBezRotKey*) pkKeys->GetKeyAt(ui + 1, ucSize);

        // Interpolate seven times into original curve.
        float fTimeDiff21 = pkKey->GetTime() - pkPrevKey->GetTime();
        float fTimeDiff32 = pkNextKey->GetTime() - pkKey->GetTime();
        float fHalfTimeDiff21 = fTimeDiff21 * 0.5f;
        float fHalfTimeDiff32 = fTimeDiff32 * 0.5f;
        float fQuarterTimeDiff21 = fHalfTimeDiff21 * 0.5f;
        float fQuarterTimeDiff32 = fHalfTimeDiff32 * 0.5f;
        unsigned int uiTemp;
        NiQuaternion kOrigInterp1 = NiRotKey::GenInterp(pkPrevKey->GetTime() +
            fQuarterTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiQuaternion kOrigInterp2 = NiRotKey::GenInterp(pkPrevKey->GetTime() +
            fHalfTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiQuaternion kOrigInterp3 = NiRotKey::GenInterp(pkKey->GetTime() -
            fQuarterTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiQuaternion kOrigInterp4 = pkKey->GetQuaternion();
        NiQuaternion kOrigInterp5 = NiRotKey::GenInterp(pkKey->GetTime() +
            fQuarterTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiQuaternion kOrigInterp6 = NiRotKey::GenInterp(pkKey->GetTime() +
            fHalfTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiQuaternion kOrigInterp7 = NiRotKey::GenInterp(pkNextKey->GetTime() -
            fQuarterTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);

        // Construct new array with just prev and next keys.
        float fTimeDiff31 = pkNextKey->GetTime() - pkPrevKey->GetTime();
        float fRatio12 = fTimeDiff21 / fTimeDiff31;
        float fRatio23 = fTimeDiff32 / fTimeDiff31;
        NiBezRotKey akTempKeys[2];
        akTempKeys[0].SetTime(pkPrevKey->GetTime());
        akTempKeys[0].SetQuaternion(pkPrevKey->GetQuaternion());
        akTempKeys[1].SetTime(pkNextKey->GetTime());
        akTempKeys[1].SetQuaternion(pkNextKey->GetQuaternion());
        pfnFillDerived(&akTempKeys[0], 2, ucSize);
        
        // Interpolate seven times into the new curve.
        NiQuaternion kNewInterp1 = NiRotKey::GenInterp(pkPrevKey->GetTime() +
            fQuarterTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiQuaternion kNewInterp2 = NiRotKey::GenInterp(pkPrevKey->GetTime() +
            fHalfTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiQuaternion kNewInterp3 = NiRotKey::GenInterp(pkKey->GetTime() -
            fQuarterTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiQuaternion kNewInterp4 = NiRotKey::GenInterp(pkKey->GetTime(),
            &akTempKeys[0], NiAnimationKey::BEZKEY, 2, uiTemp = 0, ucSize);
        NiQuaternion kNewInterp5 = NiRotKey::GenInterp(pkKey->GetTime() +
            fQuarterTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiQuaternion kNewInterp6 = NiRotKey::GenInterp(pkKey->GetTime() +
            fHalfTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiQuaternion kNewInterp7 = NiRotKey::GenInterp(pkNextKey->GetTime() -
            fQuarterTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);

        // Compare the interpolated values.
        if (QuatCloseTo(kOrigInterp1, kNewInterp1, fRotationTolerance) &&
            QuatCloseTo(kOrigInterp2, kNewInterp2, fRotationTolerance) &&
            QuatCloseTo(kOrigInterp3, kNewInterp3, fRotationTolerance) &&
            QuatCloseTo(kOrigInterp4, kNewInterp4, fRotationTolerance) &&
            QuatCloseTo(kOrigInterp5, kNewInterp5, fRotationTolerance) &&
            QuatCloseTo(kOrigInterp6, kNewInterp6, fRotationTolerance) &&
            QuatCloseTo(kOrigInterp7, kNewInterp7, fRotationTolerance))
        {
            // Unnecessary key in interpolation; don't keep.
            continue;
        }

        pkPrevKey = pkKey;
        apkNewKeys.Add(pkKey);
    }

    if (uiNumKeys > 1)
    {
        // Keep the last key.
        pkKey = (NiBezRotKey*) pkKeys->GetKeyAt(uiNumKeys - 1, ucSize);
        apkNewKeys.Add(pkKey);
    }

    // Create the new key array.
    uiNumNewKeys = apkNewKeys.GetSize();
    pkNewKeys = NULL;
    if (uiNumNewKeys > 0)
    {
        pkNewKeys = NiNew NiBezRotKey[uiNumNewKeys];
        NiAnimationKey::CopyFunction pfnCopyFunc =
            NiAnimationKey::GetCopyFunction(NiAnimationKey::ROTKEY,
            NiAnimationKey::BEZKEY);
        NIASSERT(pfnCopyFunc);
        for (ui = 0; ui < apkNewKeys.GetSize(); ui++)
        {
            pfnCopyFunc(pkNewKeys->GetKeyAt(ui, ucSize), 
                apkNewKeys.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceBezPosKeys(unsigned int uiNumKeys, NiPosKey* pkKeys,
    unsigned int& uiNumNewKeys, NiPosKey*& pkNewKeys, float fTolerance,
    NiAVObject* pkObject)
{
    // Removes redundant NiBezPosKeys.

    NiTPrimitiveSet<NiBezPosKey*>  apkNewKeys;
    NiBezPosKey* pkPrevKey = NULL;
    NiBezPosKey* pkNextKey = NULL;

    // Get fill derived values function.
    NiPosKey::FillDerivedValsFunction pfnFillDerived =
        NiPosKey::GetFillDerivedFunction(NiAnimationKey::BEZKEY);
    NIASSERT(pfnFillDerived);

    unsigned char ucSize = NiPosKey::GetKeySize(NiPosKey::BEZKEY);

    // Keep the first key.
    NiBezPosKey* pkKey = (NiBezPosKey*) pkKeys->GetKeyAt(0, ucSize);
    pkPrevKey = pkKey;
    apkNewKeys.Add(pkKey);


    unsigned int ui;
    for (ui = 1; ui < uiNumKeys - 1; ui++)
    {
        pkKey = (NiBezPosKey*) pkKeys->GetKeyAt(ui, ucSize);
        pkNextKey = (NiBezPosKey*) pkKeys->GetKeyAt(ui + 1, ucSize);

        // Interpolate seven times into original curve.
        float fTimeDiff21 = pkKey->GetTime() - pkPrevKey->GetTime();
        float fTimeDiff32 = pkNextKey->GetTime() - pkKey->GetTime();
        float fHalfTimeDiff21 = fTimeDiff21 * 0.5f;
        float fHalfTimeDiff32 = fTimeDiff32 * 0.5f;
        float fQuarterTimeDiff21 = fHalfTimeDiff21 * 0.5f;
        float fQuarterTimeDiff32 = fHalfTimeDiff32 * 0.5f;
        unsigned int uiTemp;
        NiPoint3 kOrigInterp1 = NiPosKey::GenInterp(pkPrevKey->GetTime() +
            fQuarterTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiPoint3 kOrigInterp2 = NiPosKey::GenInterp(pkPrevKey->GetTime() +
            fHalfTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiPoint3 kOrigInterp3 = NiPosKey::GenInterp(pkKey->GetTime() -
            fQuarterTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiPoint3 kOrigInterp4 = pkKey->GetPos();
        NiPoint3 kOrigInterp5 = NiPosKey::GenInterp(pkKey->GetTime() +
            fQuarterTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiPoint3 kOrigInterp6 = NiPosKey::GenInterp(pkKey->GetTime() +
            fHalfTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        NiPoint3 kOrigInterp7 = NiPosKey::GenInterp(pkNextKey->GetTime() -
            fQuarterTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);

        // Construct new array with just prev and next keys.
        float fTimeDiff31 = pkNextKey->GetTime() - pkPrevKey->GetTime();
        float fRatio12 = fTimeDiff21 / fTimeDiff31;
        float fRatio23 = fTimeDiff32 / fTimeDiff31;
        NiBezPosKey akTempKeys[2];
        akTempKeys[0].SetTime(pkPrevKey->GetTime());
        akTempKeys[0].SetPos(pkPrevKey->GetPos());
        akTempKeys[0].SetOutTan(pkPrevKey->GetOutTan() * fRatio12);
        akTempKeys[1].SetTime(pkNextKey->GetTime());
        akTempKeys[1].SetPos(pkNextKey->GetPos());
        akTempKeys[1].SetInTan(pkNextKey->GetInTan() * fRatio23);
        pfnFillDerived(&akTempKeys[0], 2, ucSize);
        
        // Interpolate seven times into the new curve.
        NiPoint3 kNewInterp1 = NiPosKey::GenInterp(pkPrevKey->GetTime() +
            fQuarterTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiPoint3 kNewInterp2 = NiPosKey::GenInterp(pkPrevKey->GetTime() +
            fHalfTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiPoint3 kNewInterp3 = NiPosKey::GenInterp(pkKey->GetTime() -
            fQuarterTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiPoint3 kNewInterp4 = NiPosKey::GenInterp(pkKey->GetTime(),
            &akTempKeys[0], NiAnimationKey::BEZKEY, 2, uiTemp = 0, ucSize);
        NiPoint3 kNewInterp5 = NiPosKey::GenInterp(pkKey->GetTime() +
            fQuarterTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiPoint3 kNewInterp6 = NiPosKey::GenInterp(pkKey->GetTime() +
            fHalfTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        NiPoint3 kNewInterp7 = NiPosKey::GenInterp(pkNextKey->GetTime() -
            fQuarterTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);

        // Compare the interpolated values.
        NiPoint3 kPosDiff31 = pkNextKey->GetPos() - pkPrevKey->GetPos();
        float fPosTolerance = fTolerance * kPosDiff31.Length();
        if ((kOrigInterp1 - kNewInterp1).Length() <= fPosTolerance &&
            (kOrigInterp2 - kNewInterp2).Length() <= fPosTolerance &&
            (kOrigInterp3 - kNewInterp3).Length() <= fPosTolerance &&
            (kOrigInterp4 - kNewInterp4).Length() <= fPosTolerance &&
            (kOrigInterp5 - kNewInterp5).Length() <= fPosTolerance &&
            (kOrigInterp6 - kNewInterp6).Length() <= fPosTolerance &&
            (kOrigInterp7 - kNewInterp7).Length() <= fPosTolerance)
        {
            // Unnecessary key in interpolation; don't keep.
            continue;
        }

        pkPrevKey = pkKey;
        apkNewKeys.Add(pkKey);
    }

    if (uiNumKeys > 1)
    {
        // Keep the last key.
        pkKey = (NiBezPosKey*) pkKeys->GetKeyAt(uiNumKeys - 1, ucSize);
        apkNewKeys.Add(pkKey);
    }

    // Create the new key array.
    uiNumNewKeys = apkNewKeys.GetSize();
    pkNewKeys = NULL;
    if (uiNumNewKeys > 0)
    {
        pkNewKeys = NiNew NiBezPosKey[uiNumNewKeys];
        NiAnimationKey::CopyFunction pfnCopyFunc =
            NiAnimationKey::GetCopyFunction(NiAnimationKey::POSKEY,
            NiAnimationKey::BEZKEY);
        NIASSERT(pfnCopyFunc);
        for (ui = 0; ui < apkNewKeys.GetSize(); ui++)
        {
            pfnCopyFunc(pkNewKeys->GetKeyAt(ui, ucSize), 
                apkNewKeys.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceBezFloatKeys(unsigned int uiNumKeys,
    NiFloatKey* pkKeys, unsigned int& uiNumNewKeys,
    NiFloatKey*& pkNewKeys, float fTolerance)
{
    // Removes redundant NiBezFloatKeys.

    NiTPrimitiveSet<NiBezFloatKey*>  apkNewKeys;
    NiBezFloatKey* pkPrevKey = NULL;
    NiBezFloatKey* pkNextKey = NULL;

    unsigned char ucSize = NiFloatKey::GetKeySize(NiFloatKey::BEZKEY);

    // Keep the first key.
    NiBezFloatKey* pkKey = (NiBezFloatKey*) pkKeys->GetKeyAt(0, ucSize);
    pkPrevKey = pkKey;
    apkNewKeys.Add(pkKey);

    unsigned int ui;
    for (ui = 1; ui < uiNumKeys - 1; ui++)
    {
        pkKey = (NiBezFloatKey*) pkKeys->GetKeyAt(ui, ucSize);
        pkNextKey = (NiBezFloatKey*) pkKeys->GetKeyAt(ui + 1, ucSize);

        // Interpolate seven times into original curve.
        float fTimeDiff21 = pkKey->GetTime() - pkPrevKey->GetTime();
        float fTimeDiff32 = pkNextKey->GetTime() - pkKey->GetTime();
        float fHalfTimeDiff21 = fTimeDiff21 * 0.5f;
        float fHalfTimeDiff32 = fTimeDiff32 * 0.5f;
        float fQuarterTimeDiff21 = fHalfTimeDiff21 * 0.5f;
        float fQuarterTimeDiff32 = fHalfTimeDiff32 * 0.5f;
        unsigned int uiTemp;
        float fOrigInterp1 = NiFloatKey::GenInterp(pkPrevKey->GetTime() +
            fQuarterTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        float fOrigInterp2 = NiFloatKey::GenInterp(pkPrevKey->GetTime() +
            fHalfTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        float fOrigInterp3 = NiFloatKey::GenInterp(pkKey->GetTime() -
            fQuarterTimeDiff21, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        float fOrigInterp4 = pkKey->GetValue();
        float fOrigInterp5 = NiFloatKey::GenInterp(pkKey->GetTime() +
            fQuarterTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        float fOrigInterp6 = NiFloatKey::GenInterp(pkKey->GetTime() +
            fHalfTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);
        float fOrigInterp7 = NiFloatKey::GenInterp(pkNextKey->GetTime() -
            fQuarterTimeDiff32, pkKeys, NiAnimationKey::BEZKEY, uiNumKeys,
            uiTemp = 0, ucSize);

        // Construct new array with just prev and next keys.
        float fTimeDiff31 = pkNextKey->GetTime() - pkPrevKey->GetTime();
        float fRatio12 = fTimeDiff21 / fTimeDiff31;
        float fRatio23 = fTimeDiff32 / fTimeDiff31;
        NiBezFloatKey akTempKeys[2];
        akTempKeys[0].SetTime(pkPrevKey->GetTime());
        akTempKeys[0].SetValue(pkPrevKey->GetValue());
        akTempKeys[0].SetOutTan(pkPrevKey->GetOutTan() * fRatio12);
        akTempKeys[1].SetTime(pkNextKey->GetTime());
        akTempKeys[1].SetValue(pkNextKey->GetValue());
        akTempKeys[1].SetInTan(pkNextKey->GetInTan() * fRatio23);
        // There are no derived values to fill here.
        
        // Interpolate seven times into the new curve.
        float fNewInterp1 = NiFloatKey::GenInterp(pkPrevKey->GetTime() +
            fQuarterTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        float fNewInterp2 = NiFloatKey::GenInterp(pkPrevKey->GetTime() +
            fHalfTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        float fNewInterp3 = NiFloatKey::GenInterp(pkKey->GetTime() -
            fQuarterTimeDiff21, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        float fNewInterp4 = NiFloatKey::GenInterp(pkKey->GetTime(),
            &akTempKeys[0], NiAnimationKey::BEZKEY, 2, uiTemp = 0, ucSize);
        float fNewInterp5 = NiFloatKey::GenInterp(pkKey->GetTime() +
            fQuarterTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        float fNewInterp6 = NiFloatKey::GenInterp(pkKey->GetTime() +
            fHalfTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);
        float fNewInterp7 = NiFloatKey::GenInterp(pkNextKey->GetTime() -
            fQuarterTimeDiff32, &akTempKeys[0], NiAnimationKey::BEZKEY, 2,
            uiTemp = 0, ucSize);

        // Compare the interpolated values.
        float fValueDiff31 = pkNextKey->GetValue() - pkPrevKey->GetValue();
        float fValueTolerance = fTolerance * (float) fabs(fValueDiff31);
        if (CloseTo(fOrigInterp1, fNewInterp1, fValueTolerance) &&
            CloseTo(fOrigInterp2, fNewInterp2, fValueTolerance) &&
            CloseTo(fOrigInterp3, fNewInterp3, fValueTolerance) &&
            CloseTo(fOrigInterp4, fNewInterp4, fValueTolerance) &&
            CloseTo(fOrigInterp5, fNewInterp5, fValueTolerance) &&
            CloseTo(fOrigInterp6, fNewInterp6, fValueTolerance) &&
            CloseTo(fOrigInterp7, fNewInterp7, fValueTolerance))
        {
            // Unnecessary key in interpolation; don't keep.
            continue;
        }

        pkPrevKey = pkKey;
        apkNewKeys.Add(pkKey);
    }

    if (uiNumKeys > 1)
    {
        // Keep the last key.
        pkKey = (NiBezFloatKey*) pkKeys->GetKeyAt(uiNumKeys - 1, ucSize);
        apkNewKeys.Add(pkKey);
    }

    // Create the new key array.
    uiNumNewKeys = apkNewKeys.GetSize();
    pkNewKeys = NULL;
    if (uiNumNewKeys > 0)
    {
        pkNewKeys = NiNew NiBezFloatKey[uiNumNewKeys];
        NiAnimationKey::CopyFunction pfnCopyFunc =
            NiAnimationKey::GetCopyFunction(NiAnimationKey::FLOATKEY,
            NiAnimationKey::BEZKEY);
        NIASSERT(pfnCopyFunc);
        for (ui = 0; ui < apkNewKeys.GetSize(); ui++)
        {
            pfnCopyFunc(pkNewKeys->GetKeyAt(ui, ucSize), apkNewKeys.GetAt(ui));
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::CollectBoneChildren(NiAVObject* pkObject,
    NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Stores a pointer to each child of bones in the scene to kMap.

    NiSkinInstance* pkInstance = NULL;
    NiGeometry* pkGeometry = NiDynamicCast(NiGeometry, pkObject);
    if (pkGeometry)
    {
        pkInstance = pkGeometry->GetSkinInstance();
    }

    if (pkInstance)
    {
        unsigned int uiBones = pkInstance->GetSkinData()->GetBoneCount();
        NiAVObject*const* ppkBones = pkInstance->GetBones();
        
        for (unsigned int ui = 0; ui < uiBones; ui++)
        {
            NiAVObject* pkBone = ppkBones[ui];
            if (pkBone && NiIsKindOf(NiNode, pkBone))
            {
                NiNode* pkNode = (NiNode*) pkBone;
                for (unsigned int uj = 0; uj < pkNode->GetArrayCount(); uj++)
                {
                    NiAVObject* pkChild = pkNode->GetAt(uj);
                    if (pkChild)
                    {
                        kMap.SetAt(pkChild, true);
                    }
                }
            }
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                CollectBoneChildren(pkChild, kMap);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::OptimizeSkinInstance(NiTriBasedGeom* pkGeom,
    NiSkinInstance* pkSkinInstance, bool bHardwareSkin, bool bSoftwareSkin, 
    unsigned int uiBonesPerPartition, bool bForcePalette)
{
    // Creates a skin partition and removes bone vert data, if appropriate.

    unsigned int uiMaterialCount = pkGeom->GetMaterialCount();
    if (uiMaterialCount != 0)
    {
        // By default shaders hardware skin
        bHardwareSkin = true;

        // Force Software Skinning IF the bones per partition are 0
        NiIntegerExtraData* pkIntED = NiDynamicCast(NiIntegerExtraData, 
            pkGeom->GetExtraData(NI_BONES_PER_PARTITION_KEY));
        if (pkIntED && (pkIntED->GetValue() <= 0))
        {
            bHardwareSkin = false;
            bSoftwareSkin = true;
        }
    }

    if (bHardwareSkin)
    {
        if (!pkSkinInstance->GetSkinPartition())
        {
            NiIntegerExtraData* pkIntED = NiDynamicCast(NiIntegerExtraData, 
                pkGeom->GetExtraData(NI_BONES_PER_PARTITION_KEY));
            if (pkIntED)
            {
                unsigned int uiTemp = pkIntED->GetValue();
                if (uiTemp > 0)
                    uiBonesPerPartition = uiTemp;
            }

            NiSkinPartition* pkSkinPartition = NiNew NiSkinPartition;
            NiTriBasedGeomData* pkData =
                (NiTriBasedGeomData*) pkGeom->GetModelData();

            pkSkinPartition->MakePartitions(pkData,
                pkSkinInstance->GetSkinData(), uiBonesPerPartition, 4,
                bForcePalette);

            pkSkinInstance->SetSkinPartition(pkSkinPartition);
        }
    }
    else
    {
        pkGeom->GetModelData()->SetConsistency(NiGeometryData::VOLATILE);
    }

    if (!bSoftwareSkin)
    {
       pkSkinInstance->GetSkinData()->FreeBoneVertData();
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::NonDegenerateTriangle(unsigned int uiI0, unsigned uiI1,
    unsigned int uiI2, const NiPoint3& kP0, const NiPoint3& kP1, 
    const NiPoint3& kP2)
{
    // check for degenerate connectivity
    if (uiI0 == uiI1 || uiI0 == uiI2 || uiI1 == uiI2)
    {
         return false;
    }

    // check for short edges
    const float fMinLenSqr = 1e-30f;

    NiPoint3 e10 = kP1 - kP0;
    float fLenSqrE10 = e10.SqrLength();
    if (fLenSqrE10 < fMinLenSqr)
        return false;

    NiPoint3 e20 = kP2 - kP0;
    float fLenSqrE20 = e20.SqrLength();
    if (fLenSqrE20 < fMinLenSqr)
        return false;

    NiPoint3 e21 = kP2 - kP1;
    float fLenSqrE21 = e21.SqrLength();
    if (fLenSqrE21 < fMinLenSqr)
        return false;

    // check for slivers
    const float fMinEdgeRatio = 1e-7f;
    const float fMaxEdgeRatio = 1e+7f;

    float fEdgeRatio = fLenSqrE10 / fLenSqrE20;
    if (fEdgeRatio < fMinEdgeRatio || fEdgeRatio > fMaxEdgeRatio)
        return false;

    // check for small area triangles
    const float fMinSineSqr = 1e-7f;

    NiPoint3 cross = e10.Cross(e20);
    float fLenSqrCross = cross.SqrLength();
    if (fLenSqrCross < fLenSqrE10 * fLenSqrE20 * fMinSineSqr)
        return false;

    return true;
}
//---------------------------------------------------------------------------
// NiTriShape optimization helper functions
//---------------------------------------------------------------------------
void NiOptimize::RemoveUnusedVertices(NiTriShape* pkShape, 
    unsigned short** ppusNewToOld)
{
    NIASSERT(pkShape);

    NiTriShapeData* pkData = (NiTriShapeData*)pkShape->GetModelData();

    unsigned short usVertices = pkData->GetVertexCount();
    int* pOldToNew = NiAlloc(int, usVertices);
    NIASSERT(pOldToNew);

    unsigned short usV;
    for (usV = 0; usV < usVertices; usV++)
        pOldToNew[usV] = -1;

    unsigned short usNewVertices = 0;

    unsigned int uiTriListLength = pkData->GetTriListLength();
    unsigned short* pTriList = pkData->GetTriList();
    unsigned int i;
    for (i = 0; i < uiTriListLength; i++)
    {
        if (pOldToNew[pTriList[i]] == -1)
            pOldToNew[pTriList[i]] = usNewVertices++;

        pTriList[i] = pOldToNew[pTriList[i]];
    }

    if ( ppusNewToOld )
    {
        *ppusNewToOld = NiAlloc(unsigned short, usNewVertices);
        NIASSERT(*ppusNewToOld);

        for (usV = 0; usV < usVertices; usV++)
        {
            if ( pOldToNew[usV] != -1 )
                (*ppusNewToOld)[pOldToNew[usV]] = usV;
        }
    }

    // Save a copy for our controller to work with.
    NiPoint3* pOldVertex = pkData->GetVertices();
    NiPoint3* pOld = NiNew NiPoint3[usVertices];
    for(i=0; i < usVertices; i++)
        pOld[i] = pOldVertex[i];

    ReallocateVertexArrays(pkData, usNewVertices, pOldToNew);

    NiSkinInstance* pkSkin = pkShape->GetSkinInstance();
    if (pkSkin)
    {
        NiSkinData* pkSkinData = pkSkin->GetSkinData();
        pkSkinData->RemoveUnusedVertices(pOldToNew);

        // Under normal circumstances, a skin partition has not been
        // created when this code runs. If it has, it now contains bogus
        // data. So delete it.
        pkSkin->SetSkinPartition(NULL);
        pkSkinData->SetSkinPartition(NULL, true);
    }

    NiDelete[] pOld;
    NiFree(pOldToNew);
}
//---------------------------------------------------------------------------
void NiOptimize::ReallocateVertexArrays(NiTriShapeData* pkData,
    unsigned short usNewVertices, int* pOldToNew)
{
    NIASSERT(pkData);

    unsigned short usVertices = pkData->GetVertexCount();
    NiPoint3* pOldVertex = pkData->GetVertices();
    NiPoint3* pOldNormal = pkData->GetNormals();
    NiColorA* pOldColor = pkData->GetColors();
    unsigned short usTextureSets = pkData->GetTextureSets();
    NiPoint2* pOldTexture = pkData->GetTextures();

    NiGeometryData::DataFlags eNBTMethod = 
        pkData->GetNormalBinormalTangentMethod();
    unsigned int uiNormalFactor = 
        (eNBTMethod == NiGeometryData::NBT_METHOD_NONE ? 1 : 3);

    NiPoint3* pNewVertex = NiNew NiPoint3[usNewVertices];
    NIASSERT(pNewVertex);

    NiPoint3* pNewNormal = 0;
    if (pOldNormal)
    {
        pNewNormal = NiNew NiPoint3[usNewVertices * uiNormalFactor];
        NIASSERT(pNewNormal);
    }

    NiColorA* pNewColor = 0;
    if (pOldColor)
    {
        pNewColor = NiNew NiColorA[usNewVertices];
        NIASSERT(pNewColor);
    }

    NiPoint2* pNewTexture = 0;
    unsigned int uiQuantity;
    if (pOldTexture)
    {
        uiQuantity = usNewVertices * usTextureSets;
        pNewTexture = NiNew NiPoint2[uiQuantity];
        NIASSERT(pNewTexture);
    }
    unsigned short i;
    for (i = 0; i < usVertices; i++)
    {
        int iNewIndex = pOldToNew[i];

        if (iNewIndex != -1)
        {
            pNewVertex[iNewIndex] = pOldVertex[i];

            if (pOldNormal)
            {
                for (unsigned short n = 0; n < uiNormalFactor; n++)
                {
                    unsigned int i0 = iNewIndex + n * usNewVertices;
                    unsigned int i1 = i + n * usVertices;
                    pNewNormal[i0] = pOldNormal[i1];
                }
            }

            if (pOldColor)
                pNewColor[iNewIndex] = pOldColor[i];

            if (pOldTexture)
            {
                for (unsigned short t = 0; t < usTextureSets; t++)
                {
                    unsigned int i0 = iNewIndex + t * usNewVertices;
                    unsigned int i1 = i + t * usVertices;
                    pNewTexture[i0] = pOldTexture[i1];
                }
            }
        }
    }

    // Must cast to access hidden NiGeometryData::Replace (hidden by
    // NiTriShapeData::Replace)
    ((NiGeometryData*)pkData)->Replace(usNewVertices, pNewVertex, 
        pNewNormal, pNewColor, pNewTexture, usTextureSets, eNBTMethod);
}
//---------------------------------------------------------------------------
bool NiOptimize::CanRemoveNormals(NiGeometry* pkObject)
{
    // Determines if normals can be removed from the geometry object.

    // Returns: true, normals can be removed from pkObject.
    //          false, normals should not be removed.

    // Observe flag that allows app to explicitly request that normals be
    // preserved.
    NiGeometryData* pkModelData = pkObject->GetModelData();
    if (pkModelData->GetKeepFlags() & NiGeometryData::KEEP_NORM)
    {
        return false;
    }

    // If the object has an environment map, do not remove normals.
    NiDynamicEffectState* pkEffectState = pkObject->GetEffectState();
    if (pkEffectState && pkEffectState->GetEnvironmentMap())
    {
        return false;
    }

    // If the NBT generation mode is anything other than NBT_METHOD_NONE, do
    // not remove normals.
    if (pkObject->GetNormalBinormalTangentMethod() !=
        NiGeometryData::NBT_METHOD_NONE)
    {
        return false;
    }

    // If the object has an NiTexturingProperty attached that has a base map
    // and an apply mode of APPLY_REPLACE, normals can be removed.
    NiTexturingProperty* pkTexProp = (NiTexturingProperty*)
        pkObject->GetProperty(NiProperty::TEXTURING);
    if (pkTexProp && pkTexProp->GetBaseMap() && pkTexProp->GetApplyMode() ==
        NiTexturingProperty::APPLY_REPLACE)
    {
        return true;
    }

    // If the object has an NiVertexColorProperty attached whose lighting
    // mode is emissive, normals can be removed.
    NiVertexColorProperty* pkVertColProp = (NiVertexColorProperty*)
        pkObject->GetProperty(NiProperty::VERTEX_COLOR);
    if (pkVertColProp && pkVertColProp->GetLightingMode() ==
        NiVertexColorProperty::LIGHTING_E)
    {
        return true;
    }

    // Otherwise, do not remove normals.
    return false;
}
//---------------------------------------------------------------------------

