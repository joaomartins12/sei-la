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

#include "NiStaticGeometryGroup.h"
#include "NiGeometryBufferData.h"
#include "NiVBSet.h"
#include <NiTriShapeData.h>
#include <NiTriStripsData.h>
#include <NiGeometryGroupManager.h>
#include <NiSkinPartition.h>
#include <NiScreenTexture.h>

//---------------------------------------------------------------------------
NiStaticGeometryGroup::~NiStaticGeometryGroup()
{
    Purge();
}
//---------------------------------------------------------------------------
void NiStaticGeometryGroup::Purge()
{
    NiTMapIterator kPos = m_kSets.GetFirstPos();
    while (kPos)
    {
        unsigned int uiFVF;
        NiVBSet* pkSet;
        m_kSets.GetNext(kPos, uiFVF, pkSet);
        m_kSets.RemoveAt(uiFVF);

        NiDelete pkSet;
    }
}
//---------------------------------------------------------------------------
NiGeometryGroup* NiStaticGeometryGroup::Create()
{
    NiStaticGeometryGroup* pkGroup = NiNew NiStaticGeometryGroup();

    return pkGroup;
}
//---------------------------------------------------------------------------
void NiStaticGeometryGroup::AddObject(
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
void NiStaticGeometryGroup::RemoveObject(NiGeometryData* pkData)
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
void NiStaticGeometryGroup::RemoveObject(
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
void NiStaticGeometryGroup::AddObject(NiScreenTexture* pkScreenTexture)
{
    NIASSERT(pkScreenTexture);

    // Create NiGeometryBufferData
    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkScreenTexture->GetRendererData();
    if (pkBuffData)
        return;

    pkBuffData = NiNew NiGeometryBufferData;
    pkBuffData->SetType(D3DPT_TRIANGLELIST);

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
void NiStaticGeometryGroup::RemoveObject(NiScreenTexture* pkScreenTexture)
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
NiVBChip* NiStaticGeometryGroup::CreateChip(NiGeometryBufferData* pkBuffData,
    unsigned int uiStream)
{
    NiVBSet* pkSet = NULL;

    unsigned int uiIndex = pkBuffData->GetFVF();
    unsigned int uiStride = pkBuffData->GetVertexStride(uiStream);
    if (uiIndex == 0)
    {
        uiIndex = INDEX_MASK_STRIDE | uiStride;
    }

    unsigned int uiFlags = 0;

    if (pkBuffData->GetSoftwareVP())
    {
        uiIndex |= INDEX_MASK_SOFTWAREVP;
        uiFlags |= NiVBSet::NIVBSET_FLAG_SOFTWAREVP;
    }

    if (!m_kSets.GetAt(uiIndex, pkSet) || pkSet == NULL)
    {
        pkSet = NiVBSet::Create(NiVBSet::NIVBSET_DEF_BLOCKSIZE, 
            m_pkD3DDevice, uiIndex, uiFlags, /*index*/0);
        m_kSets.SetAt(uiIndex, pkSet);
    }

    unsigned int uiNumStreams = pkBuffData->GetStreamCount();
    NIASSERT(uiStream < uiNumStreams);

    NiVBChip* pkChip = pkSet->AllocateChip(pkBuffData->GetMaxVertCount() *
        uiStride, (uiNumStreams > 1));

    // Fill in offset value in pkBuffData
    NIASSERT(uiNumStreams == 1 || pkChip->GetOffset() == 0);
    NIASSERT(pkChip->GetOffset() % uiStride == 0);
    pkBuffData->SetBaseVertexIndex(pkChip->GetOffset() / uiStride);

    return pkChip;
}
//---------------------------------------------------------------------------
void NiStaticGeometryGroup::ReleaseChip(NiGeometryBufferData* pkBuffData,
    unsigned int uiStream)
{
    NiVBChip* pkChip = pkBuffData->GetVBChip(uiStream);
    if (pkChip)
    {
        if (pkChip->GetVB())
        {

            NiVBBlock* pkBlock = pkChip->GetBlock();
            NIASSERT(pkBlock && pkBlock->GetSet());

            pkBlock->GetSet()->FreeChip(pkBlock->GetIndex(), 
                pkChip->GetIndex());
        }
        pkBuffData->RemoveVBChip(uiStream);
    }
}
//---------------------------------------------------------------------------
bool NiStaticGeometryGroup::IsDynamic()
{
    return false;
}
//---------------------------------------------------------------------------
