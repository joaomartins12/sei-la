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

#include "NiDynamicGeometryGroup.h"
#include "NiGeometryBufferData.h"
#include "NiVBDynamicSet.h"

#include <NiTriShapeData.h>
#include <NiTriStripsData.h>
#include <NiGeometryGroupManager.h>
#include <NiSkinPartition.h>
#include <NiScreenTexture.h>

NiDynamicGeometryGroup* NiDynamicGeometryGroup::ms_pkGroup = NULL;

//---------------------------------------------------------------------------
NiDynamicGeometryGroup::NiDynamicGeometryGroup() : 
    m_uiNextIndex(0)
{ /* */ }
//---------------------------------------------------------------------------
NiDynamicGeometryGroup::~NiDynamicGeometryGroup()
{
    // Slightly different than Purge, since the chips are deleted.
    NiTMapIterator kPos = m_kSets.GetFirstPos();
    while (kPos)
    {
        unsigned int uiIndex;
        NiVBDynamicSet* pkSet;
        m_kSets.GetNext(kPos, uiIndex, pkSet);
        m_kSets.RemoveAt(uiIndex);

        NiDelete pkSet;
    }

    kPos = m_kChips.GetFirstPos();
    while (kPos)
    {
        unsigned int uiIndex;
        NiVBChip* pkChip;
        m_kChips.GetNext(kPos, uiIndex, pkChip);
        m_kChips.RemoveAt(uiIndex);

        pkChip->SetVB(NULL);
        NiDelete pkChip;
    }

    NIASSERT(m_kMultiStreamSets.GetAllocatedSize() == 
        m_kMultiStreamChips.GetAllocatedSize());
    for (unsigned int i = 0; i < m_kMultiStreamSets.GetAllocatedSize(); i++)
    {
        NiVBDynamicSet* pkSet = m_kMultiStreamSets.RemoveAt(i);
        NiDelete pkSet;

        NiVBChip* pkChip = m_kMultiStreamChips.RemoveAt(i);
        pkChip->SetVB(NULL);
        NiDelete pkChip;
    }

    if (this == ms_pkGroup)
    {
        ms_pkGroup = NULL;
    }
}
//---------------------------------------------------------------------------
void NiDynamicGeometryGroup::Purge()
{
    NiTMapIterator kPos = m_kSets.GetFirstPos();
    while (kPos)
    {
        unsigned int uiIndex;
        NiVBDynamicSet* pkSet;
        m_kSets.GetNext(kPos, uiIndex, pkSet);
        m_kSets.RemoveAt(uiIndex);

        NiDelete pkSet;
    }

    kPos = m_kChips.GetFirstPos();
    while (kPos)
    {
        unsigned int uiIndex;
        NiVBChip* pkChip;
        m_kChips.GetNext(kPos, uiIndex, pkChip);

        pkChip->SetVB(NULL);
    }

    NIASSERT(m_kMultiStreamSets.GetAllocatedSize() == 
        m_kMultiStreamChips.GetAllocatedSize());
    for (unsigned int i = 0; i < m_kMultiStreamSets.GetAllocatedSize(); i++)
    {
        NiVBDynamicSet* pkSet = m_kMultiStreamSets.RemoveAt(i);
        NiDelete pkSet;

        NiVBChip* pkChip = m_kMultiStreamChips.GetAt(i);
        pkChip->SetVB(NULL);
    }
}
//---------------------------------------------------------------------------
NiGeometryGroup* NiDynamicGeometryGroup::Create()
{
    if (ms_pkGroup == NULL)
    {
        ms_pkGroup = NiNew NiDynamicGeometryGroup();
    }

    return ms_pkGroup;
}
//---------------------------------------------------------------------------
void NiDynamicGeometryGroup::AddObject(
    NiGeometryData* pkData, 
    NiSkinInstance* pkSkinInstance, 
    NiSkinPartition::Partition* pkPartition)
{
    NIASSERT(pkData);
    pkData->SetConsistency(NiGeometryData::VOLATILE);

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

    pkData->MarkAsChanged(0xffff);
}
//---------------------------------------------------------------------------
void NiDynamicGeometryGroup::RemoveObject(NiGeometryData* pkData)
{
    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkData->GetRendererData();
    if (!pkBuffData)
        return;

    NiGeometryGroup::RemoveObject(pkBuffData);

    pkBuffData->RemoveAllVBChips();
    NiDelete pkBuffData;
    pkData->SetRendererData(NULL);
}
//---------------------------------------------------------------------------
void NiDynamicGeometryGroup::RemoveObject(
    NiSkinPartition::Partition* pkPartition)
{
    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkPartition->m_pkBuffData;
    if (!pkBuffData)
        return;

    NiGeometryGroup::RemoveObject(pkBuffData);

    pkBuffData->RemoveAllVBChips();
    NiDelete pkBuffData;
    pkPartition->m_pkBuffData = NULL;
}
//---------------------------------------------------------------------------
void NiDynamicGeometryGroup::AddObject(NiScreenTexture* pkScreenTexture)
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

    pkScreenTexture->MarkAsChanged(0xffff);
}
//---------------------------------------------------------------------------
void NiDynamicGeometryGroup::RemoveObject(NiScreenTexture* pkScreenTexture)
{
    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkScreenTexture->GetRendererData();
    if (!pkBuffData)
        return;

    NiGeometryGroup::RemoveObject(pkBuffData);

    pkBuffData->RemoveAllVBChips();
    NiDelete pkBuffData;
    pkScreenTexture->SetRendererData(NULL);
}
//---------------------------------------------------------------------------
NiVBChip* NiDynamicGeometryGroup::CreateChip(
    NiGeometryBufferData* pkBuffData, unsigned int uiStream)
{
    NiVBChip* pkChip = NULL;
    unsigned int uiNumStreams = pkBuffData->GetStreamCount();
    NIASSERT(uiStream < uiNumStreams);

    unsigned int uiFVF = pkBuffData->GetFVF();
    unsigned int uiStride = pkBuffData->GetVertexStride(uiStream);
    unsigned int uiSize = pkBuffData->GetMaxVertCount() * uiStride;
    unsigned int uiOffset = 0; 
    unsigned int uiFlags = 0;

    if (uiNumStreams > 1)
    {
        NIASSERT(uiFVF == NULL && uiStream < uiNumStreams);

        if (m_kMultiStreamSets.GetAllocatedSize() < uiNumStreams)
        {
            m_kMultiStreamSets.SetSize(uiNumStreams);
            m_kMultiStreamChips.SetSize(uiNumStreams);
        }
        NIASSERT(m_kMultiStreamSets.GetAllocatedSize() == 
            m_kMultiStreamChips.GetAllocatedSize());

        if (m_uiNextIndex >= m_kMultiStreamSets.GetAllocatedSize())
            m_uiNextIndex = 0;

        NiVBDynamicSet* pkSet = m_kMultiStreamSets.GetAt(m_uiNextIndex);
        if (pkSet == NULL)
        {
            pkSet = NiVBDynamicSet::Create(
                NiVBDynamicSet::NIVBDYNAMICSET_DEF_BLOCKSIZE, m_pkD3DDevice, 
                0, /*index*/0);
            m_kMultiStreamSets.SetAt(m_uiNextIndex, pkSet);
        }

        // Force a fresh buffer
        D3DVertexBufferPtr pkVB = pkSet->AllocateBufferSpace(uiSize, 
            uiOffset, uiFlags, true, pkBuffData->GetSoftwareVP());

        // Assert that the offset is 0. On non-Xenon platforms, check for
        // discard semantics on the lock. D3DLOCK_DISCARD did nothing on 
        // Xenon and was removed by MS.
#if !defined(_XENON)
        NIASSERT(uiOffset == 0 && (uiFlags | D3DLOCK_DISCARD) != 0);
#else
        NIASSERT(uiOffset == 0);
#endif

        pkChip = m_kMultiStreamChips.GetAt(m_uiNextIndex);
        if (pkChip == NULL)
        {
            pkChip = NiVBChip::Create(NULL, pkVB, uiOffset, uiSize, 
                /*index*/0);
            NIASSERT(pkChip);
            m_kMultiStreamChips.SetAt(m_uiNextIndex, pkChip);
        }
        else
        {
            pkChip->SetVB(pkVB);
            pkChip->SetOffset(uiOffset);
            pkChip->SetSize(uiSize);
        }
        pkChip->SetDynamicSet(pkSet);
        pkChip->SetLockFlags(uiFlags);
        pkBuffData->SetBaseVertexIndex(0);
        m_uiNextIndex++;
    }
    else
    {
        NIASSERT(uiStream == 0);

        NiVBDynamicSet* pkSet = NULL;

        unsigned int uiIndex = (uiFVF ? uiFVF : INDEX_MASK_STRIDE | uiStride);
        if (!m_kSets.GetAt(uiIndex, pkSet) || pkSet == NULL)
        {
            pkSet = NiVBDynamicSet::Create(
                NiVBDynamicSet::NIVBDYNAMICSET_DEF_BLOCKSIZE, m_pkD3DDevice, 
                uiFVF, /*index*/0);
            m_kSets.SetAt(uiIndex, pkSet);
        }

        D3DVertexBufferPtr pkVB = pkSet->AllocateBufferSpace(uiSize, 
            uiOffset, uiFlags, false, pkBuffData->GetSoftwareVP());
        m_kChips.GetAt(uiIndex, pkChip);
        if (pkChip == NULL)
        {
            pkChip = NiVBChip::Create(NULL, pkVB, uiOffset, uiSize, 
                /*index*/0);
            NIASSERT(pkChip);
            m_kChips.SetAt(uiIndex, pkChip);
        }
        else
        {
            pkChip->SetVB(pkVB);
            pkChip->SetOffset(uiOffset);
            pkChip->SetSize(uiSize);
        }
        pkChip->SetDynamicSet(pkSet);
        pkChip->SetLockFlags(uiFlags);

        // Fill in offset value in pkBuffData
        NIASSERT(uiOffset % uiStride == 0);
        pkBuffData->SetBaseVertexIndex(uiOffset / uiStride);
    }
    return pkChip;
}
//---------------------------------------------------------------------------
void NiDynamicGeometryGroup::ReleaseChip(NiGeometryBufferData* pkBuffData,
    unsigned int uiStream)
{
    NiVBChip* pkChip = pkBuffData->GetVBChip(uiStream);
    if (pkChip)
    {
        // Free buffer from chip if it exists
        if (pkChip->GetVB())
        {
            NiVBDynamicSet* pkSet = pkChip->GetDynamicSet();
            NIASSERT(pkSet);
            pkSet->FreeBufferSpace(pkChip->GetVB());
            pkChip->SetVB(NULL);
        }

        pkBuffData->RemoveVBChip(uiStream);
    }
}
//---------------------------------------------------------------------------
bool NiDynamicGeometryGroup::IsDynamic()
{
    return true;
}
//---------------------------------------------------------------------------
