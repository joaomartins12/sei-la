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

#include "NiNeighborManager.h"
#include "NiStripifyTriangle.h"
#include "NiStripifyEdge.h"

//---------------------------------------------------------------------------
NiNeighborManager::NiNeighborManager()
{
    m_apkNeighborLists[0] =
    m_apkNeighborLists[1] =
    m_apkNeighborLists[2] =
    m_apkNeighborLists[3] = NULL;
}

//---------------------------------------------------------------------------
void NiNeighborManager::Add(NiStripifyTriangle* pkTriangle)
{
    NiStripifyTriangle* pkOldHead;
    unsigned int uiNeighbors = CountNeighbors(pkTriangle);
    
    pkOldHead = m_apkNeighborLists[uiNeighbors];
    pkTriangle->m_pkNext = pkOldHead;
    if (pkOldHead != NULL)
    {
        NIASSERT(pkOldHead->m_pkPrev == NULL);
        pkOldHead->m_pkPrev = pkTriangle;
    }
    m_apkNeighborLists[uiNeighbors] = pkTriangle;
    pkTriangle->m_pkPrev = NULL;
}

//---------------------------------------------------------------------------
// Called when the number of neighboring triangles may have changed. Remove
// triangle from whatever list it is currently in and add it to the correct 
// list.
void NiNeighborManager::Adjust(NiStripifyTriangle* pkTriangle)
{
    Remove(pkTriangle);
    Add(pkTriangle);
}

//---------------------------------------------------------------------------
// Find a triangle to start a strip. 
// If there is a triangle with no neighbors, use it. 
// Otherwise, check for a triangle with 1 neighbor. And so on.
NiStripifyTriangle* NiNeighborManager::GetMinimum() const
{
    unsigned int i;

    for (i = 0; ; i++)
    {
        NIASSERT(i < 4);
        if (m_apkNeighborLists[i] != NULL)
        {
            return m_apkNeighborLists[i];
        }        
    }
}

//---------------------------------------------------------------------------
unsigned int 
NiNeighborManager::CountNeighbors(const NiStripifyTriangle* pkTriangle)
{
    unsigned int e, uiNeighbors = 0;

    for (e = 0; e < 3; e++)
    {
        NiStripifyEdge* pkEdge = pkTriangle->GetNeighborEdge(e);

        if (pkEdge != NULL && pkEdge->GetTriangleCount() > 0)
        {
            uiNeighbors++;
        }
    }

    return uiNeighbors;
}

//---------------------------------------------------------------------------
void NiNeighborManager::Remove(NiStripifyTriangle* pkTriangle)
{
    if (pkTriangle->m_pkNext != NULL)
    {
        pkTriangle->m_pkNext->m_pkPrev = pkTriangle->m_pkPrev;
    }

    if (pkTriangle->m_pkPrev == NULL)
    {
        unsigned int i;

        for (i = 0; i < 4; i++)
        {
            if (m_apkNeighborLists[i] == pkTriangle)
            {
                m_apkNeighborLists[i] = pkTriangle->m_pkNext;
                break;
            }
        }
    }
    else
    {
        pkTriangle->m_pkPrev->m_pkNext = pkTriangle->m_pkNext;
    }
}

//---------------------------------------------------------------------------
void NiNeighborManager::AdjustNeighbors(NiStripifyTriangle* pkTriangle)
{
    unsigned int e, i;

    for (e = 0; e < 3; e++)
    {
        NiStripifyEdge* pkEdge = pkTriangle->GetNeighborEdge(e);
        if (pkEdge != NULL)
        {
            for (i = 0; i < pkEdge->GetTriangleCount(); i++)
            {
                Adjust(pkEdge->GetTriangle(i));
            }
        }
    }
}
