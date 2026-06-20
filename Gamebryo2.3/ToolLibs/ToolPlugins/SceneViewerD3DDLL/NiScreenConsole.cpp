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

#include "NiScreenConsole.h"
#include <NiSourceTexture.h>

const unsigned int NiScreenConsole::ms_uiASCIIMin = 33;
const unsigned int NiScreenConsole::ms_uiASCIIMax = 122;
const unsigned int NiScreenConsole::ms_uiASCIICols = 12;
const char* NiScreenConsole::ms_pcDefaultFontPath = "";

//---------------------------------------------------------------------------
NiScreenConsole::NiScreenConsole() : m_bIsOn(false), 
    m_iTextBoxDimensionsX(100), m_iTextBoxDimensionsY(100), 
    m_iTextOriginX(0), m_iTextOriginY(0), m_spScreenTexture(NULL), 
    m_spTextTexture(NULL), m_spActiveScreenTexture(NULL)
{
    m_apcText.SetSize(50);
    m_abTextIsReserved.SetSize(50);

    m_pkCamera = NULL;
    for (unsigned int ui = 0; ui < 50; ui++)
    {
        m_apcText.SetAt(ui, NULL);
        m_abTextIsReserved.SetAt(ui, false);
    }
}
//---------------------------------------------------------------------------
NiScreenConsole::~NiScreenConsole()
{
    NiDelete m_pkFont;
    for (unsigned int ui = 0; ui < m_apcText.GetSize(); ui++)
    {
        NiFree(m_apcText.GetAt(ui));
    }
    if (m_pkCamera)
    {
        m_spActiveScreenTexture = NULL;
    }
    m_spScreenTexture = NULL;
    m_spTextTexture = NULL;

}
//---------------------------------------------------------------------------
void NiScreenConsole::Enable(bool bOn)
{
    m_bIsOn = bOn;
    if (IsEnabled())
        SetCamera(m_pkCamera);
    else if (m_pkCamera)
        m_spActiveScreenTexture = NULL;
}
//---------------------------------------------------------------------------
bool NiScreenConsole::IsEnabled() const
{
    return m_bIsOn;
}
//---------------------------------------------------------------------------

// Internal font representation information
// We load fonts from a texture file and cannot actually
// scale them, so you need a different texture image file
// for each scale of font you wish to use

// Creates standard fonts for use

//---------------------------------------------------------------------------
NiScreenConsole::NiConsoleFont* NiScreenConsole::CreateLargeFont()
{ 
    NiScreenConsole::NiConsoleFont* pkFont = NiNew 
        NiScreenConsole::NiConsoleFont;

    pkFont->m_pcTextImage = "asciilarge.tga";
    const unsigned int uiLength = strlen(pkFont->m_pcTextImage) + 
        strlen(ms_pcDefaultFontPath) + 1;
    char* pcFullFontPath = NiAlloc(char, uiLength);
    NiSprintf(pcFullFontPath, uiLength, "%s%s", ms_pcDefaultFontPath,
        pkFont->m_pcTextImage);
    pkFont->m_pcTextImage = pcFullFontPath;
    pkFont->m_uiCharWidth = 20;
    pkFont->m_uiCharHeight = 30;
    pkFont->m_uiCharSpacingX = 21;
    pkFont->m_uiCharSpacingY = 30;
    pkFont->m_uiCharBaseU = 2;
    pkFont->m_uiCharBaseV = 7;

    return pkFont;
}
//---------------------------------------------------------------------------
NiScreenConsole::NiConsoleFont* NiScreenConsole::CreateTinyFont()
{
    NiScreenConsole::NiConsoleFont* pkFont = NiNew 
        NiScreenConsole::NiConsoleFont;

    pkFont->m_pcTextImage = "asciitiny.tga";
    const unsigned int uiLength = strlen(pkFont->m_pcTextImage) + 
        strlen(ms_pcDefaultFontPath) + 1;
    char* pcFullFontPath = NiAlloc(char, uiLength);
    NiSprintf(pcFullFontPath, uiLength, "%s%s", ms_pcDefaultFontPath,
        pkFont->m_pcTextImage);
    pkFont->m_pcTextImage = pcFullFontPath;
    pkFont->m_uiCharWidth = 5;
    pkFont->m_uiCharHeight = 8;
    pkFont->m_uiCharSpacingX = 6;
    pkFont->m_uiCharSpacingY = 9;
    pkFont->m_uiCharBaseU = 0;
    pkFont->m_uiCharBaseV = 0;

    return pkFont;
}
//---------------------------------------------------------------------------
NiScreenConsole::NiConsoleFont* NiScreenConsole::CreateConsoleFont()
{
    NiScreenConsole::NiConsoleFont* pkFont = NiNew 
        NiScreenConsole::NiConsoleFont;

    pkFont->m_pcTextImage = "ascii_con.tga";
    const unsigned int uiLength = strlen(pkFont->m_pcTextImage) + 
        strlen(ms_pcDefaultFontPath) + 1;
    char* pcFullFontPath = NiAlloc(char, uiLength);
    NiSprintf(pcFullFontPath, uiLength, "%s%s", ms_pcDefaultFontPath,
        pkFont->m_pcTextImage);
    pkFont->m_pcTextImage = pcFullFontPath;
    pkFont->m_uiCharWidth = 8;
    pkFont->m_uiCharHeight = 12;
    pkFont->m_uiCharSpacingX = 9;
    pkFont->m_uiCharSpacingY = 13;
    pkFont->m_uiCharBaseU = 0;
    pkFont->m_uiCharBaseV = 0;

    return pkFont;
}
//---------------------------------------------------------------------------
// Manipulators for the current font
void NiScreenConsole::SetFont(NiScreenConsole::NiConsoleFont* pkFont)
{ 
    if (pkFont != NULL)
    {
        m_pkFont = pkFont;
        if (m_pkCamera)
            m_spActiveScreenTexture = NULL;

        m_spTextTexture = NiSourceTexture::Create(pkFont->m_pcTextImage);

        m_spScreenTexture = NiNew NiScreenTexture(m_spTextTexture);
        m_spScreenTexture->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
        if (m_pkCamera)
            m_spActiveScreenTexture = m_spScreenTexture;
    }
}
//---------------------------------------------------------------------------
NiScreenConsole::NiConsoleFont* NiScreenConsole::GetFont() const
{
    return m_pkFont;
}
//---------------------------------------------------------------------------

// Given the current dimensions and the current text size, this
// is the total number of lines that can be drawn to the screen
//---------------------------------------------------------------------------
unsigned int NiScreenConsole::GetMaxLineCount() const
{ 
    if (m_pkFont)
        return m_iTextBoxDimensionsY / m_pkFont->m_uiCharHeight;
    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiScreenConsole::GetMaxLineLength() const
{
    if (m_pkFont)
        return m_iTextBoxDimensionsX / m_pkFont->m_uiCharWidth;
    return 0;
}
//---------------------------------------------------------------------------

// Works in screen polygon coords:  <0-1, 0-1> to set the position of 
// the lower-left edge of the text.
//---------------------------------------------------------------------------
NiPoint2 NiScreenConsole::GetOrigin() const
{ 
    return NiPoint2((float)m_iTextOriginX, (float)m_iTextOriginY);
}
//---------------------------------------------------------------------------
void NiScreenConsole::SetOrigin(NiPoint2 kOrigin)
{ 
    m_iTextOriginX = (int) kOrigin.x;
    m_iTextOriginY = (int) kOrigin.y;
}
//---------------------------------------------------------------------------
void NiScreenConsole::SetCamera(NiCamera* pkCamera)
{
    if (pkCamera != NULL)
    {
        if (m_spScreenTexture != NULL && m_pkCamera)
            m_spActiveScreenTexture = NULL;
        m_pkCamera = pkCamera;
        m_spActiveScreenTexture = m_spScreenTexture;
    }
    else
        m_pkCamera = NULL;
}    
//---------------------------------------------------------------------------

// Defines the clipping region of the text in pixels. If a line will 
// not fit within the dimensions at the current text size, it will 
// not be rendered.
//---------------------------------------------------------------------------
NiPoint2 NiScreenConsole::GetDimensions() const
{ 
    return NiPoint2((float)m_iTextBoxDimensionsX, 
        (float)m_iTextBoxDimensionsY);
}
//---------------------------------------------------------------------------
void NiScreenConsole::SetDimensions(NiPoint2 kDimensions)
{ 
    m_iTextBoxDimensionsX = (int) kDimensions.x;
    m_iTextBoxDimensionsY = (int) kDimensions.y;
}
//---------------------------------------------------------------------------

// Some applications may wish to reserve a free line number.
// This is not enforced in the SetLine method, but if handled 
// responsibly, this will act as a valid mechanism for holding line  
// numbers for specific information. If a valid index cannot be  
// generated, the function returns -1, an invalid index.
//---------------------------------------------------------------------------
int NiScreenConsole::GrabFreeLineNumber()
{ 
    int iReturn = -1;
    for (int i = 0; i < (int) m_abTextIsReserved.GetSize() && iReturn == -1; 
        i++)
    {
        if (m_abTextIsReserved.GetAt(i) == false)
            iReturn = i;
    }

    if (iReturn != -1)
        m_abTextIsReserved.SetAt(iReturn, true);

    return iReturn;
}
//---------------------------------------------------------------------------
void NiScreenConsole::ReleaseLineNumber(unsigned int uiLineNumber)
{
    m_abTextIsReserved.SetAt(uiLineNumber, false);
    NiFree(m_apcText.GetAt(uiLineNumber));
    m_apcText.SetAt(uiLineNumber, NULL);
}
//---------------------------------------------------------------------------
unsigned int NiScreenConsole::GetCurrentReservedLineCount() const
{ 
    unsigned int uiReturn = 0;
    for (int i = 0; i < (int) m_abTextIsReserved.GetSize(); i++)
    {
        if (m_abTextIsReserved.GetAt(i) == true)
            uiReturn++;
    }

    return uiReturn;
}
//---------------------------------------------------------------------------

// Manipulates the given line number. Strings will be truncated
// to fit within the dimensions of the screen.
//---------------------------------------------------------------------------
void NiScreenConsole::SetLine(const char* pcLine, unsigned int uiWhichLine)
{ 
    NiFree(m_apcText.GetAt(uiWhichLine));
    m_apcText.SetAt(uiWhichLine, TruncateString(pcLine));
}
//---------------------------------------------------------------------------
const char* NiScreenConsole::GetLine(unsigned int uiWhichLine) const
{ 
    return (const char*)(m_apcText.GetAt(uiWhichLine));
}
//---------------------------------------------------------------------------
char* NiScreenConsole::TruncateString(const char* pcString)
{ 
    unsigned int uiLength = GetMaxLineLength();
    char* pcReturnString = NiAlloc(char, uiLength + 1);
    
    NiStrncpy(pcReturnString, uiLength + 1, pcString, uiLength);
    return pcReturnString;
}
//---------------------------------------------------------------------------
void NiScreenConsole::RecreateText()
{
    m_spScreenTexture->RemoveAllScreenRects();

    m_uiNumCurrentRows = 0;
    m_uiCurrentColumn = 0;
    unsigned int uiMaxLines = GetMaxLineCount();
    int iMaxLineLength = (int) GetMaxLineLength();

    for (unsigned int uiLine = 0; uiLine < m_apcText.GetSize() && 
        uiLine < uiMaxLines; uiLine++)
    {
        char* pcString = m_apcText.GetAt(uiLine);
        if (pcString == NULL)
            continue;   
        
        unsigned int uiStrLength = strlen(pcString);    
        uiStrLength = NiMin((int) uiStrLength, iMaxLineLength);

        for (unsigned int i = 0; i < uiStrLength; i++)
        {
            char cChar = pcString[i];
       
            // if the extra char is a '\n', skip it
            if (cChar == '\n')
                continue;

            unsigned int uiChar = (unsigned int)cChar;

            // skip whitespace or unprintable character
            if ((uiChar >= ms_uiASCIIMin) && (uiChar <= ms_uiASCIIMax))
            {
                uiChar -= ms_uiASCIIMin;
    
                unsigned short usPixTop = m_iTextOriginY;
                usPixTop += (m_uiNumCurrentRows + 1) * 
                    m_pkFont->m_uiCharSpacingY;
                
                unsigned short usPixLeft = m_iTextOriginX + 
                    m_uiCurrentColumn * m_pkFont->m_uiCharSpacingX;

                unsigned short usTexTop = m_pkFont->m_uiCharBaseV + 
                    (uiChar / ms_uiASCIICols) * m_pkFont->m_uiCharSpacingY;
                unsigned short usTexLeft = m_pkFont->m_uiCharBaseU + 
                    (uiChar % ms_uiASCIICols) * m_pkFont->m_uiCharSpacingX;

                m_spScreenTexture->AddNewScreenRect(usPixTop, usPixLeft, 
                    m_pkFont->m_uiCharWidth, m_pkFont->m_uiCharHeight, 
                    usTexTop, usTexLeft);
            }

            m_uiCurrentColumn++;
        }

        m_uiNumCurrentRows++;
        m_uiCurrentColumn = 0;

    }
    m_spScreenTexture->MarkAsChanged(NiScreenTexture::EVERYTHING_MASK);
}
//---------------------------------------------------------------------------
void NiScreenConsole::SetDefaultFontPath(const char* pcPath)
{
    if (pcPath != NULL)
        ms_pcDefaultFontPath = pcPath;
}
//---------------------------------------------------------------------------
const char* NiScreenConsole::GetDefaultFontPath()
{
    return ms_pcDefaultFontPath;
}
//---------------------------------------------------------------------------
NiScreenTexture* NiScreenConsole::GetActiveScreenTexture()
{
    return m_spActiveScreenTexture;
}
//---------------------------------------------------------------------------

