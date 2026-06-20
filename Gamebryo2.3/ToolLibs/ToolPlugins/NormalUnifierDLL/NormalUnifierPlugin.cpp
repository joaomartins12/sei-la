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
#include "NormalUnifierPlugin.h"

// RTTI implementation macro.
NiImplementRTTI(NormalUnifierPlugin, NiPlugin);

//---------------------------------------------------------------------------
NormalUnifierPlugin::NormalUnifierPlugin() : NiPlugin(
    "Unify Normals Plug-in",                // name
    "1.0",                                          // version
    "A plug-in to unify normals amongst separate pieces of geometry",
    // short description
    "Averages normals of separate meshes where edges line up, removing "
    "seams.\nAll objects being unified must have \"NiUnifyNormals\" added "
    "to their user-defined properties.")
    // long description
{
    m_fTolerance = 0.001f;
}
//---------------------------------------------------------------------------
NiPluginInfo* NormalUnifierPlugin::GetDefaultPluginInfo()
{
    // Create the default parameters for this plug-in.
    NiPluginInfo* pkPluginInfo = NiNew NiPluginInfo();
    pkPluginInfo->SetName(m_strName);
    pkPluginInfo->SetVersion(m_strVersion);
    pkPluginInfo->SetClassName("NormalUnifierPlugin");
    pkPluginInfo->SetType("PROCESS");
    
    return pkPluginInfo;
}
//---------------------------------------------------------------------------
bool NormalUnifierPlugin::HasManagementDialog()
{
    // Since we have parameters that can be changed, we provide a management
    // dialog, otherwise we would return false.
    return false;
}
//---------------------------------------------------------------------------
bool NormalUnifierPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    return false;
}
//---------------------------------------------------------------------------
bool NormalUnifierPlugin::CanExecute(NiPluginInfo* pkInfo)
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

    if (pkInfo->GetClassName() == "NormalUnifierPlugin" &&
        VerifyVersionString(pkInfo->GetVersion()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr NormalUnifierPlugin::Execute(
    const NiPluginInfo* pkInfo)
{

    // Coming into this method, CanExecute has already been called.
    // Therefore, we know that we can actually execute pkInfo.

    // We'll likely want to use the scene graph in this process plug-in,
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
            pkLogger->LogElement("NormalUnifierError", "The Normal Unifier "
                "Process Plug-in could not find the NiSceneGraphSharedData!");
        }
        else
        {
            NiMessageBox("The Normal Unifier Process Plug-in could not find "
                "the NiSceneGraphSharedData!","Scene Graph Shared Data "
                "Missing");
        }

        return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_FAILURE);
    }


    // While 3ds max and Maya by default only generate one root, other 
    // plug-ins in the pipeline could potentially add new roots.
    for (unsigned int ui = 0; ui < pkSGSharedData->GetRootCount(); ui++)
    {
        NiNode* pkRoot = pkSGSharedData->GetRootAt(ui);
        pkRoot->Update(0.0f);
        CollectGeometry(pkRoot);
    }

    CollectSets();
    HandleSets();

    m_kGeoms.RemoveAll();
    m_kSets.RemoveAll();

    // Presumably, we have correctly executed our actions on the scene
    // graph. We return success in this case.
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
void NormalUnifierPlugin::CollectGeometry(NiAVObject* pkObject, bool bAutoAdd)
{
    if (pkObject == NULL)
        return;
    
    if (CheckForExtraDataTags(pkObject))
        bAutoAdd = true;

    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*) pkObject;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            CollectGeometry(pkNode->GetAt(ui), bAutoAdd);
        }
    }
    else if (NiIsKindOf(NiGeometry, pkObject) && bAutoAdd == true)
    {
        m_kGeoms.Add((NiGeometry*)pkObject);
    }
}
//---------------------------------------------------------------------------

void NormalUnifierPlugin::CollectSets()
{
    for (unsigned int ui = 0; ui < m_kGeoms.GetEffectiveSize(); ui++)
    {
        // We'll scale out the radius a little to ensure we have intersections
        NiGeometry* pkGeom1 = m_kGeoms.GetAt(ui);
        m_kAlreadyHitVerts.SetAt(pkGeom1, NiNew NiUnifyVerts);
        NiBound kBound1 = pkGeom1->GetWorldBound();
        float fRadius = kBound1.GetRadius();
        fRadius += fRadius * 0.01f;
        kBound1.SetRadius(fRadius);

        if (pkGeom1)
        {
            NiUnifySetPtr spSet = NiNew NiUnifySet;
            spSet->m_kGeoms.Add(pkGeom1);
            
            for (unsigned int uj = 0; uj < m_kGeoms.GetEffectiveSize(); uj++)
            {
                NiGeometry* pkGeom2 = m_kGeoms.GetAt(uj);
                NiBound kBound2 = pkGeom2->GetWorldBound();
                float fRadius = kBound2.GetRadius();
                fRadius += fRadius * 0.01f;
                kBound2.SetRadius(fRadius);

                if (pkGeom2 == pkGeom1 || pkGeom2 == NULL)
                    continue;

                NiPoint3 kVel1(0.0001f, 0.0001f, 0.0001f);
                NiPoint3 kVel2(0.0001f, 0.0001f, 0.0001f);
                
                // If the bounding volumes intersect, we should try to unify 
                // normals
                if (NiBound::TestIntersect(0.01f, kBound1, kVel1, kBound2, 
                    kVel2))
                {
                    spSet->m_kGeoms.Add(pkGeom2);
                }
            }

            if (spSet->m_kGeoms.GetEffectiveSize() > 1)
                m_kSets.Add(spSet);
        }
    }
}
//---------------------------------------------------------------------------

void NormalUnifierPlugin::HandleSets()
{
    for (unsigned int ui = 0; ui < m_kSets.GetEffectiveSize(); ui++)
    {
        NiUnifySet* pkSet = m_kSets.GetAt(ui);
        if (pkSet)
            Unify(pkSet);
    }
}

//---------------------------------------------------------------------------
bool CloseTo(float fA, float fB, float fEpsilon)
{
    // Determines if float fA is within fEpsilon of float fB.

    // Returns: true, if fA is within fEpsilon of fB.
    //          false, otherwise.

    return ((float) fabs(fA - fB) <= fEpsilon);
}
//---------------------------------------------------------------------------
bool Point3CloseTo(const NiPoint3& kP1,
    const NiPoint3& kP2, float fEpsilon)
{
    // Determines if point kP1 is within fEpsilon of point kP2.

    // Returns: true, if kP1 is within fEpsilon of kP2.
    //          false, otherwise.

    if (!CloseTo(kP1.x, kP2.x, fEpsilon))
    {
        return false;
    }
    if (!CloseTo(kP1.y, kP2.y, fEpsilon))
    {
        return false;
    }
    if (!CloseTo(kP1.z, kP2.z, fEpsilon))
    {
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------
bool NormalUnifierPlugin::AlreadyHitThisVertex(NiUnifyVerts* pkVertexArray, 
                                               unsigned short uiIndex)
{
    uiIndex += 1; // To get it into the correct span
    if (pkVertexArray == NULL)
        return false;

    unsigned int uiIndexCount = pkVertexArray->m_ausIndices.GetEffectiveSize();
    for (unsigned int ui = 0; ui < uiIndexCount; ui++)
    {
        if (pkVertexArray->m_ausIndices.GetAt(ui) == uiIndex)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------

void NormalUnifierPlugin::Unify(NiUnifySet* pkSet)
{

    /*  
        Okay boys and girls, here we got with this confusing algorithm By this
        point, we know that for any given vertex in the first mesh of the unify
        set, we have all meshes that share that same vertex.  This is key, b/c
        we want to average across all coincident vertices in all meshes. I know
        this is slow, but this is more of a global optimization problem than
        anything else.

        So here's what we're going to do. For each mesh, we are going to
        collect all vertices that are coincident with our vertex.

        X-----XX-----X
        |     ||     |
        |  A  ||  B  |
        |     ||     |
        X-----XX-----X
        X-----XX-----X
        |     ||     |
        |  C  ||  D  |
        |     ||     |
        X-----XX-----X

        The central vertex of meshes ABCD needs to aligned to the avg normal of
        all four meshes. This will produce the pleasing visual result we
        desire.

        Once we've set the normal in common on ABCD, we cache that index to
        insure that we never set it again.
    */

    NiGeometry* pkObj1 = pkSet->m_kGeoms.GetAt(0);
    unsigned short usVertCount1 = pkObj1->GetVertexCount();
    NiGeometryData* pkData1 = pkObj1->GetModelData();
    NiTransform kWorldTransform1 = pkObj1->GetWorldTransform();

    if (pkData1 == NULL)
        return;

    // For any given vertex in our mesh, we must find all concurrent vertices
    // in all adjacent meshes. Average the normals for that mesh and set the
    // same value for all meshes.

    float fTolerance = m_fTolerance * pkObj1->GetWorldBound().GetRadius();
    const NiPoint3* pkData1Verts = pkData1->GetVertices();
    NiPoint3* pkData1Norms = pkData1->GetNormals() ;

    unsigned int uiNumItems = pkSet->m_kGeoms.GetEffectiveSize();
    NiPoint3** ppkVerticesArray = NiAlloc(NiPoint3*, uiNumItems);
    NiPoint3** ppkNormalsArray  = NiAlloc(NiPoint3*, uiNumItems);
    NiTransform* pkTransformArray = NiNew NiTransform[uiNumItems];
    unsigned short* pkVertCountArray = NiAlloc(unsigned short, uiNumItems);
    

    memset(ppkVerticesArray, 0, sizeof(NiPoint3*));
    memset(ppkNormalsArray, 0, sizeof(NiPoint3*));
    memset(pkVertCountArray, 0, sizeof(unsigned short));

    for (unsigned int ui = 0; ui < uiNumItems; ui++)
    {
        NiGeometry* pkObj = pkSet->m_kGeoms.GetAt(ui);
        if (pkObj)
        {
            NiGeometryData* pkData = pkObj->GetModelData();
            NiTransform kWorldTransform = pkObj->GetWorldTransform();

            if (pkData)
            {
                ppkVerticesArray[ui] = pkData->GetVertices();
                ppkNormalsArray[ui] = pkData->GetNormals();
                pkTransformArray[ui] = kWorldTransform;
                pkVertCountArray[ui] = pkData->GetVertexCount();
            }
        }
    }

    // we start at 1, b/c we're always testing agains the 0'th item

    NiUnifyVertsPtr spMasterUnifiedVerts = NULL;
    m_kAlreadyHitVerts.GetAt(pkObj1, spMasterUnifiedVerts);

    if (spMasterUnifiedVerts == NULL)
        spMasterUnifiedVerts = NiNew NiUnifyVerts;

    for (unsigned short uiVert = 0; uiVert < usVertCount1; uiVert++)
    {
        // Skip this vertex if we've averaged it before
        if (AlreadyHitThisVertex(spMasterUnifiedVerts, uiVert))
            continue;

        NiUnifyVerts* pkHitVerts = NiNew NiUnifyVerts[uiNumItems];

        // Transform this vertex into world space
        NiPoint3 kVert1 = kWorldTransform1 * pkData1Verts[uiVert];
        
        float fNumAveragedNormals = 1.0f;
        NiPoint3 kNormalsSum = pkTransformArray[0].m_Rotate * 
                        ppkNormalsArray[0][uiVert];//(0.0f, 0.0f, 0.0f);

        // Collect all the vertices on all adjacent meshes that match
        // up to this vertex

        for (unsigned int uiItem = 1; uiItem < uiNumItems; uiItem++)
        {
            // Get the vertex cache for this object so we can cache
            // this vertex to skip later
            NiUnifyVertsPtr spUnifiedVerts = NULL;
            m_kAlreadyHitVerts.GetAt(pkSet->m_kGeoms.GetAt(uiItem), 
                spUnifiedVerts);
   
            for (unsigned short uj = 0; uj < pkVertCountArray[uiItem]; uj++)
            {
                if (AlreadyHitThisVertex(spUnifiedVerts, uj))
                    continue;

                NiPoint3 kVert2 = pkTransformArray[uiItem] * 
                    ppkVerticesArray[uiItem][uj];

                if (Point3CloseTo(kVert1, kVert2, fTolerance))
                {
                    fNumAveragedNormals += 1.0f;
                    // Rotate the normal into world space
                    NiPoint3 kNormal = pkTransformArray[uiItem].m_Rotate * 
                        ppkNormalsArray[uiItem][uj];
                    kNormalsSum += kNormal; 

                    // add this one to the list for the current mesh
                    pkHitVerts[uiItem].m_ausIndices.Add(uj+1);

                    // Skip this one when its parent mesh comes around
                    spUnifiedVerts->m_ausIndices.Add(uj+1);
                }
            }
        }
       
        // add this one to the list for the current mesh
        pkHitVerts[0].m_ausIndices.Add(uiVert+1);
        // Skip this one when its parent mesh comes around
        spMasterUnifiedVerts->m_ausIndices.Add(uiVert+1);

        // Average the normals now
        NiPoint3 kAvgNormal = kNormalsSum / fNumAveragedNormals;
        kAvgNormal.Unitize();

        // For all the normals we averaged, let's set their values
        // to the new, unified value
        for (unsigned int uiSetItem = 0; uiSetItem < uiNumItems; uiSetItem++)
        {
            for (unsigned int ui = 0; ui < 
                pkHitVerts[uiSetItem].m_ausIndices.GetEffectiveSize(); ui++)
            {
                // Get the vertex index
                unsigned short usVertIndex = 
                    pkHitVerts[uiSetItem].m_ausIndices.GetAt(ui) - 1;

                // Get the normal and set it to the value of the average normal
                // rotated back into model space
                NiTransform kWorldTransform = pkTransformArray[uiSetItem];
                ppkNormalsArray[uiSetItem][usVertIndex] = 
                    kWorldTransform.m_Rotate.Inverse() * kAvgNormal;

            }
        }

        NiDelete [] pkHitVerts;
    }

    NiFree(ppkVerticesArray);
    NiFree(ppkNormalsArray);
    NiDelete [] pkTransformArray;
    NiFree(pkVertCountArray);

/*  // OLD CODE THAT WORKS FOR ONLY TWO OBJECTS
    unsigned short usVertCount2 = pkObj2->GetVertexCount();

    NiGeometryData* pkData2 = pkObj2->GetModelData();
    NiTransform kWorldTransform2 = pkObj2->GetWorldTransform();

    if (pkData1 == NULL || pkData2 == NULL)
        return;

    
    const NiPoint3* pkData2Verts = pkData2->GetVertices();
    NiPoint3* pkData2Norms = pkData2->GetNormals();

    for (unsigned short ui = 0; ui < usVertCount1; ui++)
    {
        NiPoint3 kVert1 = kWorldTransform1 * pkData1Verts[ui];
        
        for (unsigned short uj = 0; uj < usVertCount2; uj++)
        {
            NiPoint3 kVert2 = kWorldTransform2 * pkData2Verts[uj];

            if (Point3CloseTo(kVert1, kVert2, fTolerance))
            {
                NiPoint3 kAvgNormal = 
                    (kWorldTransform1.m_Rotate * pkData1Norms[ui] + 
                    kWorldTransform2.m_Rotate * pkData2Norms[uj]) 
                    / 2.0f;
                kAvgNormal.Unitize();
                pkData1Norms[ui] = 
                    kWorldTransform1.m_Rotate.Inverse() * kAvgNormal;
                pkData2Norms[uj] = 
                    kWorldTransform2.m_Rotate.Inverse() * kAvgNormal;
                continue;
            }
        }
    }
    */
}

//---------------------------------------------------------------------------
bool NormalUnifierPlugin::CheckForExtraDataTags(NiAVObject* pkObject)
{
    // THIS WAS CUT AND PAST DIRECTLY FROM NIOPTIMIZE

    // Checks for extra data strings that indicate an object should not
    // be removed.

    // Returns: true, if the specified extra data tags exist.
    //          false, otherwise.

    const char* ppcTags[1] = {"NiUnifyNormals"};

    return CheckForExtraDataTags(pkObject, ppcTags, 1);
}
//---------------------------------------------------------------------------
bool NormalUnifierPlugin::CheckForExtraDataTags(NiAVObject* pkObject, 
    const char* ppcTags[], unsigned int uiNumTags)
{
    // THIS WAS CUT AND PAST DIRECTLY FROM NIOPTIMIZE

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
      