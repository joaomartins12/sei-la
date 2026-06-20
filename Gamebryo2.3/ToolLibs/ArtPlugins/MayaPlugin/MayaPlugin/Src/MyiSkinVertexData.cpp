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

// Precompiled Headers
#include "MayaPluginPCH.h"
#include "maya/MFnTransform.h"



void AssignXformToMat( NiTransform *pXform, float mat[4][4]);

MyiSkinVertexData::
MyiSkinVertexData(int iShapeNum, int iGroupID, int iNumVertices, 
    NiOptVertex *pVertexList)
{
    m_pRootBone = NULL;
    m_pSkinToBoneXforms = NULL;
    m_pRootToSkinXform = NULL;

    int iNumWeights = 0;

    // Assume Shape skeleton (joint hiearchy) created.
    // Assume NiBones attached and can be retrieved.

    // (remember that verticies may need to be exploded,
    //  duplicated, etc...)
    int iNumJoints;
    int *piJoints;

    DtShapeGetAllJoints( iShapeNum, &iNumJoints,&piJoints );
    m_iNumBones = iNumJoints;

    // Check for No Joints
    if (iNumJoints == 0)
    {
        m_bFail = true;
        return;
    }

    // If the model was not set up correctly, there may be invalid joints, ie
    // having "-1" as an index. Below, we validate that all Joints have valid
    // indices and if not, we fail.
    for(int iJ=0; iJ<m_iNumBones; iJ++)
    {
        if (piJoints[iJ] == -1)
        {
            m_bFail = true;
            return;
        }
    }

    m_ppBones = NiAlloc(NiNode*, iNumJoints);
    m_pSkinToBoneXforms = NiNew NiTransform[iNumJoints];


    // DYANAMICALY ALLOCATE FOR THE SGO VERTEX INTERNAL DATA
    for( int iLoop = 0; iLoop < iNumVertices; iLoop++ )
    {
        pVertexList[iLoop].m_puiBoneIndex = NiAlloc(unsigned int, iNumJoints);
        pVertexList[iLoop].m_pfWeight = NiAlloc(float, iNumJoints);
    }

    int cnt;
    kMDtVertexWeights *pVertexWeights;
    DtShapeGetExplodedWeights( iShapeNum, iGroupID, &cnt, &pVertexWeights);
    NIASSERT(pVertexWeights!=NULL);

    int iCount = 0;

    // Generate piBoneIndices just in case we need them.
    int* piBoneIndices = NiAlloc(int, iNumJoints);
    for (int b=0;b<iNumJoints;b++)
        piBoneIndices[b] = iCount++;

    int j;
    for (j=0; j<cnt; j++)
    {
        pVertexList[j].m_uiNumBones = pVertexWeights[j].m_iSize;
        
#if _MSC_VER >= 1400
        unsigned int uiSize = sizeof(float)*pVertexWeights[j].m_iSize;
        int iRet = memcpy_s(pVertexList[j].m_pfWeight, uiSize,
            pVertexWeights[j].m_pfWeights, uiSize);
        NIASSERT(iRet == 0);
#else // #if _MSC_VER >= 1400
        memcpy(pVertexList[j].m_pfWeight, pVertexWeights[j].m_pfWeights,
            sizeof(float)*pVertexWeights[j].m_iSize);
#endif // #if _MSC_VER >= 1400
               
        if (pVertexWeights[j].m_piJointIndices)
        {
#if _MSC_VER >= 1400
            uiSize = sizeof(int)*pVertexWeights[j].m_iSize;
            int iRet = memcpy_s(pVertexList[j].m_puiBoneIndex, uiSize,
                pVertexWeights[j].m_piJointIndices, uiSize);
            NIASSERT(iRet == 0);
#else // #if _MSC_VER >= 1400
            memcpy(pVertexList[j].m_puiBoneIndex, 
                pVertexWeights[j].m_piJointIndices,
                sizeof(int)*pVertexWeights[j].m_iSize);
#endif // #if _MSC_VER >= 1400
        }
        else
        {
            // We have to use our generated indices.
#if _MSC_VER >= 1400
            uiSize = sizeof(int)*pVertexWeights[j].m_iSize;
            int iRet = memcpy_s(pVertexList[j].m_puiBoneIndex, uiSize, 
                piBoneIndices, uiSize);
            NIASSERT(iRet == 0);
#else // #if _MSC_VER >= 1400
            memcpy(pVertexList[j].m_puiBoneIndex, piBoneIndices, 
                sizeof(int)*pVertexWeights[j].m_iSize);
#endif // #if _MSC_VER >= 1400
        }

        // m_pfWeights and m_piJointIndices were shared between an MDt class
        // So we NULL it out to ensure we don't delete memory twice.
        pVertexWeights[j].m_pfWeights = NULL;
        pVertexWeights[j].m_piJointIndices = NULL;
    }

    NiExternalDelete [] pVertexWeights;
    NiFree(piBoneIndices);

    // The BoneIndex list is indexing the master joint list
    // We need it to index the position in m_ppBones
    // This is sort of hackish, but we will make a reverse list to modify our
    // joint indices to reflect the bone list.
    // The list we create acts as a translator... This is really wasteful
    // when the number of joints in the shape is much smaller than the total
    // joints in the scene.  
    int i;
    int iTotalJointsInScene = gJointGetTotalJointsInScene();


    // Check for No Joints
    if (iTotalJointsInScene == 0)
    {
        m_bFail = true;
        return;
    }

    int* piTranslator = NiAlloc(int, iTotalJointsInScene);
    memset(piTranslator,-1,sizeof(int)*iTotalJointsInScene);
    for(i=0;i<iNumJoints;i++)
    {
        piTranslator[piJoints[i]] = i;
    }

    // for each vertex
    for(j=0;j<cnt;j++)
    {
        for (i=0;i<(int)pVertexList[j].m_uiNumBones;i++)
        {
            pVertexList[j].m_puiBoneIndex[i] = 
                piTranslator[pVertexList[j].m_puiBoneIndex[i]];

            NIASSERT(pVertexList[j].m_puiBoneIndex[i] != -1);
        }
    }

    NiFree(piTranslator);

    for (i=0; i<iNumJoints; i++)
    {
        int iComponentID = 
            gMDtObjectFindComponentID( ETypeJoint, piJoints[i] );

        NiNode *ptr = gUserData.GetNode(iComponentID);

        NIASSERT(ptr != NULL);
        
        m_ppBones[i] = (NiNode*)ptr;

        // Get the SkinToBoneXForm
        float mat[4][4];
        DtShapeGetSkinToBoneMat( iShapeNum, piJoints[i], mat);
        AssignXformToMat( &m_pSkinToBoneXforms[i], mat );


        // Check for Non-Uniform Scale
        float fScale[3];
        fScale[0] = fScale[1] = fScale[2] = 1.0f;

        gMDtObjectGetScale( iComponentID, &fScale[0], &fScale[1], 
            &fScale[2] );

        // Only Compensate for Uniform Scale
        if (!MyiNode::Equivalent(fScale[0], fScale[1], fScale[2]))
        {
            MStatus s;
            MObject kMObject;
            gMDtObjectGetTransform(iComponentID, kMObject);
            MFnTransform kTransform(kMObject, &s);

            if (s != MS::kSuccess)
                return;

            DtExt_Err("Error:: Object ( %s ) Non-Uniform Scale"
                " unsupported in the bones of a Gamebryo skinned object.\n",
                kTransform.fullPathName().asChar()); 
        }
    }


    // Now let's do a check...
    // let's make sure that each SVD entry is influenced by a bone.
    // let's also make sure that the weights for each entry add to 1.
    for ( i=0; i<iNumVertices; i++)
    {
        // For each Weight

        float fWeight = 0.0;
        for (int j=0; j < (int)pVertexList[i].m_uiNumBones; j++)
        {
            // Go through each bone...
            fWeight += pVertexList[i].m_pfWeight[j];
        }
        
        // Check for No weights
        if(fWeight < 0.01f)
        {
            char cBuffer[512];
            const char* pcName; 

            DtShapeGetShapeName(iShapeNum, &pcName);
            NiSprintf(cBuffer, 512,
                "Error Skinning %s :: Vertex %d has no skin "
                "weights. Using default value\n", pcName, i);

            DtExt_Err(cBuffer);

            // Use Bone 0 by default using 1.0 as the weight
            if(pVertexList[i].m_uiNumBones == 0)
            {
                pVertexList[i].m_uiNumBones = 1;
                pVertexList[i].m_puiBoneIndex[0] = 0;
            }
            pVertexList[i].m_pfWeight[0] = 1.0f;
        }
        else if (fWeight  < 0.95f)
        {
            char cBuffer[512];
            const char* pcName; 

            DtShapeGetShapeName(iShapeNum, &pcName);
            NiSprintf(cBuffer, 512, 
                "Error Skinning %s :: Vertex %d skin weights "
                "don't total 1.0. Increasing the weight of the first bone.\n",
                pcName, i);

            DtExt_Err(cBuffer);

            // Increase the weight of the first bone
            pVertexList[i].m_pfWeight[0] += 1.0f - fWeight;
        }
        else if (fWeight > 1.05f)
        {
            char cBuffer[512];
            const char* pcName; 

            DtShapeGetShapeName(iShapeNum, &pcName);
            NiSprintf(cBuffer, 512,
                "Error Skinning %s :: Vertex %d skin weights "
                "total exceeds 1.0. Only using the first bone.\n", pcName, i);

            DtExt_Err(cBuffer);

            // Increase the weight of the first bone
            pVertexList[i].m_uiNumBones = 1;
            pVertexList[i].m_pfWeight[0] += 1.0f;
        }
    }

    // Need to get the SkinToBoneXform's for each bone.


    // We need to get the RootParentToSkinXform
    // We will assume that the RootParent xform is always the identity
    // and so we just  need the inverse of the world xform for the mesh/
    // skin.
    // 1. Get Shapes inverse world xform
    float mat[4][4];
    DtShapeGetInverseMatrix( iShapeNum, mat);

    m_pRootToSkinXform = NiNew NiTransform();
    AssignXformToMat( m_pRootToSkinXform, mat );

    // 2. Get Shape and Skeletons Root Node (should be same)
    // Starting with a joint...
    int iComponentID = gMDtObjectFindComponentID( ETypeJoint, piJoints[0] );
    NiNode *ptr = gUserData.GetNode(iComponentID);
    
//    // Find (2nd to) top most parent 
//    // We want the 2nd to, because we don't want the scene root
//    while (ptr->GetParent() && ptr->GetParent()->GetParent())
//    {
//        ptr = ptr->GetParent();
//    }
    
    // Find top most parent 
    // We want the 2nd to, because we don't want the scene root
    while (ptr->GetParent())
    {
        ptr = ptr->GetParent();
    }

    m_pRootBone = ptr;
    
    if (piJoints)
        NiFree(piJoints);
    piJoints = NULL;

    m_bFail = false;
}

MyiSkinVertexData::
~MyiSkinVertexData( void )
{
    NiDelete m_pRootToSkinXform;
}

void MyiSkinVertexData::
AddSVDataFromThisJoint(int iJoint)
{
;
}

void AssignXformToMat( NiTransform *pXform, float mat[4][4])
{
    pXform->m_Rotate.SetCol(0,mat[0][0],mat[0][1],mat[0][2]);
    pXform->m_Rotate.SetCol(1,mat[1][0],mat[1][1],mat[1][2]);
    pXform->m_Rotate.SetCol(2,mat[2][0],mat[2][1],mat[2][2]);

    pXform->m_Translate.x = mat[3][0] * gExport.m_fLinearUnitMultiplier;
    pXform->m_Translate.y = mat[3][1] * gExport.m_fLinearUnitMultiplier;
    pXform->m_Translate.z = mat[3][2] * gExport.m_fLinearUnitMultiplier;
    
    pXform->m_fScale = 1.0f;
}
