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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiFontPCH.h"

#include "NiFontString.h"
#include "NiFont.h"

//---------------------------------------------------------------------------
NiImplementRootRTTI(NiFontString);
//---------------------------------------------------------------------------
NiWChar NiFontString::ms_akWorkingString[NI_MAX_PATH] = { 0 };
//---------------------------------------------------------------------------
NiFontString::NiFontString(NiFont* pkFont, 
    unsigned int uiFlags, unsigned int uiMaxStringLen, 
    const char* pcString, const NiColorA& kColor,
    unsigned char ucDirection /* = NIFONTSTRING_LEFT_TO_RIGHT*/ ) :
    m_spFont(pkFont), 
    m_uiMaxStringLen(uiMaxStringLen),
    m_kColor(kColor), 
    m_ucDirection(ucDirection),
    m_uiStringCount(0),
    m_pfStringWidth(0),
    m_fStringWidth(0.0f),
    m_fStringHeight(0.0f)
{
    m_uiFlags = 0;
    m_pcString = 0;
    m_pkString = 0;
    m_uiStringLen = 0;

    SetText(pcString, uiFlags);
    m_kColor = kColor;
}
//---------------------------------------------------------------------------
NiFontString::NiFontString(NiFont* pkFont, 
    unsigned int uiFlags, unsigned int uiMaxStringLen, 
    const NiWChar* pkString, const NiColorA& kColor,
    unsigned char ucDirection /* = NIFONTSTRING_LEFT_TO_RIGHT*/ ) :
    m_spFont(pkFont), 
    m_uiMaxStringLen(uiMaxStringLen),
    m_kColor(kColor), 
    m_ucDirection(ucDirection),
    m_uiStringCount(0),
    m_pfStringWidth(0),
    m_fStringWidth(0.0f),
    m_fStringHeight(0.0f)
{
    m_uiFlags = 0;
    m_pcString = 0;
    m_pkString = 0;
    m_uiStringLen = 0;

    SetText(pkString, uiFlags);
    m_kColor = kColor;
}
//---------------------------------------------------------------------------
NiFontString::~NiFontString()
{
    NiFree(m_pfStringWidth);
    NiFree(m_pcString);
    NiFree(m_pkString);
}
//---------------------------------------------------------------------------
void NiFontString::SetText(const char* pcText, unsigned int uiFlags)
{
    if (uiFlags != 0xffffffff)
        m_uiFlags = uiFlags;

    unsigned int uiNewLen = m_uiStringLen;
    if (pcText)
        uiNewLen = strlen(pcText) + 1;

    if (m_pcString)
    {
        if (strcmp(m_pcString, pcText))
        {
            //  Check the length
            if (m_uiMaxStringLen > uiNewLen)
            {
                //  Just copy it in...
                NiStrcpy(m_pcString, m_uiMaxStringLen, pcText);

                // Duplicate the string to Unicode
                NiFont::AsciiToUnicode(m_pcString, m_uiMaxStringLen, 
                    m_pkString, m_uiMaxStringLen);

                m_uiStringLen = uiNewLen;
                m_bModified = true;
            }
            else
            {
                //  We need to free it and re-create it...
                ShutdownString();
                m_uiMaxStringLen = uiNewLen + 1;
            }
        }
        else
        {
            //  They're the same! Do nothing...
            return;
        }
    }

    if (m_pcString == 0)
    {
        m_uiStringLen = uiNewLen;

        // Make Sure the maximum length is at least the new string length
        m_uiMaxStringLen = NiMax((int)m_uiMaxStringLen, (int)uiNewLen);

        // Allocate the ASCII and Unicode stirngs
        m_pcString = NiAlloc(char, m_uiMaxStringLen);
        m_pkString = NiAlloc(NiWChar, m_uiMaxStringLen);

        if (pcText)
        {
            NiStrcpy(m_pcString, m_uiMaxStringLen, pcText);

            // Fill in the Unicode strings
            NiFont::AsciiToUnicode(m_pcString, m_uiMaxStringLen,
                m_pkString, m_uiMaxStringLen);
        }
        m_bModified = true;
    }

    SplitString();
}
//---------------------------------------------------------------------------
void NiFontString::SetText(const NiWChar* pkText, unsigned int uiFlags)
{
    if (uiFlags != 0xffffffff)
        m_uiFlags = uiFlags;

    unsigned int uiNewLen = m_uiStringLen;
    if (pkText)
        uiNewLen = wcslen((const wchar_t *)pkText) + 1;

    if (m_pkString)
    {
        if (wcscmp((const wchar_t *)m_pkString, (const wchar_t *)pkText))
        {
            //  Check the length
            if (m_uiMaxStringLen > uiNewLen)
            {
                //  Just copy it in...
                NiWStrcpy(m_pkString, m_uiMaxStringLen, pkText);

                // Null the string because it can't be displayed
                m_pcString[0] = NULL;

                m_uiStringLen = uiNewLen;
                m_bModified = true;
            }
            else
            {
                //  We need to free it and re-create it...
                ShutdownString();
                m_uiMaxStringLen = uiNewLen + 1;
            }
        }
        else
        {
            //  They're the same! Do nothing...
            return;
        }
    }

    if (m_pkString == 0)
    {
        m_uiStringLen = uiNewLen;

        // Make Sure the maximum length is at least the new string length
        m_uiMaxStringLen = NiMax((int)m_uiMaxStringLen, (int)uiNewLen);

        // Allocate the ASCII and Unicode strings
        m_pcString = NiAlloc(char, m_uiMaxStringLen);
        m_pkString = NiAlloc(NiWChar, m_uiMaxStringLen);

        if (pkText)
        {
            NiWStrcpy(m_pkString, m_uiMaxStringLen, pkText);

            // Null the string because it can't be displayed
            m_pcString[0] = NULL;
        }
        m_bModified = true;
    }

    SplitString();
}
//---------------------------------------------------------------------------
void NiFontString::SetColor(const NiColorA& kColor)
{
    if (m_kColor != kColor)
    {
        m_kColor = kColor;
        m_bModified = true;
    }
}
//---------------------------------------------------------------------------
void NiFontString::SplitString()
{
    if (m_pcString == 0)
        return;

    //  Examine the string to see if there are any '\n' instances.
    unsigned int uiIndex = 0;
    unsigned int uiIndexNew = 0;
    unsigned int uiNewLineCount = 1;

    while (m_pkString[uiIndex] != 0)
    {
        if (m_pkString[uiIndex] == '\n')
            uiNewLineCount++;
        uiIndex++;
    }

    // store the width of each line
    if ((uiNewLineCount > m_uiStringCount) || 
        (m_pfStringWidth == NULL))
    {
        m_pfStringWidth = (float*) NiRealloc(m_pfStringWidth,
            sizeof(float)*uiNewLineCount);
    }

    //  Now, determine the max width
    float fTemp = 0.0f;
    m_uiStringCount = uiNewLineCount;
    if (m_uiStringCount == 1)
    {
        m_spFont->GetTextExtent(m_pkString, m_fStringWidth, m_fStringHeight,
            m_ucDirection);

        m_pfStringWidth[0] = m_fStringWidth;
    }
    else
    {
        //  Need to check each 'string'
        unsigned int uiCurrentLine = 0;
        uiIndex = 0;
        uiIndexNew = 0;
        float fNewW = 0.0f;
        m_fStringWidth = 0.0f;
        m_fStringHeight = 0.0f;
        
        while (uiIndex < m_uiStringLen)
        {
            // Convert carriage returns (Ascii code 13) to blanks.
            if (m_pkString[uiIndex] == '\r')
                m_pkString[uiIndex] = ' ';

            if ((m_pkString[uiIndex] == '\n') ||
                (m_pkString[uiIndex] == 0))
            {
                ms_akWorkingString[uiIndexNew] = 0;
                m_spFont->GetTextExtent(ms_akWorkingString, fNewW, fTemp,
                     m_ucDirection);

                m_pfStringWidth[uiCurrentLine] = fNewW;
                uiCurrentLine++;
                if (m_fStringWidth < fNewW)
                    m_fStringWidth = fNewW;
                m_fStringHeight += (float)(m_spFont->GetCharHeight());
                uiIndexNew = 0;
            }
            else
            {
                ms_akWorkingString[uiIndexNew++] = m_pkString[uiIndex];
            }
            uiIndex++;
        }
    }
}
//---------------------------------------------------------------------------
void NiFontString::ShutdownString()
{
    NiFree(m_pcString);
    NiFree(m_pkString);
    m_pcString = 0;
    m_pkString = 0;
    m_uiStringLen = 0;
    NiFree(m_pfStringWidth);
    m_pfStringWidth = 0;
}
//---------------------------------------------------------------------------
