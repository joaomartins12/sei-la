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

#include "NiD3D10ShaderInterface.h"

#include "NiD3D10Error.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10VertexDescription.h"

NiImplementRTTI(NiD3D10ShaderInterface, NiShader);

//---------------------------------------------------------------------------
NiD3D10ShaderInterface::NiD3D10ShaderInterface() :
    m_bIsBestImplementation(false)
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    NIASSERT(pkRenderer);
    pkRenderer->RegisterD3D10Shader(this);
}
//---------------------------------------------------------------------------
NiD3D10ShaderInterface::~NiD3D10ShaderInterface()
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer)
        pkRenderer->ReleaseD3D10Shader(this);
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderInterface::Initialize()
{
    return NiShader::Initialize();
}
//---------------------------------------------------------------------------
NiD3D10VertexDescription* NiD3D10ShaderInterface::GetVertexDescription() const
{
    return m_spVertexDescription;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderInterface::SetVertexDescription(
    NiD3D10VertexDescription* pkVertexDesc)
{
    m_spVertexDescription = pkVertexDesc;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderInterface::DestroyRendererData()
{
    /* */
}
//---------------------------------------------------------------------------
void NiD3D10ShaderInterface::RecreateRendererData()
{
    /* */
}
//---------------------------------------------------------------------------
bool NiD3D10ShaderInterface::GetIsBestImplementation() const
{
    return m_bIsBestImplementation;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderInterface::SetIsBestImplementation(bool bIsBest)
{
    m_bIsBestImplementation = bIsBest;
}
//---------------------------------------------------------------------------
