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

#include "NiMainPCH.h"
#include "NiScreenElementsData.h"

NiImplementRTTI(NiScreenElementsData, NiTriShapeData);

const unsigned short NiScreenElementsData::ms_usInvalid = (unsigned short)~0;

//---------------------------------------------------------------------------
NiScreenElementsData::NiScreenElementsData(bool bWantNormals,
    bool bWantColors, unsigned short usNumTextureSets, int iMaxPQuantity,
    int iPGrowBy, int iMaxVQuantity, int iVGrowBy, int iMaxTQuantity,
    int iTGrowBy)
{
    NIASSERT(iMaxPQuantity > 0 && iPGrowBy > 0);
    NIASSERT(iMaxVQuantity > 0 && iVGrowBy > 0);
    NIASSERT(iMaxTQuantity > 0 && iTGrowBy > 0);

    // polygons
    m_usMaxPQuantity =
        (unsigned short)(iMaxPQuantity > 0 ? iMaxPQuantity : 1);
    SetPGrowBy(iPGrowBy);
    m_usPQuantity = 0;
    m_akPolygon = NiAlloc(Polygon, m_usMaxPQuantity);
    m_ausPIndexer = NiAlloc(unsigned short,m_usMaxPQuantity);
    memset(m_ausPIndexer, 0xFF, m_usMaxPQuantity * sizeof(unsigned short));

    // vertices
    m_usMaxVQuantity =
        (unsigned short)(iMaxVQuantity > 0 ? iMaxVQuantity : 1);
    SetVGrowBy(iVGrowBy);
    SetVertexCount(0);
    m_pkVertex = NiNew NiPoint3[m_usMaxVQuantity];

    // normals
    if (bWantNormals)
    {
        m_pkNormal = NiNew NiPoint3[m_usMaxVQuantity];
        for (unsigned short i = 0; i < m_usMaxVQuantity; i++)
            m_pkNormal[i] = -NiPoint3::UNIT_Z;
    }
    else
    {
        m_pkNormal = 0;
    }

    // colors (initial maximum storage assumes all rectangles)
    m_pkColor = (bWantColors ? NiNew NiColorA[m_usMaxVQuantity] : 0);

    // texture coordinates (initial maximum storage assumes all rectangles)
    if (usNumTextureSets > 0)
    {
        int iQuantity = (int)usNumTextureSets * (int)m_usMaxVQuantity;
        m_pkTexture = NiNew NiPoint2[iQuantity];
        SetNumTextureSets(usNumTextureSets);
    }
    else
    {
        m_pkTexture = 0;
    }

    // triangles (initial maximum storage assumes all rectangles)
    m_usMaxIQuantity =
        (unsigned short)(iMaxTQuantity > 0 ? 3 * iMaxTQuantity : 3);
    SetTGrowBy(iTGrowBy);
    SetTriangleCount(0);
    m_uiTriListLength = 0;
    m_pusTriList = NiAlloc(unsigned short,m_usMaxIQuantity);

    m_bBoundNeedsUpdate = false;
}
//---------------------------------------------------------------------------
NiScreenElementsData::NiScreenElementsData()
{
    m_akPolygon = 0;
    m_ausPIndexer = 0;
    m_usMaxPQuantity = 0;
    m_usPGrowBy = 0;
    m_usPQuantity = 0;
    m_usMaxVQuantity = 0;
    m_usVGrowBy = 0;
    m_usMaxIQuantity = 0;
    m_usIGrowBy = 0;
    m_bBoundNeedsUpdate = false;
}
//---------------------------------------------------------------------------
NiScreenElementsData::~NiScreenElementsData()
{
    NiFree( m_akPolygon);
    NiFree( m_ausPIndexer);
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::IsValid(int iPolygon) const
{
    return 0 <= iPolygon && iPolygon < (int)m_usMaxPQuantity &&
        m_ausPIndexer[iPolygon] != ms_usInvalid;
}
//---------------------------------------------------------------------------
int NiScreenElementsData::Insert(unsigned short usNumVertices,
    unsigned short usNumTriangles, const unsigned short* ausTriList)
{
    NIASSERT(usNumVertices >= 3);
    if (usNumVertices < 3)
        return -1;

    NIASSERT(usNumTriangles == 0 || ausTriList);
    if (usNumTriangles > 0 && !ausTriList)
        return -1;

    // If the input number of triangles is zero, the polygon is assumed to be
    // convex.  A triangle fan is used to represent it.
    if (usNumTriangles == 0)
        usNumTriangles = usNumVertices - 2;

    // resize polygon array (if necessary)
    unsigned short usPolygon = m_usPQuantity++;
    unsigned short usNewMaxQuantity;
    int iNumBytes;
    unsigned int uiDestBytes;
    if (m_usPQuantity > m_usMaxPQuantity)
    {
        usNewMaxQuantity = m_usMaxPQuantity + m_usPGrowBy;

        Polygon* akNewPolygon = NiAlloc(Polygon,usNewMaxQuantity);
        iNumBytes = m_usMaxPQuantity * sizeof(Polygon);
        NiMemcpy(akNewPolygon, m_akPolygon, iNumBytes);
        NiFree( m_akPolygon);
        m_akPolygon = akNewPolygon;

        unsigned short* ausNewPIndexer = NiAlloc(unsigned short,
            usNewMaxQuantity);
        uiDestBytes = usNewMaxQuantity * sizeof(unsigned short);
        iNumBytes = m_usMaxPQuantity * sizeof(unsigned short);
        NiMemcpy(ausNewPIndexer, uiDestBytes, m_ausPIndexer, iNumBytes);
        iNumBytes =
            (usNewMaxQuantity - m_usMaxPQuantity) * sizeof(unsigned short);
        NIASSERT(iNumBytes > 0);
        memset(&ausNewPIndexer[m_usMaxPQuantity], 0xFF, iNumBytes);
        NiFree( m_ausPIndexer);
        m_ausPIndexer = ausNewPIndexer;

        m_usMaxPQuantity = usNewMaxQuantity;
    }

    // create the polygon
    Polygon& kPoly = m_akPolygon[usPolygon];
    kPoly.m_usNumVertices = usNumVertices;
    kPoly.m_usVOffset = m_usVertices;
    kPoly.m_usNumTriangles = usNumTriangles;
    kPoly.m_usIOffset = 3 * m_usTriangles;

    // Find the first available polygon handle for the new index.
    unsigned short i;
    for (i = 0; i < m_usPQuantity; i++)
    {
        if (m_ausPIndexer[i] == ms_usInvalid)
        {
            m_ausPIndexer[i] = usPolygon;
            usPolygon = i;
            break;
        }
    }
    NIASSERT(i <= m_usMaxPQuantity);

    // resize vertex arrays (if necessary)
    SetVertexCount(m_usVertices + kPoly.m_usNumVertices);
    int iDelta = (int)m_usVertices - (int)m_usMaxVQuantity;
    unsigned short usChunks;
    float fRatio;
    if (iDelta > 0)
    {
        fRatio = (float)iDelta / (float)m_usVGrowBy;
        usChunks = 1 + (unsigned short)(fRatio + 0.5f);
        usNewMaxQuantity = m_usMaxVQuantity + usChunks * m_usVGrowBy;

        // resize vertices
        NiPoint3* akNewVertex = NiNew NiPoint3[usNewMaxQuantity];
        uiDestBytes = usNewMaxQuantity * sizeof(NiPoint3);
        iNumBytes = m_usMaxVQuantity * sizeof(NiPoint3);
        NiMemcpy(akNewVertex, uiDestBytes, m_pkVertex, iNumBytes);
        NiDelete[] m_pkVertex;
        m_pkVertex = akNewVertex;

        // resize normals
        if (m_pkNormal)
        {
            NiPoint3* akNewNormal = NiNew NiPoint3[usNewMaxQuantity];
            uiDestBytes = usNewMaxQuantity * sizeof(NiPoint3);
            NiMemcpy(akNewNormal, uiDestBytes, m_pkNormal, iNumBytes);
            NiDelete[] m_pkNormal;
            m_pkNormal = akNewNormal;
            unsigned short i;
            for (i = m_usMaxVQuantity; i < usNewMaxQuantity; i++)
                m_pkNormal[i] = -NiPoint3::UNIT_Z;
        }

        // resize colors
        if (m_pkColor)
        {
            NiColorA* akNewColor = NiNew NiColorA[usNewMaxQuantity];
            iNumBytes = m_usMaxVQuantity * sizeof(NiColorA);
            uiDestBytes = usNewMaxQuantity * sizeof(NiColorA);
            NiMemcpy(akNewColor, uiDestBytes, m_pkColor, iNumBytes);
            NiDelete[] m_pkColor;
            m_pkColor = akNewColor;
        }

        // Resize texture coordinates.  The structure of the code is more
        // complicated than that of vertices, normals, and colors.  The
        // texture coordinate array stores multiple subarrays of coordinates.
        // The entire array must be resized, but each old subarray must be
        // copied into the corresponding new subarray.
        unsigned short usNumSets = GetTextureSets();
        if (usNumSets > 0)
        {
            int iNewMaxQuantity = (int)usNumSets * (int)usNewMaxQuantity;
            NiPoint2* akNewTexture = NiNew NiPoint2[iNewMaxQuantity];
            iNumBytes = m_usMaxVQuantity * sizeof(NiPoint2);
            for (unsigned short usSet = 0; usSet < usNumSets; usSet++)
            {
                int iOffset = (int)m_usVertices * (int)usSet;
                NiPoint2* akDstTexture = &akNewTexture[iOffset];
                NiPoint2* akSrcTexture = &m_pkTexture[iOffset];
                NiMemcpy(akDstTexture, akSrcTexture, iNumBytes);
            }
            NiDelete[] m_pkTexture;
            m_pkTexture = akNewTexture;
        }

        m_usMaxVQuantity = usNewMaxQuantity;
    }

    // resize index array (if necessary)
    SetTriangleCount(m_usTriangles + usNumTriangles);
    m_uiTriListLength += 3 * usNumTriangles;
    iDelta = 3 * (int)m_usTriangles - (int)m_usMaxIQuantity;
    if (iDelta > 0)
    {
        fRatio = (float)iDelta / (float)m_usIGrowBy;
        usChunks = 1 + (unsigned short)(fRatio + 0.5f);
        usNewMaxQuantity = m_usMaxIQuantity + usChunks * m_usIGrowBy;

        unsigned short* ausNewTriList = NiAlloc(unsigned short,
            usNewMaxQuantity);
        iNumBytes = m_usMaxIQuantity * sizeof(unsigned short);
        uiDestBytes = usNewMaxQuantity * sizeof(unsigned short);
        NiMemcpy(ausNewTriList, uiDestBytes, m_pusTriList, iNumBytes);
        NiFree( m_pusTriList);
        m_pusTriList = ausNewTriList;

        m_usMaxIQuantity = usNewMaxQuantity;
    }

    // Insert the polygon index array.  It is important to use the values
    // kPoly.m_usVOffset and kPoly.m_usIOffset here instead of m_usVertices
    // and m_usTriangles, since the latter quantities were incremented when
    // resizing the vertex and index arrays.
    unsigned short* pusTriList = &m_pusTriList[kPoly.m_usIOffset];
    if (ausTriList)
    {
        // simple polygon, indices are provided by the caller
        iNumBytes = usNumTriangles * 3 * sizeof(unsigned short);
        NiMemcpy(pusTriList, ausTriList, iNumBytes);

        // Copy over each index adding in the polygon offset
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < (unsigned int)usNumTriangles * 3; uiLoop++)
        {
            pusTriList[uiLoop] = ausTriList[uiLoop] + kPoly.m_usVOffset;
        }
    }
    else
    {
        // convex polygon, use a triangle fan
        for (i = 0; i < usNumTriangles; i++)
        {
            *pusTriList++ = kPoly.m_usVOffset;
            *pusTriList++ = kPoly.m_usVOffset + i + 1;
            *pusTriList++ = kPoly.m_usVOffset + i + 2;
        }
    }

    MarkAsChanged(VERTEX_MASK);
    return (int)usPolygon;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::Remove(int iPolygon)
{
    if (!IsValid(iPolygon))
        return false;

    // Update the indexer array that maps handles to array indices.  First,
    // invalidate the caller's handle.
    unsigned short usTarget = m_ausPIndexer[iPolygon];
    m_ausPIndexer[iPolygon] = ms_usInvalid;

    // Second, decrement all indices that are larger than the target index.
    // This reflects the fact that the polygons will be shifted left by one
    // to maintain a compact polygon array.
    unsigned short i, usVisited = 1;
    for (i = 0; usVisited < m_usPQuantity && i < m_usMaxPQuantity; i++)
    {
        if (m_ausPIndexer[i] != ms_usInvalid)
        {
            usVisited++;
            if (m_ausPIndexer[i] > usTarget)
                m_ausPIndexer[i]--;
        }
    }

    if (m_usPQuantity > 1)
    {
        Polygon& kDstPoly = m_akPolygon[usTarget];
        if (usTarget + 1 == m_usPQuantity)
        {
            // Removing last polygon, so no need to shift arrays.  Just
            // decrement the vertex and triangle counts.
            SetVertexCount(m_usVertices - kDstPoly.m_usNumVertices);
            SetTriangleCount(m_usTriangles - kDstPoly.m_usNumTriangles);
            m_uiTriListLength -= 3 * kDstPoly.m_usNumTriangles;
        }
        else
        {
            Polygon& kSrcPoly = m_akPolygon[usTarget + 1];

            // Shift the vertices to the left.
            int iVDst = kDstPoly.m_usVOffset;
            int iVSrc = kSrcPoly.m_usVOffset;
            NIASSERT(iVSrc - iVDst == kDstPoly.m_usNumVertices);
            int iVRem = (int)m_usVertices - iVSrc;
            int iNumBytes = iVRem * sizeof(NiPoint3);
            memmove(&m_pkVertex[iVDst], &m_pkVertex[iVSrc], iNumBytes);
            SetVertexCount(m_usVertices - kDstPoly.m_usNumVertices);

            // Shift the normals to the left.
            if (m_pkNormal)
                memmove(&m_pkNormal[iVDst], &m_pkNormal[iVSrc], iNumBytes);

            // Shift the colors to the left.
            if (m_pkColor)
            {
                iNumBytes = iVRem * sizeof(NiColorA);
                memmove(&m_pkColor[iVDst], &m_pkColor[iVSrc], iNumBytes);
            }

            // Shift the texture coordinates to the left.
            unsigned short usNumSets = GetTextureSets();
            if (usNumSets)
            {
                iNumBytes = iVRem * sizeof(NiPoint2);
                for (unsigned short usSet = 0; usSet < usNumSets; usSet++)
                {
                    NiPoint2* akTexture = GetTextureSet(usSet);
                    memmove(&akTexture[iVDst], &akTexture[iVSrc], iNumBytes);
                }
            }

            // Shift the triangle index array to the left.
            int iIDst = kDstPoly.m_usIOffset;
            int iISrc = kSrcPoly.m_usIOffset;
            NIASSERT(iISrc - iIDst == 3 * kDstPoly.m_usNumTriangles);
            int iIRem = 3 * (int)m_usTriangles - (int)iISrc;
            iNumBytes = iIRem * sizeof(unsigned short);
            memmove(&m_pusTriList[iIDst], &m_pusTriList[iISrc], iNumBytes);
            SetTriangleCount(m_usTriangles - kDstPoly.m_usNumTriangles);
            m_uiTriListLength -= 3 * kDstPoly.m_usNumTriangles;

            // Adjust the index values to account for the shift in the vertex
            // arrays.
            unsigned short* pusTriList = &m_pusTriList[iIDst];
            for (int j = 0; j < iIRem; j++)
            {
                *pusTriList -= kDstPoly.m_usNumVertices;
                pusTriList++;
            }

            // Shift the polygon array left by one to maintain a compact
            // array.
            for (i = usTarget; i + 1 < m_usPQuantity; i++)
            {
                Polygon& kDst = m_akPolygon[i];
                Polygon& kSrc = m_akPolygon[i + 1];

                kDst.m_usVOffset = kSrc.m_usVOffset - kDst.m_usNumVertices;
                kDst.m_usIOffset = kSrc.m_usIOffset -
                    3 * kDst.m_usNumTriangles;
                kDst.m_usNumVertices = kSrc.m_usNumVertices;
                kDst.m_usNumTriangles = kSrc.m_usNumTriangles;
            }
        }
    }
    else  // m_usPQuantity == 1
    {
        SetVertexCount(0);
        SetTriangleCount(0);
        m_uiTriListLength = 0;
    }

    m_usPQuantity--;
    MarkAsChanged(DIRTY_MASK);
    m_bBoundNeedsUpdate = true;
    return true;
}
//---------------------------------------------------------------------------
void NiScreenElementsData::RemoveAll()
{
    if (m_usPQuantity > 0)
    {
        memset(m_akPolygon, 0, m_usPQuantity * sizeof(Polygon));
        m_usPQuantity = 0;
        SetVertexCount(0);
        SetTriangleCount(0);
        MarkAsChanged(DIRTY_MASK);
        m_bBoundNeedsUpdate = true;
    }
}
//---------------------------------------------------------------------------
int NiScreenElementsData::GetNumVertices(int iPolygon) const
{
    if (IsValid(iPolygon))
        return (int)GetPolygon(iPolygon).m_usNumVertices;

    return 0;
}
//---------------------------------------------------------------------------
int NiScreenElementsData::GetNumTriangles(int iPolygon) const
{
    if (IsValid(iPolygon))
        return (int)GetPolygon(iPolygon).m_usNumTriangles;

    return 0;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetVertex(int iPolygon, int iVertex,
    const NiPoint2& kValue)
{
    if (IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (0 <= iVertex && iVertex < (int)kPoly.m_usNumVertices)
        {
            int i = iVertex + (int)kPoly.m_usVOffset;
            m_pkVertex[i].x = kValue.x;
            m_pkVertex[i].y = kValue.y;
            m_pkVertex[i].z = 0.0f;
            MarkAsChanged(VERTEX_MASK);
            m_bBoundNeedsUpdate = true;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetVertex(int iPolygon, int iVertex,
    NiPoint2& kValue) const
{
    if (IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (0 <= iVertex && iVertex < (int)kPoly.m_usNumVertices)
        {
            int i = iVertex + (int)kPoly.m_usVOffset;
            kValue.x = m_pkVertex[i].x;
            kValue.y = m_pkVertex[i].y;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetVertices(int iPolygon, const NiPoint2* akValue)
{
    if (IsValid(iPolygon) && akValue)
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        NiPoint3* akVertex = &m_pkVertex[kPoly.m_usVOffset];
        for (int i = 0; i < (int)kPoly.m_usNumVertices; i++)
        {
            akVertex[i].x = akValue[i].x;
            akVertex[i].y = akValue[i].y;
            akVertex[i].z = 0.0f;
        }
        MarkAsChanged(VERTEX_MASK);
        m_bBoundNeedsUpdate = true;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetVertices(int iPolygon, NiPoint2* akValue) const
{
    if (IsValid(iPolygon) && akValue)
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        NiPoint3* akVertex = &m_pkVertex[kPoly.m_usVOffset];
        for (int i = 0; i < (int)kPoly.m_usNumVertices; i++)
        {
            akValue[i].x = akVertex[i].x;
            akValue[i].y = akVertex[i].y;
        }
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetRectangle(int iPolygon, float fLeft,
    float fTop, float fWidth, float fHeight)
{
    if (IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices == 4)
        {
            float fRight = (fLeft + fWidth);
            float fBottom = (fTop + fHeight);

            int i = (int)kPoly.m_usVOffset;
            m_pkVertex[i].x = fLeft;
            m_pkVertex[i].y = fTop;
            m_pkVertex[i].z = 0.0f;
            i++;

            m_pkVertex[i].x = fLeft;
            m_pkVertex[i].y = fBottom;
            m_pkVertex[i].z = 0.0f;
            i++;

            m_pkVertex[i].x = fRight;
            m_pkVertex[i].y = fBottom;
            m_pkVertex[i].z = 0.0f;
            i++;

            m_pkVertex[i].x = fRight;
            m_pkVertex[i].y = fTop;
            m_pkVertex[i].z = 0.0f;
            i++;

            MarkAsChanged(VERTEX_MASK);
            m_bBoundNeedsUpdate = true;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetRectangle(int iPolygon, float& fLeft,
    float& fTop, float& fWidth, float& fHeight) const
{
    if (IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices == 4)
        {
            int i = (int)kPoly.m_usVOffset;
            fLeft = m_pkVertex[i].x;
            fTop = m_pkVertex[i].y;
            i += 2;
            fWidth = m_pkVertex[i].x - fLeft;
            fHeight = m_pkVertex[i].y - fTop;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetNormal(int iPolygon, int iVertex,
    const NiPoint3& kValue)
{
    if (m_pkNormal && IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (0 <= iVertex && iVertex < (int)kPoly.m_usNumVertices)
        {
            int i = iVertex + (int)kPoly.m_usVOffset;
            m_pkNormal[i] = kValue;
            MarkAsChanged(NORMAL_MASK);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetNormal(int iPolygon, int iVertex,
    NiPoint3& kValue) const
{
    if (m_pkNormal && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (0 <= iVertex && iVertex < (int)kPoly.m_usNumVertices)
        {
            int i = iVertex + (int)kPoly.m_usVOffset;
            kValue = m_pkNormal[i];
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetNormals(int iPolygon, const NiPoint3* akValue)
{
    if (m_pkNormal && IsValid(iPolygon) && akValue)
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            NiPoint3* akNormal = &m_pkNormal[kPoly.m_usVOffset];
            int iNumBytes = kPoly.m_usNumVertices * sizeof(NiPoint3);
            NiMemcpy(akNormal, akValue, iNumBytes);
            MarkAsChanged(NORMAL_MASK);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetNormals(int iPolygon,
    const NiPoint3& kCommonValue)
{
    if (m_pkNormal && IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            NiPoint3* akNormal = &m_pkNormal[kPoly.m_usVOffset];
            for (unsigned short i = 0; i < kPoly.m_usNumVertices; i++)
                akNormal[i] = kCommonValue;
            MarkAsChanged(NORMAL_MASK);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetNormals(int iPolygon, NiPoint3* akValue) const
{
    if (m_pkNormal && IsValid(iPolygon) && akValue)
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            const NiPoint3* akNormal = &m_pkNormal[kPoly.m_usVOffset];
            int iNumBytes = kPoly.m_usNumVertices * sizeof(NiPoint3);
            NiMemcpy(akValue, akNormal, iNumBytes);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetNormals(int iPolygon,
    const NiPoint3& kUpperLeft, const NiPoint3& kLowerLeft,
    const NiPoint3& kLowerRight, const NiPoint3& kUpperRight)
{
    if (m_pkNormal && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices == 4)
        {
            NiPoint3* akNormal = &m_pkNormal[kPoly.m_usVOffset];
            akNormal[0] = kUpperLeft;
            akNormal[1] = kLowerLeft;
            akNormal[2] = kLowerRight;
            akNormal[3] = kUpperRight;
            MarkAsChanged(NORMAL_MASK);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetNormals(int iPolygon, NiPoint3& kUpperLeft,
    NiPoint3& kLowerLeft, NiPoint3& kLowerRight, NiPoint3& kUpperRight) const
{
    if (m_pkNormal && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices == 4)
        {
            const NiPoint3* akNormal = &m_pkNormal[kPoly.m_usVOffset];
            kUpperLeft = akNormal[0];
            kLowerLeft = akNormal[1];
            kLowerRight = akNormal[2];
            kUpperRight = akNormal[3];
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetColor(int iPolygon, int iVertex,
    const NiColorA& kValue)
{
    if (m_pkColor && IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (0 <= iVertex && iVertex < (int)kPoly.m_usNumVertices)
        {
            int i = iVertex + (int)kPoly.m_usVOffset;
            m_pkColor[i] = kValue;
            MarkAsChanged(COLOR_MASK);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetColor(int iPolygon, int iVertex,
    NiColorA& kValue) const
{
    if (m_pkColor && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (0 <= iVertex && iVertex < (int)kPoly.m_usNumVertices)
        {
            int i = iVertex + (int)kPoly.m_usVOffset;
            kValue = m_pkColor[i];
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetColors(int iPolygon, const NiColorA* akValue)
{
    if (m_pkColor && IsValid(iPolygon) && akValue)
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            NiColorA* akColor = &m_pkColor[kPoly.m_usVOffset];
            int iNumBytes = kPoly.m_usNumVertices * sizeof(NiColorA);
            NiMemcpy(akColor, akValue, iNumBytes);
            MarkAsChanged(COLOR_MASK);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetColors(int iPolygon,
    const NiColorA& kCommonValue)
{
    if (m_pkColor && IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            NiColorA* akColor = &m_pkColor[kPoly.m_usVOffset];
            for (unsigned short i = 0; i < kPoly.m_usNumVertices; i++)
                akColor[i] = kCommonValue;
            MarkAsChanged(COLOR_MASK);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetColors(int iPolygon, NiColorA* akValue) const
{
    if (m_pkColor && IsValid(iPolygon) && akValue)
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            const NiColorA* akColor = &m_pkColor[kPoly.m_usVOffset];
            int iNumBytes = kPoly.m_usNumVertices * sizeof(NiColorA);
            NiMemcpy(akValue, akColor, iNumBytes);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetColors(int iPolygon,
    const NiColorA& kUpperLeft, const NiColorA& kLowerLeft,
    const NiColorA& kLowerRight, const NiColorA& kUpperRight)
{
    if (m_pkColor && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices == 4)
        {
            NiColorA* akColor = &m_pkColor[kPoly.m_usVOffset];
            akColor[0] = kUpperLeft;
            akColor[1] = kLowerLeft;
            akColor[2] = kLowerRight;
            akColor[3] = kUpperRight;
            MarkAsChanged(COLOR_MASK);
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetColors(int iPolygon, NiColorA& kUpperLeft,
    NiColorA& kLowerLeft, NiColorA& kLowerRight, NiColorA& kUpperRight) const
{
    if (m_pkColor && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices == 4)
        {
            const NiColorA* akColor = &m_pkColor[kPoly.m_usVOffset];
            kUpperLeft = akColor[0];
            kLowerLeft = akColor[1];
            kLowerRight = akColor[2];
            kUpperRight = akColor[3];
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetTexture(int iPolygon, int iVertex,
    unsigned short usSet, const NiPoint2& kValue)
{
    if (m_pkTexture && IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (usSet < GetTextureSets())
        {
            NiPoint2* akTexture = GetTextureSet(usSet);
            if (0 <= iVertex && iVertex < (int)kPoly.m_usNumVertices)
            {
                int i = iVertex + (int)kPoly.m_usVOffset;
                akTexture[i] = kValue;
                MarkAsChanged(TEXTURE_MASK);
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetTexture(int iPolygon, int iVertex,
    unsigned short usSet, NiPoint2& kValue) const
{
    if (m_pkTexture && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (usSet < GetTextureSets())
        {
            const NiPoint2* akTexture = GetTextureSet(usSet);
            if (0 <= iVertex && iVertex < (int)kPoly.m_usNumVertices)
            {
                int i = iVertex + (int)kPoly.m_usVOffset;
                kValue = akTexture[i];
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetTextures(int iPolygon, unsigned short usSet,
    const NiPoint2* akValue)
{
    if (m_pkTexture && IsValid(iPolygon) && akValue)
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            if (usSet < GetTextureSets())
            {
                NiPoint2* akTSet = GetTextureSet(usSet);
                NiPoint2* akTexture = &akTSet[kPoly.m_usVOffset];
                int iNumBytes = kPoly.m_usNumVertices * sizeof(NiPoint2);
                NiMemcpy(akTexture, akValue, iNumBytes);
                MarkAsChanged(TEXTURE_MASK);
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetTextures(int iPolygon, unsigned short usSet,
    NiPoint2* akValue) const
{
    if (m_pkTexture && IsValid(iPolygon) && akValue)
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            if (usSet < GetTextureSets())
            {
                const NiPoint2* akTSet = GetTextureSet(usSet);
                const NiPoint2* akTexture = &akTSet[kPoly.m_usVOffset];
                int iNumBytes = kPoly.m_usNumVertices * sizeof(NiPoint2);
                NiMemcpy(akValue, akTexture, iNumBytes);
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetTextures(int iPolygon, unsigned short usSet,
    float fLeft, float fTop, float fRight, float fBottom)
{
    if (m_pkTexture && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices == 4)
        {
            if (usSet < GetTextureSets())
            {
                NiPoint2* akTSet = GetTextureSet(usSet);
                NiPoint2* akTexture = &akTSet[kPoly.m_usVOffset];
                akTexture[0].x = fLeft;
                akTexture[0].y = fTop;
                akTexture[1].x = fLeft;
                akTexture[1].y = fBottom;
                akTexture[2].x = fRight;
                akTexture[2].y = fBottom;
                akTexture[3].x = fRight;
                akTexture[3].y = fTop;
                MarkAsChanged(TEXTURE_MASK);
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::SetTextures(int iPolygon, unsigned short usSet,
    const NiPoint2& kUpperLeft, const NiPoint2& kLowerLeft,
    const NiPoint2& kLowerRight, const NiPoint2& kUpperRight)
{
    if (m_pkTexture && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices == 4)
        {
            if (usSet < GetTextureSets())
            {
                NiPoint2* akTSet = GetTextureSet(usSet);
                NiPoint2* akTexture = &akTSet[kPoly.m_usVOffset];
                akTexture[0] = kUpperLeft;
                akTexture[1] = kLowerLeft;
                akTexture[2] = kLowerRight;
                akTexture[3] = kUpperRight;
                MarkAsChanged(TEXTURE_MASK);
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::GetTextures(int iPolygon, unsigned short usSet,
    NiPoint2& kUpperLeft, NiPoint2& kLowerLeft, NiPoint2& kLowerRight,
    NiPoint2& kUpperRight) const
{
    if (m_pkTexture && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices == 4)
        {
            if (usSet < GetTextureSets())
            {
                const NiPoint2* akTSet = GetTextureSet(usSet);
                const NiPoint2* akTexture = &akTSet[kPoly.m_usVOffset];
                kUpperLeft = akTexture[0];
                kLowerLeft = akTexture[1];
                kLowerRight = akTexture[2];
                kUpperRight = akTexture[3];
                return true;
            }
        }
    }
    return false;
}
//---------------------------------------------------------------------------
NiPoint3* NiScreenElementsData::GetVertices(int iPolygon)
{
    if (IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pkVertex[kPoly.m_usVOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
const NiPoint3* NiScreenElementsData::GetVertices(int iPolygon) const
{
    if (IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pkVertex[kPoly.m_usVOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
NiPoint3* NiScreenElementsData::GetNormals(int iPolygon)
{
    if (m_pkNormal && IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pkNormal[kPoly.m_usVOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
const NiPoint3* NiScreenElementsData::GetNormals(int iPolygon) const
{
    if (m_pkNormal && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pkNormal[kPoly.m_usVOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
NiColorA* NiScreenElementsData::GetColors(int iPolygon)
{
    if (m_pkColor && IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pkColor[kPoly.m_usVOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
const NiColorA* NiScreenElementsData::GetColors(int iPolygon) const
{
    if (m_pkColor && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pkColor[kPoly.m_usVOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
NiPoint2* NiScreenElementsData::GetTextures(int iPolygon,
    unsigned short usSet)
{
    if (m_pkTexture && IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            if (usSet < GetTextureSets())
            {
                NiPoint2* akTexture = GetTextureSet(usSet);
                return &akTexture[kPoly.m_usVOffset];
            }
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
const NiPoint2* NiScreenElementsData::GetTextures(int iPolygon,
    unsigned short usSet) const
{
    if (m_pkTexture && IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
        {
            if (usSet < GetTextureSets())
            {
                const NiPoint2* akTexture = GetTextureSet(usSet);
                return &akTexture[kPoly.m_usVOffset];
            }
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
unsigned short* NiScreenElementsData::GetIndices(int iPolygon)
{
    if (IsValid(iPolygon))
    {
        Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pusTriList[kPoly.m_usIOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
const unsigned short* NiScreenElementsData::GetIndices(int iPolygon) const
{
    if (IsValid(iPolygon))
    {
        const Polygon& kPoly = GetPolygon(iPolygon);
        if (kPoly.m_usNumVertices > 0)
            return &m_pusTriList[kPoly.m_usIOffset];
    }
    return 0;
}
//---------------------------------------------------------------------------
void NiScreenElementsData::UpdateBound()
{
    m_bBoundNeedsUpdate = false;
    if (m_usVertices > 0)
        m_kBound.ComputeFromData(m_usVertices, m_pkVertex);
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::Resize(float fXScale, float fYScale)
{
    if (fXScale > 0.0f && fYScale > 0.0f)
    {
        NiPoint3* pkVertex = m_pkVertex;
        for (int i = 0; i < (int)m_usVertices; i++)
        {
            pkVertex->x = fXScale * pkVertex->x;
            pkVertex->y = fYScale * pkVertex->y;
            pkVertex++;
        }
        MarkAsChanged(VERTEX_MASK);
        m_bBoundNeedsUpdate = true;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiScreenElementsData);
//---------------------------------------------------------------------------
void NiScreenElementsData::LoadBinary(NiStream& kStream)
{
    NiTriShapeData::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_usMaxPQuantity);
    m_akPolygon = NiAlloc(Polygon, m_usMaxPQuantity);
    m_ausPIndexer = NiAlloc(unsigned short,m_usMaxPQuantity);

    unsigned int uiMaxPQuantity = (unsigned int)m_usMaxPQuantity;
    NiStreamLoadBinary(kStream, m_akPolygon, uiMaxPQuantity);
    NiStreamLoadBinary(kStream, m_ausPIndexer, uiMaxPQuantity);
    NiStreamLoadBinary(kStream, m_usPGrowBy);
    NiStreamLoadBinary(kStream, m_usPQuantity);
    NiStreamLoadBinary(kStream, m_usMaxVQuantity);
    NiStreamLoadBinary(kStream, m_usVGrowBy);
    NiStreamLoadBinary(kStream, m_usMaxIQuantity);
    NiStreamLoadBinary(kStream, m_usIGrowBy);
}
//---------------------------------------------------------------------------
void NiScreenElementsData::LinkObject(NiStream& kStream)
{
    NiTriShapeData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::RegisterStreamables(NiStream& kStream)
{
    return NiTriShapeData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiScreenElementsData::SaveBinary(NiStream& kStream)
{
    NiTriShapeData::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_usMaxPQuantity);

    unsigned int uiMaxPQuantity = (unsigned int)m_usMaxPQuantity;
    NiStreamSaveBinary(kStream, m_akPolygon, uiMaxPQuantity);
    NiStreamSaveBinary(kStream, m_ausPIndexer, uiMaxPQuantity);
    NiStreamSaveBinary(kStream, m_usPGrowBy);
    NiStreamSaveBinary(kStream, m_usPQuantity);
    NiStreamSaveBinary(kStream, m_usMaxVQuantity);
    NiStreamSaveBinary(kStream, m_usVGrowBy);
    NiStreamSaveBinary(kStream, m_usMaxIQuantity);
    NiStreamSaveBinary(kStream, m_usIGrowBy);
}
//---------------------------------------------------------------------------
bool NiScreenElementsData::IsEqual(NiObject* pkObject)
{
    if (!NiTriShapeData::IsEqual(pkObject))
        return false;

    NiScreenElementsData* pkSED = (NiScreenElementsData*)pkObject;

    // compare array and growth sizes
    if (m_usMaxPQuantity != pkSED->m_usMaxPQuantity ||
        m_usPGrowBy != pkSED->m_usPGrowBy ||
        m_usPQuantity != pkSED->m_usPQuantity ||
        m_usMaxVQuantity != pkSED->m_usMaxVQuantity ||
        m_usVGrowBy != pkSED->m_usVGrowBy ||
        m_usMaxIQuantity != pkSED->m_usMaxIQuantity ||
        m_usIGrowBy != pkSED->m_usIGrowBy)
    {
        return false;
    }

    // compare polygons
    int iBytes = m_usMaxPQuantity * sizeof(Polygon);
    if (memcmp(m_akPolygon, pkSED->m_akPolygon, iBytes) != 0)
        return false;

    // compare polygon indexing
    iBytes = m_usMaxPQuantity * sizeof(unsigned short);
    if (memcmp(m_ausPIndexer, pkSED->m_ausPIndexer, iBytes) != 0)
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiScreenElementsData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiTriShapeData::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiScreenElementsData::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("m_akPolygon", m_akPolygon));
    pkStrings->Add(NiGetViewerString("m_ausPIndexer", m_ausPIndexer));
    pkStrings->Add(NiGetViewerString("m_usMaxPQuantity", m_usMaxPQuantity));
    pkStrings->Add(NiGetViewerString("m_usPGrowBy", m_usPGrowBy));
    pkStrings->Add(NiGetViewerString("m_usPQuantity", m_usPQuantity));
    pkStrings->Add(NiGetViewerString("m_usMaxVQuantity", m_usMaxVQuantity));
    pkStrings->Add(NiGetViewerString("m_usVGrowBy", m_usVGrowBy));
    pkStrings->Add(NiGetViewerString("m_usMaxIQuantity", m_usMaxIQuantity));
    pkStrings->Add(NiGetViewerString("m_usIGrowBy", m_usIGrowBy));
}
//---------------------------------------------------------------------------
