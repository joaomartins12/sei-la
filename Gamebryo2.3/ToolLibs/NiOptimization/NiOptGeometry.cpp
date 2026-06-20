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

#include "NiOptGeometry.h"

const unsigned int NiOptGeometry::ms_uiMaxUnsignedShort =
    (unsigned int)((unsigned short)(~0));

const unsigned int NiOptGeometry::ms_uiMaxUnsignedInt = (unsigned int)(~0);
float NiOptGeometry::ms_fMinSkinVertexWeight = 0.01f;

//---------------------------------------------------------------------------
NiOptGeometry::NiOptGeometry(unsigned int uiVertices, NiOptVertex* pkVertex,
    unsigned int uiNumBones, void** ppvBones, void *pvRootParent,
    NiTransform* pkInitXforms, NiTransform &kRootParentToSkin)
{
    // ASSERT:
    // (1) The validity of a member of m_pkVertex[i] is the same as the
    //     validity of the same member of m_pkVertex[0].  For example, if
    //     m_pkVertex[0].m_kNormal = (NI_INFINITY,NI_INFINITY,NI_INFINITY),
    //     then
    //     all m_pkVertex[i].m_kNormal = (NI_INFINITY,NI_INFINITY,NI_INFINITY).
    //     The same argument holds for m_kPosition and m_kColor members.
    // (2) m_pkVertex[i].m_uiNumUVs = m_pkVertex[0].m_uiNumUVs for all i
    // (3) m_pkVertex[i].m_uiNumBones can vary with i
    // (4) m_pkVertex[i].m_uiNumBones <= m_uiNumBones for all i
    // (5) The pointer members m_pBone, and m_pfWeight are compared
    //     as pointers for equality (shallow compare).

    m_uiVertices = uiVertices;
    m_pkVertex = pkVertex;

    m_uiNumBones = uiNumBones;
    m_ppvBones = ppvBones;
    m_pvRootParent = pvRootParent;
    m_pkInitXforms = pkInitXforms;
    m_kRootParentToSkin = kRootParentToSkin;

    m_uiNewVertices = 0;
    m_puiNewToOld = m_puiOldToNew = NULL;
}
//---------------------------------------------------------------------------
NiOptGeometry::~NiOptGeometry()
{
    for (unsigned int i = 0; i < m_uiVertices; i++)
    {
        NiFree(m_pkVertex[i].m_puiBoneIndex);
        NiFree(m_pkVertex[i].m_pfWeight);
    }
    
    NiDelete[] m_pkVertex;
    NiFree(m_puiNewToOld);
    NiFree(m_puiOldToNew);
    NiDelete[] m_pkInitXforms;
}
//---------------------------------------------------------------------------
void NiOptGeometry::RemoveWeightsHavingZeroEffect()
{
    if (m_uiNumBones <= 0)
        return;

    unsigned int uiBone;

    // Remove weights less than ms_fMinSkinVertexWeight
    unsigned int uiVertex;
    for (uiVertex = 0; uiVertex < m_uiVertices; uiVertex++)
    {
        NiOptVertex* pkVert = &m_pkVertex[uiVertex];
        
        // we could sort these bones by weight
        // which would make removing the smallest weight a
        // constant-time operation, but for now we'll just
        // assume that the number of bones per vertex 
        // is relatively small

        // remove weights less than ms_fMinSkinVertexWeight
        unsigned int uiNumBonesRemoved = 0;
        for (uiBone = 0; uiBone < pkVert->m_uiNumBones; uiBone++)
        {
            // remove smallest weight less than ms_fMinSkinVertexWeight
            // find smallest weight
            unsigned int uiSmallestBoneWeight = uiNumBonesRemoved;
            unsigned int uiBone2;
            for (uiBone2 = uiNumBonesRemoved + 1;
                uiBone2 < pkVert->m_uiNumBones; uiBone2++)
            {
                if (pkVert->m_pfWeight[uiBone2] < 
                    pkVert->m_pfWeight[uiSmallestBoneWeight])
                {
                    uiSmallestBoneWeight = uiBone2;
                }
            }
            // check threshold
            if (pkVert->m_pfWeight[uiSmallestBoneWeight] < 
                ms_fMinSkinVertexWeight)
            {
                // throw away this bone
                pkVert->m_pfWeight[uiSmallestBoneWeight] = 
                    pkVert->m_pfWeight[uiNumBonesRemoved];
                pkVert->m_puiBoneIndex[uiSmallestBoneWeight] = 
                    pkVert->m_puiBoneIndex[uiNumBonesRemoved];

                uiNumBonesRemoved++;
            }
            else
            {
                // no need to continue searching
                // or renormalize if we didn't remove
                // any bones
                break;
            }

            // renormalize remainder of weights
            float fWeightSum = 0.0f;
            for (uiBone2 = uiNumBonesRemoved; 
                uiBone2 < pkVert->m_uiNumBones; uiBone2++)
            {
                fWeightSum += pkVert->m_pfWeight[uiBone2];
            }
            if (fWeightSum != 0.0f)
            {
                float fScale = 1.0f / fWeightSum;
                for (uiBone2 = uiNumBonesRemoved; 
                    uiBone2 < pkVert->m_uiNumBones; uiBone2++)
                {
                    pkVert->m_pfWeight[uiBone2] *= fScale;
                }
            }
        }

        if (uiNumBonesRemoved > 0)
        {
            unsigned int uiNewNumBones = pkVert->m_uiNumBones - 
                uiNumBonesRemoved;
            float* pfWeight;
            unsigned int* puiBoneIndices;
            if (uiNewNumBones > 0)
            {
                // resize bone arrays
                pfWeight = NiAlloc(float, uiNewNumBones);
                puiBoneIndices = NiAlloc(unsigned int, uiNewNumBones);

                for(uiBone=uiNumBonesRemoved; uiBone < pkVert->m_uiNumBones; 
                    uiBone++)
                {
                    unsigned int uiNewIndex = uiBone-uiNumBonesRemoved;
                    pfWeight[uiNewIndex] = pkVert->m_pfWeight[uiBone];
                    puiBoneIndices[uiNewIndex] = 
                        pkVert->m_puiBoneIndex[uiBone];
                }
            }
            else
            {
                // this vertex had all its bones removed
                pfWeight = NULL;
                puiBoneIndices = NULL;
            }
            NiFree(pkVert->m_puiBoneIndex);
            NiFree(pkVert->m_pfWeight);
            pkVert->m_uiNumBones = uiNewNumBones;
            pkVert->m_pfWeight = pfWeight;
            pkVert->m_puiBoneIndex = puiBoneIndices;
        }
    }
    // After removing bone references, it is possible that there are
    // bones that have zero references.  We will now remove those
    // reference pointers from m_ppvBones and then cycle through the
    // vertices and update their bone references.

    unsigned int uiTaggedBones = 0;

    bool* pbUsed = NiAlloc(bool, m_uiNumBones);
    memset(pbUsed, 0, sizeof(bool) * m_uiNumBones);

    for (uiVertex = 0; uiVertex < m_uiVertices; uiVertex++)
    {
        NiOptVertex* pkVert = &m_pkVertex[uiVertex];

        unsigned int uiNumBones = pkVert->m_uiNumBones;

        // Tag used bones
        for (uiBone = 0; uiBone< uiNumBones; uiBone++)
        {
            if (pbUsed[pkVert->m_puiBoneIndex[uiBone]] == false)
            {
                pbUsed[pkVert->m_puiBoneIndex[uiBone]] = true;
                uiTaggedBones++;
            }
        }
    }

    if (uiTaggedBones == m_uiNumBones)
    {
        NiFree(pbUsed);
        return;
    }

    NIASSERT(uiTaggedBones < m_uiNumBones);

    int* piRefMods = NiAlloc(int, m_uiNumBones);
    memset(piRefMods, -1, sizeof(int) * m_uiNumBones);

    void** ppvBones = NiAlloc(void*, uiTaggedBones);
    NiTransform* pkInitXforms = NiNew NiTransform[uiTaggedBones];

    // Build a reference map as well as new bone,xform lists
    unsigned int uiRefCnt = 0;
    for (uiBone = 0; uiBone < m_uiNumBones; uiBone++)
    {
        if (pbUsed[uiBone])
        {
            NIASSERT(uiRefCnt < uiTaggedBones);
            ppvBones[uiRefCnt] = m_ppvBones[uiBone];
            pkInitXforms[uiRefCnt] = m_pkInitXforms[uiBone];
            piRefMods[uiBone] = uiRefCnt;
            uiRefCnt++;
            continue;
        }
    }

    // Modify the references in the vertices using the map
    for (uiVertex = 0; uiVertex < m_uiVertices; uiVertex++)
    {
        NiOptVertex* pkVert = &m_pkVertex[uiVertex];

        unsigned int uiNumBones = pkVert->m_uiNumBones;
        unsigned int uiBone;

        // Tag used bones
        for (uiBone = 0; uiBone < uiNumBones; uiBone++)
        {
            pkVert->m_puiBoneIndex[uiBone] =
                piRefMods[pkVert->m_puiBoneIndex[uiBone]];
        }
    }

    NiFree(piRefMods);
    NiFree(pbUsed);


    NiFree(m_ppvBones);
    NiDelete [] m_pkInitXforms;

    m_ppvBones = ppvBones;
    m_pkInitXforms = pkInitXforms;
    m_uiNumBones = uiTaggedBones;
}
//---------------------------------------------------------------------------
void NiOptGeometry::IndexSort(unsigned int*& puiIndex0,
    unsigned int*& puiIndex1)
{
    // After a call to IndexSort, vertex[puiIndex0[i]] is the ith smallest
    // and puiIndex1[i] tells where the ith vertex ended up in the sorted 
    // array. Therefore puiIndex0[puiIndex1[i]] == i.  This function allocates
    // puiIndex0 and puiIndex1. It is the calling function's responsibility to
    // delete them.

    puiIndex0 = NiAlloc(unsigned int, m_uiVertices);
    puiIndex1 = NiAlloc(unsigned int, m_uiVertices);

    unsigned int i;
    for (i = 0; i < m_uiVertices; i++)
    {
        puiIndex0[i] = i;
    }
    
    IndexQSort(puiIndex0, 0, m_uiVertices - 1);

    for (i = 0; i < m_uiVertices; i++)
    {
        puiIndex1[puiIndex0[i]] = i;
    }
}
//---------------------------------------------------------------------------
void NiOptGeometry::IndexQSort(unsigned int* puiIndex, int iL, int iR)
{
    if (iR > iL)
    {
        int iLower = iL - 1;
        int iUpper = iR + 1;
        int iPivot = ChoosePivot(puiIndex, iL, iR);

        while (true)
        {
            do 
            {
                iUpper--;
            }
            while (VertexCompare(iPivot, puiIndex[iUpper]) < 0);

            do
            {
                iLower++;
            }
            while (VertexCompare(puiIndex[iLower], iPivot) < 0);

            if (iLower < iUpper)
            {
                unsigned int uiTmp = puiIndex[iLower];
                puiIndex[iLower] = puiIndex[iUpper];
                puiIndex[iUpper] = uiTmp;
            }
            else
            {
                break;
            }
        }

        if (iUpper == iR)
        {
            IndexQSort(puiIndex, iL, iUpper - 1);
        }
        else
        {
            IndexQSort(puiIndex, iL, iUpper);
            IndexQSort(puiIndex, iUpper + 1, iR);
        }
    }
}
//---------------------------------------------------------------------------
int NiOptGeometry::ChoosePivot(unsigned int* puiIndex, int iL, int iR)
{
    // Check the first, middle, and last element. Choose the one which falls
    // between the other two. This has a good chance of discouraging 
    // quadratic behavior from qsort.  In the case when all three are equal,
    // this code chooses the middle element, which will prevent quadratic
    // behavior for a list with all elements equal.

    int iM = (iL + iR) >> 1;

    if (VertexCompare(puiIndex[iL], puiIndex[iM]) < 0)
    {
        if (VertexCompare(puiIndex[iM], puiIndex[iR]) < 0)
            return puiIndex[iM];
        else if (VertexCompare(puiIndex[iL], puiIndex[iR]) < 0)
            return puiIndex[iR];
        else
            return puiIndex[iL];
    }
    else
    {
        if (VertexCompare(puiIndex[iL], puiIndex[iR]) < 0)
            return puiIndex[iL];
        else if (VertexCompare(puiIndex[iM], puiIndex[iR]) < 0)
            return puiIndex[iR];
        else
            return puiIndex[iM];
    }
}
//---------------------------------------------------------------------------
void NiOptGeometry::CreateNiSkinInstance(NiAVObject* pkShape)
{
    NIASSERT(NiIsExactKindOf(NiTriShape, pkShape));

    // compute number of vertices per bone
    unsigned int* puiNumVertsPerBone = NiAlloc(unsigned int, m_uiNumBones);
    memset(puiNumVertsPerBone, 0, m_uiNumBones * sizeof(unsigned int));

    unsigned int uiV0, uiV1, uiB;
    for (uiV0 = 0; uiV0 < m_uiVertices; uiV0++)
    {
        uiV1 = m_puiOldToNew[uiV0];

        if (uiV1 != ms_uiMaxUnsignedInt)
        {
            const NiOptVertex& sv = m_pkVertex[uiV0];
            for (uiB = 0; uiB < sv.m_uiNumBones; uiB++)
            {
                puiNumVertsPerBone[sv.m_puiBoneIndex[uiB]]++;
            }
        }
    }
    
    // fill in bone data
    NiSkinData::BoneData* pkBoneData = NiNew 
        NiSkinData::BoneData[m_uiNumBones];

    for (uiB = 0; uiB < m_uiNumBones; uiB++)
    {
        NiSkinData::BoneData& kBoneData = pkBoneData[uiB];

        // initial transform matrix
        kBoneData.m_kSkinToBone = m_pkInitXforms[uiB];

        unsigned int uiNumVerts = puiNumVertsPerBone[uiB];
        kBoneData.m_pkBoneVertData = 
            NiNew NiSkinData::BoneVertData[uiNumVerts];
        kBoneData.m_usVerts = uiNumVerts;
    }

    // fill in the weights and indices
    unsigned int* puiCurVertsPerBone = NiAlloc(unsigned int, m_uiNumBones);
    memset(puiCurVertsPerBone, 0, m_uiNumBones * sizeof(unsigned int));
    for (uiV0 = 0; uiV0 < m_uiVertices; uiV0++)
    {
        uiV1 = m_puiOldToNew[uiV0];

        if (uiV1 != ms_uiMaxUnsignedInt)
        {
            const NiOptVertex& sv = m_pkVertex[uiV0];
            for (uiB = 0; uiB < sv.m_uiNumBones; uiB++)
            {
                unsigned int uiBIndex = sv.m_puiBoneIndex[uiB];
                unsigned int uiVIndex = puiCurVertsPerBone[uiBIndex];
                puiCurVertsPerBone[uiBIndex]++;
                pkBoneData[uiBIndex].m_pkBoneVertData[uiVIndex].m_fWeight =
                    sv.m_pfWeight[uiB];
                pkBoneData[uiBIndex].m_pkBoneVertData[uiVIndex].m_usVert =
                    uiV1;
            }
        }
    }

    NiFree(puiCurVertsPerBone);
    NiFree(puiNumVertsPerBone);

    NiPoint3* pkVertices = 
        ((NiTriShape*) pkShape)->GetModelData()->GetVertices();

    NiSkinData* pkSkinData = NiNew NiSkinData(m_uiNumBones, pkBoneData, 
        m_kRootParentToSkin, pkVertices);

    pkSkinData->SortAndMergeBoneData();

    NiSkinInstance* pkSkinInstance = NiNew NiSkinInstance(pkSkinData,
        (NiAVObject*) m_pvRootParent, (NiAVObject**) m_ppvBones);
    
    ((NiTriShape *) pkShape)->SetSkinInstance(pkSkinInstance);
}
//---------------------------------------------------------------------------
void NiOptGeometry::SetMinSkinVertexWeight(float fMinWeight)
{
    NIASSERT(fMinWeight >= 0.01f && fMinWeight <= 0.5f);
    if (fMinWeight >= 0.01f && fMinWeight <= 0.5f)
        ms_fMinSkinVertexWeight = fMinWeight;
}
//---------------------------------------------------------------------------
float NiOptGeometry::GetMinSkinVertexWeight()
{
    return ms_fMinSkinVertexWeight;
}
//---------------------------------------------------------------------------
