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
#include "NiD3DShaderDeclaration.h"
#include "NiD3DRendererHeaders.h"
#include "NiVBChip.h"

#include <NiGeometryData.h>
#include <NiMemManager.h>
#include <NiSkinInstance.h>

//---------------------------------------------------------------------------
void NiD3DShaderDeclaration::_SDMInit()
{
    ms_pkPackFunctions = NiNew NiTPrimitiveArray<NiPackerEntry*>;

    InitializePackingFunctions();
}
//---------------------------------------------------------------------------
void NiD3DShaderDeclaration::_SDMShutdown()
{
    for (unsigned int i = 0; i < ms_pkPackFunctions->GetSize(); i++)
    {
        NiDelete ms_pkPackFunctions->GetAt(i);
    }
    ms_pkPackFunctions->RemoveAll();
    NiDelete ms_pkPackFunctions;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::ms_auiTypeSizes[SPTYPE_COUNT];
    NiTPrimitiveArray<NiD3DShaderDeclaration::NiPackerEntry*>* 
    NiD3DShaderDeclaration::ms_pkPackFunctions;
//---------------------------------------------------------------------------
NiD3DShaderDeclaration::NiD3DShaderDeclaration() :
    NiShaderDeclaration(), 
    m_pkRenderer(NULL),
    m_pkVBManager(NULL),
    m_pkD3DDevice(NULL),
    m_uiDeclSize(0),
    m_uiCurrDeclIndex(0)
{ /* */ }
//---------------------------------------------------------------------------
NiD3DShaderDeclaration::~NiD3DShaderDeclaration()
{
    m_pkRenderer = 0;
    NiD3DRenderer::ReleaseDevice(m_pkD3DDevice);
    m_pkD3DDevice = 0;
}
//---------------------------------------------------------------------------
NiD3DShaderDeclaration::Packer 
NiD3DShaderDeclaration::RegisterPackingFunction(
    ShaderParameterType eType, unsigned int uiAGDType, Packer pfnPack)
{
    Packer pfnOld = 0;

    NIASSERT(pfnPack);

    NIASSERT(uiAGDType < NiAdditionalGeometryData::AGD_NITYPE_COUNT);

    //  Test it...
    NiD3DShaderDeclaration::NiPackerEntry* pkPackEntry = 
        NiD3DShaderDeclaration::ms_pkPackFunctions->GetAt(
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
            NiD3DShaderDeclaration::SetDefaultPackingFunction(eType, 
                uiAGDType);
        }
    }

    return pfnOld;
}
//---------------------------------------------------------------------------
bool NiD3DShaderDeclaration::InitializePackingFunctions()
{
    unsigned int uiSize = 
        NiD3DShaderDeclaration::ms_pkPackFunctions->GetSize();
    if (uiSize == NiD3DShaderDeclaration::SPTYPE_COUNT)
    {
        //  Already initialized...
        return true;
    }

    //  Fill in the packing sizes...
    unsigned int* puiSizes = NiD3DShaderDeclaration::ms_auiTypeSizes;

    puiSizes[SPTYPE_FLOAT1] = sizeof(float) * 1;
    puiSizes[SPTYPE_FLOAT2] = sizeof(float) * 2;
    puiSizes[SPTYPE_FLOAT3] = sizeof(float) * 3;
    puiSizes[SPTYPE_FLOAT4] = sizeof(float) * 4;
    puiSizes[SPTYPE_UBYTECOLOR] = sizeof(unsigned int);
    puiSizes[SPTYPE_UBYTE4] = sizeof(unsigned char) * 4;
    puiSizes[SPTYPE_SHORT2] = sizeof(short) * 2;
    puiSizes[SPTYPE_SHORT4] = sizeof(short) * 4;
    puiSizes[SPTYPE_NORMUBYTE4] = sizeof(unsigned char) * 4;
    puiSizes[SPTYPE_NORMSHORT2] = sizeof(short) * 2;
    puiSizes[SPTYPE_NORMSHORT4] = sizeof(short) * 4;
    puiSizes[SPTYPE_NORMUSHORT2] = sizeof(unsigned short) * 2;
    puiSizes[SPTYPE_NORMUSHORT4] = sizeof(unsigned short) * 4;
    puiSizes[SPTYPE_UDEC3] = sizeof(unsigned int); // 32-bit value
    puiSizes[SPTYPE_NORMDEC3] = sizeof(unsigned int); // 32-bit value
    puiSizes[SPTYPE_FLOAT16_2] = sizeof(unsigned short) * 2; // 32-bit value
    puiSizes[SPTYPE_FLOAT16_4] = sizeof(unsigned short) * 4; // 64-bit value

    //  Create it
    for (unsigned int ui = 0; ui < SPTYPE_COUNT; ui++)
    {
        NiD3DShaderDeclaration::NiPackerEntry* pkPackEntry = 
            NiNew NiD3DShaderDeclaration::NiPackerEntry();
        NIASSERT(pkPackEntry != 0);

        pkPackEntry->m_uiShaderParameterType = ui;
        pkPackEntry->m_kPackFunctions.SetSize(
            NiAdditionalGeometryData::AGD_NITYPE_COUNT + 5);

        NiD3DShaderDeclaration::ms_pkPackFunctions->SetAtGrow(ui, pkPackEntry);
    }

    //  Fill it...
    SetDefaultPackingFunctions();

    return true;
}
//---------------------------------------------------------------------------
NiD3DVertexDeclaration NiD3DShaderDeclaration::GetD3DDeclaration()
{
    // Base implementation returns 0
    return 0;
}
//---------------------------------------------------------------------------
bool NiD3DShaderDeclaration::Initialize(NiRenderer* pkRenderer, 
    unsigned int uiMaxStreamEntryCount, unsigned int uiStreamCount)
{
    NIASSERT(NiIsKindOf(NiD3DRenderer, pkRenderer));
    m_pkRenderer = (NiD3DRenderer*)pkRenderer;
    m_pkVBManager = m_pkRenderer->GetVBManager();
    m_pkD3DDevice = m_pkRenderer->GetD3DDevice();
    NIASSERT(m_pkD3DDevice);
    D3D_POINTER_REFERENCE(m_pkD3DDevice);

    m_uiMaxStreamEntryCount = uiMaxStreamEntryCount;
    m_uiStreamCount = uiStreamCount;

    NIASSERT(m_uiMaxStreamEntryCount > 0);
    NIASSERT(m_uiStreamCount > 0);

    m_pkStreamEntries = NiNew ShaderRegisterStream[m_uiStreamCount];
    NIASSERT(m_pkStreamEntries);

    for (unsigned int ui = 0; ui < m_uiStreamCount; ui++)
    {
        m_pkStreamEntries[ui].m_pkEntries = 
            NiNew ShaderRegisterEntry[m_uiMaxStreamEntryCount];
        NIASSERT(m_pkStreamEntries[ui].m_pkEntries);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3DShaderDeclaration::SetConstantData(unsigned int, unsigned int,
    float*)
{
    // Do nothing by default
    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::PackEntry(unsigned short usCount, 
    NiGeometryData* pkData, const NiSkinInstance* pkSkin, 
    NiSkinPartition::Partition* pkPartition, ShaderRegisterEntry* pkEntry,
    unsigned int uiStride, unsigned char* pucData)
{
    NIASSERT(pkEntry);
    NIASSERT(pucData);

    NiD3DShaderDeclaration::NiPackerEntry* pkPackEntry;
    NiD3DShaderDeclaration::PackingParameters kPackParams;
    Packer pfnPack;

    kPackParams.m_usCount = usCount;
    kPackParams.m_uiOutStride = uiStride;
    kPackParams.m_pvOutData = (void*)pucData;

    switch (pkEntry->m_eType)
    {
    case SPTYPE_NONE:
        return 0;
    }

    NiShaderDeclaration::ShaderParameterType eType = pkEntry->m_eType;

    // Hack alert! If it's asking for BlendIndices, pack to UByte4 
    //   rather than UByteColor!
    if (eType == NiShaderDeclaration::SPTYPE_UBYTECOLOR &&
        pkEntry->m_eInput == NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES)
    {
        eType = NiShaderDeclaration::SPTYPE_UBYTE4;
    }

    kPackParams.m_uiOutSize = 
        NiD3DShaderDeclaration::ms_auiTypeSizes[eType];

    pkPackEntry = NiD3DShaderDeclaration::ms_pkPackFunctions->GetAt(eType);
    if (pkPackEntry == 0)
    {
        NIASSERT(0);
        return 0;
    }

    NiAdditionalGeometryData* pkExtraData = 
        pkData->GetAdditionalGeometryData();

    //  Need to use the data type to grab the actual packing function
    unsigned int uiDataType;

    switch (pkEntry->m_eInput)
    {
    case SHADERPARAM_INVALID:
        {
            NIASSERT(0);
            return 0;
        }
        break;
    case SHADERPARAM_NI_POSITION:
        kPackParams.m_pvInData = (void*)pkData->GetVertices();
        kPackParams.m_uiInSize = sizeof(NiPoint3);
        kPackParams.m_uiInStride = kPackParams.m_uiInSize;
        uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT3;
        break;
    case SHADERPARAM_NI_NORMAL:
        kPackParams.m_pvInData = (void*)pkData->GetNormals();
        kPackParams.m_uiInSize = sizeof(NiPoint3);
        kPackParams.m_uiInStride = kPackParams.m_uiInSize;
        uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT3;
        break;
    case SHADERPARAM_NI_BLENDWEIGHT:
        {
            if (pkPartition == 0)
            {
                //  No partition - no bones...
                NIASSERT(0);
                return 0;
            };

            //  Determine the vertex format (w/ Blend Weights)
            NIASSERT(pkPartition->m_usBonesPerVertex >= 1);
            NIASSERT(pkPartition->m_usBonesPerVertex <= 
                NiD3DRenderer::HW_BONE_LIMIT);

            //  Determine the weights (and FVF flag)...
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
        }
        break;
    case SHADERPARAM_NI_BLENDINDICES:
        {
            if (pkPartition == 0)
            {
                //  No partition - no bones...
                NIASSERT(0);
                return 0;
            }

            if (pkPartition->m_pucBonePalette == 0)
            {
                //  No indices
                NIASSERT(0);
                return 0;
            }

            //  Make sure we are clear here...
            NIASSERT(pkPartition->m_usBonesPerVertex >= 1);
            NIASSERT(pkPartition->m_usBonesPerVertex <= 
                NiD3DRenderer::HW_BONE_LIMIT);

            //  Determine the bone index count (and FVF flag)...
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
        }
        break;
    case SHADERPARAM_NI_COLOR:
        kPackParams.m_pvInData = (void*)pkData->GetColors();
        kPackParams.m_uiInSize = sizeof(NiColorA);
        kPackParams.m_uiInStride = kPackParams.m_uiInSize;
        uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT4;
        break;
    case SHADERPARAM_NI_TEXCOORD0:
    case SHADERPARAM_NI_TEXCOORD1:
    case SHADERPARAM_NI_TEXCOORD2:
    case SHADERPARAM_NI_TEXCOORD3:
    case SHADERPARAM_NI_TEXCOORD4:
    case SHADERPARAM_NI_TEXCOORD5:
    case SHADERPARAM_NI_TEXCOORD6:
    case SHADERPARAM_NI_TEXCOORD7:
        kPackParams.m_pvInData = (void*)pkData->GetTextureSet(
            pkEntry->m_eInput - SHADERPARAM_NI_TEXCOORD0);
        kPackParams.m_uiInSize = sizeof(NiPoint2);
        kPackParams.m_uiInStride = kPackParams.m_uiInSize;
        uiDataType = NiAdditionalGeometryData::AGD_NITYPE_FLOAT2;
        break;
    case SHADERPARAM_NI_BINORMAL:
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
    case SHADERPARAM_NI_TANGENT:
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
        {
            if (pkEntry->m_eInput & SHADERPARAM_EXTRA_DATA_MASK)
            {
                unsigned int uiStream = pkEntry->m_eInput & 
                    ~SHADERPARAM_EXTRA_DATA_MASK;
                
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
                    uiDataType = NiAdditionalGeometryData::AGD_NITYPE_INVALID;
                }
            }
        }
        break;
    }

    pfnPack = pkPackEntry->m_kPackFunctions.GetAt(uiDataType);
    if (pfnPack == 0)
    {
        //  No packing function for this combination!
        //  Probably don't want to assert, just 'fail'.
        NIASSERT(0);
        return 0;
    }

    if (pkPartition && (pkEntry->m_eInput != SHADERPARAM_NI_BLENDWEIGHT) &&
        (pkEntry->m_eInput != SHADERPARAM_NI_BLENDINDICES))
        kPackParams.m_pusVertexMap = pkPartition->m_pusVertexMap;
    else
        kPackParams.m_pusVertexMap = 0;

    kPackParams.m_ePackType = eType;
    kPackParams.m_uiDataType = uiDataType;

    return pfnPack(kPackParams);
}
//---------------------------------------------------------------------------
void NiD3DShaderDeclaration::SetDefaultPackingFunctions()
{
    unsigned int uiSPType;
    unsigned int uiAGDType;

    for (uiSPType = SPTYPE_FLOAT1; uiSPType < SPTYPE_COUNT; uiSPType++)
    {
        for (uiAGDType = NiAdditionalGeometryData::AGD_NITYPE_INVALID;
            uiAGDType < NiAdditionalGeometryData::AGD_NITYPE_COUNT; 
            uiAGDType++)
        {
            SetDefaultPackingFunction((ShaderParameterType)uiSPType,
                uiAGDType);
        }
    }
}

//---------------------------------------------------------------------------
//  Default data packing functions
//---------------------------------------------------------------------------
#define _PACK_ILLEGALCOMBINATION_ASSERT_
//#define _PACK_ILLEGALCOMBINATION_ZERO_
//#define _PACK_ILLEGALCOMBINATION_NOTHING_
//---------------------------------------------------------------------------
unsigned int NiD3DShaderDeclaration::Pack_IllegalCombination(
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
//  Packing an equal number of equal type ins to out...
unsigned int NiD3DShaderDeclaration::Pack_DirectMapping(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);

    //  Assumes that the in and out sizes are the same!
    NIASSERT(kParams.m_uiInSize == kParams.m_uiOutSize);
    NIASSERT(kParams.m_pusVertexMap == 0);

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

    for (unsigned short us = 0; us < kParams.m_usCount; us++)
    {
        NiMemcpy(pucOutData, pucInData, kParams.m_uiInSize);

        pucInData += kParams.m_uiInStride;
        pucOutData += kParams.m_uiOutStride;
        uiPacked += kParams.m_uiOutSize;
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Packing a greater number of in parameters into the out fields.
//  Drop the additional in values.
//  These MUST be the same type.
unsigned int NiD3DShaderDeclaration::Pack_DropInFields(
    PackingParameters& kParams)
{
    unsigned int uiPacked = 0;
    unsigned char* pucInData = (unsigned char*)(kParams.m_pvInData);
    unsigned char* pucOutData = (unsigned char*)(kParams.m_pvOutData);

    //  Assumes that the in size is greater than the out size are the same!
    NIASSERT(kParams.m_uiInSize > kParams.m_uiOutSize);
    NIASSERT(kParams.m_pusVertexMap == 0);

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

    for (unsigned short us = 0; us < kParams.m_usCount; us++)
    {
        //  Drop the additional input fields...
        NiMemcpy(pucOutData, pucInData, kParams.m_uiOutSize);

        pucInData += kParams.m_uiInStride;
        pucOutData += kParams.m_uiOutStride;
        uiPacked += kParams.m_uiOutSize;
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Float output packers
//---------------------------------------------------------------------------
//  Packing in > out floats, dropping additional ins
unsigned int NiD3DShaderDeclaration::Pack_Float_DropIns(
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
    NIASSERT(kParams.m_ePackType >= SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= SPTYPE_FLOAT4);

    //  We need to check the out size vs the in size...
    unsigned short usInFloats = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 + 1;
    unsigned short usOutFloats = kParams.m_ePackType - SPTYPE_FLOAT1 + 1;

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
unsigned int NiD3DShaderDeclaration::Pack_Float_ExtraOutsToZero(
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
    NIASSERT(kParams.m_ePackType >= SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= SPTYPE_FLOAT4);

    //  We need to check the out size vs the in size...
    unsigned short usInFloats = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 + 1;
    unsigned short usOutFloats = kParams.m_ePackType - SPTYPE_FLOAT1 + 1;
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
unsigned int NiD3DShaderDeclaration::Pack_Float_ExtraOutsToOne(
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
    NIASSERT(kParams.m_ePackType >= SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= SPTYPE_FLOAT4);

    //  We need to check the out size vs the in size...
    unsigned short usInFloats = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 + 1;
    unsigned short usOutFloats = kParams.m_ePackType - SPTYPE_FLOAT1 + 1;
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
unsigned int NiD3DShaderDeclaration::Pack_FloatToFloat(
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
    NIASSERT(kParams.m_ePackType >= SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= SPTYPE_FLOAT4);

    //  We need to check the out size vs the in size...
    unsigned short usInFloats = kParams.m_uiDataType - 
        NiAdditionalGeometryData::AGD_NITYPE_FLOAT1 + 1;
    unsigned short usOutFloats = kParams.m_ePackType - SPTYPE_FLOAT1 + 1;
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
//  Uses the macro NIPACKER_LONG_TO_FLOAT
unsigned int NiD3DShaderDeclaration::Pack_LongToFloat(
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
    NIASSERT(kParams.m_ePackType >= SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= SPTYPE_FLOAT4);

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
    unsigned short usOutFloats = kParams.m_ePackType - SPTYPE_FLOAT1 + 1;

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
                NIPACKER_LONG_TO_FLOAT(*plInData, *pfOutData);
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
                NIPACKER_LONG_TO_FLOAT(*plInData, *pfOutData);
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
//  Uses the macro NIPACKER_ULONG_TO_FLOAT
unsigned int NiD3DShaderDeclaration::Pack_UnsignedLongToFloat(
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
    NIASSERT(kParams.m_ePackType >= SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= SPTYPE_FLOAT4);

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
    unsigned short usOutFloats = kParams.m_ePackType - SPTYPE_FLOAT1 + 1;
    
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
                NIPACKER_ULONG_TO_FLOAT(*pulInData, *pfOutData);
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
                NIPACKER_ULONG_TO_FLOAT(*pulInData, *pfOutData);
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
//  Uses the macro NIPACKER_SHORT_TO_FLOAT
unsigned int NiD3DShaderDeclaration::Pack_ShortToFloat(
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
    NIASSERT(kParams.m_ePackType >= SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= SPTYPE_FLOAT4);

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
    unsigned short usOutFloats = kParams.m_ePackType - SPTYPE_FLOAT1 + 1;
    
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
                NIPACKER_SHORT_TO_FLOAT(*psInData, *pfOutData);
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
                NIPACKER_SHORT_TO_FLOAT(*psInData, *pfOutData);
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
//  Uses the macro NIPACKER_USHORT_TO_FLOAT
unsigned int NiD3DShaderDeclaration::Pack_UnsignedShortToFloat(
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
    NIASSERT(kParams.m_ePackType >= SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= SPTYPE_FLOAT4);

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
    unsigned short usOutFloats = kParams.m_ePackType - SPTYPE_FLOAT1 + 1;
    
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
                NIPACKER_USHORT_TO_FLOAT(*pusInData, *pfOutData);
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
                NIPACKER_USHORT_TO_FLOAT(*pusInData, *pfOutData);
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
//  Uses the macro NIPACKER_BYTE_TO_FLOAT
unsigned int NiD3DShaderDeclaration::Pack_ByteToFloat(
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
    NIASSERT(kParams.m_ePackType >= SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= SPTYPE_FLOAT4);

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
    unsigned short usOutFloats = kParams.m_ePackType - SPTYPE_FLOAT1 + 1;
    
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
                NIPACKER_BYTE_TO_FLOAT(*pcInData, *pfOutData);
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
                NIPACKER_BYTE_TO_FLOAT(*pcInData, *pfOutData);
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
//  Uses the macro NIPACKER_UBYTE_TO_FLOAT
unsigned int NiD3DShaderDeclaration::Pack_UnsignedByteToFloat(
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
    NIASSERT(kParams.m_ePackType >= SPTYPE_FLOAT1);
    NIASSERT(kParams.m_ePackType <= SPTYPE_FLOAT4);

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
    unsigned short usOutFloats = kParams.m_ePackType - SPTYPE_FLOAT1 + 1;
    
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
                NIPACKER_UBYTE_TO_FLOAT(*pucInDataTemp, *pfOutData);
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
                NIPACKER_UBYTE_TO_FLOAT(*pucInDataTemp, *pfOutData);
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
unsigned int NiD3DShaderDeclaration::Pack_FloatToUByteColor(
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
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTECOLOR);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            puiOutData = (unsigned int*)(pucOutData);
            NIPACKER_FLOAT_TO_UBYTECOLOR(1.0f, 1.0f, 1.0f, 1.0f, 
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

            NIPACKER_FLOAT_TO_UBYTECOLOR(fAlpha, pfInData[0], pfInData[1], 
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

            NIPACKER_FLOAT_TO_UBYTECOLOR(fAlpha, pfInData[0], pfInData[1], 
                pfInData[2], *puiOutData);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Long - assumes one long per color (already calculated...)
unsigned int NiD3DShaderDeclaration::Pack_LongToUByteColor(
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
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTECOLOR);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            puiOutData = (unsigned int*)pucOutData;
            NIPACKER_LONG_TO_UBYTECOLOR(0xffffffff, *puiOutData);
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

            NIPACKER_LONG_TO_UBYTECOLOR(*plInData, *puiOutData);

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

            NIPACKER_LONG_TO_UBYTECOLOR(*plInData, *puiOutData);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
//  Long - assumes one long per color (already calculated...)
unsigned int NiD3DShaderDeclaration::Pack_UnsignedLongToUByteColor(
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
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTECOLOR);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            puiOutData = (unsigned int*)pucOutData;
            NIPACKER_LONG_TO_UBYTECOLOR(0xffffffff, *puiOutData);
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

            NIPACKER_ULONG_TO_UBYTECOLOR(*pulInData, *puiOutData);

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

            NIPACKER_ULONG_TO_UBYTECOLOR(*pulInData, *puiOutData);

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
unsigned int NiD3DShaderDeclaration::Pack_ShortToUByteColor(
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
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTECOLOR);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            puiOutData = (unsigned int*)(pucOutData);
            NIPACKER_SHORT_TO_UBYTECOLOR(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 
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

            NIPACKER_SHORT_TO_UBYTECOLOR(sAlpha, psInData[0], psInData[1], 
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
            NIPACKER_SHORT_TO_UBYTECOLOR(sAlpha, psInData[0], psInData[1], 
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
unsigned int NiD3DShaderDeclaration::Pack_UnsignedShortToUByteColor(
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
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTECOLOR);

    if (pucInData == 0)
    {
        for (unsigned short us = 0; us < kParams.m_usCount; us++)
        {
            puiOutData = (unsigned int*)(pucOutData);
            NIPACKER_USHORT_TO_UBYTECOLOR(0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 
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

            NIPACKER_USHORT_TO_UBYTECOLOR(usAlpha, pusInData[0], pusInData[1], 
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

            NIPACKER_USHORT_TO_UBYTECOLOR(usAlpha, pusInData[0], pusInData[1],
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
unsigned int NiD3DShaderDeclaration::Pack_ByteToUByteColor(
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
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTECOLOR);

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

            NIPACKER_BYTE_TO_UBYTECOLOR(cAlpha, pcInData[0], pcInData[1], 
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

            NIPACKER_BYTE_TO_UBYTECOLOR(cAlpha, pcInData[0], pcInData[1], 
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
unsigned int NiD3DShaderDeclaration::Pack_UnsignedByteToUByteColor(
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
    NIASSERT(kParams.m_ePackType == SPTYPE_UBYTECOLOR);

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

            NIPACKER_BYTE_TO_UBYTECOLOR(ucAlpha, pucInDataTemp[0], 
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

            NIPACKER_BYTE_TO_UBYTECOLOR(ucAlpha, pucInDataTemp[0], 
                pucInDataTemp[1], pucInDataTemp[2], *puiOutData);

            pucInData += kParams.m_uiInStride;
            pucOutData += kParams.m_uiOutStride;
            uiPacked += kParams.m_uiOutSize;
        }
    }

    return uiPacked;
}
//---------------------------------------------------------------------------
