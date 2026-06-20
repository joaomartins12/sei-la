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

#ifndef NIMAXMESHCONVERTER_H
#define NIMAXMESHCONVERTER_H


#include "NiMAX.h"
#include "NiMAXConverter.h"
#include "NiMAXUV.h"
#include "NiTHash.h"
#include "NiMAXGeomConverter.h"
#include <NiOptVertex.h>

#include <NiOptTriShape.h>
#if MAX_RELEASE >= 5000
    #define USE_RADIOSITY_MESH
    #if MAX_RELEASE < 6000
        #include "IRadiosityMesh\IRadiosityMesh.h"
    #else 
        #include <radiosityMesh.h>    
    #endif
    #define USE_EDIT_NORMALS_MODIFIER
#endif
//---------------------------------------------------------------------------

class NiMAXMeshConverter : public NiMAXGeomConverter
{
public:
    NiMAXMeshConverter(TimeValue animStart, TimeValue animEnd);
    ~NiMAXMeshConverter() {};

    static void Preprocess(INode* pNode);
    static void Postprocess(INode* pNode);

    int ConvertGeomSub(Object *pObj,
                    INode *pMaxNode, 
                    NiNode *pNode,
                    NiAVObject **ppGeom, 
                    bool bIgnoreHash,
                    bool bIgnoreMtls, 
                    ScaleInfo *pParentScale,
                    bool bDoVertexRemapping,
                    unsigned short**& ppVertexRemapping,
                    NiAVObject**& ppRemapTargets,
                    unsigned int& uiNumRemappings,
                    NiMAXUV*& pkNiMAXUV);

    void GetVerts(Object *pObj,
                  INode *pMaxNode,
                  ScaleInfo *pParentScale,
                  unsigned int &uiNumVerts,
                  NiPoint3 *&pVerts);        

    class ISkinWrapper
    {
        public:
            ISkinWrapper(Modifier* pkMod);
            ISkin* m_pkSkinMod;
            bool m_bUseRigidVertices;

    };

    static int ConvertMesh(Mesh *pkMesh,
                           MNMesh* pkMNMesh,
                           NiTriShape **ppTris, 
                           const int iAttrId, 
                           const int iMaxAttr,
                           NiMAXUV* pkNiMAXUV,
                           NiMatrix3 *pMat,
                           bool bIsTwoSided,
                           ScaleInfo *pParentScale,
                           NtPointTracker *pTracker,
                           unsigned short **ppRemapping,
                           ISkinWrapper* pkSkinWrapper, 
                           INode* pkMaxNode);
                                        
    static void Init();
    static void Shutdown();

    static void SetExportVertWeights(bool bSaveVertWeights);
    static bool GetExportVertWeights();

    static float ApplyScaleInfo(ScaleInfo *pParentScale, 
                               NiPoint3 *pVertices, 
                               int iNumVerts);

    static float ApplyScaleInfo(ScaleInfo *pParentScale, 
                               NiOptVertex *pVertices,
                               int iNumVerts, 
                               unsigned int *pIndices, 
                               int iNumFaces, bool bForceDown);

private:

#ifdef USE_EDIT_NORMALS_MODIFIER
    static bool MapMNMeshNormalsToMeshNormals(Mesh* pkMesh, int iFaceID, 
                                MNMesh* pkMNMesh, MNNormalSpec* pkNormalSpec,
                                Point3& norm0, Point3& norm1, Point3& norm2);
#endif
    static bool FaceUsesOtherMaterial(Mesh* pMesh, int iFace,
        const int iMaxAttr, const int iAttrId);
    static void ConvertUVs(NiPoint2 &UV, float* pUVs,
        int iMapIndex, NiMAXUV* pkNiMAXUV);    
    static NiStencilPropertyPtr ms_spStencilTwoSidedProperty;

#ifdef _DEBUG
    static bool HasSimpleUVs(Mesh *pMesh);
    static bool HasSimpleVertexColors(Mesh *pMesh);
    static bool HasSimpleNormals(Mesh *pMesh);
    static bool AllOneMaterial(Mesh *pMesh, 
                               const int iAttrId, 
                               const int iMaxAttr);
#endif

    static bool ComputePrecacheFlags(NiTriShape* pTris, Animatable * pObj);
    static bool ComputeHierPrecacheFlags(NiTriShape* pTris, INode* pkMaxNode);
    static bool ComputeObjectPrecacheFlags(NiTriShape* pTris, Object* pObj);

    TimeValue m_animStart, m_animEnd;

    static bool ProcessInstance(NiAVObject* pkOrig, NiAVObject* pkInst, 
        NiNode* pkInstParent, NiPoint3 kPos, NiMatrix3 kRot, ScaleInfo kScale);
    // the ObjHTab has to be AVObjects since multimaterials will
    // result in a NiNode and a bunch of Shape children
    static NiTHash<NiAVObjectPtr>* ms_pkMeshHTab;

    static bool ms_bExportVertWeights;
    static bool ms_bConvertNormals;

    static void *ms_pUserData;
};

//---------------------------------------------------------------------------
//  Inline include
#include "NiMAXMeshConverter.inl"

//---------------------------------------------------------------------------

#endif

