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

#include "NiTriStripsData.h"

//---------------------------------------------------------------------------
void NiTriStripsData::CalculateNormals()
{
    CreateNormals(true);
    
    unsigned short ui0, ui1, ui2;
    const unsigned short *pStripLists = m_pusStripLists;
    

    //  Cycle through all the strips
    for ( unsigned short ii = 0; ii < m_usStrips; ii++ )
    {
        for ( unsigned short jj = 2; jj < m_pusStripLengths[ii]; jj++ )
        {
            // get indices to triangle vertices (depends on curr tri)
            if ( ( jj % 2 ) == 0 )
            {
                ui0 = pStripLists[jj - 2];
                ui1 = pStripLists[jj - 1];
                ui2 = pStripLists[jj - 0];
            }
            else
            {
                ui0 = pStripLists[jj - 2];
                ui1 = pStripLists[jj - 0];
                ui2 = pStripLists[jj - 1];
            }

            // Skip degenerate triangles
            if (ui0 == ui1 ||
                ui1 == ui2 ||
                ui2 == ui0)
            {
                continue;
            }

            // compute normal vector
            NiPoint3& kVertex0 = m_pkVertex[ui0];
            NiPoint3& kVertex1 = m_pkVertex[ui1];
            NiPoint3& kVertex2 = m_pkVertex[ui2];
            NiPoint3 e1(kVertex1 - kVertex0);
            NiPoint3 e2(kVertex2 - kVertex1);
            NiPoint3 normal(e1.Cross(e2));

            NiPoint3::UnitizeVector(normal);
    
            // update the running sum of normals at the various vertices
            m_pkNormal[ui0] += normal;
            m_pkNormal[ui1] += normal;
            m_pkNormal[ui2] += normal;
        }
        
        pStripLists += m_pusStripLengths[ii];
    }
    
    NiPoint3::UnitizeVectors(m_pkNormal, m_usVertices, 
        sizeof(m_pkNormal[0]));
    MarkAsChanged(NORMAL_MASK);
}
//---------------------------------------------------------------------------
