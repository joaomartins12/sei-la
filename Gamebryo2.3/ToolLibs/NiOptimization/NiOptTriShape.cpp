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

#include "NiOptTriShape.h"
#include "NiOptimize.h"

//---------------------------------------------------------------------------
NiOptTriShape::NiOptTriShape(unsigned int uiVertices, NiOptVertex* pkVertex,
    unsigned int uiNumBones, void** ppvBones, void *pvRootParent,
    NiTransform* pkInitXforms, NiTransform &kRootParentToSkin,
    unsigned int uiTriListLength, unsigned int* puiTriList,
    bool bAllowLargeTriList, bool bMakeTwoSided, bool bKeepDegenerates) :
        NiOptGeometry(uiVertices, pkVertex, uiNumBones, ppvBones, pvRootParent,
            pkInitXforms, kRootParentToSkin)
{
    m_uiTriListLength = uiTriListLength;
    m_puiTriList = puiTriList;
    m_bAllowLargeTriList = bAllowLargeTriList;
    m_bKeepDegenerates = bKeepDegenerates;

    if (bMakeTwoSided)
        MakeTwoSided();
}
//---------------------------------------------------------------------------
NiOptTriShape::~NiOptTriShape()
{
    NiFree(m_puiTriList);
}
//---------------------------------------------------------------------------
NiOptTriShape::ConvertError NiOptTriShape::ConvertTo(NiTriShape*& pkShape)
{
    if (m_uiNewVertices > 0)
    {
        // ConvertTo is intended to be called only once
        NIASSERT(false);
        pkShape = NULL;
        return OPT_DUPCALLERROR;
    }

    RemoveWeightsHavingZeroEffect();

    if ((m_uiNumBones == 0) && (!m_bKeepDegenerates))
    {
        // object does not have bones, safe to remove degenerate triangles
        RemoveDegenerateTriangles();
        if (m_uiTriListLength == 0)
        {
            pkShape = NULL; 
            return OPT_ALLDEGENERATES;
        }
    }

    RemoveDegenerateVertices();

    // current Gamebryo limitation
    //NIASSERT(m_uiNewVertices < ms_uiMaxUnsignedShort);
    if (m_uiNewVertices >= ms_uiMaxUnsignedShort)
    {
        pkShape = NULL;
        return OPT_VERTLIMITERROR;
    }

    CreateNiTriShape(pkShape);

    // failed to create the tri shape
    if (pkShape == NULL)
        return OPT_TRISHAPEFAILED;

    IdentifySharedNormals(pkShape);

    if (m_uiNumBones > 0)
        CreateNiSkinInstance(pkShape);

    return OPT_OK;
}
//---------------------------------------------------------------------------
void NiOptTriShape::IdentifySharedNormals(NiTriShape* pkShape)
{
    unsigned short usNumVerts = pkShape->GetVertexCount();
    NiPoint3* pkVertices = pkShape->GetVertices();
    NiPoint3* pkNormals = pkShape->GetNormals();
    for (unsigned int i = 0; i < usNumVerts; i++)
    {
        NiPoint3 kVert1 = pkVertices[i];
        NiPoint3 kNorm1 = pkNormals[i];
        for (unsigned int j = i + 1; j < usNumVerts; j++)
        {
            NiPoint3 kVert2 = pkVertices[j];
            NiPoint3 kNorm2 = pkNormals[j];
            if (kVert1 == kVert2 && kNorm1 == kNorm2)
            {
                NiTriShapeData* pkData =
                    NiDynamicCast(NiTriShapeData, pkShape->GetModelData());
                NIASSERT(pkData);
                pkData->LetVerticesShareNormal(i, j);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiOptTriShape::RemoveDegenerateTriangles()
{
    // remove degenerate triangles and repack connectivity array in-place
    unsigned int uiNewTriangles = 0;
    for (unsigned int uiT0 = 0, uiT1 = 0; uiT0 < m_uiTriListLength; uiT0 += 3)
    {
        // test for degenerate connectivity
        unsigned int uiI0 = m_puiTriList[uiT0];
        unsigned int uiI1 = m_puiTriList[uiT0 + 1];
        unsigned int uiI2 = m_puiTriList[uiT0 + 2];
        const NiPoint3& kP0 = m_pkVertex[uiI0].m_kPosition;
        const NiPoint3& kP1 = m_pkVertex[uiI1].m_kPosition;
        const NiPoint3& kP2 = m_pkVertex[uiI2].m_kPosition;

        if (NiOptimize::NonDegenerateTriangle(uiI0, uiI1, uiI2, kP0, kP1, kP2))
        {
            if (uiT1 != uiT0)
            {
                m_puiTriList[uiT1] = m_puiTriList[uiT0];
                m_puiTriList[uiT1 + 1] = m_puiTriList[uiT0 + 1];
                m_puiTriList[uiT1 + 2] = m_puiTriList[uiT0 + 2];
            }

            uiNewTriangles++;
            uiT1 += 3;
        }
    }

    m_uiTriListLength = 3 * uiNewTriangles;
}
//---------------------------------------------------------------------------
void NiOptTriShape::RemoveDegenerateVertices()
{
    // Quicksort of vertices.  The important feature is that indices of
    // vertices that are exactly equal occur in consecutive locations in
    // the sorted index array.  This allows for a linear pass over the
    // indices to identify and remove redundant vertices.
    unsigned int* puiIndex0;
    unsigned int* puiIndex1;
    IndexSort(puiIndex0, puiIndex1);

    // puiIndex0[i] is now the index of the i-th largest vert

    // Compress index array by removing indices for redundant vertices.
    unsigned int uiV;
    for (uiV = 1; uiV < m_uiVertices; uiV++)
    {
        if (VertexCompare(puiIndex0[uiV], puiIndex0[uiV - 1]) == 0)
        {
            puiIndex0[uiV] = puiIndex0[uiV - 1];
        }
    }

    // puiIndex0[i] is now the index of the i-th largest vert w/ no duplicate
    // verts

    // Remap the connectivity list to match the sorted vertices.
    unsigned int i;
    for (i = 0; i < m_uiTriListLength; i++)
    {
        m_puiTriList[i] = puiIndex0[puiIndex1[m_puiTriList[i]]];
    }

    NiFree(puiIndex0);
    NiFree(puiIndex1);

    // m_puiTriList now indexes into m_pVertex w/ no duplicates

    // Map old vertex indices to NiNew ones. The number uiMaxUnsignedInt is
    // considered to be an invalid index.
    m_puiOldToNew = NiAlloc(unsigned int, m_uiVertices);
    memset(m_puiOldToNew, 0xFF, m_uiVertices * sizeof(m_puiOldToNew[0]));

    m_uiNewVertices = 0;
    for (i = 0; i < m_uiTriListLength; i++)
    {
        unsigned int uiOldIndex = m_puiTriList[i];
        if (m_puiOldToNew[uiOldIndex] == ms_uiMaxUnsignedInt)
        {
            m_puiOldToNew[uiOldIndex] = m_uiNewVertices++;
        }

        m_puiTriList[i] = m_puiOldToNew[uiOldIndex];
    }

    // m_puiOldToNew now contains where each vertex will be placed by
    // CreateNiTriShape.
    // m_puiTriList now indexes into the vertex list that will be created

    // compute mapping of NiNew vertex indices to old ones
    m_puiNewToOld = NiAlloc(unsigned int, m_uiNewVertices);
    for (uiV = 0; uiV < m_uiVertices; uiV++)
    {
        if (m_puiOldToNew[uiV] != ms_uiMaxUnsignedInt)
        {
            m_puiNewToOld[m_puiOldToNew[uiV]] = uiV;
        }
    }
}
//---------------------------------------------------------------------------
void NiOptTriShape::CreateNiTriShape(NiTriShape*& pkShape)
{
    if (NiOptVertex::MAX_UV <= m_pkVertex[0].m_uiNumUVs)
    {
        pkShape = NULL;
        return;
    }

    NIASSERT(m_pkVertex[0].m_kPosition.x != NI_INFINITY);
    NiPoint3* pkVertex = NiNew NiPoint3[m_uiNewVertices];

    NiPoint3* pkNormal;
    if (m_pkVertex[0].m_kNormal.x != NI_INFINITY)
        pkNormal = NiNew NiPoint3[m_uiNewVertices];
    else
        pkNormal = NULL;

    NiColorA* pkColor;
    if (m_pkVertex[0].m_kColor.r != NI_INFINITY)
        pkColor = NiNew NiColorA[m_uiNewVertices];
    else
        pkColor = NULL;

    NiPoint2* apkTexture[NiOptVertex::MAX_UV];
    unsigned int uiNumSets = m_pkVertex[0].m_uiNumUVs;
    unsigned int uiSet;
    for (uiSet = 0; uiSet < uiNumSets; uiSet++)
    {
        apkTexture[uiSet] = NiNew NiPoint2[m_uiNewVertices];
    }
    for (uiSet = uiNumSets; uiSet < NiOptVertex::MAX_UV; uiSet++)
    {
        apkTexture[uiSet] = NULL;
    }

    for (unsigned int uiV0 = 0; uiV0 < m_uiVertices; uiV0++)
    {
        unsigned int uiV1 = m_puiOldToNew[uiV0];
        if (uiV1 != ms_uiMaxUnsignedInt)
        {
            const NiOptVertex& sv = m_pkVertex[uiV0];

            pkVertex[uiV1] = sv.m_kPosition;

            if (pkNormal)
                pkNormal[uiV1] = sv.m_kNormal;

            if (pkColor)
                pkColor[uiV1] = sv.m_kColor;

            for (uiSet = 0; uiSet < uiNumSets; uiSet++)
            {
                apkTexture[uiSet][uiV1] = sv.m_akUV[uiSet];
            }
        }
    }

    // compute length of NiNew connectivity list
    unsigned int uiNewTriListLength = 0;
    unsigned int i;
    for (i = 0; i < m_uiTriListLength; i++)
    {
        if (m_puiTriList[i] != ms_uiMaxUnsignedInt)
            uiNewTriListLength++;
        else
            break;
    }

#ifdef _DEBUG
    for (/**/; i < m_uiTriListLength; i++)
    {
        // m_pTriList entries are (may be) implicitly set to (~0) in
        // RemoveDegenerateVertices by the assignment from m_pOldToNew
        // entries.
        NIASSERT(m_puiTriList[i] == ms_uiMaxUnsignedInt);
    }
#endif

    NIASSERT(uiNewTriListLength % 3 == 0);

    if (!m_bAllowLargeTriList)
    {
        // DirectX7 function DrawIndexedPrimitiveVB limits the number of
        // indices in the connectivity array to max(unsigned short).
        NIASSERT(uiNewTriListLength < ms_uiMaxUnsignedShort);
        if (uiNewTriListLength >= ms_uiMaxUnsignedShort)
        {
            NiDelete[] pkVertex;
            NiDelete[] pkNormal;
            NiDelete[] pkColor;
            for (uiSet = 0; uiSet < uiNumSets; uiSet++)
            {
                NiDelete[] apkTexture[uiSet];
            }
            
            pkShape = NULL;
            return;
        }
    }

    unsigned short* pusTriList = NiAlloc(unsigned short,
        uiNewTriListLength);
    for (i = 0; i < uiNewTriListLength; i++)
    {
        pusTriList[i] = (unsigned short) m_puiTriList[i];
    }

    NIASSERT((uiNewTriListLength / 3) < ms_uiMaxUnsignedShort);

    unsigned short usInitialUVs = 0;
    if (apkTexture[0] != NULL)
    {
        usInitialUVs = 1;
    }

    pkShape = NiNew NiTriShape(m_uiNewVertices, pkVertex, pkNormal, pkColor,
        apkTexture[0], usInitialUVs, NiGeometryData::NBT_METHOD_NONE, 
        (unsigned short) (uiNewTriListLength / 3), pusTriList);

    for (i = 1; i < uiNumSets; i++)
    {
        pkShape->AppendTextureSet(apkTexture[i]);
        NiDelete[] apkTexture[i];
    }
}
//---------------------------------------------------------------------------
void NiOptTriShape::MakeTwoSided()
{
    NiOptVertex* pkNewVerts;
    NiOptVertex* pkNewVertsBorder;
    unsigned int* puiNewTriList;
    unsigned int i, uiNewVerts, uiNewListLength;

    unsigned int uiSize;

    // If has normals, duplicate vertices, duplicate and re-order
    // connectivity array, flip normals.
    // If no normals, just duplicate and re-order connectivity array.
    if (m_pkVertex[0].m_kNormal != NiPoint3(NI_INFINITY, NI_INFINITY,
        NI_INFINITY))
    {
        uiNewVerts = m_uiVertices * 2;
        pkNewVerts = NiNew NiOptVertex[uiNewVerts];

        // Memcpy the data.
        uiSize = m_uiVertices * sizeof(m_pkVertex[0]);
        NiMemcpy(pkNewVerts, m_pkVertex, uiSize);
        pkNewVertsBorder = &pkNewVerts[m_uiVertices];

        uiSize = m_uiVertices * sizeof(m_pkVertex[0]);
        NiMemcpy(pkNewVertsBorder, m_pkVertex, uiSize);

        // Duplicate and assign bone arrays.
        for (i = 0; i < m_uiVertices; i++)
        {
            if (pkNewVerts[i].m_uiNumBones > 0)
            {
                unsigned int* puiNewBoneIndex =
                    NiAlloc(unsigned int, pkNewVerts[i].m_uiNumBones);

                float* pfNewWeight = NiAlloc(float, 
                    pkNewVerts[i].m_uiNumBones);

                uiSize = pkNewVerts[i].m_uiNumBones *
                    sizeof(pkNewVerts[i].m_puiBoneIndex[0]);
                NiMemcpy(puiNewBoneIndex, pkNewVerts[i].m_puiBoneIndex,
                    uiSize);

                uiSize = pkNewVerts[i].m_uiNumBones *
                    sizeof(pkNewVerts[i].m_pfWeight[0]);
                NiMemcpy(pfNewWeight, pkNewVerts[i].m_pfWeight, uiSize);

                pkNewVerts[i + m_uiVertices].m_puiBoneIndex = puiNewBoneIndex;
                pkNewVerts[i + m_uiVertices].m_pfWeight = pfNewWeight;
            }
            pkNewVerts[i + m_uiVertices].m_kNormal =
                -pkNewVerts[i + m_uiVertices].m_kNormal;
        }

        // Duplicate and re-order connectivity array.
        uiNewListLength = m_uiTriListLength * 2;
        puiNewTriList = NiAlloc(unsigned int, uiNewListLength);

        uiSize = m_uiTriListLength * sizeof(m_puiTriList[0]);
        NiMemcpy(puiNewTriList, m_puiTriList, uiSize);
        for (i = 0; i < m_uiTriListLength; i += 3)
        {
            puiNewTriList[i + m_uiTriListLength] = m_uiVertices +
                puiNewTriList[i + 2];
            puiNewTriList[i + 1 + m_uiTriListLength] = m_uiVertices +
                puiNewTriList[i + 1];
            puiNewTriList[i + 2 + m_uiTriListLength] = m_uiVertices +
                puiNewTriList[i];
        }

        // Delete original TriList array and assign pointer to NiNew array.
        NiFree(m_puiTriList);
        m_puiTriList = puiNewTriList;
        m_uiTriListLength = uiNewListLength;

        // Delete original vertex array and assign pointer to NiNew array.
        NiDelete[] m_pkVertex;
        m_pkVertex = pkNewVerts;
        m_uiVertices = uiNewVerts;
    }
    else
    {
        // Duplicate and re-order connectivity array.
        uiNewListLength = m_uiTriListLength * 2;
        puiNewTriList = NiAlloc(unsigned int, uiNewListLength);
        
        uiSize = m_uiTriListLength * sizeof(m_puiTriList[0]);
        NiMemcpy(puiNewTriList, m_puiTriList, uiSize);
        for (i = 0; i < m_uiTriListLength; i += 3)
        {
            puiNewTriList[i + m_uiTriListLength] = puiNewTriList[i + 2];
            puiNewTriList[i + 1 + m_uiTriListLength] = puiNewTriList[i + 1];
            puiNewTriList[i + 2 + m_uiTriListLength] = puiNewTriList[i];
        }

        // Delete original array and assign pointer to NiNew array.
        NiFree(m_puiTriList);
        m_puiTriList = puiNewTriList;
        m_uiTriListLength = uiNewListLength;
    }
}
//---------------------------------------------------------------------------