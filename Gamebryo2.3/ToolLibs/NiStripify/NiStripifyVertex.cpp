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

#include "NiStripifyVertex.h"
#include "NiStripifyEdge.h"

//---------------------------------------------------------------------------
// return edge originating at this and terminating at usVert, if one exists.
NiStripifyEdge* NiStripifyVertex::GetEdge(unsigned short usVert)
{
    unsigned int i;

    for (i = 0; i < m_kEdges.GetSize(); i++)
    {
        NiStripifyEdge* pkEdge = m_kEdges.GetAt(i);

        NIASSERT(pkEdge->GetVert(0) == m_usVert);

        if (pkEdge->GetVert(1) == usVert)
        {
            return pkEdge;
        }
    }

    return NULL;
}

