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
#include "NiFXLShaderLibPCH.h"

#include "NiFXLParameterDesc.h"
#include <NiRTLib.h>

NiFXLParameterMatrixDesc NiFXLParameterDesc::ms_kMatrixDesc;
NiFXLParameterVectorDesc NiFXLParameterDesc::ms_kVectorDesc;
NiFXLParameterFloatDesc NiFXLParameterDesc::ms_kFloatDesc;
NiFXLParameterBooleanDesc NiFXLParameterDesc::ms_kBooleanDesc;
NiFXLParameterIntegerDesc NiFXLParameterDesc::ms_kIntegerDesc;
NiFXLParameterSamplerDesc NiFXLParameterDesc::ms_kSamplerDesc;
NiFXLParameterPredefinedDesc
    NiFXLParameterDesc::ms_kPredefinedDesc;
NiFXLParameterObjectDesc NiFXLParameterDesc::ms_kObjectDesc;
NiFXLParameterArrayDesc NiFXLParameterDesc::ms_kArrayDesc;

//---------------------------------------------------------------------------
NiFXLParameterMatrixDesc* NiFXLParameterDesc::GetMatrixDesc()
{
    memset(&ms_kMatrixDesc, 0, sizeof(ms_kMatrixDesc));
    ms_kMatrixDesc.m_eParamType = PT_MATRIX;
    return &ms_kMatrixDesc;
}
//---------------------------------------------------------------------------
NiFXLParameterVectorDesc* NiFXLParameterDesc::GetVectorDesc()
{
    memset(&ms_kVectorDesc, 0, sizeof(ms_kVectorDesc));
    ms_kVectorDesc.m_eParamType = PT_VECTOR;
    return &ms_kVectorDesc;
}
//---------------------------------------------------------------------------
NiFXLParameterFloatDesc* NiFXLParameterDesc::GetFloatDesc()
{
    memset(&ms_kFloatDesc, 0, sizeof(ms_kFloatDesc));
    ms_kFloatDesc.m_eParamType = PT_FLOAT;
    return &ms_kFloatDesc;
}
//---------------------------------------------------------------------------
NiFXLParameterBooleanDesc* NiFXLParameterDesc::GetBooleanDesc()
{
    memset(&ms_kBooleanDesc, 0, sizeof(ms_kBooleanDesc));
    ms_kBooleanDesc.m_eParamType = PT_BOOL;
    return &ms_kBooleanDesc;
}
//---------------------------------------------------------------------------
NiFXLParameterIntegerDesc* NiFXLParameterDesc::GetIntegerDesc()
{
    memset(&ms_kIntegerDesc, 0, sizeof(ms_kIntegerDesc));
    ms_kIntegerDesc.m_eParamType = PT_UINT;
    return &ms_kIntegerDesc;
}
//---------------------------------------------------------------------------
NiFXLParameterSamplerDesc* NiFXLParameterDesc::GetSamplerDesc()
{
    memset(&ms_kSamplerDesc, 0, sizeof(ms_kSamplerDesc));
    ms_kSamplerDesc.m_eParamType = PT_SAMPLER;
    return &ms_kSamplerDesc;
}
//---------------------------------------------------------------------------
NiFXLParameterPredefinedDesc*
    NiFXLParameterDesc::GetPredefinedDesc()
{
    memset(&ms_kPredefinedDesc, 0, sizeof(ms_kPredefinedDesc));
    ms_kPredefinedDesc.m_eParamType = PT_PREDEFINED;
    return &ms_kPredefinedDesc;
}
//---------------------------------------------------------------------------
NiFXLParameterObjectDesc* NiFXLParameterDesc::GetObjectDesc()
{
    memset(&ms_kObjectDesc, 0, sizeof(ms_kObjectDesc));
    ms_kObjectDesc.m_eParamType = PT_OBJECT;
    return &ms_kObjectDesc;
}
//---------------------------------------------------------------------------
NiFXLParameterArrayDesc* 
NiFXLParameterDesc::GetArrayDesc()
{
    memset(&ms_kArrayDesc, 0, sizeof(ms_kArrayDesc));
    ms_kArrayDesc.m_eParamType = PT_ARRAY;
    return &ms_kArrayDesc;
}
//---------------------------------------------------------------------------
NiFXLParameterVectorDesc::VectorType 
    NiFXLParameterVectorDesc::GetVectorType()
{
    NIASSERT(m_uiNumAtomicElements != 1); // Single float
    if (m_uiNumRows == 1 && m_uiNumCols == 2 && m_uiNumAtomicElements 
        == 2)
    {
        return TYPE_POINT2;
    }
    else if (m_uiNumRows == 1 && m_uiNumCols == 3 && m_uiNumAtomicElements 
        == 3)
    {
        return TYPE_POINT3;
    }
    else
    {
        NIASSERT(m_uiNumRows == 1 && m_uiNumCols == 4 && m_uiNumAtomicElements 
            == 4);
        return TYPE_POINT4;
    }

}
//---------------------------------------------------------------------------
NiFXLParameterArrayDesc::ArrayType 
    NiFXLParameterArrayDesc::GetArrayType()
{
    if (m_uiNumRows == 1 && m_uiNumCols == 1)
    {
        return TYPE_FLOAT;
    }
    else if (m_uiNumRows == 1 && m_uiNumCols == 2)
    {
        return TYPE_POINT2;
    }
    else if (m_uiNumRows == 1 && m_uiNumCols == 3)
    {
        return TYPE_POINT3;
    }
    else if (m_uiNumRows == 1 && m_uiNumCols == 4 && m_bColor)
    {
        return TYPE_COLORA;
    }
    else
    {
        NIASSERT(m_uiNumRows == 1 && m_uiNumCols == 4);
        return TYPE_POINT4;
    }
}
//---------------------------------------------------------------------------
unsigned int NiFXLParameterArrayDesc::GetElementSize()
{
    if (m_uiNumRows == 1 && m_uiNumCols == 1)
    {
        return sizeof(float);
    }
    else if (m_uiNumRows == 1 && m_uiNumCols == 2)
    {
        return 2 * sizeof(float);
    }
    else if (m_uiNumRows == 1 && m_uiNumCols == 3)
    {
        return 3 * sizeof(float);
    }
    else
    {
        NIASSERT(m_uiNumRows == 1 && m_uiNumCols == 4);
        return 4 * sizeof(float);
    }
}
//---------------------------------------------------------------------------
bool NiFXLParameterArrayDesc::ExpandToVector4Array(
    const float* pfSrcArray, D3DXVECTOR4* pfDestArray,
    unsigned int uiRows, unsigned int uiColumns, 
    unsigned int uiNumElements)
{
    unsigned int uiAtomicTotal = uiRows * uiColumns * uiNumElements;
    if (uiRows == 1 && uiColumns == 2)
    {
        unsigned int uiArrayIdx = 0;
        for (unsigned int ui = 0; ui < uiNumElements; ui++)
        {
            unsigned int uj = 0;
            float* pfDest = (float*) pfDestArray[ui];
            pfDest[uj++] = pfSrcArray[uiArrayIdx++];
            pfDest[uj++] = pfSrcArray[uiArrayIdx++];
            pfDest[uj++] = 0.0f;
            pfDest[uj++] = 0.0f;
        }
        return true;
    }
    else if (uiRows == 1 && uiColumns == 3)
    {
        unsigned int uiArrayIdx = 0;
        for (unsigned int ui = 0; ui < uiNumElements; ui++)
        {
            unsigned int uj = 0;
            float* pfDest = (float*) pfDestArray[ui];
            pfDest[uj++] = pfSrcArray[uiArrayIdx++];
            pfDest[uj++] = pfSrcArray[uiArrayIdx++];
            pfDest[uj++] = pfSrcArray[uiArrayIdx++];
            pfDest[uj++] = 0.0f;
        }
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiFXLParameterArrayDesc::CompactFromVector4Array(
    const D3DXVECTOR4* pfSrcArray, float* pfDestArray,
    unsigned int uiRows, unsigned int uiColumns, 
    unsigned int uiNumElements)
{
    unsigned int uiAtomicTotal = uiRows * uiColumns * uiNumElements;
    if (uiRows == 1 && uiColumns == 2)
    {
        unsigned int uiArrayIdx = 0;
        for (unsigned int ui = 0; ui < uiNumElements; ui++)
        {
            unsigned int uj = 0;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].x;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].y;
        }
        return true;
    }
    else if (uiRows == 1 && uiColumns == 3)
    {
        unsigned int uiArrayIdx = 0;
        for (unsigned int ui = 0; ui < uiNumElements; ui++)
        {
            unsigned int uj = 0;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].x;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].y;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].z;
        }
        return true;
    }
    else if (uiRows == 1 && uiColumns == 4)
    {
        unsigned int uiArrayIdx = 0;
        for (unsigned int ui = 0; ui < uiNumElements; ui++)
        {
            unsigned int uj = 0;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].x;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].y;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].z;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].w;
        }
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
