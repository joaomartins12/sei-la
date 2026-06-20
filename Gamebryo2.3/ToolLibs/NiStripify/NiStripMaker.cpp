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

#include "NiStripMaker.h"
#include "NiStripifyEdge.h"
#include "NiStripifyTriangle.h"
#include "NiStripifyVertex.h"
#include "NiNeighborManager.h"
#include <NiSystem.h>

// static functions
static unsigned int TriMod3(unsigned int v);
static unsigned short GetVertexCount(const unsigned short* pTriList, 
                                     unsigned int uiTriangles);
static bool TriangleIsValid(const unsigned short* pTriangle);
static unsigned int CountValidTriangles(const unsigned short* pTriList, 
                                        unsigned int uiTriangles);
//---------------------------------------------------------------------------
NiStripMaker::NiStripMaker(const unsigned short* pTriList, 
                           unsigned int uiTriangles)
{
    m_usStrips = 0;
    m_pusStripLengths = NULL;
    m_pusStripLists = NULL;
    m_pkTriangles = NULL;

    unsigned int uiValidTriangles = CountValidTriangles(pTriList, uiTriangles);
    unsigned short* pNewTriList;

    if (uiValidTriangles == uiTriangles)
    {
        pNewTriList = (unsigned short *) pTriList;
    }
    else
    {
        // Create a new list which only contains the valid triangles from the
        // original list. A triangle is valid if it has three distinct
        // vertices.

        unsigned int i, j;

        pNewTriList = NiAlloc(unsigned short, 3 * uiValidTriangles);
        NIASSERT(pNewTriList != NULL);

        for (i = j = 0; i < uiTriangles; i++)
        {
            const unsigned short* pTriangle = &pTriList[3 * i];
            if (TriangleIsValid(pTriangle))
            {
                NIASSERT(j < uiValidTriangles);
                pNewTriList[3 * j + 0] = pTriangle[0];
                pNewTriList[3 * j + 1] = pTriangle[1];
                pNewTriList[3 * j + 2] = pTriangle[2];
                j++;
            }
        }

        NIASSERT(j == uiValidTriangles);
        uiTriangles = uiValidTriangles;
    }

    m_pkTriangles = NiNew NiStripifyTriangle[uiTriangles];
    NIASSERT(m_pkTriangles != NULL);

    BuildConnectivity(pNewTriList, uiTriangles);
    BuildStrips(pNewTriList, uiTriangles);

    if (pNewTriList != pTriList)
    {
        NiDelete pNewTriList;
    }

    NiDelete[] m_pkTriangles;
    for (unsigned int i = 0; i < m_kEdges.GetSize(); i++)
    {
        NiDelete m_kEdges.GetAt(i);
    }

    m_usTriangles = uiTriangles;
}

//---------------------------------------------------------------------------
NiStripMaker::~NiStripMaker()
{
    // NB: m_pusStripLengths and m_pStripLists are not deleted.
}

//---------------------------------------------------------------------------
void NiStripMaker::BuildConnectivity(const unsigned short* pTriList, 
                                     unsigned int uiTriangles)
{
    unsigned int i, e;
    NiStripifyVertex* pVertices;
    unsigned short usVertices = GetVertexCount(pTriList, uiTriangles);

    pVertices = NiNew NiStripifyVertex[usVertices];
    NIASSERT(pVertices != NULL);

#ifdef _DEBUG
    for (i = 0; i < usVertices; i++)
    {
        pVertices[i].m_usVert = i;
    }

    for (i = 0; i < uiTriangles; i++)
    {
        m_pkTriangles[i].m_usTriangle = i;
    }
#endif

    // create edges
    for (i = 0; i < uiTriangles; i++)
    {
        unsigned short usTriVert[3];

        usTriVert[0] = pTriList[3 * i];
        usTriVert[1] = pTriList[3 * i + 1];
        usTriVert[2] = pTriList[3 * i + 2];

        for (e = 0; e < 3; e++)
        {
            unsigned short usEdgeVert[2];

            usEdgeVert[0] = usTriVert[e == 2 ? 0 : e + 1];
            usEdgeVert[1] = usTriVert[e == 0 ? 2 : e - 1];

            NiStripifyEdge* pEdge = pVertices[usEdgeVert[0]].
                GetEdge(usEdgeVert[1]);
            if (pEdge == NULL)
            {
                pEdge = NiNew NiStripifyEdge(usEdgeVert[0], usEdgeVert[1]);
                NIASSERT(pEdge != NULL);

                // Add edge to global list of edges
                m_kEdges.Add(pEdge);

                // Add edge to list of edges originating at usEdgeVert[0].
                pVertices[usEdgeVert[0]].AddEdge(pEdge);
            }

            m_pkTriangles[i].SetEdge(e, pEdge);
            pEdge->AddTriangle(&m_pkTriangles[i]);
        }
    }

    // set each triangle's neighboring edges
    for (i = 0; i < uiTriangles; i++)
    {
        unsigned short usTriVert[3];

        usTriVert[0] = pTriList[3 * i];
        usTriVert[1] = pTriList[3 * i + 1];
        usTriVert[2] = pTriList[3 * i + 2];

        for (e = 0; e < 3; e++)
        {
            unsigned short usEdgeVert[2];

            usEdgeVert[0] = usTriVert[e == 2 ? 0 : e + 1];
            usEdgeVert[1] = usTriVert[e == 0 ? 2 : e - 1];

            NiStripifyEdge* pNeighborEdge = pVertices[usEdgeVert[1]].
                GetEdge(usEdgeVert[0]);

            m_pkTriangles[i].SetNeighborEdge(e, pNeighborEdge);
        }
    }

    NiDelete[] pVertices;
}

//---------------------------------------------------------------------------
void NiStripMaker::BuildStrips(const unsigned short* pTriList, 
                               unsigned int uiTriangles)
{
    unsigned int i, j;
    NiTPrimitiveSet<NiUnsignedShortSet*> strips;
    unsigned int uiTotalStripTris = 0, uiTotalStripLength = 0;
    NiNeighborManager NeighborManager;

    for (i = 0; i < uiTriangles; i++)
    {
        NeighborManager.Add(&m_pkTriangles[i]);
    }

    while (uiTotalStripTris < uiTriangles)
    {
        NiUnsignedShortSet* pStrip;

        pStrip = NiNew NiUnsignedShortSet(3);
        NIASSERT(pStrip != NULL);

        NiStripifyTriangle* pTriangle = NeighborManager.GetMinimum();
        unsigned short usTriangle = pTriangle - m_pkTriangles;

        // Choose a starting vertex. Initialize to 0 in case triangle
        // has no neighbors.
        unsigned short usVertex, usVertexIndex = 0;
        NiStripifyEdge* pEdge;
        
        for (i = 0; i < 3; i++)
        {
            pEdge = pTriangle->GetNeighborEdge(i);
            if (pEdge != NULL && pEdge->GetTriangleCount() > 0)
            {
                usVertexIndex = i;
                break;
            }
        }

        pStrip->Add(pTriList[usTriangle * 3 + usVertexIndex]);
        pStrip->Add(pTriList[usTriangle * 3 + TriMod3(usVertexIndex + 1)]);
        usVertex = pTriList[usTriangle * 3 + TriMod3(usVertexIndex + 2)];

        for (;;)
        {
            unsigned short usOldVertex, usNeighborIndex;

            uiTotalStripTris++;
            pStrip->Add(usVertex);
            usOldVertex = pStrip->GetAt(pStrip->GetSize() - 3);
            usNeighborIndex = GetVertexIndex(pTriList, usOldVertex, 
                usTriangle);
            pEdge = pTriangle->GetNeighborEdge(usNeighborIndex);
            pTriangle->RemoveFromEdges();
            NeighborManager.Remove(pTriangle);
            NeighborManager.AdjustNeighbors(pTriangle);

            if (pEdge == NULL || pEdge->GetTriangleCount() == 0)
            {
                break;
            }

            pTriangle = pEdge->GetTriangle(0);
            usTriangle = pTriangle - m_pkTriangles;
            usVertex = GetExcludedVertex(pTriList, pEdge, usTriangle);
        }

        strips.Add(pStrip);

        uiTotalStripLength += pStrip->GetSize();
    }

    m_usStrips = strips.GetSize();
    m_pusStripLengths = NiAlloc(unsigned short, m_usStrips);
    NIASSERT(m_pusStripLengths != NULL);

    m_pusStripLists = NiAlloc(unsigned short, uiTotalStripLength);
    NIASSERT(m_pusStripLists != NULL);

    unsigned int uiIndex = 0;

    for (i = 0; i < m_usStrips; i++)
    {
        NiUnsignedShortSet* pStrip = strips.GetAt(i);

        for (j = 0; j < pStrip->GetSize(); j++)
        {
            NIASSERT(uiIndex < uiTotalStripLength);
            m_pusStripLists[uiIndex++] = pStrip->GetAt(j);
        }

        m_pusStripLengths[i] = pStrip->GetSize();
        NiDelete pStrip;
    }

    NIASSERT(uiIndex == uiTotalStripLength);
}

//---------------------------------------------------------------------------
unsigned short NiStripMaker::GetVertexIndex(const unsigned short* pTriList,
                                            unsigned short usVertex,
                                            unsigned short usTriangle)
{
    const unsigned short* pTriangle = &pTriList[3 * usTriangle];

    if (pTriangle[0] == usVertex)
    {
        return 0;
    }
    else if (pTriangle[1] == usVertex)
    {
        return 1;
    }
    else
    {
        NIASSERT(pTriangle[2] == usVertex);
        return 2;
    }
}

//---------------------------------------------------------------------------
unsigned short NiStripMaker::GetExcludedVertex(const unsigned short* pTriList, 
                                               NiStripifyEdge* pEdge, 
                                               unsigned short usTriangle)
{
    const unsigned short* pTriangle = &pTriList[3 * usTriangle];

#ifdef _DEBUG
    unsigned int i, uiIncludedVerts = 0;

    for (i = 0; i < 3; i++)
    {
        if (pTriangle[i] == pEdge->GetVert(0) || 
            pTriangle[i] == pEdge->GetVert(1))
        {
            uiIncludedVerts++;
        }
    }

    NIASSERT(uiIncludedVerts == 2);
#endif

    if (pTriangle[0] != pEdge->GetVert(0) && pTriangle[0] != pEdge->GetVert(1))
    {
        return pTriangle[0];
    }
    if (pTriangle[1] != pEdge->GetVert(0) && pTriangle[1] != pEdge->GetVert(1))
    {
        return pTriangle[1];
    }
    return pTriangle[2];
}

//---------------------------------------------------------------------------
// static functions

//---------------------------------------------------------------------------
static inline unsigned int TriMod3(unsigned int v)
{
    NIASSERT(v < 6);
    return (v < 3) ? v : v - 3;
}

//---------------------------------------------------------------------------
static unsigned short GetVertexCount(const unsigned short* pTriList, 
                                     unsigned int uiTriangles)
{
    unsigned short usVertices = 0;
    unsigned int i;

    for (i = 0; i < 3 * uiTriangles; i++)
    {
        if (pTriList[i] + 1 > usVertices)
        {
            usVertices = pTriList[i] + 1;
        }
    }

    return usVertices;
}

//---------------------------------------------------------------------------
static bool TriangleIsValid(const unsigned short* pTriangle)
{
    return 
        pTriangle[0] != pTriangle[1] &&
        pTriangle[0] != pTriangle[2] &&
        pTriangle[1] != pTriangle[2];
}

//---------------------------------------------------------------------------
static unsigned int CountValidTriangles(const unsigned short* pTriList, 
                                        unsigned int uiTriangles)
{
    unsigned int i, uiValidTriangles = 0;

    for (i = 0; i < uiTriangles; i++)
    {
        if (TriangleIsValid(&pTriList[3 * i]))
        {
            uiValidTriangles++;
        }
    }

    return uiValidTriangles;
}
