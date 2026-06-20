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

#include "NiD3D10VertexDescription.h"
#include "NiD3D10PixelFormat.h"

#include "NiD3D10Error.h"

DXGI_FORMAT NiD3D10VertexDescription::ms_aeFormats[SPTYPE_COUNT];
unsigned int NiD3D10VertexDescription::ms_auiTypeSizes[SPTYPE_COUNT];

//---------------------------------------------------------------------------
void NiD3D10VertexDescription::_SDMInit()
{
    ms_aeFormats[SPTYPE_FLOAT1] = DXGI_FORMAT_R32_FLOAT;
    ms_aeFormats[SPTYPE_FLOAT2] = DXGI_FORMAT_R32G32_FLOAT;
    ms_aeFormats[SPTYPE_FLOAT3] = DXGI_FORMAT_R32G32B32_FLOAT;
    ms_aeFormats[SPTYPE_FLOAT4] = DXGI_FORMAT_R32G32B32A32_FLOAT;
    ms_aeFormats[SPTYPE_UBYTECOLOR] = DXGI_FORMAT_R8G8B8A8_UNORM;
    ms_aeFormats[SPTYPE_UBYTE4] = DXGI_FORMAT_R8G8B8A8_UINT;
    ms_aeFormats[SPTYPE_SHORT2] = DXGI_FORMAT_R16G16_SINT;
    ms_aeFormats[SPTYPE_SHORT4] = DXGI_FORMAT_R16G16B16A16_SINT;
    ms_aeFormats[SPTYPE_NORMUBYTE4] = DXGI_FORMAT_R8G8B8A8_UNORM;
    ms_aeFormats[SPTYPE_NORMSHORT2] = DXGI_FORMAT_R16G16_SNORM;
    ms_aeFormats[SPTYPE_NORMSHORT4] = DXGI_FORMAT_R16G16B16A16_SNORM;
    ms_aeFormats[SPTYPE_NORMUSHORT2] = DXGI_FORMAT_R16G16_UNORM;
    ms_aeFormats[SPTYPE_NORMUSHORT4] = DXGI_FORMAT_R16G16B16A16_UNORM;
    ms_aeFormats[SPTYPE_UDEC3] = DXGI_FORMAT_R10G10B10A2_UINT;
    ms_aeFormats[SPTYPE_NORMDEC3] = DXGI_FORMAT_R10G10B10A2_UNORM;
    ms_aeFormats[SPTYPE_FLOAT16_2] = DXGI_FORMAT_R16G16_FLOAT; 
    ms_aeFormats[SPTYPE_FLOAT16_4] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    ms_aeFormats[SPTYPE_NONE] = DXGI_FORMAT_UNKNOWN;

    for (unsigned int i = 0 ; i < SPTYPE_COUNT; i++)
    {
        ms_auiTypeSizes[i] = 
            NiD3D10PixelFormat::GetBitsPerPixel(ms_aeFormats[i]) / 8;
    }
}
//---------------------------------------------------------------------------
void NiD3D10VertexDescription::_SDMShutdown()
{
}
//---------------------------------------------------------------------------
NiD3D10VertexDescription::NiD3D10VertexDescription() :
    m_pkInputElementArray(NULL),
    m_uiInputElementCount(0)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10VertexDescription::~NiD3D10VertexDescription()
{
    NiFree(m_pkInputElementArray);
}
//---------------------------------------------------------------------------
bool NiD3D10VertexDescription::Create(unsigned int uiMaxStreamEntryCount, 
    unsigned int uiStreamCount,
    NiD3D10VertexDescriptionPtr& spVertexDescription)
{
    if (uiStreamCount > 15)
        return NULL;

    spVertexDescription = NiNew NiD3D10VertexDescription;

    if (spVertexDescription)
    {
        if (spVertexDescription->Initialize(uiMaxStreamEntryCount, 
            uiStreamCount) == false)
        {
            spVertexDescription = 0;
        }
    }

    return (spVertexDescription != NULL);
}
//---------------------------------------------------------------------------
NiD3D10VertexDescription* NiD3D10VertexDescription::Create(
    unsigned int uiMaxStreamEntryCount, unsigned int uiStreamCount)
{
    NiD3D10VertexDescription* pkDesc = NiNew NiD3D10VertexDescription;
    if (pkDesc->Initialize(uiMaxStreamEntryCount, uiStreamCount) == false)
    {
        NiDelete pkDesc;
        return NULL;
    }

    return pkDesc;
}
//---------------------------------------------------------------------------
bool NiD3D10VertexDescription::Initialize(unsigned int uiMaxStreamEntryCount,
    unsigned int uiStreamCount)
{
    m_uiMaxStreamEntryCount = uiMaxStreamEntryCount;
    m_uiStreamCount = uiStreamCount;

    NIASSERT(m_uiMaxStreamEntryCount > 0);
    NIASSERT(m_uiStreamCount > 0);

    m_pkStreamEntries = NiNew ShaderRegisterStream[m_uiStreamCount];
    NIASSERT(m_pkStreamEntries);

    for (unsigned int ui = 0; ui < m_uiStreamCount; ui++)
    {
        m_pkStreamEntries[ui].m_pkEntries = 
            NiNew ShaderRegisterEntry[m_uiMaxStreamEntryCount];
        m_pkStreamEntries[ui].m_uiEntryCount = m_uiMaxStreamEntryCount; 
        NIASSERT(m_pkStreamEntries[ui].m_pkEntries);
    }

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10VertexDescription::GetVertexStride(unsigned int uiStream)
{
    if (uiStream >= m_uiStreamCount)
        return 0;

    // Force an update, if one is required
    UpdateInputElementArray();

    return m_pkStreamEntries[uiStream].m_uiStride;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10VertexDescription::GetPackingOffset(unsigned int uiStream,
    unsigned int uiEntry)
{
    if (uiStream >= m_uiStreamCount || 
        uiEntry >= m_pkStreamEntries[uiStream].m_uiEntryCount)
    {
        return UINT_MAX;
    }

    // Force an update, if one is required
    UpdateInputElementArray();

    return m_pkStreamEntries[uiStream].m_pkEntries[uiEntry].m_uiPackingOffset;
}
//---------------------------------------------------------------------------
void NiD3D10VertexDescription::UpdateInputElementArray()
{
    // Quick-out if no need to update.
    if (!m_bModified)
        return;

    // Determine how many total vertex entries are present.
    unsigned int uiElementCount = 0;
    unsigned int i = 0;
    for (; i < m_uiStreamCount; i++)
    {
        ShaderRegisterStream* pkStream = &(m_pkStreamEntries[i]);
        if (pkStream)
        {
            pkStream->m_bValid = false;
            for (unsigned int j = 0; j < m_uiMaxStreamEntryCount; j++)
            {
                ShaderRegisterEntry* pkEntry = pkStream->m_pkEntries + j;

                if (pkEntry->m_eInput != SHADERPARAM_INVALID &&
                    pkEntry->m_eType < SPTYPE_NONE)
                {
                    uiElementCount++;

                    pkStream->m_bValid = true;
                }
            }
        }
    }

    // Clear existing array count, if necessary.
    if (m_uiInputElementCount != uiElementCount)
    {
        NiFree(m_pkInputElementArray);
        m_pkInputElementArray = NULL;
    }

    m_uiInputElementCount = uiElementCount;

    // Quick-out
    if (m_uiInputElementCount == 0)
        return;

    // Allocate new array, if necessary
    if (m_pkInputElementArray == NULL)
    {
        m_pkInputElementArray = NiAlloc(D3D10_INPUT_ELEMENT_DESC, 
            m_uiInputElementCount);
    }

    // Fill in array
    uiElementCount = 0;
    for (i = 0; i < m_uiStreamCount; i++)
    {
        ShaderRegisterStream* pkStream = &(m_pkStreamEntries[i]);
        if (pkStream && pkStream->m_bValid)
        {
            unsigned int uiOffset = 0;

            for (unsigned int j = 0; j < m_uiMaxStreamEntryCount; j++)
            {
                ShaderRegisterEntry* pkEntry = pkStream->m_pkEntries + j;

                if ((pkEntry->m_eInput != SHADERPARAM_INVALID) &&
                    (pkEntry->m_eType < SPTYPE_NONE))
                {
                    pkEntry->m_uiPackingOffset = uiOffset;

                    unsigned int uiEntrySize = AddDeclarationEntry(pkEntry, i,
                        uiElementCount);
                    uiOffset += uiEntrySize;
                    uiElementCount++;
                }
            }
            pkStream->m_uiStride = uiOffset;
        }
    }

    m_bModified = false;
}
//---------------------------------------------------------------------------
D3D10_INPUT_ELEMENT_DESC* NiD3D10VertexDescription::GetInputElementArray() 
    const
{
    return m_pkInputElementArray;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10VertexDescription::GetInputElementCount() const
{
    return m_uiInputElementCount;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10VertexDescription::GetSPTypeSize(
    NiShaderDeclaration::ShaderParameterType eType)
{
    if (eType < SPTYPE_COUNT)
        return ms_auiTypeSizes[eType];
    else
        return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10VertexDescription::AddDeclarationEntry(
    ShaderRegisterEntry* pkEntry, unsigned int uiStream, 
    unsigned int uiCurrentElement)
{
    //  Make sure we aren't going to exceed our bounds!
    NIASSERT(uiCurrentElement < m_uiInputElementCount);
    NIASSERT(pkEntry->m_eType < SPTYPE_COUNT);

    m_pkInputElementArray[uiCurrentElement].SemanticName = 
        pkEntry->m_kUsage;
    m_pkInputElementArray[uiCurrentElement].SemanticIndex = 
        pkEntry->m_uiUsageIndex;
    m_pkInputElementArray[uiCurrentElement].Format = 
        ms_aeFormats[pkEntry->m_eType];
    m_pkInputElementArray[uiCurrentElement].InputSlot = uiStream;
    m_pkInputElementArray[uiCurrentElement].AlignedByteOffset = 
        pkEntry->m_uiPackingOffset;
    m_pkInputElementArray[uiCurrentElement].InputSlotClass =
        D3D10_INPUT_PER_VERTEX_DATA;
    m_pkInputElementArray[uiCurrentElement].InstanceDataStepRate = 0;

    return ms_auiTypeSizes[pkEntry->m_eType];
}
//---------------------------------------------------------------------------
