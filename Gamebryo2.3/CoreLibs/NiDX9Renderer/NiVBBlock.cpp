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

#include "NiVBBlock.h"
#include "NiVBSet.h"

//---------------------------------------------------------------------------
NiVBChip* NiVBBlock::ms_pkChipDump = 0;
unsigned int NiVBBlock::ms_uiDefClipAllocSize = 32;
//---------------------------------------------------------------------------
NiVBBlock::~NiVBBlock()
{
    /* */
}
//---------------------------------------------------------------------------
NiVBChip* NiVBBlock::AllocateChip(unsigned int uiSize)
{
    if (m_uiFreeBytes < uiSize)
        return 0;

    //  Grab a free chip...
    NiVBChip* pkChip = GetFreeChip();
    NIASSERT(pkChip);

    //  Set some parameters...
    pkChip->SetBlock(this);
    pkChip->SetVB(m_pkVB);
    pkChip->SetOffset(m_uiCurrOffset);
    pkChip->SetSize(uiSize);

    //  Update local tracking parameters
    m_uiCurrOffset += uiSize;
    m_uiFreeBytes -= uiSize;
    m_uiTotalFreeBytes -= uiSize;

    //  Add to the array...
    unsigned int uiIndex = m_apkChips.AddFirstEmpty(pkChip);
    NIASSERT(uiIndex != 0xffffffff);

    pkChip->SetIndex(uiIndex);

    return pkChip;
}
//---------------------------------------------------------------------------
unsigned int NiVBBlock::FreeChip(unsigned int uiChipIndex)
{
    unsigned int uiFreed = 0;

    NiVBChip* pkVBChip = m_apkChips.GetAt(uiChipIndex);
    if (pkVBChip)
    {
        unsigned int uiChipSize = pkVBChip->GetSize();
        if ((m_uiCurrOffset - uiChipSize) == pkVBChip->GetOffset())
        {
            //  This is the last chip in the block, in terms of order.
            //  Find current last block to reset offset and free size
            m_uiCurrOffset = 0;
            NIASSERT(uiChipIndex < m_apkChips.GetSize());
            for (unsigned int i = 0; i < uiChipIndex; i++)
            {
                NiVBChip* pkChip = m_apkChips.GetAt(i);
                if (pkChip)
                {
                    unsigned int uiOffset = 
                        pkChip->GetOffset() + pkChip->GetSize();
                    if (uiOffset > m_uiCurrOffset)
                        m_uiCurrOffset = uiOffset;
                }
            }
        }
        m_uiFreeBytes = m_uiSize - m_uiCurrOffset;
        m_uiTotalFreeBytes += uiChipSize;
        uiFreed = uiChipSize;

        //  Remove it from the array...
        m_apkChips.SetAt(uiChipIndex, 0);

        //  Add to the 'dump'
        pkVBChip->m_pkPrevChip = 0;
        pkVBChip->m_pkNextChip = ms_pkChipDump;
        if (ms_pkChipDump)
            ms_pkChipDump->m_pkPrevChip = pkVBChip;
        ms_pkChipDump = pkVBChip;
    }

    return uiFreed;
}
//---------------------------------------------------------------------------
NiVBChip* NiVBBlock::GetFreeChip(void)
{
    if (ms_pkChipDump == 0)
        AllocateChips();

    NiVBChip* pkVBChip = ms_pkChipDump;
    NIASSERT(pkVBChip);

    ms_pkChipDump = ms_pkChipDump->m_pkNextChip;
    if (ms_pkChipDump)
        ms_pkChipDump->m_pkPrevChip = 0;

    pkVBChip->m_pkNextChip = 0;
    pkVBChip->m_pkPrevChip = 0;

    return pkVBChip;
}
//---------------------------------------------------------------------------
void NiVBBlock::AllocateChips(void)
{
    NIASSERT(ms_pkChipDump == 0);
    NIASSERT(ms_uiDefClipAllocSize != 0);

    NiVBChip* pkVBChip;

    for (unsigned int ui = 0; ui < ms_uiDefClipAllocSize; ui++)
    {
        pkVBChip = NiNew NiVBChip;
        NIASSERT(pkVBChip);

        if (ms_pkChipDump)
            ms_pkChipDump->m_pkPrevChip = pkVBChip;
        pkVBChip->m_pkNextChip = ms_pkChipDump;
        pkVBChip->m_pkPrevChip = 0;
        ms_pkChipDump = pkVBChip;
    }
}
//---------------------------------------------------------------------------
void NiVBBlock::DeleteChips(void)
{
    //  Delete the Chip pool
    NiVBChip* pkVBChip = ms_pkChipDump;
    NiVBChip* pkNext;

    while (pkVBChip)
    {
        pkNext = pkVBChip->m_pkNextChip;

        NiDelete pkVBChip;

        pkVBChip = pkNext;
    }
    ms_pkChipDump = 0;
}
//---------------------------------------------------------------------------
