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

#include <NiRTLib.h>

#include "NiStripifyEdge.h"

//---------------------------------------------------------------------------
NiStripifyEdge::NiStripifyEdge(unsigned short usVert0, unsigned short usVert1)
{
    // NB: Edge(v0, v1) and Edge(v1, v0) are not equal

    m_aVert[0] = usVert0;
    m_aVert[1] = usVert1;
}


//---------------------------------------------------------------------------
void NiStripifyEdge::AddTriangle(NiStripifyTriangle* pkTriangle)
{
    m_kTriangles.Add(pkTriangle);
}

//---------------------------------------------------------------------------
void NiStripifyEdge::RemoveTriangle(NiStripifyTriangle* pkTriangle)
{
    int iIndex = m_kTriangles.Find(pkTriangle);
    NIASSERT(iIndex >= 0);
    m_kTriangles.RemoveAt(iIndex);
}
