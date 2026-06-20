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

#include <NiMemoryDefines.h>
#include "NiGeometryData.h"
#include "NiRenderer.h"
#include "NiAdditionalGeometryData.h"

NiImplementRTTI(NiGeometryData, NiObject);

// zero is not used
unsigned short NiGeometryData::ms_usNextID = 1;

//---------------------------------------------------------------------------
NiCriticalSection NiGeometryData::ms_kCritSec;
//---------------------------------------------------------------------------
NiGeometryData::NiGeometryData(unsigned short usVertices, NiPoint3* pkVertex, 
    NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture,
    unsigned short usNumTextureSets, DataFlags eNBTMethod) :
    m_pkGroup(NULL),
    m_usVertices(usVertices),
    m_pkVertex(pkVertex),
    m_pkNormal(pkNormal),
    m_pkColor(pkColor),
    m_pkTexture(pkTexture),
    m_usActiveVertices(usVertices),
    m_usDataFlags(0),
    m_usDirtyFlags(0), // marks all geometry as MUTABLE by default
    m_ucKeepFlags(0),
    m_ucCompressFlags(0),
    m_spAdditionalGeomData(0),
    m_pkBuffData(0)
{
    SetNumTextureSets(usNumTextureSets);
    SetNormalBinormalTangentMethod(eNBTMethod);

    NIMEMASSERT(NiVerifyAddress(pkVertex));
    NIMEMASSERT(NiVerifyAddress(pkNormal));
    NIMEMASSERT(NiVerifyAddress(pkColor));
    NIMEMASSERT(NiVerifyAddress(pkTexture));
    unsigned short usActiveVertices = GetActiveVertexCount();
    if (usActiveVertices > 0 && m_pkVertex != NULL)
        m_kBound.ComputeFromData(usActiveVertices, m_pkVertex);


    ms_kCritSec.Lock();
    m_usID = ms_usNextID++;
    ms_kCritSec.Unlock();
    SetConsistency(MUTABLE);
}
//---------------------------------------------------------------------------
NiGeometryData::NiGeometryData() :
    m_pkGroup(NULL),
    m_usVertices(0),
    m_pkVertex(NULL),
    m_pkNormal(NULL),
    m_pkColor(NULL),
    m_pkTexture(NULL),
    m_usActiveVertices(0),
    m_usDataFlags(0),
    m_usDirtyFlags(0), // marks all geometry as MUTABLE by default
    m_ucKeepFlags(0),
    m_ucCompressFlags(0),
    m_spAdditionalGeomData(0),
    m_pkBuffData(0)
{

    ms_kCritSec.Lock();
    m_usID = ms_usNextID++;    
    ms_kCritSec.Unlock();
    SetConsistency(MUTABLE);
}
//---------------------------------------------------------------------------
NiGeometryData::~NiGeometryData()
{
    NiRenderer::RemoveGeometryData(this);
    SetAdditionalGeometryData(NULL);

    if (GetGroup())
    {
        if (m_pkVertex)
            GetGroup()->DecRefCount();
        if (m_pkNormal)
            GetGroup()->DecRefCount();
        if (m_pkColor)
            GetGroup()->DecRefCount();
        if (m_pkTexture)
            GetGroup()->DecRefCount();
    }
    else
    {
        NiDelete[] m_pkVertex;
        NiDelete[] m_pkNormal;
        NiDelete[] m_pkColor;
        NiDelete[] m_pkTexture;
    }
}
//---------------------------------------------------------------------------
void NiGeometryData::CreateVertices(bool bInitialize)
{
    // NB: Any code that allocates the vertex array by calling this function
    // should also set the bound.

    if (!m_pkVertex)
        m_pkVertex = NiNew NiPoint3[m_usVertices];

    if (bInitialize)
        memset(m_pkVertex, 0, sizeof(m_pkVertex[0]) * m_usVertices);
}
//---------------------------------------------------------------------------
void NiGeometryData::CreateNormals(bool bInitialize)
{
    unsigned int uiFactor = 1;
    if (GetNormalBinormalTangentMethod() != NBT_METHOD_NONE)
        uiFactor = 3;

    if (!m_pkNormal)
        m_pkNormal = NiNew NiPoint3[m_usVertices * uiFactor];

    if (bInitialize)
    {
        memset(m_pkNormal, 0, 
            sizeof(m_pkNormal[0]) * m_usVertices * uiFactor);
    }
}
//---------------------------------------------------------------------------
void NiGeometryData::StoreNormalBinormalTangent(DataFlags eNBTMethod)
{
    if (GetNormalBinormalTangentMethod() == eNBTMethod)
        return;

    SetNormalBinormalTangentMethod(eNBTMethod);

    if (!m_pkNormal)
        return;

    NIASSERT(!GetGroup());

    unsigned int uiFactor = 1;
    if (eNBTMethod != NBT_METHOD_NONE)
        uiFactor = 3;

    NiPoint3* pkNormal = NiNew NiPoint3[m_usVertices * uiFactor];
    for (unsigned int uiIndex = 0; uiIndex < m_usVertices; uiIndex++)
        pkNormal[uiIndex] = m_pkNormal[uiIndex];

    if (eNBTMethod != NBT_METHOD_NONE)
    {
        memset((pkNormal + m_usVertices), 0, 
            sizeof(pkNormal[0] * m_usVertices * 2));
    }

    NiDelete[] m_pkNormal;
    m_pkNormal = pkNormal;
}
//---------------------------------------------------------------------------
void NiGeometryData::CreateColors(bool bInitialize)
{
    if (!m_pkColor)
        m_pkColor = NiNew NiColorA[m_usVertices];

    if (bInitialize)
    {
        for (unsigned short usIndex = 0; usIndex < m_usVertices; usIndex++)
            m_pkColor[usIndex] = NiColorA::BLACK;
    }
}
//---------------------------------------------------------------------------
void NiGeometryData::CreateTextures(bool bInitialize, unsigned short usSets)
{
    NIASSERT(!GetGroup());

    if (m_pkTexture && usSets != GetTextureSets())
    {
        NiDelete[] m_pkTexture;
        m_pkTexture = NULL;
    }

    unsigned int uiQuantity = usSets * m_usVertices;
    if (!m_pkTexture)
    {
        m_pkTexture = NiNew NiPoint2[uiQuantity];
        SetNumTextureSets(usSets);
    }

    if (bInitialize)
        memset(m_pkTexture, 0, sizeof(m_pkTexture[0]) * uiQuantity);
}
//---------------------------------------------------------------------------
NiPoint2* NiGeometryData::GetTextureSet(unsigned short usSet)
{
    if (!m_pkTexture || usSet >= GetTextureSets())
        return 0;

    return &m_pkTexture[m_usVertices * usSet];
}
//---------------------------------------------------------------------------
const NiPoint2* NiGeometryData::GetTextureSet(unsigned short usSet) const
{
    if (!m_pkTexture || usSet >= GetTextureSets())
        return 0;

    return &m_pkTexture[m_usVertices * usSet];
}
//---------------------------------------------------------------------------
void NiGeometryData::AppendTextureSet(NiPoint2* pkSet)
{
    NIASSERT(pkSet);
    NIASSERT(!GetGroup());

    // assert: pkSet is an array with m_usVertices elements

    unsigned int uiIndex;
    unsigned short usNumTextureSets = GetTextureSets();

    if (!m_pkTexture)
    {
        m_pkTexture = NiNew NiPoint2[m_usVertices];
        NIASSERT(m_pkTexture);
        usNumTextureSets = 1;
        for (uiIndex = 0; uiIndex < m_usVertices; uiIndex++)
            m_pkTexture[uiIndex] = pkSet[uiIndex];
    }
    else
    {
        // reallocate storage for the old and new texture coordinates
        unsigned int uiBase = m_usVertices * usNumTextureSets;
        NiPoint2* pkTexture = NiNew NiPoint2[uiBase + m_usVertices];

        for (uiIndex = 0; uiIndex < uiBase; uiIndex++)
            pkTexture[uiIndex] = m_pkTexture[uiIndex];

        NiDelete[] m_pkTexture;
        m_pkTexture = pkTexture;

        for (uiIndex = 0; uiIndex < m_usVertices; uiIndex++)
            m_pkTexture[uiBase+uiIndex] = pkSet[uiIndex];

        usNumTextureSets++;
    }

    SetNumTextureSets(usNumTextureSets);
}
//---------------------------------------------------------------------------
void NiGeometryData::Replace(unsigned short usVertices, NiPoint3* pkVertex, 
    NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture,
    unsigned short usNumTextureSets, DataFlags eNBTMethod)
{
    if (GetGroup())
    {
        NIASSERT(pkVertex == 0 && pkNormal == 0 && pkColor == 0 && 
            pkTexture == 0);

        if (pkVertex != m_pkVertex)
            GetGroup()->DecRefCount();
        if (pkNormal != m_pkNormal)
            GetGroup()->DecRefCount();
        if (pkColor != m_pkColor)
            GetGroup()->DecRefCount();
        if (pkTexture != m_pkTexture)
            GetGroup()->DecRefCount();
    }
    else
    {
        
        NIMEMASSERT(NiVerifyAddress(pkVertex));
        NIMEMASSERT(NiVerifyAddress(pkNormal));
        NIMEMASSERT(NiVerifyAddress(pkColor));
        NIMEMASSERT(NiVerifyAddress(pkTexture));

        if (pkVertex != m_pkVertex)
            NiDelete [] m_pkVertex;
        if (pkNormal != m_pkNormal)
            NiDelete [] m_pkNormal;
        if (pkColor != m_pkColor)
            NiDelete [] m_pkColor;
        if (pkTexture != m_pkTexture)
            NiDelete [] m_pkTexture;
    }

    SetVertexCount(usVertices);
    m_pkVertex = pkVertex;
    m_pkNormal = pkNormal;
    unsigned short usActiveVertices = GetActiveVertexCount();
    if (usActiveVertices && m_pkVertex != 0)
        m_kBound.ComputeFromData(usActiveVertices, m_pkVertex);
    m_pkColor = pkColor;
    m_pkTexture = pkTexture;
    SetNumTextureSets(usNumTextureSets);
    SetNormalBinormalTangentMethod(eNBTMethod);
}
//---------------------------------------------------------------------------
void NiGeometryData::SetData(unsigned short usVertices, NiPoint3* pkVertex, 
    NiPoint3* pkNormal, NiColorA* pkColor, NiPoint2* pkTexture,
    unsigned short usNumTextureSets, NiGeometryData::DataFlags eNBTMethod)
{
    NIMEMASSERT(NiVerifyAddress(pkVertex));
    NIMEMASSERT(NiVerifyAddress(pkNormal));
    NIMEMASSERT(NiVerifyAddress(pkColor));
    NIMEMASSERT(NiVerifyAddress(pkTexture));

    SetVertexCount(usVertices);
    m_pkVertex = pkVertex;
    m_pkNormal = pkNormal;
    unsigned short usActiveVertices = GetActiveVertexCount();
    if (usActiveVertices && m_pkVertex != 0)
        m_kBound.ComputeFromData(usActiveVertices, m_pkVertex);
    m_pkColor = pkColor;
    m_pkTexture = pkTexture;
    SetNumTextureSets(usNumTextureSets);
    SetNormalBinormalTangentMethod(eNBTMethod);
}
//---------------------------------------------------------------------------
void NiGeometryData::SetAdditionalGeometryData(
    NiAdditionalGeometryData* pkAddData)
{
    if (pkAddData)
    {
        NIASSERT(pkAddData->GetVertexCount() == GetVertexCount());    
    }

    m_spAdditionalGeomData = pkAddData;
}
//---------------------------------------------------------------------------
bool NiGeometryData::ContainsVertexData(NiShaderDeclaration::ShaderParameter 
    eParameter) const
{
    if (m_pkBuffData && m_pkBuffData->ContainsVertexData(eParameter))
        return true;

    switch (eParameter)
    {
        case NiShaderDeclaration::SHADERPARAM_NI_POSITION:
            return m_pkVertex != NULL;
            break;  
        case NiShaderDeclaration::SHADERPARAM_NI_NORMAL:
            return m_pkNormal != NULL;
            break;          
        case NiShaderDeclaration::SHADERPARAM_NI_COLOR:
            return m_pkColor != NULL;
            break;          
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0:
            return m_pkTexture != NULL && GetTextureSets() >= 1;
            break;      
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD1:
            return m_pkTexture != NULL && GetTextureSets() >= 2;
            break;       
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD2:
            return m_pkTexture != NULL && GetTextureSets() >= 3;
            break;       
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD3:
            return m_pkTexture != NULL && GetTextureSets() >= 4;
            break;       
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4:
            return m_pkTexture != NULL && GetTextureSets() >= 5;
            break;       
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5:
            return m_pkTexture != NULL && GetTextureSets() >= 6;
            break;       
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6:
            return m_pkTexture != NULL && GetTextureSets() >= 7;
            break;       
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7:
            return m_pkTexture != NULL && GetTextureSets() >= 8;
            break;       
        case NiShaderDeclaration::SHADERPARAM_NI_TANGENT:
        case NiShaderDeclaration::SHADERPARAM_NI_BINORMAL:
            return m_pkNormal != NULL && GetNormalBinormalTangentMethod() != 
                NiGeometryData::NBT_METHOD_NONE;
            break;         
        default:
            break;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiGeometryData::IndexSort(unsigned short*& pusIndex0,
    unsigned short*& pusIndex1, VertexMask mask) const
{
    // After a call to IndexSort, vertex[pusIndex0[i]] is the ith smallest
    // and pusIndex1[i] tells where the ith vertex ended up in the sorted 
    // array. Therefore pusIndex0[pusIndex1[i]] == i. This function allocates
    // pusIndex0 & pusIndex1. It is the calling function's responsibility to
    // delete them.

    unsigned short usVertices = GetVertexCount();
    pusIndex0 = NiAlloc(unsigned short, usVertices);
    NIASSERT(pusIndex0 != NULL);

    pusIndex1 = NiAlloc(unsigned short, usVertices);
    NIASSERT(pusIndex1 != NULL);

    unsigned short i;
    for (i = 0; i < usVertices; i++)
    {
        pusIndex0[i] = i;
    }
    
    IndexQSort(pusIndex0, mask, 0, usVertices - 1);

    for (i = 0; i < usVertices; i++)
    {
        pusIndex1[pusIndex0[i]] = i;
    }
}
//---------------------------------------------------------------------------
void NiGeometryData::IndexQSort(unsigned short *pusIndex, VertexMask mask,
    int l, int r) const
{
    if (r > l)
    {
        int i, j;

        i = l - 1;
        j = r + 1;
        int iPivot = ChoosePivot(pusIndex, mask, l, r);

        for (;;)
        {
            do 
            {
                j--;
            } while (VertexCompare(iPivot, pusIndex[j], mask) < 0);

            do
            {
                i++;
            } while (VertexCompare(pusIndex[i], iPivot, mask) < 0);

            if (i < j)
            {
                unsigned short usTmp;                
                usTmp = pusIndex[i];
                pusIndex[i] = pusIndex[j];
                pusIndex[j] = usTmp;
            }
            else
            {
                break;
            }
        }

        if (j == r)
        {
            IndexQSort(pusIndex, mask, l, j - 1);
        }
        else
        {
            IndexQSort(pusIndex, mask, l, j);
            IndexQSort(pusIndex, mask, j + 1, r);
        }
    }
}
//---------------------------------------------------------------------------
int NiGeometryData::ChoosePivot(unsigned short *pusIndex, VertexMask mask, 
    int l, int r) const
{
    // Check the first, middle, and last element. Choose the one which falls
    // between the other two. This has a good chance of discouraging 
    // quadratic behavior from qsort.
    // In the case when all three are equal, this code chooses the middle
    // element, which will prevent quadratic behavior for a list with 
    // all elements equal.

    int m = (l + r) >> 1;

    if (VertexCompare(pusIndex[l], pusIndex[m], mask) < 0)
    {
        if (VertexCompare(pusIndex[m], pusIndex[r], mask) < 0)
        {
            return pusIndex[m];
        }
        else
        {
            if (VertexCompare(pusIndex[l], pusIndex[r], mask) < 0)
                return pusIndex[r];
            else
                return pusIndex[l];
        }
    }
    else
    {
        if (VertexCompare(pusIndex[l], pusIndex[r], mask) < 0)
        {
            return pusIndex[l];
        }
        else
        {
            if (VertexCompare(pusIndex[m], pusIndex[r], mask) < 0)
                return pusIndex[r];
            else
                return pusIndex[m];
        }
    }
}
//---------------------------------------------------------------------------
static int FloatCompare(const float* pf0, const float* pf1,
    unsigned int uiNum)
{
    for (unsigned int i = 0; i < uiNum; i++)
    {
        if (pf0[i] < pf1[i])
            return -1;
        if (pf0[i] > pf1[i])
            return 1;
    }

    return 0;
}
//---------------------------------------------------------------------------
int NiGeometryData::VertexCompare(unsigned short v0, unsigned short v1,
    VertexMask mask) const
{
    // This compare function is similar to the one used by the c library
    // functions qsort and bsearch.  It also relies on the fact that
    // NiPoint3 and NiColorA members are stored as consecutive floats.

    int iCmp;

    const NiPoint3* pkVertex = GetVertices();
    if (mask & VERTEX_MASK)
    {
        iCmp = FloatCompare(&pkVertex[v0].x, &pkVertex[v1].x, 3);
        if (iCmp != 0)
            return iCmp;
    }

    const NiPoint3* pkNormal = GetNormals();
    if (pkNormal != NULL && (mask & NORMAL_MASK))
    {
        iCmp = FloatCompare(&pkNormal[v0].x, &pkNormal[v1].x, 3);
        if (iCmp != 0)
            return iCmp;
    }

    const NiColorA* pkColor = GetColors();
    if (pkColor != NULL && (mask & COLOR_MASK))
    {
        iCmp = FloatCompare(&pkColor[v0].r, &pkColor[v1].r, 4);
        if (iCmp != 0)
            return iCmp;
    }

    const NiPoint2* pkTexture = GetTextures();
    if (pkTexture != NULL && (mask & TEXTURE_MASK))
    {
        unsigned short usOffset = 0;
        unsigned short usVertices = GetVertexCount();
        unsigned short usTextureSets = GetTextureSets();
        for (unsigned short usSet = 0; usSet < usTextureSets; usSet++)
        {
            iCmp = FloatCompare(&pkTexture[v0 + usOffset].x,
                &pkTexture[v1 + usOffset].x, 2);
            if (iCmp != 0)
                return iCmp;

            usOffset += usVertices;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiGeometryData::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 114))
    {
        unsigned int uiID;
        NiStreamLoadBinary(kStream, uiID);
        m_pkGroup = kStream.GetGroupFromID(uiID);
    }

    NiStreamLoadBinary(kStream, m_usVertices);
    m_usActiveVertices = m_usVertices;

    NiStreamLoadBinary(kStream, m_ucKeepFlags);
    NiStreamLoadBinary(kStream, m_ucCompressFlags);

    // flag to indicate existence of vertices
    NiBool bHasVertex;
    NiStreamLoadBinary(kStream, bHasVertex);

    if (bHasVertex)
    {
        if (GetGroup())
        {
            m_pkVertex = (NiPoint3*)GetGroup()->Allocate(m_usVertices * 
                sizeof(NiPoint3));
        }
        else
        {
            m_pkVertex = NiNew NiPoint3[m_usVertices];
        }
        NIASSERT(m_pkVertex);
        NiPoint3::LoadBinary(kStream, m_pkVertex, m_usVertices);
    }

    //Set number of normals to number of vertices.
    unsigned int uiNumberOfNormals = m_usVertices;
    NiStreamLoadBinary(kStream, m_usDataFlags);
    //Scale by 3 if binormals and tangents were streamed.
    if ((m_usDataFlags &  NBT_METHOD_MASK) != NBT_METHOD_NONE)
        uiNumberOfNormals *= 3;

    // flag to indicate existence of normals
    NiBool bHasNormal;
    NiStreamLoadBinary(kStream, bHasNormal);

    if (bHasNormal)
    {
        if (GetGroup())
        {
            m_pkNormal = (NiPoint3*)GetGroup()->Allocate(uiNumberOfNormals * 
                sizeof(NiPoint3));
        }
        else
        {
            m_pkNormal = NiNew NiPoint3[uiNumberOfNormals];
        }
        NIASSERT(m_pkNormal);
        NiPoint3::LoadBinary(kStream, m_pkNormal, uiNumberOfNormals);
    }

    m_kBound.LoadBinary(kStream);

    // flag to indicate existence of colors
    NiBool bHasColor;
    NiStreamLoadBinary(kStream, bHasColor);

    if (bHasColor)
    {
        if (GetGroup())
        {
            m_pkColor = (NiColorA*)GetGroup()->Allocate(m_usVertices * 
                sizeof(NiColorA));
        }
        else
        {
            m_pkColor = NiNew NiColorA[m_usVertices];
        }
        NIASSERT(m_pkColor);
        NiColorA::LoadBinary(kStream, m_pkColor, m_usVertices);
    }

    if (GetTextureSets() > 0)
    {
        unsigned int uiQuantity = m_usVertices * GetTextureSets();
        if (GetGroup())
        {
            m_pkTexture = (NiPoint2*)GetGroup()->Allocate(uiQuantity * 
                sizeof(NiPoint2));
        }
        else
        {
            m_pkTexture = NiNew NiPoint2[uiQuantity];
        }
        NIASSERT(m_pkTexture);
        NiPoint2::LoadBinary(kStream, m_pkTexture, uiQuantity);
    }

    // Only data in CONSISTENCY_MASK should be present
    NiStreamLoadBinary(kStream, m_usDirtyFlags);
    NIASSERT((m_usDirtyFlags & ~CONSISTENCY_MASK) == 0);

    // m_usID is a derived quantity and must not be saved
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 3, 0, 7))
    {
        kStream.ReadLinkID(); // m_spAdditionalGeomData
    }
}
//---------------------------------------------------------------------------
void NiGeometryData::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
    
    if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 3, 0, 7))
    {
        m_spAdditionalGeomData = (NiAdditionalGeometryData*) 
            kStream.GetObjectFromLinkID();
    }
}
//---------------------------------------------------------------------------
bool NiGeometryData::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    if (m_spAdditionalGeomData && 
        !m_spAdditionalGeomData->RegisterStreamables(kStream))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiGeometryData::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    unsigned int uiID = kStream.GetIDFromGroup(m_pkGroup);
    NiStreamSaveBinary(kStream, uiID);

    NiStreamSaveBinary(kStream, m_usVertices);
    NiStreamSaveBinary(kStream, m_ucKeepFlags);
    NiStreamSaveBinary(kStream, m_ucCompressFlags);

    // flag to indicate existence of vertices
    NiBool bHasVertex = (m_pkVertex != NULL);
    NiStreamSaveBinary(kStream, bHasVertex);
    if (bHasVertex)
    {
        NiPoint3::SaveBinary(kStream, m_pkVertex, m_usVertices);
    }

    // Ensure we don't save texture sets that don't exist
    NIASSERT(GetTextureSets() == 0 || m_pkTexture != NULL);
    if (m_pkTexture == NULL)
        SetNumTextureSets(0);

    NiStreamSaveBinary(kStream, m_usDataFlags);
    unsigned int uiNumberOfNormals = m_usVertices;
    if ((m_usDataFlags & NBT_METHOD_MASK) != NBT_METHOD_NONE)
        uiNumberOfNormals *= 3;

    // flag to indicate existence of normals
    NiBool bHasNormal = (m_pkNormal != NULL);
    NiStreamSaveBinary(kStream, bHasNormal);
    if (bHasNormal)
    {
        NiPoint3::SaveBinary(kStream, m_pkNormal, uiNumberOfNormals);
    }

    m_kBound.SaveBinary(kStream);

    // flag to indicate existence of colors
    NiBool bHasColor = (m_pkColor != NULL);
    NiStreamSaveBinary(kStream, bHasColor);
    if (bHasColor)
    {
        NiColorA::SaveBinary(kStream, m_pkColor, m_usVertices);
    }

    unsigned int uiQuantity = m_usVertices * GetTextureSets();
    if (uiQuantity != 0 && m_pkTexture != NULL)
        NiPoint2::SaveBinary(kStream, m_pkTexture, uiQuantity);

    NiStreamSaveBinary(kStream, (unsigned short) (m_usDirtyFlags &
        CONSISTENCY_MASK));
    // m_usID is a derived quantity and must not be saved

    kStream.SaveLinkID(m_spAdditionalGeomData);
}
//---------------------------------------------------------------------------
bool NiGeometryData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiGeometryData* pkData = (NiGeometryData*) pkObject;
    unsigned int uiIndex;

    // vertices
    if (m_usVertices != pkData->m_usVertices)
        return false;

    if (m_usActiveVertices != pkData->m_usActiveVertices)
        return false;

    if ((m_pkVertex && !pkData->m_pkVertex)
        || (!m_pkVertex && pkData->m_pkVertex))
    {
        return false;
    }

    if (GetKeepFlags() != pkData->GetKeepFlags())
        return false;

    if (GetCompressFlags() != pkData->GetCompressFlags())
        return false;

    if (m_pkVertex)
    {
        for (uiIndex = 0; uiIndex < m_usVertices; uiIndex++)
        {
            if (m_pkVertex[uiIndex] != pkData->m_pkVertex[uiIndex])
                return false;
        }
    }

    // normals
    if ((m_pkNormal && !pkData->m_pkNormal)
        || (!m_pkNormal && pkData->m_pkNormal))
    {
        return false;
    }

    if (m_pkNormal)
    {
        if (GetNormalBinormalTangentMethod() != 
            pkData->GetNormalBinormalTangentMethod())
        {
            return false;
        }
        unsigned int uiCount = 
            (GetNormalBinormalTangentMethod() == NBT_METHOD_NONE) ? 
            m_usVertices : 3 * m_usVertices; 
        for (uiIndex = 0; uiIndex < uiCount; uiIndex++)
        {
            if (m_pkNormal[uiIndex] != pkData->m_pkNormal[uiIndex])
                return false;
        }
    }

    // bounds
    if (m_kBound != pkData->m_kBound)
        return false;

    // colors
    if ((m_pkColor && !pkData->m_pkColor) || 
        (!m_pkTexture && pkData->m_pkTexture))
    {
        return false;
    }

    if (m_pkColor)
    {
        for (uiIndex = 0; uiIndex < m_usVertices; uiIndex++)
        {
            if (m_pkColor[uiIndex] != pkData->m_pkColor[uiIndex])
                return false;
        }
    }

    // texture coordinates
    if ((m_pkTexture && !pkData->m_pkTexture) || 
       (!m_pkTexture && pkData->m_pkTexture))
    {
        return false;
    }

    if (m_pkTexture)
    {
        if (GetTextureSets() != pkData->GetTextureSets())
            return false;

        unsigned int uiQuantity = m_usVertices * GetTextureSets();
        for (uiIndex = 0; uiIndex < uiQuantity; uiIndex++)
        {
            if (m_pkTexture[uiIndex] != pkData->m_pkTexture[uiIndex])
                return false;
        }
    }

    if ((m_spAdditionalGeomData && !pkData->m_spAdditionalGeomData) || 
       (!m_spAdditionalGeomData && pkData->m_spAdditionalGeomData))
    {
        return false;
    }
    
    if (m_spAdditionalGeomData && pkData->m_spAdditionalGeomData && 
        !(m_spAdditionalGeomData->IsEqual(pkData->m_spAdditionalGeomData)))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiGeometryData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiGeometryData::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_usVertices", m_usVertices));
    pkStrings->Add(NiGetViewerString("m_usActiveVertices", 
        m_usActiveVertices));
    pkStrings->Add(NiGetViewerString("m_pkVertex", m_pkVertex));
    pkStrings->Add(NiGetViewerString("m_pkNormal", m_pkNormal));
    pkStrings->Add(NiGetViewerString("NormalBinormalTangent", 
        GetNormalBinormalTangentMethod()));
    pkStrings->Add(m_kBound.GetViewerString("m_kBound"));
    pkStrings->Add(NiGetViewerString("m_pkColor", m_pkColor));
    pkStrings->Add(NiGetViewerString("Num Texture Sets", GetTextureSets()));
    pkStrings->Add(NiGetViewerString("m_pkTexture", m_pkTexture));
    
    pkStrings->Add(NiGetViewerString("m_usDirtyFlags", m_usDirtyFlags));
    pkStrings->Add(NiGetViewerString("m_ucKeepFlags", m_ucKeepFlags));
    pkStrings->Add(NiGetViewerString("m_ucCompressFlags", m_ucCompressFlags));

    if (m_spAdditionalGeomData)
        m_spAdditionalGeomData->GetViewerStrings(pkStrings);
    else
        pkStrings->Add(NiGetViewerString("m_spAdditionalGeomData", NULL));
}
//---------------------------------------------------------------------------
unsigned int NiGeometryData::GetBlockAllocationSize() const
{
    unsigned int uiSize = m_pkVertex ? (m_usVertices*sizeof(NiPoint3)) : 0;

    if (m_pkNormal)
    {
        uiSize += m_usVertices * 
            ((GetNormalBinormalTangentMethod() != 0) ? 3 : 1) * 
            sizeof(NiPoint3);
    }

    if (m_pkColor)
        uiSize += m_usVertices * sizeof(NiColorA);

    if (m_pkTexture)
        uiSize += m_usVertices * GetTextureSets() * sizeof(NiPoint2);


    return NiObject::GetBlockAllocationSize() + uiSize;
}
//---------------------------------------------------------------------------
NiObjectGroup* NiGeometryData::GetGroup() const
{
    return m_pkGroup;
}
//---------------------------------------------------------------------------
void NiGeometryData::SetGroup(NiObjectGroup* pkGroup)
{
    m_pkGroup = pkGroup;
}
//---------------------------------------------------------------------------
