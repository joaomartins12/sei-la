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
#include "NiMainPCH.h"

#include "NiSkinPartition.h"

#include "NiBool.h"
#include "NiSkinData.h"
#include "NiTriShapeData.h"
#include "NiRenderer.h"

#include <NiSystem.h>

NiImplementRTTI(NiSkinPartition, NiObject);

//---------------------------------------------------------------------------
inline bool Degenerate(const unsigned short* pusIndices)
{
    // Do the indices define a degenerate triangle?
    return
        pusIndices[0] == pusIndices[1] ||
        pusIndices[1] == pusIndices[2] ||
        pusIndices[2] == pusIndices[0];
}

//---------------------------------------------------------------------------
class NiSkinPartition::Interaction : public NiMemObject
{
public:
    unsigned int m_uiBone;
    float m_fWeight;
};

//---------------------------------------------------------------------------
class NiSkinPartition::VertexInteractions : 
    public NiTPrimitiveSet<NiSkinPartition::Interaction>
{
public:
    unsigned int GetBone(unsigned int i) const;
    float GetWeight(unsigned int i) const;
    float FindWeight(unsigned int uiBone) const;
    bool RemoveBone(unsigned int uiBone);
    void Unitize();
    void SortByWeight();
    void Truncate(unsigned int uiBones);
    bool SameBones(const VertexInteractions& kI) const;
protected:
    static int CompareWeights(const void* pv0, const void* pv1);
};

//---------------------------------------------------------------------------
class NiSkinPartition::BoneSet : public NiUnsignedShortSet
{
public:
    bool Contains(const BoneSet& kThat) const;
    unsigned int MergedSize(const BoneSet& kThat) const;
    void Merge(const BoneSet& kThat);
    bool ContainsTriangle(const unsigned short* pusTriangle,
        const VertexInteractions* pkVertex) const;
    bool ContainsVertex(const VertexInteractions& kVertex) const;
    void Sort();
protected:
    static int CompareBones(const void* pv0, const void* pv1);
};

//---------------------------------------------------------------------------
class NiSkinPartition::BoneSetMerger : public NiMemObject
{
public:
    BoneSetMerger(const BoneSet& kSet0, const BoneSet& kSet1);
    bool Done() const;
    unsigned short GetNext();
protected:
    unsigned int m_uiIndex[2];
    unsigned short m_usEl[2];
    const BoneSet& m_kSet0;
    const BoneSet& m_kSet1;
};

//---------------------------------------------------------------------------
class NiSkinPartition::TriangleSet : public NiUnsignedShortSet
{
public:
    void AddTriangles(unsigned int uiPartition,
        const unsigned int* puiTriangleToPartition, unsigned int uiTriangles);
};

//---------------------------------------------------------------------------
// NiSkinPartition::VertexInteractions member functions

//---------------------------------------------------------------------------
inline unsigned int NiSkinPartition::VertexInteractions::GetBone(
    unsigned int i) const
{
    return GetAt(i).m_uiBone;
}

//---------------------------------------------------------------------------
inline float NiSkinPartition::VertexInteractions::GetWeight(unsigned int i) 
    const
{
    return GetAt(i).m_fWeight;
}

//---------------------------------------------------------------------------
float NiSkinPartition::VertexInteractions::FindWeight(unsigned int uiBone)
    const
{
    unsigned int b;

    for (b = 0; b < m_uiUsed; b++)
    {
        if (GetAt(b).m_uiBone == uiBone)
            return GetAt(b).m_fWeight;
    }

    return 0.0f;
}

//---------------------------------------------------------------------------
void NiSkinPartition::VertexInteractions::Unitize()
{
    unsigned int b;
    float fWeightSum = 0.0f;
    
    for (b = 0; b < m_uiUsed; b++)
    {
        fWeightSum += GetAt(b).m_fWeight;
    }

    NIASSERT(fWeightSum != 0.0f);
    
    float fScale = 1.0f / fWeightSum;
    
    for (b = 0; b < m_uiUsed; b++)
    {
        m_pBase[b].m_fWeight *= fScale;
    }
}

//---------------------------------------------------------------------------
bool NiSkinPartition::VertexInteractions::RemoveBone(unsigned int uiBone)
{
    unsigned int b;

    for (b = 0; b < m_uiUsed; b++)
    {
        if (GetAt(b).m_uiBone == uiBone)
        {
            // Bones are sorted. Do an ordered remove so that they stay sorted.
            OrderedRemoveAt(b);
            return true;
        }
    }

    return false;
}

//---------------------------------------------------------------------------
void NiSkinPartition::VertexInteractions::SortByWeight()
{
    qsort(m_pBase, GetSize(), sizeof(m_pBase[0]), CompareWeights);
}

//---------------------------------------------------------------------------
int NiSkinPartition::VertexInteractions::CompareWeights(const void* pv0,
    const void* pv1)
{
    // compare function used by qsort

    float fWeight0, fWeight1;

    fWeight0 = ((Interaction*) pv0)->m_fWeight;
    fWeight1 = ((Interaction*) pv1)->m_fWeight;

    // Compare such that larger weights will be first:

    if (fWeight0 > fWeight1)
        return -1;
    if (fWeight0 < fWeight1)
        return 1;
    return 0;
}

//---------------------------------------------------------------------------
void NiSkinPartition::VertexInteractions::Truncate(unsigned int uiBones)
{
    NIASSERT(uiBones < m_uiUsed);
    m_uiUsed = uiBones;
}

//---------------------------------------------------------------------------
bool NiSkinPartition::VertexInteractions::SameBones(
    const VertexInteractions& kI) const
{
    if (m_uiUsed != kI.m_uiUsed)
        return false;

    for (unsigned int b = 0; b < m_uiUsed; b++)
    {
#ifdef _DEBUG
        // Bones should be sorted in order of increasing index at this point
        if (b > 0)
        {
            NIASSERT(m_pBase[b - 1].m_uiBone < m_pBase[b].m_uiBone);
            NIASSERT(kI.m_pBase[b - 1].m_uiBone < kI.m_pBase[b].m_uiBone);
        }
#endif

        if (m_pBase[b].m_uiBone != kI.m_pBase[b].m_uiBone)
            return false;
    }

    return true;
}

//---------------------------------------------------------------------------
// NiSkinPartition::BoneSet member functions

//---------------------------------------------------------------------------
bool NiSkinPartition::BoneSet::Contains(const BoneSet& kThat) const
{
    // This function returns true if kThat is a subset of "this".
    // Assumes "this" and kThat have been sorted.

    unsigned int uiThis = 0, uiThat = 0;

    while (uiThat < kThat.GetSize())
    {
        while (uiThis < GetSize() && GetAt(uiThis) < kThat.GetAt(uiThat))
        {
            uiThis++;
        }

        if (uiThis == GetSize() || GetAt(uiThis) != kThat.GetAt(uiThat))
        {
            return false;
        }

        uiThis++;
        uiThat++;
    }

    return true;
}

//---------------------------------------------------------------------------
unsigned int NiSkinPartition::BoneSet::MergedSize(const BoneSet& kThat) const
{
    BoneSetMerger kMerger(*this, kThat);
    unsigned int uiMergeSize = 0;

    while (!kMerger.Done())
    {
        kMerger.GetNext();
        uiMergeSize++;
    }

    return uiMergeSize;
}

//---------------------------------------------------------------------------
void NiSkinPartition::BoneSet::Merge(const BoneSet& kThat)
{
    // Add all elements of kThat to "this".

    BoneSetMerger kMerger(*this, kThat);
    unsigned int uiMergeSize = 0;
    unsigned int uiNewAlloced = GetSize() + kThat.GetSize();
    unsigned short* pusNewBase = NiAlloc(unsigned short, uiNewAlloced);
    NIASSERT(pusNewBase != NULL);

    while (!kMerger.Done())
    {
        pusNewBase[uiMergeSize++] = kMerger.GetNext();
    }

    NiFree(m_pBase);
    m_pBase = pusNewBase;
    m_uiAlloced = uiNewAlloced;
    m_uiUsed = uiMergeSize;
}

//---------------------------------------------------------------------------
bool NiSkinPartition::BoneSet::ContainsTriangle(
    const unsigned short* pusTriangle, const VertexInteractions* pkVertex)
    const
{
    unsigned int v;

    for (v = 0; v < 3; v++)
    {
        if (!ContainsVertex(pkVertex[pusTriangle[v]]))
            return false;
    }

    return true;
}

//---------------------------------------------------------------------------
bool NiSkinPartition::BoneSet::ContainsVertex(
    const VertexInteractions& kVertex) const
{
    // "this" is assumed to be sorted.

    // In order to "contain" a vertex, the boneset must include every bone
    // that influences the vertex.

    unsigned int b, uiVertexBones = kVertex.GetSize();
    
    for (b = 0; b < uiVertexBones; b++)
    {
        unsigned short usBone = kVertex.GetBone(b);
        
        if (bsearch(&usBone, m_pBase, GetSize(), sizeof(m_pBase[0]),
            CompareBones) == NULL)
        {
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------
void NiSkinPartition::BoneSet::Sort()
{
    qsort(m_pBase, GetSize(), sizeof(m_pBase[0]), CompareBones);
}

//---------------------------------------------------------------------------
int NiSkinPartition::BoneSet::CompareBones(const void* pv0, const void* pv1)
{
    // compare function used by qsort and bsearch

    unsigned short usBone0, usBone1;

    usBone0 = *((unsigned short*) pv0);
    usBone1 = *((unsigned short*) pv1);

    if (usBone0 < usBone1)
        return -1;
    if (usBone0 > usBone1)
        return 1;
    return 0;
}

//---------------------------------------------------------------------------
// NiSkinPartition::BoneSetMerger member functions

//---------------------------------------------------------------------------
NiSkinPartition::BoneSetMerger::BoneSetMerger(const BoneSet& kSet0,
    const BoneSet& kSet1)
    : m_kSet0(kSet0), m_kSet1(kSet1)
{
    NIASSERT(kSet0.GetSize() > 0);
    NIASSERT(kSet1.GetSize() > 0);

    m_uiIndex[0] = m_uiIndex[1] = 0;
    m_usEl[0] = kSet0.GetAt(0);
    m_usEl[1] = kSet1.GetAt(0);

    // Both sets are assumed to have been sorted.
}

//---------------------------------------------------------------------------
bool NiSkinPartition::BoneSetMerger::Done() const
{
    return m_usEl[0] == USHRT_MAX && m_usEl[1] == USHRT_MAX;
}

//---------------------------------------------------------------------------
unsigned short NiSkinPartition::BoneSetMerger::GetNext()
{
    unsigned short usNext;

    if (m_usEl[1] < m_usEl[0])
    {
        usNext = m_usEl[1];

        m_usEl[1] = ++m_uiIndex[1] < m_kSet1.GetSize() ? 
            m_kSet1.GetAt(m_uiIndex[1]) : USHRT_MAX;

        NIASSERT(usNext < m_usEl[1]);
    }
    else 
    {
        usNext = m_usEl[0];

        if (m_usEl[1] == m_usEl[0])
        {
            m_usEl[1] = ++m_uiIndex[1] < m_kSet1.GetSize() ? 
                m_kSet1.GetAt(m_uiIndex[1]) : USHRT_MAX;
            
            NIASSERT(usNext < m_usEl[1]);
        }
        
        m_usEl[0] = (++m_uiIndex[0] < m_kSet0.GetSize()) ? 
            m_kSet0.GetAt(m_uiIndex[0]) : USHRT_MAX;

        NIASSERT(usNext < m_usEl[0]);
    }

    return usNext;
}

//---------------------------------------------------------------------------
// NiSkinPartition::TriangleSet member functions

//---------------------------------------------------------------------------
void NiSkinPartition::TriangleSet::AddTriangles(unsigned int uiPartition,
    const unsigned int* puiTriangleToPartition, unsigned int uiTriangles)
{
    for (unsigned int t = 0; t < uiTriangles; t++)
    {
        if (puiTriangleToPartition[t] == uiPartition)
        {
            Add(t);
        }
    }
}

//---------------------------------------------------------------------------
// NiSkinPartition::Partition member functions

//---------------------------------------------------------------------------
NiSkinPartition::Partition::Partition()
{
    m_pusBones = NULL;
    m_pfWeights = NULL;
    m_pusVertexMap = NULL;
    m_pucBonePalette = NULL;
    m_pusTriList = NULL;
    m_pusStripLengths = NULL;
    m_usStrips = 0;
    m_pkBuffData = NULL;
}

//---------------------------------------------------------------------------
NiSkinPartition::Partition::~Partition()
{
    NiFree(m_pusBones);
    NiFree(m_pfWeights);
    NiFree(m_pusVertexMap);
    NiFree(m_pucBonePalette);
    NiFree(m_pusTriList);
    NiFree(m_pusStripLengths);
}

//---------------------------------------------------------------------------
void NiSkinPartition::Partition::PostPrecache()
{
    // Delete everything except strip lengths and bone indices.

    NiFree(m_pfWeights);
    m_pfWeights = NULL;
    NiFree(m_pusVertexMap);
    m_pusVertexMap = NULL;
    NiFree(m_pucBonePalette);
    m_pucBonePalette = NULL;
    NiFree(m_pusTriList);
    m_pusTriList = NULL;
}

//---------------------------------------------------------------------------
void NiSkinPartition::Partition::Create(const TriangleSet& kTriangles,
    const BoneSet& kBones, const NiTriBasedGeomData* pkTriData,
    const VertexInteractions* pkVertexInteraction, bool bNeedPalette,
    bool bForcePalette, unsigned char ucBonesPerVertex,
    unsigned int uiMaxBones)
{
    unsigned int v, b;

    m_usBones = NiMax(kBones.GetSize(), ucBonesPerVertex);
    m_usTriangles = kTriangles.GetSize();

    m_pusBones = NiAlloc(unsigned short,m_usBones);
    NIASSERT(m_pusBones != NULL);

    for (b = 0; b < m_usBones; b++)
    {
        m_pusBones[b] = (b < kBones.GetSize()) ? kBones.GetAt(b) : 0;
    }

    unsigned short* pusVertexPartitionIndex = 
        CreateVertexMap(kTriangles, pkTriData);
    CreateTriangleList(kTriangles, pkTriData, pusVertexPartitionIndex);
    NiFree(pusVertexPartitionIndex);

    if (bNeedPalette)
    {
        m_usBonesPerVertex = ucBonesPerVertex;
        unsigned int uiInteractions = m_usBonesPerVertex * m_usVertices;
        m_pfWeights = NiAlloc(float,uiInteractions);
        NIASSERT(m_pfWeights != NULL);
        m_pucBonePalette = NiAlloc(unsigned char,uiInteractions);
        NIASSERT(m_pucBonePalette != NULL);

        // puiBonePartitionIndex maps skindata bone indices to partition
        // indices.
        unsigned int* puiBonePartitionIndex = NiAlloc(unsigned int,uiMaxBones);

        for (b = 0; b < kBones.GetSize(); b++)
        {
            puiBonePartitionIndex[m_pusBones[b]] = b;
        }

        float* pfWeight = m_pfWeights;
        unsigned char* pucBonePalette = m_pucBonePalette;

        float* pfTempWeightArray = NiAlloc(float,m_usBonesPerVertex);
        unsigned char* pucTempIndexArray = 
            NiAlloc(unsigned char,m_usBonesPerVertex);
        for (v = 0; v < m_usVertices; v++)
        {
            const VertexInteractions* pkVertex =
                &pkVertexInteraction[m_pusVertexMap[v]];
            
            float* pfTempWeight = pfTempWeightArray;
            unsigned char* pucTempIndex = pucTempIndexArray;
            for (b = 0; b < pkVertex->GetSize(); b++)
            {
                const Interaction& kInteraction = pkVertex->GetAt(b);
                *pfTempWeight++ = kInteraction.m_fWeight;
                *pucTempIndex++ = 
                    puiBonePartitionIndex[kInteraction.m_uiBone];
            }
            
            for ( ; b < m_usBonesPerVertex; b++)
            {
                *pfTempWeight++ = 0.0f;
                *pucTempIndex++ = 0;
            }

            // Sort bone weights using an insertion sort
            float fTemp;
            unsigned int uiIndex, j;
            for (b = 0; b < m_usBonesPerVertex; b++)
            {
                fTemp = pfTempWeightArray[0];
                uiIndex = 0;
                for (j = 1; j < m_usBonesPerVertex; j++)
                {
                    if (pfTempWeightArray[j] > fTemp)
                    {
                        fTemp = pfTempWeightArray[j];
                        uiIndex = j;
                    }
                }
                *pfWeight++ = pfTempWeightArray[uiIndex];
                *pucBonePalette++ = pucTempIndexArray[uiIndex];
                pfTempWeightArray[uiIndex] = -FLT_MAX;
            }
        }

        NiFree(pfTempWeightArray);
        NiFree(pucTempIndexArray);
        NiFree(puiBonePartitionIndex);
    }
    else
    {
        m_usBonesPerVertex = m_usBones;
        unsigned int uiInteractions = m_usBonesPerVertex * m_usVertices;
        m_pfWeights = NiAlloc(float,uiInteractions);
        NIASSERT(m_pfWeights != NULL);
        
        float* pfWeight = m_pfWeights;

        for (v = 0; v < m_usVertices; v++)
        {
            const VertexInteractions* pkVertex =
                &pkVertexInteraction[m_pusVertexMap[v]];
            
            for (b = 0; b < kBones.GetSize(); b++)
            {
                *pfWeight++ = pkVertex->FindWeight(kBones.GetAt(b));
            }

            for ( ; b < m_usBones; b++)
            {
                *pfWeight++ = 0.0f;
            }
        }

        if (bForcePalette)
        {
            m_pucBonePalette = NiAlloc(unsigned char,uiInteractions);
            NIASSERT(m_pucBonePalette != NULL);
            unsigned char* pucBonePalette = m_pucBonePalette;

            for (v = 0; v < m_usVertices; v++)
            {
                for (b = 0; b < m_usBones; b++)
                {
                    *pucBonePalette++ = b;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
unsigned short* NiSkinPartition::Partition::CreateVertexMap(
    const TriangleSet& kTriangles, const NiTriBasedGeomData* pkTriData)
{
    // pusPartitionIndex maps original TriShape indices to partition indices.
    // m_pusVertexMap maps partition indices to original TriShape indices.

    unsigned int uiVertices = pkTriData->GetVertexCount();
    unsigned short* pusVertexPartitionIndex = NiAlloc(unsigned short, 
        uiVertices);
    NIASSERT(pusVertexPartitionIndex != NULL);
    unsigned int v;

    // Vertices not in the partition will map to USHRT_MAX, so initialize all
    // entries to that value.
    for (v = 0; v < uiVertices; v++)
    {
        pusVertexPartitionIndex[v] = USHRT_MAX;
    }

    unsigned int uiPartitionVertices = 0;

    // Assign partition indices to vertices in the partition triangles.
    for (unsigned int t = 0; t < m_usTriangles; t++)
    {
        unsigned short usTriangle = kTriangles.GetAt(t);
        unsigned short ausTriangle[3];

        pkTriData->GetTriangleIndices(usTriangle, ausTriangle[0], 
            ausTriangle[1], ausTriangle[2]);

        for (v = 0; v < 3; v++)
        {
            unsigned int uiVert = ausTriangle[v];
            if (pusVertexPartitionIndex[uiVert] == USHRT_MAX)
            {
                pusVertexPartitionIndex[uiVert] = uiPartitionVertices++;
            }
        }
    }

    m_pusVertexMap = NiAlloc(unsigned short, uiPartitionVertices);
    NIASSERT(m_pusVertexMap != NULL);

    m_usVertices = uiPartitionVertices;

    for (v = 0; v < uiVertices; v++)
    {
        unsigned short usPartitionVert = pusVertexPartitionIndex[v];
        if (usPartitionVert != USHRT_MAX)
        {
            m_pusVertexMap[usPartitionVert] = v;
        }
    }
    
    return pusVertexPartitionIndex;
}

//---------------------------------------------------------------------------
void NiSkinPartition::Partition::CreateTriangleList(
    const TriangleSet& kTriangles, const NiTriBasedGeomData* pkTriData,
    const unsigned short* pusVertexPartitionIndex)
{
    // pusVertexPartitionIndex maps TriShape indices to partition indices.
    // Use it to create a TriList for the partition that refers to partition
    // indices.

    m_pusTriList = NiAlloc(unsigned short, 3 * m_usTriangles);
    NIASSERT(m_pusTriList != NULL);

    for (unsigned int t = 0; t < m_usTriangles; t++)
    {
        unsigned short usTriangle = kTriangles.GetAt(t);
        unsigned short ausTriangle[3];

        pkTriData->GetTriangleIndices(usTriangle, ausTriangle[0], 
            ausTriangle[1], ausTriangle[2]);

        unsigned short* pusPartitionTriangle = &m_pusTriList[3 * t];

        for (unsigned int v = 0; v < 3; v++)
        {
            pusPartitionTriangle[v] =
                pusVertexPartitionIndex[ausTriangle[v]];
        }
    }
}

//---------------------------------------------------------------------------
unsigned int NiSkinPartition::Partition::GetStripLengthSum() const
{
    unsigned int uiStripLengthSum = 0;

    for (unsigned int i = 0; i < m_usStrips; i++)
    {
        uiStripLengthSum += m_pusStripLengths[i];
    }

    return uiStripLengthSum;
}

//---------------------------------------------------------------------------
void NiSkinPartition::Partition::LoadBinary(NiStream& kStream)
{
    NiStreamLoadBinary(kStream, m_usVertices);
    NiStreamLoadBinary(kStream, m_usTriangles);
    NiStreamLoadBinary(kStream, m_usBones);
    NiStreamLoadBinary(kStream, m_usStrips);
    NiStreamLoadBinary(kStream, m_usBonesPerVertex);
    
    m_pusBones = NiAlloc(unsigned short, m_usBones);
    NiStreamLoadBinary(kStream, m_pusBones, m_usBones);
    
    NiBool bHasMap;
    NiStreamLoadBinary(kStream, bHasMap);
    
    if (bHasMap)
    {
        m_pusVertexMap = NiAlloc(unsigned short, m_usVertices);
        NiStreamLoadBinary(kStream, m_pusVertexMap, m_usVertices);
    }
    
    unsigned int uiInteractions = m_usBonesPerVertex * m_usVertices;

    NiBool bHasWeights;
    NiStreamLoadBinary(kStream, bHasWeights);

    if (bHasWeights)
    {
        m_pfWeights = NiAlloc(float, uiInteractions);
        NiStreamLoadBinary(kStream, m_pfWeights, uiInteractions);
    }

    unsigned int uiListLen;

    if (m_usStrips == 0)
    {
        uiListLen = 3 * m_usTriangles;
    }
    else
    {
        m_pusStripLengths = NiAlloc(unsigned short, m_usStrips);
        NiStreamLoadBinary(kStream, m_pusStripLengths, m_usStrips);
        uiListLen = GetStripLengthSum();
    }

    NiBool bHasList;
    NiStreamLoadBinary(kStream, bHasList);

    if (bHasList)
    {
        m_pusTriList = NiAlloc(unsigned short, uiListLen);
        NiStreamLoadBinary(kStream, m_pusTriList, uiListLen);
    }

    NiBool bHasPalette;    
    NiStreamLoadBinary(kStream, bHasPalette);
    if (bHasPalette)
    {
        m_pucBonePalette = NiAlloc(unsigned char, uiInteractions);
        NiStreamLoadBinary(kStream, m_pucBonePalette, uiInteractions);
    }
}

//---------------------------------------------------------------------------
void NiSkinPartition::Partition::SaveBinary(NiStream& kStream)
{
    NiStreamSaveBinary(kStream, m_usVertices);
    NiStreamSaveBinary(kStream, m_usTriangles);
    NiStreamSaveBinary(kStream, m_usBones);
    NiStreamSaveBinary(kStream, m_usStrips);
    NiStreamSaveBinary(kStream, m_usBonesPerVertex);
    
    NiStreamSaveBinary(kStream, m_pusBones, m_usBones);

    NiBool bHasMap = (m_pusVertexMap != NULL);
    NiStreamSaveBinary(kStream, bHasMap);
    if (bHasMap)
    {
        NiStreamSaveBinary(kStream, m_pusVertexMap, m_usVertices);
    }

    unsigned int uiInteractions = m_usBonesPerVertex * m_usVertices;

    NiBool bHasWeights = (m_pfWeights != NULL);
    NiStreamSaveBinary(kStream, bHasWeights);
    if (bHasWeights)
    {
        NiStreamSaveBinary(kStream, m_pfWeights, uiInteractions);
    }

    unsigned int uiListLen;
    
    if (m_usStrips == 0)
    {
        uiListLen = 3 * m_usTriangles;
    }
    else
    {
        NiStreamSaveBinary(kStream, m_pusStripLengths, m_usStrips);
        unsigned int uiStripLengthSum = GetStripLengthSum();
        uiListLen = uiStripLengthSum;
    }

    NiBool bHasList = (m_pusTriList != NULL);
    NiStreamSaveBinary(kStream, bHasList);
    if (bHasList)
    {
        NiStreamSaveBinary(kStream, m_pusTriList, uiListLen);
    }

    NiBool bHasPalette = (m_pucBonePalette != NULL);
    NiStreamSaveBinary(kStream, bHasPalette);
    if (bHasPalette)
    {
        NiStreamSaveBinary(kStream, m_pucBonePalette, uiInteractions);
    }
}

//---------------------------------------------------------------------------
bool NiSkinPartition::Partition::IsEqual(const Partition& kPartition)
{
    if (m_usVertices != kPartition.m_usVertices)
        return false;
    if (m_usTriangles != kPartition.m_usTriangles)
        return false;
    if (m_usBones != kPartition.m_usBones)
        return false;
    if (m_usStrips != kPartition.m_usStrips)
        return false;
    if (m_usBonesPerVertex != kPartition.m_usBonesPerVertex)
        return false;
    
    if (memcmp(m_pusBones, kPartition.m_pusBones,
        sizeof(m_pusBones[0]) * m_usBones))
    {
        return false;
    }

    if (memcmp(m_pusVertexMap, kPartition.m_pusVertexMap,
        sizeof(m_pusVertexMap[0]) * m_usVertices))
    {
        return false;
    }

    unsigned int uiInteractions = m_usBonesPerVertex * m_usVertices;

    if (memcmp(m_pfWeights, kPartition.m_pfWeights,
        sizeof(m_pfWeights[0]) * uiInteractions))
    {
        return false;
    }

    if (m_usStrips == 0)
    {
        if (memcmp(m_pusTriList, kPartition.m_pusTriList,
            sizeof(m_pusTriList[0]) * 3 * m_usTriangles))
        {
            return false;
        }
    }
    else
    {
        if (memcmp(m_pusStripLengths, kPartition.m_pusStripLengths,
            sizeof(m_pusStripLengths[0]) * m_usStrips))
        {
            return false;
        }

        unsigned int uiStripLengthSum = GetStripLengthSum();
        if (memcmp(m_pusTriList, kPartition.m_pusTriList,
            sizeof(m_pusTriList[0]) * uiStripLengthSum))
        {
            return false;
        }
    }

    if ((m_pucBonePalette == NULL) != (kPartition.m_pucBonePalette == NULL))
        return false;

    if (m_pucBonePalette != NULL)
    {
        if (memcmp(m_pucBonePalette, kPartition.m_pucBonePalette,
            sizeof(m_pucBonePalette[0]) * uiInteractions))
        {
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------
// NiSkinPartition member functions

//---------------------------------------------------------------------------
NiSkinPartition::NiSkinPartition()
{
    m_pkPartitions = NULL;
}

//---------------------------------------------------------------------------
NiSkinPartition::~NiSkinPartition()
{
    NiRenderer::RemoveSkinPartitionRendererData(this);

    NiDelete[] m_pkPartitions;
}

//---------------------------------------------------------------------------
NiSkinPartition::VertexInteractions* 
NiSkinPartition::CreateVertexInteractions(unsigned int uiVertices,
    const NiSkinData* pkSkinData)
{
    // NiSkinData stores a list of vertices influenced by each bone. Shuffle 
    // the data so that we have a list of bones that influences each vertex.

    VertexInteractions* pkVertexInteraction = 
        NiNew VertexInteractions[uiVertices];
    NIASSERT(pkVertexInteraction != NULL);

    unsigned int i, uiBones = pkSkinData->GetBoneCount();

    for (i = 0; i < uiBones; i++)
    {
        NiSkinData::BoneData* pkBoneData = &pkSkinData->GetBoneData()[i];
        Interaction kInteraction;

        kInteraction.m_uiBone = i;

        for (unsigned int j = 0; j < pkBoneData->m_usVerts; j++)
        {
            unsigned int uiVert = pkBoneData->m_pkBoneVertData[j].m_usVert;
            kInteraction.m_fWeight =
                pkBoneData->m_pkBoneVertData[j].m_fWeight;

            pkVertexInteraction[uiVert].Add(kInteraction);
        }
    }

    for (i = 0; i < uiVertices; i++)
    {
        pkVertexInteraction[i].Unitize();
    }

    return pkVertexInteraction;
}

//---------------------------------------------------------------------------
void NiSkinPartition::MakeBoneMap(NiTPointerMap<unsigned int, float>& kMap,
    const unsigned short* pusTriangle,
    const VertexInteractions* pkVertexInteraction)
{
    // Creates a hash table that maps bone indices to the maximum weight that
    // bone exerts on any vertex in the triangle.

    for (unsigned int v = 0; v < 3; v++)
    {
        unsigned int uiVert = pusTriangle[v];
        const VertexInteractions* pkI = &pkVertexInteraction[uiVert];
        unsigned int uiVertexBones = pkI->GetSize();
        
        for (unsigned int b = 0; b < uiVertexBones; b++)
        {
            float fWeight;
            unsigned int uiBone = pkI->GetBone(b);
            
            bool bMapped = kMap.GetAt(uiBone, fWeight);
            if (!bMapped || fWeight < pkI->GetWeight(b))
            {
                kMap.SetAt(uiBone, pkI->GetWeight(b));
            }
        }
    }
}

//---------------------------------------------------------------------------
bool NiSkinPartition::RemoveExcessInteractions(
    const NiTriBasedGeomData* pkTriData, unsigned char ucBonesPerPartition,
    unsigned char ucBonesPerVertex, VertexInteractions* pkVertexInteraction)
{
    unsigned int uiVertices = pkTriData->GetVertexCount();
    unsigned int uiTriangles = pkTriData->GetTriangleCount();

    // Assumption made by this code: The sum of the weights of a vertex's 
    // interactions is one. This allows us to pitch the interaction with the
    // smallest weight without checking to see if that interaction is a
    // vertex's only interaction.

    // kMap maps bone indices to weights. It could be declared inside the
    // loop, but declaring it hear and calling RemoveAll at the end of the
    // loop prevents some deletion and reallocation work that would be done 
    // if the map went out of scope after every loop iteration.

    NiTPointerMap<unsigned int, float> kMap;
    unsigned int t, v;

    // Ditto remarks about kMap's declaration outside the loop.
    NiUnsignedIntSet kCoincidentVerts;

    unsigned short* pusIndex0;
    unsigned short* pusIndex1;
    pkTriData->IndexSort(pusIndex0, pusIndex1, NiGeometryData::VERTEX_MASK);

    // This loop identifies triangles that are influenced by more than the
    // allowed number of bones. For each such triangle, the least significant
    // vertex-bone interactions are thrown out until the number of bones
    // influencing the triangle reaches the allowed number.
    
    for (t = 0; t < uiTriangles; t++)
    {
        // Create a list of bones that influence this triangle. For each 
        // bone, save the maximum weight of that bone in any of the 
        // triangle's verts.

        unsigned short ausTriangle[3];

        pkTriData->GetTriangleIndices(t, ausTriangle[0], ausTriangle[1],
            ausTriangle[2]);

        if (Degenerate(ausTriangle))
            continue;

        MakeBoneMap(kMap, ausTriangle, pkVertexInteraction);

        // If the triangle is influenced by more than the maximum allowed
        // number of bones, toss out the least influential interactions until
        // the maximum allowed number is reached.

        unsigned int uiBones = kMap.GetCount();
        if (uiBones > ucBonesPerPartition)
        {
            for (;;)
            {
                NiTMapIterator pos = kMap.GetFirstPos();
                float fMinWeight = FLT_MAX;
                unsigned int uiMinBone = 0;
                
                do
                {
                    unsigned int uiBone;
                    float fWeight;
                    
                    kMap.GetNext(pos, uiBone, fWeight);
                    if (fWeight < fMinWeight)
                    {
                        fMinWeight = fWeight;
                        uiMinBone = uiBone;
                    }
                } while (pos);
                
                for (v = 0; v < 3; v++)
                {
                    unsigned int uiVert = ausTriangle[v];

                    // Identify all vertices coincident with vertex uiVert
                    // and remove the interaction of bone uiMinBone with
                    // those vertices.
                    unsigned int uiStart, uiEnd, uiCoincidentVert;

                    kCoincidentVerts.Add(uiVert);
                    VertexInteractions* pkI = &pkVertexInteraction[uiVert];

                    uiStart = uiEnd = pusIndex1[uiVert];
                    while (uiStart > 0 && pkTriData->VertexCompare(
                        uiCoincidentVert = pusIndex0[uiStart - 1], uiVert,
                        NiGeometryData::VERTEX_MASK) == 0)
                    {
                        if (pkI->SameBones(pkVertexInteraction[
                            uiCoincidentVert]))
                        {
                            kCoincidentVerts.Add(uiCoincidentVert);
                        }
                        uiStart--;
                    }

                    while (uiEnd < uiVertices - 1 && pkTriData->VertexCompare(
                        uiCoincidentVert = pusIndex0[uiEnd + 1], uiVert,
                        NiGeometryData::VERTEX_MASK) == 0)
                    {
                        if (pkI->SameBones(pkVertexInteraction[
                            uiCoincidentVert]))
                        {
                            kCoincidentVerts.Add(uiCoincidentVert);
                        }
                        uiEnd++;
                    }

                    for (unsigned int j = 0; j < kCoincidentVerts.GetSize();
                        j++)
                    {
                        uiCoincidentVert = kCoincidentVerts.GetAt(j);
                        pkI = &pkVertexInteraction[uiCoincidentVert];

                        if (pkI->RemoveBone(uiMinBone))
                        {
                            if (pkI->GetSize() == 0)
                            {
                                // Deleted bone was vertex's only bone, so
                                // return failure. This should only happen if
                                // the number of bones per partition is less
                                // than 3.
                                
                                NIASSERT(ucBonesPerPartition < 3);
                                NiFree(pusIndex0);
                                NiFree(pusIndex1);
                                return false;
                            }
                            
                            pkI->Unitize();
                        }
                    }

                    kCoincidentVerts.RemoveAll();
                }

                if (--uiBones == ucBonesPerPartition)
                    break;

                kMap.RemoveAll();
                MakeBoneMap(kMap, ausTriangle, pkVertexInteraction);
            }
        }
        
        kMap.RemoveAll();
    }

    for (v = 0; v < uiVertices; v++)
    {
        VertexInteractions* pkI = &pkVertexInteraction[v];

        // We've insured that no triangle is influenced by more than 
        // ucBonesPerPartition bones. Now make sure no vertex is influenced
        // by more tham ucBonesPerVertex bones.

        if (pkI->GetSize() > ucBonesPerVertex)
        {
            pkI->SortByWeight();
            pkI->Truncate(ucBonesPerVertex);
            pkI->Unitize();
        }
    }

    NiFree(pusIndex0);
    NiFree(pusIndex1);

    return true;
}

//---------------------------------------------------------------------------
void NiSkinPartition::CreateInitialBoneSets(
    const NiTriBasedGeomData* pkTriData,
    VertexInteractions* pkVertexInteraction,
    NiBoneSetCollection& kBoneSets)
{
    NIASSERT(kBoneSets.GetSize() == 0);
    unsigned int uiTriangles = pkTriData->GetTriangleCount();

    for (unsigned int t = 0; t < uiTriangles; t++)
    {
        unsigned short ausTriangle[3];
        pkTriData->GetTriangleIndices(t, ausTriangle[0], ausTriangle[1],
            ausTriangle[2]);

        if (Degenerate(ausTriangle))
            continue;

        BoneSet* pkNewSet = NiNew BoneSet;
        NIASSERT(pkNewSet != NULL);

        // Create a set that contains all the vertices that influence the
        // triangle.

        for (unsigned int v = 0; v < 3; v++)
        {
            unsigned int uiVert = ausTriangle[v];
            VertexInteractions* pkI = &pkVertexInteraction[uiVert];
            unsigned int uiVertexBones = pkI->GetSize();
            
            for (unsigned int b = 0; b < uiVertexBones; b++)
            {
                unsigned int uiBone = pkI->GetBone(b);
                pkNewSet->AddUnique(uiBone);
            }
        }
        
        pkNewSet->Sort();
        
        // Check existing sets.

        unsigned int s;

        for (s = 0; s < kBoneSets.GetSize(); /**/)
        {
            BoneSet* pkExistingSet = kBoneSets.GetAt(s);

            if (pkExistingSet->Contains(*pkNewSet))
            {
                // If there is already a boneset that contains this
                // triangle's boneset, we can stop.
                NiDelete pkNewSet;
                break;
            }
            
            if (pkNewSet->Contains(*pkExistingSet))
            {
                // If this triangle's bone set contains an existing
                // boneset, remove the existing boneset.
                NiDelete pkExistingSet;
                kBoneSets.RemoveAt(s);
            }
            else
            {
                s++;
            }
        }
        
        if (s == kBoneSets.GetSize())
        {
            // No boneset was found that contains this triangle's boneset, so
            // add the new boneset to the boneset set.
            kBoneSets.Add(pkNewSet);
        }
    }
}

//---------------------------------------------------------------------------
void NiSkinPartition::MergeBoneSets(unsigned char ucBonesPerPartition,
    NiBoneSetCollection& kBoneSets)
{
    // Partition merging algorithm. Time required is O(p^3), where p is the 
    // number of partitions. p is O(t), where t is the number of triangles.

    for (;;)
    {
        // Find best merge candidates. Currently, best merge candidates
        // are considered those which share the most bones.
        
        unsigned int auiMergeSets[2];
        auiMergeSets[0] = auiMergeSets[1] = 0;
        int iMaxShare = -1;
        
        for (unsigned int p0 = 0; p0 < kBoneSets.GetSize(); p0++)
        {
            BoneSet& kBoneSet0 = *kBoneSets.GetAt(p0);
            
            for (unsigned int p1 = p0 + 1; p1 < kBoneSets.GetSize(); p1++)
            {
                BoneSet& kBoneSet1 = *kBoneSets.GetAt(p1);                
                unsigned int uiMergeSize = kBoneSet0.MergedSize(kBoneSet1);
                
                if (uiMergeSize <= ucBonesPerPartition)
                {
                    int iShare = kBoneSet0.GetSize() +
                        kBoneSet1.GetSize() - uiMergeSize;
                    
                    if (iShare > iMaxShare)
                    {
                        auiMergeSets[0] = p0;
                        auiMergeSets[1] = p1;
                        iMaxShare = iShare;
                    }
                }
            }
        }
        
        if (iMaxShare < 0)
            break;
        
        // Merge
        
        kBoneSets.GetAt(auiMergeSets[0])->Merge(
            *kBoneSets.GetAt(auiMergeSets[1]));
        NIASSERT(kBoneSets.GetAt(auiMergeSets[0])->GetSize() <=
            ucBonesPerPartition);
        NiDelete kBoneSets.GetAt(auiMergeSets[1]);
        kBoneSets.RemoveAt(auiMergeSets[1]);
        
        // See if any sets are now a subset of the newly merged set.

        BoneSet* pkNewSet = kBoneSets.GetAt(auiMergeSets[0]);

        for (unsigned int p = 0; p < kBoneSets.GetSize(); /**/)
        {
            BoneSet* pkSet = kBoneSets.GetAt(p);

            if (pkSet != pkNewSet && pkNewSet->Contains(*pkSet))
            {
                NiDelete pkSet;
                kBoneSets.RemoveAt(p);
            }
            else
            {
                p++;
            }
        }
    }
}

//---------------------------------------------------------------------------
int NiSkinPartition::AssignTriangle(NiBoneSetCollection& kBoneSets,
    const unsigned short* pusTriangle,
    VertexInteractions* pkVertexInteraction, bool* pbAssigned, bool bAssign)
{
    for (unsigned int p = 0; p < kBoneSets.GetSize(); p++)
    {
        if (pbAssigned[p] == bAssign && kBoneSets.GetAt(p)->ContainsTriangle(
            pusTriangle, pkVertexInteraction))
        {
            pbAssigned[p] = true;
            return p;
        }
    }

    return -1;
}

//---------------------------------------------------------------------------
void NiSkinPartition::MakeBoneSets(const NiTriBasedGeomData* pkTriData, 
    const NiSkinData* pkSkinData, unsigned char ucBonesPerPartition,
    VertexInteractions* pkVertexInteraction, NiBoneSetCollection& kBoneSets,
    unsigned int*& puiTriangleToPartition)
{
    unsigned int uiTriangles = pkTriData->GetTriangleCount();

    CreateInitialBoneSets(pkTriData, pkVertexInteraction, kBoneSets);

    MergeBoneSets(ucBonesPerPartition, kBoneSets);
    
    // Assign each triangle to a partition. It is possible that a partition
    // will have no triangles assigned. For example, suppose these partitions
    // exist after the call to CreateInitialBoneSets:

    // 0.0: A B C
    // 0.1: A B D
    // 0.2: A B Z
    // 0.3: A C Z
    // 0.4: B D Y
    // 0.5: A D Y

    // Then, during MergeBoneSets, the following mergings occur. 0.0 merges
    // with 0.1:

    // 1.0: A B C D
    // 1.1: A B Z
    // 1.2: A C Z
    // 1.3: B D Y
    // 1.4: A D Y

    // 1.1 merges with 1.2:

    // 2.0: A B C D
    // 2.1: A B C Z
    // 2.2: B D Y
    // 2.3: A D Y

    // 2.2 merges with 2.3:

    // 3.0: A B C D
    // 3.1: A B C Z
    // 3.2: A B D Y

    // Set 3.0 was created from the merging of sets 0.0 and 0.1. But all the
    // bones from set 0.0 appear not only in 3.0, but also in 3.1.
    // Likewise, all the bones from set 0.1 appear not only in 3.0, but also
    // in 3.2. So all the triangles that led to the creation of set 3.0 can be
    // assigned to some other set. The following code attempts to take 
    // advantage of this possibility by first checking to see if a triangle 
    // can be assigned to a partition that already has triangles assigned to
    // it before checking partitions that as of yet have no triangles
    // assigned.
    
    puiTriangleToPartition = NiAlloc(unsigned int, uiTriangles);
    NIASSERT(puiTriangleToPartition != NULL);
        
    bool* pbAssigned = NiAlloc(bool, kBoneSets.GetSize());
    unsigned int p;

    for (p = 0; p < kBoneSets.GetSize(); p++)
    {
        pbAssigned[p] = false;
    }
    
    for (unsigned int t = 0; t < uiTriangles; t++)
    {
        unsigned short ausTriangle[3];
        
        pkTriData->GetTriangleIndices(t, ausTriangle[0], ausTriangle[1],
            ausTriangle[2]);

        if (Degenerate(ausTriangle))
        {
            puiTriangleToPartition[t] = UINT_MAX;
        }
        else
        {
            int iPart;

            if ((iPart = AssignTriangle(kBoneSets, ausTriangle,
                pkVertexInteraction, pbAssigned, true)) < 0)
            {
                iPart = AssignTriangle(kBoneSets, ausTriangle,
                    pkVertexInteraction, pbAssigned, false);
                NIASSERT(iPart >= 0);
            }

            puiTriangleToPartition[t] = iPart;
        }
    }

    // Delete partitions to which no triangle was assigned. 
    for (p = 0; p < kBoneSets.GetSize(); )
    {
        if (pbAssigned[p])
        {
            p++;
        }
        else
        {
            kBoneSets.RemoveAt(p);

            // Do partition reassignment necessitated by compacting
            // of kBoneSets when element p is removed:

            unsigned int uiOldIndex = kBoneSets.GetSize();

            for (unsigned int t = 0; t < uiTriangles; t++)
            {
                if (puiTriangleToPartition[t] == uiOldIndex)
                {
                    puiTriangleToPartition[t] = p;
                }
            }
        }
    }

    m_uiPartitions = kBoneSets.GetSize();

    NiFree(pbAssigned);
}

//---------------------------------------------------------------------------
bool NiSkinPartition::MakePartitions(const NiTriBasedGeomData* pkTriData,
    const NiSkinData* pkSkinData, unsigned char ucBonesPerPartition,
    unsigned char ucBonesPerVertex, bool bForcePalette)
{
    NIASSERT(ucBonesPerPartition >= ucBonesPerVertex);

    VertexInteractions* pkVertexInteraction;
    NiBoneSetCollection kBoneSets;
    unsigned int* puiTriangleToPartition;
    unsigned int uiVertices = pkTriData->GetVertexCount();

    pkVertexInteraction = CreateVertexInteractions(uiVertices, pkSkinData);

    bool bPartition = RemoveExcessInteractions(pkTriData, 
        ucBonesPerPartition, ucBonesPerVertex, pkVertexInteraction);

    if (bPartition)
    {
        MakeBoneSets(pkTriData, pkSkinData, ucBonesPerPartition,
            pkVertexInteraction, kBoneSets, puiTriangleToPartition);

        Partition* pkPartitions = NiNew Partition[m_uiPartitions];
        NIASSERT(pkPartitions != NULL);

        bool bNeedPalette = (ucBonesPerPartition != ucBonesPerVertex);

        TriangleSet kTriangles;
        for (unsigned int p = 0; p < m_uiPartitions; p++)
        {
            kTriangles.AddTriangles(p, puiTriangleToPartition,
                pkTriData->GetTriangleCount());

            BoneSet* pkBoneSet = kBoneSets.GetAt(p);

            pkPartitions[p].Create(kTriangles, *pkBoneSet, pkTriData,
                pkVertexInteraction, bNeedPalette, bForcePalette,
                ucBonesPerVertex, pkSkinData->GetBoneCount());

            kTriangles.RemoveAll();
            NiDelete pkBoneSet;
        }

        NiFree(puiTriangleToPartition);
        m_pkPartitions = pkPartitions;
    }

    NiDelete[] pkVertexInteraction;

    return bPartition;
}

//---------------------------------------------------------------------------
void NiSkinPartition::ReplacePartitions(
    NiSkinPartition::Partition* pkPartitions, unsigned int uiPartitions)
{
    NiDelete[] m_pkPartitions;
    m_uiPartitions = uiPartitions;
    m_pkPartitions = pkPartitions;
}
//---------------------------------------------------------------------------
bool NiSkinPartition::ResizePartitions(unsigned char ucBonesPerPartition,
    unsigned char ucBonesPerVertex)
{
    if (m_uiPartitions == 0 || !m_pkPartitions)
        return false;

    bool bAllStripified = m_pkPartitions[0].m_usStrips != 0;
    for (unsigned int ui = 0; ui < m_uiPartitions; ui++)
    {
        if (m_pkPartitions[ui].m_usBones > ucBonesPerPartition || 
            m_pkPartitions[ui].m_usBones * 2 > ucBonesPerPartition || 
            m_pkPartitions[ui].m_usBonesPerVertex > ucBonesPerVertex ||
            m_pkPartitions[ui].m_pkBuffData != NULL)
        {
            return false;
        }

        bool bStripified = m_pkPartitions[ui].m_usStrips != 0;
        if (bStripified != bAllStripified)
            return false;
    }

    if (m_uiPartitions > 256)
        return false;
    else if (m_uiPartitions == 1)
        return true;

    // Pre-compute which partitions will be merged.
    // There are 256 maximum partitions. This could be made more general,
    // however, it would come at the expense of more memory allocations.
    unsigned short usBonesPerPartition = (unsigned short)ucBonesPerPartition;
    unsigned short ausNewBoneList[256];
    unsigned short usNewBoneCount = 0;
    unsigned short usNewStartingPartitionIndex;
    unsigned short usNewPartitionCount = 0;
    unsigned int uiOldPartitionsHandled = 0;
    unsigned int uiOldStartingPartition = 0;

    // Allocate the same number of partitions as currently exist.
    // This is safe because the algorithm guarantees that we will never 
    // have more partitions than the original count.
    Partition* pkNewPartitions = NiNew Partition[m_uiPartitions];
    NIASSERT(pkNewPartitions != NULL);

    while (uiOldPartitionsHandled != m_uiPartitions)
    {
        usNewStartingPartitionIndex = uiOldStartingPartition;
        usNewBoneCount = 0;
        unsigned short usNewCurrentPartition = usNewPartitionCount;

        for (unsigned int ui = uiOldStartingPartition; ui < m_uiPartitions; 
            ui++)
        {
            bool bCompletedPartition = true;
            unsigned short usStartingBoneCount = usNewBoneCount;

            for (unsigned short usBone = 0; 
                usBone < m_pkPartitions[ui].m_usBones; usBone++)
            {
                // Add this partition's unique bones to the new partition's 
                // bone list. Ignore duplicates.
                bool bFound = false;
                for (unsigned short usAssignedBones = 0; 
                    usAssignedBones < usNewBoneCount; usAssignedBones++)
                {
                    if (m_pkPartitions[ui].m_pusBones[usBone] == 
                        ausNewBoneList[usAssignedBones])
                    {
                        bFound = true;
                        continue;
                    }
                }

                // If not found in the existing bone list and adding the
                // new bone won't overflow the list add it to the new 
                // partition. Otherwise, if it wasn't found, we cannot safely
                // merge this partition. It should be used as the start of 
                // the next partition. If it was found, don't do anything
                // and continue searching for more bones.
                if (!bFound && usNewBoneCount + 1 <= 
                    usBonesPerPartition)
                {
                    ausNewBoneList[usNewBoneCount] =
                        m_pkPartitions[ui].m_pusBones[usBone];
                    usNewBoneCount++;
                }
                else if (!bFound)
                {
                    bCompletedPartition = false;
                    break;
                }
            }

            if (bCompletedPartition)
            {
                uiOldPartitionsHandled++; 
                uiOldStartingPartition++;
            }
            else
            {
                // If the partition wasn't completed, we need to back out 
                // all of its bones from the list.
                usNewBoneCount = usStartingBoneCount;
                break;
            }
        }

        // Now we need to merge the partitions.
        unsigned int uiStartPartition = usNewStartingPartitionIndex;
        unsigned int uiEndPartition = uiOldStartingPartition;

        MergeIntoPartition(pkNewPartitions[usNewCurrentPartition], 
            m_pkPartitions, uiStartPartition, uiEndPartition,
            ucBonesPerPartition, ucBonesPerVertex, ausNewBoneList,
            usNewBoneCount);
            
        NIASSERT(pkNewPartitions[usNewCurrentPartition].m_usBones <=  
            ucBonesPerPartition);
        NIASSERT(pkNewPartitions[usNewCurrentPartition].m_usBonesPerVertex ==
            ucBonesPerVertex);
        usNewPartitionCount++;
    }

    ReplacePartitions(pkNewPartitions, usNewPartitionCount);
    return true;

}
//---------------------------------------------------------------------------
void NiSkinPartition::MergeIntoPartition(Partition& kDest, Partition* pkSrc, 
    unsigned int uiSrcStartIdx, unsigned int uiSrcEndIdx,
    unsigned char ucBonesPerPartition, unsigned char ucBonesPerVertex,
    unsigned short* pusDestBoneIndices, unsigned int uiDestBoneCount)
{
    NIASSERT(uiSrcStartIdx != uiSrcEndIdx);
    kDest.m_usBonesPerVertex = ucBonesPerVertex;
    kDest.m_usVertices = 0;
    kDest.m_usTriangles = 0;
    kDest.m_usStrips = 0;
    kDest.m_usBones = uiDestBoneCount;
    
    kDest.m_pusBones = NULL;
    kDest.m_pfWeights = NULL;
    kDest.m_pusVertexMap = NULL;
    kDest.m_pucBonePalette = NULL;
    kDest.m_pusTriList = NULL;
    kDest.m_pusStripLengths = NULL;

    kDest.m_pusBones = NiAlloc(unsigned short, uiDestBoneCount);
    NiMemcpy(kDest.m_pusBones, uiDestBoneCount * sizeof(unsigned short),
        pusDestBoneIndices, uiDestBoneCount * sizeof(unsigned short));

    bool bStripified = pkSrc[uiSrcStartIdx].m_usStrips != 0;
    bool bAddDegenerates = false;
    unsigned int uiNumIndices = 0;

    for (unsigned int ui = uiSrcStartIdx; ui < uiSrcEndIdx; ui++)
    {
        kDest.m_usVertices += pkSrc[ui].m_usVertices;
        if (pkSrc[ui].m_usStrips == 0)
        {
            uiNumIndices += pkSrc[ui].m_usTriangles * 3;
        }
        else
        {
            uiNumIndices += pkSrc[ui].m_usTriangles + 2;

            if (pkSrc[ui].m_usStrips == 1)
                kDest.m_usStrips = 1;
            else
                kDest.m_usStrips += pkSrc[ui].m_usStrips;
        }
        
        // Add in 2 or 3 extra triangles per partition to stitch their strips 
        // together if degenerate triangles are allowed.
        if (kDest.m_usStrips == 1 && ui != uiSrcStartIdx)
        {
            uiNumIndices += 2;
            if (pkSrc[ui-1].m_usTriangles % 2 != 0)
            {
                uiNumIndices++;
            }

            bAddDegenerates = true;
        }
    }

    if (kDest.m_usStrips == 0)
    {
        NIASSERT(uiNumIndices % 3 == 0);
        kDest.m_usTriangles = uiNumIndices / 3;
    }
    else
    {
        kDest.m_usTriangles = uiNumIndices - 2;
    }

    unsigned int uiInteractions = kDest.m_usBonesPerVertex * 
        kDest.m_usVertices;

    bool bPalette = false;
    if (ucBonesPerPartition > ucBonesPerVertex)
    {
        bPalette = true;
        kDest.m_pucBonePalette = NiAlloc(unsigned char, uiInteractions);
    }

    if (!bStripified)
    {
        kDest.m_pusStripLengths = NULL;
    }
    else
    {
        kDest.m_pusStripLengths = NiAlloc(unsigned short, kDest.m_usStrips);
    }

    if (kDest.m_pusStripLengths != NULL && kDest.m_usStrips == 1)
    {
        kDest.m_pusStripLengths[0] = uiNumIndices;
    }

    kDest.m_pusTriList = NiAlloc(unsigned short, uiNumIndices);        
    kDest.m_pfWeights = NiAlloc(float, uiInteractions);
    kDest.m_pusVertexMap = NiAlloc(unsigned short, kDest.m_usVertices);
    
    unsigned int uiInteractionsCopied = 0;
    unsigned short usVerticesCopied = 0;
    unsigned short usStripsCopied = 0;
    unsigned short* pusTriList = kDest.m_pusTriList;
    unsigned short* pusStripLengths = kDest.m_pusStripLengths;

    for (unsigned int ui = uiSrcStartIdx; ui < uiSrcEndIdx; ui++)
    {
        // Get the offsets into the destination data that match
        // the current source partition
        float* pfDestWeights = &kDest.m_pfWeights[uiInteractionsCopied];
        unsigned short* pusDestVertMap = 
            &kDest.m_pusVertexMap[usVerticesCopied];
        
        unsigned int uiSrcInteractions = pkSrc[ui].m_usBonesPerVertex * 
            pkSrc[ui].m_usVertices;

        // Copy the bone weights for this partition as-is
        NiMemcpy(pfDestWeights, sizeof(float) * 
            (uiInteractions - uiInteractionsCopied), 
            pkSrc[ui].m_pfWeights, sizeof(float) * uiSrcInteractions);

        // Copy the triangle list.
        unsigned int uiSrcListLength = 0;
        if (bStripified)
            uiSrcListLength = pkSrc[ui].GetStripLengthSum(); 
        else
            uiSrcListLength = 3 * pkSrc[ui].m_usTriangles;

        // Add in a degenerate triangle, if needed.
        if (ui != uiSrcStartIdx && bAddDegenerates)
        {
            pusTriList[0] = pusTriList[-1];
            pusTriList++;
            
            NIMEMASSERT(NiVerifyAddress(kDest.m_pusTriList));
            pusTriList[0] = usVerticesCopied + pkSrc[ui].m_pusTriList[0];
            pusTriList++;

            NIMEMASSERT(NiVerifyAddress(kDest.m_pusTriList));
            if (pkSrc[ui-1].m_usTriangles % 2 != 0)
            {
                pusTriList[0] = usVerticesCopied + pkSrc[ui].m_pusTriList[0];
                NIMEMASSERT(NiVerifyAddress(kDest.m_pusTriList));
                pusTriList++;
            }
        }

        for (unsigned int uiIndex = 0; uiIndex < uiSrcListLength; uiIndex++)
        {
            pusTriList[uiIndex] = usVerticesCopied + 
                pkSrc[ui].m_pusTriList[uiIndex];
            
            NIMEMASSERT(NiVerifyAddress(kDest.m_pusTriList));
        }

        pusTriList += uiSrcListLength;

        // Copy the strip lists, if they exist.
        if (kDest.m_usStrips > 1)
        {
            NiMemcpy(pusStripLengths, sizeof(unsigned short) *
                (kDest.m_usStrips - usStripsCopied), 
                pkSrc[ui].m_pusStripLengths, sizeof(unsigned short) *
                kDest.m_usStrips);
            usStripsCopied += kDest.m_usStrips;
        }

        // Copy the vertex map for this partition as-is. Make sure that this is
        // done after the index buffer has been repopulated so that 
        // usVerticesCopied has the total number of vertices of the prior
        // partitions and does not include this partition.
        NiMemcpy(pusDestVertMap, sizeof(unsigned short) * 
            (kDest.m_usVertices - usVerticesCopied), 
            pkSrc[ui].m_pusVertexMap, sizeof(unsigned short) * 
            pkSrc[ui].m_usVertices);
        usVerticesCopied += pkSrc[ui].m_usVertices;

        // If required, create or copy the bone palette
        // This is more complicated as a bone palette will need to be created
        // if one doesn't exist in the source or the source bone palette
        // entries will need to be remapped onto the destination bone entries.
        if (bPalette)
        {
            unsigned char* pucPalette = &kDest.m_pucBonePalette[
                uiInteractionsCopied];

            // Create a palette remapping
            unsigned char aucPaletteRemapping[256];
            unsigned int uiSrcPaletteCount = pkSrc[ui].m_usBones;

            for (unsigned int uiSrcPalIdx = 0; 
                uiSrcPalIdx < uiSrcPaletteCount; uiSrcPalIdx++)
            {
                aucPaletteRemapping[uiSrcPalIdx] = 0xFF;
                for (unsigned int uiDestPalIdx = 0; 
                    uiDestPalIdx < uiDestBoneCount; uiDestPalIdx++)
                {
                    if (pusDestBoneIndices[uiDestPalIdx] == 
                        pkSrc[ui].m_pusBones[uiSrcPalIdx])
                    {
                        aucPaletteRemapping[uiSrcPalIdx] = uiDestPalIdx;
                        break;
                    }
                }
                // If this assert is hit, somehow the destination bone list
                // does not contain a bone in the source bone list.
                NIASSERT(aucPaletteRemapping[uiSrcPalIdx] != 0xFF);
            }

            if (pkSrc[ui].m_pucBonePalette)
            {
                for (unsigned int uiVertex = 0; 
                    uiVertex < pkSrc[ui].m_usVertices; uiVertex++)
                {
                    for (unsigned int uiBone = 0; uiBone < 
                        pkSrc[ui].m_usBonesPerVertex; uiBone++)
                    {
                        NIASSERT(pkSrc[ui].m_usBonesPerVertex == 
                            kDest.m_usBonesPerVertex);
                        
                        unsigned int uiWhichIdx = 
                            uiVertex * kDest.m_usBonesPerVertex + uiBone;
                        unsigned int uiPaletteEntry =
                            pkSrc[ui].m_pucBonePalette[uiWhichIdx];

                        pucPalette[uiWhichIdx] = 
                            aucPaletteRemapping[uiPaletteEntry];
                    }
                }
            }
            else 
            {
                // Create one. The remapping array should match with the order
                // of the bone indices.
                NIASSERT(pkSrc[ui].m_usBonesPerVertex == uiSrcPaletteCount);

                for (unsigned int uiVertex = 0; 
                    uiVertex < pkSrc[ui].m_usVertices; uiVertex++)
                {
                    for (unsigned int uiBone = 0; uiBone < 
                        pkSrc[ui].m_usBonesPerVertex; uiBone++)
                    {
                        NIASSERT(pkSrc[ui].m_usBonesPerVertex == 
                            kDest.m_usBonesPerVertex);

                        unsigned int uiWhichIdx = 
                            uiVertex * kDest.m_usBonesPerVertex + uiBone;
                        pucPalette[uiWhichIdx] = aucPaletteRemapping[uiBone];
                    }
                }
            }
        }
        
        uiInteractionsCopied += uiSrcInteractions;

        NIMEMASSERT(NiVerifyAddress(kDest.m_pusBones));
        NIMEMASSERT(NiVerifyAddress(kDest.m_pfWeights));
        NIMEMASSERT(NiVerifyAddress(kDest.m_pusVertexMap));
        NIMEMASSERT(NiVerifyAddress(kDest.m_pucBonePalette));
        NIMEMASSERT(NiVerifyAddress(kDest.m_pusTriList));
        NIMEMASSERT(NiVerifyAddress(kDest.m_pusStripLengths));

    }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSkinPartition);
//---------------------------------------------------------------------------
void NiSkinPartition::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uiPartitions);

    m_pkPartitions = NiNew Partition[m_uiPartitions];

    for (unsigned int i = 0; i < m_uiPartitions; i++)
    {
        m_pkPartitions[i].LoadBinary(kStream);
    }
}
//---------------------------------------------------------------------------
void NiSkinPartition::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiSkinPartition::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiSkinPartition::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiPartitions);

    for (unsigned int i = 0; i < m_uiPartitions; i++)
    {
        m_pkPartitions[i].SaveBinary(kStream);
    }
}
//---------------------------------------------------------------------------
bool NiSkinPartition::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiSkinPartition* pkSkinPartition = (NiSkinPartition*) pkObject;

    if (m_uiPartitions != pkSkinPartition->m_uiPartitions)
    {
        return false;
    }

    for (unsigned int i = 0; i < m_uiPartitions; i++)
    {
        if (!m_pkPartitions[i].IsEqual(pkSkinPartition->m_pkPartitions[i]))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
