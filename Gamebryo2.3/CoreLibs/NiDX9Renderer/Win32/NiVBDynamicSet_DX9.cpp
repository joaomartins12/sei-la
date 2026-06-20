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

#include "NiVBDynamicSet.h"
#include "NiD3DDefines.h"

//---------------------------------------------------------------------------
unsigned int NiVBDynamicSet::ms_uiDefBlockSize = 
    NiVBDynamicSet::NIVBDYNAMICSET_DEF_BLOCKSIZE;
//---------------------------------------------------------------------------
D3DVertexBufferPtr NiVBDynamicSet::AllocateBufferSpace(
    unsigned int uiSize, unsigned int& uiOffset, unsigned int& uiFlags, 
    bool bForceDiscard, bool bSoftwareVP)
{
    //  Make sure the block is big enough to begin with...
    if (m_uiBlockSize < uiSize)
    {
        //  It's not, release it...
        ReleaseVertexBuffer();
        //  Bump the block size. Make sure to note this in the stats tracking
        SetBlockSize(uiSize);
    }

    //  Create the vertex buffer, if needed
    if (m_pkVB == 0)
        CreateVertexBuffer(bSoftwareVP);

    NIASSERT(m_pkVB);

    //  Is there enough space in the current buffer?
    if (bForceDiscard || (m_uiBlockSize - m_uiCurrOffset) < uiSize)
    {
        //  Not enough space... tell the system to discard the old one
        uiOffset = 0;
        m_uiCurrOffset = uiSize;
        uiFlags = D3DLOCK_DISCARD;
    }
    else
    {
        //  Enough space.
        uiOffset = m_uiCurrOffset;
        m_uiCurrOffset += uiSize;

        //  Safety check...
        NIASSERT(m_uiCurrOffset <= m_uiBlockSize);

        uiFlags = D3DLOCK_NOOVERWRITE;
    }

    // DISCARD needs to be specified on the first object of the block
    if (uiOffset == 0)
        uiFlags = D3DLOCK_DISCARD;

    return m_pkVB;
}
//---------------------------------------------------------------------------
void NiVBDynamicSet::FreeBufferSpace(D3DVertexBufferPtr pkVB)
{
    // Do nothing on DX9
}
//---------------------------------------------------------------------------
void NiVBDynamicSet::CreateVertexBuffer(bool bSoftwareVP)
{
    NIASSERT(m_pkD3DDevice);
    //  Safety check... maybe we should just return?
    NIASSERT(!m_pkVB);

    unsigned int uiUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
    if (bSoftwareVP)
        uiUsage |= D3DUSAGE_SOFTWAREPROCESSING;
    
    //  Create a vertex buffer of the determined size...
    HRESULT d3dRet = m_pkD3DDevice->CreateVertexBuffer(m_uiBlockSize, 
        uiUsage, m_uiFVF, D3DPOOL_DEFAULT, &m_pkVB, NULL);

    if (FAILED(d3dRet))
    {
        NIASSERT(!"NiVBDynamicSet> CreateVertexBuffer FAILED!");
        return;
    }

    //  Ensure that we start at the beginning of the buffer
    m_uiCurrOffset = 0;

    NIMETRICS_DX9RENDERER_AGGREGATEVALUE(VERTEX_BUFFER_SIZE, m_uiBlockSize);
}
//---------------------------------------------------------------------------
void NiVBDynamicSet::ReleaseVertexBuffer(void)
{
    if (m_pkVB)
    {
        NiD3DRenderer::ReleaseVBResource(m_pkVB);
    }
    m_pkVB = 0;
}
//---------------------------------------------------------------------------
