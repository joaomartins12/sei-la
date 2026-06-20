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
#include "NiMainPCH.h"
#include "NiGeometry.h"
#include "NiMaterialInstance.h"
#include "NiMath.h"
#include "NiShaderDeclaration.h"

NiFixedString NiShaderDeclaration::ms_akUsageStrings[SPUSAGE_COUNT];

NiImplementRTTI(NiShaderDeclaration, NiObject);
//---------------------------------------------------------------------------
NiShaderDeclaration::NiShaderDeclaration() :
    m_uiMaxStreamEntryCount(0),
    m_uiStreamCount(0),
    m_pkStreamEntries(NULL),
    m_bModified(false)
{
}
//---------------------------------------------------------------------------
NiShaderDeclaration::~NiShaderDeclaration()
{
    if (m_pkStreamEntries)
    {
        for (unsigned int ui = 0; ui < m_uiStreamCount; ui++)
        {
            ShaderRegisterStream* pkStream = &(m_pkStreamEntries[ui]);
            if (pkStream)
                NiDelete [] pkStream->m_pkEntries;
        }
        NiDelete[] m_pkStreamEntries;
    }
    m_pkStreamEntries = 0;
}
//---------------------------------------------------------------------------
NiShaderDeclaration::ShaderRegisterEntry::ShaderRegisterEntry() :
    m_uiPackingOffset(0),
    m_eInput(SHADERPARAM_INVALID),
    m_eType(SPTYPE_NONE),
    m_uiUsageIndex(0),
    m_uiExtraData(0)
{
    m_kUsage = UsageToString(SPUSAGE_POSITION);
}
//---------------------------------------------------------------------------
void NiShaderDeclaration::_SDMInit()
{
    // Establish the mapping table.
    ms_akUsageStrings[SPUSAGE_POSITION] = NiFixedString("POSITION");
    ms_akUsageStrings[SPUSAGE_BLENDWEIGHT] = NiFixedString("BLENDWEIGHT");
    ms_akUsageStrings[SPUSAGE_BLENDINDICES] = NiFixedString("BLENDINDICES");
    ms_akUsageStrings[SPUSAGE_NORMAL] = NiFixedString("NORMAL");
    ms_akUsageStrings[SPUSAGE_PSIZE] = NiFixedString("PSIZE");
    ms_akUsageStrings[SPUSAGE_TEXCOORD] = NiFixedString("TEXCOORD");
    ms_akUsageStrings[SPUSAGE_TANGENT] = NiFixedString("TANGENT");
    ms_akUsageStrings[SPUSAGE_BINORMAL] = NiFixedString("BINORMAL");
    ms_akUsageStrings[SPUSAGE_TESSFACTOR] = NiFixedString("TESSFACTOR");
    ms_akUsageStrings[SPUSAGE_POSITIONT] = NiFixedString("POSITIONT");
    ms_akUsageStrings[SPUSAGE_COLOR] = NiFixedString("COLOR");
    ms_akUsageStrings[SPUSAGE_FOG] = NiFixedString("FOG");
    ms_akUsageStrings[SPUSAGE_DEPTH] = NiFixedString("DEPTH");
    ms_akUsageStrings[SPUSAGE_SAMPLE] = NiFixedString("SAMPLE");
}
//---------------------------------------------------------------------------
void NiShaderDeclaration::_SDMShutdown()
{
    for (unsigned int ui = 0; ui < SPUSAGE_COUNT; ++ui)
    {
        ms_akUsageStrings[ui] = 0;
    }
}
//---------------------------------------------------------------------------
bool NiShaderDeclaration::SetEntry(unsigned int uiEntry, 
    ShaderParameter eInput, ShaderParameterType eType, unsigned int uiStream)
{
    ShaderParameterUsage eUsage;
    unsigned int uiIndex = 0; 

    switch (eInput)
    {
    case SHADERPARAM_NI_POSITION:
        eUsage = SPUSAGE_POSITION;
        break;
    case SHADERPARAM_NI_BLENDWEIGHT:
        eUsage = SPUSAGE_BLENDWEIGHT;
        break;
    case SHADERPARAM_NI_BLENDINDICES:
        eUsage = SPUSAGE_BLENDINDICES;
        break;
    case SHADERPARAM_NI_NORMAL:
        eUsage = SPUSAGE_NORMAL;
        break;
    case SHADERPARAM_NI_COLOR:
        eUsage = SPUSAGE_COLOR;
        break;
    case SHADERPARAM_NI_TEXCOORD0:
        eUsage = SPUSAGE_TEXCOORD;
        break;
    case SHADERPARAM_NI_TEXCOORD1:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 1;
        break;
    case SHADERPARAM_NI_TEXCOORD2:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 2;
        break;
    case SHADERPARAM_NI_TEXCOORD3:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 3;
        break;
    case SHADERPARAM_NI_TEXCOORD4:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 4;
        break;
    case SHADERPARAM_NI_TEXCOORD5:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 5;
        break;
    case SHADERPARAM_NI_TEXCOORD6:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 6;
        break;
    case SHADERPARAM_NI_TEXCOORD7:
        eUsage = SPUSAGE_TEXCOORD;
        uiIndex = 7;
        break;
    case SHADERPARAM_NI_TANGENT:
        eUsage = SPUSAGE_TANGENT;
        break;
    case SHADERPARAM_NI_BINORMAL:
        eUsage = SPUSAGE_BINORMAL;
        break;
    default:
        NIASSERT(!"Unsupported ShaderParameter type - "
            "try other SetEntry function");
        return false;
    }

    return SetEntry(uiStream, uiEntry, eInput, eType, UsageToString(eUsage), 
        uiIndex, 0);
}
//---------------------------------------------------------------------------
bool NiShaderDeclaration::SetEntry(unsigned int uiStream,
    unsigned int uiEntry, ShaderParameter eInput,
    ShaderParameterType eType, const NiFixedString& kUsage,
    unsigned int uiIndex, unsigned int uiExtraData)
{
    NIASSERT(m_pkStreamEntries);
    
    //  Check entry slot
    if (uiEntry >= m_uiMaxStreamEntryCount)
        return false;

    //  Check stream validity
    if (uiStream >= m_uiStreamCount)
        return false;

    //  Set it
    ShaderRegisterStream* pkStream = &(m_pkStreamEntries[uiStream]);
    NIASSERT(pkStream);
    ShaderRegisterEntry* pkEntry = &(pkStream->m_pkEntries[uiEntry]);
    NIASSERT(pkEntry);

    if ((pkEntry->m_eInput != eInput) ||
        (pkEntry->m_eType != eType) ||
        (pkEntry->m_kUsage != kUsage) ||
        (pkEntry->m_uiUsageIndex != uiIndex) ||
        (pkEntry->m_uiExtraData != uiExtraData))
    {
        pkEntry->m_eInput = eInput;
        pkEntry->m_eType = eType;
        pkEntry->m_kUsage = kUsage;
        pkEntry->m_uiUsageIndex = uiIndex;
        pkEntry->m_uiExtraData = uiExtraData;

        pkStream->m_uiEntryCount = NiMax((int)(uiEntry + 1),
            (int)(pkStream->m_uiEntryCount));

        //  Force blend weights to output float 3
        if (eInput == SHADERPARAM_NI_BLENDWEIGHT)
            pkEntry->m_eType = SPTYPE_FLOAT3;
        MarkAsModified();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderDeclaration::RemoveEntry(unsigned int uiEntry, 
    unsigned int uiStream)
{
    NIASSERT(m_pkStreamEntries);
    
    //  Check entry slot
    if (uiEntry >= m_uiMaxStreamEntryCount)
        return false;

    //  Check stream validity
    if (uiStream >= m_uiStreamCount)
        return false;

    //  Set it
    ShaderRegisterStream* pkStream = &(m_pkStreamEntries[uiStream]);
    NIASSERT(pkStream);
    ShaderRegisterEntry* pkEntry = &(pkStream->m_pkEntries[uiEntry]);
    NIASSERT(pkEntry);

    //  Set it to invalid
    pkEntry->m_eInput = SHADERPARAM_INVALID;
    pkEntry->m_eType = SPTYPE_NONE;
    MarkAsModified();

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderDeclaration::ClearAllEntries(unsigned int uiStream)
{
    NIASSERT(m_pkStreamEntries);
    
    //  Check stream validity
    if (uiStream >= m_uiStreamCount)
        return false;

    //  Set it
    ShaderRegisterEntry* pkEntry;

    ShaderRegisterStream* pkStream = &(m_pkStreamEntries[uiStream]);
    NIASSERT(pkStream);
    for (unsigned int ui = 0; ui < m_uiMaxStreamEntryCount; ui++)
    {
        pkEntry = &(pkStream->m_pkEntries[ui]);
        NIASSERT(pkEntry);
        //  Set it to invalid
        pkEntry->m_eInput = SHADERPARAM_INVALID;
        pkEntry->m_eType = SPTYPE_NONE;
    }
    MarkAsModified();

    return true;
}
//---------------------------------------------------------------------------
void NiShaderDeclaration::MarkAsModified()
{
    m_bModified = true;
}
//---------------------------------------------------------------------------
bool NiShaderDeclaration::CreateDeclForGeometry(NiGeometry* pkGeometry)
{
    if (!pkGeometry)
        return false;

    NiShaderDeclaration::ShaderRegisterEntry 
        akGeomSemantics[NiMaterial::VS_INPUTS_MAX_NUM];

    NiShaderDeclaration::ShaderRegisterEntry 
        akMaterialSemantics[NiMaterial::VS_INPUTS_MAX_NUM];
    akMaterialSemantics[0].m_uiUsageIndex = 0xFFFFFFFF;

    unsigned int uiGeomSemantics = 0;
    bool bSuccess = false;
    bool bPackAllOnGeom = false;

    // Consider the active material first.
    const NiMaterialInstance* pkActiveInstance = 
        pkGeometry->GetActiveMaterialInstance();
    if (pkActiveInstance)
    {
        bSuccess = pkActiveInstance->GetMaterial()->GetVertexInputSemantics(
            pkGeometry, pkActiveInstance->GetMaterialExtraData(),
            akMaterialSemantics);
        if (bSuccess)
        {
            MergeInputSemantics(akGeomSemantics, akMaterialSemantics, 
                uiGeomSemantics);
        }
        else if(akMaterialSemantics[0].m_uiUsageIndex == 
            NiMaterial::VS_INPUTS_USE_GEOMETRY)
        {
            bPackAllOnGeom = true;
        }
    }

    // Iterate over all of the material instances and consider any that are
    // not the renderer default.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    if (!pkRenderer)
        return false;
    
    const NiMaterial* pkDefMaterial = pkRenderer->GetDefaultMaterial();
    const NiMaterialInstance* pkDefInstance = NULL;

    unsigned int uiMaterialCount = pkGeometry->GetMaterialCount();
    for (unsigned int ui = 0; ui < uiMaterialCount; ++ui)
    {
        const NiMaterialInstance* pkMatInstance = 
            pkGeometry->GetMaterialInstance(ui);
        if (pkMatInstance && pkMatInstance != pkActiveInstance)
        {
            if (pkMatInstance->GetMaterial() == pkDefMaterial)
            {
                pkDefInstance = pkMatInstance;
                continue;
            }

            bool bMatSuccess = 
                pkMatInstance->GetMaterial()->GetVertexInputSemantics(
                pkGeometry, pkMatInstance->GetMaterialExtraData(),
                akMaterialSemantics);
            bSuccess |= bMatSuccess;
            if (bMatSuccess)
            {
                MergeInputSemantics(akGeomSemantics, akMaterialSemantics, 
                    uiGeomSemantics);
            }
            else if(akMaterialSemantics[0].m_uiUsageIndex == 
                NiMaterial::VS_INPUTS_USE_GEOMETRY)
            {
                bPackAllOnGeom = true;
            }
        }
    }

    // If no semantics found yet, apply default material.
    if (uiGeomSemantics == NULL)
    {
        pkGeometry->ApplyMaterial((NiMaterial*)pkDefMaterial);
        uiMaterialCount = pkGeometry->GetMaterialCount();
        for (unsigned int ui = 0; ui < uiMaterialCount; ++ui)
        {
            const NiMaterialInstance* pkMatInstance = 
                pkGeometry->GetMaterialInstance(ui);
            if (pkMatInstance && pkMatInstance->GetMaterial() == pkDefMaterial)
            {
                pkDefInstance = pkMatInstance;
                break;
            }
        }
    }

    // Consider the renderer default last if applied to the object or if no
    // semantics found yet.
    if (pkDefInstance)
    {
        bool bMatSuccess = 
            pkDefInstance->GetMaterial()->GetVertexInputSemantics(
            pkGeometry, pkDefInstance->GetMaterialExtraData(),
            akMaterialSemantics);
        bSuccess |= bMatSuccess;
        if (bMatSuccess)
        {
            MergeInputSemantics(akGeomSemantics, akMaterialSemantics, 
                uiGeomSemantics);
        }
        else if(akMaterialSemantics[0].m_uiUsageIndex == 
            NiMaterial::VS_INPUTS_USE_GEOMETRY)
        {
            bPackAllOnGeom = true;
        }
    }

    if (bPackAllOnGeom)
    {
        GenerateSemanticsFromGeom(akMaterialSemantics, pkGeometry);
        MergeInputSemantics(akGeomSemantics, akMaterialSemantics, 
            uiGeomSemantics);
        bSuccess = true;
    }

    // fail if no semantics by now
    if (!bSuccess)
        return false;

    unsigned int uiNumStreams = 0;
    unsigned int uiEntriesPerStream = 0;

    // Determine number of streams
    for (unsigned int ui = 0;ui < uiGeomSemantics; ++ui)
    {
        uiNumStreams = NiMax((int)uiNumStreams, 
            (int)(akGeomSemantics[ui].m_uiPreferredStream + 1));
    }

    // Determine max number of entries per stream
    for (unsigned int ui = 0; ui < uiNumStreams; ++ui)
    {
        unsigned int uiTemp = 0;
        unsigned int uiTemp2 = 0;
        while (uiTemp2 < uiGeomSemantics)
        {
            if (akGeomSemantics[uiTemp2++].m_uiPreferredStream == ui)
                ++uiTemp;
        }
        uiEntriesPerStream = NiMax((int)uiEntriesPerStream, (int)uiTemp);
    }

    NiShaderDeclaration* pkDecl = pkRenderer->CreateShaderDeclaration(
        uiEntriesPerStream, uiNumStreams);
    
    if (!pkDecl)
        return false;

    // Set the entries on the declaration.
    for (unsigned int ui = 0; ui < uiNumStreams; ++ui)
    {
        unsigned int uiEntryCount = 0;
        for (unsigned int uj = 0; uj < uiGeomSemantics; ++uj)
        {
            if (akGeomSemantics[uj].m_uiPreferredStream == ui)
            {
                pkDecl->SetEntry(ui, uiEntryCount, 
                    akGeomSemantics[uj].m_eInput, 
                    akGeomSemantics[uj].m_eType,
                    akGeomSemantics[uj].m_kUsage,
                    akGeomSemantics[uj].m_uiUsageIndex,
                    akGeomSemantics[uj].m_uiExtraData);
                ++uiEntryCount;
            }
        }
    }

    // We'll return true if any one of the materials set its vertex inputs.
    pkGeometry->SetShaderDecl(pkDecl);
    return true;
}
//---------------------------------------------------------------------------
bool NiShaderDeclaration::MergeInputSemantics(
    NiShaderDeclaration::ShaderRegisterEntry* pakGeomSemantics, 
    NiShaderDeclaration::ShaderRegisterEntry* pakMaterialSemantics, 
    unsigned int& uiGeomSemantics)
{
    unsigned int uiMatEntry = 0;
    while (pakMaterialSemantics[uiMatEntry].m_uiUsageIndex != 0xFFFFFFFF)
    {
        if (uiGeomSemantics == NiMaterial::VS_INPUTS_MAX_NUM)
            return false;

        unsigned int uiGeomEntry = 0;
        NiShaderDeclaration::ShaderRegisterEntry* pkMatEntry = 
            pakMaterialSemantics + uiMatEntry++;
        bool bFound = false;
        while (uiGeomEntry < uiGeomSemantics)
        {
            if ((pakGeomSemantics[uiGeomEntry].m_kUsage ==
                pkMatEntry->m_kUsage) && 
                (pakGeomSemantics[uiGeomEntry].m_uiUsageIndex == 
                pkMatEntry->m_uiUsageIndex))
            {
                bFound = true;
                break;
            }
            ++uiGeomEntry;
        }
        if (!bFound)
        {
            // Add the entry.
            pakGeomSemantics[uiGeomSemantics].m_uiPreferredStream = 
                pkMatEntry->m_uiPreferredStream;
            pakGeomSemantics[uiGeomSemantics].m_eInput = pkMatEntry->m_eInput;
            pakGeomSemantics[uiGeomSemantics].m_eType = pkMatEntry->m_eType;
            pakGeomSemantics[uiGeomSemantics].m_kUsage = pkMatEntry->m_kUsage;
            pakGeomSemantics[uiGeomSemantics].m_uiUsageIndex = 
                pkMatEntry->m_uiUsageIndex; 
            pakGeomSemantics[uiGeomSemantics].m_uiExtraData = 
                pkMatEntry->m_uiExtraData;
            ++uiGeomSemantics;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
void NiShaderDeclaration::GenerateSemanticsFromGeom(
    NiShaderDeclaration::ShaderRegisterEntry* pakGeomSemantics, 
    NiGeometry* pkGeometry)
{
    // We'll always need position. We'll always request stream 0.
    pakGeomSemantics[0].m_uiPreferredStream = 0;
    pakGeomSemantics[0].m_eInput = 
        NiShaderDeclaration::SHADERPARAM_NI_POSITION;
    pakGeomSemantics[0].m_eType = NiShaderDeclaration::SPTYPE_FLOAT3;
    pakGeomSemantics[0].m_kUsage = NiShaderDeclaration::UsageToString(
        NiShaderDeclaration::SPUSAGE_POSITION);
    pakGeomSemantics[0].m_uiUsageIndex = 0; 
    pakGeomSemantics[0].m_uiExtraData = 0;

    unsigned int uiEntryCount = 1;

    // Handle skinned transforms.
    NiSkinPartition* pkPartition = NULL;
    NiSkinInstance* pkSkin = pkGeometry->GetSkinInstance();
    if (pkSkin != NULL)
        pkPartition = pkSkin->GetSkinPartition();

    if (pkSkin != NULL && pkPartition != NULL)
    {
        NiSkinPartition::Partition* pkIndexedPartition = 
            pkPartition->GetPartitions();

        // Add bone weights.
        pakGeomSemantics[uiEntryCount].m_uiPreferredStream = 0;
        pakGeomSemantics[uiEntryCount].m_eInput = 
            NiShaderDeclaration::SHADERPARAM_NI_BLENDWEIGHT;
        pakGeomSemantics[uiEntryCount].m_eType = 
            NiShaderDeclaration::SPTYPE_FLOAT3;
        pakGeomSemantics[uiEntryCount].m_kUsage = 
            NiShaderDeclaration::UsageToString(
            NiShaderDeclaration::SPUSAGE_BLENDWEIGHT);
        pakGeomSemantics[uiEntryCount].m_uiUsageIndex = 0; 
        pakGeomSemantics[uiEntryCount].m_uiExtraData = 0;
        ++uiEntryCount;

        if (pkPartition->GetPartitionCount() > 0 && 
            pkIndexedPartition != NULL && 
            pkIndexedPartition[0].m_pucBonePalette != NULL)
        {
            pakGeomSemantics[uiEntryCount].m_uiPreferredStream = 0;
            pakGeomSemantics[uiEntryCount].m_eInput = 
                NiShaderDeclaration::SHADERPARAM_NI_BLENDINDICES;
            pakGeomSemantics[uiEntryCount].m_eType = 
                NiShaderDeclaration::SPTYPE_UBYTE4;
            pakGeomSemantics[uiEntryCount].m_kUsage = 
                NiShaderDeclaration::UsageToString(
                NiShaderDeclaration::SPUSAGE_BLENDINDICES);
            pakGeomSemantics[uiEntryCount].m_uiUsageIndex = 0; 
            pakGeomSemantics[uiEntryCount].m_uiExtraData = 0;
            ++uiEntryCount;
        }
    }

    if (pkGeometry->ContainsVertexData(
        NiShaderDeclaration::SHADERPARAM_NI_NORMAL))
    {
        pakGeomSemantics[uiEntryCount].m_uiPreferredStream = 0;
        pakGeomSemantics[uiEntryCount].m_eInput = 
            NiShaderDeclaration::SHADERPARAM_NI_NORMAL;
        pakGeomSemantics[uiEntryCount].m_eType = 
            NiShaderDeclaration::SPTYPE_FLOAT3;
        pakGeomSemantics[uiEntryCount].m_kUsage = 
            NiShaderDeclaration::UsageToString(
            NiShaderDeclaration::SPUSAGE_NORMAL);
        pakGeomSemantics[uiEntryCount].m_uiUsageIndex = 0; 
        pakGeomSemantics[uiEntryCount].m_uiExtraData = 0;
        ++uiEntryCount;
    }

    if (pkGeometry->ContainsVertexData(
        NiShaderDeclaration::SHADERPARAM_NI_COLOR))
    {
        pakGeomSemantics[uiEntryCount].m_uiPreferredStream = 0;
        pakGeomSemantics[uiEntryCount].m_eInput = 
            NiShaderDeclaration::SHADERPARAM_NI_COLOR;
        pakGeomSemantics[uiEntryCount].m_eType = 
            NiShaderDeclaration::SPTYPE_UBYTECOLOR;
        pakGeomSemantics[uiEntryCount].m_kUsage = 
            NiShaderDeclaration::UsageToString(
            NiShaderDeclaration::SPUSAGE_COLOR);
        pakGeomSemantics[uiEntryCount].m_uiUsageIndex = 0; 
        pakGeomSemantics[uiEntryCount].m_uiExtraData = 0;
        ++uiEntryCount;
    }

    for (unsigned int ui = 0; ui < 8; ui++)
    {
        NiShaderDeclaration::ShaderParameter eInput = 
            (NiShaderDeclaration::ShaderParameter)
            (NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0 + ui);
        if (pkGeometry->ContainsVertexData(eInput))
        {
            pakGeomSemantics[uiEntryCount].m_uiPreferredStream = 0;
            pakGeomSemantics[uiEntryCount].m_eInput = eInput;
            pakGeomSemantics[uiEntryCount].m_eType = 
                NiShaderDeclaration::SPTYPE_FLOAT2;
            pakGeomSemantics[uiEntryCount].m_kUsage = 
                NiShaderDeclaration::UsageToString(
                NiShaderDeclaration::SPUSAGE_TEXCOORD);
            pakGeomSemantics[uiEntryCount].m_uiUsageIndex = ui; 
            pakGeomSemantics[uiEntryCount].m_uiExtraData = 0;
            ++uiEntryCount;
        }
    }
 
    // Binormals and tangents must be last, so this shader declaration
    // can be used with the fixed-function pipeline in DX9.
    if (pkGeometry->GetNormalBinormalTangentMethod() != 
        NiGeometryData::NBT_METHOD_NONE && 
        pkGeometry->ContainsVertexData(
        NiShaderDeclaration::SHADERPARAM_NI_BINORMAL) &&
        pkGeometry->ContainsVertexData(
        NiShaderDeclaration::SHADERPARAM_NI_TANGENT))
    {
        pakGeomSemantics[uiEntryCount].m_uiPreferredStream = 0;
        pakGeomSemantics[uiEntryCount].m_eInput = 
            NiShaderDeclaration::SHADERPARAM_NI_BINORMAL;
        pakGeomSemantics[uiEntryCount].m_eType = 
            NiShaderDeclaration::SPTYPE_FLOAT3;
        pakGeomSemantics[uiEntryCount].m_kUsage = 
            NiShaderDeclaration::UsageToString(
            NiShaderDeclaration::SPUSAGE_BINORMAL);
        pakGeomSemantics[uiEntryCount].m_uiUsageIndex = 0; 
        pakGeomSemantics[uiEntryCount].m_uiExtraData = 0;
        ++uiEntryCount;

        pakGeomSemantics[uiEntryCount].m_uiPreferredStream = 0;
        pakGeomSemantics[uiEntryCount].m_eInput = 
            NiShaderDeclaration::SHADERPARAM_NI_TANGENT;
        pakGeomSemantics[uiEntryCount].m_eType = 
            NiShaderDeclaration::SPTYPE_FLOAT3;
        pakGeomSemantics[uiEntryCount].m_kUsage = 
            NiShaderDeclaration::UsageToString(
            NiShaderDeclaration::SPUSAGE_TANGENT);
        pakGeomSemantics[uiEntryCount].m_uiUsageIndex = 0; 
        pakGeomSemantics[uiEntryCount].m_uiExtraData = 0;
        ++uiEntryCount;
    }

    // Signal the end of our requirements by packing a usage index of 
    // NiMaterial::VS_INPUTS_TERMINATE_ARRAY.
    NIASSERT(uiEntryCount < NiMaterial::VS_INPUTS_MAX_NUM);
    pakGeomSemantics[uiEntryCount].m_uiUsageIndex = 
        NiMaterial::VS_INPUTS_TERMINATE_ARRAY;
}
//---------------------------------------------------------------------------
