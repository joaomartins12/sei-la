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

#include "NiVBSet.h"

//---------------------------------------------------------------------------
NiVBBlock* NiVBSet::ms_pkBlockDump = 0;
unsigned int NiVBSet::ms_uiDefBlockAllocSize = 32;
unsigned int NiVBSet::ms_uiDefBlockSize = NIVBSET_DEF_BLOCKSIZE;
unsigned int NiVBSet::ms_uiDefFreeSpaceThreshold = NIVBSET_DEF_FREETHRESHOLD;
//---------------------------------------------------------------------------
NiVBSet::~NiVBSet()
{
    // Release blocks
    for (unsigned int i = 0; i < m_apkBlocks.GetSize(); i++)
    {
        NiVBBlock* pkBlock = m_apkBlocks.RemoveAt(i);
        FreeBlock(pkBlock);
    }

    //  Release our D3DDevice
    if (m_pkD3DDevice)
        NiD3DRenderer::ReleaseDevice(m_pkD3DDevice);
    m_pkD3DDevice = 0;
}
//---------------------------------------------------------------------------
NiVBChip* NiVBSet::AllocateChip(unsigned int uiSize, bool bForceNewBlock)
{
    NIASSERT(uiSize != 0);

    NiVBChip* pkVBChip = 0;

    // See if there is a block with enough free space
    NiVBBlock* pkVBBlock = NULL;
    if (bForceNewBlock == false)
        pkVBBlock = FindBlockWithFreeSpace(uiSize);
    if (!pkVBBlock)
    {
        //  There wasn't one with enough space... create a new one
        pkVBBlock = CreateBlock(uiSize);
    }

    if (pkVBBlock)
    {
        pkVBChip = pkVBBlock->AllocateChip(uiSize);

#if defined(_NIVB_TRACK_STATS_)
        m_uiRunningChipSize += uiSize;
        m_uiRunningChipCount++
#endif  //#if defined(_NIVB_TRACK_STATS_)
        
        //  Now, if there is free space > threshold, add to free list
        if (pkVBBlock->GetFreeBytes() > m_uiFreeSpaceThreshold)
            InsertBlockIntoFreeSpaceList(pkVBBlock);
    }

    return pkVBChip;
}
//---------------------------------------------------------------------------
void NiVBSet::FreeChip(unsigned int uiBlockIndex, unsigned int uiChipIndex)
{
    //  Find the block...
    NiVBBlock* pkVBBlock = m_apkBlocks.GetAt(uiBlockIndex);

    if (!pkVBBlock)
        return;

    unsigned int uiPreFreeSpace = pkVBBlock->GetFreeBytes();

#if defined(_NIVB_TRACK_STATS_)
    unsigned int uiChipSize = pkVBBlock->GetChipSize(uiChipIndex);
    m_uiRunningChipSize -= uiChipSize;
    m_uiRunningChipCount--;
#endif  //#if defined(_NIVB_TRACK_STATS_)

    //  Free the chip.
    pkVBBlock->FreeChip(uiChipIndex);

    if (uiPreFreeSpace != pkVBBlock->GetFreeBytes())
    {
        //  If not, shift it's position in the list.
        RemoveBlockFromFreeSpaceList(pkVBBlock);
    
        //  See if the block is totally empty now... if so, free it
        if (pkVBBlock->GetTotalFreeBytes() == pkVBBlock->GetSize())
        {
            //  The block is now completely empty...
            //  Free it and throw it on the empty block list
            m_apkBlocks.RemoveAt(uiBlockIndex);
            FreeBlock(pkVBBlock);
        }
        else
        {
            //  Reinsert it
            InsertBlockIntoFreeSpaceList(pkVBBlock);
        }
    }
    else
    {
        //  We can leave it be, since it has the same 'free' space
    }
}
//---------------------------------------------------------------------------
void NiVBSet::FreeBlock(NiVBBlock* pkBlock)
{
    if (pkBlock == 0)
        return;

    D3DVertexBufferPtr pkVB = pkBlock->GetVB();
    if (pkVB)
    {
        NiD3DRenderer::ReleaseVBResource(pkVB);
        pkBlock->SetVB(0);
    }

#if defined(_NIVB_TRACK_STATS_)
    m_uiRunningBlockBytesAllocated -= pkBlock->GetTotalFreeBytes();
    m_uiRunningBlockCount--;
#endif  //#if defined(_NIVB_TRACK_STATS_)

    pkBlock->ClearAllData();

    //  Add to the 'dump'
    AddToBlockDump(pkBlock);
}
//---------------------------------------------------------------------------
void NiVBSet::DetermineUsageAndPool(NiVBSet* pkSet)
{
    pkSet->m_uiUsage = D3DUSAGE_WRITEONLY;
    if ((pkSet->m_uiFlags & NIVBSET_FLAG_SOFTWAREVP) != 0)
        pkSet->m_uiUsage |= D3DUSAGE_SOFTWAREPROCESSING;
    pkSet->m_ePool = D3DPOOL_MANAGED;
}
//---------------------------------------------------------------------------
NiVBBlock* NiVBSet::CreateBlock(unsigned int uiSize)
{
    // Depending on the flags, create a block of the SetSize, or,
    // just create a block the given size...
    unsigned int uiVBSize;

    if (m_uiFlags & 0x10000000 || uiSize > m_uiBlockSize)
        uiVBSize = uiSize;
    else
        uiVBSize = m_uiBlockSize;

#if defined(_NIVB_TRACK_STATS_)
    m_uiTotalBlockBytesAllocated += uiVBSize;
    m_uiTotalBlockCount++;
    m_uiRunningBlockBytesAllocated += uiVBSize;
    m_uiRunningBlockCount++;
#endif  //#if defined(_NIVB_TRACK_STATS_)

    //  Create a vertex buffer of the determined size...
    D3DVertexBufferPtr pkVB = 0;
    
    HRESULT d3dRet = m_pkD3DDevice->CreateVertexBuffer(uiVBSize, m_uiUsage, 
        m_uiFVF, m_ePool, &pkVB, NULL);

    if (FAILED(d3dRet))
        return 0;

    NIMETRICS_DX9RENDERER_AGGREGATEVALUE(VERTEX_BUFFER_SIZE, uiVBSize);

    //  Create a VBBlock for this vertex buffer...
    NiVBBlock* pkVBBlock = GetFreeBlock();
    if (pkVBBlock)
    {
        pkVBBlock->SetSet(this);
        pkVBBlock->SetVB(pkVB);
        pkVBBlock->SetSize(uiVBSize);
        pkVBBlock->SetFVF(m_uiFVF);
        pkVBBlock->SetFlags(m_uiFlags);
        pkVBBlock->SetPool(m_ePool);
        pkVBBlock->SetUsage(m_uiUsage);

        //  Add the block to the array...
        unsigned int uiIndex = m_apkBlocks.AddFirstEmpty(pkVBBlock);
        NIASSERT(uiIndex != 0xffffffff);

        pkVBBlock->SetIndex(uiIndex);
    }

    return pkVBBlock;
}
//---------------------------------------------------------------------------
void NiVBSet::DestroyBlock(unsigned int uiBlockIndex)
{
    NiVBBlock* pkVBBlock = m_apkBlocks.GetAt(uiBlockIndex);
    if (pkVBBlock)
    {
        //  Free the vertex buffer associated with it...
        D3DVertexBufferPtr pkVB = pkVBBlock->GetVB();
        //  There had better be one??? assert if this is the case
        if (pkVB)
            NiD3DRenderer::ReleaseVBResource(pkVB);

        //  See if it is in the 'free' list & if so, remove it
        RemoveBlockFromFreeSpaceList(pkVBBlock);

        //  Remove from the array
        m_apkBlocks.SetAt(uiBlockIndex, 0);

        //  Add to the 'dump'
        AddToBlockDump(pkVBBlock);
    }
}
//---------------------------------------------------------------------------
NiVBBlock* NiVBSet::FindBlockWithFreeSpace(unsigned int uiFreeSize)
{
    //  Do we want to do a linear search through this list?
    //  Is so, when we add it, do we want to add it in inc/decr order???
    NiVBBlock* pkVBBlock = 0;
    NiVBBlock* pkVBCheckBlock = m_pkBlockList;

    while (pkVBCheckBlock)
    {
        if (pkVBCheckBlock->GetFreeBytes() >= uiFreeSize)
        {
            //  Got one.
            pkVBBlock = pkVBCheckBlock;
            //  Remove it from the list...
            if (pkVBCheckBlock->m_pkNextBlock)
            {
                pkVBCheckBlock->m_pkNextBlock->m_pkPrevBlock = 
                    pkVBCheckBlock->m_pkPrevBlock;
            }
            if (pkVBCheckBlock->m_pkPrevBlock)
            {
                pkVBCheckBlock->m_pkPrevBlock->m_pkNextBlock = 
                    pkVBCheckBlock->m_pkNextBlock;
            }
            //  See if it was the top
            if (pkVBCheckBlock == m_pkBlockList)
                m_pkBlockList = pkVBCheckBlock->m_pkNextBlock;

            break;
        }

        pkVBCheckBlock = pkVBCheckBlock->m_pkNextBlock;
    }

    return pkVBBlock;
}
//---------------------------------------------------------------------------
void NiVBSet::InsertBlockIntoFreeSpaceList(NiVBBlock* pkVBBlock)
{
    NIASSERT(pkVBBlock);

    // Determine list ordering method:
    // For now, do an ordered insertion - low to high.
    // This is to ensure that we pack as efficiently as possibly,
    // in terms of memory usage.
    if (m_pkBlockList)
    {
        //  See if it goes on the front.
        NiVBBlock* pkVBPrevBlock = 0;
        NiVBBlock* pkVBCheckBlock = m_pkBlockList;
        bool bInserted = false;

        while (pkVBCheckBlock)
        {
            if (pkVBCheckBlock->GetFreeBytes() >
                pkVBBlock->GetFreeBytes())
            {
                //  Stick it in here.
                if (pkVBPrevBlock)
                    pkVBPrevBlock->m_pkNextBlock = pkVBBlock;
                pkVBCheckBlock->m_pkPrevBlock = pkVBBlock;
                pkVBBlock->m_pkNextBlock = pkVBCheckBlock;
                pkVBBlock->m_pkPrevBlock = pkVBPrevBlock;

                if (pkVBCheckBlock == m_pkBlockList)
                    m_pkBlockList = pkVBBlock;

                bInserted = true;
                break;
            }

            pkVBPrevBlock = pkVBCheckBlock;
            pkVBCheckBlock = pkVBCheckBlock->m_pkNextBlock;
        }

        //  Tag it on the end of the list
        if (!bInserted)
        {
            //  pkVBPrevBlock should be the last block in the list
            NIASSERT(pkVBPrevBlock);

            pkVBPrevBlock->m_pkNextBlock = pkVBBlock;
            pkVBBlock->m_pkPrevBlock = pkVBPrevBlock;
            pkVBBlock->m_pkNextBlock = 0;
        }
    }
    else
    {
        m_pkBlockList = pkVBBlock;
        m_pkBlockList->m_pkNextBlock = 0;
        m_pkBlockList->m_pkPrevBlock = 0;
    }
}
//---------------------------------------------------------------------------
void NiVBSet::RemoveBlockFromFreeSpaceList(NiVBBlock* pkVBBlock)
{
    NiVBBlock* pkVBCheckBlock = m_pkBlockList;

    while (pkVBCheckBlock)
    {
        if (pkVBCheckBlock == pkVBBlock)
        {
            //  Remove it from the list...
            if (pkVBCheckBlock->m_pkNextBlock)
            {
                pkVBCheckBlock->m_pkNextBlock->m_pkPrevBlock = 
                    pkVBCheckBlock->m_pkPrevBlock;
            }
            if (pkVBCheckBlock->m_pkPrevBlock)
            {
                pkVBCheckBlock->m_pkPrevBlock->m_pkNextBlock = 
                    pkVBCheckBlock->m_pkNextBlock;
            }
            //  See if it was the top
            if (pkVBCheckBlock == m_pkBlockList)
                m_pkBlockList = pkVBCheckBlock->m_pkNextBlock;

            break;
        }

        pkVBCheckBlock = pkVBCheckBlock->m_pkNextBlock;
    }
}
//---------------------------------------------------------------------------
NiVBBlock* NiVBSet::GetFreeBlock(void)
{
    if (ms_pkBlockDump == 0)
        AllocateBlocks();

    NiVBBlock* pkVBBlock = ms_pkBlockDump;
    NIASSERT(pkVBBlock);

    ms_pkBlockDump = ms_pkBlockDump->m_pkNextBlock;
    if (ms_pkBlockDump)
        ms_pkBlockDump->m_pkPrevBlock = 0;

    pkVBBlock->m_pkNextBlock = 0;
    pkVBBlock->m_pkPrevBlock = 0;

    return pkVBBlock;
}
//---------------------------------------------------------------------------
void NiVBSet::AllocateBlocks(void)
{
    NIASSERT(ms_uiDefBlockAllocSize != 0);
    //  This should not be called if there are already blocks in the dump
    NIASSERT(!ms_pkBlockDump);

    NiVBBlock* pkVBBlock;

    for (unsigned int ui = 0; ui < ms_uiDefBlockAllocSize; ui++)
    {
        pkVBBlock = NiNew NiVBBlock;
        NIASSERT(pkVBBlock);

        //  Add to the 'dump'
        AddToBlockDump(pkVBBlock);
    }
}
//---------------------------------------------------------------------------
void NiVBSet::DeleteBlocks(void)
{
    //  Delete the chips from the blocks...
    NiVBBlock::DeleteChips();

    //  Delete the block pool
    NiVBBlock* pkVBBlock = ms_pkBlockDump;
    NiVBBlock* pkNext;

    while (pkVBBlock)
    {
        pkNext = pkVBBlock->m_pkNextBlock;

        NiDelete pkVBBlock;

        pkVBBlock = pkNext;
    }
    ms_pkBlockDump = 0;
}
//---------------------------------------------------------------------------
