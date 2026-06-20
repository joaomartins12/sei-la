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

// Precompiled Header
#include "NiAnimationPCH.h"

#include "NiBoneLODController.h"
#include <NiCloningProcess.h>
#include <NiTriBasedGeom.h>
#include <NiStringExtraData.h>
#include <NiSystem.h>

NiImplementRTTI(NiBoneLODController, NiTimeController);

int NiBoneLODController::ms_iGlobalLOD = -1;
//---------------------------------------------------------------------------
void NiBoneLODController::CleanUp()
{
    unsigned int ui;

    for (ui = 0; ui < m_kBoneArray.GetSize(); ui++)
    {
        NiTPrimitiveSet<NiNode*>* pkSet = 
            m_kBoneArray.GetAt(ui);
        NiDelete pkSet;
    }
    m_kBoneArray.RemoveAll();

    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        NiTPrimitiveSet<SkinInfo*>* pkSet = 
            m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                SkinInfo* pkInfo = pkSet->GetAt(uj);
                NiDelete pkInfo;
            }
        }
        NiDelete pkSet;
    }
    m_kSkinArray.RemoveAll();

    m_kSkinSet.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiBoneLODController::SetBoneLOD(int iLOD)
{
    if (m_uiNumLODs > 0 && m_iLOD != iLOD &&
        (iLOD == -1 || (iLOD > -1 && (unsigned int) iLOD < m_uiNumLODs)))
    {
        // Set bone update flags and activate the appropriate skins.

        unsigned int ui, uj;

        // Set bone update flags.
        bool bUpdate = false;
        for (ui = 0; ui < m_kBoneArray.GetSize(); ui++)
        {
            NiTPrimitiveSet<NiNode*>* pkSet = 
                m_kBoneArray.GetAt(ui);
            if (pkSet)
            {
                if ((int) ui == iLOD)
                {
                    bUpdate = true;
                }
                for (uj = 0; uj < pkSet->GetSize(); uj++)
                {
                    pkSet->GetAt(uj)->SetSelectiveUpdate(bUpdate);
                }
            }
        }

        // Disable all skins.
        for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
        {
            NiTriBasedGeom* pkSkinGeom = m_kSkinSet.GetAt(ui);
            pkSkinGeom->SetAppCulled(true);
            pkSkinGeom->SetSelectiveUpdate(false);
        }

        // Activate skins and set appropriate NiSkinInstance objects.
        if (iLOD != -1)
        {
            NiTPrimitiveSet<SkinInfo*>* pkSet = 
                m_kSkinArray.GetAt(iLOD);
            if (pkSet)
            {
                for (ui = 0; ui < pkSet->GetSize(); ui++)
                {
                    NiTriBasedGeom* pkSkinGeom = pkSet->GetAt(ui)
                        ->m_pkSkinGeom;
                    pkSkinGeom->SetAppCulled(false);
                    pkSkinGeom->SetSelectiveUpdate(true);
                    pkSkinGeom->SetSkinInstance(pkSet->GetAt(ui)
                        ->m_spSkinInst);
                }
            }
        }
        
        m_iLOD = iLOD;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiBoneLODController::RemoveSkinFromAllLODs(
    NiTriBasedGeom* pkSkinToRemove)
{
    unsigned int ui;

    // Search m_kSkinArray.
    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet = m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            bool bRemovedItem;
            do
            {
                bRemovedItem = false;
                for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
                {
                    SkinInfo* pkInfo = pkSet->GetAt(uj);
                    if (pkInfo->m_pkSkinGeom == pkSkinToRemove)
                    {
                        pkSet->RemoveAt(uj);
                        NiDelete pkInfo;
                        bRemovedItem = true;
                        break;
                    }
                }
            } while (bRemovedItem);

            if (pkSet->GetSize() == 0)
            {
                m_kSkinArray.RemoveAt(ui);
                NiDelete pkSet;
            }
        }
    }

    // Search m_kSkinSet.
    bool bRemovedItem;
    do
    {
        bRemovedItem = false;
        for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
        {
            NiTriBasedGeom* pkSkin = m_kSkinSet.GetAt(ui);
            if (pkSkin == pkSkinToRemove)
            {
                m_kSkinSet.RemoveAt(ui);
                bRemovedItem = true;
                break;
            }
        }
    } while (bRemovedItem);
}
//---------------------------------------------------------------------------
bool NiBoneLODController::ProcessScene(NiNode* pkSceneRoot)
{
    // Returns: true if all NiBoneLODControllers were successfully created
    //              and attached in the scene graph.
    //          false otherwise.

    // Find bone root nodes.
    NiNodeSet kBoneRootNodes;
    FindBoneRootNodes(pkSceneRoot, kBoneRootNodes);
    if (kBoneRootNodes.GetSize() == 0)
    {
        return false;
    }

    for (unsigned int ui = 0; ui < kBoneRootNodes.GetSize(); ui++)
    {
        NiNode* pkBoneRoot = kBoneRootNodes.GetAt(ui);

        // Find skin geometry.
        NiTPointerMap<NiTriBasedGeom*, NiUnsignedIntSet*> kSkinToLOD;

        FindSkinGeometry(pkSceneRoot, kSkinToLOD, pkBoneRoot);
        if (kSkinToLOD.IsEmpty())
        {
            NiMessageBox("No skin geometry has been defined for use with "
                "Bone LOD.\n\nFix this problem in the art package and export "
                "again. No Bone\nLODs will be created at this time.",
                "Bone LOD Creation Error");
            return false;
        }
        
        // Check to make sure the bone lists contain only bones in the
        // bone LOD hierarchy
        NiTMapIterator kIter = kSkinToLOD.GetFirstPos();
        while (kIter != NULL)
        {
            NiTriBasedGeom* pkGeom = NULL;
            NiUnsignedIntSet* kSet = 0;
            kSkinToLOD.GetNext(kIter, pkGeom, kSet);

            if (pkGeom)
            {
                NiSkinInstance* pkSkinInstance = pkGeom->GetSkinInstance();
                if (pkSkinInstance)
                {
                    NiSkinData* pkSkinData = pkSkinInstance->GetSkinData();
                    NiAVObject*const* pkBones = pkSkinInstance->GetBones();
                    if (!pkSkinData || !pkBones || !pkBoneRoot)
                        continue;

                    for (unsigned int uk = 0; uk < pkSkinData->GetBoneCount();
                        uk++)
                    {
                        if (pkBones[uk] == pkBoneRoot)
                            continue;
                        NiAVObject* pkParent = pkBones[uk]->GetParent();

                        while(pkParent != NULL && pkParent != pkBoneRoot)
                            pkParent = pkParent->GetParent();

                        if (pkParent == pkBoneRoot)
                            continue;

                        bool bFoundNonZeroWeight = false;
                        NiSkinData::BoneData* pkBoneData = 
                            pkSkinData->GetBoneData();

                        for (unsigned int uiVert = 0; 
                            uiVert < pkBoneData[uk].m_usVerts; uiVert++)
                        {
                            NiSkinData::BoneVertData kVert = 
                                pkBoneData[uk].m_pkBoneVertData[uiVert];
                            if (kVert.m_fWeight > 0.0000001f)
                            {
                                bFoundNonZeroWeight = true;
                                break;
                            }

                        }

                        if (bFoundNonZeroWeight)
                        {
                            char acString[1024];
                            NiSprintf(acString, 1024, "Bone \"%s\" affecting"
                                " skin \"%s\" is outside of the root bone"
                                " hierarchy.\n\nFix this problem in the art"
                                " package and export again. No Bone LODs"
                                " will be \ncreated at this time.",
                                (const char*) (pkBones[uk]->GetName()), 
                                (const char*) (pkGeom->GetName()));
                            NiMessageBox(acString, "Bone LOD Creation Error");
                            return false;
                        }
                    }
                }
            }
        }


        // Create NiBoneLODController.
        NiBoneLODController* pkCtlr = NiNew NiBoneLODController;
        pkCtlr->SetTarget(pkBoneRoot);

        // Initialize controller data.
        bool bSuccess = pkCtlr->InitializeData(pkBoneRoot, kSkinToLOD);
        if (bSuccess)
        {
            // Activate the most detailed LOD.
            pkCtlr->SetBoneLOD(0);
        }

        // Free memory from kSkinToLOD
        kIter = kSkinToLOD.GetFirstPos();
        while (kIter)
        {
            NiTriBasedGeom* pkTemp;
            NiUnsignedIntSet* pkSet;
            kSkinToLOD.GetNext(kIter, pkTemp, pkSet);
            NiDelete pkSet;
        }

        // If initialization was not successful, remove controller and return
        // false.
        if (!bSuccess)
        {
            pkBoneRoot->RemoveController(pkCtlr);
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBoneLODController::FindBoneRootNodes(NiNode* pkNode,
    NiNodeSet& kBoneRootNodes)
{
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetExtraDataSize(); ui++)
    {
        NiExtraData* pkExtra = pkNode->GetExtraDataAt(ui);
        NiStringExtraData* pkStringExtra = NiDynamicCast(NiStringExtraData,
            pkExtra);
        if (pkStringExtra)
        {
            const char* pcString = pkStringExtra->GetValue();
            if (pcString && strstr(pcString, "NiBoneLOD#BoneRoot#"))
            {
                if (strcmp(pcString, "NiBoneLOD#BoneRoot#") == 0)
                {
                    if (!pkNode->RemoveExtraDataAt(ui))
                    {
                        NiOutputDebugString("Error:  Unsuccessful attempt to "
                                "remove extra data.\n");
                    }
                }
                kBoneRootNodes.Add(pkNode);

                // Do not recurse on children of a bone root node.
                return;
            }
        }
    }

    // Recurse over children.
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild && NiIsKindOf(NiNode, pkChild))
        {
            FindBoneRootNodes((NiNode*) pkChild, kBoneRootNodes);
        }
    }
}
//---------------------------------------------------------------------------
void NiBoneLODController::FindSkinGeometry(NiAVObject* pkObject,
    NiTPointerMap<NiTriBasedGeom*, NiUnsignedIntSet*>& kSkinToLOD,
    NiNode* pkBoneRoot)
{
    unsigned int ui;
    for (ui = 0; ui < pkObject->GetExtraDataSize(); ui++)
    {
        NiExtraData* pkExtra = pkObject->GetExtraDataAt(ui);

        NiStringExtraData* pkStringExtra = NiDynamicCast(NiStringExtraData,
            pkExtra);
        if (pkStringExtra)
        {
            const char* pcString = pkStringExtra->GetValue();
            if (pcString && strstr(pcString, "NiBoneLOD#Skin#"))
            {
                NiTriBasedGeomSet kSkinGeomObjs;
                NiTriBasedGeom* pkGeom = NiDynamicCast(NiTriBasedGeom,
                    pkObject);
                if (pkGeom)
                {
                    kSkinGeomObjs.Add(pkGeom);
                }
                else if (NiIsKindOf(NiNode, pkObject))
                {
                    // Get all Geometry objects underneath this node.
                    GetGeomChildren((NiNode*) pkObject, kSkinGeomObjs);
                }
                if (kSkinGeomObjs.GetSize() == 0)
                {
                    break;
                }

                // Get the skin LODs from string extra data.
                unsigned int uiLoLOD = 0, uiHiLOD = 0;
                unsigned int uiLen = strlen(pcString) + 1;
                char* pcPtr = NiAlloc(char,uiLen);
                NiStrcpy(pcPtr, uiLen, pcString);
                unsigned int uiStrSize = 0;
                char* pcContext;
                char* pcLOD = NiStrtok(pcPtr, "#", &pcContext);
                if (!pcLOD || (pcLOD && strcmp(pcLOD, "NiBoneLOD") != 0))
                {
                    break;
                }
                uiStrSize += strlen(pcLOD) + 1;

                pcLOD = NiStrtok(NULL, "#", &pcContext);
                if (!pcLOD || (pcLOD && strcmp(pcLOD, "Skin") != 0))
                {
                    break;
                }
                uiStrSize += strlen(pcLOD) + 1;

                pcLOD = NiStrtok(NULL, "#", &pcContext);
                if (!pcLOD)
                {
                    break;
                }
                uiStrSize += strlen(pcLOD) + 1;
                uiLoLOD = (unsigned int) atoi(pcLOD);

                pcLOD = NiStrtok(NULL, "#", &pcContext);
                if (!pcLOD)
                {
                    break;
                }
                uiStrSize += strlen(pcLOD) + 1;
                uiHiLOD = (unsigned int) atoi(pcLOD);

                // Delete the extra data if it only contained this string.
                if (strlen(pcString) == uiStrSize)
                {
                    if (!pkObject->RemoveExtraDataAt(ui))
                    {
                        NiOutputDebugString("Error:  Unsuccessful attempt to "
                                "remove extra data.\n");
                    }
                }
                NiFree(pcPtr);

                // Loop through the geometry objects found, adding each to
                // the kSkinToLOD map.
                for (unsigned int uj = 0; uj < kSkinGeomObjs.GetSize(); uj++)
                {
                    NiTriBasedGeom* pkSkinGeom = kSkinGeomObjs.GetAt(uj);

                    // Verify that the skin geometry is controlled by the bone
                    // tree rooted at pkBoneRoot.
                    NiSkinInstance* pkSkinInst = pkSkinGeom
                        ->GetSkinInstance();
                    if (!pkSkinInst)
                    {
                        continue;
                    }
                    NiAVObject* pkBone = pkSkinInst->GetBones()[0];
                    if (pkBone != pkBoneRoot)
                    {
                        bool bMatch = false;
                        NiNode* pkParent = pkBone->GetParent();
                        while (pkParent)
                        {
                            if (pkParent == pkBoneRoot)
                            {
                                bMatch = true;
                                break;
                            }
                            pkParent = pkParent->GetParent();
                        }
                        if (!bMatch)
                        {
                            continue;
                        }
                    }

                    // Add NiTriBasedGeom object to map with its LODs.
                    NiUnsignedIntSet* pkSet = NiNew NiUnsignedIntSet;
                    for (unsigned int uiLOD = uiLoLOD; uiLOD <= uiHiLOD;
                        uiLOD++)
                    {
                        pkSet->Add(uiLOD);
                    }
                    kSkinToLOD.SetAt(pkSkinGeom, pkSet);
                }

                break;
            }
        }
    }

    // Recurse over children.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                FindSkinGeometry(pkChild, kSkinToLOD, pkBoneRoot);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiBoneLODController::GetGeomChildren(NiNode* pkNode,
    NiTriBasedGeomSet& kGeomChildren)
{
    for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (NiIsKindOf(NiTriBasedGeom, pkChild))
            {
                kGeomChildren.Add((NiTriBasedGeom*) pkChild);
            }
            else if (NiIsKindOf(NiNode, pkChild))
            {
                GetGeomChildren((NiNode*) pkChild, kGeomChildren);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiBoneLODController::InitializeData(NiNode* pkBoneRoot,
    const NiTPointerMap<NiTriBasedGeom*, NiUnsignedIntSet*>& kSkinToLOD)
{
    // Returns: true if the data was initialized properly and there are no
    //              LODs missing.
    //          false if the skins and bones do not have the same particular
    //              LODs specified.

    CleanUp();

    // Collect bones marked with an LOD in kBoneToLOD.
    NiTPointerMap<NiNode*, unsigned int> kBoneToLOD;
    NiTPointerMap<unsigned int, bool> kUsedLODs;

    CollectMarkedBones(pkBoneRoot, kBoneToLOD, kUsedLODs);

    // Find number of LODs.
    unsigned int uiMaxLOD = 0;
    NiTMapIterator kIter = kUsedLODs.GetFirstPos();
    while (kIter)
    {
        unsigned int uiLOD;
        bool bTemp;
        kUsedLODs.GetNext(kIter, uiLOD, bTemp);

        if (uiLOD > uiMaxLOD)
        {
            uiMaxLOD = uiLOD;
        }
    }
    m_uiNumLODs = uiMaxLOD + 1;

    // Initialize and populate bone array.
    m_kBoneArray.SetSize(m_uiNumLODs);
    unsigned int ui;
    for (ui = 0; ui < m_uiNumLODs; ui++)
    {
        m_kBoneArray.SetAt(ui, NULL);
    }
    kIter = kBoneToLOD.GetFirstPos();
    while (kIter)
    {
        NiNode* pkBone;
        unsigned int uiLOD;
        kBoneToLOD.GetNext(kIter, pkBone, uiLOD);

        NiNodeSet* pkSet = m_kBoneArray.GetAt(uiLOD);
        if (!pkSet)
        {
            pkSet = NiNew NiNodeSet;
            m_kBoneArray.SetAt(uiLOD, pkSet);
        }
        pkSet->Add(pkBone);
    }

    // Build bone map.
    unsigned int uiCurrentLOD = 0;
    BuildBoneMap(pkBoneRoot, kBoneToLOD, uiCurrentLOD);

    // Fill skin array and create modified NiSkinInstance objects for each
    // LOD.
    m_kSkinArray.SetSize(m_uiNumLODs);
    for (ui = 0; ui < m_uiNumLODs; ui++)
    {
        m_kSkinArray.SetAt(ui, NULL);
    }
    kUsedLODs.RemoveAll();
    if (!FillSkinArray(kSkinToLOD, kBoneToLOD, kUsedLODs))
    {
        return false;
    }

    // Fill skin set.
    kIter = kSkinToLOD.GetFirstPos();
    while (kIter)
    {
        NiTriBasedGeom* pkGeom;
        NiUnsignedIntSet* pkTemp;
        kSkinToLOD.GetNext(kIter, pkGeom, pkTemp);

        m_kSkinSet.Add(pkGeom);
    }

    SetBoneLOD(-1);

    return true;
}
//---------------------------------------------------------------------------
void NiBoneLODController::CollectMarkedBones(NiNode* pkBone,
    NiTPointerMap<NiNode*, unsigned int>& kBoneToLOD,
    NiTPointerMap<unsigned int, bool>& kUsedLODs)
{
    unsigned int ui;
    for (ui = 0; ui < pkBone->GetExtraDataSize(); ui++)
    {
        NiExtraData* pkExtra = pkBone->GetExtraDataAt(ui);

        NiStringExtraData* pkStringExtra = NiDynamicCast(NiStringExtraData,
            pkExtra);
        if (pkStringExtra)
        {
            const char* pcString = pkStringExtra->GetValue();
            if (pcString && strstr(pcString, "NiBoneLOD#Bone#"))
            {
                unsigned int uiLen = strlen(pcString) + 1;
                char* pcPtr = NiAlloc(char,uiLen);
                NiStrcpy(pcPtr, uiLen, pcString);

                // Get bone LOD from string extra data.
                char* pcContext;
                char* pcLOD = NiStrtok(pcPtr, "#", &pcContext);
                if (pcLOD && strcmp(pcLOD, "NiBoneLOD") == 0)
                {
                    pcLOD = NiStrtok(NULL, "#", &pcContext);
                    if (pcLOD && strcmp(pcLOD, "Bone") == 0)
                    {
                        pcLOD = NiStrtok(NULL, "#", &pcContext);
                        if (pcLOD)
                        {
                            unsigned int uiLOD = (unsigned int) atoi(pcLOD);

                            kBoneToLOD.SetAt(pkBone, uiLOD);
                            kUsedLODs.SetAt(uiLOD, true);
                        }
                    }
                }
                NiFree(pcPtr);
                break;
            }
        }
    }

    // Recurse over children.
    for (ui = 0; ui < pkBone->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkBone->GetAt(ui);
        if (pkChild && NiIsKindOf(NiNode, pkChild))
        {
            CollectMarkedBones((NiNode*) pkChild, kBoneToLOD, kUsedLODs);
        }
    }
}
//---------------------------------------------------------------------------
void NiBoneLODController::BuildBoneMap(NiNode* pkBone,
    NiTPointerMap<NiNode*, unsigned int>& kBoneToLOD,
    unsigned int& uiCurrentLOD)
{
    // Recurse over children.
    unsigned int uiOrigLOD = uiCurrentLOD;
    for (unsigned int ui = 0; ui < pkBone->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkBone->GetAt(ui);
        if (pkChild && NiIsKindOf(NiNode, pkChild))
        {
            unsigned int uiReturnedLOD = uiOrigLOD;
            BuildBoneMap((NiNode*) pkChild, kBoneToLOD, uiReturnedLOD);
            if (uiReturnedLOD > uiCurrentLOD)
            {
                uiCurrentLOD = uiReturnedLOD;
            }
        }
    }

    unsigned int uiLOD;
    if (kBoneToLOD.GetAt(pkBone, uiLOD))
    {
        if (uiLOD > uiCurrentLOD)
        {
            uiCurrentLOD = uiLOD;
        }
    }
    else
    {
        kBoneToLOD.SetAt(pkBone, uiCurrentLOD);
    }
}
//---------------------------------------------------------------------------
bool NiBoneLODController::FillSkinArray(
    const NiTPointerMap<NiTriBasedGeom*, NiUnsignedIntSet*>& kSkinToLOD,
    const NiTPointerMap<NiNode*, unsigned int>& kBoneToLOD,
    NiTPointerMap<unsigned int, bool>& kUsedLODs)
{
    NiTMapIterator kIter = kSkinToLOD.GetFirstPos();
    while (kIter)
    {
        NiTriBasedGeom* pkGeom;
        NiUnsignedIntSet* pkLODs;
        kSkinToLOD.GetNext(kIter, pkGeom, pkLODs);

        for (unsigned int ui = 0; ui < pkLODs->GetSize(); ui++)
        {
            unsigned int uiLOD = pkLODs->GetAt(ui);

            // This assertion was removed because this case is properly
            // handled below. However, this case, which occurs when there
            // are more skin LODs specified than BoneLODs, should generate
            // a warning for the user during export and be avoided through
            // the BoneLOD interface in the art packages.
//            NIASSERT(uiLOD < m_uiNumLODs);
            if (uiLOD >= m_uiNumLODs)
            {
                continue;
            }

            // Store geometry in SkinInfo object.
            SkinInfo* pkInfo = NiNew SkinInfo;
            pkInfo->m_pkSkinGeom = pkGeom;

            // Modify skin instance data for LOD.
            NiSkinInstance* pkNewInst = CreateLODSkinInstance(pkGeom,
                uiLOD, kSkinToLOD, kBoneToLOD);
            if (!pkNewInst)
            {
                NiDelete pkInfo;
                return false;
            }

            // Store skin instance in SkinInfo object.
            pkInfo->m_spSkinInst = pkNewInst;

            // Add SkinInfo object to m_pkSkinArray.
            SkinInfoSet* pkLODSet = m_kSkinArray.GetAt(uiLOD);
            if (!pkLODSet)
            {
                pkLODSet = NiNew SkinInfoSet;
                m_kSkinArray.SetAt(uiLOD, pkLODSet);
            }
            pkLODSet->Add(pkInfo);
            kUsedLODs.SetAt(uiLOD, true);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiSkinInstance* NiBoneLODController::CreateLODSkinInstance(
    NiTriBasedGeom* pkOldGeom, unsigned int uiDesiredLOD,
    const NiTPointerMap<NiTriBasedGeom*, NiUnsignedIntSet*>& kSkinToLOD,
    const NiTPointerMap<NiNode*, unsigned int>& kBoneToLOD)
{
    NiSkinInstance* pkOldSkinInst = pkOldGeom->GetSkinInstance();
    NIASSERT(pkOldSkinInst);
    NiSkinData* pkOldSkinData = pkOldSkinInst->GetSkinData();
    NIASSERT(pkOldSkinData);
    NiAVObject*const* ppkOldBones = pkOldSkinInst->GetBones();
    NIASSERT(ppkOldBones);
    NiSkinData::BoneData* pkOldBoneData = pkOldSkinData->GetBoneData();
    NIASSERT(pkOldBoneData);

    // Find bones to remove.
    NiUnsignedIntSet kBonesToRemove;
    NiTPrimitiveSet<NiAVObject*> kNewBones;
    BoneDataObjSet kNewBoneDataObjs;
    unsigned int ui;
    for (ui = 0; ui < pkOldSkinData->GetBoneCount(); ui++)
    {
        unsigned int uiBoneLOD;
        if (kBoneToLOD.GetAt((NiNode*) ppkOldBones[ui], uiBoneLOD))
        {
            if (uiBoneLOD < uiDesiredLOD)
            {
                kBonesToRemove.Add(ui);
            }
            else
            {
                kNewBones.Add(ppkOldBones[ui]);
                BoneDataObj* pkBoneDataObj = NiNew BoneDataObj;
                pkBoneDataObj->m_pkBoneData = &pkOldBoneData[ui];
                for (unsigned int uj = 0; uj < pkOldBoneData[ui].m_usVerts;
                    uj++)
                {
                    pkBoneDataObj->m_kBoneVertData.Add(
                        &pkOldBoneData[ui].m_pkBoneVertData[uj]);
                }
                kNewBoneDataObjs.Add(pkBoneDataObj);
            }
        }
    }

    // For each bone to remove, migrate the vertices it controls to the
    // nearest parent that is at an acceptable LOD.
    for (ui = 0; ui < kBonesToRemove.GetSize(); ui++)
    {
        unsigned int uiCurBoneIdx = kBonesToRemove.GetAt(ui);
        
        // Find next highest bone with an acceptable LOD.
        NiAVObject* pkValidBone = ppkOldBones[uiCurBoneIdx];
        unsigned int uiValidLOD;
        unsigned int uiValidBoneIdx = 0;
        bool bFoundValidBone = false;
        while (!bFoundValidBone)
        {
            pkValidBone = pkValidBone->GetParent();
            NIASSERT(pkValidBone);
            bool bSuccess = kBoneToLOD.GetAt((NiNode*) pkValidBone,
                uiValidLOD);

            if (!bSuccess)
            {
                // This assertion occurs when the current bone
                // ppkOldBones[uiCurBoneIdx] does not have a parent that
                // is at or above uiDesiredLOD and serves as a bone for one
                // of the skin instances. This situation should be avoided
                // in the source art.
                NiOutputDebugString("Error: Bone without valid bone parent "
                    "found during NiBoneLODController conversion.\n");

                char acMsg[1024];
                NiSprintf(acMsg, 1024, "\"%s\" is not active at Bone LOD %d. "
                    "It must have a bone above it that\nis active at Bone "
                    "LOD %d and that controls at least one vertex in a skin "
                    "that is\nactive at Bone LOD %d.\n\nFix this problem in "
                    "the art package and export again. No Bone LODs will be\n"
                    "created at this time.",
                    (const char*) 
                        (ppkOldBones[kBonesToRemove.GetAt(ui)]->GetName()),
                    uiDesiredLOD, uiDesiredLOD, uiDesiredLOD);
                NiMessageBox(acMsg, "Bone LOD Creation Error");

                // Delete allocated memory.
                for (ui = 0; ui < kNewBoneDataObjs.GetSize(); ui++)
                {
                    NiDelete kNewBoneDataObjs.GetAt(ui);
                }

                return NULL;
            }

            if (uiValidLOD < uiDesiredLOD)
            {
                continue;
            }

            // Find index of valid bone.
            for (uiValidBoneIdx = 0; uiValidBoneIdx < kNewBones.GetSize();
                uiValidBoneIdx++)
            {
                if (kNewBones.GetAt(uiValidBoneIdx) == pkValidBone)
                {
                    bFoundValidBone = true;
                    break;
                }
            }

            // If the valid bone is not currently in the kNewBones set,
            // try to find it in one of the other skins in the system.
            if (!bFoundValidBone)
            {
                NiSkinData::BoneData* pkTempBoneData = NULL;

                NiTMapIterator kIter = kSkinToLOD.GetFirstPos();
                while (kIter)
                {
                    NiTriBasedGeom* pkTempGeom;
                    NiUnsignedIntSet* pkTempLODSet;
                    kSkinToLOD.GetNext(kIter, pkTempGeom, pkTempLODSet);

                    NiSkinInstance* pkTempSkinInst =
                        pkTempGeom->GetSkinInstance();
                    NiSkinData* pkTempSkinData = pkTempSkinInst
                        ->GetSkinData();
                    for (unsigned int uiCount = 0;
                        uiCount < pkTempSkinData->GetBoneCount(); uiCount++)
                    {
                        if (pkTempSkinInst->GetBones()[uiCount] ==
                            pkValidBone)
                        {
                            pkTempBoneData =
                                &pkTempSkinData->GetBoneData()[uiCount];
                            break;
                        }
                    }

                    if (pkTempBoneData)
                    {
                        break;
                    }
                }

                if (pkTempBoneData)
                {
                    // Add the new bone and set uiValidBoneIdx.
                    kNewBones.Add(pkValidBone);
                    BoneDataObj* pkBoneDataObj = NiNew BoneDataObj;
                    pkBoneDataObj->m_pkBoneData = pkTempBoneData;
                    kNewBoneDataObjs.Add(pkBoneDataObj);
                    uiValidBoneIdx = kNewBones.GetSize() - 1;
                    bFoundValidBone = true;
                }
            }
        }

        // Propagate vertices from current bone to valid bone.
        BoneDataObj* pkNewBoneData = kNewBoneDataObjs.GetAt(uiValidBoneIdx);
        for (unsigned int uj = 0; uj < pkOldBoneData[uiCurBoneIdx].m_usVerts;
            uj++)
        {
            pkNewBoneData->m_kBoneVertData.Add(
                &pkOldBoneData[uiCurBoneIdx].m_pkBoneVertData[uj]);
        }
    }

    // Create the new arrays.
    unsigned int uiNumNewBones = kNewBones.GetSize();
    NiAVObject** ppkNewBones = NiAlloc(NiAVObject*, uiNumNewBones);
    NiSkinData::BoneData* pkNewBoneData =
        NiNew NiSkinData::BoneData[uiNumNewBones];

    // Fill the new arrays.
    for (ui = 0; ui < uiNumNewBones; ui++)
    {
        ppkNewBones[ui] = kNewBones.GetAt(ui);

        BoneDataObj* pkBoneDataObj = kNewBoneDataObjs.GetAt(ui);
        pkNewBoneData[ui].m_kBound = pkBoneDataObj->m_pkBoneData->m_kBound;
        pkNewBoneData[ui].m_kSkinToBone =
            pkBoneDataObj->m_pkBoneData->m_kSkinToBone;
        NIASSERT(pkBoneDataObj->m_kBoneVertData.GetSize() <
            (unsigned int) (unsigned short) ~0);
        pkNewBoneData[ui].m_usVerts =
            pkBoneDataObj->m_kBoneVertData.GetSize();
        pkNewBoneData[ui].m_pkBoneVertData =
            NiNew NiSkinData::BoneVertData[pkNewBoneData[ui].m_usVerts];

        for (unsigned short us = 0; us < pkNewBoneData[ui].m_usVerts; us++)
        {
            pkNewBoneData[ui].m_pkBoneVertData[us].m_fWeight =
                pkBoneDataObj->m_kBoneVertData.GetAt(us)->m_fWeight;
            pkNewBoneData[ui].m_pkBoneVertData[us].m_usVert =
                pkBoneDataObj->m_kBoneVertData.GetAt(us)->m_usVert;
        }
    }

    // Create the new NiSkinData and NiSkinInstance objects.
    NiSkinData* pkNewSkinData = NiNew NiSkinData(uiNumNewBones, pkNewBoneData,
        pkOldSkinData->GetRootParentToSkin(), pkOldGeom->GetVertices());
    pkNewSkinData->SortAndMergeBoneData();
    NiSkinInstance* pkNewSkinInst = NiNew NiSkinInstance(pkNewSkinData,
        pkOldSkinInst->GetRootParent(), ppkNewBones);

    // Delete allocated memory.
    for (ui = 0; ui < kNewBoneDataObjs.GetSize(); ui++)
    {
        NiDelete kNewBoneDataObjs.GetAt(ui);
    }

    return pkNewSkinInst;
}
//---------------------------------------------------------------------------
void NiBoneLODController::GetSkinData(NiTriBasedGeomSet& kGeoms,
    NiSkinInstanceSet& kInstances)
{
    kGeoms.RemoveAll();
    kInstances.RemoveAll();

    for (unsigned int ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet = m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                SkinInfo* pkInfo = pkSet->GetAt(uj);
                kGeoms.Add(pkInfo->m_pkSkinGeom);
                kInstances.Add(pkInfo->m_spSkinInst);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiBoneLODController::ReplaceSkin(NiTriBasedGeom* pkOldSkin,
    NiTriBasedGeom* pkNewSkin)
{
    unsigned int ui;

    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet = m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                SkinInfo* pkInfo = pkSet->GetAt(uj);
                if (pkInfo->m_pkSkinGeom == pkOldSkin)
                {
                    pkInfo->m_pkSkinGeom = pkNewSkin;
                }
            }
        }
    }

    for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
    {
        if (m_kSkinSet.GetAt(ui) == pkOldSkin)
        {
            m_kSkinSet.ReplaceAt(ui, pkNewSkin);
        }
    }
}
//---------------------------------------------------------------------------
bool NiBoneLODController::FindGeom(NiTriBasedGeom* pkGeom)
{
    for (unsigned int ui = 0; ui < m_kSkinSet.GetSize(); ui++)
    {
        if (pkGeom == m_kSkinSet.GetAt(ui))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBoneLODController);
//---------------------------------------------------------------------------
void NiBoneLODController::CopyMembers(NiBoneLODController* pkDest,
    NiCloningProcess& kCloning)
{
    NiTimeController::CopyMembers(pkDest, kCloning);

    pkDest->m_iLOD = m_iLOD;
    pkDest->m_uiNumLODs = m_uiNumLODs;
}
//---------------------------------------------------------------------------
void NiBoneLODController::ProcessClone(NiCloningProcess& kCloning)
{
    NiTimeController::ProcessClone(kCloning);

    NiObject* pkDestObj = 0;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkDestObj));
    NiBoneLODController* pkDest = (NiBoneLODController*)pkDestObj;

    unsigned int ui;

    for (ui = 0; ui < m_kBoneArray.GetSize(); ui++)
    {
        NiNodeSet* pkOldSet = m_kBoneArray.GetAt(ui);
        if (pkOldSet)
        {
            NiNodeSet* pkNewSet = NiNew NiNodeSet;
            pkDest->m_kBoneArray.SetAtGrow(ui, pkNewSet);

            for (unsigned int uj = 0; uj < pkOldSet->GetSize(); uj++)
            {
                NiNode* pkOldBone = pkOldSet->GetAt(uj);

                NiObject* pkNewBoneObj = 0;
                bool bCloned = kCloning.m_pkCloneMap->GetAt(pkOldBone,
                    pkNewBoneObj);
                NiNode* pkNewBone = (NiNode*)pkNewBoneObj;

                if (bCloned)
                {
                    pkNewSet->Add(pkNewBone);
                }
                else
                {
                    pkNewSet->Add(pkOldBone);
                }
            }
        }
        else
        {
            pkDest->m_kBoneArray.SetAtGrow(ui, NULL);
        }
    }

    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkOldSet = m_kSkinArray.GetAt(ui);
        if (pkOldSet)
        {
            SkinInfoSet* pkNewSet = NiNew SkinInfoSet;
            pkDest->m_kSkinArray.SetAtGrow(ui, pkNewSet);

            for (unsigned int uj = 0; uj < pkOldSet->GetSize(); uj++)
            {
                SkinInfo* pkOldInfo = pkOldSet->GetAt(uj);
                SkinInfo* pkNewInfo = NiNew SkinInfo;
                pkNewSet->Add(pkNewInfo);

                NiObject* pkNewSkinGeom = 0;
                bool bCloned = kCloning.m_pkCloneMap->
                    GetAt(pkOldInfo->m_pkSkinGeom, pkNewSkinGeom);
                if (bCloned)
                {
                    pkNewInfo->m_pkSkinGeom = (NiTriBasedGeom*)pkNewSkinGeom;
                }
                else
                {
                    pkNewInfo->m_pkSkinGeom = pkOldInfo->m_pkSkinGeom;
                }

                pkNewInfo->m_spSkinInst =
                    (NiSkinInstance*) pkOldInfo->m_spSkinInst->CreateClone(
                        kCloning);
                pkOldInfo->m_spSkinInst->ProcessClone(kCloning);
            }
        }
        else
        {
            pkDest->m_kSkinArray.SetAtGrow(ui, NULL);
        }
    }

    for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
    {
        NiTriBasedGeom* pkOldSkinGeom = m_kSkinSet.GetAt(ui);

        NiObject* pkNewSkinGeomObj = 0;
        bool bCloned = kCloning.m_pkCloneMap->GetAt(pkOldSkinGeom,
            pkNewSkinGeomObj);
        NiTriBasedGeom* pkNewSkinGeom = (NiTriBasedGeom*)pkNewSkinGeomObj;
        if (bCloned)
        {
            pkDest->m_kSkinSet.Add(pkNewSkinGeom);
        }
        else
        {
            pkDest->m_kSkinSet.Add(pkOldSkinGeom);
        }
    }

    int iLOD = pkDest->m_iLOD;
    pkDest->m_iLOD = -1;
    pkDest->SetBoneLOD(iLOD);
}
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
bool NiBoneLODController::RegisterStreamables(NiStream& kStream)
{
    if (!NiTimeController::RegisterStreamables(kStream))
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet = m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                SkinInfo* pkInfo = pkSet->GetAt(uj);
                if (pkInfo->m_spSkinInst)
                {
                    pkInfo->m_spSkinInst->RegisterStreamables(kStream);
                }
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBoneLODController::SaveBinary(NiStream& kStream)
{
    NiTimeController::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_iLOD);
    NiStreamSaveBinary(kStream, m_uiNumLODs);

    unsigned int ui;

    NiStreamSaveBinary(kStream, m_kBoneArray.GetSize());
    for (ui = 0; ui < m_kBoneArray.GetSize(); ui++)
    {
        NiNodeSet* pkSet = m_kBoneArray.GetAt(ui);
        if (pkSet)
        {
            NiStreamSaveBinary(kStream, pkSet->GetSize());
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                NiNode* pkBone = pkSet->GetAt(uj);
                kStream.SaveLinkID(pkBone);
            }
        }
        else
        {
            NiStreamSaveBinary(kStream, 0);
        }
    }

    NiStreamSaveBinary(kStream, m_kSkinArray.GetSize());
    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet = m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            NiStreamSaveBinary(kStream, pkSet->GetSize());
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                SkinInfo* pkInfo = pkSet->GetAt(uj);
                kStream.SaveLinkID(pkInfo->m_pkSkinGeom);
                kStream.SaveLinkID(pkInfo->m_spSkinInst);
            }
        }
        else
        {
            NiStreamSaveBinary(kStream, 0);
        }
    }

    NiStreamSaveBinary(kStream, m_kSkinSet.GetSize());
    for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
    {
        NiTriBasedGeom* pkGeom = m_kSkinSet.GetAt(ui);
        kStream.SaveLinkID(pkGeom);
    }
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBoneLODController);

//---------------------------------------------------------------------------
void NiBoneLODController::LoadBinary(NiStream& kStream)
{
    NiTimeController::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_iLOD);
    NiStreamLoadBinary(kStream, m_uiNumLODs);

    unsigned int ui;

    unsigned int uiBoneArraySize = 0;
    NiStreamLoadBinary(kStream, uiBoneArraySize);
    for (ui = 0; ui < uiBoneArraySize; ui++)
    {
        NiNodeSet* pkSet = NiNew NiNodeSet;
        m_kBoneArray.SetAtGrow(ui, pkSet);

        unsigned int uiSetSize = 0;
        NiStreamLoadBinary(kStream, uiSetSize);
        for (unsigned int uj = 0; uj < uiSetSize; uj++)
        {
            pkSet->Add((NiNode*) this);
            kStream.ReadLinkID();   // pkBone
        }
    }

    unsigned int uiSkinArraySize = 0;
    NiStreamLoadBinary(kStream, uiSkinArraySize);
    for (ui = 0; ui < uiSkinArraySize; ui++)
    {
        SkinInfoSet* pkSet = NiNew SkinInfoSet;
        m_kSkinArray.SetAtGrow(ui, pkSet);

        unsigned int uiSetSize = 0;
        NiStreamLoadBinary(kStream, uiSetSize);
        for (unsigned int uj = 0; uj < uiSetSize; uj++)
        {
            SkinInfo* pkInfo = NiNew SkinInfo;
            pkSet->Add(pkInfo);

            kStream.ReadLinkID();   // pkInfo->m_pkSkinGeom
            kStream.ReadLinkID();   // pkInfo->m_spSkinInst
        }
    }

    unsigned int uiSkinSetSize = 0;
    NiStreamLoadBinary(kStream, uiSkinSetSize);
    for (ui = 0; ui < uiSkinSetSize; ui++)
    {
        m_kSkinSet.Add((NiTriBasedGeom*) this);
        kStream.ReadLinkID();   // pkGeom
    }
}
//---------------------------------------------------------------------------
void NiBoneLODController::LinkObject(NiStream& kStream)
{
    NiTimeController::LinkObject(kStream);

    unsigned int ui;

    for (ui = 0; ui < m_kBoneArray.GetSize(); ui++)
    {
        NiNodeSet* pkSet = m_kBoneArray.GetAt(ui);
        if (pkSet)
        {
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                pkSet->ReplaceAt(uj,
                    (NiNode*) kStream.GetObjectFromLinkID());
            }
        }
    }

    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet = m_kSkinArray.GetAt(ui);
        if (pkSet)
        {
            for (unsigned int uj = 0; uj < pkSet->GetSize(); uj++)
            {
                SkinInfo* pkInfo = pkSet->GetAt(uj);
                pkInfo->m_pkSkinGeom =
                    (NiTriBasedGeom*) kStream.GetObjectFromLinkID();
                pkInfo->m_spSkinInst =
                    (NiSkinInstance*) kStream.GetObjectFromLinkID();
            }
        }
    }

    for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
    {
        m_kSkinSet.ReplaceAt(ui, (NiTriBasedGeom*)
            kStream.GetObjectFromLinkID());
    }
}
//---------------------------------------------------------------------------
void NiBoneLODController::PostLinkObject(NiStream& kStream)
{
    NiTimeController::PostLinkObject(kStream);

    int iLOD = m_iLOD;
    m_iLOD = -1;
    SetBoneLOD(iLOD);
}
//---------------------------------------------------------------------------
bool NiBoneLODController::IsEqual(NiObject* pkObject)
{
    if (!NiTimeController::IsEqual(pkObject))
    {
        return false;
    }

    NiBoneLODController* pkCtlr = (NiBoneLODController*) pkObject;

    if (m_iLOD != pkCtlr->m_iLOD ||
        m_uiNumLODs != pkCtlr->m_uiNumLODs ||
        m_kBoneArray.GetSize() != pkCtlr->m_kBoneArray.GetSize() ||
        m_kSkinArray.GetSize() != pkCtlr->m_kSkinArray.GetSize() ||
        m_kSkinSet.GetSize() != pkCtlr->m_kSkinSet.GetSize())
    {
        return false;
    }

    unsigned int ui;

    for (ui = 0; ui < m_kBoneArray.GetSize(); ui++)
    {
        NiNodeSet* pkSet0 = m_kBoneArray.GetAt(ui);
        NiNodeSet* pkSet1 = pkCtlr->m_kBoneArray.GetAt(ui);

        if ((pkSet0 && !pkSet1) || (!pkSet0 && pkSet1))
        {
            return false;
        }

        if (pkSet0 && pkSet1)
        {
            if (pkSet0->GetSize() != pkSet1->GetSize())
            {
                return false;
            }

            for (unsigned int uj = 0; uj < pkSet0->GetSize(); uj++)
            {
                if (!pkSet0->GetAt(uj)->IsEqual(pkSet1->GetAt(uj)))
                {
                    return false;
                }
            }
        }
    }

    for (ui = 0; ui < m_kSkinArray.GetSize(); ui++)
    {
        SkinInfoSet* pkSet0 = m_kSkinArray.GetAt(ui);
        SkinInfoSet* pkSet1 = pkCtlr->m_kSkinArray.GetAt(ui);

        if ((pkSet0 && !pkSet1) || (!pkSet0 && pkSet1))
        {
            return false;
        }

        if (pkSet0 && pkSet1)
        {
            if (pkSet0->GetSize() != pkSet1->GetSize())
            {
                return false;
            }

            for (unsigned int uj = 0; uj < pkSet0->GetSize(); uj++)
            {
                SkinInfo* pkInfo0 = pkSet0->GetAt(uj);
                SkinInfo* pkInfo1 = pkSet1->GetAt(uj);
                if (!pkInfo0->m_pkSkinGeom->IsEqual(pkInfo1->m_pkSkinGeom) ||
                    !pkInfo0->m_spSkinInst->IsEqual(pkInfo1->m_spSkinInst))
                {
                    return false;
                }
            }
        }
    }

    for (ui = 0; ui < m_kSkinSet.GetSize(); ui++)
    {
        if (!m_kSkinSet.GetAt(ui)->IsEqual(pkCtlr->m_kSkinSet.GetAt(ui)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBoneLODController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    pkStrings->Add(NiGetViewerString(NiBoneLODController::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("m_iLOD", m_iLOD));
    pkStrings->Add(NiGetViewerString("m_uiNumLODs", m_uiNumLODs));
}
//---------------------------------------------------------------------------
