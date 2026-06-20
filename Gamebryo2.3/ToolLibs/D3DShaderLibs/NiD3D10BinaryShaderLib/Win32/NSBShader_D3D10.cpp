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
#include "NiD3D10BinaryShaderLibPCH.h"

#include "NSBD3D10Shader.h"
#include "NiD3D10BinaryShader.h"
#include "NSBD3D10Pass.h"
#include "NSBD3D10TextureStage.h"

//---------------------------------------------------------------------------
NiD3D10BinaryShader* NSBD3D10Shader::GetD3DShader_D3D10(
    unsigned int uiImplementation)
{
    NSBD3D10Implementation* pkImplementation = 0;

    VersionInfo kVersionInfo;
    SetupVersionInfo(kVersionInfo);

    if (uiImplementation != NiShader::DEFAULT_IMPLEMENTATION)
    {
        // Grab the proper implementation
        pkImplementation = m_kImplementationArray.GetAt(uiImplementation);
        if (!IsImplementationValid(pkImplementation, kVersionInfo))
        {
            NiD3D10Renderer::Warning("NSBD3D10Shader::GetD3DShader - %s - "
                "Requested implementation (%d) invalid on operating "
                "hardware.\n", GetName(), uiImplementation);
            pkImplementation = 0;
        }
    }

    if (!pkImplementation)
    {
        // Get the best for the hardware
        pkImplementation = GetBestImplementation();
        if (!pkImplementation)
        {
            NiD3D10Renderer::Warning("NSBD3D10Shader::GetD3DShader - %s - "
                "Unable to find valid implementation for hardware.\n",
                GetName());
            return 0;
        }
    }

    NiD3D10BinaryShader* pkShader = NiNew NiD3D10BinaryShader();
    NIASSERT(pkShader);

    pkShader->SetName(GetName());
    pkShader->SetImplementation(uiImplementation);

    if (!pkImplementation->SetupNiD3D10BinaryShader(*pkShader, m_spShaderDesc))
    {
        NiD3D10Renderer::Error("NSBD3D10Shader::GetD3DShader - %s - Failed "
            "to setup the binary shader.\n", GetName());
        NiDelete pkShader;
        return 0;
    }

    // Setup the packing definition
    if (pkImplementation->GetPackingDef())
    {
        NSBD3D10PackingDef* pkPackingDef = GetPackingDef(
            pkImplementation->GetPackingDef(), false);
        if (!pkPackingDef)
        {
            NiD3D10Renderer::Error("NSBD3D10Shader::GetD3DShader - %s - "
                "Failed to find packing definition for implementation.\n",
                GetName());
            NIASSERT(!"Failed to find packing def!");
            NiDelete pkShader;
            return 0;
        }
        else
        {

            NiD3D10VertexDescription* pkVertexDesc =
                pkPackingDef->GetVertexDescription();
            if (!pkVertexDesc)
            {
                NiD3D10Renderer::Error("NSBD3D10Shader::GetD3DShader - %s - "
                    "Failed to convert packing definition to "
                    "NiD3D10VertexDescription.\n", GetName());
                NIASSERT(!"Failed to convert packing def!");
                NiDelete pkShader;
                return 0;
            }

            pkShader->SetVertexDescription(pkVertexDesc);
        }
    }
    else
    {
        pkShader->SetVertexDescription(NULL);
    }

    return pkShader;
}
//---------------------------------------------------------------------------
