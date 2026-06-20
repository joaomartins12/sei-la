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

#include "NiTriShapeData.h"

//---------------------------------------------------------------------------
void NiTriShapeData::CalculateNormals ()
{
    CreateNormals(true);
    
    unsigned short* pTriList = m_pusTriList;
    unsigned short usTris = GetActiveTriangleCount();

    for (unsigned short i = 0; i < usTris; i++)
    {
        // get indices to triangle vertices
        unsigned short i0 = *pTriList++;
        unsigned short i1 = *pTriList++;
        unsigned short i2 = *pTriList++;

        // compute unit length triangle normal
        NiPoint3 e1 = m_pkVertex[i1] - m_pkVertex[i0];
        NiPoint3 e2 = m_pkVertex[i2] - m_pkVertex[i1];
        NiPoint3 normal = e1.Cross(e2);
        NiPoint3::UnitizeVector(normal);

        // update the running sum of normals at the various vertices
        m_pkNormal[i0] += normal;
        m_pkNormal[i1] += normal;
        m_pkNormal[i2] += normal;

        // update the normals for vertices that share normals with these verts
        if (m_pkSharedNormals && m_usSharedNormalsArraySize == m_usVertices)
        {
            unsigned short* aSimilarVertexArray = 
                m_pkSharedNormals[i0].GetArray();
            unsigned short usCount = m_pkSharedNormals[i0].GetCount();
            while (usCount--)
                m_pkNormal[aSimilarVertexArray[usCount]] += normal;
        
            aSimilarVertexArray = m_pkSharedNormals[i1].GetArray();
            usCount = m_pkSharedNormals[i1].GetCount();
            while (usCount--)
                m_pkNormal[aSimilarVertexArray[usCount]] += normal;
        
            aSimilarVertexArray = m_pkSharedNormals[i2].GetArray();
            usCount = m_pkSharedNormals[i2].GetCount();
            while (usCount--)
                m_pkNormal[aSimilarVertexArray[usCount]] += normal;
        }
    }
    
    NiPoint3::UnitizeVectors(m_pkNormal, GetActiveVertexCount(),
        sizeof(m_pkNormal[0]));

    MarkAsChanged(NORMAL_MASK);
}
//---------------------------------------------------------------------------
