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

#include "MaxImmerse.h"
#include "NiUtils.h"
#include "NiMAXMeshConverter.h"
#include "NiMAXMaterialConverter.h"
#include "NiMAXHierConverter.h"
#include "NiMAXMorphConverter.h"
#include "NiMAXCustAttribConverter.h"
#include "NiVertWeightsExtraData.h"

bool NiMAXMeshConverter::ms_bExportVertWeights = false;
bool NiMAXMeshConverter::ms_bConvertNormals = true;
NiStencilPropertyPtr NiMAXMeshConverter::ms_spStencilTwoSidedProperty = NULL;

const char* gpcMultiMtlName = "__NDL_MultiMtl_Node";

static bool PFMeshHashEq(const void *pSearch, const void *p, 
    const void *p2Search, const void *p2);
static void PFMeshHashFree(const void *p2, NiAVObjectPtr &spObject);

//---------------------------------------------------------------------------
NiTHash<NiAVObjectPtr>* NiMAXMeshConverter::ms_pkMeshHTab = NULL;

void *NiMAXMeshConverter::ms_pUserData = NULL;

//---------------------------------------------------------------------------
bool PFMeshHashEq(const void *pSearch, const void *p, const void *p2Search, 
    const void *p2)
{
    if (pSearch != p)
        return(false);

    ScaleInfo *pScaleSearch = (ScaleInfo *) p2Search;
    ScaleInfo *pScale = (ScaleInfo *) p2;

    if (pScaleSearch != NULL)
    {
        if (pScale->bIsIdent && pScaleSearch->bIsIdent)
            return(true);

        if (NiOptimize::CloseTo(pScaleSearch->scale.s.x, pScale->scale.s.x) &&
            NiOptimize::CloseTo(pScaleSearch->scale.s.y, pScale->scale.s.y) &&
            NiOptimize::CloseTo(pScaleSearch->scale.s.z, pScale->scale.s.z) &&
            NiOptimize::CloseTo(pScaleSearch->scale.q.x, pScale->scale.q.x) &&
            NiOptimize::CloseTo(pScaleSearch->scale.q.y, pScale->scale.q.y) &&
            NiOptimize::CloseTo(pScaleSearch->scale.q.z, pScale->scale.q.z) &&
            NiOptimize::CloseTo(pScaleSearch->scale.q.w, pScale->scale.q.w))
            return(true);

        // if the scales are isotropic the rots don't matter
        if (!pScaleSearch->IsAnisotropic() && !pScale->IsAnisotropic())
            return(true);
    }
    else
        return(true);

    return(false);
}
//---------------------------------------------------------------------------
void 
PFMeshHashFree(const void *p2, NiAVObjectPtr &spObject)
{
    spObject = NULL;
    NiDelete ((ScaleInfo *) p2);
}

//---------------------------------------------------------------------------
NiMAXMeshConverter::NiMAXMeshConverter(TimeValue animStart, 
    TimeValue animEnd)
{
    m_animStart = animStart;
    m_animEnd = animEnd;
}

//---------------------------------------------------------------------------
void NiMAXMeshConverter::Preprocess(INode* pNode)
{
    CHECK_MEMORY();
    // Clear node flags

    SetFlags(pNode, ND_CLEAR);

    for (int i = 0; i < pNode->NumberOfChildren(); i++)
        Preprocess(pNode->GetChildNode(i));
        
    ObjectState kOState = pNode->EvalWorldState(0);
    Object* pkEvalObj = kOState.obj;
    if (!pkEvalObj) 
        return;

    if (pkEvalObj->SuperClassID() == GEOMOBJECT_CLASS_ID)
        SetFlags(pkEvalObj, ND_CLEAR);

    Object* pkObjectRef = pNode->GetObjectRef();
    // Disable Skin Modifier so that we can get the skin in the
    // initial pose
    Modifier* pkMod = FindModifier(pkObjectRef, SKIN_CLASSID);

    if (pkMod)
        pkMod->DisableMod();


#ifndef NO_CSTUDIO
    // Disable Physique Modifier so that we can get the skin in the
    // initial pose
    pkMod = FindModifier(pkObjectRef,
        Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));

    if (pkMod)
        pkMod->DisableMod();
#endif

    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void NiMAXMeshConverter::Postprocess(INode* pNode)
{
    CHECK_MEMORY();
    // Re-enable Skin Modifier 
    Object* pkObjectRef = pNode->GetObjectRef();
    Modifier* pkMod = FindModifier(pkObjectRef, SKIN_CLASSID);
    if (pkMod)
        pkMod->EnableMod();

#ifndef NO_CSTUDIO
    // Re-enable Physique Modifier 
    pkMod = FindModifier(pkObjectRef,
        Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));

    if (pkMod)
        pkMod->EnableMod();
#endif

    for (int i = 0; i < pNode->NumberOfChildren(); i++)
        Postprocess(pNode->GetChildNode(i));
    CHECK_MEMORY();

}
//---------------------------------------------------------------------------
static Point3 
NfGetVertexNormal(Mesh* pkMesh, int iFaceNo, RVertex* pRV)
{
    CHECK_MEMORY();
    Face* pFace = &pkMesh->faces[iFaceNo];
    DWORD smGroup = pFace->smGroup;
    int iNumNormals;
    Point3 vertexNormal;
    
    // Is normal specified
    // SPECIFIED is not currently used, but may be used in future versions.
    if (pRV->rFlags & SPECIFIED_NORMAL) 
        vertexNormal = pRV->rn.getNormal();

    // If normal is not specified it's only available if the face belongs
    // to a smoothing group
    else if ((iNumNormals = pRV->rFlags & NORCT_MASK) && smGroup) 
    {
        // If there is only one vertex is found in the rn member.
        if (iNumNormals == 1) 
            vertexNormal = pRV->rn.getNormal();
        else
        {
            // If two or more vertices are there you need to step 
            // through them and find the vertex with the same 
            // smoothing group as the current face.
            // You will find multiple normals in the ern member.
            for (int i = 0; i < iNumNormals; i++) 
            {
                vertexNormal = pRV->ern[i].getNormal();
                if (pRV->ern[i].getSmGroup() & smGroup)
                    break;
            }
        }
    }
    else {
        // Get the normal from the Face if no smoothing groups are there
        vertexNormal = pkMesh->getFaceNormal(iFaceNo);
    }
    CHECK_MEMORY();
    
    return vertexNormal;
}

#ifdef _DEBUG
//---------------------------------------------------------------------------
// Does "pkMesh" have one normal per vertex?
bool NiMAXMeshConverter::HasSimpleNormals(Mesh *pkMesh)
{
    CHECK_MEMORY();
    bool bSimple;
    int i, j;

    pkMesh->buildNormals();
        
    bSimple = true;
    for (i = 0; i < pkMesh->getNumFaces() && bSimple; i++)
    {
        DWORD smGroup = pkMesh->faces[i].smGroup;

        if (!smGroup)
        {
            bSimple = false;
            break;
        }

        for (j = 0; j < 3; j++)
        {
            RVertex *pRVert = pkMesh->getRVertPtr(pkMesh->faces[i].v[j]);
                
            if ((pRVert->rFlags & NORCT_MASK) != 1)
            {        
                bSimple = false;
                break;
            }
        }
    }

    CHECK_MEMORY();
    return(bSimple);
}

//---------------------------------------------------------------------------
// A Mesh has a simple set of Vertex Colors if each vertex only has one
// vertex color (not necessarily the case for MAX) and the face array
// of the vertex colors matches the face array for the base mesh
bool NiMAXMeshConverter::HasSimpleVertexColors(Mesh *pkMesh)
{
    CHECK_MEMORY();
    int i, iNumMeshVerts, iNumCVerts;
    
    iNumMeshVerts = pkMesh->getNumVerts();

    iNumCVerts = pkMesh->numCVerts;
    if (iNumCVerts)
    {
        for (i = 0; i < pkMesh->getNumFaces(); i++)
        {
            if (pkMesh->faces[i].v[0] != pkMesh->vcFace[i].t[0] ||
                pkMesh->faces[i].v[1] != pkMesh->vcFace[i].t[1] ||
                pkMesh->faces[i].v[2] != pkMesh->vcFace[i].t[2])
                return(false);
        }
    }
    CHECK_MEMORY();

    return(true);
}

//---------------------------------------------------------------------------
// A Mesh has a simple set of UVs if each vertex has only one UV and the
// face arrays of all the UV maps match the face array of the base mesh
bool NiMAXMeshConverter::HasSimpleUVs(Mesh *pkMesh)
{
    int i, iNumMeshVerts, iNumTVerts;

     CHECK_MEMORY();
   iNumMeshVerts = pkMesh->getNumVerts();

    // first examine the primary set of texture coordinates
    iNumTVerts = pkMesh->getNumTVerts();
    if (iNumTVerts)
    {
        if (iNumTVerts != iNumMeshVerts)
            return(false);

        if (iNumTVerts)
        {
            for (i = 0; i < pkMesh->getNumFaces(); i++)
            {
                if (pkMesh->faces[i].v[0] != pkMesh->tvFace[i].t[0] ||
                    pkMesh->faces[i].v[1] != pkMesh->tvFace[i].t[1] ||
                    pkMesh->faces[i].v[2] != pkMesh->tvFace[i].t[2])
                    return(false);
            }
        }
    }

    // then examine the extra UV channels
    for (int iMapIndex = 2; iMapIndex < MAX_MESHMAPS-1; iMapIndex++)
    {
        if (pkMesh->mapSupport(iMapIndex))
        {
            int iNumMapVerts = pkMesh->getNumMapVerts(iMapIndex);

            if (iNumMapVerts != iNumMeshVerts)
                return(false);

            if (iNumMapVerts)
            {
                for (i = 0; i < pkMesh->getNumFaces(); i++)
                {
                    if (pkMesh->faces[i].v[0] != 
                                pkMesh->mapFaces(iMapIndex)[i].t[0] ||
                        pkMesh->faces[i].v[1] != 
                                pkMesh->mapFaces(iMapIndex)[i].t[1] ||
                        pkMesh->faces[i].v[2] != 
                                pkMesh->mapFaces(iMapIndex)[i].t[2])
                        return(false);
                }
            }
        }
    }

    CHECK_MEMORY();
    return(true);
}

//---------------------------------------------------------------------------
bool NiMAXMeshConverter::AllOneMaterial(Mesh *pkMesh, const int iAttrId, 
    const int iMaxAttr)
{
    MtlID mID;
    int i;

    for (i = 0; i < pkMesh->getNumFaces(); i++)
    {
        mID = pkMesh->getFaceMtlIndex(i);
        mID %= iMaxAttr;

        if (mID != iAttrId)
            return(false);
    }

    return(true);
}
#endif

//---------------------------------------------------------------------------
// Apply the effect of negative or anisotropic scaling to a mesh
float NiMAXMeshConverter::ApplyScaleInfo(ScaleInfo *pParentScale, 
    NiOptVertex *pVertices, int iNumVerts, unsigned int *pIndices, 
    int iNumFaces, bool bForceDown)
{
    CHECK_MEMORY();
    if (pParentScale == NULL)
        return 1.0f;

    int iFlips = 0;

    ScaleValue localScale = pParentScale->scale;
    
    if (localScale.s.x < 0.0f)
        iFlips++;
    
    if (localScale.s.y < 0.0f)
        iFlips++;

    if (localScale.s.z < 0.0f)
        iFlips++;

    int i;
    if (iFlips % 2)
    {
        for (i = 0; i < iNumFaces; i++)
        {
            unsigned short usTmp;

            usTmp = pIndices[i*3];
            pIndices[i*3] = pIndices[i*3+2];
            pIndices[i*3+2] = usTmp;
        }

        for (i = 0; i < iNumVerts; i++)
            pVertices[i].m_kNormal = -pVertices[i].m_kNormal;
    }

    if (bForceDown || pParentScale->IsAnisotropic())
    {
        Matrix3 srtm, stm, srtminv, tm;
        Point3 p1, p2;
        Quat qinv;        

        stm = ScaleMatrix(pParentScale->scale.s);
        pParentScale->scale.q.MakeMatrix(srtm);
        qinv = Inverse(pParentScale->scale.q);
        qinv.MakeMatrix(srtminv);
        tm = srtm * stm * srtminv;
        
        for (i = 0; i < iNumVerts; i++)
        {
            NiPoint3& pos = pVertices[i].m_kPosition;
            p1.x = pos.x;
            p1.y = pos.y;
            p1.z = pos.z;
            
            p2 = p1 * tm;
            
            pos.x = p2.x;
            pos.y = p2.y;
            pos.z = p2.z;
        }
        return 1.0f;
    }

     CHECK_MEMORY();
   // return uniform scale
    return pParentScale->scale.s.x;
}
//---------------------------------------------------------------------------
// Apply the effect of negative or anisotropic scaling to raw vertices
float NiMAXMeshConverter::ApplyScaleInfo(ScaleInfo *pParentScale, 
    NiPoint3 *pVertices, int iNumVerts)
{
    CHECK_MEMORY();
    int i;

    if (pParentScale == NULL)
        return 1.0f;

    if (pParentScale->IsAnisotropic())
    {
        Matrix3 srtm, stm, srtminv, tm;
        Point3 p1, p2;
        Quat qinv;        

        stm = ScaleMatrix(pParentScale->scale.s);
        pParentScale->scale.q.MakeMatrix(srtm);
        qinv = Inverse(pParentScale->scale.q);
        qinv.MakeMatrix(srtminv);
        tm = srtm * stm * srtminv;
        
        for (i = 0; i < iNumVerts; i++)
        {
            NiPoint3& vert = pVertices[i];
            p1.x = vert.x;
            p1.y = vert.y;
            p1.z = vert.z;
            
            p2 = p1 * tm;
            
            vert.x = p2.x;
            vert.y = p2.y;
            vert.z = p2.z;
        }
        return 1.0f;
    }
   CHECK_MEMORY();
   return pParentScale->scale.s.x;
}

#ifdef USE_EDIT_NORMALS_MODIFIER
//---------------------------------------------------------------------------
bool NiMAXMeshConverter::MapMNMeshNormalsToMeshNormals(Mesh* pkMesh, 
    int iFaceID, MNMesh* pkMNMesh, MNNormalSpec* pkNormalSpec,
    Point3& norm0, Point3& norm1, Point3& norm2)
{
    if (pkMesh == NULL || pkMNMesh == NULL || pkNormalSpec == NULL)
        return false;

    Point3 kMesh0 = pkMesh->verts[pkMesh->faces[iFaceID].v[0]];
    Point3 kMesh1 = pkMesh->verts[pkMesh->faces[iFaceID].v[1]];
    Point3 kMesh2 = pkMesh->verts[pkMesh->faces[iFaceID].v[2]];

    int iNumFaces = pkMNMesh->FNum();
    int iIndex0 = -1;
    int iIndex1 = -1;
    int iIndex2 = -1;
    int iVertIndex0 = -1;
    int iVertIndex1 = -1;
    int iVertIndex2 = -1;
    int iFaceIndex = -1;
    bool bFound = false;

    for (int i = 0; i < iNumFaces; i++)
    {
        MNFace * pkFace = pkMNMesh->F(i);
        iIndex0 = -1;
        iIndex1 = -1;
        iIndex2 = -1;

        if (pkFace != NULL)
        {
            for (int j = 0; j < pkFace->deg; j++)
            {
               int iVertIndex = pkFace->vtx[j];
               MNVert kVert = pkMNMesh->v[iVertIndex];
               if (kVert.p == kMesh0)
               {
                   iIndex0 = j;
                   iVertIndex0 = iVertIndex;
               }
               else if (kVert.p == kMesh1)
               {
                   iIndex1 = j;
                   iVertIndex1 = iVertIndex;
               }
               else if (kVert.p == kMesh2)
               {
                   iIndex2 = j;
                   iVertIndex2 = iVertIndex;
               }
            }
        }

        if (iIndex0 != -1 && iIndex1 != -1 && iIndex2 != -1)
        {
            iFaceIndex = i;
            bFound = true;
            break;
        }
    }

    Point3* pNormals = pkNormalSpec->GetNormalArray();
    if (bFound && pNormals != NULL)
    {       
        int iNormIndex0 = pkNormalSpec->GetNormalIndex (iFaceIndex, iIndex0);
        int iNormIndex1 = pkNormalSpec->GetNormalIndex (iFaceIndex, iIndex1);
        int iNormIndex2 = pkNormalSpec->GetNormalIndex (iFaceIndex, iIndex2);
        bool bNormSpec0 = pkNormalSpec->GetNormalExplicit (iNormIndex0); 
        bool bNormSpec1 = pkNormalSpec->GetNormalExplicit (iNormIndex1); 
        bool bNormSpec2 = pkNormalSpec->GetNormalExplicit (iNormIndex2); 


        if (bNormSpec0)
            norm0 = pkNormalSpec->GetNormal(iFaceIndex, iIndex0);
        if (bNormSpec1)
            norm1 = pkNormalSpec->GetNormal(iFaceIndex, iIndex1);
        if (bNormSpec2)
            norm2 = pkNormalSpec->GetNormal(iFaceIndex, iIndex2);


        return true;
    }

    return false;
}
#endif
//---------------------------------------------------------------------------
// Extract all triangles from "pkMesh" having attribute ID "iAttrID" and
// place them in "ppTris". If "iAttrID" is -1 then ignore attributeID.

// One of the central problems in converting from MAX to NI is that MAX
// represents its meshes differently. MAX has topology (a face set) for
// each set of vertex data (colors, normals, UVs.) This causes problems
// since NI only has one set of topology information that indexes into
// lists of data. To get around this we will split the Mesh
// into its component triangles and then optimize the result to remove
// redundant vertices.

// ppRemapping tells the application where a given NI vertex
// originated in the MAX Mesh

// pTextureSetMapping tells the application where a given MAX UV channel
// has been placed in NI's texture sets

// pTracker is nonnull if there is a skin and bones system

// fTiling indices 0 and 2 are U offset (phase) and tiling (frequency),
//                 1 and 3 are V offset (phase) and tiling (frequency),
//                 respectively.

// usUVOrder index 0 is the MAX U/V/W that maps to NI U
//           index 1 is the MAX U/V/W that maps to NI V

// pMat is an xform to apply to the mesh.  Currently only used by the
// particle system conversion.

int NiMAXMeshConverter::ConvertMesh(Mesh* pkMesh, MNMesh* pkMNMesh,
    NiTriShape** ppTris, const int iAttrId, const int iMaxAttr, 
    NiMAXUV* pkNiMAXUV, NiMatrix3* pMat, bool bIsTwoSided, 
    ScaleInfo* pParentScale, NtPointTracker* pTracker, 
    unsigned short** ppRemapping, ISkinWrapper* pkSkinWrapper, 
    INode* pkMaxNode)
{
    CHECK_MEMORY();
    int i, j;

    *ppTris = NULL;

    // First, count the number of faces we will be converting
    int iNumFaces;
    if (iAttrId == -1)
    {
        // '-1' means ignore all materials so we will be converting all the
        // triangles
        iNumFaces = pkMesh->getNumFaces();
    }
    else
    {
        iNumFaces = 0;
        
        // otherwise we need to loop and count
        for (i = 0; i < pkMesh->getNumFaces(); i++)
        {
            if (!FaceUsesOtherMaterial(pkMesh, i, iMaxAttr, iAttrId))
            {
                iNumFaces++;
            }
        }
    }
    
    if (iNumFaces == 0)
    {
        return(W3D_STAT_OK);
    }
    else if (iNumFaces > USHRT_MAX)
    {
        char acString[1024];
        NiSprintf(acString, 1024, "Mesh \"%s\" has more than %d faces.\n"
            "It will not be exported", pkMaxNode->GetName(), USHRT_MAX);
        NILOGWARNING(acString);
        return(W3D_STAT_OK);
    }
    
    int iNumVerts = 3*iNumFaces;

    unsigned short* pRemapping = NULL;
    bool bDeleteRemapping = false;    
    if (ppRemapping)
    {
        *ppRemapping = NULL;
        if (bIsTwoSided)
            pRemapping = NiAlloc(unsigned short, iNumVerts*2);
        else
            pRemapping = NiAlloc(unsigned short, iNumVerts);
        NIASSERT(pRemapping);
    }

    unsigned int* pIndices = NiAlloc(unsigned int, iNumVerts);
    NIASSERT(pIndices);

    NiOptVertex* pVertices = NiNew NiOptVertex[iNumVerts];
    NIASSERT(pVertices);

    float* pMeshVertWeights = NULL;
    float* pVertWeights = NULL;
    if (ms_bExportVertWeights && 
        (pMeshVertWeights = pkMesh->getVertexWeights()))
    {
        pVertWeights = NiAlloc(float, iNumVerts);
        NIASSERT(pVertWeights);
    }

#ifndef NO_CSTUDIO
    if (pTracker)
    {
        // pTracker is nonnull if there is a skin and bones system
        
        // if this assert fails then need to change code below that
        // uses the former instead of the latter:
        NIASSERT(pkMesh->getNumVerts() == 
            pTracker->pMCExport->GetNumberVertices());

        // Figure out which vertices are actually
        // used in this segment of the trishape
        unsigned char *pUsed;
        pUsed = NiAlloc(unsigned char, pkMesh->getNumVerts());
        NIASSERT(pUsed);

        memset(pUsed, 0, pkMesh->getNumVerts() * sizeof(unsigned char));

        for (i = 0; i < pkMesh->getNumFaces(); i++)
        {
            if(FaceUsesOtherMaterial(pkMesh, i, iMaxAttr, iAttrId))
            {
                // skip any face that doesn't belong in this trishape
                continue;
            }

            // mark each vertex that is used
            for (j = 0; j < 3; j++)
                pUsed[pkMesh->faces[i].v[j]] = 1;
        }

        TrackInit(pTracker, pkMesh->getNumVerts());

        // Loop over all the vertices and extract the 
        // skin and bones information for those we use
        for (i = 0; i < pkMesh->getNumVerts(); i++)
        {
            if (pUsed[i])
                TrackVert(pTracker, i);
        }

        NiFree(pUsed);
    }
#endif

    // We make sure we have a remapping array. 
    if (!pRemapping && (pTracker || pkSkinWrapper->m_pkSkinMod))
    {
        if (bIsTwoSided)
            pRemapping = NiAlloc(unsigned short, iNumVerts*2);
        else
            pRemapping = NiAlloc(unsigned short, iNumVerts);
        NIASSERT(pRemapping);
        bDeleteRemapping = true;
    }
    
    // First convert the vertices. Loop through all the faces.
    int iCurFace;
    for (iCurFace = i = 0; i < pkMesh->getNumFaces(); i++)
    {
        if(FaceUsesOtherMaterial(pkMesh, i, iMaxAttr, iAttrId))
        {
            // skip any face that doesn't belong in this trishape
            continue;
        }
        
        NIASSERT(iCurFace < iNumFaces);
        
        // For each face we duplicate the verticecs
        for (j = 0; j < 3; j++)
        {
            unsigned int uiVert = 3*iCurFace+j;
            unsigned int uiMaxVert = pkMesh->faces[i].v[j];
            // copy the verts from this triangle into the vertex array
            NiPoint3& nivert = pVertices[uiVert].m_kPosition;
            Point3& maxvert = pkMesh->verts[uiMaxVert];
            nivert.x = maxvert.x;
            nivert.y = maxvert.y;
            nivert.z = maxvert.z;

            if (pMeshVertWeights)
            {
                pVertWeights[uiVert] = pMeshVertWeights[uiMaxVert];
            }
            
            // if we have a remapping array store where it came from in MAX
            if (pRemapping)
            {
                pRemapping[uiVert] = uiMaxVert;
            }
        }
        
        iCurFace++;
    }
    
    // The construction of the topology array should really bring home what
    // we are doing. We can construct this simple mapping because
    // we are splitting the Mesh into its individual triangles!
    for (i = 0; i < iNumVerts; i++)
        pIndices[i] = i;
    
    // convert normals if they are desired
    if (ms_bConvertNormals)
    {
        pkMesh->buildNormals();
       
        for (iCurFace = i = 0; i < pkMesh->getNumFaces(); i++)
        {
            if(FaceUsesOtherMaterial(pkMesh, i, iMaxAttr, iAttrId))
            {
                // skip any face that doesn't belong in this trishape
                continue;
            }
        
            NIASSERT(iCurFace < iNumFaces);
        
            for (j = 0; j < 3; j++)
            {
                RVertex *pRVert;
                Point3 norm;
            
                pRVert = pkMesh->getRVertPtr(pkMesh->faces[i].v[j]);
            
                norm = NfGetVertexNormal(pkMesh, i, pRVert);
                NiPoint3& normal = pVertices[3*iCurFace+j].m_kNormal;

                normal.x = norm.x;
                normal.y = norm.y;
                normal.z = norm.z;
            }
        
            iCurFace++;
        }
    }

#ifdef USE_EDIT_NORMALS_MODIFIER
    // If we have the edit normals modifier attached, 
    // we need to do a little different processing for setting the normals
    if (ms_bConvertNormals)
    {
        if(pkMNMesh)
        {
            MNNormalSpec * pkNormalSpec = pkMNMesh->GetSpecifiedNormals();

            if (pkNormalSpec && (pkNormalSpec->GetNumNormals() > 0))
            {
                int iNumNormFaces = pkNormalSpec->GetNumFaces();
                int iNumNormals = pkNormalSpec->GetNumNormals(); 

                for (iCurFace = i = 0; i < pkMesh->getNumFaces(); i++)
                {
                    if(FaceUsesOtherMaterial(pkMesh, i, iMaxAttr, iAttrId))
                    {
                        // skip any face that doesn't belong in this trishape
                        continue;
                    }
        
                    NIASSERT(iCurFace < iNumFaces);
        
                    NiPoint3& normal0 = pVertices[3*iCurFace+0].m_kNormal;
                    NiPoint3& normal1 = pVertices[3*iCurFace+1].m_kNormal;
                    NiPoint3& normal2 = pVertices[3*iCurFace+2].m_kNormal;

                    Point3 norm0 = Point3(normal0.x, normal0.y, normal0.z);
                    Point3 norm1 = Point3(normal1.x, normal1.y, normal1.z);
                    Point3 norm2 = Point3(normal2.x, normal2.y, normal2.z);
        
                    bool bSuccess = MapMNMeshNormalsToMeshNormals(pkMesh,
                        i, pkMNMesh, pkNormalSpec, norm0, norm1, norm2);
       
                    if (bSuccess)
                    {
                        normal0.x = norm0.x;
                        normal0.y = norm0.y;
                        normal0.z = norm0.z;

                        normal1.x = norm1.x;
                        normal1.y = norm1.y;
                        normal1.z = norm1.z;

                        normal2.x = norm2.x;
                        normal2.y = norm2.y;
                        normal2.z = norm2.z;
                    }
        
                    iCurFace++;
                }
            }
        }
        else
        {
            MeshNormalSpec* pkNormalSpec = pkMesh->GetSpecifiedNormals();
        
            if (pkNormalSpec && (pkNormalSpec->GetNumNormals() > 0))
            {
                for (iCurFace = i = 0; i < pkMesh->getNumFaces(); i++)
                {
                    if(FaceUsesOtherMaterial(pkMesh, i, iMaxAttr, iAttrId))
                    {
                        // skip any face that doesn't belong in this trishape
                        continue;
                    }
        
                    NIASSERT(iCurFace < iNumFaces);
        
                    NiPoint3& normal0 = pVertices[3*iCurFace+0].m_kNormal;
                    NiPoint3& normal1 = pVertices[3*iCurFace+1].m_kNormal;
                    NiPoint3& normal2 = pVertices[3*iCurFace+2].m_kNormal;

                    Point3 norm0 = pkNormalSpec->GetNormal(iCurFace, 0);
                    Point3 norm1 = pkNormalSpec->GetNormal(iCurFace, 1);
                    Point3 norm2 = pkNormalSpec->GetNormal(iCurFace, 2);

                    normal0.x = norm0.x;
                    normal0.y = norm0.y;
                    normal0.z = norm0.z;

                    normal1.x = norm1.x;
                    normal1.y = norm1.y;
                    normal1.z = norm1.z;

                    normal2.x = norm2.x;
                    normal2.y = norm2.y;
                    normal2.z = norm2.z;

                    iCurFace++;
                }
            }
        }
    }
#endif
    // apply xform to verts and normals (currently only used by psys
    // converter)
    if (pMat)
    {
        for (iCurFace = i = 0; i < pkMesh->getNumFaces(); i++)
        {
            if(FaceUsesOtherMaterial(pkMesh, i, iMaxAttr, iAttrId))
            {
                // skip any face that doesn't belong in this trishape
                continue;
            }
        
            NIASSERT(iCurFace < iNumFaces);
        
            for (j = 0; j < 3; j++)
            {
                NiOptVertex& vert = pVertices[3*iCurFace+j];

                vert.m_kPosition = (*pMat) * vert.m_kPosition;
                vert.m_kNormal = (*pMat) * vert.m_kNormal;
            }

            iCurFace++;
        }
    }

    Mesh* pkVertexColorMesh = NULL;
    bool bForceVertexColorsOn = false;
    bool bRadiosityOn = false;
    
#ifdef USE_RADIOSITY_MESH
    #if MAX_RELEASE < 6000
        IRadiosityMesh * rMesh = (IRadiosityMesh*)GetCOREInterface(
            IRADIOSITYMESHINTERFACE);
        if(rMesh && rMesh->IsNodeInSolution(pkMaxNode))
        {
            pkVertexColorMesh = &rMesh->GetRadiosityMesh(pkMaxNode);
            bForceVertexColorsOn = true;
            bRadiosityOn = true;
        }
        else
    #else 
        RadiosityInterface* pkRadInterface = (RadiosityInterface*) 
            GetCOREInterface(RADIOSITY_INTERFACE);
        RadiosityMesh* rMesh = NULL;
        if (pkRadInterface)
        {
            RadiosityEffect* pkRadEffect = pkRadInterface->GetRadiosity();
            if (pkRadEffect)
                rMesh = (RadiosityMesh*) GetRadiosityMesh(pkRadEffect);
        }

        if(rMesh && rMesh->DoesMeshExist(pkMaxNode))
        {
            if (rMesh->GetMesh(pkMaxNode, pkVertexColorMesh))
            {
                if (!rMesh->DoesSolutionExist())
                {
                    pkVertexColorMesh = pkMesh;
                    NiString strWarning = "Node \"";
                    strWarning += (const char*)pkMaxNode->GetName();
                    strWarning += "\" has radiosity, but the solution does "
                        "not exist.\nRadiosity will not be exported. Please"
                        " Reset the radiosity computation.";

                    NILOGWARNING((const char*)strWarning);
                }
                else if (pkVertexColorMesh != NULL && 
                    (pkVertexColorMesh->getNumFaces() != 
                    pkMesh->getNumFaces() ||
                     pkVertexColorMesh->numVerts != pkMesh->numVerts))
                {
                    pkVertexColorMesh = pkMesh;
                    NiString strWarning = "Node \"";
                    strWarning += (const char*)pkMaxNode->GetName();
                    strWarning += "\" has radiosity, but the computed "
                        "radiosity mesh does not match the base MAX Mesh.\n"
                        "Either the vertex counts do not match or the face "
                        "counts do not match.\n"
                        "This is likely due to the subdivision settings.\n"
                        "Radiosity will not be exported. Please Reset the "
                        "radiosity computation.";

                    NILOGWARNING((const char*)strWarning);
                }
                else
                {
                    bForceVertexColorsOn = true;
                    bRadiosityOn = true;
                }
            }
        }
        else
    #endif
    
#endif
    pkVertexColorMesh = pkMesh;

    bool bPerVertexAlpha = 
        (pkVertexColorMesh->mapSupport(-VDATA_ALPHA) ? true : false);
    if (!bPerVertexAlpha)
    {
        bPerVertexAlpha = 
            (pkVertexColorMesh->mapSupport(MAP_ALPHA) ? true : false);
    }

    // convert vertex colors if the object has them
    if (((pkVertexColorMesh->getNumVertCol()) && 
        !pkNiMAXUV->GetUseVCForUV()) || bPerVertexAlpha)
    {
        // convert the vertex colors as is
        for (iCurFace = i = 0; i < pkVertexColorMesh->getNumFaces(); i++)
        {
            if(FaceUsesOtherMaterial(pkVertexColorMesh, i, iMaxAttr, iAttrId))
            {
                // skip any face that doesn't belong in this trishape
                continue;
            }
            
            NIASSERT(iCurFace < iNumFaces);
            
            for (j = 0; j < 3; j++)
            {
                NiColorA& color = pVertices[3*iCurFace+j].m_kColor;
                if ((pkVertexColorMesh->vertColArray != NULL) && 
                    (pkVertexColorMesh->vcFaceData != NULL))
                {
                    Point3 vertCol = pkVertexColorMesh->vertColArray[
                        pkVertexColorMesh->vcFaceData[i].t[j]];
                    color.r = vertCol.x;
                    color.g = vertCol.y;
                    color.b = vertCol.z;
                }
                else if ((pkVertexColorMesh->vertCol != NULL) &&
                    (pkVertexColorMesh->vcFace != NULL))
                {
                    Point3 vertCol = pkVertexColorMesh->vertCol[
                        pkVertexColorMesh->vcFace[i].t[j]];
                    color.r = vertCol.x;
                    color.g = vertCol.y;
                    color.b = vertCol.z;
                }
                else
                {
                    color.r = 1.0f;
                    color.g = 1.0f;
                    color.b = 1.0f;
                }

#if MAX_RELEASE > 3000
                if (bPerVertexAlpha)
                {
                    UVVert * pMapVerts = 
                        pkVertexColorMesh->mapVerts(MAP_ALPHA);
                    if (pMapVerts) 
                    {
                        TVFace* tvf = 
                            &pkVertexColorMesh->mapFaces(MAP_ALPHA)[i];
                        Color PickCol(0,0,0);
                        PickCol = pMapVerts[tvf->t[j]];
                        color.a = PickCol.r;
                    }
                }
                else
                {
                    color.a = 1.0f;    
                }
#else
                color.a = 1.0f;    
#endif
            }
            
            iCurFace++;
        }
    }
    else if (((pkVertexColorMesh->vertColArray != NULL) && 
        !pkNiMAXUV->GetUseVCForUV()) || bPerVertexAlpha)
    {
        // convert the vertex colors as is
        for (iCurFace = i = 0; i < pkVertexColorMesh->getNumFaces(); i++)
        {
            if(FaceUsesOtherMaterial(pkVertexColorMesh, i, iMaxAttr, iAttrId))
            {
                // skip any face that doesn't belong in this trishape
                continue;
            }
            
            NIASSERT(iCurFace < iNumFaces);
            
            for (j = 0; j < 3; j++)
            {
                NiColorA& color = pVertices[3*iCurFace+j].m_kColor;
                if (pkVertexColorMesh->vertColArray != NULL)
                {
                    Point3 vertCol = pkVertexColorMesh->vertColArray[
                        pkVertexColorMesh->vcFaceData[i].t[j]];
                    color.r = vertCol.x;
                    color.g = vertCol.y;
                    color.b = vertCol.z;
                }
                else
                {
                    color.r = 1.0f;
                    color.g = 1.0f;
                    color.b = 1.0f;
                }

                if (bPerVertexAlpha)
                {
                    UVVert * pMapVerts = 
                        pkVertexColorMesh->mapVerts(MAP_ALPHA);
                    if (pMapVerts) 
                    {
                        TVFace* tvf = 
                            &pkVertexColorMesh->mapFaces(MAP_ALPHA)[i];
                        Color PickCol(0,0,0);
                        PickCol = pMapVerts[tvf->t[j]];
                        color.a = PickCol.r;
                    }
                }
                else
                {
                    color.a = 1.0f;    
                }
            }
            iCurFace++;
        }
    }
    
    // convert UV channels 
    int iSlot = 0;
    int uiNumberOfMapChannels = pkNiMAXUV->GetNumberOfChannels();
    // MapIndex is: 0=vert colors, 1=original TVerts,
    // 2 and up=new map channels

    int iNumberOfMaxChannels = pkMesh->getNumMaps();

    // iterate over all known convertable UV channels based off
    // of the material
    int k = 0;
    for(; k < uiNumberOfMapChannels; k++)
    {
        int iMaxChannelID = k;
        if (!pkNiMAXUV->IsChannel(k)) 
            continue;

        //grab the max channel id and use it to get the pointer to the face
        //object.  if null, then UVW mappings were not applied to that 
        //channel in Max, and we abort from the loop so as not to generate
        //a memory access exception.

        iMaxChannelID = pkNiMAXUV->GetMAXChannelForNiChannelAt(k);

        if (iMaxChannelID >= iNumberOfMaxChannels)
            continue;
        
        if (pkMesh->mapSupport(iMaxChannelID) &&
            pkMesh->getNumMapVerts(iMaxChannelID))
        {
            TVFace* pkMapFaces = pkMesh->mapFaces(iMaxChannelID);
            if(!pkMapFaces)
            {
                continue;
            }
            for (iCurFace = i = 0; i < pkMesh->getNumFaces(); i++)
            {
                if(FaceUsesOtherMaterial(pkMesh, i, iMaxAttr, iAttrId))
                {
                    // skip any face that doesn't belong in this trishape
                    continue;
                }//if(FaceUses...0
        
                NIASSERT(iCurFace < iNumFaces);

                for (j = 0; j < 3; j++)
                {
                
                    int iMapVertsIndex = pkMapFaces[i].t[j];
                    float* pMaxUV = (float *) &pkMesh->mapVerts(
                        iMaxChannelID)[iMapVertsIndex];

                    ConvertUVs(pVertices[3*iCurFace+j].m_akUV[iSlot],
                        pMaxUV, k, pkNiMAXUV);                        
                }
                iCurFace++;
            }//for(iCurFace....


            // This next line lets MI know where this channel went.
            pkNiMAXUV->SetMAXtoNI(k, iSlot++);
        }//if (pkMesh....
    }//for(k ....

    // now there may be some UV channels that need to be exported that 
    // haven't already been exported explicitly by the material
    for(int iMaxChannel = 0; iMaxChannel < iNumberOfMaxChannels; iMaxChannel++)
    {
        int iMaxChannelID = iMaxChannel;
        bool bAlreadyExported = false;
        for (int iConvertedChannels = 0; 
            iConvertedChannels < pkNiMAXUV->GetNumberOfChannels();
            iConvertedChannels++)
        {
            if (pkNiMAXUV->IsChannel(iConvertedChannels))
            {
                if (iMaxChannelID == 
                    pkNiMAXUV->GetMAXChannelForNiChannelAt(
                    iConvertedChannels))
                {
                    bAlreadyExported = true;
                    break;
                }
            }               
        }

        if (bAlreadyExported == true)
            continue;

        if (pkMesh->mapSupport(iMaxChannelID) &&
            pkMesh->getNumMapVerts(iMaxChannelID))
        {
            TVFace* pkMapFaces = pkMesh->mapFaces(iMaxChannelID);
            if(!pkMapFaces)
            {
                continue;
            }
            for (iCurFace = i = 0; i < pkMesh->getNumFaces(); i++)
            {
                if(FaceUsesOtherMaterial(pkMesh, i, iMaxAttr, iAttrId))
                {
                    // skip any face that doesn't belong in this trishape
                    continue;
                }//if(FaceUses...0
        
                NIASSERT(iCurFace < iNumFaces);

                for (j = 0; j < 3; j++)
                {
                
                    int iMapVertsIndex = pkMapFaces[i].t[j];
                    float* pMaxUV = (float *) &pkMesh->mapVerts(
                        iMaxChannelID)[iMapVertsIndex];

                    ConvertUVs(pVertices[3*iCurFace+j].m_akUV[iSlot],
                        pMaxUV, k, NULL);                        
                }
                iCurFace++;
            }//for(iCurFace....
            iSlot++;
        }//if (pkMesh....
    }//for(unsigned int k ....



    bool bForceDown = (pTracker || pkSkinWrapper->m_pkSkinMod);
    float fUniformScale = ApplyScaleInfo(pParentScale, pVertices, iNumVerts, 
        pIndices, iNumFaces, bForceDown);


    for (i = 0; i < iNumVerts; i++)
        pVertices[i].m_uiNumUVs = iSlot;

    // copy the skin/bone information into the vertices before simplification
    unsigned int uiNumBones = 0;
    INode** ppBones = NULL;
    NiTransform* pInitXforms = NULL;
    NiTransform kRootParentToSkin;
    kRootParentToSkin.MakeIdentity();
    INode* pRootParent = NULL;

    if (pRemapping && bIsTwoSided)
    {
        unsigned int uiSize = iNumVerts * sizeof(unsigned short);
        NiMemcpy(pRemapping + iNumVerts, pRemapping, uiSize);
    }
    
#ifndef NO_CSTUDIO
    TrackExtractInfo(pTracker, pVertices, iNumVerts, pRemapping,
        &uiNumBones, &ppBones, &pRootParent, &pInitXforms,
        &kRootParentToSkin);
#endif
    if (pkSkinWrapper->m_pkSkinMod && !pTracker)
    {

        ConvertSkin(pkSkinWrapper->m_pkSkinMod, pVertices, iNumVerts, 
            pRemapping, uiNumBones, ppBones, pRootParent, pInitXforms,
            kRootParentToSkin, pkMaxNode, 
            pkSkinWrapper->m_bUseRigidVertices);

    }

    bool bKeepDegenerates = NiMAXMorphConverter::IsMorphObject(pkMaxNode);

    NiOptTriShape* temp = NiNew NiOptTriShape(iNumVerts, pVertices, uiNumBones,
        (void **) ppBones, (void *)pRootParent, pInitXforms, kRootParentToSkin,
        iNumVerts, pIndices, true, bIsTwoSided, bKeepDegenerates);

    NiTriShape *pTris;
    
    //Gamebryo vertex limit check
    if(temp->ConvertTo(pTris) == NiOptTriShape::OPT_VERTLIMITERROR)
    {
        NiFree(pVertWeights);
        NiFree(pRemapping);
        NiDelete temp;

        char acString[1024];
        NiSprintf(acString, 1024, "Mesh \"%s\" will have more than %d verts\n"
           "after export. It will not be exported\n", 
           pkMaxNode->GetName(), USHRT_MAX);
        NILOGWARNING(acString);
        return(W3D_STAT_OK);
    }

    if (pTris == NULL)
    {
        NiFree(pVertWeights);
        NiFree(pRemapping);
        NiDelete temp;
        return(W3D_STAT_OK);
    }
    
    pTris->GetModelData()->SetConsistency(NiGeometryData::STATIC);
    pTris->SetScale(fUniformScale);
    
    const unsigned int *pNewToOld = temp->GetNewToOldMapping();
    
    iNumVerts = temp->GetNewVertexCount();
    
    if (pVertWeights)
    {
        float *pNewVertWeights = NiAlloc(float, iNumVerts);
        NIASSERT(pNewVertWeights);

        for (i = 0; i < iNumVerts; i++)
            pNewVertWeights[i] = pVertWeights[pNewToOld[i]];

        NiFree(pNewVertWeights);
    }

    if (pRemapping)
    {
        unsigned short *pNewRemapping = NiAlloc(unsigned short, iNumVerts);
        NIASSERT(pNewRemapping);
                
        for (i = 0; i < iNumVerts; i++)
            pNewRemapping[i] = pRemapping[pNewToOld[i]];
                
        // delete the temporary remapping we created to hold where each
        // final vertex came from
        NiFree(pRemapping);
                
        pRemapping = pNewRemapping;
    }

    NiDelete temp;
    *ppTris = pTris;

    // If a skin controller was created by the ConvertTo call we will
    // need to resolve its bone pointers once all the conversion is done
    NiSkinInstance* pkSkin = pTris->GetSkinInstance();
    if (pkSkin)
        m_skinList2.AddHead(pkSkin);
    
    NiFree(pVertWeights);

    if (ppRemapping)
    {
        NIASSERT(!bDeleteRemapping);
        *ppRemapping = pRemapping;
    }
    else if (bDeleteRemapping)
    {
        NIASSERT(ppRemapping == NULL);
        NiFree(pRemapping);
    }

    if (bForceVertexColorsOn)
    {
        NiVertexColorProperty* pkVCP = NiNew NiVertexColorProperty;
        pkVCP->SetSourceMode(NiVertexColorProperty::SOURCE_EMISSIVE);
        pkVCP->SetLightingMode(NiVertexColorProperty::LIGHTING_E);        
        pTris->AttachProperty(pkVCP);
    }
    CHECK_MEMORY();
    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
void NiMAXMeshConverter::GetVerts(Object *pObj, INode *pMaxNode, 
    ScaleInfo *pParentScale, unsigned int &uiNumVerts, NiPoint3 *&pVerts)
{
    CHECK_MEMORY();
    Mesh *pkMesh;
    BOOL bDeleteMesh;
    NullView nullView;
    GeomObject *pGeomObj;
    Quat q;
    Point3 p;
    ScaleInfo scaleInfo;
    unsigned int i;

    uiNumVerts = 0;
    pVerts = NULL;
    
    NiMAXHierConverter::GetOOTM(pMaxNode, &p, &q, pParentScale, &scaleInfo);

    ReferenceMaker *pRm = pMaxNode;
    pRm->RenderBegin(0);

    pGeomObj = (GeomObject *) pObj;
    pkMesh = pGeomObj->GetRenderMesh(m_animStart, pMaxNode, 
                                    nullView, bDeleteMesh);

    uiNumVerts = pkMesh->getNumVerts();
    if (uiNumVerts == 0)
        return;
    
    pVerts = NiNew NiPoint3[uiNumVerts];
    NIASSERT(pVerts);

    // convert the vertices
    for (i = 0; i < uiNumVerts; i++)
    {
        pVerts[i].x = pkMesh->verts[i].x;
        pVerts[i].y = pkMesh->verts[i].y;
        pVerts[i].z = pkMesh->verts[i].z;
    }
        
    ApplyScaleInfo(&scaleInfo, pVerts, uiNumVerts);

    if (bDeleteMesh)
        pkMesh->DeleteThis();
    
    pRm->RenderEnd(0);
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
// Accept a MAX object "pObj" and return the NI geometry in "ppGeom"
int NiMAXMeshConverter::ConvertGeomSub(Object *pObj,
    INode *pMaxNode, NiNode *pNode, NiAVObject **ppGeom, 
    bool bIgnoreHash, bool bIgnoreMtls, ScaleInfo *pParentScale,
    bool bDoVertexRemapping, unsigned short**& ppVertexRemapping,
    NiAVObject**& ppRemapTargets, unsigned int& uiNumRemappings,
    NiMAXUV*& pkNiMAXUV)
{
    CHECK_MEMORY();
    bool bIs2Sided;
    NullView nullView;
    GeomObject *pGeomObj;
    int iNumMats, iStatus;
    NiTriShape *pTris;
    NiMAXMaterialConverter mtl(m_animStart, m_animEnd);
    Quat q;
    Point3 p;
    ScaleInfo scaleInfo;
    Matrix3 rot;
    NiMatrix3 niRot;
    ScaleInfo *pScaleInfo;

    NiMAXHierConverter::GetOOTM(pMaxNode, &p, &q, pParentScale, &scaleInfo);

    *ppGeom = NULL;

    // check the hash table to see if this Mesh is to be instanced
    if (!pObj->TestAFlag(A_WORK1))
        SetFlags(pObj, ND_SET);
    else
    {
        if (!bIgnoreHash)
        {
            // instanced node
            *ppGeom = ms_pkMeshHTab->Retrieve(pObj, &scaleInfo);
            if (*ppGeom)
            {
                NiAVObject* pkOriginalObj = (NiAVObject*)*ppGeom;
                *ppGeom = (NiAVObject*)(*ppGeom)->Clone();
                NiAVObject* pkClonedObj = *ppGeom;
                q.MakeMatrix(rot);
                MaxToNI(rot, niRot);

                ProcessInstance(pkOriginalObj, pkClonedObj, pNode,
                    NiPoint3(p.x, p.y, p.z), niRot, scaleInfo);

                return(W3D_STAT_OK);
            }
        }
    }

    ReferenceMaker *pRm = pMaxNode;
    pRm->RenderBegin(0);

    pGeomObj = (GeomObject*) pObj;

    // Get the appropriate Mesh from MAX
    BOOL bDeleteMesh;
    Mesh* pkMesh = NULL;
    MNMesh* pkMNMesh = NULL;

    if (pObj->ClassID()==polyObjectClassID) 
    {
        PolyObject* pkPolyObject = (PolyObject*) pObj;
        NIASSERT(pkPolyObject);
        pkMNMesh = &pkPolyObject->GetMesh();
    }

    pkMesh = pGeomObj->GetRenderMesh(m_animStart, pMaxNode, nullView, 
        bDeleteMesh);
    NIASSERT(pkMesh != NULL);

    NtPointTracker* pTracker = NULL;
    Modifier* pkMod;
    Object *pObj2 = pMaxNode->GetObjectRef();
#ifndef NO_CSTUDIO
    NtPointTracker tracker;
    pkMod = FindModifier(pObj2, 
        Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B));

    if (pkMod)
    {
        pTracker = &tracker;
        tracker.pMod = pkMod;
        tracker.pPExport = (IPhysiqueExport *) 
            pkMod->GetInterface(I_PHYINTERFACE);
        tracker.pMCExport = (IPhyContextExport *) 
            pTracker->pPExport->GetContextInterface(pMaxNode);
        tracker.pMCExport->ConvertToRigid(true);
        tracker.pMCExport->AllowBlending(true);
        tracker.bInvalid = false;

        Matrix3 skininitTM;
        int iStatus = tracker.pPExport->GetInitNodeTM(pMaxNode, skininitTM);

        if (iStatus != MATRIX_RETURNED || 
            !MaxToNI(skininitTM, tracker.kSkinInitTM))
        {
            NILOGWARNING("Object has a physique modifier that does\n"
                " not include the skin's bind-pose\n"
                " transformation matrix. Delete the Physique\n"
                " modifier and create a new one in order to\n"
                " export skinning data correctly.\n");
            skininitTM = pMaxNode->GetObjectTM(0);
            MaxToNI(skininitTM, tracker.kSkinInitTM);
        }

        tracker.pkSkin = pMaxNode;
    }
#endif

    pkMod = FindModifier(pObj2, SKIN_CLASSID);
    ISkinWrapper* pkSkinMod = NiExternalNew ISkinWrapper(pkMod);
    bool bForceMultiSub = false;
    iNumMats = NiMAXMaterialConverter::GetNumMaterials(pMaxNode->GetMtl(), 
        bForceMultiSub);

    q.MakeMatrix(rot);
    MaxToNI(rot, niRot);

    if (!bForceMultiSub && (iNumMats == 1 || bIgnoreMtls))
    {
        if (bDoVertexRemapping)
        {
            ppVertexRemapping = NiAlloc(unsigned short*, 1);
            ppRemapTargets = NiAlloc(NiAVObject*, 1);
            uiNumRemappings = 1;
        }
        else
        {
            ppVertexRemapping = NULL;
            ppRemapTargets = NULL;
            uiNumRemappings = 0;
        }

        NiMAXMaterialConverter::GetUVInfo(pMaxNode->GetMtl(), m_animStart, 
            pkNiMAXUV, -1);

        bIs2Sided = NiMAXMaterialConverter::IsTwoSided(pMaxNode->GetMtl(),
            0);

        bool bAttachTwoSidedProperty = false;

        if(bIs2Sided  && !IsTrue(NiMAXOptions::GetValue(NI_USEOLDBEHAVIOR)))
        {
            bAttachTwoSidedProperty = true;
            bIs2Sided = false;
        }

        if (bDoVertexRemapping)
        {
            iStatus = ConvertMesh(pkMesh, pkMNMesh, &pTris, -1, 1, pkNiMAXUV,
                NULL, bIs2Sided, &scaleInfo, pTracker, &ppVertexRemapping[0],
                pkSkinMod, pMaxNode);
            ppRemapTargets[0] = pTris;
        }
        else
        {
            iStatus = ConvertMesh(pkMesh, pkMNMesh, &pTris, -1, 1, pkNiMAXUV,
                NULL, bIs2Sided, &scaleInfo, pTracker, NULL, pkSkinMod,
                pMaxNode);
        }

        if (iStatus || pTris == NULL)
            goto cleanup_and_exit;

        if (!ComputeObjectPrecacheFlags(pTris, pObj))
            ComputeHierPrecacheFlags(pTris, pMaxNode);
        
        if(bAttachTwoSidedProperty) 
            pTris->AttachProperty(ms_spStencilTwoSidedProperty);

        pTris->SetRotate(niRot);
        pTris->SetTranslate(p.x, p.y, p.z);

        if (!bIgnoreMtls)
        {
            NiNode* pkEffectNode = NULL;
            iStatus = mtl.Convert(pMaxNode->GetMtl(), pNode, 
                pTris, pMaxNode, 0, pkNiMAXUV, pkEffectNode);
            NiDelete pkEffectNode;

            
            if (iStatus)
            {
                NiDelete pTris;
                goto cleanup_and_exit;
            }
            else if (pTris && pkNiMAXUV)
            {
                NiTexturingProperty* pkProperty = (NiTexturingProperty*) 
                pTris->GetProperty(NiProperty::TEXTURING);
                
                if (pkProperty)
                {
                    NiMAXUV::BakeTransformsIntoGeometry(pTris, pkProperty);
                }
            }
        }

        *ppGeom = pTris;

        (*ppGeom)->SetName(pGeomObj->GetObjectName());

    }
    else
    {
        NiNode *pNode;
        int i;

        // There is more than one material on this Mesh. We will have
        // to break it into multiple fragments
        if (bDoVertexRemapping)
        {
            ppVertexRemapping = NiAlloc(unsigned short*, iNumMats);
            memset(ppVertexRemapping, 0, sizeof(unsigned short*) * iNumMats);
            ppRemapTargets = NiAlloc(NiAVObject*, iNumMats);
            uiNumRemappings = iNumMats;
        }
        else
        {
            ppVertexRemapping = NULL;
            ppRemapTargets = NULL;
            uiNumRemappings = 0;
        }

        pNode = NiNew NiNode;
        pNode->SetName(gpcMultiMtlName);

        for (i = 0; i < iNumMats; i++)
        {
            NiMAXMaterialConverter::GetUVInfo(pMaxNode->GetMtl(), 
                m_animStart, pkNiMAXUV, i);
        }
        for (i = 0; i < iNumMats; i++)
        {
            bIs2Sided = NiMAXMaterialConverter::IsTwoSided(
                pMaxNode->GetMtl(), i);
            bool bAttachTwoSidedProperty = false;
            if(bIs2Sided  && !IsTrue(
                NiMAXOptions::GetValue(NI_USEOLDBEHAVIOR)))
            {
                bAttachTwoSidedProperty = true;
                bIs2Sided = false;
            }

            // Convert the Mesh fragment having material # "i"
            NIASSERT(pkNiMAXUV);
            pkNiMAXUV->SetAttrId(i);

            if (bDoVertexRemapping)
            {
                iStatus = ConvertMesh(pkMesh, pkMNMesh, &pTris, i, iNumMats,
                    pkNiMAXUV, NULL, bIs2Sided, &scaleInfo, pTracker,
                    &ppVertexRemapping[i], pkSkinMod, pMaxNode);
                ppRemapTargets[i] = pTris;
            }
            else
            {
                iStatus = ConvertMesh(pkMesh, pkMNMesh, &pTris, i, iNumMats,
                    pkNiMAXUV, NULL, bIs2Sided, &scaleInfo, pTracker,
                    NULL, pkSkinMod, pMaxNode);
            }

            if (iStatus)
            {
                NiDelete pNode;
                goto cleanup_and_exit;
            }

            if (pTris == NULL)
                continue;

            if (!ComputeObjectPrecacheFlags(pTris, pObj))
                ComputeHierPrecacheFlags(pTris, pMaxNode);

            if(bAttachTwoSidedProperty) 
                pTris->AttachProperty(ms_spStencilTwoSidedProperty);

            pTris->SetRotate(niRot);
            pTris->SetTranslate(p.x, p.y, p.z);

            // pkEffectNode will be allocated by the material converter in the
            // case of a reflection map
            NiNode* pkEffectNode = NULL;
            iStatus = mtl.Convert(pMaxNode->GetMtl(), pNode, pTris, pMaxNode, 
                i, pkNiMAXUV, pkEffectNode, false);
            if (iStatus)
            {
                NiDelete pTris;
                NiDelete pNode;
                goto cleanup_and_exit;
            }

            // create an appropriate name for the Mesh fragments
            char number[10], *pFinalName, *pBaseName;

            NiSprintf(number, 10, ":%d", i);
            pBaseName = pMaxNode->GetName();    

            unsigned int uiLen = strlen(number) + strlen(pBaseName) + 1;
            pFinalName = NiAlloc(char, uiLen);
            NIASSERT(pFinalName);

            NiSprintf(pFinalName, uiLen, "%s%s", pBaseName, number);

            pTris->SetName(pFinalName);
            NiFree(pFinalName);

            if (!pkEffectNode)
            {
                pNode->AttachChild(pTris);
            }
            else
            {
                // if there was an effect node created, the geometry should be
                // the child of it instead of the parent
                pkEffectNode->AttachChild(pTris);
            }

            if (pTris && pkNiMAXUV)
            {
                NiTexturingProperty* pkProperty = (NiTexturingProperty*) 
                    pTris->GetProperty(NiProperty::TEXTURING);
                if(pkProperty)
                {
                    NiMAXUV::BakeTransformsIntoGeometry(pTris, pkProperty);
                }
            }
        
        }

        if (pkNiMAXUV)
        {
            NiDelete pkNiMAXUV;
            pkNiMAXUV = 0;
        }

        *ppGeom = pNode;
    }

    pScaleInfo = NiNew ScaleInfo;
    *pScaleInfo = scaleInfo;

    ms_pkMeshHTab->Insert(pObj, pScaleInfo, *ppGeom);

    iStatus = W3D_STAT_OK;

cleanup_and_exit:
#ifndef NO_CSTUDIO
    if (pTracker)
    {
        pTracker->pPExport->ReleaseContextInterface(pTracker->pMCExport);
        pTracker->pMod->ReleaseInterface(I_PHYINTERFACE, pTracker->pPExport);
    }
#endif
    
    if (bDeleteMesh)
        pkMesh->DeleteThis();
//        delete pkMesh;
    
    pRm->RenderEnd(0);
    CHECK_MEMORY();

    return(iStatus);
}

//---------------------------------------------------------------------------
void
NiMAXMeshConverter::Init()
{
    NIASSERT(ms_pkMeshHTab == NULL);
    ms_pkMeshHTab = NiNew NiTHash<NiAVObjectPtr>(113, PFMeshHashEq, 
        PFMeshHashFree);
    NIASSERT(ms_pkMeshHTab->GetNumHashed() == 0);
    ms_spStencilTwoSidedProperty = NiNew NiStencilProperty;
    ms_spStencilTwoSidedProperty->SetDrawMode(NiStencilProperty::DRAW_BOTH);
    
}
//---------------------------------------------------------------------------
void
NiMAXMeshConverter::Shutdown()
{
    // cleanup the object hash table
    NiDelete ms_pkMeshHTab;
    ms_pkMeshHTab = NULL;
    ms_spStencilTwoSidedProperty = NULL;
}
//---------------------------------------------------------------------------
void NiMAXMeshConverter::ConvertUVs(NiPoint2 &UV, float* pUVs,
    int iMapIndex, NiMAXUV* pkNiMAXUV)
{

// Tiling indices 0 and 2 are U offset (phase) and tiling (frequency),
//                1 and 3 are V offset (phase) and tiling (frequency),
//                respectively.

// Order index 0 is the MAX U/V/W that maps to NI U
//       index 1 is the MAX U/V/W that maps to NI V

    if (pkNiMAXUV && pkNiMAXUV->IsChannel(iMapIndex))
    {
        UV.x = pUVs[pkNiMAXUV->GetOrder(iMapIndex)[0]];
        UV.y = 1.0f - pUVs[pkNiMAXUV->GetOrder(iMapIndex)[1]];        

        NiPoint2 kCropDim = pkNiMAXUV->GetCropDimensions(iMapIndex);
        NiPoint2 kCropOffset = pkNiMAXUV->GetCropOffset(iMapIndex);

        if (kCropOffset.x != 0.0f || kCropOffset.y != 0.0f || 
            kCropDim.x != 1.0f || kCropDim.y != 1.0f)
        {
            float URange = kCropDim.x;
            float VRange = kCropDim.y;
            UV.x = kCropOffset.x  + UV.x * URange;
            UV.y = kCropOffset.y  + UV.y * VRange;
        }
    }
    else
    {
        UV.x = pUVs[0];
        UV.y = 1.0f - pUVs[1];
    }
}
//---------------------------------------------------------------------------
#define NICONSISTENCYFLAG           "Consistency"
#define NIKEEPPOSITIONSFLAG         "KeepPositions"
#define NIKEEPNORMALSSFLAG          "KeepNormals"       
#define NIKEEPVERTCOLORSSFLAG       "KeepVertColors"    
#define NIKEEPUVSFLAG               "KeepUVCoords"  
#define NIKEEPWEIGHTSFLAG           "KeepBoneIndices"
#define NIKEEPBONEDATAFLAG          "KeepBoneData"
#define NICOMPRESSPOSITIONSFLAG     "CompressPositions"
#define NICOMPRESSNORMALSSFLAG      "CompressNormals"
#define NICOMPRESSVERTCOLORSSFLAG   "CompressVertColors"
#define NICOMPRESSUVSFLAG           "CompressUVCoords"
#define NICOMPRESSWEIGHTSFLAG       "CompressBoneWeights"


bool NiMAXMeshConverter::ComputePrecacheFlags(NiTriShape* pTris, 
    Animatable* pkObj)
{
    if (!pkObj || !pTris)
        return false;

    NiNodePtr spTempNode = NiNew NiNode;
    NiMAXCustAttribConverter kConverter(0, 1);
    kConverter.Convert(pkObj, spTempNode, false);

    NiBooleanExtraData* pkBoolED = NULL;
    NiIntegerExtraData* pkIntED = 
        (NiIntegerExtraData*) spTempNode->GetExtraData(NICONSISTENCYFLAG);

    if (!pkIntED)
        return false;
    
    NiGeometryData* pkData = pTris->GetModelData();
    if (!pkData)
        return false;

    switch (pkIntED->GetValue())
    {
        case 2:
            pkData->SetConsistency(NiGeometryData::STATIC);
            break;
        case 3:
            pkData->SetConsistency(NiGeometryData::MUTABLE);
            break;
        case 4:
            pkData->SetConsistency(NiGeometryData::VOLATILE);
            break;
    }

    unsigned int uiCompressFlags = pkData->GetCompressFlags();
    unsigned int uiKeepFlags = pkData->GetKeepFlags();

    pkBoolED = 
        (NiBooleanExtraData*) spTempNode->GetExtraData(NIKEEPPOSITIONSFLAG);

    if (!pkBoolED)
        return false;
    if (pkBoolED->GetValue())
        uiKeepFlags |= NiGeometryData::KEEP_XYZ;

    pkBoolED = 
        (NiBooleanExtraData*) spTempNode->GetExtraData(NIKEEPNORMALSSFLAG);

    if (!pkBoolED)
        return false;
    if (pkBoolED->GetValue())
        uiKeepFlags |= NiGeometryData::KEEP_NORM;

    pkBoolED = 
        (NiBooleanExtraData*) spTempNode->GetExtraData(
        NIKEEPVERTCOLORSSFLAG);

    if (!pkBoolED)
        return false;
    if (pkBoolED->GetValue())
        uiKeepFlags |= NiGeometryData::KEEP_COLOR;
            
            
    pkBoolED = 
        (NiBooleanExtraData*) spTempNode->GetExtraData(NIKEEPUVSFLAG);

    if (!pkBoolED)
        return false;
    if (pkBoolED->GetValue())
        uiKeepFlags |= NiGeometryData::KEEP_UV;
            
    pkBoolED = 
        (NiBooleanExtraData*) spTempNode->GetExtraData(NIKEEPWEIGHTSFLAG);

    if (!pkBoolED)
        return false;
    if (pkBoolED->GetValue())
        uiKeepFlags |= NiGeometryData::KEEP_INDICES;
            
    pkBoolED = 
        (NiBooleanExtraData*) spTempNode->GetExtraData(NIKEEPBONEDATAFLAG);

    if (!pkBoolED)
        return false;
    if (pkBoolED->GetValue())
        uiKeepFlags |= NiGeometryData::KEEP_BONEDATA;
            
          
    pkBoolED = 
        (NiBooleanExtraData*) spTempNode->GetExtraData(
        NICOMPRESSPOSITIONSFLAG);

    if (!pkBoolED)
        return false;
    if (pkBoolED->GetValue())
        uiCompressFlags |= NiGeometryData::COMPRESS_POSITION;

    pkBoolED = 
        (NiBooleanExtraData*) spTempNode->GetExtraData(
        NICOMPRESSNORMALSSFLAG);

    if (!pkBoolED)
        return false;
    if (pkBoolED->GetValue())
        uiCompressFlags |= NiGeometryData::COMPRESS_NORM;

    pkBoolED = 
        (NiBooleanExtraData*) spTempNode->GetExtraData(
        NICOMPRESSVERTCOLORSSFLAG);

    if (!pkBoolED)
        return false;
    if (pkBoolED->GetValue())
        uiCompressFlags |= NiGeometryData::COMPRESS_COLOR;
            
            
    pkBoolED = 
        (NiBooleanExtraData*) spTempNode->GetExtraData(NICOMPRESSUVSFLAG);

    if (!pkBoolED)
        return false;
    if (pkBoolED->GetValue())
        uiCompressFlags |= NiGeometryData::COMPRESS_UV;
            
    pkBoolED = 
        (NiBooleanExtraData*) spTempNode->GetExtraData(
        NICOMPRESSWEIGHTSFLAG);

    if (!pkBoolED)
        return false;
    if (pkBoolED->GetValue())
        uiCompressFlags |= NiGeometryData::COMPRESS_WEIGHT;
           
    pkData->SetCompressFlags(uiCompressFlags);
    pkData->SetKeepFlags(uiKeepFlags);    
    return true;
}
//---------------------------------------------------------------------------
bool NiMAXMeshConverter::ComputeObjectPrecacheFlags(NiTriShape* pTris, 
    Object* pkObject)
{
    bool bReturn = ComputePrecacheFlags(pTris, pkObject);
    if (!bReturn && pkObject && pkObject->SuperClassID() == 
        GEN_DERIVOB_CLASS_ID)
    {
        IDerivedObject* pkDerived = (IDerivedObject*) pkObject;
        for (int i = 0; i < pkDerived->NumModifiers(); i++)
        {
            Modifier* pkMod = pkDerived->GetModifier(i);
            if (pkMod && ComputePrecacheFlags(pTris, pkMod))
                return true;
        }
    }
    if (!bReturn && pkObject)
    {
        Object* pkBaseObj = pkObject->FindBaseObject();
        if (pkBaseObj && ComputePrecacheFlags(pTris, pkBaseObj))
            return true;
    }

    return bReturn;
}
//---------------------------------------------------------------------------
bool NiMAXMeshConverter::ComputeHierPrecacheFlags(NiTriShape* pTris, 
    INode* pkMaxNode)
{
    if (pkMaxNode == NULL)
        return false;

    if (ComputePrecacheFlags(pTris, pkMaxNode))
        return true;
    else
    {
        if (!ComputeObjectPrecacheFlags(pTris, pkMaxNode->GetObjectRef()))
        {
            return ComputeHierPrecacheFlags(pTris, 
                pkMaxNode->GetParentNode());
        }
        else
        {
            return true;
        }
    }
}
//---------------------------------------------------------------------------
bool NiMAXMeshConverter::ProcessInstance(NiAVObject* pkOrig, 
    NiAVObject* pkInst, NiNode* pkInstParent, NiPoint3 kPos, NiMatrix3 kRot, 
    ScaleInfo kScale)
{
    // This method recursively propagates transform information, collects
    // skin instance references, and re-targets texture effects so that an
    // instanced scene can be properly cloned
    if (pkInst->IsLeaf() && pkOrig->IsLeaf())
    {
        // if it is a leaf object, pass down the transform
        if(!kScale.IsAnisotropic())
        {
            pkInst->SetScale(kScale.scale.s.x);
        }
        else
        {
            pkInst->SetScale(1.0f);
        }
        pkInst->SetRotate(kRot);
        pkInst->SetTranslate(kPos);

        // check if the parent is the target of a reflection map
        // if so, make the clone also a target
        // we can't assume the clone has already been attached to the scene
        NiNode* pkOrigParent = pkOrig->GetParent();
        if (pkOrigParent && pkInstParent)
        {
            const NiDynamicEffectList* pkList = 
                &pkOrigParent->GetEffectList();
            NiTListIterator kIter = pkList->GetHeadPos();
            while (kIter)
            {
                NiDynamicEffect* pkEffect = NiDynamicCast(
                    NiTextureEffect, pkList->GetNext(kIter));
                if (pkEffect)
                {
                    pkEffect->AttachAffectedNode(pkInstParent);
                }
            }
        }

        // if we have cloned a skinned geometry, we must register
        // the new skin instance
        NiGeometry* pkGeometry = NiDynamicCast(NiGeometry, 
            pkInst);
        if (pkGeometry)
        {
            NiSkinInstance* pkSkin = pkGeometry->GetSkinInstance();
            if (pkSkin)
                m_skinList2.AddHead(pkSkin);
        }
    }
    else if (pkInst->IsNode() && pkOrig->IsNode())
    {
        // if it is a node, just recurse
        bool bResult = true;
        NiNode* pkInstNode = (NiNode*)pkInst;
        NiNode* pkOrigNode = (NiNode*)pkOrig;
        NIASSERT(pkInstNode->GetArrayCount() == pkOrigNode->GetArrayCount());
        for (unsigned int ui = 0; ui < pkInstNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkInstChild = pkInstNode->GetAt(ui);
            NiAVObject* pkOrigChild = pkOrigNode->GetAt(ui);
            if (pkInstChild && pkOrigChild)
            {
                // if any child fails, they all do
                if (!ProcessInstance(pkOrigChild, pkInstChild, pkInstNode, 
                    kPos, kRot, kScale))
                {
                    bResult = false;
                }
            }
        }
        return bResult;
    }
    else
    {
        // if we have gotten here, the cloned object does not match the
        // original
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiMAXMeshConverter::ISkinWrapper::ISkinWrapper(Modifier* pkMod)
{
    m_pkSkinMod = NULL;
    m_bUseRigidVertices = false;
    if (pkMod)
        m_pkSkinMod = (ISkin*) pkMod->GetInterface(I_SKIN);
    bool bSetRigid = false;
    if (m_pkSkinMod)
    {
        NiString strRigidVertices("rigid_vertices");
        Animatable* pkAnim = ((Animatable*)pkMod);
        NiMAXCustAttribConverter kAttrib(0,0);
        for (int ui = 0; ui < pkAnim->NumParamBlocks() && !bSetRigid;ui++)
        {
            IParamBlock2* pkParamBlock = pkAnim->GetParamBlock(ui);
            if (pkParamBlock)
            {
                ParamBlockDesc2 *pkParamDesc = pkParamBlock->GetDesc();
                int num_params = pkParamBlock->NumParams();
                for (int iIdx = 0; iIdx < num_params; iIdx++)
                {
                    ParamDef& kParamDef = 
                        pkParamDesc->GetParamDef(pkParamDesc->IndextoID(
                        iIdx));
                    NiString strValueName = kParamDef.int_name;

                    if (strValueName.EqualsNoCase(strRigidVertices))
                    {
                        int ivalue = 0;
                        Interval iValid;
                        if (pkParamBlock->GetValue(pkParamDesc->IndextoID(
                            iIdx), 0, ivalue, iValid))
                        {
                            m_bUseRigidVertices = ivalue ? true : false;
                            bSetRigid = true;
                            break;
                        }
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
