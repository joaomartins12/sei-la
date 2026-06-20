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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10GeometryData.h"
#include "NiD3D10Error.h"
#include "NiD3D10Renderer.h"

//---------------------------------------------------------------------------
NiD3D10GeometryData::PrimitiveDesc::PrimitiveDesc() :
    m_pkIndexBuffer(NULL),
    m_eIBFormat(DXGI_FORMAT_UNKNOWN),
    m_uiIBOffset(0),
    m_puiVBOffsetArray(NULL),
    m_uiIndexCount(0),
    m_uiStartIndexLocation(0),
    m_iBaseVertexLocation(0)
{
}
//---------------------------------------------------------------------------
NiD3D10GeometryData::PrimitiveDesc::~PrimitiveDesc()
{
    if (m_pkIndexBuffer)
        m_pkIndexBuffer->Release();

    NiFree(m_puiVBOffsetArray);
}
//---------------------------------------------------------------------------
NiD3D10GeometryData::NiD3D10GeometryData(unsigned int uiPrimitiveCount) :
    m_uiPartitionCount(1),
    m_puiPartitionStartPrimitive(NULL),
    m_ePrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED),
    m_uiVBCount(0),
    m_pspVBDataStreamArray(NULL),
    m_ppkVBArray(NULL),
    m_puiVBStrideArray(NULL),
    m_pkElementDescArray(NULL),
    m_uiElementCount(0),
    m_pkCurrentInputLayout(NULL),
    m_kInputLayouts(7),
    m_pkPrimitives(NULL),
    m_pkSkinInstance(NULL),
    m_pkNext(NULL),
    m_uiFlags(0)
{
    m_puiPartitionStartPrimitive = NiAlloc(unsigned int, 2);
    m_puiPartitionStartPrimitive[0] = 0;
    m_puiPartitionStartPrimitive[1] = uiPrimitiveCount;

    NIASSERT (uiPrimitiveCount > 0);
    m_pkPrimitives = NiNew PrimitiveDesc[uiPrimitiveCount];
}
//---------------------------------------------------------------------------
NiD3D10GeometryData::NiD3D10GeometryData(NiSkinInstance* pkSkinInstance,
    unsigned int uiPartitions, unsigned int* puiPrimitiveCount) :
    m_uiPartitionCount(uiPartitions),
    m_puiPartitionStartPrimitive(NULL),
    m_ePrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED),
    m_uiVBCount(0),
    m_pspVBDataStreamArray(NULL),
    m_ppkVBArray(NULL),
    m_puiVBStrideArray(NULL),
    m_pkElementDescArray(NULL),
    m_uiElementCount(0),
    m_pkCurrentInputLayout(NULL),
    m_kInputLayouts(7),
    m_pkPrimitives(NULL),
    m_pkSkinInstance(pkSkinInstance),
    m_pkNext(NULL),
    m_uiFlags(0)
{
    NIASSERT(uiPartitions && puiPrimitiveCount);

    m_puiPartitionStartPrimitive = NiAlloc(unsigned int, uiPartitions + 1);
    m_puiPartitionStartPrimitive[0] = 0;
    for (unsigned int i = 0; i < uiPartitions; i++)
    {
        m_puiPartitionStartPrimitive[i + 1] = 
            m_puiPartitionStartPrimitive[i] + puiPrimitiveCount[i];
    }

    NIASSERT (m_puiPartitionStartPrimitive[m_uiPartitionCount] > 0);
    m_pkPrimitives = 
        NiNew PrimitiveDesc[m_puiPartitionStartPrimitive[m_uiPartitionCount]];
}
//---------------------------------------------------------------------------
NiD3D10GeometryData::~NiD3D10GeometryData()
{
    ReleaseCachedInputLayouts();

    ReleaseElementArray();

    for (unsigned int i = 0; i < m_uiVBCount; i++)
    {
        if (m_ppkVBArray[i])
            m_ppkVBArray[i]->Release();
        m_pspVBDataStreamArray[i] = NULL;
    }
    NiFree(m_puiPartitionStartPrimitive);
    NiDelete[] m_pspVBDataStreamArray;
    NiFree(m_ppkVBArray);
    NiFree(m_puiVBStrideArray);
    NiDelete[] m_pkPrimitives;

    m_puiPartitionStartPrimitive = NULL;
    m_pspVBDataStreamArray = NULL;
    m_ppkVBArray = NULL;
    m_puiVBStrideArray = NULL;
    m_pkPrimitives = NULL;

    NiDelete m_pkNext;
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::UpdateInputLayout(void* pvInputSignature,
    unsigned int uiInputSignatureSize)
{
    ID3D10InputLayout* pkInputLayout = NULL;
    NIVERIFY(m_kInputLayouts.GetAt(pvInputSignature, pkInputLayout) == false
        || pkInputLayout != NULL);
    
    if (pkInputLayout == NULL)
    {
        NIASSERT(NiD3D10Renderer::GetRenderer() != NULL);
        ID3D10Device* pkDevice = 
            NiD3D10Renderer::GetRenderer()->GetD3D10Device();
        NIASSERT(pkDevice != NULL);

        HRESULT hr = pkDevice->CreateInputLayout(m_pkElementDescArray, 
            m_uiElementCount, pvInputSignature, uiInputSignatureSize, 
            &pkInputLayout);

        if (FAILED(hr) || pkInputLayout == NULL)
        {
            if (FAILED(hr))
            {
                NiD3D10Error::ReportError(
                    NiD3D10Error::NID3D10ERROR_INPUT_LAYOUT_CREATION_FAILED,
                    "Error HRESULT = 0x%08X.", (unsigned int)hr);
            }
            else
            {
                NiD3D10Error::ReportError(
                    NiD3D10Error::NID3D10ERROR_INPUT_LAYOUT_CREATION_FAILED,
                    "No error message from D3D10, but input layout is NULL.");
            }

            if (pkInputLayout)
            {
                pkInputLayout->Release();
                pkInputLayout = NULL;
            }
            return;
        }

        m_kInputLayouts.SetAt(pvInputSignature, pkInputLayout);
    }

    m_pkCurrentInputLayout = pkInputLayout;
}
//---------------------------------------------------------------------------
bool NiD3D10GeometryData::ContainsVertexData(
    NiShaderDeclaration::ShaderParameter eParameter) const
{
    switch (eParameter)
    {
    case NiShaderDeclaration::SHADERPARAM_NI_COLOR:
        return GetHasColorsFromFlags(m_uiFlags);
        break;
    case NiShaderDeclaration::SHADERPARAM_NI_NORMAL:
        return GetHasNormalsFromFlags(m_uiFlags);
        break;
    case NiShaderDeclaration::SHADERPARAM_NI_BINORMAL:
    case NiShaderDeclaration::SHADERPARAM_NI_TANGENT:
        return GetHasBinormalsTangentsFromFlags(m_uiFlags);
        break;
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0:
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD1:
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD2:
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD3:
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4:
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5:
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6:
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7:
        {
            unsigned int uiNumUVs = GetNumUVsFromFlags(m_uiFlags);
            unsigned int uiUVSet = (unsigned int)(eParameter - 
                NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0);

            return uiUVSet < uiNumUVs;
        }
        break;
    }

    return false;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryData::GetPartitionCount() const
{
    return m_uiPartitionCount;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryData::GetPrimitiveStart(unsigned int uiPartition)
    const
{
    if (uiPartition < m_uiPartitionCount)
        return m_puiPartitionStartPrimitive[uiPartition];
    else
        return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryData::GetPrimitiveCount(unsigned int uiPartition)
    const
{
    if (uiPartition < m_uiPartitionCount)
    {
        return m_puiPartitionStartPrimitive[uiPartition + 1] - 
            m_puiPartitionStartPrimitive[uiPartition];
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryData::GetTotalPrimitiveCount() const
{
    return m_puiPartitionStartPrimitive[m_uiPartitionCount];
}
//---------------------------------------------------------------------------
D3D10_INPUT_ELEMENT_DESC* NiD3D10GeometryData::GetInputElementDescArray() const
{
    return m_pkElementDescArray;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryData::GetInputElementDescCount() const
{
    return m_uiElementCount;
}
//---------------------------------------------------------------------------
D3D10_PRIMITIVE_TOPOLOGY NiD3D10GeometryData::GetPrimitiveTopology() const
{
    return m_ePrimitiveTopology;
}
//---------------------------------------------------------------------------
ID3D10InputLayout* NiD3D10GeometryData::GetInputLayout() const
{
    return m_pkCurrentInputLayout;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryData::GetVBCount() const
{
    return m_uiVBCount;
}
//---------------------------------------------------------------------------
NiD3D10DataStream* NiD3D10GeometryData::GetVBDataStream(unsigned int uiIndex)
    const
{
    if (uiIndex < m_uiVBCount)
        return m_pspVBDataStreamArray[uiIndex];

    NiD3D10Error::ReportWarning("Attempting to access invalid VB in "
        "NiD3D10GeometryBuffer!");
    return NULL;
}
//---------------------------------------------------------------------------
ID3D10Buffer*const* NiD3D10GeometryData::GetVBArray() const
{
    return m_ppkVBArray;
}
//---------------------------------------------------------------------------
const unsigned int* NiD3D10GeometryData::GetVBStrideArray() const
{
    return m_puiVBStrideArray;
}
//---------------------------------------------------------------------------
ID3D10Buffer*const NiD3D10GeometryData::GetIB(unsigned int uiPrimitive) const
{
    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
        return m_pkPrimitives[uiPrimitive].m_pkIndexBuffer;

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
    return NULL;
}
//---------------------------------------------------------------------------
NiD3D10DataStream* NiD3D10GeometryData::GetIBDataStream(
    unsigned int uiPrimitive) const
{
    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
        return m_pkPrimitives[uiPrimitive].m_spIBDataStream;

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
    return NULL;
}
//---------------------------------------------------------------------------
DXGI_FORMAT NiD3D10GeometryData::GetIBFormat(unsigned int uiPrimitive) const
{
    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
        return m_pkPrimitives[uiPrimitive].m_eIBFormat;

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
    return DXGI_FORMAT_UNKNOWN;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryData::GetIBOffset(unsigned int uiPrimitive) const
{
    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
        return m_pkPrimitives[uiPrimitive].m_uiIBOffset;

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
    return 0;
}
//---------------------------------------------------------------------------
const unsigned int* NiD3D10GeometryData::GetVBOffsetArray(
    unsigned int uiPrimitive) const
{
    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
        return m_pkPrimitives[uiPrimitive].m_puiVBOffsetArray;

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
    return NULL;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryData::GetIndexCount(unsigned int uiPrimitive)
    const
{
    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
        return m_pkPrimitives[uiPrimitive].m_uiIndexCount;

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryData::GetStartIndexLocation(
    unsigned int uiPrimitive) const
{
    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
        return m_pkPrimitives[uiPrimitive].m_uiStartIndexLocation;

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
    return 0;
}
//---------------------------------------------------------------------------
int NiD3D10GeometryData::GetBaseVertexLocation(unsigned int uiPrimitive) const
{
    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
        return m_pkPrimitives[uiPrimitive].m_iBaseVertexLocation;

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryData::GetFlags() const
{
    return m_uiFlags;
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetPrimitiveTopology(
    D3D10_PRIMITIVE_TOPOLOGY eTopology)
{
    m_ePrimitiveTopology = eTopology;
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetInputElementDescArray(
    D3D10_INPUT_ELEMENT_DESC* pkElementDescArray, unsigned int uiElementCount)
{
    if (uiElementCount != m_uiElementCount)
    {
        ReleaseElementArray();
    }

    if (pkElementDescArray && uiElementCount)
    {
        if (m_pkElementDescArray == NULL)
        {
            m_pkElementDescArray = NiAlloc(D3D10_INPUT_ELEMENT_DESC, 
                uiElementCount);
        }
        for (unsigned int i = 0; i < uiElementCount; i++)
        {
            m_pkElementDescArray[i] = pkElementDescArray[i];
        }

        m_uiElementCount = uiElementCount;
    }
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetInputLayout(ID3D10InputLayout* pkInputLayout,
    void* pvInputSignature)
{
    ID3D10InputLayout* pkCachedInputLayout = NULL;
    NIVERIFY(m_kInputLayouts.GetAt(pvInputSignature, pkCachedInputLayout) == 
        false || pkCachedInputLayout != NULL);

    if (pkInputLayout != pkCachedInputLayout)
    {
        if (pkCachedInputLayout)
        {
            // Replace original with this one.
            pkCachedInputLayout->Release();
        }

        pkInputLayout->AddRef();
        m_kInputLayouts.SetAt(pvInputSignature, pkInputLayout);
    }

    m_pkCurrentInputLayout = pkInputLayout;
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetVBCount(unsigned int uiVBCount)
{
    // This function always clears out and releases all existing VBs on the 
    // PrimitiveDesc.

    unsigned int i = 0;
    for (; i < m_uiVBCount; i++)
    {
        if (m_ppkVBArray[i])
            m_ppkVBArray[i]->Release();
        m_pspVBDataStreamArray[i] = NULL;
    }

    NiFree(m_ppkVBArray);
    NiFree(m_puiVBStrideArray);
    NiDelete[] m_pspVBDataStreamArray;

    for (i = 0; i < m_puiPartitionStartPrimitive[m_uiPartitionCount]; i++)
    {
        PrimitiveDesc* pkDesc = &m_pkPrimitives[i];

        NiFree(pkDesc->m_puiVBOffsetArray);
    }

    m_uiVBCount = uiVBCount;

    if (uiVBCount)
    {
        m_ppkVBArray = NiAlloc(ID3D10Buffer*, uiVBCount);
        memset(m_ppkVBArray, 0, uiVBCount * sizeof(*m_ppkVBArray));

        m_puiVBStrideArray = NiAlloc(unsigned int, uiVBCount);
        memset(m_puiVBStrideArray, 0, uiVBCount * sizeof(*m_puiVBStrideArray));

        m_pspVBDataStreamArray = NiNew NiD3D10DataStreamPtr[uiVBCount];

        for (i = 0; i < m_puiPartitionStartPrimitive[m_uiPartitionCount]; i++)
        {
            PrimitiveDesc* pkDesc = &m_pkPrimitives[i];
    
            pkDesc->m_puiVBOffsetArray = NiAlloc(unsigned int, uiVBCount);

            memset(pkDesc->m_puiVBOffsetArray, 0, 
                uiVBCount * sizeof(*(pkDesc->m_puiVBOffsetArray)));
        }
    }

    return;
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetVBDataStream(unsigned int uiIndex, 
    NiD3D10DataStream* pkVBDataStream)
{
    if (uiIndex >= m_uiVBCount)
    {
        NiD3D10Error::ReportWarning("Attempting to set vertex buffer index %d "
            "in a NiD3D10GeometryData object that expects %d "
            "vertex buffers.", uiIndex, m_uiVBCount);
        return;
    }

    if (pkVBDataStream)
    {
        if (pkVBDataStream->GetBuffer() == NULL)
        {
            NiD3D10Error::ReportWarning("Attempting to set an "
                "NiD3D10DataStream with no buffer as a vertex buffer on an "
                "NiD3D10GeometryData object.");
            return;
        }
        if ((pkVBDataStream->GetUsageFlags() & 
            NiD3D10DataStream::USAGE_VERTEX) == 0)
        {
            NiD3D10Error::ReportWarning("Attempting to set a non-vertex "
                "buffer NiD3D10DataStream object as a vertex buffer on an "
                "NiD3D10GeometryData object.");
            return;
        }
    }

    if (m_pspVBDataStreamArray[uiIndex] == pkVBDataStream)
    {
        // Setting same VB data stream
        NIASSERT((pkVBDataStream == NULL && m_ppkVBArray[uiIndex] == NULL) ||
            (m_ppkVBArray[uiIndex] == pkVBDataStream->GetBuffer()));
        return;
    }

    NIASSERT(m_ppkVBArray[uiIndex] != pkVBDataStream->GetBuffer());

    m_pspVBDataStreamArray[uiIndex] = pkVBDataStream;

    if (m_ppkVBArray[uiIndex])
        m_ppkVBArray[uiIndex]->Release();

    if (pkVBDataStream)
    {
        m_ppkVBArray[uiIndex] = pkVBDataStream->GetBuffer();
        NIASSERT(pkVBDataStream->GetBuffer());
        pkVBDataStream->GetBuffer()->AddRef();
    }
    else
    {
        m_ppkVBArray[uiIndex] = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetVB(unsigned int uiIndex, ID3D10Buffer*const pkVB)
{
    if (uiIndex >= m_uiVBCount)
    {
        NiD3D10Error::ReportWarning("Attempting to set vertex buffer index %d "
            "in a NiD3D10GeometryData object that expects %d "
            "vertex buffers.", uiIndex, m_uiVBCount);
        return;
    }

    if (m_ppkVBArray[uiIndex] == pkVB)
    {
        // Setting same VB
        NIASSERT((m_pspVBDataStreamArray[uiIndex] == NULL && pkVB == NULL) ||
            (m_pspVBDataStreamArray[uiIndex]->GetBuffer() == pkVB));
        return;
    }

    if (pkVB)
    {
        NiD3D10DataStreamPtr spNewVB;
        NIVERIFY(NiD3D10DataStream::Create(pkVB, spNewVB));
        NIASSERT(spNewVB);

        if ((spNewVB->GetUsageFlags() & NiD3D10DataStream::USAGE_VERTEX) != 0)
        {
            NiD3D10Error::ReportWarning("Attempting to set a buffer that is "
                "not a vertex buffer as a vertex buffer on an "
                "NiD3D10GeometryData object.");
            return;
        }

        m_pspVBDataStreamArray[uiIndex] = spNewVB;
    }
    else
    {
        m_pspVBDataStreamArray[uiIndex] = NULL;
    }

    if (m_ppkVBArray[uiIndex])
        m_ppkVBArray[uiIndex]->Release();
    m_ppkVBArray[uiIndex] = pkVB;
    if (m_ppkVBArray[uiIndex])
        m_ppkVBArray[uiIndex]->AddRef();
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetVBArray(ID3D10Buffer*const* ppkVBArray, 
    unsigned int uiVBCount)
{
    if (uiVBCount != m_uiVBCount)
    {
        NiD3D10Error::ReportWarning("Attempting to set %d vertex buffers "
            "in a NiD3D10GeometryData object that expects %d "
            "vertex buffers.", uiVBCount, m_uiVBCount);
        return;
    }

    for (unsigned int i = 0; i < uiVBCount; i++)
    {
        if (ppkVBArray)
            SetVB(i, ppkVBArray[i]);
        else
            SetVB(i, NULL);
    }

    return;
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetVBStrideArray(
    const unsigned int* puiVBStrideArray, unsigned int uiVBCount)
{
    if (uiVBCount != m_uiVBCount)
    {
        NiD3D10Error::ReportWarning("Attempting to set %d vertex buffer "
            "strides in a NiD3D10GeometryData object that expects %d "
            "vertex buffers.", uiVBCount, m_uiVBCount);
        return;
    }

    for (unsigned int i = 0; i < uiVBCount; i++)
    {
        m_puiVBStrideArray[i] = 
            (puiVBStrideArray == NULL ? 0 : puiVBStrideArray[i]);
    }
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetIBDataStream(unsigned int uiPrimitive, 
    NiD3D10DataStream* pkIBDataStream)
{
    if (uiPrimitive >= m_puiPartitionStartPrimitive[m_uiPartitionCount])
    {
        NiD3D10Error::ReportWarning("Attempting to access invalid primitive "
            "in NiD3D10GeometryBuffer!");
        return;
    }

    PrimitiveDesc* pkDesc = &m_pkPrimitives[uiPrimitive];

    if (pkIBDataStream)
    {
        if (pkIBDataStream->GetBuffer() == NULL)
        {
            NiD3D10Error::ReportWarning("Attempting to set an "
                "NiD3D10DataStream with no buffer as a index buffer on an "
                "NiD3D10GeometryData object.");
            return;
        }
        if ((pkIBDataStream->GetUsageFlags() & 
            NiD3D10DataStream::USAGE_INDEX) == 0)
        {
            NiD3D10Error::ReportWarning("Attempting to set a non-index "
                "buffer NiD3D10DataStream object as a index buffer on an "
                "NiD3D10GeometryData object.");
            return;
        }
    }

    if (pkDesc->m_spIBDataStream == pkIBDataStream)
    {
        // Setting same VB data stream
        NIASSERT((pkIBDataStream == NULL && pkDesc->m_pkIndexBuffer == NULL) ||
            (pkDesc->m_pkIndexBuffer == pkIBDataStream->GetBuffer()));
        return;
    }

    NIASSERT(pkDesc->m_pkIndexBuffer != pkIBDataStream->GetBuffer());

    pkDesc->m_spIBDataStream = pkIBDataStream;

    if (pkDesc->m_pkIndexBuffer)
        pkDesc->m_pkIndexBuffer->Release();

    if (pkIBDataStream)
    {
        pkDesc->m_pkIndexBuffer = pkIBDataStream->GetBuffer();
        NIASSERT(pkIBDataStream->GetBuffer());
        pkIBDataStream->GetBuffer()->AddRef();
    }
    else
    {
        pkDesc->m_pkIndexBuffer = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetIB(unsigned int uiPrimitive, 
    ID3D10Buffer* pkIndexBuffer)
{
    if (uiPrimitive >= m_puiPartitionStartPrimitive[m_uiPartitionCount])
    {
        NiD3D10Error::ReportWarning("Attempting to access invalid primitive "
            "in NiD3D10GeometryBuffer!");
        return;
    }

    PrimitiveDesc* pkDesc = &m_pkPrimitives[uiPrimitive];

    if (pkDesc->m_pkIndexBuffer == pkIndexBuffer)
    {
        // Setting same VB
        NIASSERT((pkDesc->m_spIBDataStream == NULL && pkIndexBuffer == NULL) ||
            (pkDesc->m_spIBDataStream->GetBuffer() == pkIndexBuffer));
        return;
    }

    if (pkIndexBuffer)
    {
        NiD3D10DataStreamPtr spNewIB;
        NIVERIFY(NiD3D10DataStream::Create(pkIndexBuffer, spNewIB));
        NIASSERT(spNewIB);

        if ((spNewIB->GetUsageFlags() & NiD3D10DataStream::USAGE_INDEX) != 0)
        {
            NiD3D10Error::ReportWarning("Attempting to set a buffer that is "
                "not an index buffer as an index buffer on an "
                "NiD3D10GeometryData object.");
            return;
        }

        pkDesc->m_spIBDataStream = spNewIB;
    }
    else
    {
        pkDesc->m_spIBDataStream = NULL;
    }

    if (pkDesc->m_pkIndexBuffer)
        pkDesc->m_pkIndexBuffer->Release();
    pkDesc->m_pkIndexBuffer = pkIndexBuffer;
    if (pkDesc->m_pkIndexBuffer)
        pkDesc->m_pkIndexBuffer->AddRef();
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetIBFormat(unsigned int uiPrimitive, 
    DXGI_FORMAT eIBFormat)
{
    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
    {
        m_pkPrimitives[uiPrimitive].m_eIBFormat = eIBFormat;
        return;
    }

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetIBOffset(unsigned int uiPrimitive, 
    unsigned int uiIBOffset)
{
    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
    {
        m_pkPrimitives[uiPrimitive].m_uiIBOffset = uiIBOffset;
        return;
    }

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetVBOffsetArray(unsigned int uiPrimitive, 
    const unsigned int* puiVBOffsetArray, unsigned int uiVBCount)
{
    if (uiVBCount != m_uiVBCount)
    {
        NiD3D10Error::ReportWarning("Attempting to set %d VB offsets "
            "in a NiD3D10GeometryData object that expects %d "
            "VB offsets", uiVBCount, m_uiVBCount);
        return;
    }

    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
    {
        PrimitiveDesc* pkDesc = &m_pkPrimitives[uiPrimitive];
        for (unsigned int i = 0; i < uiVBCount; i++)
            pkDesc->m_puiVBOffsetArray[i] = puiVBOffsetArray[i];
        return;
    }

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetIndexCount(unsigned int uiPrimitive, 
    unsigned int uiIndexCount)
{
    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
    {
        m_pkPrimitives[uiPrimitive].m_uiIndexCount = uiIndexCount;
        return;
    }

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetStartIndexLocation(unsigned int uiPrimitive, 
    unsigned int uiStartIndexLocation)
{
    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
    {
        m_pkPrimitives[uiPrimitive].m_uiStartIndexLocation = 
            uiStartIndexLocation;
        return;
    }

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetBaseVertexLocation(unsigned int uiPrimitive, 
    int iBaseVertexLocation)
{
    if (uiPrimitive < m_puiPartitionStartPrimitive[m_uiPartitionCount])
    {
        m_pkPrimitives[uiPrimitive].m_iBaseVertexLocation = 
            iBaseVertexLocation;
        return;
    }

    NiD3D10Error::ReportWarning("Attempting to access invalid primitive in "
        "NiD3D10GeometryBuffer!");
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetFlags(unsigned int uiFlags)
{
    m_uiFlags = uiFlags;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryData::CreateVertexFlags(bool bHasColors, 
    bool bHasNormals, bool bHasBinormalsTangents, unsigned int uiUVCount)
{
    unsigned int uiFlags = 0;

    if (bHasColors)
        uiFlags |= VERTEX_FLAG_HASCOLORS;

    if (bHasNormals)
        uiFlags |= VERTEX_FLAG_HASNORMALS;

    if (bHasBinormalsTangents)
        uiFlags |= VERTEX_FLAG_HASBINORMALSTANGENTS;

    uiFlags |= uiUVCount << VERTEX_FLAG_TEXTURESHIFT;

    return uiFlags;
}
//---------------------------------------------------------------------------
bool NiD3D10GeometryData::GetHasColorsFromFlags(unsigned int uiFlags)
{
    if (uiFlags & VERTEX_FLAG_HASCOLORS)
        return true;
    return false;
}
//---------------------------------------------------------------------------
bool NiD3D10GeometryData::GetHasNormalsFromFlags(unsigned int uiFlags)
{
    if (uiFlags & VERTEX_FLAG_HASNORMALS)
        return true;
    return false;
}
//---------------------------------------------------------------------------

bool NiD3D10GeometryData::GetHasBinormalsTangentsFromFlags(
    unsigned int uiFlags)
{
    if (uiFlags & VERTEX_FLAG_HASBINORMALSTANGENTS)
        return true;
    return false;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryData::GetNumUVsFromFlags(unsigned int uiFlags)
{
    return (uiFlags & VERTEX_FLAG_TEXTUREMASK) >> VERTEX_FLAG_TEXTURESHIFT;
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::UpdateVBs()
{
    for (unsigned int i = 0; i < m_uiVBCount; i++)
    {
        if (m_pspVBDataStreamArray[i])
        {
            ID3D10Buffer* pkBuffer = m_pspVBDataStreamArray[i]->GetBuffer();
            if (pkBuffer != m_ppkVBArray[i])
            {
                if (m_ppkVBArray[i])
                    m_ppkVBArray[i]->Release();
                m_ppkVBArray[i] = pkBuffer;
                if (m_ppkVBArray[i])
                    m_ppkVBArray[i]->AddRef();
            }
        }
        else
        {
            NIASSERT(m_ppkVBArray[i] == NULL);
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::UpdateIBs()
{
    for (unsigned int i = 0; 
        i < m_puiPartitionStartPrimitive[m_uiPartitionCount]; i++)
    {
        PrimitiveDesc* pkDesc = m_pkPrimitives + i;
        if (pkDesc->m_spIBDataStream)
        {
            ID3D10Buffer* pkBuffer = pkDesc->m_spIBDataStream->GetBuffer();
            if (pkBuffer != pkDesc->m_pkIndexBuffer)
            {
                if (pkDesc->m_pkIndexBuffer)
                    pkDesc->m_pkIndexBuffer->Release();
                pkDesc->m_pkIndexBuffer = pkBuffer;
                if (pkDesc->m_pkIndexBuffer)
                    pkDesc->m_pkIndexBuffer->AddRef();
            }
        }
        else
        {
            NIASSERT(pkDesc->m_pkIndexBuffer == NULL);
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::ReleaseCachedInputLayouts()
{
    NiTMapIterator kIter = m_kInputLayouts.GetFirstPos();
    while (kIter)
    {
        void* pvInputSignature;
        ID3D10InputLayout* pkInputLayout;
        m_kInputLayouts.GetNext(kIter, pvInputSignature, pkInputLayout);
        if (pkInputLayout)
            pkInputLayout->Release();
    }

    m_kInputLayouts.RemoveAll();

    m_pkCurrentInputLayout = NULL;
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::ReleaseElementArray()
{
    NiFree(m_pkElementDescArray);
    m_pkElementDescArray = NULL;
    m_uiElementCount = 0;
}
//---------------------------------------------------------------------------
NiSkinInstance* NiD3D10GeometryData::GetSkinInstance() const
{
    return m_pkSkinInstance;
}
//---------------------------------------------------------------------------
NiD3D10GeometryData* NiD3D10GeometryData::GetNext() const
{
    return m_pkNext;
}
//---------------------------------------------------------------------------
void NiD3D10GeometryData::SetNext(NiD3D10GeometryData* pkNext)
{
    NiDelete m_pkNext;

    m_pkNext = pkNext;
}
//---------------------------------------------------------------------------
