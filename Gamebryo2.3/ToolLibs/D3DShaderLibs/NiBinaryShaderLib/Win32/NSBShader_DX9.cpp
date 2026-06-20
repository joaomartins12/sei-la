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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiBinaryShaderLibPCH.h"

#include "NSBShader.h"
#include "NiBinaryShader.h"
#include "NSBPass.h"
#include "NSBTextureStage.h"
#include <NiD3DRendererHeaders.h>

#if defined(WIN32)
//---------------------------------------------------------------------------
NiBinaryShader* NSBShader::GetD3DShader_DX9(NiD3DRenderer* pkRenderer, 
    unsigned int uiImplementation)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, pkRenderer));
    NSBImplementation* pkImplementation = 0;

    VersionInfo kVersionInfo;
    SetupVersionInfo(pkRenderer, kVersionInfo);

    if (uiImplementation != NiShader::DEFAULT_IMPLEMENTATION)
    {
        // Grab the proper implementation
        pkImplementation = m_kImplementationArray.GetAt(uiImplementation);
        if (!IsImplementationValid(pkImplementation, kVersionInfo))
        {
            NiD3DRenderer::Warning("NSBShader::GetD3DShader - %s - "
                "Requested implementation (%d) invalid on operating "
                "hardware.\n", GetName(), uiImplementation);
            pkImplementation = 0;
        }
    }

    if (!pkImplementation)
    {
        // Get the best for the hardware
        pkImplementation = GetBestImplementation(pkRenderer);
        if (!pkImplementation)
        {
            NiD3DRenderer::Warning("NSBShader::GetD3DShader - %s - Unable "
                "to find valid implementation for hardware.\n",
                GetName());
            return 0;
        }
    }

    NiBinaryShader* pkShader = NiNew NiBinaryShader();
    NIASSERT(pkShader);

    pkShader->SetD3DRenderer(pkRenderer);
    pkShader->SetName(GetName());
    pkShader->SetImplementation(uiImplementation);

    if (!RegisterTextureStageGlobals(pkRenderer, pkImplementation))
    {
        // Determine how to handle the error case
        NiD3DRenderer::Warning("NSBShader::GetD3DShader - %s - Failed "
            "to register global texture stage variables.\n",
            GetName());
    }

    if (!pkImplementation->SetupNiBinaryShader(*pkShader, m_spShaderDesc))
    {
        NiD3DRenderer::Error("NSBShader::GetD3DShader - %s - Failed "
            "to setup the binary shader.\n", GetName());
        NiDelete pkShader;
        return 0;
    }

    // Setup the packing definition
    if (pkImplementation->GetPackingDef())
    {
        NSBPackingDef* pkPackingDef = GetPackingDef(
            pkImplementation->GetPackingDef(), false);
        if (!pkPackingDef)
        {
            NiD3DRenderer::Error("NSBShader::GetD3DShader - %s - Failed "
                "to find packing definition for implementation.\n",
                GetName());
            NIASSERT(!"Failed to find packing def!");
            NiDelete pkShader;
            return 0;
        }
        else
        {

            NiD3DShaderDeclaration* pkShaderDecl = 
                pkPackingDef->GetShaderDeclaration(NiShader::NISHADER_AGNOSTIC,
                    pkRenderer);
            if (!pkShaderDecl)
            {
                NiD3DRenderer::Error("NSBShader::GetD3DShader - %s - "
                    "Failed to convert packing definition to "
                    "NiD3DShaderDeclaration.\n", GetName());
                NIASSERT(!"Failed to convert packing def!");
                NiDelete pkShader;
                return 0;
            }

            pkShader->SetShaderDecl(pkShaderDecl);
        }
    }
    else
    {
        pkShader->SetShaderDecl(0);
    }

    if (!ReleaseTextureStageGlobals(pkRenderer, pkImplementation))
    {
        // Determine how to handle the error case
        NiD3DRenderer::Warning("NSBShader::GetD3DShader - %s - Failed "
            "to release global texture stage variables.\n",
            GetName());
    }

    return pkShader;
}
//---------------------------------------------------------------------------
NiBinaryShader* NSBShader::GetD3DShader_Xenon(NiD3DRenderer* pkRenderer, 
    unsigned int uiImplementation)
{
    NIASSERT(!"NSBShader::GetD3DShader_Xenon> Compiled for DX9!");
    return 0;
}
//---------------------------------------------------------------------------
#endif  //#if defined(WIN32)
