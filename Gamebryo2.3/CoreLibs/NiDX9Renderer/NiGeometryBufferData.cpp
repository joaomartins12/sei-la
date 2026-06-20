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
#include "NiD3DRendererPCH.h"

#include <NiGeometryGroup.h>
#include "NiGeometryBufferData.h"
#include "NiVBChip.h"

//---------------------------------------------------------------------------
NiGeometryBufferData::~NiGeometryBufferData()
{
    if (m_pkGeometryGroup)
    {
        for (unsigned int i = 0; i < m_uiStreamCount; i++)
            m_pkGeometryGroup->ReleaseChip(this, i);
    }

    RemoveIB();

    NiFree(m_ppkVBChip);
    NiFree(m_puiVertexStride);

    if (m_hDeclaration)
    {
        NiD3DRenderer::ReleaseVertexDecl(m_hDeclaration);
    }
}
//---------------------------------------------------------------------------
bool NiGeometryBufferData::IsVBChipValid() const
{
    if (m_uiStreamCount == 0)
        return false;

    for (unsigned int i = 0; i < m_uiStreamCount; i++)
    {
        if (m_ppkVBChip[i] == NULL || 
            m_ppkVBChip[i]->GetVB() == NULL)
        {
            return false;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
void NiGeometryBufferData::RemoveIB()
{
    if (m_pkIB)
    {
        NiD3DRenderer::ReleaseIBResource(m_pkIB);
        m_pkIB = NULL;
    }
    m_uiIBSize = 0;
}
//---------------------------------------------------------------------------
void NiGeometryBufferData::SetStreamCount(unsigned int uiStreamCount)
{
    if (uiStreamCount == m_uiStreamCount)
        return;

    NIASSERT(uiStreamCount > 0);

    NiVBChip** ppkVBChip = NiAlloc(NiVBChip*, uiStreamCount);
    unsigned int* puiVertexStride = NiAlloc(unsigned int, uiStreamCount);
    unsigned int uiByteSize;
    
    if (uiStreamCount < m_uiStreamCount)
    {
        uiByteSize = uiStreamCount * sizeof(*ppkVBChip);
        NiMemcpy(ppkVBChip, m_ppkVBChip, uiByteSize);

        uiByteSize = uiStreamCount * sizeof(*puiVertexStride);
        NiMemcpy(puiVertexStride, m_puiVertexStride, uiByteSize);
        if (m_pkGeometryGroup)
        {
            for (unsigned int i = uiStreamCount; i < m_uiStreamCount; i++)
                m_pkGeometryGroup->ReleaseChip(this, i);
        }
    }
    else
    {
        uiByteSize = m_uiStreamCount * sizeof(*ppkVBChip);
        NiMemcpy(ppkVBChip, m_ppkVBChip, uiByteSize);

        uiByteSize = m_uiStreamCount * sizeof(*puiVertexStride);
        NiMemcpy(puiVertexStride, m_puiVertexStride, uiByteSize);

        memset(ppkVBChip + m_uiStreamCount, 0, 
            (uiStreamCount - m_uiStreamCount) * sizeof(*ppkVBChip)); 
        memset(puiVertexStride + m_uiStreamCount, 0, 
            (uiStreamCount - m_uiStreamCount) * sizeof(*puiVertexStride)); 
    }

    NiDelete[] m_ppkVBChip;
    NiFree(m_puiVertexStride);
    m_ppkVBChip = ppkVBChip;
    m_puiVertexStride = puiVertexStride;
    m_uiStreamCount = uiStreamCount;
}
//---------------------------------------------------------------------------
void NiGeometryBufferData::SetFVF(unsigned int uiFVF)
{
    m_uiFVF = uiFVF;
    if (m_hDeclaration)
    {
        NiD3DRenderer::ReleaseVertexDecl(m_hDeclaration);
        m_hDeclaration = NULL;
    }
}
//---------------------------------------------------------------------------
void NiGeometryBufferData::SetVertexDeclaration(
    NiD3DVertexDeclaration hDecl)
{
    if (hDecl != m_hDeclaration)
    {
        if (m_hDeclaration)
        {
            NiD3DRenderer::ReleaseVertexDecl(m_hDeclaration);
        }

        m_hDeclaration = hDecl;

        if (m_hDeclaration)
        {
            D3D_POINTER_REFERENCE(m_hDeclaration);
        }
    }
    m_uiFVF = 0;
}
//---------------------------------------------------------------------------
bool NiGeometryBufferData::ContainsVertexData(
    NiShaderDeclaration::ShaderParameter eParameter) const
{
    switch (eParameter)
    {
        case NiShaderDeclaration::SHADERPARAM_NI_COLOR:
            return NiD3DRenderer::GetHasColorsFromFlags(m_uiFlags);
            break;
        case NiShaderDeclaration::SHADERPARAM_NI_NORMAL:
            return NiD3DRenderer::GetHasNormalsFromFlags(m_uiFlags);
            break;
        case NiShaderDeclaration::SHADERPARAM_NI_BINORMAL:
        case NiShaderDeclaration::SHADERPARAM_NI_TANGENT:
            return NiD3DRenderer::GetHasBinormalsTangentsFromFlags(m_uiFlags);
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
                unsigned int uiNumUVs = 
                    NiD3DRenderer::GetNumUVsFromFlags(m_uiFlags);
                unsigned int uiUVSet = (unsigned int)(eParameter - 
                    NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0);

                return uiUVSet < uiNumUVs;
            }
            break;
    }

    return false;
}
//---------------------------------------------------------------------------
