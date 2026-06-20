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

#include "NiD3D10Error.h"
#include "NiD3D10RenderStateManager.h"

#include <NiD3D10RenderStateGroup.h>
#include <NiPropertyState.h>
#include <NiWireframeProperty.h>

//---------------------------------------------------------------------------
NiD3D10RenderStateManager::NiD3D10RenderStateManager(ID3D10Device* pkDevice,
    NiD3D10DeviceState* pkDeviceState) :
    m_pkDevice(pkDevice),
    m_pkDeviceState(pkDeviceState),
    m_uiDefaultSampleMask(UINT_MAX),
    m_uiDefaultStencilRef(0),
    m_uiCurrentSampleMask(UINT_MAX),
    m_uiCurrentStencilRef(0),
    m_bLeftRightSwap(false)
{
    assert (m_pkDevice);
    if (m_pkDevice)
        m_pkDevice->AddRef();

    // Fill in with D3D10's default values, not Gamebryo's default values.
    memset(&m_kDefaultBlendDesc, 0, sizeof(m_kDefaultBlendDesc));
    m_kDefaultBlendDesc.AlphaToCoverageEnable = false;
    m_kDefaultBlendDesc.BlendEnable[0] = false;
    m_kDefaultBlendDesc.BlendEnable[1] = false;
    m_kDefaultBlendDesc.BlendEnable[2] = false;
    m_kDefaultBlendDesc.BlendEnable[3] = false;
    m_kDefaultBlendDesc.BlendEnable[4] = false;
    m_kDefaultBlendDesc.BlendEnable[5] = false;
    m_kDefaultBlendDesc.BlendEnable[6] = false;
    m_kDefaultBlendDesc.BlendEnable[7] = false;
    m_kDefaultBlendDesc.SrcBlend = D3D10_BLEND_ONE;
    m_kDefaultBlendDesc.DestBlend = D3D10_BLEND_ZERO; 
    m_kDefaultBlendDesc.BlendOp = D3D10_BLEND_OP_ADD; 
    m_kDefaultBlendDesc.SrcBlendAlpha = D3D10_BLEND_ONE; 
    m_kDefaultBlendDesc.DestBlendAlpha = D3D10_BLEND_ZERO; 
    m_kDefaultBlendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD; 
    m_kDefaultBlendDesc.RenderTargetWriteMask[0] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kDefaultBlendDesc.RenderTargetWriteMask[1] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kDefaultBlendDesc.RenderTargetWriteMask[2] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kDefaultBlendDesc.RenderTargetWriteMask[3] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kDefaultBlendDesc.RenderTargetWriteMask[4] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kDefaultBlendDesc.RenderTargetWriteMask[5] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kDefaultBlendDesc.RenderTargetWriteMask[6] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kDefaultBlendDesc.RenderTargetWriteMask[7] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;

    memset(&m_kCurrentBlendDesc, 0, sizeof(m_kCurrentBlendDesc));
    m_kCurrentBlendDesc.AlphaToCoverageEnable = false;
    m_kCurrentBlendDesc.BlendEnable[0] = false;
    m_kCurrentBlendDesc.BlendEnable[1] = false;
    m_kCurrentBlendDesc.BlendEnable[2] = false;
    m_kCurrentBlendDesc.BlendEnable[3] = false;
    m_kCurrentBlendDesc.BlendEnable[4] = false;
    m_kCurrentBlendDesc.BlendEnable[5] = false;
    m_kCurrentBlendDesc.BlendEnable[6] = false;
    m_kCurrentBlendDesc.BlendEnable[7] = false;
    m_kCurrentBlendDesc.SrcBlend = D3D10_BLEND_ONE;
    m_kCurrentBlendDesc.DestBlend = D3D10_BLEND_ZERO; 
    m_kCurrentBlendDesc.BlendOp = D3D10_BLEND_OP_ADD; 
    m_kCurrentBlendDesc.SrcBlendAlpha = D3D10_BLEND_ONE; 
    m_kCurrentBlendDesc.DestBlendAlpha = D3D10_BLEND_ZERO; 
    m_kCurrentBlendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD; 
    m_kCurrentBlendDesc.RenderTargetWriteMask[0] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kCurrentBlendDesc.RenderTargetWriteMask[1] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kCurrentBlendDesc.RenderTargetWriteMask[2] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kCurrentBlendDesc.RenderTargetWriteMask[3] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kCurrentBlendDesc.RenderTargetWriteMask[4] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kCurrentBlendDesc.RenderTargetWriteMask[5] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kCurrentBlendDesc.RenderTargetWriteMask[6] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;
    m_kCurrentBlendDesc.RenderTargetWriteMask[7] = 
        D3D10_COLOR_WRITE_ENABLE_ALL;

    memset(&m_kDefaultDepthStencilDesc, 0, sizeof(m_kDefaultDepthStencilDesc));
    m_kDefaultDepthStencilDesc.DepthEnable = true;
    m_kDefaultDepthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
    m_kDefaultDepthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
    m_kDefaultDepthStencilDesc.StencilEnable = false;
    m_kDefaultDepthStencilDesc.StencilReadMask = 0;
    m_kDefaultDepthStencilDesc.StencilWriteMask = 0;
    m_kDefaultDepthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
    m_kDefaultDepthStencilDesc.FrontFace.StencilDepthFailOp = 
        D3D10_STENCIL_OP_KEEP;
    m_kDefaultDepthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    m_kDefaultDepthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    m_kDefaultDepthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
    m_kDefaultDepthStencilDesc.BackFace.StencilDepthFailOp = 
        D3D10_STENCIL_OP_KEEP;
    m_kDefaultDepthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    m_kDefaultDepthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;

    memset(&m_kCurrentDepthStencilDesc, 0, sizeof(m_kCurrentDepthStencilDesc));
    m_kCurrentDepthStencilDesc.DepthEnable = true;
    m_kCurrentDepthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
    m_kCurrentDepthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
    m_kCurrentDepthStencilDesc.StencilEnable = false;
    m_kCurrentDepthStencilDesc.StencilReadMask = 0;
    m_kCurrentDepthStencilDesc.StencilWriteMask = 0;
    m_kCurrentDepthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
    m_kCurrentDepthStencilDesc.FrontFace.StencilDepthFailOp = 
        D3D10_STENCIL_OP_KEEP;
    m_kCurrentDepthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    m_kCurrentDepthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    m_kCurrentDepthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
    m_kCurrentDepthStencilDesc.BackFace.StencilDepthFailOp = 
        D3D10_STENCIL_OP_KEEP;
    m_kCurrentDepthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    m_kCurrentDepthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;

    memset(&m_kDefaultRasterizerDesc, 0, sizeof(m_kDefaultRasterizerDesc));
    m_kDefaultRasterizerDesc.FillMode = D3D10_FILL_SOLID;
    m_kDefaultRasterizerDesc.CullMode = D3D10_CULL_BACK;
    m_kDefaultRasterizerDesc.FrontCounterClockwise = false;
    m_kDefaultRasterizerDesc.DepthBias = 0;
    m_kDefaultRasterizerDesc.DepthBiasClamp = 0.0f;
    m_kDefaultRasterizerDesc.SlopeScaledDepthBias = 0.0f;
    m_kDefaultRasterizerDesc.DepthClipEnable = true;
    m_kDefaultRasterizerDesc.ScissorEnable = false;
    m_kDefaultRasterizerDesc.MultisampleEnable = false;
    m_kDefaultRasterizerDesc.AntialiasedLineEnable = false;

    memset(&m_kCurrentRasterizerDesc, 0, sizeof(m_kCurrentRasterizerDesc));
    m_kCurrentRasterizerDesc.FillMode = D3D10_FILL_SOLID;
    m_kCurrentRasterizerDesc.CullMode = D3D10_CULL_BACK;
    m_kCurrentRasterizerDesc.FrontCounterClockwise = false;
    m_kCurrentRasterizerDesc.DepthBias = 0;
    m_kCurrentRasterizerDesc.DepthBiasClamp = 0.0f;
    m_kCurrentRasterizerDesc.SlopeScaledDepthBias = 0.0f;
    m_kCurrentRasterizerDesc.DepthClipEnable = true;
    m_kCurrentRasterizerDesc.ScissorEnable = false;
    m_kCurrentRasterizerDesc.MultisampleEnable = false;
    m_kCurrentRasterizerDesc.AntialiasedLineEnable = false;

    for (unsigned int i = 0; i < NiGPUProgram::PROGRAM_MAX; i++)
    {
        memset(m_aakDefaultSamplerDescs[i], 0, 
            sizeof(m_aakDefaultSamplerDescs[i]));
        memset(m_aakCurrentSamplerDescs[i], 0, 
            sizeof(m_aakCurrentSamplerDescs[i]));
        for (unsigned int j = 0; j < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; 
            j++)
        {
            m_aakDefaultSamplerDescs[i][j].Filter = 
                D3D10_FILTER_MIN_MAG_MIP_LINEAR;
            m_aakDefaultSamplerDescs[i][j].AddressU = 
                D3D10_TEXTURE_ADDRESS_CLAMP;
            m_aakDefaultSamplerDescs[i][j].AddressV = 
                D3D10_TEXTURE_ADDRESS_CLAMP;
            m_aakDefaultSamplerDescs[i][j].AddressW = 
                D3D10_TEXTURE_ADDRESS_CLAMP;
            m_aakDefaultSamplerDescs[i][j].MipLODBias = 0;
            m_aakDefaultSamplerDescs[i][j].MaxAnisotropy = 1;
            m_aakDefaultSamplerDescs[i][j].ComparisonFunc = 
                D3D10_COMPARISON_NEVER;
            m_aakDefaultSamplerDescs[i][j].BorderColor[0] = 1.0f;
            m_aakDefaultSamplerDescs[i][j].BorderColor[1] = 1.0f;
            m_aakDefaultSamplerDescs[i][j].BorderColor[2] = 1.0f;
            m_aakDefaultSamplerDescs[i][j].BorderColor[3] = 1.0f;
            m_aakDefaultSamplerDescs[i][j].MinLOD = -FLT_MAX;
            m_aakDefaultSamplerDescs[i][j].MaxLOD = FLT_MAX;

            m_aakCurrentSamplerDescs[i][j].Filter = 
                D3D10_FILTER_MIN_MAG_MIP_LINEAR;
            m_aakCurrentSamplerDescs[i][j].AddressU = 
                D3D10_TEXTURE_ADDRESS_CLAMP;
            m_aakCurrentSamplerDescs[i][j].AddressV = 
                D3D10_TEXTURE_ADDRESS_CLAMP;
            m_aakCurrentSamplerDescs[i][j].AddressW = 
                D3D10_TEXTURE_ADDRESS_CLAMP;
            m_aakCurrentSamplerDescs[i][j].MipLODBias = 0;
            m_aakCurrentSamplerDescs[i][j].MaxAnisotropy = 1;
            m_aakCurrentSamplerDescs[i][j].ComparisonFunc = 
                D3D10_COMPARISON_NEVER;
            m_aakCurrentSamplerDescs[i][j].BorderColor[0] = 1.0f;
            m_aakCurrentSamplerDescs[i][j].BorderColor[1] = 1.0f;
            m_aakCurrentSamplerDescs[i][j].BorderColor[2] = 1.0f;
            m_aakCurrentSamplerDescs[i][j].BorderColor[3] = 1.0f;
            m_aakCurrentSamplerDescs[i][j].MinLOD = -FLT_MAX;
            m_aakCurrentSamplerDescs[i][j].MaxLOD = FLT_MAX;
        }
    }

    memset(m_afDefaultBlendFactor, 0, sizeof(m_afDefaultBlendFactor));
    memset(m_afCurrentBlendFactor, 0, sizeof(m_afCurrentBlendFactor));

    InitDefaultValues();
}
//---------------------------------------------------------------------------
NiD3D10RenderStateManager::~NiD3D10RenderStateManager()
{
    if (m_pkDevice)
        m_pkDevice->Release();
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateManager::GetLeftRightSwap() const
{
    return m_bLeftRightSwap;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetLeftRightSwap(bool bSwap)
{
    m_bLeftRightSwap = bSwap;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::InitDefaultValues()
{
    // Fill in Gamebryo's default values.

    // Winding order
    m_kDefaultRasterizerDesc.FrontCounterClockwise = true;
    m_kCurrentRasterizerDesc.FrontCounterClockwise = true;

    // Enable multisampling
    m_kDefaultRasterizerDesc.MultisampleEnable = true;
    m_kDefaultRasterizerDesc.AntialiasedLineEnable = true;
    m_kCurrentRasterizerDesc.MultisampleEnable = true;
    m_kCurrentRasterizerDesc.AntialiasedLineEnable = true;

    ResetCurrentState();
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateManager::SetProperties(const NiPropertyState* pkState)
{
    if (pkState == NULL)
        return false;

    // Note that Gamebryo's Dither, Fog, Material, Shading, and Specular 
    // properties are either entirely implemented by shaders or are not
    // supported in D3D10.
    if ((SetAlphaProperty(pkState->GetAlpha()) == false) ||
        (SetStencilProperty(pkState->GetStencil()) == false) ||
        (SetWireframeProperty(pkState->GetWireframe()) == false) ||
        (SetZBufferProperty(pkState->GetZBuffer()) == false))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateManager::SetAlphaProperty(const NiAlphaProperty* pkNew)
{
    if (pkNew == NULL)
    {
        NiD3D10Error::ReportWarning("Cannot apply alpha state with NULL "
            "alpha property.");
        return false;
    }

    if (pkNew->GetAlphaBlending())
    {
        m_kCurrentBlendDesc.BlendEnable[0] = true;
        m_kCurrentBlendDesc.BlendEnable[1] = true;
        m_kCurrentBlendDesc.BlendEnable[2] = true;
        m_kCurrentBlendDesc.BlendEnable[3] = true;
        m_kCurrentBlendDesc.BlendEnable[4] = true;
        m_kCurrentBlendDesc.BlendEnable[5] = true;
        m_kCurrentBlendDesc.BlendEnable[6] = true;
        m_kCurrentBlendDesc.BlendEnable[7] = true;
        m_kCurrentBlendDesc.SrcBlend = ConvertGbBlendToD3D10Blend(
            pkNew->GetSrcBlendMode());
        m_kCurrentBlendDesc.DestBlend = ConvertGbBlendToD3D10Blend(
            pkNew->GetDestBlendMode());
        m_kCurrentBlendDesc.BlendOp = D3D10_BLEND_OP_ADD;
    }
    else
    {
        m_kCurrentBlendDesc.BlendEnable[0] = false;
        m_kCurrentBlendDesc.BlendEnable[1] = false;
        m_kCurrentBlendDesc.BlendEnable[2] = false;
        m_kCurrentBlendDesc.BlendEnable[3] = false;
        m_kCurrentBlendDesc.BlendEnable[4] = false;
        m_kCurrentBlendDesc.BlendEnable[5] = false;
        m_kCurrentBlendDesc.BlendEnable[6] = false;
        m_kCurrentBlendDesc.BlendEnable[7] = false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateManager::SetStencilProperty(
    const NiStencilProperty* pkNew)
{
    if (pkNew == NULL)
    {
        NiD3D10Error::ReportWarning("Cannot apply stencil state with NULL "
            "stencil property.");
        return false;
    }

    if (pkNew->GetStencilOn())
    {
        m_kCurrentDepthStencilDesc.StencilEnable = true;

        m_kCurrentDepthStencilDesc.StencilWriteMask = 
            pkNew->GetStencilMask() & UCHAR_MAX;
        m_kCurrentDepthStencilDesc.StencilReadMask = 
            pkNew->GetStencilMask() & UCHAR_MAX;

        m_uiCurrentStencilRef = pkNew->GetStencilReference();

        m_kCurrentDepthStencilDesc.FrontFace.StencilFunc = 
            ConvertGbStencilFuncToD3D10Comparison(pkNew->GetStencilFunction());
        m_kCurrentDepthStencilDesc.FrontFace.StencilDepthFailOp = 
            ConvertGbStencilActionToD3D10StencilOp(
            pkNew->GetStencilPassZFailAction());
        m_kCurrentDepthStencilDesc.FrontFace.StencilPassOp = 
            ConvertGbStencilActionToD3D10StencilOp(
            pkNew->GetStencilPassAction());
        m_kCurrentDepthStencilDesc.FrontFace.StencilFailOp = 
            ConvertGbStencilActionToD3D10StencilOp(
            pkNew->GetStencilFailAction());
        m_kCurrentDepthStencilDesc.BackFace = 
            m_kCurrentDepthStencilDesc.FrontFace;
    }
    else
    {
        m_kCurrentDepthStencilDesc.StencilEnable = false;
    }

    NiStencilProperty::DrawMode eMode = pkNew->GetDrawMode();

    m_kCurrentRasterizerDesc.FrontCounterClockwise = true;
    if (eMode == NiStencilProperty::DRAW_CW)
    {
        m_kCurrentRasterizerDesc.CullMode = D3D10_CULL_FRONT;
    }
    else if (eMode == NiStencilProperty::DRAW_BOTH)
    {
        m_kCurrentRasterizerDesc.CullMode = D3D10_CULL_NONE;
    }
    else //(eMode == NiStencilProperty::DRAW_CCW_OR_BOTH ||
         // eMode == NiStencilProperty::DRAW_CCW)
    {
        m_kCurrentRasterizerDesc.CullMode = D3D10_CULL_BACK;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateManager::SetWireframeProperty(
    const NiWireframeProperty* pkNew)
{
    if (pkNew == NULL)
    {
        NiD3D10Error::ReportWarning("Cannot apply wireframe state with NULL "
            "wireframe property.");
        return false;
    }

    if (pkNew->GetWireframe())
        m_kCurrentRasterizerDesc.FillMode = D3D10_FILL_WIREFRAME;
    else
        m_kCurrentRasterizerDesc.FillMode = D3D10_FILL_SOLID;

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateManager::SetZBufferProperty(
    const NiZBufferProperty* pkNew)
{
    if (pkNew == NULL)
    {
        NiD3D10Error::ReportWarning("Cannot apply Z buffer state with NULL "
            "Z buffer property.");
        return false;
    }

    m_kCurrentDepthStencilDesc.DepthEnable = 
        (pkNew->GetZBufferTest() || pkNew->GetZBufferWrite());
    if (pkNew->GetZBufferWrite())
    {
        m_kCurrentDepthStencilDesc.DepthWriteMask = 
            D3D10_DEPTH_WRITE_MASK_ALL;
    }
    else
    {
        m_kCurrentDepthStencilDesc.DepthWriteMask =
            D3D10_DEPTH_WRITE_MASK_ZERO;
    }

    if (pkNew->GetZBufferTest())
    {
        m_kCurrentDepthStencilDesc.DepthFunc =
            ConvertGbDepthFuncToD3D10Comparison(pkNew->GetTestFunction());
    }
    else
    {
        m_kCurrentDepthStencilDesc.DepthFunc = D3D10_COMPARISON_ALWAYS;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateManager::SetRenderStateGroup(
    const NiD3D10RenderStateGroup* pkRSGroup)
{
    D3D10_BLEND_DESC kBlendDesc;
    unsigned int uiBlendStateValid = 0;
    pkRSGroup->GetBlendStateDesc(kBlendDesc, uiBlendStateValid);

    SetBlendStateDesc(kBlendDesc, uiBlendStateValid);

    float afBlendFactor[4];
    if (pkRSGroup->GetBlendFactor(afBlendFactor))
    {
        SetBlendFactor(afBlendFactor);
    }

    unsigned int uiSampleMask = 0;
    if (pkRSGroup->GetSampleMask(uiSampleMask))
    {
        SetSampleMask(uiSampleMask);
    }

    D3D10_DEPTH_STENCIL_DESC kDepthStencilDesc;
    unsigned int uiDepthStencilStateValid = 0;
    pkRSGroup->GetDepthStencilStateDesc(kDepthStencilDesc, 
        uiDepthStencilStateValid);

    SetDepthStencilStateDesc(kDepthStencilDesc, uiDepthStencilStateValid);

    unsigned int uiStencilRef = 0;
    if (pkRSGroup->GetStencilRef(uiStencilRef))
    {
        SetStencilRef(uiStencilRef);
    }

    D3D10_RASTERIZER_DESC kRasterizerDesc;
    unsigned int uiRasterizerStateValid = 0;
    pkRSGroup->GetRasterizerStateDesc(kRasterizerDesc, uiRasterizerStateValid);

    SetRasterizerStateDesc(kRasterizerDesc, uiRasterizerStateValid);

    D3D10_SAMPLER_DESC kSamplerDesc;
    unsigned int uiSamplerValid = 0;
    for (unsigned int i = 0; i < NiGPUProgram::PROGRAM_MAX; i++)
    {
        const unsigned char* pucSamplerRemapping = 
            pkRSGroup->GetSamplerRemapping((NiGPUProgram::ProgramType)i);
        for (unsigned int j = 0; j < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; 
            j++)
        {
            uiSamplerValid = 0;
            pkRSGroup->GetSamplerDesc((NiGPUProgram::ProgramType)i, 
                pucSamplerRemapping[j], kSamplerDesc, uiSamplerValid);
            SetSamplerDesc((NiGPUProgram::ProgramType)i, j,
                kSamplerDesc, uiSamplerValid);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetBlendStateDesc(
    const D3D10_BLEND_DESC& kDesc, unsigned int uiValidFlags)
{
    if ((uiValidFlags & BSVALID_ALPHATOCOVERAGEENABLE) != 0)
    {
        m_kCurrentBlendDesc.AlphaToCoverageEnable = 
            kDesc.AlphaToCoverageEnable;
    }
    if ((uiValidFlags & BSVALID_BLENDENABLE_0) != 0)
    {
        m_kCurrentBlendDesc.BlendEnable[0] = kDesc.BlendEnable[0];
    }
    if ((uiValidFlags & BSVALID_BLENDENABLE_1) != 0)
    {
        m_kCurrentBlendDesc.BlendEnable[1] = kDesc.BlendEnable[1];
    }
    if ((uiValidFlags & BSVALID_BLENDENABLE_2) != 0)
    {
        m_kCurrentBlendDesc.BlendEnable[2] = kDesc.BlendEnable[2];
    }
    if ((uiValidFlags & BSVALID_BLENDENABLE_3) != 0)
    {
        m_kCurrentBlendDesc.BlendEnable[3] = kDesc.BlendEnable[3];
    }
    if ((uiValidFlags & BSVALID_BLENDENABLE_4) != 0)
    {
        m_kCurrentBlendDesc.BlendEnable[4] = kDesc.BlendEnable[4];
    }
    if ((uiValidFlags & BSVALID_BLENDENABLE_5) != 0)
    {
        m_kCurrentBlendDesc.BlendEnable[5] = kDesc.BlendEnable[5];
    }
    if ((uiValidFlags & BSVALID_BLENDENABLE_6) != 0)
    {
        m_kCurrentBlendDesc.BlendEnable[6] = kDesc.BlendEnable[6];
    }
    if ((uiValidFlags & BSVALID_BLENDENABLE_7) != 0)
    {
        m_kCurrentBlendDesc.BlendEnable[7] = kDesc.BlendEnable[7];
    }
    if ((uiValidFlags & BSVALID_SRCBLEND) != 0)
    {
        m_kCurrentBlendDesc.SrcBlend = kDesc.SrcBlend;
    }
    if ((uiValidFlags & BSVALID_DESTBLEND) != 0)
    {
        m_kCurrentBlendDesc.DestBlend = kDesc.DestBlend;
    }
    if ((uiValidFlags & BSVALID_BLENDOP) != 0)
    {
        m_kCurrentBlendDesc.BlendOp = kDesc.BlendOp;
    }
    if ((uiValidFlags & BSVALID_SRCBLENDALPHA) != 0)
    {
        m_kCurrentBlendDesc.SrcBlendAlpha = kDesc.SrcBlendAlpha;
    }
    if ((uiValidFlags & BSVALID_DESTBLENDALPHA) != 0)
    {
        m_kCurrentBlendDesc.DestBlendAlpha = kDesc.DestBlendAlpha;
    }
    if ((uiValidFlags & BSVALID_BLENDOPALPHA) != 0)
    {
        m_kCurrentBlendDesc.BlendOpAlpha = kDesc.BlendOpAlpha;
    }
    if ((uiValidFlags & BSVALID_RENDERTARGETWRITEMASK_0) != 0)
    {
        m_kCurrentBlendDesc.RenderTargetWriteMask[0] = 
            kDesc.RenderTargetWriteMask[0];
    }
    if ((uiValidFlags & BSVALID_RENDERTARGETWRITEMASK_1) != 0)
    {
        m_kCurrentBlendDesc.RenderTargetWriteMask[1] = 
            kDesc.RenderTargetWriteMask[1];
    }
    if ((uiValidFlags & BSVALID_RENDERTARGETWRITEMASK_2) != 0)
    {
        m_kCurrentBlendDesc.RenderTargetWriteMask[2] = 
            kDesc.RenderTargetWriteMask[2];
    }
    if ((uiValidFlags & BSVALID_RENDERTARGETWRITEMASK_3) != 0)
    {
        m_kCurrentBlendDesc.RenderTargetWriteMask[3] = 
            kDesc.RenderTargetWriteMask[3];
    }
    if ((uiValidFlags & BSVALID_RENDERTARGETWRITEMASK_4) != 0)
    {
        m_kCurrentBlendDesc.RenderTargetWriteMask[4] = 
            kDesc.RenderTargetWriteMask[4];
    }
    if ((uiValidFlags & BSVALID_RENDERTARGETWRITEMASK_5) != 0)
    {
        m_kCurrentBlendDesc.RenderTargetWriteMask[5] = 
            kDesc.RenderTargetWriteMask[5];
    }
    if ((uiValidFlags & BSVALID_RENDERTARGETWRITEMASK_6) != 0)
    {
        m_kCurrentBlendDesc.RenderTargetWriteMask[6] = 
            kDesc.RenderTargetWriteMask[6];
    }
    if ((uiValidFlags & BSVALID_RENDERTARGETWRITEMASK_7) != 0)
    {
        m_kCurrentBlendDesc.RenderTargetWriteMask[7] = 
            kDesc.RenderTargetWriteMask[7];
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetBlendStateDesc(
    D3D10_BLEND_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    kDesc = m_kCurrentBlendDesc;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetBlendFactor(const float afBlendFactor[4])
{
    m_afCurrentBlendFactor[0] = afBlendFactor[0];
    m_afCurrentBlendFactor[1] = afBlendFactor[1];
    m_afCurrentBlendFactor[2] = afBlendFactor[2];
    m_afCurrentBlendFactor[3] = afBlendFactor[3];
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetBlendFactor(float afBlendFactor[4]) const
{
    afBlendFactor[0] = m_afCurrentBlendFactor[0];
    afBlendFactor[1] = m_afCurrentBlendFactor[1];
    afBlendFactor[2] = m_afCurrentBlendFactor[2];
    afBlendFactor[3] = m_afCurrentBlendFactor[3];
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetSampleMask(unsigned int uiSampleMask)
{
    m_uiCurrentSampleMask = uiSampleMask;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetSampleMask(unsigned int& uiSampleMask) const
{
    uiSampleMask = m_uiCurrentSampleMask;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetDepthStencilStateDesc(
    const D3D10_DEPTH_STENCIL_DESC& kDesc, unsigned int uiValidFlags)
{
    if ((uiValidFlags & DSSVALID_DEPTHENABLE) != 0)
    {
        m_kCurrentDepthStencilDesc.DepthEnable = kDesc.DepthEnable;
    }
    if ((uiValidFlags & DSSVALID_DEPTHWRITEMASK) != 0)
    {
        m_kCurrentDepthStencilDesc.DepthWriteMask = kDesc.DepthWriteMask;
    }
    if ((uiValidFlags & DSSVALID_DEPTHFUNC) != 0)
    {
        m_kCurrentDepthStencilDesc.DepthFunc = kDesc.DepthFunc;
    }
    if ((uiValidFlags & DSSVALID_STENCILENABLE) != 0)
    {
        m_kCurrentDepthStencilDesc.StencilEnable = kDesc.StencilEnable;
    }
    if ((uiValidFlags & DSSVALID_STENCILREADMASK) != 0)
    {
        m_kCurrentDepthStencilDesc.StencilReadMask = kDesc.StencilReadMask;
    }
    if ((uiValidFlags & DSSVALID_STENCILWRITEMASK) != 0)
    {
        m_kCurrentDepthStencilDesc.StencilWriteMask = kDesc.StencilWriteMask;
    }
    if ((uiValidFlags & DSSVALID_FRONTFACE_STENCILFAILOP) != 0)
    {
        m_kCurrentDepthStencilDesc.FrontFace.StencilFailOp = 
            kDesc.FrontFace.StencilFailOp;
    }
    if ((uiValidFlags & DSSVALID_FRONTFACE_STENCILDEPTHFAILOP) != 0)
    {
        m_kCurrentDepthStencilDesc.FrontFace.StencilDepthFailOp = 
            kDesc.FrontFace.StencilDepthFailOp;
    }
    if ((uiValidFlags & DSSVALID_FRONTFACE_STENCILPASSOP) != 0)
    {
        m_kCurrentDepthStencilDesc.FrontFace.StencilPassOp = 
            kDesc.FrontFace.StencilPassOp;
    }
    if ((uiValidFlags & DSSVALID_FRONTFACE_STENCILFUNC) != 0)
    {
        m_kCurrentDepthStencilDesc.FrontFace.StencilFunc = 
            kDesc.FrontFace.StencilFunc;
    }
    if ((uiValidFlags & DSSVALID_BACKFACE_STENCILFAILOP) != 0)
    {
        m_kCurrentDepthStencilDesc.BackFace.StencilFailOp = 
            kDesc.BackFace.StencilFailOp;
    }
    if ((uiValidFlags & DSSVALID_BACKFACE_STENCILDEPTHFAILOP) != 0)
    {
        m_kCurrentDepthStencilDesc.BackFace.StencilDepthFailOp = 
            kDesc.BackFace.StencilDepthFailOp;
    }
    if ((uiValidFlags & DSSVALID_BACKFACE_STENCILPASSOP) != 0)
    {
        m_kCurrentDepthStencilDesc.BackFace.StencilPassOp = 
            kDesc.BackFace.StencilPassOp;
    }
    if ((uiValidFlags & DSSVALID_BACKFACE_STENCILFUNC) != 0)
    {
        m_kCurrentDepthStencilDesc.BackFace.StencilFunc = 
            kDesc.BackFace.StencilFunc;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetDepthStencilStateDesc(
    D3D10_DEPTH_STENCIL_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    kDesc = m_kCurrentDepthStencilDesc;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetStencilRef(unsigned int uiStencilRef)
{
    m_uiCurrentStencilRef = uiStencilRef;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetStencilRef(unsigned int& uiStencilRef) const
{
    uiStencilRef = m_uiCurrentStencilRef;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetRasterizerStateDesc(
    const D3D10_RASTERIZER_DESC& kDesc, unsigned int uiValidFlags)
{
    if ((uiValidFlags & RSVALID_FILLMODE) != 0)
    {
        m_kCurrentRasterizerDesc.FillMode = kDesc.FillMode;
    }
    if ((uiValidFlags & RSVALID_CULLMODE) != 0)
    {
        m_kCurrentRasterizerDesc.CullMode = kDesc.CullMode;
    }
    if ((uiValidFlags & RSVALID_FRONTCOUNTERCLOCKWISE) != 0)
    {
        m_kCurrentRasterizerDesc.FrontCounterClockwise = 
            kDesc.FrontCounterClockwise;
    }
    if ((uiValidFlags & RSVALID_DEPTHBIAS) != 0)
    {
        m_kCurrentRasterizerDesc.DepthBias = kDesc.DepthBias;
    }
    if ((uiValidFlags & RSVALID_DEPTHBIASCLAMP) != 0)
    {
        m_kCurrentRasterizerDesc.DepthBiasClamp = kDesc.DepthBiasClamp;
    }
    if ((uiValidFlags & RSVALID_SLOPESCALEDDEPTHBIAS) != 0)
    {
        m_kCurrentRasterizerDesc.SlopeScaledDepthBias = 
            kDesc.SlopeScaledDepthBias;
    }
    if ((uiValidFlags & RSVALID_DEPTHCLIPENABLE) != 0)
    {
        m_kCurrentRasterizerDesc.DepthClipEnable = kDesc.DepthClipEnable;
    }
    if ((uiValidFlags & RSVALID_SCISSORENABLE) != 0)
    {
        m_kCurrentRasterizerDesc.ScissorEnable = kDesc.ScissorEnable;
    }
    if ((uiValidFlags & RSVALID_MULTISAMPLEENABLE) != 0)
    {
        m_kCurrentRasterizerDesc.MultisampleEnable = kDesc.MultisampleEnable;
    }
    if ((uiValidFlags & RSVALID_ANTIALIASEDLINEENABLE) != 0)
    {
        m_kCurrentRasterizerDesc.AntialiasedLineEnable = 
            kDesc.AntialiasedLineEnable;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetRasterizerStateDesc(
    D3D10_RASTERIZER_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    kDesc = m_kCurrentRasterizerDesc;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetSamplerDesc(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    const D3D10_SAMPLER_DESC& kDesc, unsigned int uiValidFlags)
{
    if (uiSampler >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        return;

    D3D10_SAMPLER_DESC& kCurrentDesc = 
        m_aakCurrentSamplerDescs[(unsigned int)eType][uiSampler];

    if ((uiValidFlags & SVALID_FILTER) != 0)
    {
        kCurrentDesc.Filter = kDesc.Filter;
    }
    if ((uiValidFlags & SVALID_ADDRESSU) != 0)
    {
        kCurrentDesc.AddressU = kDesc.AddressU;
    }
    if ((uiValidFlags & SVALID_ADDRESSV) != 0)
    {
        kCurrentDesc.AddressV = kDesc.AddressV;
    }
    if ((uiValidFlags & SVALID_ADDRESSW) != 0)
    {
        kCurrentDesc.AddressW = kDesc.AddressW;
    }
    if ((uiValidFlags & SVALID_MIPLODBIAS) != 0)
    {
        kCurrentDesc.MipLODBias = kDesc.MipLODBias;
    }
    if ((uiValidFlags & SVALID_MAXANISOTROPY) != 0)
    {
        kCurrentDesc.MaxAnisotropy = kDesc.MaxAnisotropy;
    }
    if ((uiValidFlags & SVALID_COMPARISONFUNC) != 0)
    {
        kCurrentDesc.ComparisonFunc = kDesc.ComparisonFunc;
    }
    if ((uiValidFlags & SVALID_BORDERCOLOR) != 0)
    {
        kCurrentDesc.BorderColor[0] = kDesc.BorderColor[0];
        kCurrentDesc.BorderColor[1] = kDesc.BorderColor[1];
        kCurrentDesc.BorderColor[2] = kDesc.BorderColor[2];
        kCurrentDesc.BorderColor[3] = kDesc.BorderColor[3];
    }
    if ((uiValidFlags & SVALID_MINLOD) != 0)
    {
        kCurrentDesc.MinLOD = kDesc.MinLOD;
    }
    if ((uiValidFlags & SVALID_MAXLOD) != 0)
    {
        kCurrentDesc.MaxLOD = kDesc.MaxLOD;
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetSamplerDesc(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    D3D10_SAMPLER_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        kDesc = m_aakCurrentSamplerDescs[(unsigned int)eType][uiSampler];
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::ResetCurrentState()
{
    m_kCurrentBlendDesc = m_kDefaultBlendDesc;
    m_kCurrentDepthStencilDesc = m_kDefaultDepthStencilDesc;
    m_kCurrentRasterizerDesc = m_kDefaultRasterizerDesc;
    unsigned int i = 0;
    for (; i < NiGPUProgram::PROGRAM_MAX; i++)
    {
        for (unsigned int j = 0; j < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; 
            j++)
        {
            m_aakCurrentSamplerDescs[i][j] = m_aakDefaultSamplerDescs[i][j];
        }
    }

    for (i = 0; i < 4; i++)
        m_afCurrentBlendFactor[i] = m_afDefaultBlendFactor[i];

    m_uiCurrentSampleMask = m_uiDefaultSampleMask;
    m_uiCurrentStencilRef = m_uiDefaultStencilRef;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetDefaultBlendStateDesc(
    const D3D10_BLEND_DESC& kDesc)
{
    m_kDefaultBlendDesc = kDesc;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetDefaultBlendStateDesc(
    D3D10_BLEND_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    kDesc = m_kDefaultBlendDesc;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetDefaultBlendFactor(
    const float afBlendFactor[4])
{
    m_afDefaultBlendFactor[0] = afBlendFactor[0];
    m_afDefaultBlendFactor[1] = afBlendFactor[1];
    m_afDefaultBlendFactor[2] = afBlendFactor[2];
    m_afDefaultBlendFactor[3] = afBlendFactor[3];
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetDefaultBlendFactor(
    float afBlendFactor[4]) const
{
    afBlendFactor[0] = m_afDefaultBlendFactor[0];
    afBlendFactor[1] = m_afDefaultBlendFactor[1];
    afBlendFactor[2] = m_afDefaultBlendFactor[2];
    afBlendFactor[3] = m_afDefaultBlendFactor[3];
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetDefaultSampleMask(unsigned int uiSampleMask)
{
    m_uiDefaultSampleMask = uiSampleMask;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetDefaultSampleMask(
    unsigned int& uiSampleMask) const
{
    uiSampleMask = m_uiDefaultSampleMask;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetDefaultDepthStencilStateDesc(
    const D3D10_DEPTH_STENCIL_DESC& kDesc)
{
    m_kDefaultDepthStencilDesc = kDesc;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetDefaultDepthStencilStateDesc(
    D3D10_DEPTH_STENCIL_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    kDesc = m_kDefaultDepthStencilDesc;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetDefaultStencilRef(unsigned int uiStenciRef)
{
    m_uiDefaultStencilRef = uiStenciRef;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetDefaultStencilRef(
    unsigned int& uiStencilRef) const
{
    uiStencilRef = m_uiDefaultStencilRef;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetDefaultRasterizerStateDesc(
    const D3D10_RASTERIZER_DESC& kDesc)
{
    m_kDefaultRasterizerDesc = kDesc;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetDefaultRasterizerStateDesc(
    D3D10_RASTERIZER_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    kDesc = m_kDefaultRasterizerDesc;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::SetDefaultSamplerDesc(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    const D3D10_SAMPLER_DESC& kDesc)
{
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        m_aakDefaultSamplerDescs[(unsigned int)eType][uiSampler] = kDesc;
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::GetDefaultSamplerDesc(
    NiGPUProgram::ProgramType eType, unsigned int uiSampler, 
    D3D10_SAMPLER_DESC& kDesc) const
{
    memset(&kDesc, 0, sizeof(kDesc));
    if (uiSampler < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        kDesc = m_aakDefaultSamplerDescs[(unsigned int)eType][uiSampler];
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::ApplyCurrentState()
{
    ApplyCurrentBlendState();
    ApplyCurrentDepthStencilState();
    ApplyCurrentRasterizerState();
    ApplyCurrentSamplers();
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::ApplyCurrentBlendState()
{
    ID3D10BlendState* pkBState = NULL;
    HRESULT hr = m_pkDevice->CreateBlendState(&m_kCurrentBlendDesc, 
        &pkBState);

    if (FAILED(hr) || pkBState == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_BLEND_STATE_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_BLEND_STATE_CREATION_FAILED,
                "No error message from D3D10, but blend state is NULL.");
        }
    }
    else
    {
        m_pkDeviceState->OMSetBlendState(pkBState, m_afCurrentBlendFactor, 
            m_uiCurrentSampleMask);
    }

    if (pkBState)
        pkBState->Release();
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::ApplyCurrentDepthStencilState()
{
    ID3D10DepthStencilState* pkDSState = NULL;
    HRESULT hr = m_pkDevice->CreateDepthStencilState(
        &m_kCurrentDepthStencilDesc, &pkDSState);

    if (FAILED(hr) || pkDSState == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_DEPTH_STENCIL_STATE_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(NiD3D10Error::
                NID3D10ERROR_DEPTH_STENCIL_STATE_CREATION_FAILED,
                "No error message from D3D10, but depth stencil state is "
                "NULL.");
        }
    }
    else
    {
        m_pkDeviceState->OMSetDepthStencilState(pkDSState, 
            m_uiCurrentStencilRef);
    }

    if (pkDSState)
        pkDSState->Release();
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::ApplyCurrentRasterizerState()
{
    // If left/right swap is enabled, the front/back state must be swapped.
    if (m_bLeftRightSwap)
    {
        m_kCurrentRasterizerDesc.FrontCounterClockwise = 
            !m_kCurrentRasterizerDesc.FrontCounterClockwise;
    }

    ID3D10RasterizerState* pkRState = NULL;
    HRESULT hr = m_pkDevice->CreateRasterizerState(&m_kCurrentRasterizerDesc, 
        &pkRState);

    if (FAILED(hr) || pkRState == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_RASTERIZER_STATE_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_RASTERIZER_STATE_CREATION_FAILED,
                "No error message from D3D10, but rasterizer state is "
                "NULL.");
        }
    }
    else
    {
        m_pkDeviceState->RSSetState(pkRState);
    }

    if (pkRState)
        pkRState->Release();
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::ApplyCurrentSamplers()
{
    ApplyCurrentVertexSamplers();
    ApplyCurrentGeometrySamplers();
    ApplyCurrentPixelSamplers();
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::ApplyCurrentVertexSamplers(
    unsigned int uiSamplerStart, unsigned int uiSamplerCount)
{
    if (uiSamplerStart >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        return;

    if (uiSamplerCount > 
        D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiSamplerStart)
    {
        uiSamplerCount = 
            D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiSamplerStart;
    }

    ID3D10SamplerState* apkSamplers[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    memset(apkSamplers, 0, sizeof(apkSamplers));

    D3D10_SAMPLER_DESC* pkIterator = 
        m_aakCurrentSamplerDescs[NiGPUProgram::PROGRAM_VERTEX] + 
        uiSamplerStart;

    unsigned int i = 0;
    for (; i < uiSamplerCount; i++)
    {
        HRESULT hr = m_pkDevice->CreateSamplerState(pkIterator++, 
            &apkSamplers[i]);

        if (FAILED(hr) || apkSamplers[i] == NULL)
        {
            if (FAILED(hr))
            {
                NiD3D10Error::ReportError(
                    NiD3D10Error::NID3D10ERROR_SAMPLER_CREATION_FAILED,
                    "Error HRESULT = 0x%08X.", (unsigned int)hr);
            }
            else
            {
                NiD3D10Error::ReportError(
                    NiD3D10Error::NID3D10ERROR_SAMPLER_CREATION_FAILED,
                    "No error message from D3D10, but sampler is NULL.");
            }
        }
    }

    m_pkDeviceState->VSSetSamplers(uiSamplerStart, uiSamplerCount, 
        apkSamplers);

    for (i = 0; i < uiSamplerCount; i++)
    {
        if (apkSamplers[i])
            apkSamplers[i]->Release();
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::ApplyCurrentGeometrySamplers(
    unsigned int uiSamplerStart, unsigned int uiSamplerCount)
{
    if (uiSamplerStart >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        return;

    if (uiSamplerCount > 
        D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiSamplerStart)
    {
        uiSamplerCount = 
            D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiSamplerStart;
    }

    ID3D10SamplerState* apkSamplers[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    memset(apkSamplers, 0, sizeof(apkSamplers));

    D3D10_SAMPLER_DESC* pkIterator = 
        m_aakCurrentSamplerDescs[NiGPUProgram::PROGRAM_GEOMETRY] + 
        uiSamplerStart;

    unsigned int i = 0;
    for (; i < uiSamplerCount; i++)
    {
        HRESULT hr = m_pkDevice->CreateSamplerState(pkIterator++, 
            &apkSamplers[i]);

        if (FAILED(hr) || apkSamplers[i] == NULL)
        {
            if (FAILED(hr))
            {
                NiD3D10Error::ReportError(
                    NiD3D10Error::NID3D10ERROR_SAMPLER_CREATION_FAILED,
                    "Error HRESULT = 0x%08X.", (unsigned int)hr);
            }
            else
            {
                NiD3D10Error::ReportError(
                    NiD3D10Error::NID3D10ERROR_SAMPLER_CREATION_FAILED,
                    "No error message from D3D10, but sampler is NULL.");
            }
        }
    }

    m_pkDeviceState->GSSetSamplers(uiSamplerStart, uiSamplerCount, 
        apkSamplers);

    for (i = 0; i < uiSamplerCount; i++)
    {
        if (apkSamplers[i])
            apkSamplers[i]->Release();
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::ApplyCurrentPixelSamplers(
    unsigned int uiSamplerStart, unsigned int uiSamplerCount)
{
    if (uiSamplerStart >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        return;

    if (uiSamplerCount > 
        D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiSamplerStart)
    {
        uiSamplerCount = 
            D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiSamplerStart;
    }

    ID3D10SamplerState* apkSamplers[D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];
    memset(apkSamplers, 0, sizeof(apkSamplers));

    D3D10_SAMPLER_DESC* pkIterator = 
        m_aakCurrentSamplerDescs[NiGPUProgram::PROGRAM_PIXEL] + 
        uiSamplerStart;

    unsigned int i = 0;
    for (; i < uiSamplerCount; i++)
    {
        HRESULT hr = m_pkDevice->CreateSamplerState(pkIterator++, 
            &apkSamplers[i]);

        if (FAILED(hr) || apkSamplers[i] == NULL)
        {
            if (FAILED(hr))
            {
                NiD3D10Error::ReportError(
                    NiD3D10Error::NID3D10ERROR_SAMPLER_CREATION_FAILED,
                    "Error HRESULT = 0x%08X.", (unsigned int)hr);
            }
            else
            {
                NiD3D10Error::ReportError(
                    NiD3D10Error::NID3D10ERROR_SAMPLER_CREATION_FAILED,
                    "No error message from D3D10, but sampler is NULL.");
            }
        }
    }

    m_pkDeviceState->PSSetSamplers(uiSamplerStart, uiSamplerCount, 
        apkSamplers);

    for (i = 0; i < uiSamplerCount; i++)
    {
        if (apkSamplers[i])
            apkSamplers[i]->Release();
    }
}
//---------------------------------------------------------------------------
void NiD3D10RenderStateManager::ApplyCurrentSamplers(
    NiGPUProgram::ProgramType eType, unsigned int uiSamplerStart, 
    unsigned int uiSamplerCount)
{
    if (eType == NiGPUProgram::PROGRAM_VERTEX)
        ApplyCurrentVertexSamplers(uiSamplerStart, uiSamplerCount);
    else if (eType == NiGPUProgram::PROGRAM_GEOMETRY)
        ApplyCurrentGeometrySamplers(uiSamplerStart, uiSamplerCount);
    else if (eType == NiGPUProgram::PROGRAM_PIXEL)
        ApplyCurrentPixelSamplers(uiSamplerStart, uiSamplerCount);
}
//---------------------------------------------------------------------------
D3D10_BLEND NiD3D10RenderStateManager::ConvertGbBlendToD3D10Blend(
    NiAlphaProperty::AlphaFunction eFunction)
{
    switch (eFunction)
    {
    case NiAlphaProperty::ALPHA_ONE:
        return D3D10_BLEND_ONE;
    case NiAlphaProperty::ALPHA_ZERO:
        return D3D10_BLEND_ZERO;
    case NiAlphaProperty::ALPHA_SRCCOLOR:
        return D3D10_BLEND_SRC_COLOR;
    case NiAlphaProperty::ALPHA_INVSRCCOLOR:
        return D3D10_BLEND_INV_SRC_COLOR;
    case NiAlphaProperty::ALPHA_DESTCOLOR:
        return D3D10_BLEND_DEST_COLOR;
    case NiAlphaProperty::ALPHA_INVDESTCOLOR:
        return D3D10_BLEND_INV_DEST_COLOR;
    case NiAlphaProperty::ALPHA_SRCALPHA:
        return D3D10_BLEND_SRC_ALPHA;
    case NiAlphaProperty::ALPHA_INVSRCALPHA:
        return D3D10_BLEND_INV_SRC_ALPHA;
    case NiAlphaProperty::ALPHA_DESTALPHA:
        return D3D10_BLEND_DEST_ALPHA;
    case NiAlphaProperty::ALPHA_INVDESTALPHA:
        return D3D10_BLEND_INV_DEST_ALPHA;
    case NiAlphaProperty::ALPHA_SRCALPHASAT:
        return D3D10_BLEND_SRC_ALPHA_SAT;
    }
    NiD3D10Error::ReportWarning("Invalid alpha function passed into "
        "NiD3D10RenderStateManager::ConvertGbBlendToD3D10Blend");
    return D3D10_BLEND_ZERO;
}
//---------------------------------------------------------------------------
D3D10_COMPARISON_FUNC 
    NiD3D10RenderStateManager::ConvertGbStencilFuncToD3D10Comparison(
    NiStencilProperty::TestFunc eFunction)
{
    switch (eFunction)
    {
    case NiStencilProperty::TEST_NEVER:
        return D3D10_COMPARISON_NEVER;
    case NiStencilProperty::TEST_LESS:
        return D3D10_COMPARISON_LESS;
    case NiStencilProperty::TEST_EQUAL:
        return D3D10_COMPARISON_EQUAL;
    case NiStencilProperty::TEST_LESSEQUAL:
        return D3D10_COMPARISON_LESS_EQUAL;
    case NiStencilProperty::TEST_GREATER:
        return D3D10_COMPARISON_GREATER;
    case NiStencilProperty::TEST_NOTEQUAL:
        return D3D10_COMPARISON_NOT_EQUAL;
    case NiStencilProperty::TEST_GREATEREQUAL:
        return D3D10_COMPARISON_GREATER_EQUAL;
    case NiStencilProperty::TEST_ALWAYS:
        return D3D10_COMPARISON_ALWAYS;
    }
    NiD3D10Error::ReportWarning("Invalid stencil function passed into "
        "NiD3D10RenderStateManager::ConvertGbStencilFuncToD3D10Comparison");
    return D3D10_COMPARISON_NEVER;
}
//---------------------------------------------------------------------------
D3D10_STENCIL_OP 
    NiD3D10RenderStateManager::ConvertGbStencilActionToD3D10StencilOp(
    NiStencilProperty::Action eAction)
{
    switch (eAction)
    {
    case NiStencilProperty::ACTION_KEEP:
        return D3D10_STENCIL_OP_KEEP;
    case NiStencilProperty::ACTION_ZERO:
        return D3D10_STENCIL_OP_ZERO;
    case NiStencilProperty::ACTION_REPLACE:
        return D3D10_STENCIL_OP_REPLACE;
    case NiStencilProperty::ACTION_INCREMENT:
        return D3D10_STENCIL_OP_INCR_SAT;
    case NiStencilProperty::ACTION_DECREMENT:
        return D3D10_STENCIL_OP_DECR_SAT;
    case NiStencilProperty::ACTION_INVERT:
        return D3D10_STENCIL_OP_INVERT;
    }
    NiD3D10Error::ReportWarning("Invalid stencil action passed into "
        "NiD3D10RenderStateManager::ConvertGbStencilActionToD3D10StencilOp");
    return D3D10_STENCIL_OP_KEEP;
}
//---------------------------------------------------------------------------
D3D10_COMPARISON_FUNC 
    NiD3D10RenderStateManager::ConvertGbDepthFuncToD3D10Comparison(
    NiZBufferProperty::TestFunction eFunction)
{
    switch (eFunction)
    {
    case NiZBufferProperty::TEST_ALWAYS:
        return D3D10_COMPARISON_ALWAYS;
    case NiZBufferProperty::TEST_LESS:
        return D3D10_COMPARISON_LESS;
    case NiZBufferProperty::TEST_EQUAL:
        return D3D10_COMPARISON_EQUAL;
    case NiZBufferProperty::TEST_LESSEQUAL:
        return D3D10_COMPARISON_LESS_EQUAL;
    case NiZBufferProperty::TEST_GREATER:
        return D3D10_COMPARISON_GREATER;
    case NiZBufferProperty::TEST_NOTEQUAL:
        return D3D10_COMPARISON_NOT_EQUAL;
    case NiZBufferProperty::TEST_GREATEREQUAL:
        return D3D10_COMPARISON_GREATER_EQUAL;
    case NiZBufferProperty::TEST_NEVER:
        return D3D10_COMPARISON_NEVER;
    }
    NiD3D10Error::ReportWarning("Invalid Z buffer test function passed into "
        "NiD3D10RenderStateManager::ConvertGbDepthFuncToD3D10Comparison");
    return D3D10_COMPARISON_NEVER;
}
//---------------------------------------------------------------------------
D3D10_FILTER NiD3D10RenderStateManager::ConvertGbFilterModeToD3D10Filter(
    NiTexturingProperty::FilterMode eFilterMode)
{
    switch (eFilterMode)
    {
    case NiTexturingProperty::FILTER_NEAREST:
    case NiTexturingProperty::FILTER_NEAREST_MIPNEAREST:
        return D3D10_FILTER_MIN_MAG_MIP_POINT;
    case NiTexturingProperty::FILTER_BILERP:
    case NiTexturingProperty::FILTER_BILERP_MIPNEAREST:
        return D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    case NiTexturingProperty::FILTER_TRILERP:
        return D3D10_FILTER_MIN_MAG_MIP_LINEAR;
    case NiTexturingProperty::FILTER_NEAREST_MIPLERP:
        return D3D10_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    }
    NiD3D10Error::ReportWarning("Invalid filter mode passed into "
        "NiD3D10RenderStateManager::ConvertGbFilterModeToD3D10Filter");
    return D3D10_FILTER_MIN_MAG_MIP_POINT;
}
//---------------------------------------------------------------------------
bool NiD3D10RenderStateManager::ConvertGbFilterModeToMipmapEnable(
    NiTexturingProperty::FilterMode eFilterMode)
{
    switch (eFilterMode)
    {
    case NiTexturingProperty::FILTER_NEAREST:
    case NiTexturingProperty::FILTER_BILERP:
        return false;
    case NiTexturingProperty::FILTER_NEAREST_MIPNEAREST:
    case NiTexturingProperty::FILTER_BILERP_MIPNEAREST:
    case NiTexturingProperty::FILTER_TRILERP:
    case NiTexturingProperty::FILTER_NEAREST_MIPLERP:
        return true;
    }
    NiD3D10Error::ReportWarning("Invalid filter mode passed into "
        "NiD3D10RenderStateManager::ConvertGbFilterModeToMipmapEnable");
    return true;
}
//---------------------------------------------------------------------------
D3D10_TEXTURE_ADDRESS_MODE 
    NiD3D10RenderStateManager::ConvertGbClampModeToD3D10AddressU(
    NiTexturingProperty::ClampMode eClampMode)
{
    switch (eClampMode)
    {
    case NiTexturingProperty::CLAMP_S_CLAMP_T:
    case NiTexturingProperty::CLAMP_S_WRAP_T:
        return D3D10_TEXTURE_ADDRESS_CLAMP;
    case NiTexturingProperty::WRAP_S_CLAMP_T:
    case NiTexturingProperty::WRAP_S_WRAP_T:
        return D3D10_TEXTURE_ADDRESS_WRAP;
    }
    NiD3D10Error::ReportWarning("Invalid address mode passed into "
        "NiD3D10RenderStateManager::ConvertGbClampModeToD3D10AddressU");
    return D3D10_TEXTURE_ADDRESS_CLAMP;
}
//---------------------------------------------------------------------------
D3D10_TEXTURE_ADDRESS_MODE 
    NiD3D10RenderStateManager::ConvertGbClampModeToD3D10AddressV(
    NiTexturingProperty::ClampMode eClampMode)
{
    switch (eClampMode)
    {
    case NiTexturingProperty::CLAMP_S_CLAMP_T:
    case NiTexturingProperty::WRAP_S_CLAMP_T:
        return D3D10_TEXTURE_ADDRESS_CLAMP;
    case NiTexturingProperty::CLAMP_S_WRAP_T:
    case NiTexturingProperty::WRAP_S_WRAP_T:
        return D3D10_TEXTURE_ADDRESS_WRAP;
    }
    NiD3D10Error::ReportWarning("Invalid address mode passed into "
        "NiD3D10RenderStateManager::ConvertGbClampModeToD3D10AddressV");
    return D3D10_TEXTURE_ADDRESS_CLAMP;
}
//---------------------------------------------------------------------------
