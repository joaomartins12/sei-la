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

#include "NiPackerMacros.h"
#include "NiDX9ShaderDeclaration.h"
#include "NiVBChip.h"
#include "NiDX9VBManager.h"
#include "NiDX9RenderState.h"
#include "NiGeometryBufferData.h"
#include "NiDX9Renderer.h"

#include <NiGeometryData.h>
#include <NiMemManager.h>
#include <NiSkinInstance.h>

D3DDECLTYPE NiDX9ShaderDeclaration::ms_aeTypes[
    NiD3DShaderDeclaration::SPTYPE_COUNT] = 
{
    D3DDECLTYPE_FLOAT1,
    D3DDECLTYPE_FLOAT2,
    D3DDECLTYPE_FLOAT3,
    D3DDECLTYPE_FLOAT4,
    D3DDECLTYPE_D3DCOLOR,
    D3DDECLTYPE_UBYTE4,
    D3DDECLTYPE_SHORT2,
    D3DDECLTYPE_SHORT4,
    D3DDECLTYPE_UBYTE4N,
    D3DDECLTYPE_SHORT2N,
    D3DDECLTYPE_SHORT4N,
    D3DDECLTYPE_USHORT2N,
    D3DDECLTYPE_USHORT4N,
    D3DDECLTYPE_UDEC3,
    D3DDECLTYPE_DEC3N,
    D3DDECLTYPE_FLOAT16_2,
    D3DDECLTYPE_FLOAT16_4,
};

D3DDECLMETHOD NiDX9ShaderDeclaration::ms_aeMethods[
    NiD3DShaderDeclaration::SPTESS_COUNT] = 
{
    D3DDECLMETHOD_DEFAULT,
    D3DDECLMETHOD_PARTIALU,
    D3DDECLMETHOD_PARTIALV,
    D3DDECLMETHOD_CROSSUV,
    D3DDECLMETHOD_UV,
    D3DDECLMETHOD_LOOKUP,
    D3DDECLMETHOD_LOOKUPPRESAMPLED,
};

D3DDECLUSAGE NiDX9ShaderDeclaration::ms_aeUsage[
    NiD3DShaderDeclaration::SPUSAGE_COUNT] = 
{
    D3DDECLUSAGE_POSITION,
    D3DDECLUSAGE_BLENDWEIGHT,
    D3DDECLUSAGE_BLENDINDICES,
    D3DDECLUSAGE_NORMAL,
    D3DDECLUSAGE_PSIZE,
    D3DDECLUSAGE_TEXCOORD,
    D3DDECLUSAGE_TANGENT,
    D3DDECLUSAGE_BINORMAL,
    D3DDECLUSAGE_TESSFACTOR,
    D3DDECLUSAGE_POSITIONT,
    D3DDECLUSAGE_COLOR,
    D3DDECLUSAGE_FOG,
    D3DDECLUSAGE_DEPTH,
    D3DDECLUSAGE_SAMPLE
};

//---------------------------------------------------------------------------
NiDX9ShaderDeclaration* NiDX9ShaderDeclaration::Create(NiRenderer* pkRenderer,
    unsigned int uiMaxStreamEntryCount, 
    unsigned int uiStreamCount)
{
    NIASSERT(pkRenderer && NiIsKindOf(NiD3DRenderer, pkRenderer));

    // Check against max stream count for card
    if (uiStreamCount > ((NiD3DRenderer*)pkRenderer)->GetMaxStreams())
        return NULL;

    NiDX9ShaderDeclaration* pkShaderDeclaration = 
        NiNew NiDX9ShaderDeclaration;

    if (pkShaderDeclaration)
    {
        if (pkShaderDeclaration->Initialize(pkRenderer, uiMaxStreamEntryCount, 
            uiStreamCount) == false)
        {
            NiDelete pkShaderDeclaration;
            pkShaderDeclaration = 0;
        }
    }

    return pkShaderDeclaration;
}
//---------------------------------------------------------------------------
NiDX9ShaderDeclaration::NiDX9ShaderDeclaration() :
    NiD3DShaderDeclaration(),
    m_pkElements(NULL),
    m_hVertexDecl(NULL),
    m_bSoftwareVB(false)
{ /* */ }
//---------------------------------------------------------------------------
NiDX9ShaderDeclaration::~NiDX9ShaderDeclaration()
{
    NiExternalDelete[] m_pkElements;

    if (m_hVertexDecl)
    {
        D3D_POINTER_RELEASE(m_hVertexDecl);
        m_hVertexDecl = NULL;
    }
}
//---------------------------------------------------------------------------
void NiDX9ShaderDeclaration::MarkAsModified()
{
    m_bModified = true;
    if (m_hVertexDecl)
    {
        D3D_POINTER_RELEASE(m_hVertexDecl);
        m_hVertexDecl = 0;
    }
}
//---------------------------------------------------------------------------
unsigned int NiDX9ShaderDeclaration::GetVertexStride(unsigned int uiStream)
{
    if (uiStream >= m_uiStreamCount)
        return 0;

    // Force an update, if one is required
    GetD3DDeclaration();

    return m_pkStreamEntries[uiStream].m_uiStride;
}
//---------------------------------------------------------------------------
void NiDX9ShaderDeclaration::SetSoftwareVertexProcessing(bool bSoftwareVB)
{
    m_bSoftwareVB = bSoftwareVB;
}
//---------------------------------------------------------------------------
bool NiDX9ShaderDeclaration::GetSoftwareVertexProcessing() const
{
    return m_bSoftwareVB;
}
//---------------------------------------------------------------------------
NiVBChip* NiDX9ShaderDeclaration::PackUntransformedVB(NiGeometryData* pkData,
    NiSkinInstance* pkSkin, NiSkinPartition::Partition* pkPartition, 
    unsigned short usDirtyFlags, NiVBChip* pkOldVBChip, 
    unsigned int uiStream, void* pvLockedBuff)
{
    unsigned short usVerts;

    // Need to handle partitions correctly!
    if (pkPartition)
        usVerts = pkPartition->m_usVertices;
    else
        usVerts = pkData->GetActiveVertexCount();

    // Return existing VB immediately if the object has no active vertices
    if (!usVerts)
        return pkOldVBChip;

    NiD3DVertexDeclaration hDeclaration = 0;
    unsigned int uiStreamCount = 0;

    //  Determine the total size of the buffer needed
    bool bValid = GenerateVertexParameters(hDeclaration, uiStreamCount);
    if (!bValid)
        return NULL;

    NIASSERT(uiStream < uiStreamCount);

    NiGeometryBufferData* pkBuffData = NULL;
    if (pkPartition)
        pkBuffData = (NiGeometryBufferData*)pkPartition->m_pkBuffData;
    else
        pkBuffData = (NiGeometryBufferData*)pkData->GetRendererData();

    pkBuffData->SetVertexDeclaration(hDeclaration);
    pkBuffData->SetVertexStride(uiStream, 
        m_pkStreamEntries[uiStream].m_uiStride);

    if (m_pkRenderer->GetSWVertexCapable())
        pkBuffData->SetSoftwareVP(GetSoftwareVertexProcessing());

    // get a buffer of the right type and size and lock it
    NiVBChip* pkVBChip = NULL;
    bool bUnlock = true;

    if (pvLockedBuff)
    {
        // Already locked - chip must already exist
        pkVBChip = pkOldVBChip;
        bUnlock = false;

        NIASSERT(pkVBChip);
    }
    else
    {
        if (pkOldVBChip && pkOldVBChip->GetVB() && 
            pkData->GetConsistency() != NiGeometryData::VOLATILE)
        {
            D3DVERTEXBUFFER_DESC kVBDesc;

            pkOldVBChip->GetVB()->GetDesc(&kVBDesc);

            // if the size or format has changed, or the buffer was optimized,
            // then skip the old VB
            if ((kVBDesc.Format == D3DFMT_VERTEXDATA) && 
                (kVBDesc.Size >= (usVerts * GetVertexStride(uiStream))))
            {
               pkVBChip = pkOldVBChip;
            }
        }

        if (!pkVBChip)
        {
            if (!m_pkVBManager->AllocateBufferSpace(pkBuffData, uiStream))
                return NULL;

            pkVBChip = pkBuffData->GetVBChip(uiStream);

            // New vertex buffer - we must repack all elements, even if the
            // upper level did not ask for it
            usDirtyFlags = NiGeometryData::DIRTY_MASK;
        }

        pvLockedBuff = m_pkVBManager->LockVB(pkVBChip->GetVB(), 
            pkVBChip->GetOffset(), pkVBChip->GetSize(), 
            pkVBChip->GetLockFlags());

        NIASSERT(pkVBChip && pvLockedBuff);
    }

    // now, copy the values into the locked buffer
    unsigned int uiPackedSize = 0;

    // Check for software skinning
    bool bSoftwareDeform = (pkSkin != 0 && pkPartition == 0);
//    if ((usDirtyFlags & NiGeometryData::VERTEX_MASK) == 0)
//        bSoftwareDeform = false;

    NiPoint3* pkPos = NULL;
    NiPoint3* pkNorm = NULL;
    NiPoint3* pkBinorm = NULL;
    NiPoint3* pkTan = NULL;
    ShaderRegisterStream* pkStream = &(m_pkStreamEntries[uiStream]);
    if (pkStream)
    {
        NIASSERT(pkStream->m_bValid);

        unsigned char* pucTmp;
        ShaderRegisterEntry* pkEntry;

        for (unsigned int uiEntry = 0; uiEntry < m_uiMaxStreamEntryCount; 
            uiEntry++)
        {
            pkEntry = &(pkStream->m_pkEntries[uiEntry]);
            NIASSERT(pkEntry);

            pucTmp = (unsigned char*)pvLockedBuff + pkEntry->m_uiPackingOffset;
            bool bPack = false;

            switch (pkEntry->m_eInput)
            {
            case SHADERPARAM_NI_POSITION:
                if ((usDirtyFlags & NiGeometryData::VERTEX_MASK) != 0)
                {
                    NIASSERT(pkEntry->m_eType == SPTYPE_FLOAT3);
                    if (bSoftwareDeform)
                        pkPos = (NiPoint3*)pucTmp;
                    else
                        bPack = true;
                }
                break;
            case SHADERPARAM_NI_NORMAL:
                if ((usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
                {
                    NIASSERT(pkEntry->m_eType == SPTYPE_FLOAT3);
                    if (bSoftwareDeform)
                        pkNorm = (NiPoint3*)pucTmp;
                    else
                        bPack = true;
                }
                break;
            case SHADERPARAM_NI_BINORMAL:
                if ((usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
                {
                    NIASSERT(pkEntry->m_eType == SPTYPE_FLOAT3);
                    if (bSoftwareDeform)
                        pkBinorm = (NiPoint3*)pucTmp;
                    else
                        bPack = true;
                }
                break;
            case SHADERPARAM_NI_TANGENT:
                if ((usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
                {
                    NIASSERT(pkEntry->m_eType == SPTYPE_FLOAT3);
                    if (bSoftwareDeform)
                        pkTan = (NiPoint3*)pucTmp;
                    else
                        bPack = true;
                }
                break;
            case SHADERPARAM_NI_COLOR:
                if ((usDirtyFlags & NiGeometryData::COLOR_MASK) != 0)
                    bPack = true;
                break;
            case SHADERPARAM_NI_TEXCOORD0:
            case SHADERPARAM_NI_TEXCOORD1:
            case SHADERPARAM_NI_TEXCOORD2:
            case SHADERPARAM_NI_TEXCOORD3:
            case SHADERPARAM_NI_TEXCOORD4:
            case SHADERPARAM_NI_TEXCOORD5:
            case SHADERPARAM_NI_TEXCOORD6:
            case SHADERPARAM_NI_TEXCOORD7:
                if ((usDirtyFlags & NiGeometryData::TEXTURE_MASK) != 0)
                    bPack = true;
                break;
            default:
                // Pack everything else
                bPack = true;
                break;
            }

            if (bPack)
            {
                uiPackedSize += PackEntry(usVerts, pkData, pkSkin, 
                    pkPartition, pkEntry, pkStream->m_uiStride, 
                    pucTmp);
            }
            else
            {
                uiPackedSize += ms_auiTypeSizes[pkEntry->m_eType] * usVerts;
            }
        }
    }

    if (bSoftwareDeform)
    {
        NIASSERT((pkBinorm == NULL) == (pkTan == NULL));
        // This assumes that only positions and NBT will be deformed.
        pkSkin->Deform(pkData, pkPos, pkNorm, pkBinorm, pkTan, 
            pkStream->m_uiStride);
    }

    if (uiPackedSize != pkVBChip->GetSize())
    {
        //  This is an error case?
    }

    // unlock the buffer and return it
    if (bUnlock)
    {
        if (m_pkVBManager->UnlockVB(pkVBChip->GetVB()) == false)
            return NULL;
    }

    return pkVBChip;
}
//---------------------------------------------------------------------------
bool NiDX9ShaderDeclaration::GenerateVertexParameters(
    NiD3DVertexDeclaration& pkDeclaration, unsigned int& uiNumStreams)
{
    pkDeclaration = GetD3DDeclaration();
    uiNumStreams = m_uiStreamCount;

    return (pkDeclaration != NULL);
}
//---------------------------------------------------------------------------
NiD3DVertexDeclaration NiDX9ShaderDeclaration::GetD3DDeclaration()
{
    unsigned int uiDeclSize = 0;
    unsigned int uiStream;
    unsigned int uiEntry;
    ShaderRegisterStream* pkStream;
    ShaderRegisterEntry* pkEntry;
    
    //  Do we need to fill it in?
    if (!m_bModified)
        return m_hVertexDecl;

    // Clear out existing declaration
    if (m_hVertexDecl)
    {
        D3D_POINTER_RELEASE(m_hVertexDecl);
        m_hVertexDecl = NULL;
    }

    //  Determine the needed size...

    //  Examine each stream...
    for (uiStream = 0; uiStream < m_uiStreamCount; uiStream++)
    {
        pkStream = &(m_pkStreamEntries[uiStream]);
        if (pkStream)
        {
            pkStream->m_bValid = false;
            for (uiEntry = 0; uiEntry < m_uiMaxStreamEntryCount; uiEntry++)
            {
                pkEntry = &(pkStream->m_pkEntries[uiEntry]);

                if ((pkEntry->m_eInput != SHADERPARAM_INVALID) &&
                    (pkEntry->m_eType != SPTYPE_NONE))
                {
                    //  Each entry takes one 'slot' in the declaration
                    uiDeclSize++;
                    //  Valid!
                    pkStream->m_bValid = true;
                }
            }
        }
    }

    //  Make sure we have some valid entries...
    if (uiDeclSize > 0)
    {
        //  The D3DDECL_END() takes one 'slot'
        uiDeclSize++;
    }

    //  If we have one allocated, check the size...
    if (m_uiDeclSize < uiDeclSize)
    {
        NiExternalDelete[] m_pkElements;
        m_pkElements = 0;
    }

    //  Quick out...
    if (uiDeclSize == 0)
    {
        m_uiDeclSize = 0;
        return NULL;
    }

    //  Do we need to allocate?
    if (m_pkElements == 0)
    {
        //  Allocate it
        m_uiDeclSize = uiDeclSize;
        m_pkElements = NiExternalNew D3DVERTEXELEMENT9[m_uiDeclSize];
        NIASSERT(m_pkElements);
    }

    m_uiCurrDeclIndex = 0;
    for (uiStream = 0; uiStream < m_uiStreamCount; uiStream++)
    {
        pkStream = &(m_pkStreamEntries[uiStream]);
        if (pkStream->m_bValid)
        {
            unsigned int uiOffset = 0;
            unsigned int uiNewOffset;

            for (uiEntry = 0; uiEntry < m_uiMaxStreamEntryCount; uiEntry++)
            {
                pkEntry = &(pkStream->m_pkEntries[uiEntry]);
                if ((pkEntry->m_eInput != SHADERPARAM_INVALID) &&
                    (pkEntry->m_eType != SPTYPE_NONE))
                {
                    pkEntry->m_uiPackingOffset = uiOffset;
                    
                    uiNewOffset = AddDeclarationEntry(pkEntry, uiStream);
                    uiOffset += uiNewOffset;
                }
            }
            pkStream->m_uiStride = uiOffset;
        }
    }

    D3DVERTEXELEMENT9 akEnd[] = { D3DDECL_END() };

    m_pkElements[m_uiCurrDeclIndex].Stream = akEnd[0].Stream;
    m_pkElements[m_uiCurrDeclIndex].Offset = akEnd[0].Offset;
    m_pkElements[m_uiCurrDeclIndex].Type = akEnd[0].Type;
    m_pkElements[m_uiCurrDeclIndex].Method = akEnd[0].Method;
    m_pkElements[m_uiCurrDeclIndex].Usage = akEnd[0].Usage;
    m_pkElements[m_uiCurrDeclIndex].UsageIndex = akEnd[0].UsageIndex;

    NIASSERT(++m_uiCurrDeclIndex == m_uiDeclSize);

    HRESULT eResult = m_pkD3DDevice->CreateVertexDeclaration(
        m_pkElements, &m_hVertexDecl);

    if (FAILED(eResult))
        return NULL;

    m_bModified = false;

    return m_hVertexDecl;
}
//---------------------------------------------------------------------------
//  Returns the size of the field...
//  This is to allow for determining offsets into the data and to calculate
//  the stride of the buffer being packed.
unsigned int NiDX9ShaderDeclaration::AddDeclarationEntry(
    ShaderRegisterEntry* pkEntry, unsigned int uiStream)
{
    unsigned int uiRet = 0;

    //  Make sure we aren't going to exceed our bounds!
    NIASSERT(m_uiCurrDeclIndex < m_uiDeclSize);

    m_pkElements[m_uiCurrDeclIndex].Stream = uiStream;
    m_pkElements[m_uiCurrDeclIndex].Offset = pkEntry->m_uiPackingOffset;
    m_pkElements[m_uiCurrDeclIndex].Type = ms_aeTypes[pkEntry->m_eType];
    m_pkElements[m_uiCurrDeclIndex].Method = 
        ms_aeMethods[pkEntry->m_uiExtraData];
    m_pkElements[m_uiCurrDeclIndex].Usage = ms_aeUsage[
        NiShaderDeclaration::StringToUsage(pkEntry->m_kUsage)];
    m_pkElements[m_uiCurrDeclIndex].UsageIndex = pkEntry->m_uiUsageIndex;

    m_uiCurrDeclIndex++;

    if (pkEntry->m_eType >= SPTYPE_COUNT)
        return 0;

    return ms_auiTypeSizes[pkEntry->m_eType];
}
//---------------------------------------------------------------------------
bool NiDX9ShaderDeclaration::GetShaderParameterValues(
    D3DDECLUSAGE eD3DUsage, unsigned int uiUsageIndex,
    NiD3DShaderDeclaration::ShaderParameter& eInput, 
    NiD3DShaderDeclaration::ShaderParameterType& eType, 
    NiD3DShaderDeclaration::ShaderParameterUsage& eUsage, 
    NiD3DShaderDeclaration::ShaderParameterTesselator& eTess)
{
    switch (eD3DUsage)
    {
    case D3DDECLUSAGE_POSITION:
        eInput = SHADERPARAM_NI_POSITION;
        eType = SPTYPE_FLOAT3;
        eUsage = SPUSAGE_POSITION;
        break;
    case D3DDECLUSAGE_NORMAL:
        eInput = SHADERPARAM_NI_NORMAL;
        eType = SPTYPE_FLOAT3;
        eUsage = SPUSAGE_NORMAL;
        break;
    case D3DDECLUSAGE_TEXCOORD:
        if (uiUsageIndex > 7)
            return false;
        eInput = (NiD3DShaderDeclaration::ShaderParameter)
            (SHADERPARAM_NI_TEXCOORD0 + uiUsageIndex);
        eType = SPTYPE_FLOAT2;
        eUsage = SPUSAGE_TEXCOORD;
        break;
    case D3DDECLUSAGE_COLOR:
        eInput = SHADERPARAM_NI_COLOR;
        eType = SPTYPE_UBYTECOLOR;
        eUsage = SPUSAGE_COLOR;
        break;
    case D3DDECLUSAGE_BLENDWEIGHT:
        eInput = SHADERPARAM_NI_BLENDWEIGHT;
        eType = SPTYPE_FLOAT3;
        eUsage = SPUSAGE_BLENDWEIGHT;
        break;
    case D3DDECLUSAGE_BLENDINDICES:
        eInput = SHADERPARAM_NI_BLENDINDICES;
        eType = SPTYPE_UBYTECOLOR;
        eUsage = SPUSAGE_BLENDINDICES;
        break;
    case D3DDECLUSAGE_TANGENT:
        eInput = SHADERPARAM_NI_TANGENT;
        eType = SPTYPE_FLOAT3;
        eUsage = SPUSAGE_TANGENT;
        break;
    case D3DDECLUSAGE_BINORMAL:
        eInput = SHADERPARAM_NI_BINORMAL;
        eType = SPTYPE_FLOAT3;
        eUsage = SPUSAGE_BINORMAL;
        break;
    case D3DDECLUSAGE_PSIZE:
    case D3DDECLUSAGE_TESSFACTOR:
    case D3DDECLUSAGE_POSITIONT:
    case D3DDECLUSAGE_FOG:
    case D3DDECLUSAGE_DEPTH:
    case D3DDECLUSAGE_SAMPLE:
    default:
        // No default Gamebryo match-up
        return false;
    }
    eTess = SPTESS_DEFAULT;
    return true;
}
//---------------------------------------------------------------------------
void NiD3DShaderDeclaration::SetDefaultPackingFunction(
    ShaderParameterType eType, unsigned int uiAGDType)
{
    NiPackerEntry* pPackEntry = ms_pkPackFunctions->GetAt(eType);
    NIASSERT(pPackEntry != 0);

    Packer pfnPack = 0;

    if (uiAGDType == NiAdditionalGeometryData::AGD_NITYPE_INVALID)
    {
        pfnPack = NiD3DShaderDeclaration::Pack_IllegalCombination;
        pPackEntry->m_kPackFunctions.SetAt(uiAGDType, pfnPack);
        return;
    }

    if ((eType >= SPTYPE_FLOAT1) && (eType <= SPTYPE_FLOAT4))
    {
        //  Output Float functions
        switch (uiAGDType)
        {
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT1:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT2:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT3:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT4:
            {
                unsigned int uiIn = uiAGDType - 
                    NiAdditionalGeometryData::AGD_NITYPE_FLOAT1;
                unsigned int uiOut = eType - SPTYPE_FLOAT1;
                
                if (uiIn == uiOut)
                {
                    pfnPack = NiD3DShaderDeclaration::Pack_FloatToFloat;
                }
                else
                if (uiIn < uiOut)
                {
                    pfnPack = 
                        NiD3DShaderDeclaration::Pack_Float_ExtraOutsToZero;
                }
                else
                {
                    pfnPack = NiD3DShaderDeclaration::Pack_Float_DropIns;
                }
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_LONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_LongToFloat;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_UnsignedLongToFloat;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_ShortToFloat;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_UnsignedShortToFloat;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_ByteToFloat;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_UnsignedByteToFloat;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND1:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND2:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND3:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND4:
            {
                pfnPack = 
                    NiD3DShaderDeclaration::Pack_Float_ExtraOutsToZero;
            }
            break;
        }

        if (pfnPack)
            pPackEntry->m_kPackFunctions.SetAt(uiAGDType, pfnPack);
    }
    else
    if (eType == SPTYPE_UBYTECOLOR)
    {
        //  Output UByteColor (D3DCOLOR) function
        switch (uiAGDType)
        {
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT1:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT2:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_IllegalCombination;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT3:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_FloatToUByteColor;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_LONG1:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_LongToUByteColor;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_LONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_IllegalCombination;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG1:
            {
                pfnPack = 
                    NiD3DShaderDeclaration::Pack_UnsignedLongToUByteColor;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_IllegalCombination;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT3:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_IllegalCombination;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_ShortToUByteColor;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT3:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_IllegalCombination;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT4:
            {
                pfnPack = 
                    NiD3DShaderDeclaration::Pack_UnsignedShortToUByteColor;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_ByteToUByteColor;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE4:
            {
                pfnPack = 
                    NiD3DShaderDeclaration::Pack_UnsignedByteToUByteColor;
            }
            break;

        case NiAdditionalGeometryData::AGD_NITYPE_BLEND1:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND2:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND3:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_FloatToUByteColor;
            }
            break;
        }

        if (pfnPack)
            pPackEntry->m_kPackFunctions.SetAt(uiAGDType, pfnPack);
    }
    else
    if ((eType == SPTYPE_SHORT2) || (eType == SPTYPE_SHORT4))
    {
        switch (uiAGDType)
        {
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT1:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT2:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT3:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_FloatToShort;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_LONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_LongToShort;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_UnsignedLongToShort;
            }
            break;

        case NiAdditionalGeometryData::AGD_NITYPE_SHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_ShortToShort;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_UnsignedShortToShort;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_ByteToShort;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_UnsignedByteToShort;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND1:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND2:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND3:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_IllegalCombination;
            }
            break;
        }

        if (pfnPack)
            pPackEntry->m_kPackFunctions.SetAt(uiAGDType, pfnPack);
    }
    else
    if (eType == SPTYPE_UBYTE4)
    {
        switch (uiAGDType)
        {
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT1:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT2:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT3:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_FloatToUnsignedByte;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_LONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_LongToUnsignedByte;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG4:
            {
                pfnPack = 
                    NiD3DShaderDeclaration::Pack_UnsignedLongToUnsignedByte;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_ShortToUnsignedByte;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT4:
            {
                pfnPack = 
                    NiD3DShaderDeclaration::Pack_UnsignedShortToUnsignedByte;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_ByteToUnsignedByte;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE4:
            {
                pfnPack = 
                    NiD3DShaderDeclaration::Pack_UnsignedByteToUnsignedByte;
            }
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND1:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND2:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND3:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND4:
            {
                pfnPack = NiD3DShaderDeclaration::Pack_IllegalCombination;
            }
            break;
        }

        if (pfnPack)
            pPackEntry->m_kPackFunctions.SetAt(uiAGDType, pfnPack);
    }
}
//---------------------------------------------------------------------------
//  Short output packers
//---------------------------------------------------------------------------
// Signed shorts map to the range [-32768, 32767]
unsigned int NiD3DShaderDeclaration::Pack_FloatToShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    float* pfInData;
    SHORT* psOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT4);
    NIASSERT(kParams.m_ePackType >= SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= SPTYPE_SHORT4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInFloats = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 + 1;
    unsigned short usOutShorts = 
        (kParams.m_ePackType == SPTYPE_SHORT2) ? 2 : 4;
    
    unsigned short usPackCount;
    if (usInFloats > usOutShorts)
        usPackCount = usOutShorts;
    else
        usPackCount = usInFloats;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pfInData = (float*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            psOutData = (SHORT*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_FLOAT_TO_SHORT(*pfInData, *psOutData);
                psOutData++;
                pfInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pfInData = (float*)(pucInData);
            psOutData = (SHORT*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_FLOAT_TO_SHORT(*pfInData, *psOutData);
                psOutData++;
                pfInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_LongToShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    long* plInData;
    SHORT* psOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_LONG1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_LONG4);
    NIASSERT(kParams.m_ePackType >= SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= SPTYPE_SHORT4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInLongs = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_LONG1 + 1;
    unsigned short usOutShorts = 
        (kParams.m_ePackType == SPTYPE_SHORT2) ? 2 : 4;
    
    unsigned short usPackCount;
    if (usInLongs > usOutShorts)
        usPackCount = usOutShorts;
    else
        usPackCount = usInLongs;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            plInData = (long*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            psOutData = (SHORT*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_LONG_TO_SHORT(*plInData, *psOutData);
                psOutData++;
                plInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            plInData = (long*)(pucInData);
            psOutData = (SHORT*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_LONG_TO_SHORT(*plInData, *psOutData);
                psOutData++;
                plInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_UnsignedLongToShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned long* pulInData;
    SHORT* psOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_ULONG1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_ULONG4);
    NIASSERT(kParams.m_ePackType >= SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= SPTYPE_SHORT4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInULongs = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_ULONG1 + 1;
    unsigned short usOutShorts = 
        (kParams.m_ePackType == SPTYPE_SHORT2) ? 2 : 4;
    
    unsigned short usPackCount;
    if (usInULongs > usOutShorts)
        usPackCount = usOutShorts;
    else
        usPackCount = usInULongs;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pulInData = (unsigned long*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            psOutData = (SHORT*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_ULONG_TO_SHORT(*pulInData, *psOutData);
                psOutData++;
                pulInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pulInData = (unsigned long*)(pucInData);
            psOutData = (SHORT*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_ULONG_TO_SHORT(*pulInData, *psOutData);
                psOutData++;
                pulInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_ShortToShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    short* psInData;
    SHORT* psOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_SHORT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_SHORT4);
    NIASSERT(kParams.m_ePackType >= SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= SPTYPE_SHORT4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInShorts = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_SHORT1 + 1;
    unsigned short usOutShorts = 
        (kParams.m_ePackType == SPTYPE_SHORT2) ? 2 : 4;
    
    unsigned short usPackCount;
    if (usInShorts > usOutShorts)
        usPackCount = usOutShorts;
    else
        usPackCount = usInShorts;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            psInData = (short*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);

            memcpy(pucOutData, psInData, usPackCount * sizeof(short));

            psOutData = (SHORT*)(pucOutData) + usPackCount;

            unsigned short usPack = usPackCount;

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            psInData = (short*)(pucInData);

            memcpy(pucOutData, psInData, usPackCount * sizeof(short));

            psOutData = (SHORT*)(pucOutData) + usPackCount;

            unsigned short usPack = usPackCount;

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_UnsignedShortToShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned short* pusInData;
    SHORT* psOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_USHORT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_USHORT4);
    NIASSERT(kParams.m_ePackType >= SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= SPTYPE_SHORT4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInUShorts = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_USHORT1 + 1;
    unsigned short usOutShorts = 
        (kParams.m_ePackType == SPTYPE_SHORT2) ? 2 : 4;
    
    unsigned short usPackCount;
    if (usInUShorts > usOutShorts)
        usPackCount = usOutShorts;
    else
        usPackCount = usInUShorts;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pusInData = (unsigned short*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            psOutData = (SHORT*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                *psOutData++ = *pusInData++;

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pusInData = (unsigned short*)(pucInData);
            psOutData = (SHORT*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                *psOutData++ = *pusInData++;

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_ByteToShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    char* pcInData;
    SHORT* psOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_BYTE1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_BYTE4);
    NIASSERT(kParams.m_ePackType >= SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= SPTYPE_SHORT4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInBytes = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_BYTE1 + 1;
    unsigned short usOutShorts = 
        (kParams.m_ePackType == SPTYPE_SHORT2) ? 2 : 4;
    
    unsigned short usPackCount;
    if (usInBytes > usOutShorts)
        usPackCount = usOutShorts;
    else
        usPackCount = usInBytes;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pcInData = (char*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            psOutData = (SHORT*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_BYTE_TO_SHORT(*pcInData, *psOutData);
                psOutData++;
                pcInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pcInData = (char*)(pucInData);
            psOutData = (SHORT*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_BYTE_TO_SHORT(*pcInData, *psOutData);
                psOutData++;
                pcInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_UnsignedByteToShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned char* pucInDataTemp;
    SHORT* psOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_UBYTE1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_UBYTE4);
    NIASSERT(kParams.m_ePackType >= SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= SPTYPE_SHORT4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInUBytes = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_UBYTE1 + 1;
    unsigned short usOutShorts = 
        (kParams.m_ePackType == SPTYPE_SHORT2) ? 2 : 4;
    
    unsigned short usPackCount;
    if (usInUBytes > usOutShorts)
        usPackCount = usOutShorts;
    else
        usPackCount = usInUBytes;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pucInDataTemp = (unsigned char*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            psOutData = (SHORT*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_UBYTE_TO_SHORT(*pucInDataTemp, *psOutData);
                psOutData++;
                pucInDataTemp++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pucInDataTemp = (unsigned char*)(pucInData);
            psOutData = (SHORT*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_UBYTE_TO_SHORT(*pucInDataTemp, *psOutData);
                psOutData++;
                pucInDataTemp++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutShorts; usPack++)
                *psOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Normalized short output packers
//---------------------------------------------------------------------------
// (signed, normalized short maps from -1.0 to 1.0)
unsigned int NiD3DShaderDeclaration::Pack_FloatToNormalizeShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_LongToNormalizeShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_UnsignedLongToNormalizeShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_ShortToNormalizeShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_UnsignedShortToNormalizeShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_ByteToNormalizeShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_UnsignedByteToNormalizeShort(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
//  Normalize packed output packers
//---------------------------------------------------------------------------
// 3 signed, normalized components packed in 32-bits.  (11,11,10).  
// Each component ranges from -1.0 to 1.0.  
// Expanded to (value, value, value, 1.)
unsigned int NiD3DShaderDeclaration::Pack_FloatToNormalizedPacked(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
//  Packed byte output packers
//---------------------------------------------------------------------------
// Packed bytes map to the range [0, 1]
unsigned int NiD3DShaderDeclaration::Pack_FloatToPackedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_LongToPackedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_UnsignedLongToPackedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_ShortToPackedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_UnsignedShortToPackedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_ByteToPackedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_UnsignedByteToPackedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//-----------------------------------------------------------------------
//  Unsigned byte output packers
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_FloatToUnsignedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    float* pfInData;
    BYTE* pbyOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT4);
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTE4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInFloats = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 + 1;
    unsigned short usOutUBytes = 4;
    
    unsigned short usPackCount;
    if (usInFloats > usOutUBytes)
        usPackCount = usOutUBytes;
    else
        usPackCount = usInFloats;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pfInData = (float*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pbyOutData = (BYTE*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_FLOAT_TO_UBYTE(*pfInData, *pbyOutData);
                pbyOutData++;
                pfInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pfInData = (float*)(pucInData);
            pbyOutData = (BYTE*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_FLOAT_TO_UBYTE(*pfInData, *pbyOutData);
                pbyOutData++;
                pfInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_LongToUnsignedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    LONG* plInData;
    BYTE* pbyOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_LONG1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_LONG4);
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTE4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInLongs = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_LONG1 + 1;
    unsigned short usOutUBytes = 4;
    
    unsigned short usPackCount;
    if (usInLongs > usOutUBytes)
        usPackCount = usOutUBytes;
    else
        usPackCount = usInLongs;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            plInData = (long*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pbyOutData = (BYTE*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_LONG_TO_UBYTE(*plInData, *pbyOutData);
                pbyOutData++;
                plInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            plInData = (LONG*)(pucInData);
            pbyOutData = (BYTE*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_LONG_TO_UBYTE(*plInData, *pbyOutData);
                pbyOutData++;
                plInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_UnsignedLongToUnsignedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned long* pulInData;
    BYTE* pbyOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_ULONG1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_ULONG4);
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTE4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInULongs = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_ULONG1 + 1;
    unsigned short usOutUBytes = 4;
    
    unsigned short usPackCount;
    if (usInULongs > usOutUBytes)
        usPackCount = usOutUBytes;
    else
        usPackCount = usInULongs;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pulInData = (unsigned long*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pbyOutData = (BYTE*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_ULONG_TO_UBYTE(*pulInData, *pbyOutData);
                pbyOutData++;
                pulInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pulInData = (unsigned long*)(pucInData);
            pbyOutData = (BYTE*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_ULONG_TO_UBYTE(*pulInData, *pbyOutData);
                pbyOutData++;
                pulInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_ShortToUnsignedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    SHORT* psInData;
    BYTE* pbyOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_SHORT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_SHORT4);
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTE4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInShorts = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_SHORT1 + 1;
    unsigned short usOutUBytes = 4;
    
    unsigned short usPackCount;
    if (usInShorts > usOutUBytes)
        usPackCount = usOutUBytes;
    else
        usPackCount = usInShorts;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            psInData = (short*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pbyOutData = (BYTE*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_SHORT_TO_UBYTE(*psInData, *pbyOutData);
                pbyOutData++;
                psInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            psInData = (short*)(pucInData);
            pbyOutData = (BYTE*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_SHORT_TO_UBYTE(*psInData, *pbyOutData);
                pbyOutData++;
                psInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_UnsignedShortToUnsignedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned short* pusInData;
    BYTE* pbyOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_USHORT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_USHORT4);
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTE4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInUShorts = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_USHORT1 + 1;
    unsigned short usOutUBytes = 4;
    
    unsigned short usPackCount;
    if (usInUShorts > usOutUBytes)
        usPackCount = usOutUBytes;
    else
        usPackCount = usInUShorts;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pusInData = (unsigned short*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pbyOutData = (BYTE*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_USHORT_TO_UBYTE(*pusInData, *pbyOutData);
                pbyOutData++;
                pusInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pusInData = (unsigned short*)(pucInData);
            pbyOutData = (BYTE*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_USHORT_TO_UBYTE(*pusInData, *pbyOutData);
                pbyOutData++;
                pusInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_ByteToUnsignedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    BYTE* pbyInData;
    BYTE* pbyOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >=
        NiAdditionalGeometryData::AGD_NITYPE_BYTE1);
    NIASSERT(kParams.m_uiDataType <=
        NiAdditionalGeometryData::AGD_NITYPE_BYTE4);
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTE4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInBytes = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_BYTE1 + 1;
    unsigned short usOutUBytes = 4;
    
    unsigned short usPackCount;
    if (usInBytes > usOutUBytes)
        usPackCount = usOutUBytes;
    else
        usPackCount = usInBytes;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pbyInData = (BYTE*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pbyOutData = (BYTE*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                *pbyOutData++ = *pbyInData++;

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pbyInData = (BYTE*)(pucInData);
            pbyOutData = (BYTE*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                *pbyOutData++ = *pbyInData++;

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_UnsignedByteToUnsignedByte(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    BYTE* pbyInData;
    BYTE* pbyOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_UBYTE1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_UBYTE4);
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTE4);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInUBytes = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_UBYTE1 + 1;
    unsigned short usOutUBytes = 4;
    
    unsigned short usPackCount;
    if (usInUBytes > usOutUBytes)
        usPackCount = usOutUBytes;
    else
        usPackCount = usInUBytes;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pbyInData = (BYTE*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);

            memcpy(pucOutData, pbyInData, usPackCount * sizeof(BYTE));

            pbyOutData = (BYTE*)(pucOutData) + usPackCount;

            //  Fill empty outs, if needed
            unsigned short usPack = usPackCount;
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pbyInData = (BYTE*)(pucInData);

            memcpy(pucOutData, pbyInData, usPackCount * sizeof(BYTE));

            pbyOutData = (BYTE*)(pucOutData) + usPackCount;

            //  Fill empty outs, if needed
            unsigned short usPack = usPackCount;
            for (; usPack < usOutUBytes; usPack++)
                *pbyOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  2D Homogeneous float output packers
//---------------------------------------------------------------------------
// 2D homogeneous float expanded to (value, value,0., value.)
// Useful for projective texture coordinates.
unsigned int NiD3DShaderDeclaration::Pack_FloatTo2DHomogenous(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    return uiPacked;
}
//---------------------------------------------------------------------------
