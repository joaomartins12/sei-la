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
#include <NiSwitchNode.h>
#include <NiTriShape.h>
#include <NiAlphaProperty.h>
#include <NiLookAtInterpolator.h>
#include <NiParticles.h>
#include <NiVisController.h>
#include <NiStringExtraData.h>
#include <NiLookAtController.h>
#include <NiTransformController.h>
#include <NiBoneLODController.h>
#include <NiGeomMorpherController.h>
#include <NiBoolInterpolator.h>
#include <NiTransformInterpolator.h>
#include <NiStepBoolKey.h>

typedef NiTPrimitiveSet<NiTriBasedGeom*>  NiTriBasedGeomSet;
//---------------------------------------------------------------------------
// Optimization functions.
//---------------------------------------------------------------------------
bool NiOptimize::IsPropertyUnique(NiProperty* pkProp, 
    NiTPointerList<NiProperty*>& pkPropList, NiProperty* &pkDupProp)
{
    // Given a property and a list of properties. Check to see if the 
    // given property is a duplicate/Equal to any in the list. Return
    // if a duplicate was found and set the duplicate property. This function
    // assumes the list contains unique properties so it will only return
    // the first duplicate found.

    // Returns: true/false if a duplicate/equal property was found.

    NiTListIterator kIter;

    kIter = pkPropList.GetHeadPos();

    // Loop through the list looking for duplicate properties
    while(kIter != NULL)
    {
        if(pkPropList.Get(kIter)->IsEqual(pkProp))
        {
            pkDupProp = pkPropList.Get(kIter);
            return false;
        }

        kIter = pkPropList.GetNextPos(kIter);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiOptimize::RecursiveRemoveDupProperties(NiAVObject* pkObject,
   NiTPropertyPointerListArray& apaUniqueProps)
{
    // This function will remove duplicate properties from a scene graph.
    // It will check each property of this object against the lists of unique
    // properties. The unique properties consists of a TArray where there is
    // one element for each type of property. And each TArray element
    // consists of a list of unique properties of that type. Each duplicate
    // property will be replaced

    // Check the uniqueness of each property
    NiTListIterator kPos = pkObject->GetPropertyList().GetHeadPos();
    while(kPos)
    {
        NiTListIterator kPosDelete = kPos;
        NiProperty* pkProp = pkObject->GetPropertyList().GetNext(kPos);

        if (pkProp)
        {
            // Get the unique property list for this type of property
            NiTPointerList<NiProperty*>* pkUniqueList = 
                apaUniqueProps.GetAt(pkProp->Type());
            NiProperty* pkDupProp;

            if (IsPropertyUnique(pkProp, *pkUniqueList, pkDupProp))
            {
                pkUniqueList->AddTail(pkProp);
            }
            else
            {
                // call RemovePos instead of DetachProperty to avoid
                // a search
                pkObject->GetPropertyList().RemovePos(kPosDelete);
                pkObject->AttachProperty(pkDupProp);
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
                RecursiveRemoveDupProperties(pkChild, apaUniqueProps);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveDupProperties(NiAVObject* pkObject)
{
    // This function will remove all of the duplicate properties from the 
    // scene graph. We will need to create a structure of record keeping
    // lists to hold unique properties.

    // Create the record keeping lists
   NiTPropertyPointerListArray apaUniqueProps(
        NiProperty::MAX_TYPES);

    int iLoop;
    for(iLoop = 0; iLoop < NiProperty::MAX_TYPES; iLoop++)
    {
        apaUniqueProps.SetAt(iLoop, NiNew NiTPointerList<NiProperty*>);
    }

    
    RecursiveRemoveDupProperties(pkObject, apaUniqueProps);

    // Delete our record keeping lists
    for(iLoop = 0; iLoop < NiProperty::MAX_TYPES; iLoop++)
    {
        NiDelete apaUniqueProps.GetAt(iLoop);
    }

}
//---------------------------------------------------------------------------
void NiOptimize::CollectChildlessNodes(NiNode* pkNode,
    NiNodeArray& apkNodes, const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Detects the presence of childless nodes that can be removed by
    // RemoveChildlessNodes and stores a pointer to each in apkNodes.

    bool bTemp;
    if (pkNode->GetChildCount() == 0 && !CheckForExtraDataTags(pkNode) &&
        !kMap.GetAt(pkNode, bTemp))
    {
        apkNodes.Add(pkNode);
    }

    // Recurse over children.
    for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (NiIsKindOf(NiNode, pkChild))
            {
                CollectChildlessNodes((NiNode*) pkChild, apkNodes, kMap);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::RemoveChildlessNodes(NiNode* pkNode,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Removes all childless nodes. A node will not be removed if it is
    // singled out by BuildHashTable or has an "sgoKeep" extra data tag.

    // Returns: true, parent should delete the object.
    //          false, parent should keep the object.

    // If pkNode is an NiSwitchNode, save the active index.
    int iIndex = -1;
    if (NiIsKindOf(NiSwitchNode, pkNode))
    {
        iIndex = ((NiSwitchNode*) pkNode)->GetIndex();
    }

    // Recurse over children.
    NiTObjectArray<NiAVObjectPtr> aspSaveChildren(50);
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            bool bRemove;
            if (NiIsKindOf(NiNode, pkChild))
            {
                // Recurse on NiNode child.
                bRemove = RemoveChildlessNodes((NiNode*) pkChild, kMap);
            }
            else
            {
                // Child is some other kind of object; don't remove.
                bRemove = false;
            }

            if (bRemove)
            {
                NiNode* pkChildNode = NiDynamicCast(NiNode, pkChild);
                if (pkChildNode)
                {
                    // Save the children of the node.
                    for (unsigned int uj = 0;
                        uj < pkChildNode->GetArrayCount(); uj++)
                    {
                        aspSaveChildren.Add(pkChildNode->DetachChildAt(uj));
                    }
                }
                pkNode->DetachChildAt(ui);
            }
            else
            {
                aspSaveChildren.Add(pkNode->DetachChildAt(ui));
            }
        }
    }

    // Re-attach the saved children to the node.
    NIASSERT(pkNode->GetChildCount() == 0);
    pkNode->RemoveAllChildren();
    for (ui = 0; ui < aspSaveChildren.GetSize(); ui++)
        pkNode->AttachChild(aspSaveChildren.RemoveAt(ui));

    // If pkNode is an NiSwitchNode, restore the saved index.
    if ((iIndex > -1) && (pkNode->GetChildCount() != 0))
    {
        ((NiSwitchNode*) pkNode)->SetIndex(iIndex);
    }

    // Return true if node is childless and can be removed.
    bool bTemp;
    if (pkNode->GetChildCount() == 0 && !CheckForExtraDataTags(pkNode) &&
        !kMap.GetAt(pkNode, bTemp))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiOptimize::CollectMultiMtlNodes(NiNode* pkNode,
    NiNodeArray& apkNodes, const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Detects the presence of multi-material nodes that can be removed by
    // RemoveMultiMtlNodes and stores a pointer to each in apkNodes.

    const char* pcName = pkNode->GetName();
    bool bTemp;
    if (pcName && strcmp(pcName, "__NDL_MultiMtl_Node") == 0 &&
        !CheckForExtraDataTags(pkNode) && !kMap.GetAt(pkNode, bTemp) &&
        !pkNode->GetControllers())
    {
        apkNodes.Add(pkNode);
    }

    // Recurse over children.
    for (unsigned int ui=0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (NiIsKindOf(NiNode, pkChild))
            {
                CollectMultiMtlNodes((NiNode*) pkChild, apkNodes, kMap);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::RemoveMultiMtlNodes(NiNode* pkNode,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Recursively removes all multi-material nodes. A node will not be
    // removed if it is singled out by BuildHashTable, has an "sgoKeep" extra
    // data tag, or has any controllers.

    // Returns: true, pkNode should be deleted.
    //          false, pkNode should not be deleted.

    // If pkNode is an NiSwitchNode, save the active index.
    int iIndex = -1;
    if (NiIsKindOf(NiSwitchNode, pkNode))
    {
        iIndex = ((NiSwitchNode*) pkNode)->GetIndex();
    }

    // Recurse over children.
    NiTObjectArray<NiAVObjectPtr> aspSaveChildren(50);
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            bool bRemove;
            if (NiIsKindOf(NiNode, pkChild))
            {
                // Recurse on NiNode child.
                bRemove = RemoveMultiMtlNodes((NiNode*) pkChild, kMap);
            }
            else
            {
                // Child is some other kind of object; don't remove.
                bRemove = false;
            }

            if (bRemove)
            {
                NiNode* pkChildNode = NiDynamicCast(NiNode, pkChild);
                if (pkChildNode)
                {
                    // Propagate the AppCulled flag to children.
                    bool bAppCulled = pkChildNode->GetAppCulled();

                    // Save the children of the node.
                    for (unsigned int uj = 0;
                        uj < pkChildNode->GetArrayCount(); uj++)
                    {
                        NiAVObjectPtr spGrandchild =
                            pkChildNode->DetachChildAt(uj);
                        if (bAppCulled || spGrandchild->GetAppCulled())
                        {
                            spGrandchild->SetAppCulled(true);
                        }
                        aspSaveChildren.Add(spGrandchild);
                    }
                }
                pkNode->DetachChildAt(ui);
            }
            else
            {
                aspSaveChildren.Add(pkNode->DetachChildAt(ui));
            }
        }
    }

    // Re-attach the saved children to the node.
    NIASSERT(pkNode->GetChildCount() == 0);
    pkNode->RemoveAllChildren();
    for (ui = 0; ui < aspSaveChildren.GetSize(); ui++)
        pkNode->AttachChild(aspSaveChildren.RemoveAt(ui));

    // If pkNode is an NiSwitchNode, restore the saved index.
    if ((iIndex > -1) && (pkNode->GetChildCount() != 0))
    {
        ((NiSwitchNode*) pkNode)->SetIndex(iIndex);
    }

    // Return true if pkNode is a multi-material node that can be removed.
    const char* pcName = pkNode->GetName();
    bool bTemp;
    if (pcName && strcmp(pcName, "__NDL_MultiMtl_Node") == 0 &&
        !CheckForExtraDataTags(pkNode) && !kMap.GetAt(pkNode, bTemp) &&
        !pkNode->GetControllers())
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiOptimize::CollectSingleChildNodes(NiNode* pkNode,
    NiNodeArray& apkNodes, const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Detects the presence of single-child nodes that can be removed by
    // RemoveSingleChildNodes and stores a pointer to each in apkNodes.

    bool bChildCtlrFound = false;
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild && pkChild->GetControllers())
        {
            bChildCtlrFound = true;
            break;
        }
    }

    bool bTemp;
    if (!bChildCtlrFound && !pkNode->GetControllers() &&
        NiIsExactKindOf(NiNode, pkNode) && pkNode->GetChildCount() == 1 &&
        !CheckForExtraDataTags(pkNode) && kMap.GetAt(pkNode, bTemp))
    {
        apkNodes.Add(pkNode);
    }

    // Recurse over children.
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (NiIsKindOf(NiNode, pkChild))
            {
                CollectSingleChildNodes((NiNode*) pkChild, apkNodes, kMap);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::RemoveSingleChildNodes(NiNode* pkNode,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Recursively removes all single-child nodes in the scene of exact type
    // NiNode, propagating their properties, transforms, extra data, and
    // names to the single child. A node will not be removed if it is
    // singled out by BuildHashTable, has an "sgoKeep" extra data tag, has
    // any controllers, or whose child has any controllers.

    // Returns: true, parent should delete the object.
    //          false, parent should keep the object.

    // If pkNode is an NiSwitchNode, save the active index.
    int iIndex = -1;
    if (NiIsKindOf(NiSwitchNode, pkNode))
    {
        iIndex = ((NiSwitchNode*) pkNode)->GetIndex();
    }

    // Recurse over children.
    NiTObjectArray<NiAVObjectPtr> aspSaveChildren(50);
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            bool bRemove;
            if (NiIsKindOf(NiNode, pkChild))
            {
                // Recurse on NiNode child.
                bRemove = RemoveSingleChildNodes((NiNode*) pkChild, kMap);
            }
            else
            {
                // Child is some other kind of object; don't remove.
                bRemove = false;
            }

            if (bRemove)
            {
                NiNode* pkChildNode = NiDynamicCast(NiNode, pkChild);
                if (pkChildNode)
                {
                    // Propagate properties, transforms, extra data, and name
                    // to the child and remove the node.
                    for (unsigned int uj = 0;
                        uj < pkChildNode->GetArrayCount(); uj++)
                    {
                        NiAVObject* pkGrandchild = pkChildNode->GetAt(uj);
                        if (pkGrandchild)
                        {
                            PropagateProperties(pkChildNode, pkGrandchild);
                            PropagateTransforms(pkChildNode, pkGrandchild);

                            for (unsigned int uk=0;
                                uk < pkChildNode->GetExtraDataSize(); uk++)
                            {
                                NiExtraData* pkExtra =
                                    pkChildNode->GetExtraDataAt(uk);
                                pkGrandchild->AddExtraData(pkExtra);
                            }

                            if (!CheckForExtraDataTags(pkGrandchild))
                            {
                                pkGrandchild->SetName(pkChildNode->GetName());
                            }
                            if (pkChildNode->GetAppCulled() ||
                                pkGrandchild->GetAppCulled())
                            {
                                pkGrandchild->SetAppCulled(true);
                            }
                        }
                        aspSaveChildren.Add(pkChildNode->DetachChildAt(uj));
                    }
                }
                pkNode->DetachChildAt(ui);
            }
            else
            {
                aspSaveChildren.Add(pkNode->DetachChildAt(ui));
            }
        }
    }

    // Re-attach the saved children to the node.
    NIASSERT(pkNode->GetChildCount() == 0);
    pkNode->RemoveAllChildren();
    for (ui = 0; ui < aspSaveChildren.GetSize(); ui++)
    {
        pkNode->AttachChild(aspSaveChildren.RemoveAt(ui));
    }

    // If pkNode is an NiSwitchNode, restore the saved index.
    if ((iIndex > -1) && (pkNode->GetChildCount() != 0))
    {
        ((NiSwitchNode*) pkNode)->SetIndex(iIndex);
    }

    bool bChildCtlrFound = false;
    bool bChildExtraDataFound = false;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (pkChild->GetControllers())
            {
                bChildCtlrFound = true;
            }

            for (unsigned int uj = 0; uj < pkChild->GetExtraDataSize(); uj++)
            {
                NiExtraData* pkExtra = pkChild->GetExtraDataAt(uj);
                if (pkNode->GetExtraData(pkExtra->GetName()))
                {
                    bChildExtraDataFound = true;
                }
            }

            if (bChildCtlrFound && bChildExtraDataFound)
            {
                break;
            }
        }

    }
    bool bTemp;
    if (!bChildCtlrFound && !bChildExtraDataFound && !pkNode->GetControllers()
        && NiIsExactKindOf(NiNode, pkNode) && pkNode->GetChildCount() == 1 &&
        !CheckForExtraDataTags(pkNode) && !kMap.GetAt(pkNode, bTemp))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiOptimize::MergeSiblingNodes(NiNode* pkNode,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Recursively merges sibling nodes. A node will not be merged if it is
    // singled out by BuildHashTable, has an "sgoKeep" extra data tag, has
    // any controllers, has an ABV, has any extra data, or whose children
    // have any controllers.
    // Two nodes will be merged if the following holds:
    // The nodes are
    //     sibling nodes,
    //     of exact type NiNode,
    // and they have
    //     a parent of exact type NiNode,
    //     the same properties,
    //     the same AppCulled flag.
    // If two nodes can be merged and they have different transforms on them,
    // the transforms for both nodes are pushed down to their children before
    // they are merged.

    // Recurse through children.
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild && NiIsKindOf(NiNode, pkChild))
        {
            MergeSiblingNodes((NiNode*) pkChild, kMap);
        }
    }

    // Do not merge if this node is not of exact type NiNode.
    if (!NiIsExactKindOf(NiNode, pkNode))
    {
        return;
    }

    // Merge sibling nodes.
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild1 = pkNode->GetAt(ui);
        if (pkChild1 && NiIsExactKindOf(NiNode, pkChild1))
        {
            NiNode* pkChildNode1 = (NiNode*) pkChild1;
            NiTimeController* pkController = pkChildNode1->GetControllers();
            bool bChildCtlrFound = false;
            unsigned int uiChild;
            for (uiChild = 0; uiChild < pkChildNode1->GetArrayCount();
                uiChild++)
            {
                NiAVObject* pkChild = pkChildNode1->GetAt(uiChild);
                if (pkChild && pkChild->GetControllers())
                {
                    bChildCtlrFound = true;
                    break;
                }
            }
            bool bTemp;
            if (bChildCtlrFound || pkController ||
                pkChildNode1->GetExtraDataSize() ||
                CheckForExtraDataTags(pkChildNode1) ||
                kMap.GetAt(pkChildNode1, bTemp))
            {
                continue;
            }

            for (unsigned int uj = ui + 1; uj < pkNode->GetArrayCount(); uj++)
            {
                NiAVObject* pkChild2 = pkNode->GetAt(uj);
                if (pkChild2 && NiIsExactKindOf(NiNode, pkChild2))
                {
                    NiNode* pkChildNode2 = (NiNode*) pkChild2;
                    pkController = pkChildNode2->GetControllers();
                    bChildCtlrFound = false;
                    for (uiChild = 0; uiChild < pkChildNode2->GetArrayCount();
                        uiChild++)
                    {
                        NiAVObject* pkChild = pkChildNode2->GetAt(uiChild);
                        if (pkChild && pkChild->GetControllers())
                        {
                            bChildCtlrFound = true;
                            break;
                        }
                    }
                    bool bTemp;
                    if (!bChildCtlrFound && !pkController &&
                        !pkChildNode2->GetExtraDataSize() &&
                        !CheckForExtraDataTags(pkChildNode2) &&
                        !kMap.GetAt(pkChildNode2, bTemp) &&
                        CompareProperties(pkChildNode1, pkChildNode2) &&
                        pkChild1->GetAppCulled() == pkChild2->GetAppCulled())

                    {
                        // Propagate transforms from pkChildNode1 to its
                        // children and reset the transforms on the node.
                        unsigned int uiChild;
                        for (uiChild = 0;
                            uiChild < pkChildNode1->GetArrayCount();
                            uiChild++)
                        {
                            NiAVObject* pkChild =
                                pkChildNode1->GetAt(uiChild);
                            if (pkChild)
                            {
                                PropagateTransforms(pkChildNode1, pkChild);
                            }
                        }
                        pkChildNode1->SetTranslate(NiPoint3::ZERO);
                        pkChildNode1->SetRotate(NiMatrix3::IDENTITY);
                        pkChildNode1->SetScale(1.0f);

                        // Propagate transforms from pkChildNode2 to its
                        // children and move those children under
                        // pkChildNode1.
                        for (uiChild = 0;
                            uiChild < pkChildNode2->GetArrayCount();
                            uiChild++)
                        {
                            NiAVObject* pkChild =
                                pkChildNode2->GetAt(uiChild);
                            if (pkChild)
                            {
                                PropagateTransforms(pkChildNode2, pkChild);
                                pkChildNode1->AttachChild(pkChild);
                                pkChildNode2->DetachChild(pkChild);
                            }
                        }
                        pkNode->DetachChild(pkChild2);
                    }
                }
            }
        }
    }
    pkNode->CompactChildArray();
}
//---------------------------------------------------------------------------
void NiOptimize::OptimizeTriShapes(NiNode* pkNode)
{
    // Recurses through the scene graph, removing degenerate triangles and
    // collapsing vertices of all NiTriShape objects, deleting them if all
    // their triangles are degenerate.

    // Recurse over children.
    for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (NiIsKindOf(NiNode, pkChild))
            {
                // Continue recursing.
                OptimizeTriShapes((NiNode*) pkChild);
            }
            else if (NiIsKindOf(NiTriShape, pkChild))
            {
                NiGeomMorpherController* pkMorpher = 
                    NiGetController(NiGeomMorpherController, pkChild);

                if (pkMorpher == NULL)
                {
                    // Optimize tri shape.
                    bool bRemove = OptimizeTriShape((NiTriShape*) pkChild);
                    if (bRemove)
                        pkNode->DetachChild(pkChild);
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::MergeSiblingTriShapes(NiNode* pkNode, bool bNoMergeOnAlpha,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Recursively merges all sibling NiTriShapes except those singled out
    // by BuildHashTable and those with an "sgoKeep" extra data tag. If
    // bNoMergeOnAlpha is true, recursive merging does not occur below nodes
    // with an NiAlphaProperty attached when alpha blending is enabled or
    // sorters are allowed.
    // Two NiTriShapes can be merged if the following holds:
    // They have
    //     a common parent of exact type NiNode,
    //     the same number of properties,
    //     the same set of property pointers,
    //     the same number of texture sets,
    //     no time controllers attached,
    //     different NiGeometryData pointers,
    //     the same AppCulled flag,
    // and they both/neither have
    //     vertices,
    //     normals,
    //     textures,
    //     colors.

    if (bNoMergeOnAlpha)
    {
        // Check if alpha blending is enabled for the node and whether or
        // not sorters are allowed. If so, then recursive optimization
        // should not be performed. This prevents a merge of two siblings
        // with disjoint geometry but the same alpha property. Such a merge
        // causes a problem when an alpha sorter is used (the bounding
        // sphere center for the alpha blended objects may cause incorrect
        // drawing order).
        NiAlphaProperty* pkAlphaProp = (NiAlphaProperty*)
            pkNode->GetProperty(NiProperty::ALPHA);
        
        if (pkAlphaProp && pkAlphaProp->GetAlphaBlending() &&
            !pkAlphaProp->GetNoSorter())
        {
            return;
        }
    }

    // Recurse over children.
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (NiIsKindOf(NiNode, pkChild))
            {
                MergeSiblingTriShapes((NiNode*) pkChild, bNoMergeOnAlpha,
                    kMap);
            }
        }
    }

    // Do not merge if this node is not of exact type NiNode.
    if (!NiIsExactKindOf(NiNode, pkNode))
    {
        return;
    }

    // Collect sibling TriShapes that can be merged.
    NiTPointerList<NiTriShape*> kMergeList;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (!pkChild || !NiIsKindOf(NiTriShape, pkChild))
        {
            continue;
        }

        NiTriShape* pkShape = (NiTriShape*) pkChild;

        if (bNoMergeOnAlpha)
        {
            NiAlphaProperty* pkAlphaProp = (NiAlphaProperty*) pkShape->
                GetProperty(NiProperty::ALPHA);
            if (pkAlphaProp && pkAlphaProp->GetAlphaBlending() &&
                !pkAlphaProp->GetNoSorter())
            {
                continue;
            }
        }

        NiTimeController* pkCtlr = pkShape->GetControllers();
        bool bTemp;
        if (pkCtlr || CheckForExtraDataTags(pkShape) ||
            kMap.GetAt(pkShape, bTemp))
        {
            continue;
        }

        // Don't Merge Skinned Objects
        if (pkShape->GetSkinInstance())
        {
            continue;
        }

        // Add to list sorted by geometry data pointer.
        NiGeometryData* pkGeomData = pkShape->GetModelData();
        NiTListIterator kPos = kMergeList.GetHeadPos();
        if (!kPos)
        {
            kMergeList.AddHead(pkShape);
        }
        else
        {
            while (kPos)
            {
                if (pkGeomData < kMergeList.Get(kPos)->GetModelData())
                {
                    kMergeList.InsertBefore(kPos, pkShape);
                    break;
                }
                kPos = kMergeList.GetNextPos(kPos);
                if (!kPos)
                {
                    kMergeList.AddTail(pkShape);
                }
            }
        }
    }

    // Clean out NiTriShapes with duplicate geometry pointers (clones).
    NiTListIterator kPos1 = kMergeList.GetHeadPos();
    while (kPos1)
    {
        NiTriShape* pkShape1 = kMergeList.Get(kPos1);
        NiTListIterator kPos2 = kMergeList.GetNextPos(kPos1);
        if (!kPos2)
        {
            break;
        }
        NiTriShape* pkShape2 = kMergeList.Get(kPos2);

        bool bDuplicateData = false;
        while (pkShape1->GetModelData() == pkShape2->GetModelData())
        {
            bDuplicateData = true;
            NiTListIterator kPos3 = kMergeList.GetNextPos(kPos2);
            kMergeList.RemovePos(kPos2);
            if (!kPos3)
            {
                break;
            }
            kPos2 = kPos3;
            pkShape2 = kMergeList.Get(kPos2);
        }
        if (bDuplicateData)
        {
            kMergeList.RemovePos(kPos1);
        }

        kPos1 = kPos2;
    }

    // Iterate through node children, merging NiTriShapes that can be.
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiTriShape* pkChild1 = NiDynamicCast(NiTriShape, pkNode->GetAt(ui));
        if (!pkChild1 || !kMergeList.FindPos(pkChild1))
        {
            continue;
        }

        NiTriShapePtrArray aspMergeShapes(pkNode->GetChildCount());
        NiUnsignedIntSet kIndices;
        kIndices.Add(ui);
        aspMergeShapes.Add(pkChild1);
        for (unsigned int uj = ui + 1; uj < pkNode->GetArrayCount(); uj++)
        {
            NiTriShape* pkChild2 = NiDynamicCast(NiTriShape,
                pkNode->GetAt(uj));
            if (!pkChild2 || !kMergeList.FindPos(pkChild2))
            {
                continue;
            }

            if (pkChild1->GetAppCulled() == pkChild2->GetAppCulled()
                && CanMergeTriShape(pkChild1, pkChild2))
            {
                aspMergeShapes.Add(pkChild2);
                pkNode->DetachChild(pkChild2);
                kIndices.Add(uj);
            }
        }

        if (aspMergeShapes.GetEffectiveSize() > 1)
        {
            // Only merge TriShapes that will not exceed half an unsigned
            // short number of vertices or triangles.  Only half
            // because we need to leave room for any degenerate
            // triangles stripifying may create.
            NiTriShapePtrArray aspMerge(
                aspMergeShapes.GetEffectiveSize());
            unsigned int uiHalfMaxUnsignedShort =
                (unsigned int) ((unsigned short) (~0)) / 2;
            unsigned int uiNumVertices = 0;
            unsigned int uiNumTriangles = 0;
            unsigned int uiIndex = 0;
            for (unsigned int uk = 0;
                uk < aspMergeShapes.GetEffectiveSize(); uk++)
            {
                NiTriShapePtr spShape = aspMergeShapes.GetAt(uk);
                uiNumVertices += spShape->GetVertexCount();
                uiNumTriangles += spShape->GetTriangleCount();

                if (uiNumVertices < uiHalfMaxUnsignedShort &&
                    uiNumTriangles < uiHalfMaxUnsignedShort)
                {
                    aspMerge.Add(spShape);
                }
                else
                {
                    if (aspMerge.GetEffectiveSize() > 0)
                    {
                        pkNode->SetAt(kIndices.GetAt(uiIndex++),
                            MergeTriShapes(aspMerge));
                    }
                    aspMerge.RemoveAll();
                    aspMerge.Compact();
                    aspMerge.UpdateSize();
                    aspMerge.Add(spShape);
                    uiNumVertices = spShape->GetVertexCount();
                    uiNumTriangles = spShape->GetTriangleCount();
                }
            }
            pkNode->SetAt(kIndices.GetAt(uiIndex),
                MergeTriShapes(aspMerge));
        }
    }

    pkNode->CompactChildArray();
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveExtraUVSets(NiAVObject* pkObject)
{
    // Removes all UV sets in geometry that are not used by any
    // NiTexturingProperty attached to the current or a higher node.

    unsigned int uiSize;

    if (NiIsKindOf(NiGeometry, pkObject) && 
        !NiIsKindOf(NiParticles, pkObject))
    {
        NiTexturingProperty* pkTexProp = FindTexturingProperty(pkObject);

        //BUGFIX 5838
        //If we can't find the texture in the hierarchy, maybe 
        //it is in the hierarchy of
        //an instance of the geometry. If that is the case we want to keep the 
        //coordinates
        //
        if(!pkTexProp)
        {
            NiGeometry* pkGeometry = NiDynamicCast(NiGeometry, pkObject);
            //Even though this is almost impossible
            NIASSERT(pkGeometry);
            
            NiGeometryData* pkGeometryData = pkGeometry->GetModelData();
            NiTPrimitiveArray<NiGeometry*> kInstances;
            NiAVObject* pkRoot = FindRoot(pkGeometry);

            FindInstances(pkGeometryData, pkRoot, &kInstances);
            
            //For each instance check if there is a texture property
            unsigned int uiInstanceCount = kInstances.GetSize();
            for(unsigned int uiInstanceIndex = 0; 
                uiInstanceIndex < uiInstanceCount; uiInstanceIndex++)
            {
                NiGeometry* pkInstance = kInstances.GetAt(uiInstanceIndex);

                if(pkInstance)
                {
                    pkTexProp = FindTexturingProperty(pkInstance);
                    if(pkTexProp)
                    {
                        //there is a texture property so just return, 
                        //the appropriate 
                        //steps will happen when we 
                        //recurse to that part of the hierarchy 
                        return;
                    }
                }
            }
        }

        if (!pkTexProp)
        {
            // Remove all UVs from the geometry object.
            NiGeometry* pkGeom = (NiGeometry*) pkObject;
            NiGeometryData* pkData = pkGeom->GetModelData();
            NIASSERT(pkData);
            unsigned short usNumVerts = pkData->GetVertexCount();

            // Retrieve all geometry data.
            NiPoint3* pkVertices = pkData->GetVertices();
            NIASSERT(pkVertices);
            NiPoint3* pkNewVertices = NiNew NiPoint3[usNumVerts];
            uiSize = usNumVerts * sizeof(NiPoint3);
            NiMemcpy(pkNewVertices, pkVertices, uiSize);

            NiPoint3* pkNormals = pkData->GetNormals();
            NiGeometryData::DataFlags eNBTMethod = 
                pkData->GetNormalBinormalTangentMethod();
            unsigned int uiNormalFactor = 
                (eNBTMethod == NiGeometryData::NBT_METHOD_NONE ? 1 : 3);
            NiPoint3* pkNewNormals = NULL;
            if (pkNormals)
            {
                pkNewNormals = NiNew NiPoint3[usNumVerts * uiNormalFactor];
                uiSize = usNumVerts * uiNormalFactor * sizeof(NiPoint3);
                NiMemcpy(pkNewNormals, pkNormals, uiSize);
            }

            NiColorA* pkColors = pkData->GetColors();
            NiColorA* pkNewColors = NULL;
            if (pkColors)
            {
                pkNewColors = NiNew NiColorA[usNumVerts];
                uiSize = usNumVerts * sizeof(NiColorA);
                NiMemcpy(pkNewColors, pkColors, uiSize);
            }

            // Replace NiGeometryData data without any texture sets.
            // Old data is deleted by pkData.
            pkData->Replace(usNumVerts, pkNewVertices, pkNewNormals,
                pkNewColors, NULL, 0, eNBTMethod);
            /*
            pkData->MarkAsChanged(NiGeometryData::VERTEX_MASK |
                NiGeometryData::NORMAL_MASK | NiGeometryData::COLOR_MASK |
                NiGeometryData::TEXTURE_MASK);*/
        }
        else
        {
            if (pkTexProp->GetShaderMapCount() > 0)
            {
                // Don't run this optimization if the texturing property
                // has any shader maps.
                return;
            }

            // Get pointer to NiGeometryData for pkObject.
            NiGeometry* pkGeom = (NiGeometry*) pkObject;
            NiGeometryData* pkData = pkGeom->GetModelData();
            NIASSERT(pkData);
            unsigned short usNumVerts = pkData->GetVertexCount();

            // Loop through all the maps, storing used texture indices.
            NiTMap<unsigned int, NiTPrimitiveSet<NiTexturingProperty::Map*>*>
                kTexIndexToMap;

            const NiTexturingProperty::NiMapArray& kArray = 
                pkTexProp->GetMaps();

            unsigned int ui = 0;
            for (ui = 0; ui < kArray.GetSize(); ui++)
            {
                NiTexturingProperty::Map* pkMap = kArray.GetAt(ui);

                if (pkMap)
                {
                    // Get texture index from map.
                    unsigned int uiTexIndex = pkMap->GetTextureIndex();

                    // If the texture index of the map refers to texture
                    // coordinates that don't exist, remove the map.
                    if (uiTexIndex > pkData->GetTextureSets())
                    {
                        pkTexProp->SetMap(ui, NULL);
                    }
                    else
                    {
                        NiTPrimitiveSet<NiTexturingProperty::Map*>*
                            pkMapSet = NULL;
                        if (!kTexIndexToMap.GetAt(uiTexIndex, pkMapSet))
                        {
                            pkMapSet = NiNew 
                                NiTPrimitiveSet<NiTexturingProperty::Map*>;
                            kTexIndexToMap.SetAt(uiTexIndex, pkMapSet);
                        }
                        pkMapSet->Add(pkMap);
                    }
                }
            }

            // puiOldToNew is for keeping track of any order changes
            // in the texture sets.  We need to know this to remap any UV 
            // controllers.
            unsigned int* puiOldToNew = 
                NiAlloc(unsigned int, pkData->GetTextureSets());
            for (ui = 0; ui < pkData->GetTextureSets(); ui++)
            {
                puiOldToNew[ui] = ui;
            }

            // Create new texture coordinate array.
            NiPoint2* pkNewTexCoords = NULL;
            NiPoint2* pkTexCoordPtr = NULL;
            if (kTexIndexToMap.GetCount() == 0)
            {
                pkObject->DetachProperty(pkTexProp);
            }
            else
            {
                pkNewTexCoords = NiNew NiPoint2[kTexIndexToMap.GetCount() *
                    usNumVerts];
                pkTexCoordPtr = &pkNewTexCoords[0];
            }
            unsigned int uiCurTexIndex = 0;
            bool bParticlesObject = false;
            NiTMapIterator kIter = kTexIndexToMap.GetFirstPos();
            while (kIter)
            {
                unsigned int uiTexIndex;
                NiTPrimitiveSet<NiTexturingProperty::Map*>* pkMapSet;
                kTexIndexToMap.GetNext(kIter, uiTexIndex, pkMapSet);

                NiPoint2* pkTextureSet = pkData->GetTextureSet(uiTexIndex);
                if (pkTextureSet)
                {
                    uiSize = usNumVerts * sizeof(NiPoint2);
                    NiMemcpy(pkTexCoordPtr, pkTextureSet, uiSize);
                    pkTexCoordPtr = &pkTexCoordPtr[usNumVerts];

                    for (ui = 0; ui < pkMapSet->GetSize(); ui++)
                    {
                        pkMapSet->GetAt(ui)->SetTextureIndex(uiCurTexIndex);
                    }
                    puiOldToNew[uiTexIndex] = uiCurTexIndex++;
                }
                else
                {
                    // Particles objects can have textures without texture
                    // coordinates.
                    if (NiIsKindOf(NiParticles, pkObject))
                    {
                        bParticlesObject = true;
                        for (ui = 0; ui < pkMapSet->GetSize(); ui++)
                        {
                            pkMapSet->GetAt(ui)->SetTextureIndex(0);
                        }
                    }
                }

                NiDelete pkMapSet;
            }
            kTexIndexToMap.RemoveAll();

            NiFree(puiOldToNew);

            // If this object is a particles object, do not set new texture
            // coordinates.
            if (bParticlesObject)
            {
                NiDelete[] pkNewTexCoords;
                pkNewTexCoords = NULL;
            }

            // Retrieve and copy all other NiGeometryData data.
            NiPoint3* pkVertices = pkData->GetVertices();
            NIASSERT(pkVertices);
            NiPoint3* pkNewVertices = NiNew NiPoint3[usNumVerts];
            uiSize = usNumVerts * sizeof(NiPoint3); 
            NiMemcpy(pkNewVertices, pkVertices, uiSize);

            NiPoint3* pkNormals = pkData->GetNormals();
            NiGeometryData::DataFlags eNBTMethod = 
                pkData->GetNormalBinormalTangentMethod();
            unsigned int uiNormalFactor = 
                (eNBTMethod == NiGeometryData::NBT_METHOD_NONE ? 1 : 3);
            NiPoint3* pkNewNormals = NULL;
            if (pkNormals)
            {
                pkNewNormals = NiNew NiPoint3[usNumVerts * uiNormalFactor];
                uiSize = usNumVerts * uiNormalFactor * sizeof(NiPoint3);
                NiMemcpy(pkNewNormals, pkNormals, uiSize);
            }

            NiColorA* pkColors = pkData->GetColors();
            NiColorA* pkNewColors = NULL;
            if (pkColors)
            {
                pkNewColors = NiNew NiColorA[usNumVerts];
                uiSize = usNumVerts * sizeof(NiColorA);
                NiMemcpy(pkNewColors, pkColors, uiSize);
            }

            // Replace NiGeometryData data with new texture set.
            // Old data is deleted by pData.
            pkData->Replace(usNumVerts, pkNewVertices, pkNewNormals,
                pkNewColors, pkNewTexCoords, uiCurTexIndex, eNBTMethod);
            /*pkData->MarkAsChanged(NiGeometryData::VERTEX_MASK |
                NiGeometryData::NORMAL_MASK | NiGeometryData::COLOR_MASK |
                NiGeometryData::TEXTURE_MASK);*/
        }
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {

        // Recurse over children.
        NiNode* pkNode = (NiNode*) pkObject;
        for (unsigned int ui=0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                RemoveExtraUVSets(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveUnnecessaryNormals(NiAVObject* pkObject)
{
    // Recursively removes normals that are not needed. Will not remove
    // normals from scenes without lights.

    if (NiIsKindOf(NiGeometry, pkObject))
    {
        NiGeometry* pkGeometry = (NiGeometry*) pkObject;

        if (CanRemoveNormals(pkGeometry))
        {
            // Remove normals.
            NiPoint3* pkNormals = pkGeometry->GetNormals();
            if (pkNormals)
            {
                unsigned short usVertices = pkGeometry->GetVertexCount();
                NiPoint3* pkVertex = pkGeometry->GetVertices();
                NiColorA* pkColor = pkGeometry->GetColors();
                NiPoint2* pkTexture = pkGeometry->GetTextures();
                unsigned short usNumTexSets = pkGeometry->GetTextureSets();
                NiGeometryData::DataFlags eNBTMethod = 
                    pkGeometry->GetNormalBinormalTangentMethod();
                pkGeometry->GetModelData()->Replace(usVertices, pkVertex,
                    NULL, pkColor, pkTexture, usNumTexSets, eNBTMethod);
            }
        }
    }
    else if (NiIsKindOf(NiNode, pkObject))
    {
        // Recurse over children.
        NiNode* pkNode = (NiNode*) pkObject;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                RemoveUnnecessaryNormals(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::CollectUnnecessaryVisControllers(NiAVObject* pkObject,
    NiVisControllerArray& apkCtlrs)
{
    // Detects the presence of any visibility controllers in the scene whose
    // keys all have the same visibility flag. Adds a pointer to each
    // offending controller to apkCtlrs.

    // Returns: true, if spurious visibility controllers are present.
    //          false, otherwise.

    NiVisController* pkBoolCtlr = NiGetController(NiVisController, pkObject);
    if (pkBoolCtlr)
    {
        unsigned int uiNumBoolKeys;
        
        NiInterpolator* pkInterp = pkBoolCtlr->GetInterpolator();
        if (NiIsKindOf(NiBoolInterpolator, pkInterp))
        {
            NiBoolData* pkBoolData = ((NiBoolInterpolator*)pkInterp)->
                GetBoolData();

            bool bSame = true;

            // If there is no data then the Iterator has been collapsed
            if (pkBoolData)
            {
                NiBoolKey::KeyType eType;
                unsigned char ucSize;
                NiBoolKey* pkBoolKeys = pkBoolData->GetAnim(uiNumBoolKeys,
                    eType, ucSize);

                NIASSERT(eType == NiAnimationKey::STEPKEY);

                // There are only Stepped Bool Keys
                NiStepBoolKey* pkStepKeys = (NiStepBoolKey*)pkBoolKeys;
                bool bValue = pkStepKeys->GetBool();

                // Make sure each value is the same as the first.
                for (unsigned int ui = 1; bSame && ui < uiNumBoolKeys; ui++)
                {
                    if (pkStepKeys[ui].GetBool() != bValue)
                        bSame = false;
                }
            }

            // Remove the controller if all are the same
            if (bSame)
            {
                apkCtlrs.Add(pkBoolCtlr);
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
                CollectUnnecessaryVisControllers(pkChild, apkCtlrs);
            }
        }
    }

    return (apkCtlrs.GetEffectiveSize() > 0);
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveUnnecessaryVisControllers(NiAVObject* pkObject)
{
    // Removes all visibility controllers whose keys all have the same
    // visibility data. If that data is not the same as the AppCulled flag of
    // pkObject, pkObject's AppCulled flag is updated to reflect the
    // visibility data.

    NiVisControllerArray apkCtlrs(100);
    CollectUnnecessaryVisControllers(pkObject, apkCtlrs);
    apkCtlrs.Compact();
    apkCtlrs.UpdateSize();

    for (unsigned int ui = 0; ui < apkCtlrs.GetSize(); ui++)
    {
        NiVisController* pkBoolCtlr = apkCtlrs.GetAt(ui);

        // Update the controller to set the value
        pkBoolCtlr->Update(0.0);

        // Remove the Controller
        NiObjectNET* pkTarget = pkBoolCtlr->GetTarget();
        pkTarget->RemoveController(pkBoolCtlr);
    }
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveNames(NiAVObject* pkObject)
{
    // Removes the name string from all objects and their material
    // properties.

    NiProperty* pkMat = pkObject->GetProperty(NiProperty::MATERIAL);

    if (pkMat)
    {
        pkMat->SetName(NULL);
    }

    pkObject->SetName(NULL);

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                RemoveNames(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::NameUnnamedObjects(NiAVObject* pkObject)
{
    // Adds unique names to all objects in the scene that have none.

    static unsigned int uiNameCounter = 0;

    if (!pkObject->GetName())
    {
        char acName[NI_MAX_PATH];
        NiSprintf(acName, NI_MAX_PATH, "SgoUnnamedObject %u", 
            uiNameCounter++);
        pkObject->SetName(acName);
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                NameUnnamedObjects(pkChild);
        }
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::CollectBadTexturingProps(NiAVObject* pkObject,
    NiTPointerMap<NiTexturingProperty*,bool>& kBadProps)
{
    // Detects the presence of NiTexturingProperties with no image data and
    // those that use a nonexistent set of texture coordinates. Will ignore
    // bad texturing properties on NiParticles objects. Stores a pointer
    // to each bad texturing property in kBadProps.

    // Returns: true, if pkObject contains a badly formed property.
    //          false, otherwise.

    NiTexturingProperty* pkProp = (NiTexturingProperty*)
        pkObject->GetProperty(NiProperty::TEXTURING);
    if (pkProp)
    {
        NiTexturingProperty::Map* pkMap;
        bool bBadProp = false;

        pkMap = pkProp->GetBaseMap();
        if (pkMap && !bBadProp)
        {
            if (!pkMap->GetTexture())
                bBadProp = true;
            NiGeometry* pkGeom = NiDynamicCast(NiGeometry, pkObject);
            if (pkGeom)
            {
                if (pkMap->GetTextureIndex() >= pkGeom->GetTextureSets())
                    bBadProp = true;
            }
        }
        pkMap = pkProp->GetDarkMap();
        if (pkMap && !bBadProp)
        {
            if (!pkMap->GetTexture())
                bBadProp = true;
            NiGeometry* pkGeom = NiDynamicCast(NiGeometry, pkObject);
            if (pkGeom)
            {
                if (pkMap->GetTextureIndex() >= pkGeom->GetTextureSets())
                    bBadProp = true;
            }
        }
        pkMap = pkProp->GetDetailMap();
        if (pkMap && !bBadProp)
        {
            if (!pkMap->GetTexture())
                bBadProp = true;
            NiGeometry* pkGeom = NiDynamicCast(NiGeometry, pkObject);
            if (pkGeom)
            {
                if (pkMap->GetTextureIndex() >= pkGeom->GetTextureSets())
                    bBadProp = true;
            }
        }
        pkMap = pkProp->GetGlossMap();
        if (pkMap && !bBadProp)
        {
            if (!pkMap->GetTexture())
                bBadProp = true;
            NiGeometry* pkGeom = NiDynamicCast(NiGeometry, pkObject);
            if (pkGeom)
            {
                if (pkMap->GetTextureIndex() >= pkGeom->GetTextureSets())
                    bBadProp = true;
            }
        }
        pkMap = pkProp->GetGlowMap();
        if (pkMap && !bBadProp)
        {
            if (!pkMap->GetTexture())
                bBadProp = true;
            NiGeometry* pkGeom = NiDynamicCast(NiGeometry, pkObject);
            if (pkGeom)
            {
                if (pkMap->GetTextureIndex() >= pkGeom->GetTextureSets())
                    bBadProp = true;
            }
        }
        for (unsigned int ui = 0; !bBadProp &&
            ui < pkProp->GetDecalArrayCount(); ui++)
        {
            pkMap = pkProp->GetDecalMap(ui);
            if (pkMap)
            {
                if (!pkMap->GetTexture())
                    bBadProp = true;
                NiGeometry* pkGeom = NiDynamicCast(NiGeometry, pkObject);
                if (pkGeom)
                {
                    if (pkMap->GetTextureIndex() >= pkGeom->GetTextureSets())
                        bBadProp = true;
                }
            }
        }

        // Don't remove the property if pkObject is an NiParticles object.
        if (bBadProp && !NiIsKindOf(NiParticles, pkObject))
            kBadProps.SetAt(pkProp, true);
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
                CollectBadTexturingProps(pkChild, kBadProps);
            }
        }
    }

    return (kBadProps.GetCount() > 0);
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveBadTexturingProps(NiAVObject* pkObject,
    NiTPointerMap<NiTexturingProperty*,bool>& kBadProps)
{
    // Removes NiTexturingProperties with no image data and those that use
    // a nonexistent set of texture coordinates. This fuction only removes
    // bad texturing properties if they are directly attached to the
    // objects that use them (and aren't just inherited from ancestors).
    // Will not remove bad texturing properties on NiParticles objects.

    NiTexturingProperty* pkProp = (NiTexturingProperty*)
        pkObject->GetProperty(NiProperty::TEXTURING);
    if (pkProp)
    {
        bool bTemp;
        if (kBadProps.GetAt(pkProp, bTemp))
        {
            pkObject->DetachProperty(pkProp);
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
                RemoveBadTexturingProps(pkChild, kBadProps);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveUnnecessaryExtraData(NiAVObject* pkObject,
        bool bRemoveKeepTags)
{
    // Removes extra data strings that are empty or that contain the strings
    // "zMode" or "billboard". If bRemoveKeepTags is true, extra data strings
    // that contain the string "sgoKeep" are also removed.

    NiTPrimitiveArray<unsigned int> apkRemoveList(4, 4);

    for (unsigned int i=0; i < pkObject->GetExtraDataSize(); i++)
    {
        NiExtraData* pkExtraData = pkObject->GetExtraDataAt(i);

        NiStringExtraData* pkStrExData =
            NiDynamicCast(NiStringExtraData, pkExtraData);
        if (pkStrExData)
        {
            const char* pcString = pkStrExData->GetValue();
            if (pcString)
            {
                if (strcmp(pcString, "") == 0 ||
                    strcmp(pcString, "zMode") == 0 ||
                    strcmp(pcString, "billboard") == 0 ||
                    (bRemoveKeepTags && (strcmp(pcString, "sgoKeep") == 0 ||
                        strcmp(pcString, "NiOptimizeKeep") == 0)))
                {
                    apkRemoveList.Add(i);
                }
            }
            else
            {
                apkRemoveList.Add(i);
            }
        }
    }

    // Remove from end of list, to preserve validity of index for remaining
    //    entries.
    for (int j = ((int)(apkRemoveList.GetSize()) - 1); j >= 0; j--)
    {
        pkObject->RemoveExtraDataAt(apkRemoveList.GetAt(j));
    }

    // Remove bones per partition integer extra data.
    pkObject->RemoveExtraData(NI_BONES_PER_PARTITION_KEY);

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
                RemoveUnnecessaryExtraData(pkChild, bRemoveKeepTags);
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveUnnecessaryLookAtControllers(NiAVObject* pkObject)
{
    // Removes NiLookAtControllers in the scene if there are no keyframe
    // controllers or path controllers on either the target or the LookAt
    // of the controller or above them in the scene graph.

    NiLookAtController* pkCtlr = NiGetController(NiLookAtController,
        pkObject);
    if (pkCtlr)
    {
        NiAVObject* pkLookAt = pkCtlr->GetLookAt();
        if (pkLookAt &&
            !CheckForTransformControllers(pkObject, true) &&
            !CheckForTransformControllers(pkLookAt, true))
        {
            pkObject->RemoveController(pkCtlr);
        }
    }

    NiTransformController* pkTransCtlr = NiGetController(NiTransformController,
        pkObject);
    if (pkTransCtlr)
    {
        NiInterpolator* pkInterp = pkTransCtlr->GetInterpolator(0);
        NiLookAtInterpolator* pkLookAtInterp = NiDynamicCast(
            NiLookAtInterpolator, pkInterp);
        if (pkLookAtInterp)
        {
            NiAVObject* pkLookAt = pkLookAtInterp->GetLookAt();
            if (pkLookAt &&
                !CheckForTransformControllers(pkLookAt, true) &&
                !pkLookAtInterp->GetTranslateInterpolator() &&
                !pkLookAtInterp->GetScaleInterpolator())
            {
                pkObject->RemoveController(pkTransCtlr);
            }
        }
    }


    // Recurse on children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                RemoveUnnecessaryLookAtControllers(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveHiddenBoneGeometry(NiAVObject* pkObject,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    // Removes any NiGeometry object that is a child of a bone and whose
    // AppCulled flag is true.

    bool bTemp;
    if (kMap.GetAt(pkObject, bTemp))
    {
        // If pkObject is of type NiGeometry and is hidden, remove it from
        // the scene graph.
        if (NiIsKindOf(NiGeometry, pkObject) && pkObject->GetAppCulled())
        {
            NiNode* pkParent = pkObject->GetParent();
            if (pkParent)
            {
                pkParent->DetachChild(pkObject);
                return;
            }
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        // Recurse over children.
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                RemoveHiddenBoneGeometry(pkChild, kMap);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceAnimationKeys(NiAVObject* pkObject, float fTolerance)
{
    // Reduces redundant animation keys for all NiTransformControllers in the
    // scene graph rooted at pkObject. fTolerance specifies a ratio of
    // how far off keys are allowed to be to still be considered redundant.

    NiTimeController* pkCtlr = pkObject->GetControllers();
    while (pkCtlr)
    {
        NiTransformController* pkKFCtlr = NiDynamicCast(NiTransformController,
            pkCtlr);
        if (pkKFCtlr)
        {
            ReduceAnimationKeys(pkKFCtlr, fTolerance);
        }

        pkCtlr = pkCtlr->GetNext();
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
                ReduceAnimationKeys(pkChild, fTolerance);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::ReduceAnimationKeys(NiTransformController* pkCtlr,
    float fTolerance)
{
    // Reduces redundant animation keys for the specified
    // NiTransformController fTolerance specifies a ratio of how far off keys
    // are allowed to be to still be considered redundant.

    NIASSERT(pkCtlr);
    
    // Obtain the target object which is needed to ensure
    // transforms (out of Maya) if after completely reducing
    // keys 
    NiAVObject* pkObject = NiDynamicCast(NiAVObject, pkCtlr->GetTarget());
    NIASSERT(pkObject);

    NiInterpolator* pkInterp = pkCtlr->GetInterpolator();
    if (NiIsKindOf(NiTransformInterpolator, pkInterp))
    {
        NiTransformInterpolator* pkTransformInterp = 
            (NiTransformInterpolator*) pkInterp;

        NiTransformData* pkData = pkTransformInterp->GetTransformData();
        if (pkData)
        {
            // Reduce rotation keys.
            bool bKeysReduced = false;
            unsigned int uiNumRotKeys;
            NiRotKey::KeyType eRotKeyType;
            unsigned char ucRotSize;
            NiRotKey* pkRotKeys = pkData->GetRotAnim(uiNumRotKeys, 
                eRotKeyType, ucRotSize);
            unsigned int uiNumNewRotKeys = 0;
            NiRotKey* pkNewRotKeys = NULL;
            if (uiNumRotKeys > 1 || eRotKeyType == NiRotKey::EULERKEY)
            {
                switch (eRotKeyType)
                {
                    case NiRotKey::LINKEY:
                        ReduceLinRotKeys(uiNumRotKeys, pkRotKeys, 
                            uiNumNewRotKeys, pkNewRotKeys, 
                            fTolerance, pkObject);
                        bKeysReduced = true;
                        break;
                    case NiRotKey::BEZKEY:
                        ReduceBezRotKeys(uiNumRotKeys, pkRotKeys, 
                            uiNumNewRotKeys, pkNewRotKeys, 
                            fTolerance, pkObject);
                        bKeysReduced = true;
                        break;
                    case NiRotKey::TCBKEY:
                        // TCB keys are not reduced, since redundant 
                        // TCB keys only appear when placed there 
                        // by the artist.
                        break;
                    case NiRotKey::EULERKEY:
                        // Note - pkObject not passed in.
                        ReduceEulerRotKeys(uiNumRotKeys, pkRotKeys, 
                            fTolerance);

                        // EulerRotKeys replace data within themselves.  
                        // pkNewRotKeys will always be NULL since it is 
                        // not set within ReduceEulerRotKeys. Setting this 
                        // flag to false is redundant, but it insures that 
                        // the key data is not replaced with NULL.
                        bKeysReduced = false;
                        break;
                    default:
                        break;
                }
            }
            if (bKeysReduced)
            {
                // Replace the animation data through the interpolator so
                // it can update it internal variables
                pkTransformInterp->ReplaceRotData(pkNewRotKeys, 
                    uiNumNewRotKeys, eRotKeyType);
            }

            // Reduce position keys.
            bKeysReduced = false;
            unsigned int uiNumPosKeys;
            NiPosKey::KeyType ePosKeyType;
            unsigned char ucPosSize;
            NiPosKey* pkPosKeys = pkData->GetPosAnim(uiNumPosKeys,
                ePosKeyType, ucPosSize);
            unsigned int uiNumNewPosKeys = 0;
            NiPosKey* pkNewPosKeys = NULL;

            if (uiNumPosKeys > 1)
            {
                switch (ePosKeyType)
                {
                    case NiPosKey::LINKEY:
                        ReduceLinPosKeys(uiNumPosKeys, pkPosKeys, 
                            uiNumNewPosKeys, pkNewPosKeys, fTolerance, 
                            pkObject);
                        bKeysReduced = true;
                        break;
                    case NiPosKey::BEZKEY:
                        ReduceBezPosKeys(uiNumPosKeys, pkPosKeys, 
                            uiNumNewPosKeys, pkNewPosKeys, fTolerance, 
                            pkObject);
                        bKeysReduced = true;
                        break;
                    case NiPosKey::TCBKEY:
                        // TCB keys are not reduced, since redundant 
                        // TCB keys only appear when placed there 
                        // by the artist.
                        break;
                    default:
                        break;
                }
            }
            if (bKeysReduced)
            {
                // Replace the animation data through the interpolator so
                // it can update it internal variables
                pkTransformInterp->ReplacePosData(pkNewPosKeys, 
                    uiNumNewPosKeys, ePosKeyType);
            }

            // Reduce scale keys.
            bKeysReduced = false;
            unsigned int uiNumScaleKeys;
            NiFloatKey::KeyType eScaleKeyType;
            unsigned char ucScaleSize;
            NiFloatKey* pkScaleKeys = pkData->GetScaleAnim(uiNumScaleKeys,
                eScaleKeyType, ucScaleSize);
            unsigned int uiNumNewScaleKeys = 0;
            NiFloatKey* pkNewScaleKeys = NULL;

            if (uiNumScaleKeys > 1)
            {
                switch (eScaleKeyType)
                {
                    // NOTE - pkObject not passed in - removal of 2 keys
                    // assumes that pkObject has correct scale outside of
                    // animations
                    case NiFloatKey::LINKEY:
                        ReduceLinFloatKeys(uiNumScaleKeys, pkScaleKeys,
                            uiNumNewScaleKeys, pkNewScaleKeys, fTolerance);
                        bKeysReduced = true;
                        break;
                    case NiFloatKey::BEZKEY:
                        ReduceBezFloatKeys(uiNumScaleKeys, pkScaleKeys,
                            uiNumNewScaleKeys, pkNewScaleKeys, fTolerance);
                        bKeysReduced = true;
                        break;
                    case NiFloatKey::TCBKEY:
                        // TCB keys are not reduced, since redundant TCB 
                        // keys only appear when placed there by the artist.
                        break;
                    default:
                        break;
                }
            }
            if (bKeysReduced)
            {
                // Replace the animation data through the interpolator so
                // it can update it internal variables
                pkTransformInterp->ReplaceScaleData(pkNewScaleKeys, 
                    uiNumNewScaleKeys, eScaleKeyType);
            }
        }

       pkTransformInterp->Collapse();
       pkCtlr->ResetTimeExtrema();
    }
}
//---------------------------------------------------------------------------
void NiOptimize::SortByTexturingProp(NiNode* pkNode,
    NiTexturingProperty*& pkLoProp, NiTexturingProperty*& pkHiProp)
{
    // Recursively sorts all the children of each node by the address of the
    // NiTexturingProperty present in their property states. Children with no
    // NiTexturingProperty in their property state attached are sorted first.

    if (!NiIsExactKindOf(NiNode, pkNode) || pkNode->GetChildCount() == 0)
    {
        pkLoProp = NULL;
        pkHiProp = NULL;
        return;
    }

    NiTPointerList<TexSortObj*> kSortList;

    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            TexSortObj* pkNewSortObj = NiNew TexSortObj;
            pkNewSortObj->m_pkObject = pkChild;
            pkNewSortObj->m_pkLoProp = pkNewSortObj->m_pkHiProp = NULL;

            if (NiIsKindOf(NiNode, pkChild))
            {
                SortByTexturingProp((NiNode*) pkChild,
                    pkNewSortObj->m_pkLoProp, pkNewSortObj->m_pkHiProp);
            }
            else if (NiIsKindOf(NiGeometry, pkChild))
            {
                NiPropertyState* pkPropState = ((NiGeometry*) pkChild)
                    ->GetPropertyState();
                NiTexturingProperty* pkTexProp = NULL;
                if (pkPropState)
                {
                    pkTexProp = pkPropState->GetTexturing();
                }

                if (pkTexProp)
                {
                    // Check for an active map.
                    bool bMapFound = false;
                    for (unsigned int uiIndex = 0;
                        uiIndex < pkTexProp->GetMaps().GetSize(); uiIndex++)
                    {
                        if (pkTexProp->GetMaps().GetAt(uiIndex))
                        {
                            bMapFound = true;
                            break;
                        }
                    }

                    if (bMapFound)
                    {
                        pkNewSortObj->m_pkLoProp = pkNewSortObj->m_pkHiProp =
                            pkTexProp;
                    }
                }
            }

            NiTListIterator pos = kSortList.GetHeadPos();
            if (!pos)
            {
                kSortList.AddHead(pkNewSortObj);
            }
            else
            {
                bool bInsertAfter = false;

                while (pos)
                {
                    TexSortObj* pkCurSortObj = kSortList.Get(pos);
                    if (pkNewSortObj->m_pkLoProp < pkCurSortObj->m_pkLoProp)
                    {
                        NiTListIterator prevpos = kSortList.GetPrevPos(pos);
                        TexSortObj* pkPrevSortObj = NULL;
                        if (prevpos)
                        {
                            pkPrevSortObj = kSortList.Get(prevpos);
                        }

                        if (!pkPrevSortObj || pkPrevSortObj->m_pkHiProp !=
                            pkCurSortObj->m_pkHiProp)
                        {
                            break;
                        }
                    }
                    else if (pkCurSortObj->m_pkHiProp ==
                        pkNewSortObj->m_pkLoProp)
                    {
                        bInsertAfter = true;
                        break;
                    }
                    pos = kSortList.GetNextPos(pos);
                }

                if (!pos)
                {
                    kSortList.AddTail(pkNewSortObj);
                }
                else
                {
                    if (bInsertAfter)
                    {
                        kSortList.InsertAfter(pos, pkNewSortObj);
                    }
                    else
                    {
                        kSortList.InsertBefore(pos, pkNewSortObj);
                    }
                }
            }
        }
    }

    pkLoProp = kSortList.GetHead()->m_pkLoProp;
    pkHiProp = kSortList.GetTail()->m_pkHiProp;

    NiTObjectArray<NiAVObjectPtr> aspNewChildren(pkNode->GetChildCount());

    NiTListIterator pos = kSortList.GetHeadPos();
    while (pos)
    {
        TexSortObj* pkSortObj = kSortList.GetNext(pos);
        aspNewChildren.Add(pkSortObj->m_pkObject);
        NiDelete pkSortObj;
    }

    pkNode->RemoveAllChildren();
    for (ui = 0; ui < aspNewChildren.GetSize(); ui++)
    {
        pkNode->AttachChild(aspNewChildren.GetAt(ui));
    }
}
//---------------------------------------------------------------------------
bool NiOptimize::CollectImproperlySizedTextures(NiAVObject* pkObject,
    NiTPointerMap<NiTexture*,bool>& kBadTex)
{
    // Detects if there are any images whose dimensions are not powers of two.
    // A pointer to each improperly sized texture is stored in kBadTex.

    // Returns: true, if any improperly sized textures are found.
    //          false, otherwise.

    NiTexturingProperty* pkProp = (NiTexturingProperty*)
        pkObject->GetProperty(NiProperty::TEXTURING);
    if (pkProp)
    {
        NiTexturingProperty::Map* pkMap;

        pkMap = pkProp->GetBaseMap();
        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (pkTexture && !CheckTextureSize(pkTexture))
                kBadTex.SetAt(pkTexture, true);
        }
        pkMap = pkProp->GetDarkMap();
        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (pkTexture && !CheckTextureSize(pkTexture))
                kBadTex.SetAt(pkTexture, true);
        }
        pkMap = pkProp->GetDetailMap();
        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (pkTexture && !CheckTextureSize(pkTexture))
                kBadTex.SetAt(pkTexture, true);
        }
        pkMap = pkProp->GetGlossMap();
        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (pkTexture && !CheckTextureSize(pkTexture))
                kBadTex.SetAt(pkTexture, true);
        }
        pkMap = pkProp->GetGlowMap();
        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (pkTexture && !CheckTextureSize(pkTexture))
                kBadTex.SetAt(pkTexture, true);
        }
        for (unsigned int ui = 0; ui < pkProp->GetDecalArrayCount(); ui++)
        {
            pkMap = pkProp->GetDecalMap(ui);
            if (pkMap)
            {
                NiTexture* pkTexture = pkMap->GetTexture();
                if (pkTexture && !CheckTextureSize(pkTexture))
                    kBadTex.SetAt(pkTexture, true);
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
                CollectImproperlySizedTextures(pkChild, kBadTex);
        }
    }

    return (kBadTex.GetCount() > 0);
}
//---------------------------------------------------------------------------
void NiOptimize::OptimizeSkinData(NiAVObject* pkObject, bool bHardwareSkin, 
    bool bSoftwareSkin, unsigned int uiBonesPerPartition, bool bForcePalette)
{
    NiTimeController* pkCtlr = pkObject->GetControllers();
    while (pkCtlr)
    {
        NiBoneLODController* pkBoneLOD = NiDynamicCast(NiBoneLODController,
            pkCtlr);
        if (pkBoneLOD)
        {
            NiTriBasedGeomSet kGeoms;
            NiTPrimitiveSet<NiSkinInstance*> kInstances;
            pkBoneLOD->GetSkinData(kGeoms, kInstances);
            NIASSERT(kGeoms.GetSize() == kInstances.GetSize());

            for (unsigned int ui = 0; ui < kGeoms.GetSize(); ui++)
            {
                OptimizeSkinInstance(kGeoms.GetAt(ui), kInstances.GetAt(ui),
                    bHardwareSkin, bSoftwareSkin, uiBonesPerPartition,
                    bForcePalette);
            }
        }

        pkCtlr = pkCtlr->GetNext();
    }

    NiTriBasedGeom* pkGeom = NiDynamicCast(NiTriBasedGeom, pkObject);
    if (pkGeom)
    {
        NiSkinInstance* pkSkinInstance = pkGeom->GetSkinInstance();
        if (pkSkinInstance)
        {
            OptimizeSkinInstance(pkGeom, pkSkinInstance, bHardwareSkin,
                bSoftwareSkin, uiBonesPerPartition, bForcePalette);
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
                OptimizeSkinData(pkChild, bHardwareSkin, bSoftwareSkin, 
                    uiBonesPerPartition, bForcePalette);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptimize::CleanUpAnimationKeys(NiAVObject* pkObject)
{
    // cleanup object controllers
    NiTimeController* pkCtlr = pkObject->GetControllers();
    while (pkCtlr)
    {
        NiInterpController* pkInterpCtlr = NiDynamicCast(NiInterpController,
            pkCtlr);
        if (pkInterpCtlr && 
            pkInterpCtlr->GetBeginKeyTime() != pkInterpCtlr->GetEndKeyTime())
        {
            pkInterpCtlr->GuaranteeTimeRange(pkInterpCtlr->GetBeginKeyTime(),
                pkInterpCtlr->GetEndKeyTime());
        }
        pkCtlr = pkCtlr->GetNext();
    }

    // cleanup property controllers
    NiTListIterator kPos = pkObject->GetPropertyList().GetHeadPos();
    while (kPos)
    {
        NiProperty* pkProperty = pkObject->GetPropertyList().GetNext(kPos);
        if (pkProperty)
        {
            pkCtlr = pkObject->GetControllers();
            while (pkCtlr)
            {
                NiInterpController* pkInterpCtlr = NiDynamicCast(
                    NiInterpController, pkCtlr);
                if (pkInterpCtlr && pkInterpCtlr->GetBeginKeyTime() !=
                    pkInterpCtlr->GetEndKeyTime())
                {
                    pkInterpCtlr->GuaranteeTimeRange(
                        pkInterpCtlr->GetBeginKeyTime(),
                        pkInterpCtlr->GetEndKeyTime());
                }
                pkCtlr = pkCtlr->GetNext();
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
                CleanUpAnimationKeys(pkChild);
            }
        }
    }
}
//---------------------------------------------------------------------------
// NiTriShape optimization
//---------------------------------------------------------------------------
bool NiOptimize::RemoveDegenerateTriangles(NiTriShape* pkShape)
{
    NIASSERT(pkShape);

    NiTriShapeData* pkData = (NiTriShapeData*)pkShape->GetModelData();
    unsigned short usNewTriangles = 0;
    unsigned short usTriangles = pkData->GetTriangleCount();
    unsigned short* pusTriList = pkData->GetTriList();
    unsigned short* pusNew = pusTriList;
    unsigned short* pusOld = pusTriList;
    NiPoint3* pkVertex = pkData->GetVertices();

    unsigned int uiSize;

    for (unsigned short t = 0; t < usTriangles; t++, pusOld += 3)
    {

        // if the tri doesn't have repeated verts, keep it
        unsigned int uiTri = 3 * t;
        unsigned int uiI0 = pusTriList[uiTri];
        unsigned int uiI1 = pusTriList[uiTri + 1];
        unsigned int uiI2 = pusTriList[uiTri + 2];

        const NiPoint3& kP0 = pkVertex[uiI0];
        const NiPoint3& kP1 = pkVertex[uiI1];
        const NiPoint3& kP2 = pkVertex[uiI2];

        if (NiOptimize::NonDegenerateTriangle(uiI0, uiI1, uiI2,
            kP0, kP1, kP2))
        {
            if (pusNew != pusOld)
            {
                pusNew[0] = pusOld[0];
                pusNew[1] = pusOld[1];
                pusNew[2] = pusOld[2];
            }

            usNewTriangles++;
            pusNew += 3;
        }
    }

    // reallocate m_pusTriList if necessary
    if (usNewTriangles != usTriangles)
    {
        unsigned int uiNewListLen = 3 * usNewTriangles;
        unsigned short* pusNewTriList = NiAlloc(unsigned short, 
            uiNewListLen);
        NIASSERT(pusNewTriList);
        uiSize = uiNewListLen * sizeof(pusNewTriList[0]);
        NiMemcpy(pusNewTriList, pusTriList, uiSize);

        pkData->Replace(usNewTriangles, pusNewTriList);
    }

    return usNewTriangles == 0;
}
//---------------------------------------------------------------------------
void NiOptimize::CollapseVertices(NiTriShape* pkShape,
    unsigned short** ppusNewToOld)
{
    NIASSERT(pkShape);

    NiTriShapeData* pkData = (NiTriShapeData*)pkShape->GetModelData();
    unsigned short* pIdx0;
    unsigned short* pIdx1;
    pkData->IndexSort(pIdx0, pIdx1);

    unsigned short usVertices = pkData->GetVertexCount();
    unsigned int i;

    // When all components are compared, merge-able vertices must be adjacent.

    for (i = 1; i < usVertices; i++)
    {
        if (pkData->VertexCompare(pIdx0[i], pIdx0[i - 1]) == 0)
        {
            pIdx0[i] = pIdx0[i - 1];
        }
    }

    unsigned int uiTriListLength = pkData->GetTriListLength();
    unsigned short* pTriList = pkData->GetTriList();
    for (i = 0; i < uiTriListLength; i++)
        pTriList[i] = pIdx0[pIdx1[pTriList[i]]];

    NiFree(pIdx0);
    NiFree(pIdx1);

    RemoveUnusedVertices(pkShape, ppusNewToOld);
}
//---------------------------------------------------------------------------
void NiOptimize::RemoveAppCulledObjects(NiAVObject* pkObject,
    const NiTPointerMap<NiAVObject*, bool>& kMap)
{
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                RemoveAppCulledObjects(pkChild, kMap);
            }
        }
    }

    bool bWillBeAppCulled = false;
    for (NiAVObject* pkCurrent = pkObject; pkCurrent != NULL; 
        pkCurrent = pkCurrent->GetParent())
    {
        if (pkCurrent->GetAppCulled())
            bWillBeAppCulled = true;
    }

    NiNode* pkParent = pkObject->GetParent();
    if (bWillBeAppCulled && pkParent)
    {
        bool bTemp;
        if (!kMap.GetAt(pkObject, bTemp) &&
            !NiIsKindOf(NiSwitchNode, pkParent) &&
            !CheckForExtraDataTags(pkObject) &&
            (!pkNode || pkNode->GetChildCount() == 0))
        {
            // Remove this object.
            pkParent->DetachChild(pkObject);
        }
    }
}
//---------------------------------------------------------------------------
