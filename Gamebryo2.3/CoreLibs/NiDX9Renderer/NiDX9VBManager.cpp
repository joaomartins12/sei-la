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

#include "NiDX9RendererLibType.h"
#include "NiDX9VBManager.h"
#include "NiDX9Resource.h"
#include "NiDX9Renderer.h"
#include "NiD3DUtils.h"
#include "NiD3DPass.h"
#include "NiGeometryGroup.h"
#include "NiVBChip.h"

#include <NiGeometryData.h>
#include <NiScreenTexture.h>
#include <NiSkinInstance.h>

//---------------------------------------------------------------------------
NiDX9VertexBufferManager::NiDX9VertexBufferManager(
    LPDIRECT3DDEVICE9 pkD3DDevice9) :
    m_pvTempBuffer(NULL),
    m_uiTempBufferSize(0),
    m_pvLockedBuffer(NULL),
    m_uiLockedBufferSize(0)
{
    m_pkD3DDevice9 = pkD3DDevice9;
    D3D_POINTER_REFERENCE(m_pkD3DDevice9);

    m_kDefaultResources.RemoveAll();
    m_kManagedResources.RemoveAll();
    m_kSystemMemResources.RemoveAll();

    m_uiFrameID = 0;
}
//---------------------------------------------------------------------------
NiDX9VertexBufferManager::~NiDX9VertexBufferManager()
{
    //  Cycle through all lists, and free the resources
    NiTMapIterator kPos;

    NiTPointerMap<unsigned int, NiDX9VBInfo*>* pkResourceMap;
    
    for (unsigned int ui = 0; ui < 3; ui++)
    {
        switch (ui)
        {
        case 0:
            pkResourceMap = &m_kDefaultResources;
            break;
        case 1:
            pkResourceMap = &m_kManagedResources;
            break;
        case 2:
            pkResourceMap = &m_kSystemMemResources;
            break;
        }
        kPos = pkResourceMap->GetFirstPos();
        while (kPos)
        {
            unsigned int uiKey;
            NiDX9VBInfo* pkVBInfo;
            pkResourceMap->GetNext(kPos, uiKey, pkVBInfo);

            //  Kill the vertex buffers...
            if (pkVBInfo)
            {
                NiDX9VBResource* pkVBRes;
            
                for (unsigned int uj = 0; uj < DX9_MAX_VBS; uj++)
                {
                    pkVBRes = pkVBInfo->m_pkVBRes[uj];
                    if (pkVBRes)
                    {
                        if (pkVBRes->m_pkVB)
                            D3D_POINTER_RELEASE(pkVBRes->m_pkVB);
                    }
                    NiDelete pkVBRes;
                }
                NiDelete pkVBInfo;
            }
        }
    }

    m_kDefaultResources.RemoveAll();
    m_kManagedResources.RemoveAll();
    m_kSystemMemResources.RemoveAll();

    D3D_POINTER_RELEASE(m_pkD3DDevice9);
    m_pkD3DDevice9 = 0;

    NiFree(m_pvTempBuffer);
}
//---------------------------------------------------------------------------
bool NiDX9VertexBufferManager::AllocateBufferSpace(
    NiGeometryBufferData* pkBuffData, unsigned int uiStream)
{
    NIASSERT(pkBuffData);
    unsigned int uiStreams = pkBuffData->GetStreamCount();
    NIASSERT(uiStreams > 0);
    NiGeometryGroup* pkGroup = pkBuffData->GetGeometryGroup();
    NIASSERT(pkGroup);

    pkGroup->ReleaseChip(pkBuffData, uiStream);

    NiVBChip* pkChip = pkGroup->CreateChip(pkBuffData, uiStream);
    if (pkChip)
    {
        pkBuffData->SetVBChip(uiStream, pkChip);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void* NiDX9VertexBufferManager::LockVB(LPDIRECT3DVERTEXBUFFER9 pkVB,
    unsigned int uiOffset, unsigned int uiSize, unsigned int uiLockFlags)
{
    void* pvBufferData = NULL;
    if (FAILED(pkVB->Lock(uiOffset, uiSize, &pvBufferData, uiLockFlags)))
    {
        return NULL;
    }

    m_kVBCriticalSection.Lock();

    NIASSERT(m_pvLockedBuffer == NULL);
    m_pvLockedBuffer = pvBufferData;
    m_uiLockedBufferSize = uiSize;

    if (m_uiTempBufferSize < uiSize)
    {
        NiFree(m_pvTempBuffer);
        m_pvTempBuffer = NiAlloc(char, uiSize);
        m_uiTempBufferSize = uiSize;
    }

    if ((uiLockFlags & (D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE)) == 0)
    {
        // not a dynamic buffer - copy buffer contents
        NiMemcpy(m_pvTempBuffer, pvBufferData, uiSize);
    }

    return m_pvTempBuffer;
}
//---------------------------------------------------------------------------
void NiDX9VertexBufferManager::PackVB(unsigned char* pucDestBuffer, 
    unsigned char* pucSrcBuffer, unsigned int uiVertexStride, 
    unsigned int uiDataSize, unsigned int uiNumVerts)
{
    for (unsigned int i = 0; i < uiNumVerts; i++)
    {
        NiMemcpy(pucDestBuffer, pucSrcBuffer, uiDataSize);
        pucDestBuffer += uiVertexStride;
        pucSrcBuffer += uiDataSize;
    }
}
//---------------------------------------------------------------------------
bool NiDX9VertexBufferManager::UnlockVB(LPDIRECT3DVERTEXBUFFER9 pkVB)
{
    NIASSERT(m_pvLockedBuffer);

    NiMemcpy(m_pvLockedBuffer, m_pvTempBuffer, m_uiLockedBufferSize);
    m_pvLockedBuffer = NULL;
    m_uiLockedBufferSize = 0;

    m_kVBCriticalSection.Unlock();

    return SUCCEEDED(pkVB->Unlock());
}
//---------------------------------------------------------------------------
void NiDX9VertexBufferManager::GenerateVertexParameters(NiGeometryData* pkData,
    NiSkinPartition::Partition* pkPartition, unsigned int& uiVertexTypeDesc, 
    unsigned int& uiVertexStride, unsigned int& uiPosOffset, 
    unsigned int& uiWeightOffset, unsigned int& uiNormOffset, 
    unsigned int& uiColorOffset, unsigned int& uiTexOffset)
{
    uiVertexTypeDesc = 0;
    uiVertexStride = 0;

    unsigned short usBones = 0;
    if (pkPartition)
        usBones = pkPartition->m_usBones;
    // Objects with more bones than this need to use an NiShader with
    // matrix palette skinning.
    NIASSERT(usBones <= NiDX9Renderer::HW_BONE_LIMIT);

    //  We know we have XYZ values
    uiPosOffset = uiVertexStride;
    uiVertexStride += 3 * sizeof(float);
    uiWeightOffset = uiVertexStride;

    if (usBones == 0)
    {
        uiVertexTypeDesc |= D3DFVF_XYZ;
    }
    else
    {
        //  Force the packing to use 3 bones... the 4th is calculated by D3D
        uiVertexTypeDesc = D3DFVF_XYZB3;
        uiVertexStride += 3 * sizeof(float);
    }

    if (pkData->GetNormals())
    {
        //  Setup for normal values
        uiVertexTypeDesc |= D3DFVF_NORMAL;
        uiNormOffset = uiVertexStride;
        uiVertexStride += 3 * sizeof(float);
    }

    if (pkData->GetColors())
    {
        //  Setup for diffuse color values
        uiVertexTypeDesc |= D3DFVF_DIFFUSE;
        uiColorOffset = uiVertexStride;
        uiVertexStride += sizeof(unsigned int);
    }

    unsigned int uiT = pkData->GetTextureSets();
    if (uiT == 0)
    {
        // Must add a dummy set of texture UVs, as the SW T&L does not
        // handle things well if they are not present.  Specifically, there
        // are issues with projected texture and dropped objects that have
        // no texture coords.
        uiT = 1;
    }
    else if (uiT > NiD3DPass::ms_uiMaxTextureBlendStages)
    {
        uiT = NiD3DPass::ms_uiMaxTextureBlendStages;
    }
    

    if (uiT)
    {
        uiVertexTypeDesc |= uiT << D3DFVF_TEXCOUNT_SHIFT;
        uiTexOffset = uiVertexStride;
        uiVertexStride += 2 * sizeof(float) * uiT;
    }
}
//---------------------------------------------------------------------------
NiVBChip* NiDX9VertexBufferManager::PackUntransformedVB(
    NiGeometryData* pkData, NiSkinInstance* pkSkin, 
    unsigned short usDirtyFlags, NiVBChip* pkOldVBChip, void* pvLockedBuff)
{
    // Note:  if pkSkin != NULL, we are doing software skinning.

    unsigned short usVerts = pkData->GetActiveVertexCount();

    // Return existing VB immediately if the object has no active vertices
    if (!usVerts)
        return pkOldVBChip;

    NiPoint3* pkP = pkData->GetVertices();
    NiPoint3* pkN = pkData->GetNormals();
    unsigned int uiT = pkData->GetTextureSets();
    NiColorA* pkC = pkData->GetColors();

    // Must clamp the texture coords to be less than or equal to 
    // NiD3DPass::ms_uiMaxTextureBlendStages, or else
    // D3D will crash.
    uiT = (uiT > NiD3DPass::ms_uiMaxTextureBlendStages) ? 
        NiD3DPass::ms_uiMaxTextureBlendStages : uiT;

    unsigned int uiVertexTypeDesc = 0;
    unsigned int uiVertexStride = 0;
    unsigned int uiPosOffset, uiWeightOffset, uiNormOffset, uiTexOffset, 
        uiColorOffset;

    GenerateVertexParameters(pkData, NULL, uiVertexTypeDesc, uiVertexStride, 
        uiPosOffset, uiWeightOffset, uiNormOffset, uiColorOffset, uiTexOffset);

    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkData->GetRendererData();
    pkBuffData->SetFVF(uiVertexTypeDesc);
    pkBuffData->SetVertexStride(0, uiVertexStride);

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

            // if the size, format, or FVF has changed, then skip the old VB
            if ((kVBDesc.Format == D3DFMT_VERTEXDATA) && 
                (kVBDesc.FVF == uiVertexTypeDesc) && 
                (kVBDesc.Size >= (usVerts * uiVertexStride)))
            {
               pkVBChip = pkOldVBChip;
            }
        }

        if (!pkVBChip)
        {
            if (!AllocateBufferSpace(pkBuffData, 0))
                return NULL;

            // Only 1 stream is supported here
            NIASSERT(pkBuffData->GetStreamCount() == 1);
            pkVBChip = pkBuffData->GetVBChip(0);

            // New vertex buffer - we must repack all elements, even if the
            // upper level did not ask for it
            usDirtyFlags = NiGeometryData::DIRTY_MASK;
        }
        NIASSERT(pkVBChip);

        pvLockedBuff = LockVB(pkVBChip->GetVB(), pkVBChip->GetOffset(),
            pkVBChip->GetSize(), pkVBChip->GetLockFlags());

        NIASSERT(pvLockedBuff);
    }

    if (pkSkin)
    {
        // Frame ID check has already occurred at this point - results are
        // in the dirty flags

        if (pkP && (usDirtyFlags & NiGeometryData::VERTEX_MASK) != 0)
        {
            // Skin data present - deform skin into VB
            // Assume normals require skinning if vertices do
            NiPoint3* pkPos = 
                (NiPoint3*)((unsigned char*)pvLockedBuff + uiPosOffset);
            NiPoint3* pkNorm = pkN ? 
                (NiPoint3*)((unsigned char*)pvLockedBuff + uiNormOffset) : 
                NULL;

            // Binormal and tangent require NiD3DShaderDeclarations, since
            // no standard FVF describes them
            pkSkin->Deform(pkData, pkPos, pkNorm, NULL, NULL, uiVertexStride);
        }
    }
    else
    {
        // now, copy the values into the locked buffer
        unsigned char* pucTmp = (unsigned char*)pvLockedBuff + uiPosOffset;

        // It is valid for vertices to be missing if this geometry 
        // has already been packed
        if (pkP && (usDirtyFlags & NiGeometryData::VERTEX_MASK) != 0)
        {
            // Pack points
            PackVB((unsigned char*)pvLockedBuff + uiPosOffset, 
                (unsigned char*)pkP, uiVertexStride, 3 * sizeof(float), 
                usVerts);
        }

        if (pkN && (usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
        {
            // Pack normals
            PackVB((unsigned char*)pvLockedBuff + uiNormOffset, 
                (unsigned char*)pkN, uiVertexStride, 3 * sizeof(float), 
                usVerts);
        }
    }
    
    if (pkC && (usDirtyFlags & NiGeometryData::COLOR_MASK) != 0)
    {
        unsigned char* pucTmp = (unsigned char*)pvLockedBuff + uiColorOffset;
        for (unsigned int ui = 0; ui < usVerts; ui++, pucTmp += uiVertexStride,
            pkC++)
        {
            ((unsigned int*)pucTmp)[0] = 
                ((FastFloatToInt(pkC->a*255.0f))<<24) |
                ((FastFloatToInt(pkC->r*255.0f))<<16) |
                ((FastFloatToInt(pkC->g*255.0f))<<8) |
                (FastFloatToInt(pkC->b*255.0f));
        }
    }

    // must interleave texture coords...
    if ((usDirtyFlags & NiGeometryData::TEXTURE_MASK) != 0)
    {
        for (unsigned int j = 0; j < uiT; j++)
        {
            // Pack texture coords
            PackVB((unsigned char*)pvLockedBuff + uiTexOffset + 
                (j * 2 * sizeof(float)), 
                (unsigned char*)pkData->GetTextureSet(j), 
                uiVertexStride, 2 * sizeof(float), usVerts);
        }
    }

    // unlock the buffer and return it
    if (bUnlock)
    {
        if (UnlockVB(pkVBChip->GetVB()) == false)
            return NULL;
    }

    return pkVBChip;
}
//---------------------------------------------------------------------------
NiVBChip* NiDX9VertexBufferManager::PackTransformedVB(
    NiGeometryBufferData* pkBuffData, unsigned short usVerts, 
    const NiPoint2* pkScreenSpaceVerts, const NiColorA* pkColors, 
    NiPoint2* pkTex, unsigned short usDirtyFlags, NiVBChip* pkOldVBChip, 
    void* pvLockedBuff)
{
    // Return existing VB immediately if the object has no active vertices
    if (usVerts == 0 || pkScreenSpaceVerts == 0)
        return pkOldVBChip;

    unsigned int uiVertexTypeDesc = D3DFVF_XYZRHW;
    unsigned int uiVertexStride = 0;
    unsigned int uiPosOffset, uiTexOffset, uiColorOffset;

    uiPosOffset = uiVertexStride;
    uiVertexStride += 4 * sizeof(float);

    if (pkColors)
    {
        uiVertexTypeDesc |= D3DFVF_DIFFUSE;
        uiColorOffset = uiVertexStride;
        uiVertexStride += sizeof(unsigned int);
    }

    if (pkTex)
    {
        uiVertexTypeDesc |= 1 << D3DFVF_TEXCOUNT_SHIFT;
        uiTexOffset = uiVertexStride;
        uiVertexStride += 2 * sizeof(float);
    }

    pkBuffData->SetFVF(uiVertexTypeDesc);
    pkBuffData->SetVertexStride(0, uiVertexStride);

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
        if (pkOldVBChip && pkOldVBChip->GetVB())
        {
            D3DVERTEXBUFFER_DESC kVBDesc;

            pkOldVBChip->GetVB()->GetDesc(&kVBDesc);

            // if the size, format, or FVF has changed, then skip the old VB
            if ((kVBDesc.Format == D3DFMT_VERTEXDATA) && 
                (kVBDesc.FVF == uiVertexTypeDesc) && 
                (kVBDesc.Size >= (usVerts * uiVertexStride)))
            {
               pkVBChip = pkOldVBChip;
            }
        }
    
        if (!pkVBChip)
        {
            if (!AllocateBufferSpace(pkBuffData, 0))
                return NULL;

            // Only 1 stream is supported here
            NIASSERT(pkBuffData->GetStreamCount() == 1);
            pkVBChip = pkBuffData->GetVBChip(0);
        }
        NIASSERT(pkVBChip);

        pvLockedBuff = LockVB(pkVBChip->GetVB(), pkVBChip->GetOffset(),
            pkVBChip->GetSize(), pkVBChip->GetLockFlags());

        NIASSERT(pvLockedBuff);
    }

    // now, transform the values into the locked buffer
    unsigned char* pucTmp = NULL;
    unsigned short us = 0;
    if (pkScreenSpaceVerts && 
        (usDirtyFlags & NiScreenTexture::VERTEX_MASK) != 0)
    {
        pucTmp = (unsigned char*)pvLockedBuff + uiPosOffset;
        for (; us < usVerts; us++, pucTmp += uiVertexStride, 
            pkScreenSpaceVerts++)
        {
            unsigned int uiByteSize = 2 * sizeof(float);
            NiMemcpy(pucTmp, pkScreenSpaceVerts, uiByteSize);
            ((float*)pucTmp)[2] = 0.0f;
            ((float*)pucTmp)[3] = 1.0f;
        }
    }

    if (pkColors && (usDirtyFlags & NiScreenTexture::COLOR_MASK) != 0)
    {
        pucTmp = (unsigned char*)pvLockedBuff + uiColorOffset;
        for (us = 0; us < usVerts; us++, pucTmp += uiVertexStride, pkColors++)
        {
            ((unsigned int*)pucTmp)[0] = 
                ((FastFloatToInt(pkColors->a*255.0f))<<24) |
                ((FastFloatToInt(pkColors->r*255.0f))<<16) |
                ((FastFloatToInt(pkColors->g*255.0f))<<8) |
                (FastFloatToInt(pkColors->b*255.0f));
        }
    }

    // must interleave texture coords...
    if (pkTex && (usDirtyFlags & NiScreenTexture::TEXTURE_MASK) != 0)
    {
        pucTmp = (unsigned char*)pvLockedBuff + uiTexOffset;
        for (us = 0; us < usVerts; us++, pucTmp += uiVertexStride, pkTex++)
        {
            unsigned int uiByteSize = 2 * sizeof(float);
            NiMemcpy(pucTmp, pkTex, uiByteSize);
        }
    }

    // unlock the buffer and return it
    if (bUnlock)
    {
        if (UnlockVB(pkVBChip->GetVB()) == false)
            return NULL;
    }

    return pkVBChip;
}
//---------------------------------------------------------------------------
NiVBChip* NiDX9VertexBufferManager::PackSkinnedVB(NiGeometryData* pkData, 
    const NiSkinInstance* pkSkin, NiSkinPartition::Partition* pkPartition, 
    unsigned short usDirtyFlags, NiVBChip* pkOldVBChip, void* pvLockedBuff)
{
    NIASSERT(pkSkin);
    NIASSERT(pkPartition);

    NiPoint3* pkP = pkData->GetVertices();
    NiPoint3* pkN = pkData->GetNormals();
    unsigned int uiT = pkData->GetTextureSets();
    NiColorA* pkC = pkData->GetColors();

    // Must clamp the texture coords to be less than or equal to 
    // NiD3DPass::ms_uiMaxTextureBlendStages, or else
    // D3D will crash.
    uiT = (uiT > NiD3DPass::ms_uiMaxTextureBlendStages) ? 
        NiD3DPass::ms_uiMaxTextureBlendStages : uiT;

    //  Determine the vertex format (w/ Blend Weights)
    unsigned short usBones = pkPartition->m_usBones;
    NIASSERT((usBones > 0) && (usBones <= NiDX9Renderer::HW_BONE_LIMIT));

    unsigned short* pusVertMap = pkPartition->m_pusVertexMap;
    unsigned short usVerts = pkPartition->m_usVertices;

    unsigned int uiVertexTypeDesc = 0;
    unsigned int uiVertexStride = 0;
    unsigned int uiPosOffset, uiWeightOffset, uiNormOffset, uiTexOffset, 
        uiColorOffset;

    GenerateVertexParameters(pkData, pkPartition, uiVertexTypeDesc, 
        uiVertexStride, uiPosOffset, uiWeightOffset, uiNormOffset, 
        uiColorOffset, uiTexOffset);

    NiGeometryBufferData* pkBuffData = 
        (NiGeometryBufferData*)pkPartition->m_pkBuffData;
    pkBuffData->SetFVF(uiVertexTypeDesc);
    pkBuffData->SetVertexStride(0, uiVertexStride);

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

            // if the size, format, or FVF has changed, then skip the old VB
            if ((kVBDesc.Format == D3DFMT_VERTEXDATA) && 
                (kVBDesc.FVF == uiVertexTypeDesc) && 
                (kVBDesc.Size >= (usVerts * uiVertexStride)))
            {
               pkVBChip = pkOldVBChip;
            }
        }    
    
        if (!pkVBChip)
        {
            if (!AllocateBufferSpace(pkBuffData, 0))
                return NULL;

            // Only 1 stream is supported here
            NIASSERT(pkBuffData->GetStreamCount() == 1);
            pkVBChip = pkBuffData->GetVBChip(0);

            // New vertex buffer - we must repack all elements, even if the
            // upper level did not ask for it
            usDirtyFlags = NiGeometryData::DIRTY_MASK;
        }
        NIASSERT(pkVBChip);

        pvLockedBuff = LockVB(pkVBChip->GetVB(), pkVBChip->GetOffset(),
            pkVBChip->GetSize(), pkVBChip->GetLockFlags());

        NIASSERT(pkVBChip && pvLockedBuff);
    }

    // now, copy the values into the locked buffer
    unsigned char* pucTmp = (unsigned char*)pvLockedBuff + uiPosOffset;
    unsigned int ui;
    if (pkP && (usDirtyFlags & NiGeometryData::VERTEX_MASK) != 0)
    {
        for (ui = 0; ui < usVerts; ui++, pucTmp += uiVertexStride)
        {
            // Use the vertex map!
            unsigned int uiByteSize = sizeof(NiPoint3);
            NiMemcpy(pucTmp, &pkP[pusVertMap[ui]], uiByteSize);
        }

        // Copy the weights...
        // Use VERTEX_MASK for weights as well
        pucTmp = (unsigned char*)pvLockedBuff + uiWeightOffset;
        float* pfWeight = pkPartition->m_pfWeights;
        for (ui = 0; ui < usVerts; ui++, pucTmp += uiVertexStride)
        {
            float fTestVal = 1.0f;
            for (unsigned int uiTemp = 0; uiTemp < usBones; uiTemp++)
                fTestVal -= pfWeight[uiTemp];

            if (fabs(fTestVal) > 1e-5f)
            {
                char acTemp[256];

                NiSprintf(acTemp, 256, "ILLEGAL WEIGHTS! Vertex %4d - "
                    "Weight Sum %8.5f  (%8.5f)\n", ui, 1.0f - fTestVal, 
                    fTestVal);
                NiOutputDebugString(acTemp);
            }

            unsigned short us;
            float* pfTemp = (float*)pucTmp;
            for (us = 0; us < usBones - 1; us++)
                pfTemp[us] = pfWeight[us];
            if ((usBones == 3) || (usBones == 1))
                pfTemp[us] = pfWeight[us];

            pfWeight += usBones;
        }
    }

    if (pkN && (usDirtyFlags & NiGeometryData::NORMAL_MASK) != 0)
    {
        pucTmp = (unsigned char*)pvLockedBuff + uiNormOffset;
        for (ui = 0; ui < usVerts; ui++, pucTmp += uiVertexStride)
        {
            unsigned int uiByteSize = sizeof(NiPoint3);
            NiMemcpy(pucTmp, &pkN[pusVertMap[ui]], uiByteSize);
        }
    }
    
    if (pkC && (usDirtyFlags & NiGeometryData::COLOR_MASK) != 0)
    {
        pucTmp = (unsigned char*)pvLockedBuff + uiColorOffset;
        for (ui = 0; ui < usVerts; ui++, pucTmp += uiVertexStride)
        {
            ((unsigned int*)pucTmp)[0] = 
                ((FastFloatToInt(pkC[pusVertMap[ui]].a * 255.0f)) << 24) |
                ((FastFloatToInt(pkC[pusVertMap[ui]].r * 255.0f)) << 16) |
                ((FastFloatToInt(pkC[pusVertMap[ui]].g * 255.0f)) << 8) |
                (FastFloatToInt(pkC[pusVertMap[ui]].b * 255.0f));
        }
    }

    // must interleave texture coords...
    if ((usDirtyFlags & NiGeometryData::TEXTURE_MASK) != 0)
    {
        NiPoint2* pkT;

        for (unsigned int j = 0; j < uiT; j++)
        {
            pucTmp = (unsigned char*)pvLockedBuff + uiTexOffset + 
                (j*2*sizeof(float));
            pkT = pkData->GetTextureSet(j);

            for (ui = 0; ui < usVerts; ui++, pucTmp += uiVertexStride)
            {
                unsigned int uiByteSize = 2*sizeof(float);
                NiMemcpy(pucTmp, &pkT[pusVertMap[ui]], uiByteSize);
            }
        }
    }

    // unlock the buffer and return it
    if (bUnlock)
    {
        if (UnlockVB(pkVBChip->GetVB()) == false)
            return NULL;
    }

    return pkVBChip;
}
//---------------------------------------------------------------------------
NiDX9VertexBufferManager::NiDX9VBInfo* 
    NiDX9VertexBufferManager::GetVBInfo(unsigned int uiUsage, 
    unsigned int uiFVF, D3DPOOL ePool)
{
#ifdef _DEBUG
    //  Test to confirm the indexing method won't cause problems
    unsigned int uiUsageTest = uiUsage & 0x0000FFFF;
    NIASSERT(uiUsageTest == uiUsage);
    unsigned int uiFVFTest = uiFVF & 0x0000FFFF;
    NIASSERT(uiFVFTest == uiFVF);
#endif
    //  Form the index value
    unsigned int uiIndex = ((uiUsage << 16) & 0xFFFF0000) | uiFVF;
    NiDX9VBInfo* pkVBInfo = NULL;

    switch (ePool)
    {
    case D3DPOOL_DEFAULT:
        if (m_kDefaultResources.GetAt(uiIndex, pkVBInfo) == false)
        {
            //  Doesn't exist, add it
            pkVBInfo = CreateVBInfo();
            pkVBInfo->m_uiVBID = uiIndex;
            m_kDefaultResources.SetAt(uiIndex, pkVBInfo);
        }
        break;
    case D3DPOOL_MANAGED:
        if (m_kManagedResources.GetAt(uiIndex, pkVBInfo) == false)
        {
            //  Doesn't exist, add it
            pkVBInfo = CreateVBInfo();
            pkVBInfo->m_uiVBID = uiIndex;
            m_kManagedResources.SetAt(uiIndex, pkVBInfo);
        }
        break;
    case D3DPOOL_SYSTEMMEM:
        if (m_kSystemMemResources.GetAt(uiIndex, pkVBInfo) == false)
        {
            //  Doesn't exist, add it
            pkVBInfo = CreateVBInfo();
            pkVBInfo->m_uiVBID = uiIndex;
            m_kSystemMemResources.SetAt(uiIndex, pkVBInfo);
        }
        break;
    default:
        NIASSERT(false);
        break;
    }

    return pkVBInfo;
}
//---------------------------------------------------------------------------
NiDX9VertexBufferManager::NiDX9VBInfo* 
    NiDX9VertexBufferManager::CreateVBInfo()
{
    NiDX9VBInfo* pkVBInfo = NiNew NiDX9VBInfo;
    NIASSERT(pkVBInfo);
    pkVBInfo->m_uiVBID = 0;
    pkVBInfo->m_ucIndex = 0;
    memset((void*)&(pkVBInfo->m_pkVBRes), 0, 
        sizeof(NiDX9VBResource*) * DX9_MAX_VBS);
    
    return pkVBInfo;
}
//---------------------------------------------------------------------------
LPDIRECT3DVERTEXBUFFER9 NiDX9VertexBufferManager::CreateVertexBuffer(
    unsigned int uiLength, unsigned int uiUsage, unsigned int uiFVF, 
    D3DPOOL ePool, D3DVERTEXBUFFER_DESC* pkDesc)
{
    LPDIRECT3DVERTEXBUFFER9 pkVB9 = NULL;

    NIASSERT(m_pkD3DDevice9);

    HRESULT eD3dRet = m_pkD3DDevice9->CreateVertexBuffer(uiLength, uiUsage, 
        uiFVF, ePool, &pkVB9, NULL);
    if (SUCCEEDED(eD3dRet))
    {
        if (pkDesc)
        {
            pkVB9->GetDesc(pkDesc);
        }

        NIMETRICS_DX9RENDERER_AGGREGATEVALUE(VERTEX_BUFFER_SIZE, uiLength);
    }
    else
    {
        NiDX9Renderer::Error("NiDX9VertexBufferManager::CreateVertexBuffer "
            "FAILED\n    %s", NiDX9ErrorString((unsigned int)eD3dRet));
        pkVB9 = NULL;
    }

    return pkVB9;
}
//---------------------------------------------------------------------------
void NiDX9VertexBufferManager::DestroyDefaultBuffers()
{
    NiTMapIterator kPos = m_kDefaultResources.GetFirstPos();
    while (kPos)
    {
        unsigned int uiKey;
        NiDX9VBInfo* pkVBInfo;
        m_kDefaultResources.GetNext(kPos, uiKey, pkVBInfo);

        //  Kill the vertex buffers...
        if (pkVBInfo)
        {
            NiDX9VBResource* pkVBRes;
        
            for (unsigned int i = 0; i < DX9_MAX_VBS; i++)
            {
                pkVBRes = pkVBInfo->m_pkVBRes[i];
                if (pkVBRes)
                {
                    if (pkVBRes->m_pkVB)
                    {
                        NiD3DRenderer::ReleaseVBResource(pkVBRes->m_pkVB);
                    }
                }
                NiDelete pkVBRes;
            }
            NiDelete pkVBInfo;
        }
    }

    m_kDefaultResources.RemoveAll();
}
//---------------------------------------------------------------------------
void NiDX9VertexBufferManager::EndFrame()
{
    m_uiFrameID++;
}
//---------------------------------------------------------------------------
unsigned int NiDX9VertexBufferManager::GetFrameID() const
{
    return m_uiFrameID;
}
//---------------------------------------------------------------------------
