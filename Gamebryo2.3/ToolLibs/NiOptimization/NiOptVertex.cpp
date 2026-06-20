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

#include "NiOptVertex.h"

//---------------------------------------------------------------------------
NiOptVertex::NiOptVertex ()
    :
    m_kPosition(NI_INFINITY, NI_INFINITY, NI_INFINITY),
    m_kNormal(NI_INFINITY, NI_INFINITY, NI_INFINITY),
    m_kColor(NI_INFINITY, NI_INFINITY, NI_INFINITY, NI_INFINITY)
{
    m_uiNumUVs = 0;
    memset(m_akUV, 0, MAX_UV * sizeof(m_akUV[0]));

    m_uiNumVertexFloats = 0;
    for (unsigned int i = 0; i < MAX_VERTEX_FLOATS; i++)
    {
        m_afVertexFloat[i] = 0;
        m_auiVertexFloatType[i] = INVALID;
    }

    m_uiNumBones = 0;
    m_puiBoneIndex = 0;
    m_pfWeight = 0;
}
//---------------------------------------------------------------------------
template <class T>
static int ArrayCompare(const T* p0, const T* p1, unsigned int uiSize)
{
    for (unsigned int i = 0; i < uiSize; i++)
    {
        if (p0[i] < p1[i])
            return -1;
        if (p0[i] > p1[i])
            return 1;
    }

    return 0;
}
//---------------------------------------------------------------------------
int NiOptVertex::Compare(const NiOptVertex& kVert) const
{
    int iCmp;

    iCmp = ArrayCompare(&m_kPosition.x, &kVert.m_kPosition.x, 3);
    if (iCmp != 0)
        return iCmp;

    iCmp = ArrayCompare(&m_kNormal.x, &kVert.m_kNormal.x, 3);
    if (iCmp != 0)
        return iCmp;

    iCmp = ArrayCompare(&m_kColor.r, &kVert.m_kColor.r, 4);
    if (iCmp != 0)
        return iCmp;

    iCmp = ArrayCompare(&m_uiNumUVs, &kVert.m_uiNumUVs, 1);
    if (iCmp != 0)
        return iCmp;

    iCmp = ArrayCompare(&m_akUV[0].x, &kVert.m_akUV[0].x, 2 * m_uiNumUVs);
    if (iCmp != 0)
        return iCmp;

    iCmp = ArrayCompare(&m_uiNumVertexFloats, &kVert.m_uiNumVertexFloats, 1);
    if (iCmp != 0)
        return iCmp;

    iCmp = ArrayCompare(&m_afVertexFloat[0], &kVert.m_afVertexFloat[0],
        m_uiNumVertexFloats);
    if (iCmp != 0)
        return iCmp;

    iCmp = ArrayCompare(&m_auiVertexFloatType[0],
        &kVert.m_auiVertexFloatType[0], m_uiNumVertexFloats);
    if (iCmp != 0)
        return iCmp;

    iCmp = ArrayCompare(&m_uiNumBones, &kVert.m_uiNumBones, 1);
    if (iCmp != 0)
        return iCmp;

    if (m_uiNumBones == 0)
        return 0;

    NIASSERT(m_puiBoneIndex != NULL);
    NIASSERT(kVert.m_puiBoneIndex != NULL);
    iCmp = ArrayCompare(&m_puiBoneIndex[0], &kVert.m_puiBoneIndex[0],
        m_uiNumBones);
    if (iCmp != 0)
        return iCmp;

    NIASSERT(m_pfWeight != NULL);
    NIASSERT(kVert.m_pfWeight != NULL);
    iCmp = ArrayCompare(&m_pfWeight[0], &kVert.m_pfWeight[0], m_uiNumBones);
    return iCmp;
}
