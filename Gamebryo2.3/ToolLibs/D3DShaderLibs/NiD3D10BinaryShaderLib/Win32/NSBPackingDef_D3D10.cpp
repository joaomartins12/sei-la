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

//---------------------------------------------------------------------------
#include "NSBD3D10PackingDef.h"

#include <NiD3D10VertexDescription.h>

//---------------------------------------------------------------------------
NiD3D10VertexDescriptionPtr NSBD3D10PackingDef::GetVertexDesc_D3D10()
{
    // Determine the stream count, and the max entry count
    unsigned int uiStreamCount = 0;
    unsigned int uiMaxStreamEntryCount = 0;

    GetStreamInfo(uiStreamCount, uiMaxStreamEntryCount);

    NiD3D10VertexDescriptionPtr spVertexDescription;
    bool bSuccess = NiD3D10VertexDescription::Create(uiMaxStreamEntryCount,
        uiStreamCount, spVertexDescription);

    if (spVertexDescription != NULL)
    {
        unsigned int uiEntry = 0;
        unsigned int uiStream = 0xFFFFFFFF;

        NSBD3D10PDEntry* pkEntry = GetFirstEntry();
        while (pkEntry)
        {
            // Add the entry to the vertex description.
            NiD3D10VertexDescription::ShaderParameterType eType;

            eType = ConvertPackingDefType(pkEntry->GetType());

            NIASSERT((unsigned int)eType != 0xffffffff);

            if (uiStream != pkEntry->GetStream())
            {
                uiStream = pkEntry->GetStream();
                uiEntry = 0;
            }

            if (pkEntry->GetUsage()
                == NiD3D10VertexDescription::SPUSAGE_COUNT)
            {
                spVertexDescription->SetEntry(uiEntry, 
                    (NiD3D10VertexDescription::ShaderParameter)
                    pkEntry->GetInput(), eType, pkEntry->GetStream());
            }
            else
            {
                spVertexDescription->SetEntry(pkEntry->GetStream(), uiEntry,
                    (NiD3D10VertexDescription::ShaderParameter)
                    pkEntry->GetInput(), eType,
                    NiShaderDeclaration::UsageToString(pkEntry->GetUsage()), 
                    pkEntry->GetUsageIndex(), 0);
            }

            uiEntry++;
            pkEntry = GetNextEntry();
        }
    }

    return spVertexDescription;
}
//---------------------------------------------------------------------------
NiD3D10VertexDescription::ShaderParameterType
    NSBD3D10PackingDef::ConvertPackingDefType_D3D10(
    NiD3D10PackingDefType eType)
{
    switch (eType)
    {
    case NID3DPDT_FLOAT1:
        return NiD3D10VertexDescription::SPTYPE_FLOAT1;
    case NID3DPDT_FLOAT2:
        return NiD3D10VertexDescription::SPTYPE_FLOAT2;
    case NID3DPDT_FLOAT3:
        return NiD3D10VertexDescription::SPTYPE_FLOAT3;
    case NID3DPDT_FLOAT4:
        return NiD3D10VertexDescription::SPTYPE_FLOAT4;
    case NID3DPDT_UBYTECOLOR:
        return NiD3D10VertexDescription::SPTYPE_UBYTECOLOR;
    case NID3DPDT_UBYTE4:
        return NiD3D10VertexDescription::SPTYPE_UBYTE4;
    case NID3DPDT_SHORT2:
        return NiD3D10VertexDescription::SPTYPE_SHORT2;
    case NID3DPDT_SHORT4:
        return NiD3D10VertexDescription::SPTYPE_SHORT4;
    case NID3DPDT_NORMUBYTE4:
        return NiD3D10VertexDescription::SPTYPE_NORMUBYTE4;
    case NID3DPDT_NORMSHORT2:
        return NiD3D10VertexDescription::SPTYPE_NORMSHORT2;
    case NID3DPDT_NORMSHORT4:
        return NiD3D10VertexDescription::SPTYPE_NORMSHORT4;
    case NID3DPDT_NORMUSHORT2:
        return NiD3D10VertexDescription::SPTYPE_NORMUSHORT2;
    case NID3DPDT_NORMUSHORT4:
        return NiD3D10VertexDescription::SPTYPE_NORMUSHORT4;
    case NID3DPDT_UDEC3:
        return NiD3D10VertexDescription::SPTYPE_UDEC3;
    case NID3DPDT_NORMDEC3:
        return NiD3D10VertexDescription::SPTYPE_NORMDEC3;
    case NID3DPDT_FLOAT16_2:
        return NiD3D10VertexDescription::SPTYPE_FLOAT16_2;
    case NID3DPDT_FLOAT16_4:
        return NiD3D10VertexDescription::SPTYPE_FLOAT16_4;
    }

    return (NiD3D10VertexDescription::ShaderParameterType)0xffffffff;
}
//---------------------------------------------------------------------------
