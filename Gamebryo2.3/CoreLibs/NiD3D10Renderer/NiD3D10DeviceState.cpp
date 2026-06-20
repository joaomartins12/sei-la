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

#include "NiD3D10DeviceState.h"

#include <NiRTLib.h>

//---------------------------------------------------------------------------
NiD3D10DeviceState::NiD3D10DeviceState(ID3D10Device* pkDevice) :
    m_pkDevice(pkDevice),
    m_pkBlendState(NULL),
    m_pkDepthStencilState(NULL),
    m_pkRasterizerState(NULL),
    m_pkVertexShader(NULL),
    m_pkGeometryShader(NULL),
    m_pkPixelShader(NULL),
    m_uiSampleMask(UINT_MAX),
    m_uiStencilRef(0)
{
    assert (m_pkDevice);
    if (m_pkDevice)
        m_pkDevice->AddRef();

    memset(m_apkVertexSamplers, 0, sizeof(m_apkVertexSamplers));
    memset(m_apkGeometrySamplers, 0, sizeof(m_apkGeometrySamplers));
    memset(m_apkPixelSamplers, 0, sizeof(m_apkPixelSamplers));

    memset(m_afBlendFactor, 0, sizeof(m_afBlendFactor));

    memset(m_apkVertexTextures, 0, sizeof(m_apkVertexTextures));
    memset(m_apkGeometryTextures, 0, sizeof(m_apkGeometryTextures));
    memset(m_apkPixelTextures, 0, sizeof(m_apkPixelTextures));

    memset(m_apkVertexConstantBuffers, 0, sizeof(m_apkVertexConstantBuffers));
    memset(m_apkGeometryConstantBuffers, 0, 
        sizeof(m_apkGeometryConstantBuffers));
    memset(m_apkPixelConstantBuffers, 0, sizeof(m_apkPixelConstantBuffers));
}
//---------------------------------------------------------------------------
NiD3D10DeviceState::~NiD3D10DeviceState()
{
    if (m_pkDevice)
        m_pkDevice->Release();

    if (m_pkBlendState)
        m_pkBlendState->Release();

    if (m_pkDepthStencilState)
        m_pkDepthStencilState->Release();

    if (m_pkRasterizerState)
        m_pkRasterizerState->Release();

    unsigned int i = 0;
    for (; i < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; i++)
    {
        if (m_apkVertexSamplers[i])
            m_apkVertexSamplers[i]->Release();
        if (m_apkGeometrySamplers[i])
            m_apkGeometrySamplers[i]->Release();
        if (m_apkPixelSamplers[i])
            m_apkPixelSamplers[i]->Release();

        if (m_apkVertexTextures[i])
            m_apkVertexTextures[i]->Release();
        if (m_apkGeometryTextures[i])
            m_apkGeometryTextures[i]->Release();
        if (m_apkPixelTextures[i])
            m_apkPixelTextures[i]->Release();
    }

    for (; i < D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT; i++)
    {
        if (m_apkVertexConstantBuffers[i])
            m_apkVertexConstantBuffers[i]->Release();
        if (m_apkGeometryConstantBuffers[i])
            m_apkGeometryConstantBuffers[i]->Release();
        if (m_apkPixelConstantBuffers[i])
            m_apkPixelConstantBuffers[i]->Release();
    }

    if (m_pkVertexShader)
        m_pkVertexShader->Release();

    if (m_pkGeometryShader)
        m_pkGeometryShader->Release();

    if (m_pkPixelShader)
        m_pkPixelShader->Release();
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::OMSetBlendState(ID3D10BlendState* pkBlendState, 
    const float afBlendFactor[4], unsigned int uiSampleMask)
{
    m_pkDevice->OMSetBlendState(pkBlendState, afBlendFactor, uiSampleMask);

    if (m_pkBlendState != pkBlendState)
    {
        if (m_pkBlendState)
            m_pkBlendState->Release();
        m_pkBlendState = pkBlendState;
        if (m_pkBlendState)
            m_pkBlendState->AddRef();
    }

    m_afBlendFactor[0] = afBlendFactor[0];
    m_afBlendFactor[1] = afBlendFactor[1];
    m_afBlendFactor[2] = afBlendFactor[2];
    m_afBlendFactor[3] = afBlendFactor[3];

    m_uiSampleMask = uiSampleMask;
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::OMGetBlendState(ID3D10BlendState** ppkBlendState, 
    float afBlendFactor[4], unsigned int* puiSampleMask) const
{
    if (ppkBlendState)
        *ppkBlendState = m_pkBlendState;

    afBlendFactor[0] = m_afBlendFactor[0];
    afBlendFactor[1] = m_afBlendFactor[1];
    afBlendFactor[2] = m_afBlendFactor[2];
    afBlendFactor[3] = m_afBlendFactor[3];

    if (puiSampleMask)
        *puiSampleMask = m_uiSampleMask;
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::OMClearBlendState()
{
    if (m_pkBlendState)
    {
        m_pkBlendState->Release();
        m_pkBlendState = NULL;
    }

    memset(m_afBlendFactor, 0, sizeof(m_afBlendFactor));
    m_uiSampleMask = UINT_MAX;

    m_pkDevice->OMSetBlendState(NULL, m_afBlendFactor, m_uiSampleMask);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::OMSetDepthStencilState(
    ID3D10DepthStencilState* pkDepthStencilState, unsigned int uiStencilRef)
{
    m_pkDevice->OMSetDepthStencilState(pkDepthStencilState, uiStencilRef);

    if (m_pkDepthStencilState != pkDepthStencilState)
    {
        if (m_pkDepthStencilState)
            m_pkDepthStencilState->Release();
        m_pkDepthStencilState = pkDepthStencilState;
        if (m_pkDepthStencilState)
            m_pkDepthStencilState->AddRef();
    }

    m_uiStencilRef = uiStencilRef;
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::OMGetDepthStencilState(
    ID3D10DepthStencilState** ppkDepthStencilState, 
    unsigned int* puiStencilRef) const
{
    if (ppkDepthStencilState)
        *ppkDepthStencilState = m_pkDepthStencilState;
    if (puiStencilRef)
        *puiStencilRef = m_uiStencilRef;
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::OMClearDepthStencilState()
{
    if (m_pkDepthStencilState)
    {
        m_pkDepthStencilState->Release();
        m_pkDepthStencilState = NULL;
    }

    m_uiStencilRef = 0;

    m_pkDevice->OMSetDepthStencilState(NULL, m_uiStencilRef);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::RSSetState(ID3D10RasterizerState* pkRasterizerState)
{
    m_pkDevice->RSSetState(pkRasterizerState);

    if (m_pkRasterizerState != pkRasterizerState)
    {
        if (m_pkRasterizerState)
            m_pkRasterizerState->Release();
        m_pkRasterizerState = pkRasterizerState;
        if (m_pkRasterizerState)
            m_pkRasterizerState->AddRef();
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::RSGetState(ID3D10RasterizerState** ppkRasterizerState)
    const
{
    if (ppkRasterizerState)
        *ppkRasterizerState = m_pkRasterizerState;
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::RSClearState()
{
    if (m_pkRasterizerState)
    {
        m_pkRasterizerState->Release();
        m_pkRasterizerState = NULL;
    }

    m_pkDevice->RSSetState(NULL);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::VSSetSamplers(unsigned int uiStartSlot, 
    unsigned int uiNumSamplers, ID3D10SamplerState*const* ppkSamplers)
{
    if (uiStartSlot >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT ||
        ppkSamplers == NULL)
    {
        return;
    }
    if (uiNumSamplers > D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot)
        uiNumSamplers = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot;

    m_pkDevice->VSSetSamplers(uiStartSlot, uiNumSamplers, ppkSamplers);

    ID3D10SamplerState** ppkIterator = m_apkVertexSamplers + uiStartSlot;

    for (unsigned int i = 0; i < uiNumSamplers; i++)
    {
        NIASSERT(i + uiStartSlot < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);
        if (*ppkIterator != ppkSamplers[i])
        {
            if (*ppkIterator)
                (*ppkIterator)->Release();
            *ppkIterator = ppkSamplers[i];
            if (*ppkIterator)
                (*ppkIterator)->AddRef();
        }
        ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::VSGetSamplers(unsigned int uiStartSlot, 
    unsigned int uiNumSamplers, ID3D10SamplerState** ppkSamplers) const
{
    if (uiStartSlot >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT ||
        ppkSamplers == NULL)
    {
        return;
    }
    if (uiNumSamplers > D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot)
        uiNumSamplers = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot;

    ID3D10SamplerState*const* ppkIterator = m_apkVertexSamplers + uiStartSlot;

    for (unsigned int i = 0; i < uiNumSamplers; i++)
    {
        NIASSERT(i + uiStartSlot < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);
        ppkSamplers[i] = *ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::VSClearSamplers()
{
    for (unsigned int i = 0; i < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; i++)
    {
        if (m_apkVertexSamplers[i])
        {
            m_apkVertexSamplers[i]->Release();
            m_apkVertexSamplers[i] = NULL;
        }
    }

    m_pkDevice->VSSetSamplers(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT, 
        m_apkVertexSamplers);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::GSSetSamplers(unsigned int uiStartSlot, 
    unsigned int uiNumSamplers, ID3D10SamplerState*const* ppkSamplers)
{
    if (uiStartSlot >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT ||
        ppkSamplers == NULL)
    {
        return;
    }
    if (uiNumSamplers > D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot)
        uiNumSamplers = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot;

    m_pkDevice->GSSetSamplers(uiStartSlot, uiNumSamplers, ppkSamplers);

    ID3D10SamplerState** ppkIterator = m_apkGeometrySamplers + uiStartSlot;

    for (unsigned int i = 0; i < uiNumSamplers; i++)
    {
        NIASSERT(i + uiStartSlot < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);
        if (*ppkIterator != ppkSamplers[i])
        {
            if (*ppkIterator)
                (*ppkIterator)->Release();
            *ppkIterator = ppkSamplers[i];
            if (*ppkIterator)
                (*ppkIterator)->AddRef();
        }
        ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::GSGetSamplers(unsigned int uiStartSlot, 
    unsigned int uiNumSamplers, ID3D10SamplerState** ppkSamplers) const
{
    if (uiStartSlot >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT ||
        ppkSamplers == NULL)
    {
        return;
    }
    if (uiNumSamplers > D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot)
        uiNumSamplers = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot;

    ID3D10SamplerState*const* ppkIterator = 
        m_apkGeometrySamplers + uiStartSlot;

    for (unsigned int i = 0; i < uiNumSamplers; i++)
    {
        NIASSERT(i + uiStartSlot < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);
        ppkSamplers[i] = *ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::GSClearSamplers()
{
    for (unsigned int i = 0; i < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; i++)
    {
        if (m_apkGeometrySamplers[i])
        {
            m_apkGeometrySamplers[i]->Release();
            m_apkGeometrySamplers[i] = NULL;
        }
    }

    m_pkDevice->GSSetSamplers(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT, 
        m_apkVertexSamplers);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::PSSetSamplers(unsigned int uiStartSlot, 
    unsigned int uiNumSamplers, ID3D10SamplerState*const* ppkSamplers)
{
    if (uiStartSlot >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT ||
        ppkSamplers == NULL)
    {
        return;
    }
    if (uiNumSamplers > D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot)
        uiNumSamplers = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot;

    m_pkDevice->PSSetSamplers(uiStartSlot, uiNumSamplers, ppkSamplers);

    ID3D10SamplerState** ppkIterator = m_apkPixelSamplers + uiStartSlot;

    for (unsigned int i = 0; i < uiNumSamplers; i++)
    {
        NIASSERT(i + uiStartSlot < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);
        if (*ppkIterator != ppkSamplers[i])
        {
            if (*ppkIterator)
                (*ppkIterator)->Release();
            *ppkIterator = ppkSamplers[i];
            if (*ppkIterator)
                (*ppkIterator)->AddRef();
        }
        ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::PSGetSamplers(unsigned int uiStartSlot, 
    unsigned int uiNumSamplers, ID3D10SamplerState** ppkSamplers) const
{
    if (uiStartSlot >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT ||
        ppkSamplers == NULL)
    {
        return;
    }
    if (uiNumSamplers > D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot)
        uiNumSamplers = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot;

    ID3D10SamplerState*const* ppkIterator = m_apkPixelSamplers + uiStartSlot;

    for (unsigned int i = 0; i < uiNumSamplers; i++)
    {
        NIASSERT(i + uiStartSlot < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);
        ppkSamplers[i] = *ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::PSClearSamplers()
{
    for (unsigned int i = 0; i < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; i++)
    {
        if (m_apkPixelSamplers[i])
        {
            m_apkPixelSamplers[i]->Release();
            m_apkPixelSamplers[i] = NULL;
        }
    }

    m_pkDevice->PSSetSamplers(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT, 
        m_apkVertexSamplers);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::VSSetShaderResources(unsigned int uiStartSlot, 
    unsigned int uiNumViews, ID3D10ShaderResourceView*const* ppkResourceViews)
{
    if (uiStartSlot >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT ||
        ppkResourceViews == NULL)
    {
        return;
    }
    if (uiNumViews > D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot)
        uiNumViews = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot;

    m_pkDevice->VSSetShaderResources(uiStartSlot, uiNumViews, 
        ppkResourceViews);

    ID3D10ShaderResourceView** ppkIterator = m_apkVertexTextures + uiStartSlot;

    for (unsigned int i = 0; i < uiNumViews; i++)
    {
        NIASSERT(i + uiStartSlot < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);
        if (*ppkIterator != ppkResourceViews[i])
        {
            if (*ppkIterator)
                (*ppkIterator)->Release();
            *ppkIterator = ppkResourceViews[i];
            if (*ppkIterator)
                (*ppkIterator)->AddRef();
        }
        ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::VSGetShaderResources(unsigned int uiStartSlot, 
    unsigned int uiNumViews, ID3D10ShaderResourceView** ppkResourceViews) const
{
    if (uiStartSlot >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT ||
        ppkResourceViews == NULL)
    {
        return;
    }
    if (uiNumViews > D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot)
        uiNumViews = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot;

    ID3D10ShaderResourceView*const* ppkIterator = 
        m_apkVertexTextures + uiStartSlot;

    for (unsigned int i = 0; i < uiNumViews; i++)
    {
        NIASSERT(i + uiStartSlot < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);
        ppkResourceViews[i] = *ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::VSClearShaderResources()
{
    for (unsigned int i = 0; i < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; i++)
    {
        if (m_apkVertexTextures[i])
        {
            m_apkVertexTextures[i]->Release();
            m_apkVertexTextures[i] = NULL;
        }
    }

    m_pkDevice->VSSetShaderResources(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT, 
        m_apkVertexTextures);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::GSSetShaderResources(unsigned int uiStartSlot, 
    unsigned int uiNumViews, ID3D10ShaderResourceView*const* ppkResourceViews)
{
    if (uiStartSlot >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT ||
        ppkResourceViews == NULL)
    {
        return;
    }
    if (uiNumViews > D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot)
        uiNumViews = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot;

    m_pkDevice->GSSetShaderResources(uiStartSlot, uiNumViews, 
        ppkResourceViews);

    ID3D10ShaderResourceView** ppkIterator = 
        m_apkGeometryTextures + uiStartSlot;

    for (unsigned int i = 0; i < uiNumViews; i++)
    {
        NIASSERT(i + uiStartSlot < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);
        if (*ppkIterator != ppkResourceViews[i])
        {
            if (*ppkIterator)
                (*ppkIterator)->Release();
            *ppkIterator = ppkResourceViews[i];
            if (*ppkIterator)
                (*ppkIterator)->AddRef();
        }
        ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::GSGetShaderResources(unsigned int uiStartSlot, 
    unsigned int uiNumViews, ID3D10ShaderResourceView** ppkResourceViews) const
{
    if (uiStartSlot >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT ||
        ppkResourceViews == NULL)
    {
        return;
    }
    if (uiNumViews > D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot)
        uiNumViews = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot;

    ID3D10ShaderResourceView*const* ppkIterator = 
        m_apkGeometryTextures + uiStartSlot;

    for (unsigned int i = 0; i < uiNumViews; i++)
    {
        NIASSERT(i + uiStartSlot < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);
        ppkResourceViews[i] = *ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::GSClearShaderResources()
{
    for (unsigned int i = 0; i < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; i++)
    {
        if (m_apkGeometryTextures[i])
        {
            m_apkGeometryTextures[i]->Release();
            m_apkGeometryTextures[i] = NULL;
        }
    }

    m_pkDevice->GSSetShaderResources(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT, 
        m_apkVertexTextures);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::PSSetShaderResources(unsigned int uiStartSlot, 
    unsigned int uiNumViews, ID3D10ShaderResourceView*const* ppkResourceViews)
{
    if (uiStartSlot >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT ||
        ppkResourceViews == NULL)
    {
        return;
    }
    if (uiNumViews > D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot)
        uiNumViews = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot;

    m_pkDevice->PSSetShaderResources(uiStartSlot, uiNumViews, 
        ppkResourceViews);

    ID3D10ShaderResourceView** ppkIterator = m_apkPixelTextures + uiStartSlot;

    for (unsigned int i = 0; i < uiNumViews; i++)
    {
        NIASSERT(i + uiStartSlot < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);
        if (*ppkIterator != ppkResourceViews[i])
        {
            if (*ppkIterator)
                (*ppkIterator)->Release();
            *ppkIterator = ppkResourceViews[i];
            if (*ppkIterator)
                (*ppkIterator)->AddRef();
        }
        ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::PSGetShaderResources(unsigned int uiStartSlot, 
    unsigned int uiNumViews, ID3D10ShaderResourceView** ppkResourceViews) const
{
    if (uiStartSlot >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT ||
        ppkResourceViews == NULL)
    {
        return;
    }
    if (uiNumViews > D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot)
        uiNumViews = D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT - uiStartSlot;

    ID3D10ShaderResourceView*const* ppkIterator = 
        m_apkPixelTextures + uiStartSlot;

    for (unsigned int i = 0; i < uiNumViews; i++)
    {
        NIASSERT(i + uiStartSlot < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);
        ppkResourceViews[i] = *ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::PSClearShaderResources()
{
    for (unsigned int i = 0; i < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; i++)
    {
        if (m_apkPixelTextures[i])
        {
            m_apkPixelTextures[i]->Release();
            m_apkPixelTextures[i] = NULL;
        }
    }

    m_pkDevice->PSSetShaderResources(0, D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT, 
        m_apkVertexTextures);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::VSSetConstantBuffers(unsigned int uiStartSlot, 
    unsigned int uiNumBuffers, ID3D10Buffer*const* ppkConstantBuffers)
{
    if (uiStartSlot >= D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT ||
        ppkConstantBuffers == NULL)
    {
        return;
    }
    if (uiNumBuffers > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot)
    {
        uiNumBuffers = 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot;
    }

    unsigned int uiTempNumBuffers = uiNumBuffers;
    while (uiTempNumBuffers > 0)
    {
        if (ppkConstantBuffers[uiTempNumBuffers - 1] != NULL)
            break;
        uiTempNumBuffers--;
    }

    if (uiTempNumBuffers != 0)
    {
        m_pkDevice->VSSetConstantBuffers(uiStartSlot, uiTempNumBuffers, 
            ppkConstantBuffers);
    }

    ID3D10Buffer** ppkIterator = m_apkVertexConstantBuffers + uiStartSlot;

    for (unsigned int i = 0; i < uiNumBuffers; i++)
    {
        NIASSERT(i + uiStartSlot < 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT);
        if (*ppkIterator != ppkConstantBuffers[i])
        {
            if (*ppkIterator)
                (*ppkIterator)->Release();
            *ppkIterator = ppkConstantBuffers[i];
            if (*ppkIterator)
                (*ppkIterator)->AddRef();
        }
        ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::VSGetConstantBuffers(unsigned int uiStartSlot, 
    unsigned int uiNumBuffers, ID3D10Buffer** ppkConstantBuffers) const
{
    if (uiStartSlot >= D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT ||
        ppkConstantBuffers == NULL)
    {
        return;
    }
    if (uiNumBuffers > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot)
    {
        uiNumBuffers = 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot;
    }

    ID3D10Buffer*const* ppkIterator = m_apkVertexConstantBuffers + uiStartSlot;

    for (unsigned int i = 0; i < uiNumBuffers; i++)
    {
        NIASSERT(i + uiStartSlot < 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT);
        ppkConstantBuffers[i] = *ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::VSClearConstantBuffers()
{
    for (unsigned int i = 0; 
        i < D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT; i++)
    {
        if (m_apkVertexConstantBuffers[i])
        {
            m_apkVertexConstantBuffers[i]->Release();
            m_apkVertexConstantBuffers[i] = NULL;
        }
    }

    m_pkDevice->VSSetConstantBuffers(0, 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT, 
        m_apkVertexConstantBuffers);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::GSSetConstantBuffers(unsigned int uiStartSlot, 
    unsigned int uiNumBuffers, ID3D10Buffer*const* ppkConstantBuffers)
{
    if (uiStartSlot >= D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT ||
        ppkConstantBuffers == NULL)
    {
        return;
    }
    if (uiNumBuffers > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot)
    {
        uiNumBuffers = 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot;
    }

    unsigned int uiTempNumBuffers = uiNumBuffers;
    while (uiTempNumBuffers > 0)
    {
        if (ppkConstantBuffers[uiTempNumBuffers - 1] != NULL)
            break;
        uiTempNumBuffers--;
    }
    if (uiTempNumBuffers > 0)
    {
        m_pkDevice->GSSetConstantBuffers(uiStartSlot, uiTempNumBuffers, 
            ppkConstantBuffers);
    }

    ID3D10Buffer** ppkIterator = m_apkGeometryConstantBuffers + uiStartSlot;

    for (unsigned int i = 0; i < uiNumBuffers; i++)
    {
        NIASSERT(i + uiStartSlot < 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT);
        if (*ppkIterator != ppkConstantBuffers[i])
        {
            if (*ppkIterator)
                (*ppkIterator)->Release();
            *ppkIterator = ppkConstantBuffers[i];
            if (*ppkIterator)
                (*ppkIterator)->AddRef();
        }
        ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::GSGetConstantBuffers(unsigned int uiStartSlot, 
    unsigned int uiNumBuffers, ID3D10Buffer** ppkConstantBuffers) const
{
    if (uiStartSlot >= D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT ||
        ppkConstantBuffers == NULL)
    {
        return;
    }
    if (uiNumBuffers > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot)
    {
        uiNumBuffers = 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot;
    }

    ID3D10Buffer*const* ppkIterator = 
        m_apkGeometryConstantBuffers + uiStartSlot;

    for (unsigned int i = 0; i < uiNumBuffers; i++)
    {
        NIASSERT(i + uiStartSlot < 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT);
        ppkConstantBuffers[i] = *ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::GSClearConstantBuffers()
{
    for (unsigned int i = 0; 
        i < D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT; i++)
    {
        if (m_apkGeometryConstantBuffers[i])
        {
            m_apkGeometryConstantBuffers[i]->Release();
            m_apkGeometryConstantBuffers[i] = NULL;
        }
    }

    m_pkDevice->GSSetConstantBuffers(0, 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT, 
        m_apkVertexConstantBuffers);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::PSSetConstantBuffers(unsigned int uiStartSlot, 
    unsigned int uiNumBuffers, ID3D10Buffer*const* ppkConstantBuffers)
{
    if (uiStartSlot >= D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT ||
        ppkConstantBuffers == NULL)
    {
        return;
    }
    if (uiNumBuffers > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot)
    {
        uiNumBuffers = 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot;
    }

    unsigned int uiTempNumBuffers = uiNumBuffers;
    while (uiTempNumBuffers > 0)
    {
        if (ppkConstantBuffers[uiTempNumBuffers - 1] != NULL)
            break;
        uiTempNumBuffers--;
    }

    if (uiTempNumBuffers)
    {
        m_pkDevice->PSSetConstantBuffers(uiStartSlot, uiTempNumBuffers, 
            ppkConstantBuffers);
    }

    ID3D10Buffer** ppkIterator = m_apkPixelConstantBuffers + uiStartSlot;

    for (unsigned int i = 0; i < uiNumBuffers; i++)
    {
        NIASSERT(i + uiStartSlot < 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT);
        if (*ppkIterator != ppkConstantBuffers[i])
        {
            if (*ppkIterator)
                (*ppkIterator)->Release();
            *ppkIterator = ppkConstantBuffers[i];
            if (*ppkIterator)
                (*ppkIterator)->AddRef();
        }
        ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::PSGetConstantBuffers(unsigned int uiStartSlot, 
    unsigned int uiNumBuffers, ID3D10Buffer** ppkConstantBuffers) const
{
    if (uiStartSlot >= D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT ||
        ppkConstantBuffers == NULL)
    {
        return;
    }
    if (uiNumBuffers > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot)
    {
        uiNumBuffers = 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot;
    }

    ID3D10Buffer*const* ppkIterator = m_apkPixelConstantBuffers + uiStartSlot;

    for (unsigned int i = 0; i < uiNumBuffers; i++)
    {
        NIASSERT(i + uiStartSlot < 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT);
        ppkConstantBuffers[i] = *ppkIterator++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::PSClearConstantBuffers()
{
    for (unsigned int i = 0; 
        i < D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT; i++)
    {
        if (m_apkPixelConstantBuffers[i])
        {
            m_apkPixelConstantBuffers[i]->Release();
            m_apkPixelConstantBuffers[i] = NULL;
        }
    }

    m_pkDevice->PSSetConstantBuffers(0, 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT, 
        m_apkVertexConstantBuffers);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::VSSetShader(ID3D10VertexShader* pkVertexShader)
{
    if (m_pkVertexShader != pkVertexShader)
    {
        if (m_pkVertexShader)
            m_pkVertexShader->Release();
        m_pkVertexShader = pkVertexShader;
        if (pkVertexShader)
            pkVertexShader->AddRef();
    }

    m_pkDevice->VSSetShader(pkVertexShader);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::VSGetShader(ID3D10VertexShader** ppkVertexShader)
    const
{
    if (ppkVertexShader)
        *ppkVertexShader = m_pkVertexShader;
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::VSClearShader()
{
    if (m_pkVertexShader)
    {
        m_pkVertexShader->Release();
        m_pkVertexShader = NULL;
    }

    m_pkDevice->VSSetShader(NULL);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::GSSetShader(ID3D10GeometryShader* pkGeometryShader)
{
    if (m_pkGeometryShader != pkGeometryShader)
    {
        if (m_pkGeometryShader)
            m_pkGeometryShader->Release();
        m_pkGeometryShader = pkGeometryShader;
        if (pkGeometryShader)
            pkGeometryShader->AddRef();
    }

    m_pkDevice->GSSetShader(pkGeometryShader);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::GSGetShader(ID3D10GeometryShader** ppkGeometryShader)
    const
{
    if (ppkGeometryShader)
        *ppkGeometryShader = m_pkGeometryShader;
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::GSClearShader()
{
    if (m_pkGeometryShader)
    {
        m_pkGeometryShader->Release();
        m_pkGeometryShader = NULL;
    }

    m_pkDevice->GSSetShader(NULL);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::PSSetShader(ID3D10PixelShader* pkPixelShader)
{
    if (m_pkPixelShader != pkPixelShader)
    {
        if (m_pkPixelShader)
            m_pkPixelShader->Release();
        m_pkPixelShader = pkPixelShader;
        if (pkPixelShader)
            pkPixelShader->AddRef();
    }

    m_pkDevice->PSSetShader(pkPixelShader);
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::PSGetShader(ID3D10PixelShader** ppkPixelShader) const
{
    if (ppkPixelShader)
        *ppkPixelShader = m_pkPixelShader;
}
//---------------------------------------------------------------------------
void NiD3D10DeviceState::PSClearShader()
{
    if (m_pkPixelShader)
    {
        m_pkPixelShader->Release();
        m_pkPixelShader = NULL;
    }

    m_pkDevice->PSSetShader(NULL);
}
//---------------------------------------------------------------------------
