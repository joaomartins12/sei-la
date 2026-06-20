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

#include "NiD3D10GeometryPacker.h"
#include "NiD3D10PackerMacros.h"
#include "NiD3D10VertexDescription.h"

#include <NiAdditionalGeometryData.h>
#include <NiSkinInstance.h>
#include <NiSkinPartition.h>

NiTPrimitiveArray<NiD3D10GeometryPacker::NiPackerEntry*>* 
    NiD3D10GeometryPacker::ms_pkPackFunctions;

//---------------------------------------------------------------------------
void NiD3D10GeometryPacker::_SDMInit()
{
    ms_pkPackFunctions = NiNew NiTPrimitiveArray<NiPackerEntry*>;

    InitializePackingFunctions();
}
//---------------------------------------------------------------------------
void NiD3D10GeometryPacker::_SDMShutdown()
{
    for (unsigned int i = 0; i < ms_pkPackFunctions->GetSize(); i++)
    {
        NiDelete ms_pkPackFunctions->GetAt(i);
    }
    ms_pkPackFunctions->RemoveAll();
    NiDelete ms_pkPackFunctions;
}
//---------------------------------------------------------------------------
NiD3D10GeometryPacker::NiPackerEntry::NiPackerEntry()
{
    m_uiShaderParameterType = NiShaderDeclaration::SPTYPE_NONE;
    m_kPackFunctions.RemoveAll();
}
//---------------------------------------------------------------------------
NiD3D10GeometryPacker::NiPackerEntry::NiPackerEntry(
    const NiPackerEntry& kSrc)
{
    m_uiShaderParameterType = kSrc.m_uiShaderParameterType;
    // Clear the function array
    m_kPackFunctions.RemoveAll();
}
//---------------------------------------------------------------------------
NiD3D10GeometryPacker::NiPackerEntry::~NiPackerEntry()
{
    m_kPackFunctions.RemoveAll();
}
//---------------------------------------------------------------------------
bool NiD3D10GeometryPacker::NiPackerEntry::operator==(
    const NiPackerEntry& kSrc)
{
    if (m_uiShaderParameterType != kSrc.m_uiShaderParameterType)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10GeometryPacker::NiPackerEntry::operator!=(
    const NiPackerEntry& kSrc)
{
    if (m_uiShaderParameterType == kSrc.m_uiShaderParameterType)
        return false;

    return true;
}
//---------------------------------------------------------------------------
NiD3D10GeometryPacker::NiPackerEntry& 
    NiD3D10GeometryPacker::NiPackerEntry::operator=(const NiPackerEntry& kSrc)
{
    m_uiShaderParameterType = kSrc.m_uiShaderParameterType;
    //  Clear the function array...
    m_kPackFunctions.RemoveAll();
    //  Copy the function arrays...

    return *(this);
}
//---------------------------------------------------------------------------
NiD3D10GeometryPacker::NiD3D10GeometryPacker()
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10GeometryPacker::~NiD3D10GeometryPacker()
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10GeometryPacker::Packer NiD3D10GeometryPacker::RegisterPackingFunction(
    NiShaderDeclaration::ShaderParameterType eType, unsigned int uiAGDType, 
    Packer pfnPack)
{
    Packer pfnOld = 0;

    NIASSERT(pfnPack);

    NIASSERT(uiAGDType < NiAdditionalGeometryData::AGD_NITYPE_COUNT);

    //  Test it...
    NiD3D10GeometryPacker::NiPackerEntry* pkPackEntry = 
        NiD3D10GeometryPacker::ms_pkPackFunctions->GetAt(
        (unsigned int)eType);
    if (pkPackEntry != 0)
    {
        NIASSERT(pkPackEntry->m_uiShaderParameterType == (unsigned int)eType);

        pfnOld = pkPackEntry->m_kPackFunctions.GetAt(uiAGDType);

        if (pfnPack)
        {
            pkPackEntry->m_kPackFunctions.SetAtGrow(uiAGDType, pfnPack);
        }
        else
        {
            NiD3D10GeometryPacker::SetDefaultPackingFunction(eType, 
                uiAGDType);
        }
    }

    return pfnOld;
}
//---------------------------------------------------------------------------
bool NiD3D10GeometryPacker::InitializePackingFunctions()
{
    unsigned int uiSize = 
        NiD3D10GeometryPacker::ms_pkPackFunctions->GetSize();
    if (uiSize == NiShaderDeclaration::SPTYPE_COUNT)
    {
        // Already initialized
        return true;
    }

    // Create it
    for (unsigned int ui = 0; ui < NiShaderDeclaration::SPTYPE_COUNT; ui++)
    {
        NiD3D10GeometryPacker::NiPackerEntry* pkPackEntry = 
            NiNew NiD3D10GeometryPacker::NiPackerEntry();
        NIASSERT(pkPackEntry != 0);

        pkPackEntry->m_uiShaderParameterType = ui;
        pkPackEntry->m_kPackFunctions.SetSize(
            NiAdditionalGeometryData::AGD_NITYPE_COUNT + 5);

        NiD3D10GeometryPacker::ms_pkPackFunctions->SetAtGrow(ui, pkPackEntry);
    }

    // Fill it
    SetDefaultPackingFunctions();

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryPacker::PackEntry(unsigned short usCount, 
    NiGeometryData* pkData, const NiSkinInstance* pkSkin, 
    const NiShaderDeclaration::ShaderRegisterEntry* pkEntry, 
    unsigned int uiStride, void* pvDest, unsigned int uiInputStride, 
    void* pvOverrideInput)
{
    NIASSERT(pkEntry);
    NIASSERT(pvDest);

    NiShaderDeclaration::ShaderParameterType eType = pkEntry->m_eType;
    if (eType == NiShaderDeclaration::SPTYPE_NONE)
    {
        return 0;
    }

    // D3D10 no longer has a UBYTECOLOR format (normalized B8G8R8A8), so
    // switch it to either NORMUBYTE4 (normalized R8G8B8A8) or UBYTE4 in the
    // case of BlendIndices.
    if (eType == NiShaderDeclaration::SPTYPE_UBYTECOLOR)
    {
        if (pkEntry->m_eInput == 
            NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES)
        {
            eType = NiShaderDeclaration::SPTYPE_UBYTE4;
        }
        else
        {
            eType = NiShaderDeclaration::SPTYPE_NORMUBYTE4;
        }
    }

    PackingParameters kPackParams;
    kPackParams.m_usCount = usCount;
    kPackParams.m_uiOutStride = uiStride;
    kPackParams.m_pvOutData = pvDest;

    kPackParams.m_uiOutSize = NiD3D10VertexDescription::GetSPTypeSize(eType);

    NiPackerEntry* pkPackEntry = 
        NiD3D10GeometryPacker::ms_pkPackFunctions->GetAt(eType);
    if (pkPackEntry == 0)
    {
        NIASSERT(0);
        return 0;
    }

    NiAdditionalGeometryData* pkExtraData = 
        pkData->GetAdditionalGeometryData();

    // Need to use the data type to grab the actual packing function
    unsigned int uiDataType;

    NiSkinPartition* pkSkinPartition = 
        (pkSkin ? pkSkin->GetSkinPartition() : NULL);
    NiSkinPartition::Partition* pkPartition = 
        (pkSkinPartition ? pkSkinPartition->GetPartitions() : NULL);
    const unsigned int uiPartitionCount = 
        (pkSkinPartition ? pkSkinPartition->GetPartitionCount() : 1);

    unsigned int uiVerticesPacked = 0;

    for (unsigned int i = 0; i < uiPartitionCount; i++)
    {
        switch (pkEntry->m_eInput)
        {
        case NiShaderDeclaration::SHADERPARAM_INVALID:
            {
                NIASSERT(0);
                return 0;
            }
            break;
        case NiShaderDeclaration::SHADERPARAM_NI_POSITION:
            kPackParams.m_pvInData = (void*)pkData->GetVertices();
            kPackParams.m_uiInSize = sizeof(NiPoint3);
            kPackParams.m_uiInStride = kPackParams.m_uiInSize;
            uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT3;
            break;
        case NiShaderDeclaration::SHADERPARAM_NI_NORMAL:
            kPackParams.m_pvInData = (void*)pkData->GetNormals();
            kPackParams.m_uiInSize = sizeof(NiPoint3);
            kPackParams.m_uiInStride = kPackParams.m_uiInSize;
            uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT3;
            break;
        case NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT:
            if (pkPartition == 0)
            {
                // No partition - no bones
                NIASSERT(0);
                return 0;
            };

            // Determine the vertex format (w/ Blend Weights)
            NIASSERT(pkPartition->m_usBonesPerVertex >= 1);
            NIASSERT(pkPartition->m_usBonesPerVertex <= 4);

            // Determine the weights (and FVF flag)
            switch (pkPartition->m_usBonesPerVertex)
            {
            case 1:
                uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT1;
                kPackParams.m_uiInSize = sizeof(float) * 1;
                kPackParams.m_uiInStride = kPackParams.m_uiInSize;
                break;
            case 2:
                uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT2;
                kPackParams.m_uiInSize = sizeof(float) * 2;
                kPackParams.m_uiInStride = kPackParams.m_uiInSize;
                break;
            case 3:
                uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT3;
                kPackParams.m_uiInSize = sizeof(float) * 3;
                kPackParams.m_uiInStride = kPackParams.m_uiInSize;
                break;
            case 4:
                uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT3;
                kPackParams.m_uiInSize = sizeof(float) * 3;
                kPackParams.m_uiInStride = sizeof(float) * 4;
                break;
            }
            kPackParams.m_pvInData = (void*)pkPartition->m_pfWeights;
            break;
        case NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES:
            if (pkPartition == 0)
            {
                // No partition - no bones
                NIASSERT(0);
                return 0;
            }

            if (pkPartition->m_pucBonePalette == 0)
            {
                // No indices
                NIASSERT(0);
                return 0;
            }

            // Make sure we are clear here
            NIASSERT(pkPartition->m_usBonesPerVertex >= 1);
            NIASSERT(pkPartition->m_usBonesPerVertex <= 4);

            // Determine the bone index count
            switch (pkPartition->m_usBonesPerVertex)
            {
            case 1:
                uiDataType = NiAdditionalGeometryData::AGD_NITYPE_UBYTE1;
                kPackParams.m_uiInSize = sizeof(unsigned char) * 1;
                kPackParams.m_uiInStride = kPackParams.m_uiInSize;
                break;
            case 2:
                uiDataType = NiAdditionalGeometryData::AGD_NITYPE_UBYTE2;
                kPackParams.m_uiInSize = sizeof(unsigned char) * 2;
                kPackParams.m_uiInStride = kPackParams.m_uiInSize;
                break;
            case 3:
                uiDataType = NiAdditionalGeometryData::AGD_NITYPE_UBYTE3;
                kPackParams.m_uiInSize = sizeof(unsigned char) * 3;
                kPackParams.m_uiInStride = kPackParams.m_uiInSize;
                break;
            case 4:
                uiDataType = NiAdditionalGeometryData::AGD_NITYPE_UBYTE4;
                kPackParams.m_uiInSize = sizeof(unsigned char) * 4;
                kPackParams.m_uiInStride = kPackParams.m_uiInSize;
                break;
            }
            kPackParams.m_pvInData = (void*)pkPartition->m_pucBonePalette;
            break;
        case NiShaderDeclaration::SHADERPARAM_NI_COLOR:
            kPackParams.m_pvInData = (void*)pkData->GetColors();
            kPackParams.m_uiInSize = sizeof(NiColorA);
            kPackParams.m_uiInStride = kPackParams.m_uiInSize;
            uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT4;
            break;
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0:
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD1:
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD2:
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD3:
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD4:
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD5:
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD6:
        case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD7:
            kPackParams.m_pvInData = (void*)pkData->GetTextureSet(
                pkEntry->m_eInput - 
                NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0);
            kPackParams.m_uiInSize = sizeof(NiPoint2);
            kPackParams.m_uiInStride = kPackParams.m_uiInSize;
            uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT2;
            break;
        case NiShaderDeclaration::SHADERPARAM_NI_BINORMAL:
            {
                NiPoint3* pkNormals = pkData->GetNormals();
                if (pkNormals && 
                    pkData->GetNormalBinormalTangentMethod() != 
                    NiGeometryData::NBT_METHOD_NONE)
                {
                    kPackParams.m_pvInData = 
                        (void*)(pkNormals + pkData->GetVertexCount());
                }
                else
                {
                    kPackParams.m_pvInData = NULL;
                }
            }
            kPackParams.m_uiInSize = sizeof(NiPoint3);
            kPackParams.m_uiInStride = kPackParams.m_uiInSize;
            uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT3;
            break;
        case NiShaderDeclaration::SHADERPARAM_NI_TANGENT:
            {
                NiPoint3* pkNormals = pkData->GetNormals();
                if (pkNormals && 
                    pkData->GetNormalBinormalTangentMethod() != 
                    NiGeometryData::NBT_METHOD_NONE)
                {
                    kPackParams.m_pvInData = 
                        (void*)(pkNormals + 2 * pkData->GetVertexCount());
                }
                else
                {
                    kPackParams.m_pvInData = NULL;
                }
            }
            kPackParams.m_uiInSize = sizeof(NiPoint3);
            kPackParams.m_uiInStride = kPackParams.m_uiInSize;
            uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT3;
            break;
        default:
            if (pkEntry->m_eInput & 
                NiShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK)
            {
                unsigned int uiStream = pkEntry->m_eInput & 
                    ~NiShaderDeclaration::SHADERPARAM_EXTRA_DATA_MASK;

                NIASSERT(pkExtraData);
                unsigned short usAGDCount;
                unsigned int uiTotalSize;
                unsigned char* pucData;
                bool bGotDataStream = pkExtraData->GetDataStream(uiStream,
                    pucData, uiDataType, usAGDCount, 
                    uiTotalSize, kPackParams.m_uiInSize,
                    kPackParams.m_uiInStride);

                if (bGotDataStream)
                {
                    NIASSERT(usAGDCount == usCount);
                    kPackParams.m_pvInData = pucData;
                }
                else
                {
                    NiOutputDebugString("WARNING> Attempting to pack "
                        "additional per-vertex geometry data, but the "
                        "NiAdditionalGeometryData did not contain the"
                        " DataStream.\n");

                    kPackParams.m_pvInData = NULL;
                    kPackParams.m_uiInSize = NULL;
                    kPackParams.m_uiInStride = NULL;
                    uiDataType = 
                        NiAdditionalGeometryData::AGD_NITYPE_INVALID;
                }
            }
            break;
        }

        if (pvOverrideInput)
        {
            kPackParams.m_pvInData = pvOverrideInput;
            kPackParams.m_uiInSize = kPackParams.m_uiOutSize;
            kPackParams.m_uiInStride = uiInputStride;
        }

        Packer pfnPack = pkPackEntry->m_kPackFunctions.GetAt(uiDataType);
        if (pfnPack == 0)
        {
            // No packing function for this combination!
            // Probably don't want to assert, just 'fail'.
            NIASSERT(0);
            return 0;
        }

        if (pkPartition && (pkEntry->m_eInput != 
            NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT) &&
            (pkEntry->m_eInput != 
            NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES))
        {
            kPackParams.m_pusVertexMap = pkPartition->m_pusVertexMap;
        }
        else
        {
            kPackParams.m_pusVertexMap = 0;
        }

        if (pkPartition)
            kPackParams.m_usCount = pkPartition->m_usVertices;

        kPackParams.m_ePackType = eType;
        kPackParams.m_uiDataType = uiDataType;

        if (pfnPack(kPackParams) == false)
            return false;

        if (pkPartition)
            pkPartition++;

        kPackParams.m_pvOutData = (char*)(kPackParams.m_pvOutData) +
            kPackParams.m_uiOutStride * kPackParams.m_usCount;

        uiVerticesPacked += kPackParams.m_usCount;
    }

    NIASSERT(uiVerticesPacked == usCount);

    return true;
}
//---------------------------------------------------------------------------
void NiD3D10GeometryPacker::SetDefaultPackingFunctions()
{
    for (unsigned int i = NiShaderDeclaration::SPTYPE_FLOAT1; 
        i < NiShaderDeclaration::SPTYPE_COUNT; i++)
    {
        for (unsigned int j = NiAdditionalGeometryData::AGD_NITYPE_INVALID;
            j < NiAdditionalGeometryData::AGD_NITYPE_COUNT; j++)
        {
            SetDefaultPackingFunction(
                (NiShaderDeclaration::ShaderParameterType)i, j);
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10GeometryPacker::SetDefaultPackingFunction(
    NiShaderDeclaration::ShaderParameterType eType, unsigned int uiAGDType)
{
    NiPackerEntry* pPackEntry = ms_pkPackFunctions->GetAt(eType);
    NIASSERT(pPackEntry != 0);

    Packer pfnPack = 0;

    if (uiAGDType == NiAdditionalGeometryData::AGD_NITYPE_INVALID)
    {
        pfnPack = Pack_IllegalCombination;
        pPackEntry->m_kPackFunctions.SetAt(uiAGDType, pfnPack);
        return;
    }

    if ((eType >= NiShaderDeclaration::SPTYPE_FLOAT1) && 
        (eType <= NiShaderDeclaration::SPTYPE_FLOAT4))
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
            unsigned int uiOut = eType - 
                NiShaderDeclaration::SPTYPE_FLOAT1;
            
            if (uiIn == uiOut)
                pfnPack = Pack_Float32ToFloat32;
            else if (uiIn < uiOut)
                pfnPack = Pack_Float32_ExtraOutsToZero;
            else
                pfnPack = Pack_Float32_DropIns;
            break;
        }
        case NiAdditionalGeometryData::AGD_NITYPE_LONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG4:
            pfnPack = Pack_SNorm32ToFloat32;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG4:
            pfnPack = Pack_UNorm32ToFloat32;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT4:
            pfnPack = Pack_SNorm16ToFloat32;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT4:
            pfnPack = Pack_UNorm16ToFloat32;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE4:
            pfnPack = Pack_SNorm8ToFloat32;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE4:
            pfnPack = Pack_UNorm8ToFloat32;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND1:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND2:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND3:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND4:
            pfnPack = Pack_Float32_ExtraOutsToZero;
            break;
        }

        if (pfnPack)
            pPackEntry->m_kPackFunctions.SetAt(uiAGDType, pfnPack);
    }
    else if (eType == NiShaderDeclaration::SPTYPE_UBYTECOLOR)
    {
        //  Output UByteColor (D3DCOLOR) function
        switch (uiAGDType)
        {
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT1:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT2:
            pfnPack = Pack_IllegalCombination;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT3:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT4:
            pfnPack = Pack_Float32ToUByteColor;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_LONG1:
            pfnPack = Pack_SInt32ToUByteColor;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_LONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG4:
            pfnPack = Pack_IllegalCombination;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG1:
            pfnPack = Pack_UInt32ToUByteColor;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG4:
            pfnPack = Pack_IllegalCombination;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT3:
            pfnPack = Pack_IllegalCombination;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT4:
            pfnPack = Pack_SInt16ToUByteColor;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT3:
            pfnPack = Pack_IllegalCombination;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT4:
            pfnPack = Pack_UInt16ToUByteColor;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE4:
            pfnPack = Pack_SInt8ToUByteColor;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE4:
            pfnPack = Pack_UInt8ToUByteColor;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND1:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND2:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND3:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND4:
            pfnPack = Pack_Float32ToUByteColor;
            break;
        }

        if (pfnPack)
            pPackEntry->m_kPackFunctions.SetAt(uiAGDType, pfnPack);
    }
    else if (eType == NiShaderDeclaration::SPTYPE_NORMUBYTE4)
    {
        //  Output UByteColor (D3DCOLOR) function
        switch (uiAGDType)
        {
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT1:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT2:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT3:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT4:
            pfnPack = Pack_Float32ToUNorm8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_LONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG4:
            pfnPack = Pack_SInt32ToUNorm8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG4:
            pfnPack = Pack_UInt32ToUNorm8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT4:
            pfnPack = Pack_SInt16ToUNorm8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT4:
            pfnPack = Pack_UInt16ToUNorm8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE4:
            pfnPack = Pack_SInt8ToUNorm8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE4:
            pfnPack = Pack_UInt8ToUNorm8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND1:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND2:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND3:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND4:
            pfnPack = Pack_Float32ToUNorm8;
            break;
        }

        if (pfnPack)
            pPackEntry->m_kPackFunctions.SetAt(uiAGDType, pfnPack);
    }
    else if ((eType == NiShaderDeclaration::SPTYPE_SHORT2) || 
        (eType == NiShaderDeclaration::SPTYPE_SHORT4))
    {
        switch (uiAGDType)
        {
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT1:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT2:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT3:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT4:
            pfnPack = Pack_Float32ToSInt16;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_LONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG4:
            pfnPack = Pack_SInt32ToSInt16;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG4:
            pfnPack = Pack_UInt32ToSInt16;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT4:
            pfnPack = Pack_SInt16ToSInt16;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT4:
            pfnPack = Pack_UInt16ToSInt16;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE4:
            pfnPack = Pack_SInt8ToSInt16;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE4:
            pfnPack = Pack_UInt8ToSInt16;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND1:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND2:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND3:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND4:
            pfnPack = Pack_IllegalCombination;
            break;
        }

        if (pfnPack)
            pPackEntry->m_kPackFunctions.SetAt(uiAGDType, pfnPack);
    }
    else if (eType == NiShaderDeclaration::SPTYPE_UBYTE4)
    {
        switch (uiAGDType)
        {
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT1:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT2:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT3:
        case NiAdditionalGeometryData::AGD_NITYPE_FLOAT4:
            pfnPack = Pack_Float32ToUInt8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_LONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_LONG4:
            pfnPack = Pack_SInt32ToUInt8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG1:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG2:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG3:
        case NiAdditionalGeometryData::AGD_NITYPE_ULONG4:
            pfnPack = Pack_UInt32ToUInt8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_SHORT4:
            pfnPack = Pack_SInt16ToUInt8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT1:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT2:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT3:
        case NiAdditionalGeometryData::AGD_NITYPE_USHORT4:
            pfnPack = Pack_UInt16ToUInt8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_BYTE4:
            pfnPack = Pack_SInt8ToUInt8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE1:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE2:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE3:
        case NiAdditionalGeometryData::AGD_NITYPE_UBYTE4:
            pfnPack = Pack_UInt8ToUInt8;
            break;
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND1:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND2:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND3:
        case NiAdditionalGeometryData::AGD_NITYPE_BLEND4:
            pfnPack = Pack_IllegalCombination;
            break;
        }

        if (pfnPack)
            pPackEntry->m_kPackFunctions.SetAt(uiAGDType, pfnPack);
    }
}
//---------------------------------------------------------------------------
//  Default data packing functions
//---------------------------------------------------------------------------
#define _PACK_ILLEGALCOMBINATION_ASSERT_
//#define _PACK_ILLEGALCOMBINATION_ZERO_
//#define _PACK_ILLEGALCOMBINATION_NOTHING_
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryPacker::Pack_IllegalCombination(
    PackingParameters& kParams)
{
    //  Allow for customer to determine what to do here...
#if defined(_PACK_ILLEGALCOMBINATION_ASSERT_)
    //  Throw an assertion to indicate an invalid pack.
    NIASSERT(0);
    return 0;
#endif  //#if defined(_PACK_ILLEGALCOMBINATION_ASSERT_)

#if defined(_PACK_ILLEGALCOMBINATION_NOTHING_)
    //  Pack nothing here, and don't increase the size
    return 0;
#endif  //#if defined(_PACK_ILLEGALCOMBINATION_NOTHING_)

#if defined(_PACK_ILLEGALCOMBINATION_ZERO_)
    //  Pack zeros into the data...
    unsigned int uiPacked = 0;
    unsigned char* pucData = (unsigned char*)(kParams.m_pvOutData);

    for (unsigned short us = 0; us < kParams.m_usCount; us++)
    {
        memset((void*)pucData, 0, kParams.m_uiOutSize);
        pucData += kParams.m_uiOutStride;
        uiPacked += kParams.m_uiOutSize;
    }

    return uiPacked;
#endif  //#if defined(_PACK_ILLEGALCOMBINATION_ZERO_)
}
//---------------------------------------------------------------------------
//  Float output packers
//---------------------------------------------------------------------------
//  Packing in > out floats, dropping additional ins
unsigned int NiD3D10GeometryPacker::Pack_Float32_DropIns(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;
    float* pfInData;
    float* pfOutData;

    //  Safety check!
    NIASSERT(kParams.m_uiInSize > kParams.m_uiOutSize);
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT4);
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_FLOAT4);

    //  We need to check the out size vs the in size...
    unsigned short usInFloats = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 + 1;
    unsigned short usOutFloats = kParams.m_ePackType - 
        NiShaderDeclaration::SPTYPE_FLOAT1 + 1;

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

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pfInData = (float*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pfOutData = (float*)(pucOutData);
            NiMemcpy(pfOutData, pfInData, kParams.m_uiOutSize);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            //  
            NiMemcpy(pucOutData, pucInData, kParams.m_uiOutSize);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Packing in < out floats, filling the additional outs w/ 0.0f
unsigned int NiD3D10GeometryPacker::Pack_Float32_ExtraOutsToZero(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;
    float* pfInData;
    float* pfOutData;

    //  Safety check!
    NIASSERT(kParams.m_uiInSize < kParams.m_uiOutSize);
    NIASSERT(kParams.m_uiDataType >=
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT4);
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_FLOAT4);

    //  We need to check the out size vs the in size...
    unsigned short usInFloats = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 + 1;
    unsigned short usOutFloats = kParams.m_ePackType - 
        NiShaderDeclaration::SPTYPE_FLOAT1 + 1;
    unsigned short usExtraOuts = usOutFloats - usInFloats;
    unsigned short usExtra;

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

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pfInData = (float*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            NiMemcpy(pucOutData, pfInData, kParams.m_uiInSize);

            pfOutData = (float*)(pucOutData + kParams.m_uiInSize);

            for (usExtra = 0; usExtra < usExtraOuts; usExtra++)
                *pfOutData++ = 0.0f;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            NiMemcpy(pucOutData, pucInData, kParams.m_uiInSize);

            pfOutData = (float*)(pucOutData + kParams.m_uiInSize);

            for (usExtra = 0; usExtra < usExtraOuts; usExtra++)
                *pfOutData++ = 0.0f;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Packing in < out floats, filling the additional outs w/ 1.0f
unsigned int NiD3D10GeometryPacker::Pack_Float32_ExtraOutsToOne(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;
    float* pfInData;
    float* pfOutData;

    //  Safety check!
    NIASSERT(kParams.m_uiInSize < kParams.m_uiOutSize);
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT4);
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_FLOAT4);

    //  We need to check the out size vs the in size...
    unsigned short usInFloats = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 + 1;
    unsigned short usOutFloats = kParams.m_ePackType - 
        NiShaderDeclaration::SPTYPE_FLOAT1 + 1;
    unsigned short usExtraOuts = usOutFloats - usInFloats;

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pfOutData = (float*)(pucOutData);

            for (unsigned short usFill = 0; usFill < usOutFloats; usFill++)
                *pfOutData++ = 1.0f;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pfInData = (float*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            NiMemcpy(pucOutData, pfInData, kParams.m_uiInSize);

            pfOutData = (float*)(pucOutData + kParams.m_uiInSize);

            for (unsigned short usFill = 0; usFill < usExtraOuts; usFill++)
                *pfOutData++ = 1.0f;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            NiMemcpy(pucOutData, pucInData, kParams.m_uiInSize);

            pfOutData = (float*)(pucOutData + kParams.m_uiInSize);

            for (unsigned short usFill = 0; usFill < usExtraOuts; usFill++)
                *pfOutData++ = 1.0f;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Packing a float to float. Direct Mapping...
unsigned int NiD3D10GeometryPacker::Pack_Float32ToFloat32(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;
    float* pfInData;
    float* pfOutData;

    //  Safety check!
    NIASSERT(kParams.m_uiInSize == kParams.m_uiOutSize);
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT4);
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_FLOAT4);

    //  We need to check the out size vs the in size...
    unsigned short usInFloats = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 + 1;
    unsigned short usOutFloats = kParams.m_ePackType - 
        NiShaderDeclaration::SPTYPE_FLOAT1 + 1;
    unsigned short usExtraOuts = usOutFloats - usInFloats;

    NIASSERT(usExtraOuts == 0);

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

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pfInData = (float*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pfOutData = (float*)(pucOutData);
            NiMemcpy(pfOutData, pfInData, kParams.m_uiInSize);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            NiMemcpy(pucOutData, pucInData, kParams.m_uiInSize);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Packing a long to a float. Handles count differences w/in the function.
//  Uses the macro NIPACKER_SNORM32_TO_FLOAT32
unsigned int NiD3D10GeometryPacker::Pack_SNorm32ToFloat32(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    LONG* plInData;
    float* pfOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_LONG1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_LONG4);
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_FLOAT4);

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
    unsigned short usOutFloats = kParams.m_ePackType - 
        NiShaderDeclaration::SPTYPE_FLOAT1 + 1;

    unsigned short usPackCount;
    if (usInLongs > usOutFloats)
        usPackCount = usOutFloats;
    else
        usPackCount = usInLongs;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            plInData = (LONG*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pfOutData = (float*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_SNORM32_TO_FLOAT32(*plInData, *pfOutData);
                pfOutData++;
                plInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutFloats; usPack++)
                *pfOutData++ = 0.0f;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            plInData = (LONG*)(pucInData);
            pfOutData = (float*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_SNORM32_TO_FLOAT32(*plInData, *pfOutData);
                pfOutData++;
                plInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutFloats; usPack++)
                *pfOutData++ = 0.0f;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Packing a long to a float. Handles count differences w/in the function.
//  Uses the macro NIPACKER_UNORM32_TO_FLOAT32
unsigned int NiD3D10GeometryPacker::Pack_UNorm32ToFloat32(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned long* pulInData;
    float* pfOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_ULONG1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_ULONG4);
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_FLOAT4);

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
    unsigned short usOutFloats = kParams.m_ePackType - 
        NiShaderDeclaration::SPTYPE_FLOAT1 + 1;

    unsigned short usPackCount;
    if (usInULongs > usOutFloats)
        usPackCount = usOutFloats;
    else
        usPackCount = usInULongs;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pulInData = (unsigned long*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pfOutData = (float*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_UNORM32_TO_FLOAT32(*pulInData, *pfOutData);
                pfOutData++;
                pulInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutFloats; usPack++)
                *pfOutData++ = 0.0f;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pulInData = (unsigned long*)(pucInData);
            pfOutData = (float*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_UNORM32_TO_FLOAT32(*pulInData, *pfOutData);
                pfOutData++;
                pulInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutFloats; usPack++)
                *pfOutData++ = 0.0f;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Packing a short to a float. Handles count differences w/in the function.
//  Uses the macro NIPACKER_SNORM16_TO_FLOAT32
unsigned int NiD3D10GeometryPacker::Pack_SNorm16ToFloat32(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    SHORT* psInData;
    float* pfOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_SHORT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_SHORT4);
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_FLOAT4);

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
    unsigned short usOutFloats = kParams.m_ePackType - 
        NiShaderDeclaration::SPTYPE_FLOAT1 + 1;

    unsigned short usPackCount;
    if (usInShorts > usOutFloats)
        usPackCount = usOutFloats;
    else
        usPackCount = usInShorts;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            psInData = (SHORT*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pfOutData = (float*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_SNORM16_TO_FLOAT32(*psInData, *pfOutData);
                pfOutData++;
                psInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutFloats; usPack++)
                *pfOutData++ = 0.0f;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            psInData = (SHORT*)(pucInData);
            pfOutData = (float*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_SNORM16_TO_FLOAT32(*psInData, *pfOutData);
                pfOutData++;
                psInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutFloats; usPack++)
                *pfOutData++ = 0.0f;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Packing a short to a float. Handles count differences w/in the function.
//  Uses the macro NIPACKER_UNORM16_TO_FLOAT32
unsigned int NiD3D10GeometryPacker::Pack_UNorm16ToFloat32(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned short* pusInData;
    float* pfOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_USHORT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_USHORT4);
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_FLOAT4);

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
    unsigned short usOutFloats = kParams.m_ePackType - 
        NiShaderDeclaration::SPTYPE_FLOAT1 + 1;

    unsigned short usPackCount;
    if (usInUShorts > usOutFloats)
        usPackCount = usOutFloats;
    else
        usPackCount = usInUShorts;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pusInData = (unsigned short*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pfOutData = (float*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_UNORM16_TO_FLOAT32(*pusInData, *pfOutData);
                pfOutData++;
                pusInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutFloats; usPack++)
                *pfOutData++ = 0.0f;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pusInData = (unsigned short*)(pucInData);
            pfOutData = (float*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_UNORM16_TO_FLOAT32(*pusInData, *pfOutData);
                pfOutData++;
                pusInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutFloats; usPack++)
                *pfOutData++ = 0.0f;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Packing a byte to a float. Handles count differences w/in the function.
//  Uses the macro NIPACKER_SNORM8_TO_FLOAT32
unsigned int NiD3D10GeometryPacker::Pack_SNorm8ToFloat32(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    char* pcInData;
    float* pfOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_BYTE1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_BYTE4);
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_FLOAT4);

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
    unsigned short usOutFloats = kParams.m_ePackType - 
        NiShaderDeclaration::SPTYPE_FLOAT1 + 1;

    unsigned short usPackCount;
    if (usInBytes > usOutFloats)
        usPackCount = usOutFloats;
    else
        usPackCount = usInBytes;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pcInData = (char*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pfOutData = (float*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_SNORM8_TO_FLOAT32(*pcInData, *pfOutData);
                pfOutData++;
                pcInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutFloats; usPack++)
                *pfOutData++ = 0.0f;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pcInData = (char*)(pucInData);
            pfOutData = (float*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_SNORM8_TO_FLOAT32(*pcInData, *pfOutData);
                pfOutData++;
                pcInData++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutFloats; usPack++)
                *pfOutData++ = 0.0f;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Packing a byte to a float. Handles count differences w/in the function.
//  Uses the macro NIPACKER_UNORM8_TO_FLOAT32
unsigned int NiD3D10GeometryPacker::Pack_UNorm8ToFloat32(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned char* pucInDataTemp;
    float* pfOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_UBYTE1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_UBYTE4);
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_FLOAT4);

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
    unsigned short usOutFloats = kParams.m_ePackType - 
        NiShaderDeclaration::SPTYPE_FLOAT1 + 1;

    unsigned short usPackCount;
    if (usInUBytes > usOutFloats)
        usPackCount = usOutFloats;
    else
        usPackCount = usInUBytes;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pucInDataTemp = (unsigned char*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            pfOutData = (float*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_UNORM8_TO_FLOAT32(*pucInDataTemp, *pfOutData);
                pfOutData++;
                pucInDataTemp++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutFloats; usPack++)
                *pfOutData++ = 0.0f;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pucInDataTemp = (unsigned char*)(pucInData);
            pfOutData = (float*)(pucOutData);

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
            {
                NIPACKER_UNORM8_TO_FLOAT32(*pucInDataTemp, *pfOutData);
                pfOutData++;
                pucInDataTemp++;
            }

            //  Fill empty outs, if needed
            for (; usPack < usOutFloats; usPack++)
                *pfOutData++ = 0.0f;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  UByteColor output packers
//---------------------------------------------------------------------------
//  UByte is a D3DCOLOR...
//  Float - assumes there will be 3 or 4 floats per color...
//  order Red,Green,Blue<,Alpha>
unsigned int NiD3D10GeometryPacker::Pack_Float32ToUByteColor(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    float* pfInData;
    unsigned int* puiOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT3);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT4);
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTECOLOR);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            puiOutData = (unsigned int*)(pucOutData);
            NIPACKER_FLOAT32_TO_UBYTECOLOR(1.0f, 1.0f, 1.0f, 1.0f, 
                *puiOutData);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInFloats = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 + 1;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pfInData = (float*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            puiOutData = (unsigned int*)(pucOutData);

            float fAlpha = (usInFloats == 3 ? 1.0f : pfInData[3]);

            NIPACKER_FLOAT32_TO_UBYTECOLOR(fAlpha, pfInData[0], pfInData[1], 
                pfInData[2], *puiOutData);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pfInData = (float*)(pucInData);
            puiOutData = (unsigned int*)(pucOutData);

            float fAlpha = (usInFloats == 3 ? 1.0f : pfInData[3]);

            NIPACKER_FLOAT32_TO_UBYTECOLOR(fAlpha, pfInData[0], pfInData[1], 
                pfInData[2], *puiOutData);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  SInt32 - assumes one long per color (already calculated...)
unsigned int NiD3D10GeometryPacker::Pack_SInt32ToUByteColor(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    LONG* plInData;
    unsigned int* puiOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType == 
        NiAdditionalGeometryData::AGD_NITYPE_LONG1);
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTECOLOR);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            puiOutData = (unsigned int*)pucOutData;
            NIPACKER_SINT32_TO_UBYTECOLOR(0xffffffff, *puiOutData);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            plInData = (LONG*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            puiOutData = (unsigned int*)pucOutData;

            NIPACKER_SINT32_TO_UBYTECOLOR(*plInData, *puiOutData);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            plInData = (LONG*)(pucInData);
            puiOutData = (unsigned int*)pucOutData;

            NIPACKER_SINT32_TO_UBYTECOLOR(*plInData, *puiOutData);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  UInt32 - assumes one long per color (already calculated...)
unsigned int NiD3D10GeometryPacker::Pack_UInt32ToUByteColor(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned long* pulInData;
    unsigned int* puiOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType == 
        NiAdditionalGeometryData::AGD_NITYPE_ULONG1);
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTECOLOR);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            puiOutData = (unsigned int*)pucOutData;
            NIPACKER_UINT32_TO_UBYTECOLOR(0xffffffff, *puiOutData);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pulInData = (unsigned long*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            puiOutData = (unsigned int*)pucOutData;

            NIPACKER_UINT32_TO_UBYTECOLOR(*pulInData, *puiOutData);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pulInData = (unsigned long*)(pucInData);
            puiOutData = (unsigned int*)pucOutData;

            NIPACKER_UINT32_TO_UBYTECOLOR(*pulInData, *puiOutData);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Assumes that there a 3 or 4 shorts per output color.
//  Ordering to be R,G,B<,A>
//  Will truncate the shorts to form the color.
unsigned int NiD3D10GeometryPacker::Pack_SInt16ToUByteColor(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    SHORT* psInData;
    unsigned int* puiOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_SHORT3);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_SHORT4);
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTECOLOR);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            puiOutData = (unsigned int*)(pucOutData);
            NIPACKER_SINT16_TO_UBYTECOLOR(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 
                *puiOutData);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInShorts = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_SHORT1 + 1;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            psInData = (SHORT*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            puiOutData = (unsigned int*)(pucOutData);

            short sAlpha = (usInShorts == 3 ? 0xFFFF : psInData[3]);

            NIPACKER_SINT16_TO_UBYTECOLOR(sAlpha, psInData[0], psInData[1], 
                psInData[2], *puiOutData);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            psInData = (SHORT*)(pucInData);
            puiOutData = (unsigned int*)pucOutData;

            short sAlpha = (usInShorts == 3 ? 0xFFFF : psInData[3]);
            NIPACKER_SINT16_TO_UBYTECOLOR(sAlpha, psInData[0], psInData[1], 
                psInData[2], *puiOutData);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Assumes that there a 3 or 4 shorts per output color.
//  Ordering to be R,G,B<,A>
//  Will truncate the shorts to form the color.
unsigned int NiD3D10GeometryPacker::Pack_UInt16ToUByteColor(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned short* pusInData;
    unsigned int* puiOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_USHORT3);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_USHORT4);
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTECOLOR);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            puiOutData = (unsigned int*)(pucOutData);
            NIPACKER_UINT16_TO_UBYTECOLOR(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 
                *puiOutData);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInShorts = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_USHORT1 + 1;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pusInData = (unsigned short*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            puiOutData = (unsigned int*)(pucOutData);

            unsigned short usAlpha = (usInShorts == 3 ? 0xFFFF : pusInData[3]);

            NIPACKER_UINT16_TO_UBYTECOLOR(usAlpha, pusInData[0], pusInData[1], 
                pusInData[2], *puiOutData);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pusInData = (unsigned short*)(pucInData);
            puiOutData = (unsigned int*)pucOutData;

            unsigned short usAlpha = (usInShorts == 3 ? 0xFFFF : pusInData[3]);

            NIPACKER_UINT16_TO_UBYTECOLOR(usAlpha, pusInData[0], pusInData[1],
                pusInData[2], *puiOutData);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Assumes that there a 3 or 4 bytes per output color.
//  Ordering to be R,G,B<,A>
unsigned int NiD3D10GeometryPacker::Pack_SInt8ToUByteColor(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    char* pcInData;
    unsigned int* puiOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >=
        NiAdditionalGeometryData::AGD_NITYPE_BYTE3);
    NIASSERT(kParams.m_uiDataType <=
        NiAdditionalGeometryData::AGD_NITYPE_BYTE4);
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTECOLOR);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0xFF, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInBytes = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_BYTE1 + 1;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pcInData = (char*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            puiOutData = (unsigned int*)(pucOutData);

            char cAlpha = (usInBytes == 3 ? -1 : pcInData[3]);

            NIPACKER_SINT8_TO_UBYTECOLOR(cAlpha, pcInData[0], pcInData[1], 
                pcInData[2], *puiOutData);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pcInData = (char*)(pucInData);
            puiOutData = (unsigned int*)(pucOutData);

            char cAlpha = (usInBytes == 3 ? -1 : pcInData[3]);

            NIPACKER_SINT8_TO_UBYTECOLOR(cAlpha, pcInData[0], pcInData[1], 
                pcInData[2], *puiOutData);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Assumes that there a 3 or 4 bytes per output color.
//  Ordering to be R,G,B<,A>
unsigned int NiD3D10GeometryPacker::Pack_UInt8ToUByteColor(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned char* pucInDataTemp;
    unsigned int* puiOutData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_UBYTE3);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_UBYTE4);
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTECOLOR);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            memset(pucOutData, 0xFF, kParams.m_uiOutSize);
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }

        return uiPacked;
    }

    //  We need to check the out size vs the in size...
    unsigned short usInBytes = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_UBYTE1 + 1;

    if (pusVertMap)
    {
        unsigned short usMapIndex = 0;

        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pucInDataTemp = (unsigned char*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            puiOutData = (unsigned int*)(pucOutData);

            unsigned char ucAlpha = (usInBytes == 3 ? 0xFF : pucInDataTemp[3]);

            NIPACKER_UINT8_TO_UBYTECOLOR(ucAlpha, pucInDataTemp[0], 
                pucInDataTemp[1], pucInDataTemp[2], *puiOutData);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pucInDataTemp = (unsigned char*)(pucInData);
            puiOutData = (unsigned int*)(pucOutData);

            unsigned char ucAlpha = (usInBytes == 3 ? 0xFF : pucInDataTemp[3]);

            NIPACKER_UINT8_TO_UBYTECOLOR(ucAlpha, pucInDataTemp[0], 
                pucInDataTemp[1], pucInDataTemp[2], *puiOutData);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  UNorm8 output packers
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryPacker::Pack_Float32ToUNorm8(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    float* pfInData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT((kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 &&
        kParams.m_uiDataType <= NiAdditionalGeometryData::AGD_NITYPE_FLOAT4) ||
        (kParams.m_uiDataType >= NiAdditionalGeometryData::AGD_NITYPE_BLEND1 &&
        kParams.m_uiDataType <= NiAdditionalGeometryData::AGD_NITYPE_BLEND4));

    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_NORMUBYTE4);

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
    unsigned short usBase = (kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 &&
        kParams.m_uiDataType <= NiAdditionalGeometryData::AGD_NITYPE_FLOAT4) ?
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 :
        NiAdditionalGeometryData::AGD_NITYPE_BLEND1;

    unsigned short usInFloats = kParams.m_uiDataType - usBase + 1;
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
            unsigned char* pucTempOutData = pucOutData;

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                NIPACKER_FLOAT32_TO_UNORM8(*pfInData++, *pucTempOutData++);

            //  Fill empty outs, if needed - use 1.0f
            for (; usPack < usOutUBytes; usPack++)
                NIPACKER_FLOAT32_TO_UNORM8(1.0f, *pucTempOutData++);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pfInData = (float*)(pucInData);
            unsigned char* pucTempOutData = pucOutData;

            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                NIPACKER_FLOAT32_TO_UNORM8(*pfInData++, *pucTempOutData++);

            //  Fill empty outs, if needed - use 1.0f
            for (; usPack < usOutUBytes; usPack++)
                NIPACKER_FLOAT32_TO_UNORM8(1.0f, *pucTempOutData++);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryPacker::Pack_SInt32ToUNorm8(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    LONG* plInData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_LONG1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_LONG4);
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_NORMUBYTE4);

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
            plInData = (LONG*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            unsigned char* pucTempOutData = pucOutData;

            // Bitwise, UINT8 and UNORM8 are identical, so pack using UINT8.
            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                NIPACKER_SINT32_TO_UINT8(*plInData++, *pucTempOutData++);

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                NIPACKER_SINT32_TO_UINT8(0, *pucTempOutData++);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            plInData = (LONG*)(pucInData);
            unsigned char* pucTempOutData = pucOutData;

            // Bitwise, UINT8 and UNORM8 are identical, so pack using UINT8.
            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                NIPACKER_SINT32_TO_UINT8(*plInData++, *pucTempOutData++);

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                NIPACKER_SINT32_TO_UINT8(0, *pucTempOutData++);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryPacker::Pack_UInt32ToUNorm8(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned long* pulInData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_ULONG1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_ULONG4);
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_NORMUBYTE4);

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
            unsigned char* pucTempOutData = pucOutData;

            // Bitwise, UINT8 and UNORM8 are identical, so pack using UINT8.
            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                NIPACKER_UINT32_TO_UINT8(*pulInData++, *pucTempOutData++);

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                NIPACKER_UINT32_TO_UINT8(0, *pucTempOutData++);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pulInData = (unsigned long*)(pucInData);
            unsigned char* pucTempOutData = pucOutData;

            // Bitwise, UINT8 and UNORM8 are identical, so pack using UINT8.
            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                NIPACKER_UINT32_TO_UINT8(*pulInData++, *pucTempOutData++);

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                NIPACKER_UINT32_TO_UINT8(0, *pucTempOutData++);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryPacker::Pack_SInt16ToUNorm8(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    SHORT* psInData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_SHORT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_SHORT4);
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_NORMUBYTE4);

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
            psInData = (SHORT*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);
            unsigned char* pucTempOutData = pucOutData;

            // Bitwise, UINT8 and UNORM8 are identical, so pack using UINT8.
            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                NIPACKER_SINT16_TO_UINT8(*psInData++, *pucTempOutData++);

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                NIPACKER_SINT16_TO_UINT8(0, *pucTempOutData++);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            psInData = (SHORT*)(pucInData);
            unsigned char* pucTempOutData = pucOutData;

            // Bitwise, UINT8 and UNORM8 are identical, so pack using UINT8.
            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                NIPACKER_SINT16_TO_UINT8(*psInData++, *pucTempOutData++);

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                NIPACKER_SINT16_TO_UINT8(0, *pucTempOutData++);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryPacker::Pack_UInt16ToUNorm8(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned short* pusInData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_USHORT1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_USHORT4);
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_NORMUBYTE4);

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
            unsigned char* pucTempOutData = pucOutData;

            // Bitwise, UINT8 and UNORM8 are identical, so pack using UINT8.
            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                NIPACKER_UINT16_TO_UINT8(*pusInData++, *pucTempOutData++);

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                NIPACKER_UINT16_TO_UINT8(0, *pucTempOutData++);

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pusInData = (unsigned short*)(pucInData);
            unsigned char* pucTempOutData = pucOutData;

            // Bitwise, UINT8 and UNORM8 are identical, so pack using UINT8.
            unsigned short usPack = 0;
            for (; usPack < usPackCount; usPack++)
                NIPACKER_UINT16_TO_UINT8(*pusInData++, *pucTempOutData++);

            //  Fill empty outs, if needed
            for (; usPack < usOutUBytes; usPack++)
                NIPACKER_UINT16_TO_UINT8(0, *pucTempOutData++);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryPacker::Pack_SInt8ToUNorm8(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    BYTE* pbyInData;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_BYTE1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_BYTE4);
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_NORMUBYTE4);

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

            // Bitwise, UINT8 and UNORM8 are identical, so pack using UINT8.
            memcpy(pucOutData, pbyInData, 
                usPackCount * sizeof(unsigned char));

            unsigned char* pucTempOutData = 
                (unsigned char*)pucOutData + usPackCount;

            //  Fill empty outs, if needed
            unsigned short usPack = usPackCount;
            for (; usPack < usOutUBytes; usPack++)
                *pucTempOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            pbyInData = (BYTE*)(pucInData);

            // Bitwise, UINT8 and UNORM8 are identical, so pack using UINT8.
            memcpy(pucOutData, pbyInData, 
                usPackCount * sizeof(unsigned char));

            unsigned char* pucTempOutData = 
                (unsigned char*)pucOutData + usPackCount;

            //  Fill empty outs, if needed
            unsigned short usPack = usPackCount;
            for (; usPack < usOutUBytes; usPack++)
                *pucTempOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryPacker::Pack_UInt8ToUNorm8(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;

    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);
    unsigned short* pusVertMap = kParams.m_pusVertexMap;

    //  Safety check!
    NIASSERT(kParams.m_uiDataType >= 
        NiAdditionalGeometryData::AGD_NITYPE_UBYTE1);
    NIASSERT(kParams.m_uiDataType <= 
        NiAdditionalGeometryData::AGD_NITYPE_UBYTE4);
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_NORMUBYTE4);

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
            unsigned char* pucTempInData = (unsigned char*)
                &(pucInData[pusVertMap[usMapIndex++] * kParams.m_uiInStride]);

            // Bitwise, UINT8 and UNORM8 are identical, so pack using UINT8.
            memcpy(pucOutData, pucTempInData, 
                usPackCount * sizeof(unsigned char));

            unsigned char* pucTempOutData = 
                (unsigned char*)pucOutData + usPackCount;

            //  Fill empty outs, if needed
            unsigned short usPack = usPackCount;
            for (; usPack < usOutUBytes; usPack++)
                *pucTempOutData++ = 0;

            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }
    else
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            // Bitwise, UINT8 and UNORM8 are identical, so pack using UINT8.
            memcpy(pucOutData, pucInData, 
                usPackCount * sizeof(unsigned char));

            unsigned char* pucTempOutData = 
                (unsigned char*)pucOutData + usPackCount;

            //  Fill empty outs, if needed
            unsigned short usPack = usPackCount;
            for (; usPack < usOutUBytes; usPack++)
                *pucTempOutData++ = 0;

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Short output packers
//---------------------------------------------------------------------------
// Signed shorts map to the range [-32768, 32767]
unsigned int NiD3D10GeometryPacker::Pack_Float32ToSInt16(
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
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_SHORT4);

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
        (kParams.m_ePackType == NiShaderDeclaration::SPTYPE_SHORT2) ? 2 : 4;

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
                NIPACKER_FLOAT32_TO_SINT16(*pfInData, *psOutData);
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
                NIPACKER_FLOAT32_TO_SINT16(*pfInData, *psOutData);
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
unsigned int NiD3D10GeometryPacker::Pack_SInt32ToSInt16(
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
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_SHORT4);

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
        (kParams.m_ePackType == NiShaderDeclaration::SPTYPE_SHORT2) ? 2 : 4;

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
                NIPACKER_SINT32_TO_SINT16(*plInData, *psOutData);
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
                NIPACKER_SINT32_TO_SINT16(*plInData, *psOutData);
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
unsigned int NiD3D10GeometryPacker::Pack_UInt32ToSInt16(
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
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_SHORT4);

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
        (kParams.m_ePackType == NiShaderDeclaration::SPTYPE_SHORT2) ? 2 : 4;

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
                NIPACKER_UINT32_TO_SINT16(*pulInData, *psOutData);
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
                NIPACKER_UINT32_TO_SINT16(*pulInData, *psOutData);
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
unsigned int NiD3D10GeometryPacker::Pack_SInt16ToSInt16(
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
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_SHORT4);

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
        (kParams.m_ePackType == NiShaderDeclaration::SPTYPE_SHORT2) ? 2 : 4;

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
unsigned int NiD3D10GeometryPacker::Pack_UInt16ToSInt16(
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
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_SHORT4);

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
        (kParams.m_ePackType == NiShaderDeclaration::SPTYPE_SHORT2) ? 2 : 4;

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
unsigned int NiD3D10GeometryPacker::Pack_SInt8ToSInt16(
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
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_SHORT4);

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
        (kParams.m_ePackType == NiShaderDeclaration::SPTYPE_SHORT2) ? 2 : 4;

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
                NIPACKER_SINT8_TO_SINT16(*pcInData, *psOutData);
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
                NIPACKER_SINT8_TO_SINT16(*pcInData, *psOutData);
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
unsigned int NiD3D10GeometryPacker::Pack_UInt8ToSInt16(
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
    NIASSERT(kParams.m_ePackType >= NiShaderDeclaration::SPTYPE_SHORT2);
    NIASSERT(kParams.m_ePackType <= NiShaderDeclaration::SPTYPE_SHORT4);

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
        (kParams.m_ePackType == NiShaderDeclaration::SPTYPE_SHORT2) ? 2 : 4;

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
                NIPACKER_UINT8_TO_SINT16(*pucInDataTemp, *psOutData);
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
                NIPACKER_UINT8_TO_SINT16(*pucInDataTemp, *psOutData);
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
//-----------------------------------------------------------------------
//  Unsigned byte output packers
//---------------------------------------------------------------------------
unsigned int NiD3D10GeometryPacker::Pack_Float32ToUInt8(
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
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTE4);

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
                NIPACKER_FLOAT32_TO_UINT8(*pfInData, *pbyOutData);
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
                NIPACKER_FLOAT32_TO_UINT8(*pfInData, *pbyOutData);
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
unsigned int NiD3D10GeometryPacker::Pack_SInt32ToUInt8(
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
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTE4);

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
                NIPACKER_SINT32_TO_UINT8(*plInData, *pbyOutData);
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
                NIPACKER_SINT32_TO_UINT8(*plInData, *pbyOutData);
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
unsigned int NiD3D10GeometryPacker::Pack_UInt32ToUInt8(
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
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTE4);

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
                NIPACKER_UINT32_TO_UINT8(*pulInData, *pbyOutData);
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
                NIPACKER_UINT32_TO_UINT8(*pulInData, *pbyOutData);
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
unsigned int NiD3D10GeometryPacker::Pack_SInt16ToUInt8(
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
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTE4);

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
                NIPACKER_SINT16_TO_UINT8(*psInData, *pbyOutData);
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
                NIPACKER_SINT16_TO_UINT8(*psInData, *pbyOutData);
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
unsigned int NiD3D10GeometryPacker::Pack_UInt16ToUInt8(
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
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTE4);

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
                NIPACKER_UINT16_TO_UINT8(*pusInData, *pbyOutData);
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
                NIPACKER_UINT16_TO_UINT8(*pusInData, *pbyOutData);
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
unsigned int NiD3D10GeometryPacker::Pack_SInt8ToUInt8(
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
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTE4);

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
unsigned int NiD3D10GeometryPacker::Pack_UInt8ToUInt8(
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
    NIASSERT(kParams.m_ePackType == NiShaderDeclaration::SPTYPE_UBYTE4);

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
