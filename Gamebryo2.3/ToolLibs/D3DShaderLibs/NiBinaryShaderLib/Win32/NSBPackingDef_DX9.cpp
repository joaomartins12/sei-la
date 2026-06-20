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

// Only include this file if it's a WIN32 build!
#if defined(WIN32)
//---------------------------------------------------------------------------
#include "NSBPackingDef.h"
#include "NSBUtility.h"
#include <NiD3DUtility.h>
#include <NiDX9ShaderDeclaration.h>

//---------------------------------------------------------------------------
NiD3DShaderDeclaration* NSBPackingDef::GetShaderDeclaration_DX9(
    NiD3DRenderer* pkRenderer)
{
    NIASSERT(NiIsKindOf(NiDX9Renderer, pkRenderer));
    // Determine the stream count, and the max entry count
    unsigned int uiStreamCount = 0;
    unsigned int uiMaxStreamEntryCount = 0;

    GetStreamInfo(uiStreamCount, uiMaxStreamEntryCount);

    NiDX9ShaderDeclaration* pkShaderDecl = NiDX9ShaderDeclaration::Create(
        (NiRenderer*)pkRenderer, uiMaxStreamEntryCount, uiStreamCount);

    if (pkShaderDecl)
    {
        unsigned int uiEntry = 0;
        unsigned int uiStream = 0xFFFFFFFF;

        NSBPDEntry* pkEntry = GetFirstEntry();
        while (pkEntry)
        {
            // Add the entry to the shader declaration
            NiD3DShaderDeclaration::ShaderParameterType eType;

            eType = ConvertPackingDefType(pkEntry->GetType());

            NIASSERT((unsigned int)eType != 0xffffffff);

            if (uiStream != pkEntry->GetStream())
            {
                uiStream = pkEntry->GetStream();
                uiEntry = 0;
            }

            if (pkEntry->GetUsage() == NiD3DShaderDeclaration::SPUSAGE_COUNT)
            {
                pkShaderDecl->SetEntry(uiEntry, 
                    (NiD3DShaderDeclaration::ShaderParameter)
                    pkEntry->GetInput(), eType, pkEntry->GetStream());
            }
            else
            {
                pkShaderDecl->SetEntry(pkEntry->GetStream(), uiEntry, 
                    (NiD3DShaderDeclaration::ShaderParameter)
                    pkEntry->GetInput(), eType, 
                    NiShaderDeclaration::UsageToString(pkEntry->GetUsage()), 
                    pkEntry->GetUsageIndex(), pkEntry->GetTesselator());
            }

            uiEntry++;
            pkEntry = GetNextEntry();
        }
    }

    return pkShaderDecl;
}
//---------------------------------------------------------------------------
NiD3DShaderDeclaration* NSBPackingDef::GetShaderDeclaration_Xenon(
    NiD3DRenderer* pkRenderer)
{
    NIASSERT(!"NSBPackingDef::GetShaderDeclaration_Xenon> DX9 BUILD!");
    return 0;
}
//---------------------------------------------------------------------------
NiD3DShaderDeclaration::ShaderParameterType 
    NSBPackingDef::ConvertPackingDefType_DX9(NiD3DPackingDefType eType)
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    NIASSERT(NiIsKindOf(NiDX9Renderer, pkRenderer));
    switch (eType)
    {
    case NID3DPDT_FLOAT1:
        return NiD3DShaderDeclaration::SPTYPE_FLOAT1;
    case NID3DPDT_FLOAT2:
        return NiD3DShaderDeclaration::SPTYPE_FLOAT2;
    case NID3DPDT_FLOAT3:
        return NiD3DShaderDeclaration::SPTYPE_FLOAT3;
    case NID3DPDT_FLOAT4:
        return NiD3DShaderDeclaration::SPTYPE_FLOAT4;
    case NID3DPDT_UBYTECOLOR:
        return NiD3DShaderDeclaration::SPTYPE_UBYTECOLOR;
    case NID3DPDT_UBYTE4:
        return NiD3DShaderDeclaration::SPTYPE_UBYTE4;
    case NID3DPDT_SHORT2:
        return NiD3DShaderDeclaration::SPTYPE_SHORT2;
    case NID3DPDT_SHORT4:
        return NiD3DShaderDeclaration::SPTYPE_SHORT4;
    case NID3DPDT_NORMUBYTE4:
        return NiD3DShaderDeclaration::SPTYPE_NORMUBYTE4;
    case NID3DPDT_NORMSHORT2:
        return NiD3DShaderDeclaration::SPTYPE_NORMSHORT2;
    case NID3DPDT_NORMSHORT4:
        return NiD3DShaderDeclaration::SPTYPE_NORMSHORT4;
    case NID3DPDT_NORMUSHORT2:
        return NiD3DShaderDeclaration::SPTYPE_NORMUSHORT2;
    case NID3DPDT_NORMUSHORT4:
        return NiD3DShaderDeclaration::SPTYPE_NORMUSHORT4;
    case NID3DPDT_UDEC3:
        return NiD3DShaderDeclaration::SPTYPE_UDEC3;
    case NID3DPDT_NORMDEC3:
        return NiD3DShaderDeclaration::SPTYPE_NORMDEC3;
    case NID3DPDT_FLOAT16_2:
        return NiD3DShaderDeclaration::SPTYPE_FLOAT16_2;
    case NID3DPDT_FLOAT16_4:
        return NiD3DShaderDeclaration::SPTYPE_FLOAT16_4;
    }

    return (NiD3DShaderDeclaration::ShaderParameterType)0xffffffff;
}
//---------------------------------------------------------------------------
NiD3DShaderDeclaration::ShaderParameterType 
    NSBPackingDef::ConvertPackingDefType_Xenon(NiD3DPackingDefType eType)
{
    NIASSERT(!"NSBPackingDef::ConvertPackingDefType_Xenon> DX9 BUILD!");
    return (NiD3DShaderDeclaration::ShaderParameterType)0xffffffff;
}
//---------------------------------------------------------------------------
#endif  //#if defined(WIN32)
//---------------------------------------------------------------------------
