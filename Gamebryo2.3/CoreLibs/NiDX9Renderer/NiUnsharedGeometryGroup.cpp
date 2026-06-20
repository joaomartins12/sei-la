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

#if defined(_XENON)
    #include "NiXenonRenderer.h"
#endif  //#if defined(_XENON)

#include "NiUnsharedGeometryGroup.h"
#include "NiGeometryBufferData.h"
#include "NiVBSet.h"
#include <NiTriShapeData.h>
#include <NiTriStripsData.h>
#include <NiGeometryGroupManager.h>
#include <NiScreenTexture.h>

NiUnsharedGeometryGroup* NiUnsharedGeometryGroup::ms_pkGroup = NULL;

//---------------------------------------------------------------------------
NiUnsharedGeometryGroup::~NiUnsharedGeometryGroup()
{
    Purge();

    if (this == ms_pkGroup)
        ms_pkGroup = NULL;
}
//---------------------------------------------------------------------------
void NiUnsharedGeometryGroup::Purge()
{ /* */ }
//---------------------------------------------------------------------------
NiGeometryGroup* NiUnsharedGeometryGroup::Create()
{
    if (ms_pkGroup == NULL)
    {
        ms_pkGroup = NiNew NiUnsharedGeometryGroup();
    }

    return ms_pkGroup;
}
//---------------------------------------------------------------------------
void NiUnsharedGeometryGroup::AddObject(
    NiGeometryData* pkData, 
    NiSkinInstance* pkSkinInstance, 
    NiSkinPartition::Partition* pkPartition)
{
    NIASSERT(pkData && pkData->GetConsistency() != NiGeometryData::VOLATILE);

    NiGeometryBufferData* pkBuffData = NULL;
    if (pkPartition)
    {
        NIASSERT(pkSkinInstance);

        // Create NiGeometryBufferData
        pkBuffData = (NiGeometryBufferData*)pkPartition->m_pkBuffData;
        if (pkBuffData)
            return;

        pkBuffData = NiNew NiGeometryBufferData;
        if (pkPartition->m_usStrips == 0)
            pkBuffData->SetType(D3DPT_TRIANGLELIST);
        else // (pkPartition->m_usStrips != 0)
            pkBuffData->SetType(D3DPT_TRIANGLESTRIP);

        pkPartition->m_pkBuffData = pkBuffData;
    }
    else
    {
        // Create NiGeometryBufferData
        pkBuffData = (NiGeometryBufferData*)pkData->GetRendererData();
        if (pkBuffData)
            return;

        pkBuffData = NiNew NiGeometryBufferData;
        if (NiIsKindOf(NiTriShapeData, pkData))
            pkBuffData->SetType(D3DPT_TRIANGLELIST);
        else if (NiIsKindOf(NiTriStripsData, pkData))
            pkBuffData->SetType(D3DPT_TRIANGLESTRIP);
        // ignore anything else like NiParticles or NiLines

        pkData->SetRendererData(pkBuffData);
    }

    { // Set buffer flags
        bool bColors = (pkData->GetColors() != NULL);
        bool bNorms = (pkData->GetNormals() != NULL);
        bool bBinormsTangents = (bNorms && 
            pkData->GetNormalBinormalTangentMethod() != 
            NiGeometryData::NBT_METHOD_NONE);
        unsigned int uiT = pkData->GetTextureSets();
        if (uiT > NiD3DPass::ms_uiMaxTextureBlendStages)
            uiT = NiD3DPass::ms_uiMaxTextureBlendStages;
        pkBuffData->SetFlags(
            NiD3DRenderer::CreateVertexFlags(bColors, bNorms, 
            bBinormsTangents, uiT));
    }

    NiGeometryGroup::AddObject(pkBuffData);
}
//---------------------------------------------------------------------------
void NiUnsharedGeometryGroup::RemoveObject(NiGeometryData* pkData)
{
    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkData->GetRendererData();
    if (!pkBuffData)
        return;

    NiGeometryGroup::RemoveObject(pkBuffData);

    NiDelete pkBuffData;
    pkData->SetRendererData(NULL);
}
//---------------------------------------------------------------------------
void NiUnsharedGeometryGroup::RemoveObject(
    NiSkinPartition::Partition* pkPartition)
{
    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkPartition->m_pkBuffData;
    if (!pkBuffData)
        return;

    NiGeometryGroup::RemoveObject(pkBuffData);

    NiDelete pkBuffData;
    pkPartition->m_pkBuffData = NULL;
}
//---------------------------------------------------------------------------
void NiUnsharedGeometryGroup::AddObject(NiScreenTexture* pkScreenTexture)
{
    NIASSERT(pkScreenTexture);

    // Create NiGeometryBufferData
    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkScreenTexture->GetRendererData();
    if (pkBuffData)
        return;

    pkBuffData = NiNew NiGeometryBufferData;
    pkBuffData->SetType(D3DPT_TRIANGLELIST);

    pkScreenTexture->SetRendererData(pkBuffData);

    { // Set buffer flags
        bool bColors = true;
        bool bNorms = false;
        bool bBinormsTangents = false;
        unsigned int uiT = 1;
        pkBuffData->SetFlags(
            NiD3DRenderer::CreateVertexFlags(bColors, bNorms, 
            bBinormsTangents, uiT));
    }

    NiGeometryGroup::AddObject(pkBuffData);
}
//---------------------------------------------------------------------------
void NiUnsharedGeometryGroup::RemoveObject(NiScreenTexture* pkScreenTexture)
{
    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkScreenTexture->GetRendererData();
    if (!pkBuffData)
        return;

    NiGeometryGroup::RemoveObject(pkBuffData);

    NiDelete pkBuffData;
    pkScreenTexture->SetRendererData(NULL);
}
//---------------------------------------------------------------------------
NiVBChip* NiUnsharedGeometryGroup::CreateChip(
    NiGeometryBufferData* pkBuffData, unsigned int uiStream)
{
    // Create new NiVBChip for each object - no blocks or sets
    unsigned int uiBufferSize = pkBuffData->GetMaxVertCount() * 
        pkBuffData->GetVertexStride(uiStream);

    D3DVertexBufferPtr pkVB;

    unsigned int uiUsage = D3DUSAGE_WRITEONLY;
    if (pkBuffData->GetSoftwareVP())
        uiUsage |= D3DUSAGE_SOFTWAREPROCESSING;

    HRESULT eResult = m_pkD3DDevice->CreateVertexBuffer(
        uiBufferSize, uiUsage, pkBuffData->GetFVF(), 
        D3DPOOL_MANAGED, &pkVB, NULL);

    NIASSERT(SUCCEEDED(eResult));

    NIMETRICS_DX9RENDERER_AGGREGATEVALUE(VERTEX_BUFFER_SIZE, uiBufferSize);

    NiVBChip* pkChip = NiVBChip::Create(NULL, pkVB, 0, uiBufferSize, 0);

    return pkChip;
}
//---------------------------------------------------------------------------
void NiUnsharedGeometryGroup::ReleaseChip(NiGeometryBufferData* pkBuffData,
    unsigned int uiStream)
{
    NiVBChip* pkChip = pkBuffData->GetVBChip(uiStream);
    if (pkChip)
    {
        if (pkChip->GetVB())
        {
            NiD3DRenderer::ReleaseVBResource(pkChip->GetVB());
        }
        pkBuffData->RemoveVBChip(uiStream);
        NiDelete pkChip;
    }
}
//---------------------------------------------------------------------------
bool NiUnsharedGeometryGroup::IsDynamic()
{
    return false;
}
//---------------------------------------------------------------------------
