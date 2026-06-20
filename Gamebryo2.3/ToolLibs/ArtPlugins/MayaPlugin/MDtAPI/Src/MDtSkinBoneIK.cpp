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

// MDtSkinBoneIK.cpp

// Precompiled Headers
#include "MDtAPIPCH.h"
#include "maya/MFnPointArrayData.h"

extern const char * objectName( MObject object );
extern const char * objectType( MObject object );

extern int DtExt_FindShapeWithDagpath(MDagPath dagPath);
extern int DtExt_FindShapeWithNode(MObject obj);
extern bool DtExt_GetShapeHasSkin(int iShapeID);
extern void DtExt_SetShapeHasSkin(int iShapeID, bool bVal);
extern int  DtShapeSetVertex( int shapeID, int vert, float x, float y, 
    float z);

extern void DtShapeAddJoint( int iShape, kMDtJointInfo *pJ );
extern kMDtJointInfo* DtShapeJointGet(int index,int iShape);
extern void DtShapeStoreWeights( int iShape, int iVertex, int iNumWeights, 
    float* pfWeights, int *piJointIndices);

int MyDtShapeGetVertices( MObject&  objInput, MObject&  objOutput, 
    int* pcVertices, DtVec3f** prgVertices);

// Global
kDtIK gIKData;

//---------------------------------------------------------------------------
void AssertGood(MStatus &status)
{
    NIASSERT(status==MS::kSuccess);
}
//---------------------------------------------------------------------------
kDtIK::kDtIK(void){};
//---------------------------------------------------------------------------
kDtIK::~kDtIK(void){};
//---------------------------------------------------------------------------
void kDtIK::GetSmoothSkinningInfo(void)
{
    MStatus status;

    // Iterate through all the kSkinClusterFilters's as these are
    // the key nodes for find info about joints and the
    // geometry that they deform.
    
    MItDependencyNodes DNIter( MFn::kSkinClusterFilter, &status);  

    int iTotal;
    for (iTotal = 0; !DNIter.isDone(); DNIter.next(), iTotal++ );
    DNIter.reset();

    if(DtExt_HaveGui() == true)
    {
        // Update the progress window
        MProgressWindow::setProgressStatus("Extracting Smooth Skinning Info");
        MProgressWindow::setProgressRange(0, iTotal);
        MProgressWindow::setProgress(0);
    }


    for (; !DNIter.isDone(); DNIter.next() )
    {
        if(DtExt_HaveGui() == true)
        {
            // Increment the progress window
            MProgressWindow::advanceProgress(1);
        }

        MObject object = DNIter.item();
        if (object.apiType() != MFn::kSkinClusterFilter)
            continue;

        // For each skin cluster node, get the list of influence objects
        MFnSkinCluster skinCluster(object);
        MDagPathArray infs;
        unsigned int nInfs = skinCluster.influenceObjects(infs, &status);

        AssertGood(status);

        if (nInfs == 0)
        {
            DtExt_Err("WARNING: Smooth Skin without any Influence Objects\n");
            continue;
        }

        // Loop through geometries affected by this cluster
        unsigned int nGeoms = skinCluster.numOutputConnections();
        for (size_t ii = 0; ii < nGeoms; ++ii)
        {
            unsigned int Index = 
                skinCluster.indexForOutputConnection(ii,&status);

            AssertGood(status);

            MDagPath skinPath;
            status = skinCluster.getPathAtIndex(Index,skinPath);
            AssertGood(status);

//            int iShape = DtExt_FindShapeWithNode(skinPath.node());
            int iShape = DtExt_FindShapeWithDagpath( skinPath );

            if (iShape < 0)
            {
                // This is probably occuring because the Maya user is
                // attempting to skin non-polygonal geometry - like
                // a NURB.  We can't do this... so escape.
                DtExt_Err("WARNING: Can't Skin associated Geometry\n");
                continue;
            }

            // The shape has a skin - so lets mark it as such.
            DtExt_SetShapeHasSkin(iShape,true);

          // We already have the verts don't get them again

            // Find the Orignal bind pose vertices...
            // and store them now...
/*
            MObjectArray rgInputs;
            skinCluster.getInputGeometry(rgInputs);
            int InputLen = rgInputs.length();
            for(int inputIndex=0;inputIndex<InputLen;inputIndex++)
            {
                MObject objInput = rgInputs[inputIndex];
                NIASSERT(objInput.hasFn(MFn::kMesh));
                MFnMesh fnInput(objInput);

                int numVerts = fnInput.numVertices();
                MFloatPointArray rgOrigVertices;
                fnInput.getPoints(rgOrigVertices);
                for (int iVertex = 0; iVertex < numVerts; iVertex++) 
                {
                    float x = rgOrigVertices[iVertex][0];
                    float y = rgOrigVertices[iVertex][1];
                    float z = rgOrigVertices[iVertex][2];

                    DtShapeSetVertex( iShape, iVertex, x, y, z);
                }
            }
*/
            // GET THE MESH XFORM
            MObject objTransform;
            DtExt_ShapeGetTransform(iShape,objTransform);
            MDagPath    pathTransform;
            MFnDagNode(objTransform).getPath(pathTransform);
            MMatrix matMeshWorldTransform = pathTransform.inclusiveMatrix();

            int numvertsTest;
            DtShapeGetVertexCount( iShape, &numvertsTest );
            // Get the number of vertices

            // Store the weights on a per vertex basis in the shape...
            unsigned int infCount;

            MItGeometry gIter(skinPath);
            int iCurrVertex = -1;
            for ( ; !gIter.isDone(); gIter.next() )
            {
                iCurrVertex++;
                MObject comp = gIter.component(&status);
                AssertGood(status);
                const char* apiName = comp.apiTypeStr();
                MFloatArray wts;

                switch (comp.apiType())
                {
                    case MFn::kMeshVertComponent:
                    {
                        status = skinCluster.getWeights(skinPath, comp, 
                            wts, infCount);

                        AssertGood(status);

                        if (infCount == 0)
                            DtExt_Err("ERROR: Smooth Skin has 0 Influence "
                                "Objects.");

                        unsigned l = wts.length(); 


                        // For every joint that has an influence...
                        // This for loop may be overkill because usually,
                        // indices will be in order 0-1-2-3-4-5...
                        // If we could guarantee this, we wouldn't have to
                        // do the for loop, allocate memory, etc on a per
                        // vertex basis.
                        NIASSERT(l>0);

                        int* piJointIndices = NULL;

                        piJointIndices = NiAlloc(int, l);
                        for (int j=0; j<(int)l; j++)
                        {
                             MObject jointObj = infs[j].node();
                             int iJointIndex = 
                                 gJointXformMan.GetJoint( jointObj );

                             piJointIndices[j] = iJointIndex;
                        }

                        float *pfWeights = NiAlloc(float, l);
                        wts.get(pfWeights);

                        // Allocate space for JointIndices
                        // Find all joint indices (vertIter?)
                        DtShapeStoreWeights(iShape,iCurrVertex,l,pfWeights, 
                            piJointIndices);
                    }
                    break;

                    default:
                        break;
                }
            }

            // For each joint or weight...
            for (int weightIndex=0; weightIndex<(int)nInfs; ++weightIndex)
            {
                const char* jointname = 
                    infs[weightIndex].partialPathName().asChar();
                
                // Should already exist - so find it...
                MObject jointObj = infs[weightIndex].node();
                
                kMDtJointInfo *pJointInfo = NiExternalNew kMDtJointInfo;
                pJointInfo->m_iJointIndex = 
                    gJointXformMan.GetJoint( jointObj );

                DtShapeAddJoint( iShape, pJointInfo );

                MFnIkJoint fnBone(infs[weightIndex]);
                MObject objBindPose;
                fnBone.findPlug("bindPose").getValue(objBindPose);
                MFnMatrixData fnBindPose(objBindPose);
                (matMeshWorldTransform * fnBindPose.matrix().inverse()).
                    get(pJointInfo->m_fSkinToBoneMat);
           }
        }
    }
}
//---------------------------------------------------------------------------
void kDtIK::GetRigidSkinningInfo(void)
{
    MStatus status;

    // Iterate through all the kJointCluster's as these are
    // the key nodes for find info about joints and the
    // geometry that they deform.
    MItDependencyNodes DNIter( MFn::kJointCluster, &status);  

    int iTotal;
    for (iTotal = 0; !DNIter.isDone(); DNIter.next(), iTotal++ );
    DNIter.reset();

    if(DtExt_HaveGui() == true)
    {
        // Update the progress window
        MProgressWindow::setProgressStatus("Extracting Rigid Skinning Info");
        MProgressWindow::setProgressRange(0, iTotal);
        MProgressWindow::setProgress(0);
    }

    for (; !DNIter.isDone(); DNIter.next() )
    {
        if(DtExt_HaveGui()== true)
        {
            // Increment the progress window
            MProgressWindow::advanceProgress(1);
        }

        MObject clusterHandle = DNIter.item(&status);
        const char *pName = objectName( clusterHandle );

        MFnWeightGeometryFilter fnCluster(clusterHandle); 

        if (!clusterHandle.hasFn(MFn::kClusterFilter))
            continue;
        
        MFnDependencyNode   cluster(clusterHandle);

        MObject attrWeightsList = cluster.attribute("wl");
        MPlug   plugWeights = cluster.findPlug("weights",&status);
        AssertGood(status);

        MPlug plugOutputGeoms   = cluster.findPlug("outputGeometry",&status); 
        AssertGood(status);
        unsigned numOutputGeoms = plugOutputGeoms.numConnectedElements();

        MPlug plugInputs = cluster.findPlug("input",&status);       
        AssertGood(status);
        
        MObject attrGroupId = cluster.attribute("groupId",&status); 
        AssertGood(status);

        unsigned i;
        // for each joint
        for (i=0; i< numOutputGeoms;i++)
        {
            MPlug plugOutputGeom = 
                plugOutputGeoms.elementByPhysicalIndex(i,&status);

            AssertGood(status);
            unsigned logicalIndex = plugOutputGeom.logicalIndex(&status);
            AssertGood(status);

            MPlug plugInput = plugInputs.elementByLogicalIndex(logicalIndex);
            MPlug plugGroupId = plugInput.child(attrGroupId,&status);
            AssertGood(status);

            if (!plugGroupId.isConnected())
                continue;

            status = plugWeights.selectAncestorLogicalIndex(logicalIndex,
                attrWeightsList);

            AssertGood(status);

            // plugOutputGeom links us to joints, etc...
            // we want to know the shape(s) that it deforms...
            MItDependencyGraph dgIter(plugOutputGeom,MFn::kShape);

            MObject outputGeom = dgIter.thisNode();
            const char *geomName = objectName( outputGeom );

            int iShapeForThisJoint = DtExt_FindShapeWithNode(outputGeom);

            if (DtExt_GetShapeHasSkin(iShapeForThisJoint) == false)
            {
                // We haven't processed this object yet.  This is our first 
                // time.

                DtExt_SetShapeHasSkin(iShapeForThisJoint,true);

                MObject objOutput;
                DtExt_ShapeGetShapeNode(iShapeForThisJoint, objOutput);
                const char *outputName = objectName( objOutput );
                unsigned int uiLen = strlen(outputName) + strlen("Orig") + 1;
                char* szOrigName = NiAlloc(char, uiLen);
                NiStrcpy(szOrigName, uiLen, outputName);
                NiStrcat(szOrigName, uiLen, "Orig");

                // Find the Orignal bind pose vertices...
                // and store them now...
                MObjectArray rgInputs;
                fnCluster.getInputGeometry(rgInputs);
                int InputLen = rgInputs.length();
                for(int inputIndex=0;inputIndex<InputLen;inputIndex++)
                {
                    MObject objInput = rgInputs[inputIndex];
                    const char *inputName = objectName( objInput );

                    // If the input doesn't connect to the output...
                    if (strcmp(inputName,szOrigName)!=0)
                        continue;

//MyDtShapeGetVertices could be modified... it handles tweaks, etc.
                    //DtVec3f* prgVertices;
                    //int iVertices;
//MyDtShapeGetVertices( objInput, objOutput, &iVertices, &prgVertices);

                    NIASSERT(objInput.hasFn(MFn::kMesh));
                    MFnMesh fnInput(objInput);
                    int numVerts = fnInput.numVertices();
                    MFloatPointArray rgOrigVertices;
                    fnInput.getPoints(rgOrigVertices);

                    for (int iVertex = 0; iVertex < numVerts; iVertex++) 
                    {
                        float x = rgOrigVertices[iVertex][0];
                        float y = rgOrigVertices[iVertex][1];
                        float z = rgOrigVertices[iVertex][2];

                        DtShapeSetVertex( iShapeForThisJoint, iVertex, x, y, 
                            z);
                    }
                }

                NiFree(szOrigName);
            }

            MPlug   plugMessage = cluster.findPlug("message");

            MPlugArray connections;
            plugMessage.connectedTo(connections,false,true);

            MObject setHandle;

            if (connections.length() ==1 )
            {
                plugMessage = connections[0];
                setHandle = plugMessage.node();
                const char *setHandleName = objectName( setHandle );
            }

            if (setHandle.hasFn(MFn::kSet))
            {
                MFnSet setNode(setHandle, &status);
                NIASSERT(status == MS::kSuccess);

                MSelectionList members;
                MStringArray selectionStrings;

                status = setNode.getMembers(members,true);
                NIASSERT(status == MS::kSuccess);

                MItSelectionList iter(members, MFn::kComponent, &status);
                NIASSERT(status == MS::kSuccess);

                MDagPath dagPath;
                MObject component;

                for (; !iter.isDone(); iter.next())
                {

                    status = iter.getDagPath(dagPath, component);
                    NIASSERT(status == MS::kSuccess);

                    const char *dagname = 
                        dagPath.fullPathName(&status).asChar();

                    NIASSERT(status == MS::kSuccess);

                    const char *cpcDagNodeName = objectName( dagPath.node());

                    int iShape = DtExt_FindShapeWithNode(dagPath.node());
                    if (iShape == -1)
                        continue;

//                    int iShape = DtExt_FindShapeWithDagpath( dagPath );

                    // If this shape/joint combo has already been processed,
                    // continue...
// kMDtJointInfo* pTestJoint = DtShapeJointGet(int index,iShape);

                    int numverts;
                    DtShapeGetVertexCount( iShape, &numverts );

                    if (dagPath.node() != outputGeom)
                        continue;

                    const char *apiName = component.apiTypeStr ();
                    switch (component.apiType())
                    {
                        case MFn::kMeshVertComponent:
                        {
// FIND THE JOINT INDEX...
// Store it within the shape along with the vertex weights
                            MPlug inClusterPlug = cluster.findPlug("matrix", 
                                &status);

                            NIASSERT(status==MS::kSuccess);
                            MPlugArray tempArr;
                            MObject jointObj;

                            //so we want to use the inClusterPlug as 
                            // destination and want to find the source
                            if (inClusterPlug.connectedTo(tempArr, true, 
                                false, &status) == false)
                            {
                            // Joint must have been detached in some way...
                            // Basically it is in the DG - but is useless...
                            // that is my thinking anyway.
                            //NIASSERT(0);
                                DtExt_Err("Warning: Found useless cluster..."
                                    " %s\n",pName);
                                continue;
                            }

                            int plg = 0;
                            MPlug outJointPlug = tempArr[plg];
                            jointObj = outJointPlug.node();
                            int iJointIndex = 
                                gJointXformMan.GetJoint( jointObj );

                            MItMeshVertex vertIter(dagPath, component);
                            for(;!vertIter.isDone();vertIter.next())
                            {
                                MObject mvert = vertIter.vertex();
                                long vertIndex = vertIter.index();

                                MPlug weight = plugWeights.
                                    elementByLogicalIndex(vertIndex,&status);

                                AssertGood(status);
                                float value;
                                status = weight.getValue(value);

                                float* pfWeights = NiAlloc(float, 1);
                                pfWeights[0] = value;
                            
                                int* piJointIndices = NiAlloc(int, 2);
                                *piJointIndices = iJointIndex;
    
                                // length will always be 1 for rigid...
                                DtShapeStoreWeights(iShape,vertIndex, 1, 
                                    pfWeights, piJointIndices);
                            }
                            break;
                        }
                        default:
                            break;
                    }


                    // Find the corresponding joint node...
                    MPlug inClusterPlug = 
                        cluster.findPlug("matrix", &status);

                    NIASSERT(status==MS::kSuccess);
                    MPlugArray tempArr;
                    MObject jointObj;

                    //so we want to use the inClusterPlug as destination 
                    // and want to find the source
                    if (inClusterPlug.connectedTo(tempArr, true, false, 
                            &status) == false)
                    {
                        // Joint must have been detached in some way...
                        // Basically it is in the DG - but is useless...
                        // that is my thinking anyway.
                        //NIASSERT(0);
                        DtExt_Err("ERROR: Found useless cluster... %s\n",
                            pName);
                        continue;
                    }

                    MPlug outJointPlug = tempArr[0];
                    jointObj = outJointPlug.node();
                    const char *pJointName = objectName( jointObj );

                    // NEW SKINNING CODE
                    MObject objTransform;
                    DtExt_ShapeGetTransform(iShape,objTransform);
                    MDagPath    pathTransform;
                    MFnDagNode(objTransform).getPath(pathTransform);
                    MMatrix matMeshWorldTransform = 
                        pathTransform.inclusiveMatrix();

                    MFnIkJoint fnBone(jointObj);
                    MObject objBindPose;
                    fnBone.findPlug("bindPose").getValue(objBindPose);
                    MFnMatrixData fnBindPose(objBindPose);

                    NIASSERT(status==MS::kSuccess);

                    // Should already exist - so find it...
                    int iJointIndex = gJointXformMan.GetJoint( jointObj );
                    if (iJointIndex == -1)
                    {
                        DtExt_Err("ERROR: Found cluster with unreferenced "\
                            "joint. [%s]\n", pName);
                    }

                    kMDtJointInfo *pJointInfo = NiExternalNew kMDtJointInfo;
                    pJointInfo->m_iJointIndex  = iJointIndex;

                    (matMeshWorldTransform * fnBindPose.matrix().inverse()).
                        get(pJointInfo->m_fSkinToBoneMat);

                    DtShapeAddJoint( iShape, pJointInfo );

                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void kDtIK::
IKNew(void)
{
    // First we will gather all the Joint Info
    // There are essentially two types of skinning:
    // smooth and rigid.
    // 
    // Currently, we are supporting Rigid.
    GetRigidSkinningInfo();
    GetSmoothSkinningInfo();
}
//---------------------------------------------------------------------------
int addTransformJoint( MDagPath &transformPath,
    MObject transformNode,
    MObject parentNode )
{
    
    // matrix = [S] * [RO] * [R] * [JO] * [IS] * [T] 
    //
    // [S] : getScale 
    // [RO]: getScaleOrientation 
    // [R] : getRotation 
    // [JO]: getOrientation 
    // [IS]: (the inverse of the getScale on the parent transformation matrix) 
    // [T] : translation

    // We know we are working with a Transform,Joint pair...

    // For debugging purposes... lets get the name.
    const char *pXformName = objectName( transformNode );
    const char *pParentName= objectName( parentNode );

    // Ensure the Node we are looking at is a joint node.
    if (!transformNode.hasFn(MFn::kJoint))
        return -1;

    // Ensure the joint isn't already stored...
    int index = gJointXformMan.GetJoint( transformNode );
    if (index != -1)
        return -1;

    kMDtJointXform *pJointXform = NiExternalNew kMDtJointXform;
    pJointXform->m_MObjectJoint = transformNode;
    pJointXform->m_MObjectParent = parentNode;
    int iJointXform = gJointXformMan.Add( pJointXform );

    // Alternatively, we could construct it by hand...
    // JointMatrix = S * RO * R * JO * IS * T

    return iJointXform;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// C WRAPPERS
//---------------------------------------------------------------------------

void gIKDataNew(void)
{

    gIKData.IKNew();

}


// OLD ARCHIVE CODE


// Tweak code may be necessary for later...
// below (in comments) is rudimentary code for reading tweak values
//            // check for tweaks
//            MObject objShape;
//            DtExt_ShapeGetShapeNode(iShape, objShape);
//            MFnMesh   fnOutput(objShape);
//            MPlug plgTweakLoc = fnOutput.findPlug("tweakLocation");
//            MObject   objTweakLocVal;
//            plgTweakLoc.getValue(objTweakLocVal);
//            if (!objTweakLocVal.isNull()) // tweak found
//            { 
//               MPlugArray rgplgTweakLocConnections;
//plgTweakLoc.connectedTo(rgplgTweakLocConnections, true, false);     
// get source plugs
//                  NIASSERT(rgplgTweakLocConnections.length() == 1);
//                  MObject objTweak = rgplgTweakLocConnections[0].node();
//                  NIASSERT(objTweak.hasFn(MFn::kTweak));
//                  MFnGeometryFilter   fnTweak(objTweak);
//                  bool    bRelativeTweak;
//                fnTweak.findPlug("relativeTweak").getValue(bRelativeTweak);
//                  MPlug plgOffsets = fnTweak.findPlug("vlist")[0].child(0);
//                  float   fEnvelope   = fnTweak.envelope();
//                  for (int iVertex = 0; iVertex < 6; iVertex++) 
//                  {
//                     float x,y,z;
//      plgOffsets.elementByLogicalIndex(iVertex).child(0).getValue(x);
//      plgOffsets.elementByLogicalIndex(iVertex).child(1).getValue(y);
//     plgOffsets.elementByLogicalIndex(iVertex).child(2).getValue(z);
//
//                      x   *= fEnvelope;
//                      y   *= fEnvelope;
//                      z   *= fEnvelope;
//                }
//            }


// 12-11-00
// What we would like to do is...
// We want to store any bindPose information that is available
// or at least a reference to the plug with the information.
/*
                // REVISIT THIS CODE....

  MFnDependencyNode jointDepend(jointObj);
MPlug plugJointBindPose = jointDepend.findPlug("bindPose",&status);
MObject objJointBindPose = plugJointBindPose.node();               //nn 
const char *objJointBindPoseName = objectName( objJointBindPose ); //nn
const char *objJointBindPoseAPI  = objJointBindPose.apiTypeStr();  //nn
AssertGood(status);

                MPlugArray connections;
bool bResult = plugJointBindPose.connectedTo ( connections , false, true, 
    &status );
                AssertGood(status);

                if (connections.length() ==1 )
                {
                    float dest[4][4];
                    memset(dest,0,4*4*sizeof(float));

                     // = bindPose.worldMatrix[x]
                    MPlug   plugBindPoseWM = connections[0];
                    MObject objVal;
                    status = plugBindPoseWM.getValue( objVal );
                    const char *objValAPI  = objVal.apiTypeStr();  //nn
                    if (objVal.hasFn(MFn::kMatrixData))
                    {
                        MMatrix mMat;
                        MFnMatrixData mMatData( objVal );
                        mMat = mMatData.matrix ( &status );
                        status = mMat.get ( dest );
                    }

                    MObject objBindPoseWM = plugBindPoseWM.node();
                    const char *objBindPoseWMName = 
                        objectName( objBindPoseWM );
                    const char *APIName = objBindPoseWM.apiTypeStr();

                    if (objBindPoseWM.hasFn(MFn::kDagPose))
                    {
                        MFnDependencyNode jointDepend2(objBindPoseWM);
                        MPlug plugWM = 
                            jointDepend2.findPlug("xformMatrix",&status);
                        AssertGood(status);
                        MObject objval2;
                        status = plugWM.getValue( objval2 );
                        AssertGood(status);
                       const char *objVal2API  = objval2.apiTypeStr();  //nn

                        MObject setHandle2 = plugWM.node();
                      const char *setHandle2Name = objectName( setHandle2 );
                        const char *xfMAPI  = setHandle2.apiTypeStr();  //nn

                        const char *BPWMName = objectName( plugWM );
                        AssertGood(status);
                    }
                }*/

/*
                            case MFn::kSurfaceCVComponent:
                            {
                                MItSurfaceCV cvIter(dagPath, component);
                                for(;!cvIter.isDone();cvIter.next())
                                {
                                    MSelectionList tmp;
                                    tmp.add(dagPath,cvIter.cv());
                                  tmp.getSelectionStrings(selectionStrings);
                                    
MPlug weight = plugWeights.elementByLogicalIndex(cvIter.index(),&status);
                                    
                                    if (status != MS::kSuccess)
                                    {
                                        ;
                                    }

                                    float value;
                                    status = weight.getValue(value);
                                }
                                break;
                            }
*/

int MyDtShapeGetVertices
    (
        MObject&    objInput, 
        MObject&    objOutput, 
        int*        pcVertices, 
        DtVec3f**   prgVertices
    ) 
{
    *pcVertices     = 0;
    *prgVertices    = NULL;

    
    NIASSERT(objInput.hasFn(MFn::kMesh) && objOutput.hasFn(MFn::kMesh));


    MFnMesh fnOutput(objOutput);
    MFnMesh fnInput(objInput);

    *pcVertices     = fnOutput.numVertices();
    *prgVertices    = NiExternalNew DtVec3f[*pcVertices];

    if (!*prgVertices) 
    {
        *pcVertices = 0;

        return 0;
    }

    // check for tweaks
    MPlug   plgTweakLoc = fnOutput.findPlug("tweakLocation");

    MObject objTweakLocVal;

    plgTweakLoc.getValue(objTweakLocVal);

    if (!objTweakLocVal.isNull())   // tweak found
    {   
        MPlugArray  rgplgTweakLocConnections;

         // get source plugs
        plgTweakLoc.connectedTo(rgplgTweakLocConnections, true, false);    

        NIASSERT(rgplgTweakLocConnections.length() == 1);

        MObject objTweak = rgplgTweakLocConnections[0].node();

        NIASSERT(objTweak.hasFn(MFn::kTweak));

        MFnGeometryFilter   fnTweak(objTweak);

        bool    bRelativeTweak;

        fnTweak.findPlug("relativeTweak").getValue(bRelativeTweak);

        if (!bRelativeTweak) 
            DtExt_Err("WARNING: Encountered an absolute tweak; treating as "
                "relative!\n");

        MPlug plgOffsets = fnTweak.findPlug("vlist")[0].child(0);


//  WARNING: Seems like Maya doesn't initialize it's numElements properly!!
//      NIASSERT((int)plgOffsets.numElements() == cVertices);
//      if ((int)plgOffsets.numElements() != *pcVertices)
//   cout << "\t\tWARNING: tweak count doesn't match vertex count!" << endl;

        float   fEnvelope   = fnTweak.envelope();

        for (int iVertex = 0; iVertex < *pcVertices; iVertex++) 
        {
            plgOffsets.elementByLogicalIndex(iVertex).child(0).getValue(
                (*prgVertices)[iVertex].vec[0]);

            plgOffsets.elementByLogicalIndex(iVertex).child(1).getValue(
                (*prgVertices)[iVertex].vec[1]);

            plgOffsets.elementByLogicalIndex(iVertex).child(2).getValue(
                (*prgVertices)[iVertex].vec[2]);

            (*prgVertices)[iVertex].vec[0]  *= fEnvelope;
            (*prgVertices)[iVertex].vec[1]  *= fEnvelope;
            (*prgVertices)[iVertex].vec[2]  *= fEnvelope;
        }
    }
    else 
    {
        for (int iVertex = 0; iVertex < *pcVertices; iVertex++) 
        {
            (*prgVertices)[iVertex].vec[0]  = 0.0f;
            (*prgVertices)[iVertex].vec[1]  = 0.0f;
            (*prgVertices)[iVertex].vec[2]  = 0.0f;
        }
    }

    // load vertices and add them to the tweaks
    int ifnVerts = fnInput.numVertices();
    NIASSERT(*pcVertices == ifnVerts);

    MFloatPointArray rgOrigVertices;

    fnInput.getPoints(rgOrigVertices);

    NIASSERT(*pcVertices == (int)rgOrigVertices.length());

    for (int iVertex = 0; iVertex < *pcVertices; iVertex++) 
    {
        (*prgVertices)[iVertex].vec[0] += rgOrigVertices[iVertex][0];
        (*prgVertices)[iVertex].vec[1] += rgOrigVertices[iVertex][1];
        (*prgVertices)[iVertex].vec[2] += rgOrigVertices[iVertex][2];
    }

    return 1;
}