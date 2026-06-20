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

#include "NiFont.h"
#include <NiSourceTexture.h>

//---------------------------------------------------------------------------
NiImplementRootRTTI(NiFont);

unsigned int NiFont::ms_uiCurrentVersion = 3;
NiWChar NiFont::ms_akConversionBuffer[2048];


//---------------------------------------------------------------------------
NiFont::NiFont()
{
    m_uiHeight = 0;
    m_uiCharHeight = 0;
    m_usMaxPixelHeight = 0;
    m_usMaxPixelWidth = 0;
    m_uiFlags = 0;
    m_uiNumTextures = 0;
    m_uiGlyphCount = 0;
    m_pkGlyphMap = NULL;
    m_eLastError = ERR_OK;
    m_acName[0] = NULL;
    m_pucTextureLocation = NULL;
    m_pusTextureCoordinates = NULL;
}
//---------------------------------------------------------------------------
NiFont::NiFont(NiRenderer* pkRenderer) :
    m_spRenderer(pkRenderer)
{
    m_uiHeight = 0;
    m_uiCharHeight = 0;
    m_usMaxPixelHeight = 0;
    m_usMaxPixelWidth = 0;
    m_uiFlags = 0;
    m_uiNumTextures = 0;
    m_uiGlyphCount = 0;
    m_pkGlyphMap = NULL;
    m_acName[0] = NULL;
    m_pucTextureLocation = NULL;
    m_pusTextureCoordinates = NULL;
    m_eLastError = ERR_OK;

    NIASSERT(pkRenderer);
}
NiFont::~NiFont()
{
    NiFree(m_pucTextureLocation);
    NiFree(m_pusTextureCoordinates);
    NiDelete(m_pkGlyphMap);
}
//---------------------------------------------------------------------------
NiFont* NiFont::Create(NiRenderer* pkRenderer, const char* pcFontFile)
{
    NiFont* pkFont = NiNew NiFont(pkRenderer);
    if (pkFont)
    {
        if (!pkFont->Load(pcFontFile))
        {
            NiDelete pkFont;
            pkFont = 0;
        }
    }
    return pkFont;
}
//---------------------------------------------------------------------------
void NiFont::SumGlyphExtent(NiWChar kChar, unsigned short& usRowWidth, 
    unsigned short& usRowHeight, unsigned short& usWidth, 
    unsigned short& usHeight, unsigned char ucDirection) const
{
    // This function will sum the extent of a character based on
    // the direction is will be drawn. ie Left to Right, Top to bottom
    unsigned short usGlyphIndex;

    switch (ucDirection)
    {
        case NIFONT_LEFT_TO_RIGHT:
        case NIFONT_RIGHT_TO_LEFT:
            // Starting a new Row, Move down by the height of a chaacter
            if (kChar == '\n')
            {
                usRowWidth = 0;
                usHeight += m_uiCharHeight;
                return;
            }
            
            // Find the Glyph index
            usGlyphIndex = GetGlyphMapIndex(kChar);

            // Texture coordinates are Left,Top,Right, Bottom and 4 entries per
            // index. Width will be the Right - Left
            usRowWidth += m_pusTextureCoordinates[(usGlyphIndex*4) + 2] - 
                m_pusTextureCoordinates[(usGlyphIndex*4)];

            // Look for the maxim Row
            if (usRowWidth > usWidth)
                usWidth = usRowWidth;
            return;

        case NIFONT_TOP_TO_BOTTOM:
        case NIFONT_BOTTOM_TO_TOP:
            // Starting a new Column, Move over by the width of a chaacter
            if (kChar == '\n')
            {
                usRowHeight = 0;
                usWidth += m_uiCharHeight;
                return;
            }
            
            // Find the Glyph index
            usGlyphIndex = GetGlyphMapIndex(kChar);

            // Use a constant character size to compute top to bottom or
            // bottom to top
            usRowHeight += m_uiCharHeight;

            // Look for the maxim Row
            if (usRowHeight > usHeight)
                usHeight = usRowHeight;
            return;

        default:
            // You should never get here
            NIASSERT(false);
    }

}
//---------------------------------------------------------------------------
void NiFont::GetTextExtent(const char* pcText, float& fWidth, float& fHeight,
    unsigned char ucDirection /* = NIFONT_LEFT_TO_RIGHT */ ) const
{
    if (pcText == NULL)
        return;

    // Compute the width and height of this string in pixels
    unsigned short usRowWidth = 0;
    unsigned short usRowHeight = 0;
    unsigned short usWidth = 0;
    unsigned short usHeight = 0;

    // Set the initial values based on how we will be computing
    switch (ucDirection)
    {
        case NIFONT_LEFT_TO_RIGHT:
        case NIFONT_RIGHT_TO_LEFT:
            usHeight = m_uiCharHeight;
        break;
        case NIFONT_TOP_TO_BOTTOM:
        case NIFONT_BOTTOM_TO_TOP:
            usWidth = m_uiCharHeight;
        break;
        default:
            // You should never get here.
            NIASSERT(false);
    }

    while (*pcText)
    {
        char c = *pcText++;

        SumGlyphExtent(c, usRowWidth, usRowHeight, usWidth, usHeight,
            ucDirection);
    }

    fWidth = (float)usWidth;
    fHeight = (float)usHeight;
}
//---------------------------------------------------------------------------
void NiFont::GetTextExtent(const NiWChar* pkText, float& fWidth,
    float& fHeight, unsigned char ucDirection /* = NIFONT_LEFT_TO_RIGHT */ )
    const
{
    if (pkText == NULL)
        return;

    // Compute the width and height of this string in pixels
    unsigned short usRowWidth = 0;
    unsigned short usRowHeight = 0;
    unsigned short usWidth = 0;
    unsigned short usHeight = 0;

    // Set the initial values based on how we will be computing
    switch (ucDirection)
    {
        case NIFONT_LEFT_TO_RIGHT:
        case NIFONT_RIGHT_TO_LEFT:
            usHeight = m_uiCharHeight;
        break;
        case NIFONT_TOP_TO_BOTTOM:
        case NIFONT_BOTTOM_TO_TOP:
            usWidth = m_uiCharHeight;
        break;
        default:
            // You should never get here.
            NIASSERT(false);
    }

    while (*pkText)
    {
        NiWChar c = *pkText++;

        SumGlyphExtent(c, usRowWidth, usRowHeight, usWidth, usHeight,
            ucDirection);
    }

    fWidth = (float)usWidth;
    fHeight = (float)usHeight;
}

//---------------------------------------------------------------------------
void NiFont::GetTextLineExtent(const char* pcText, float& fWidth,
    float& fHeight, unsigned char ucDirection /* = NIFONT_LEFT_TO_RIGHT */ )
    const
{
    if (pcText == NULL)
        return;

    // Compute the width and height of this string in pixels
    unsigned short usRowWidth = 0;
    unsigned short usRowHeight = 0;
    unsigned short usWidth = 0;
    unsigned short usHeight = 0;

    // Set the initial values based on how we will be computing
    switch (ucDirection)
    {
        case NIFONT_LEFT_TO_RIGHT:
        case NIFONT_RIGHT_TO_LEFT:
            usHeight = m_uiCharHeight;
        break;
        case NIFONT_TOP_TO_BOTTOM:
        case NIFONT_BOTTOM_TO_TOP:
            usWidth = m_uiCharHeight;
        break;
        default:
            // You should never get here.
            NIASSERT(false);
    }

    while (*pcText)
    {
        char c = *pcText++;

        // Quit at the first new line
        if (c == '\n')
            break;

        SumGlyphExtent(c, usRowWidth, usRowHeight, usWidth, usHeight,
            ucDirection);
    }

    fWidth = (float)usWidth;
    fHeight = (float)usHeight;
}
//---------------------------------------------------------------------------
void NiFont::GetTextLineExtent(const NiWChar* pkText, float& fWidth, 
    float& fHeight, unsigned char ucDirection /* = NIFONT_LEFT_TO_RIGHT */ )
    const
{
    if (pkText == NULL)
        return;

    // Compute the width and height of this string in pixels
    unsigned short usRowWidth = 0;
    unsigned short usRowHeight = 0;
    unsigned short usWidth = 0;
    unsigned short usHeight = 0;

    // Set the initial values based on how we will be computing
    switch (ucDirection)
    {
        case NIFONT_LEFT_TO_RIGHT:
        case NIFONT_RIGHT_TO_LEFT:
            usHeight = m_uiCharHeight;
        break;
        case NIFONT_TOP_TO_BOTTOM:
        case NIFONT_BOTTOM_TO_TOP:
            usWidth = m_uiCharHeight;
        break;
        default:
            // You should never get here.
            NIASSERT(false);
    }

    while (*pkText)
    {
        NiWChar c = *pkText++;

        // Quit at the first new line
        if (c == '\n')
            break;

        SumGlyphExtent(c, usRowWidth, usRowHeight, usWidth, usHeight,
            ucDirection);
    }

    fWidth = (float)usWidth;
    fHeight = (float)usHeight;
}
//---------------------------------------------------------------------------
void NiFont::GetTextRangeExtent(const char* pcText, unsigned int uiNumChars,
    float& fWidth, float& fHeight, 
    unsigned char ucDirection /* = NIFONT_LEFT_TO_RIGHT */) const
{
    if (pcText == NULL)
        return;

    // Compute the width and height of this string in pixels
    unsigned short usRowWidth = 0;
    unsigned short usRowHeight = 0;
    unsigned short usWidth = 0;
    unsigned short usHeight = 0;

    // Set the initial values based on how we will be computing
    switch (ucDirection)
    {
        case NIFONT_LEFT_TO_RIGHT:
        case NIFONT_RIGHT_TO_LEFT:
            usHeight = m_uiCharHeight;
        break;
        case NIFONT_TOP_TO_BOTTOM:
        case NIFONT_BOTTOM_TO_TOP:
            usWidth = m_uiCharHeight;
        break;
        default:
            // You should never get here.
            NIASSERT(false);
    }

    while (*pcText && (uiNumChars != 0))
    {
        char c = *pcText++;

        SumGlyphExtent(c, usRowWidth, usRowHeight, usWidth, usHeight,
            ucDirection);

        --uiNumChars;
    }

    fWidth = (float)usWidth;
    fHeight = (float)usHeight;
}
//---------------------------------------------------------------------------
void NiFont::GetTextRangeExtent(const NiWChar* pkText, unsigned int uiNumChars,
    float& fWidth, float& fHeight, 
    unsigned char ucDirection /* = NIFONT_LEFT_TO_RIGHT */) const
{
    if (pkText == NULL)
        return;

    // Compute the width and height of this string in pixels
    unsigned short usRowWidth = 0;
    unsigned short usRowHeight = 0;
    unsigned short usWidth = 0;
    unsigned short usHeight = 0;

    // Set the initial values based on how we will be computing
    switch (ucDirection)
    {
        case NIFONT_LEFT_TO_RIGHT:
        case NIFONT_RIGHT_TO_LEFT:
            usHeight = m_uiCharHeight;
        break;
        case NIFONT_TOP_TO_BOTTOM:
        case NIFONT_BOTTOM_TO_TOP:
            usWidth = m_uiCharHeight;
        break;
        default:
            // You should never get here.
            NIASSERT(false);
    }

    while (*pkText && (uiNumChars != 0))
    {
        NiWChar c = *pkText++;

        SumGlyphExtent(c, usRowWidth, usRowHeight, usWidth, usHeight,
            ucDirection);

        --uiNumChars;
    }

    fWidth = (float)usWidth;
    fHeight = (float)usHeight;
}
//---------------------------------------------------------------------------
void NiFont::GetCharExtent(char cChar, float& fWidth, float& fHeight) const
{
    // Find the Glyph index
    unsigned short usGlyphIndex = GetGlyphMapIndex(cChar);

    // Texture coordinates are Left,Top,Right, Bottom and 4 entries per
    // index. Width will be the Right - Left
    fHeight = (float)(m_pusTextureCoordinates[(usGlyphIndex*4) + 3] - 
        m_pusTextureCoordinates[(usGlyphIndex*4) + 1]);

    fWidth = (float)(m_pusTextureCoordinates[(usGlyphIndex*4) + 2] - 
        m_pusTextureCoordinates[(usGlyphIndex*4)]);
}
//---------------------------------------------------------------------------
void NiFont::GetCharExtent(NiWChar kChar, float& fWidth, float& fHeight) const
{
    // Find the Glyph index
    unsigned short usGlyphIndex = GetGlyphMapIndex(kChar);

    // Texture coordinates are Left,Top,Right, Bottom and 4 entries per
    // index. Width will be the Right - Left
    fHeight = (float)(m_pusTextureCoordinates[(usGlyphIndex*4) + 3] - 
        m_pusTextureCoordinates[(usGlyphIndex*4) + 1]);

    fWidth = (float)(m_pusTextureCoordinates[(usGlyphIndex*4) + 2] - 
        m_pusTextureCoordinates[(usGlyphIndex*4)]);
}
//---------------------------------------------------------------------------
void NiFont::GetCharUVPair(const char cChar, float* pkUVs) const
{
    GetCharUVPair((NiWChar)cChar, pkUVs);
}
//---------------------------------------------------------------------------
void NiFont::GetCharUVPair(const NiWChar kChar, float* pkUVs) const
{
    if (pkUVs == NULL)
        return;

    // Find the Glyph index
    unsigned short usGlyphIndex = GetGlyphMapIndex(kChar);
    unsigned char ucTexture = 0;

    // Use the first texture if there is not texture location array
    if (m_pucTextureLocation)
        ucTexture = m_pucTextureLocation[usGlyphIndex];

    int iWidth = m_spTextures.GetAt(ucTexture)->GetWidth();
    int iHeight = m_spTextures.GetAt(ucTexture)->GetHeight();

    pkUVs[0] = (float)m_pusTextureCoordinates[usGlyphIndex*4] / 
        (float)iWidth;

    pkUVs[1] = (float)m_pusTextureCoordinates[(usGlyphIndex*4) + 1] / 
        (float)iHeight;

    pkUVs[2] = (float)m_pusTextureCoordinates[(usGlyphIndex*4) + 2] / 
        (float)iWidth;

    pkUVs[3] = (float)m_pusTextureCoordinates[(usGlyphIndex*4) + 3] / 
        (float)iHeight;

}
//---------------------------------------------------------------------------
void NiFont::GetCharUVPair(const char cChar, unsigned short* pusPixelLocation)
    const
{
    GetCharUVPair((NiWChar)cChar, pusPixelLocation);
}
//---------------------------------------------------------------------------
void NiFont::GetCharUVPair(const NiWChar kChar, 
    unsigned short* pusPixelLocation) const
{
    if (pusPixelLocation == NULL)
        return;

    // Find the Glyph index
    unsigned short usGlyphIndex = GetGlyphMapIndex(kChar);

    pusPixelLocation[0] = m_pusTextureCoordinates[usGlyphIndex*4];
    pusPixelLocation[1] = m_pusTextureCoordinates[(usGlyphIndex*4) + 1];
    pusPixelLocation[2] = m_pusTextureCoordinates[(usGlyphIndex*4) + 2];
    pusPixelLocation[3] = m_pusTextureCoordinates[(usGlyphIndex*4) + 3];
}
//---------------------------------------------------------------------------
bool NiFont::Load(const char* pacFilename)
{
    NIASSERT(pacFilename);

    char acStandardPath[NI_MAX_PATH];
    NiStrcpy(acStandardPath, NI_MAX_PATH, pacFilename);
    NiPath::Standardize(acStandardPath);

    NiFile kFile(acStandardPath, NiFile::READ_ONLY);

    // Set file to read as little endian
    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    kFile.SetEndianSwap(!bPlatformLittle);

    if (!kFile)
    {
        m_eLastError = ERR_BADFILE;
        return false;
    }

    if (!LoadFromStream(kFile))
        return false;

/*
    // Load The Textures TGA's
    if (m_spTextures.GetAt(0) == NULL)
    {
        char acStandardTargaPath[NI_MAX_PATH];
        NiStrcpy(acStandardTargaPath, NI_MAX_PATH, acStandardPath);

        // Remove the last four character .NFF
        acStandardTargaPath[strlen(acStandardTargaPath) - 4] = '\0';

        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < m_spTextures.GetAllocatedSize(); uiLoop++)
        {

            // Build the file name
            char acPageName[NI_MAX_PATH];
            NiSprintf(acPageName, NI_MAX_PATH, "%s_%d.tga",
                acStandardTargaPath, uiLoop);

            NiSourceTexturePtr spTexture;
            spTexture = NiSourceTexture::Create(acPageName);
            spTexture->LoadPixelDataFromFile();

            m_spTextures.SetAt(uiLoop, spTexture);
        }
    }
*/
    return true;
}
//---------------------------------------------------------------------------
bool NiFont::LoadFromStream(NiBinaryStream& kStream)
{
    unsigned int uiVersion;

    if (!LoadFontHeader(kStream, uiVersion))
        return false;

    // Load pixel data: for new Unicode nff
    if (uiVersion >= 3)
        LoadPixelData(kStream);

    return true;
}
//---------------------------------------------------------------------------
bool NiFont::LoadFontHeader(NiBinaryStream& kStream, unsigned int& uiVersion)
{
    unsigned int uiMagic;
    NiStreamLoadBinary(kStream, uiMagic);
    if (uiMagic != MAGIC_NUMBER)
    {
        m_eLastError = ERR_BADFILE;
        return false;
    }

    NiStreamLoadBinary(kStream, uiVersion);
    if (uiVersion > ms_uiCurrentVersion)
    {
        m_eLastError = ERR_INVALIDVERSION;
        return false;
    }

    // Load the pre-unicode NiFont files
    if (uiVersion <= 2)
    {
        if (!LegacyLoadFontHeader(kStream, uiVersion))
            return false;

        LegacyLoadPixelData(kStream);
        return true;
    }

    unsigned int uiNameLength;
    NiStreamLoadBinary(kStream, uiNameLength);
    NiStreamLoadBinary(kStream, m_acName, uiNameLength + 1);
    NiStreamLoadBinary(kStream, m_uiHeight);
    NiStreamLoadBinary(kStream, m_uiCharHeight);
    NiStreamLoadBinary(kStream, m_usMaxPixelWidth);
    NiStreamLoadBinary(kStream, m_usMaxPixelHeight);
    NiStreamLoadBinary(kStream, m_uiFlags);
    NiStreamLoadBinary(kStream, m_uiGlyphCount);

    unsigned short* pusGlyphMap = NiAlloc(unsigned short, m_uiGlyphCount);
    NIASSERT(pusGlyphMap);
    NiStreamLoadBinary(kStream, pusGlyphMap, m_uiGlyphCount);

    // Allocate a new glyph map
    // Pick a good hash size
    m_pkGlyphMap = NiNew NiTMap<NiWChar, unsigned short>;

    // Insert into the Glyph Map
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < m_uiGlyphCount; uiLoop++)
        m_pkGlyphMap->SetAt(pusGlyphMap[uiLoop], uiLoop);

    // Free the array we used for loading
    NiFree(pusGlyphMap);

    // Load the Texture Location
    m_pucTextureLocation = NiAlloc(unsigned char, m_uiGlyphCount);
    NiStreamLoadBinary(kStream, m_pucTextureLocation, m_uiGlyphCount);

    // Allocate the Texture Coordinates
    m_pusTextureCoordinates = NiAlloc(unsigned short, 4 * m_uiGlyphCount);
    NiStreamLoadBinary(kStream, m_pusTextureCoordinates, 4 * m_uiGlyphCount);

    // Read in the number of textures
    NiStreamLoadBinary(kStream, m_uiNumTextures);

    m_spTextures.SetSize(m_uiNumTextures);

    return true;
}
//---------------------------------------------------------------------------
bool NiFont::LoadPixelData(NiBinaryStream& kStream)
{
    unsigned int uiLoop;
    unsigned int uiWidth;
    unsigned int uiHeight;
    unsigned int uiBytes;
    unsigned char* pucPixels;
    unsigned int uiBitsPerPixel = 32;
    const NiPixelFormat& kDesc = NiPixelFormat::RGBA32;

    for (uiLoop = 0; uiLoop < m_uiNumTextures; ++uiLoop)
    {
        NiStreamLoadBinary(kStream, uiWidth);
        NiStreamLoadBinary(kStream, uiHeight);
        NiStreamLoadBinary(kStream, uiBitsPerPixel);

        // Sanity checks.  Width should be >= 256, except legacy NFFs may not
        // impose that restriction.
        NIASSERT((uiWidth >= 16) && (uiWidth <= 2048));
        NIASSERT((uiHeight >= 16) && (uiHeight <= 2048));

        NiPixelDataPtr spPixelData = NiNew NiPixelData(uiWidth,
            uiHeight, kDesc);

        pucPixels = spPixelData->GetPixels();
        uiBytes = uiWidth * uiHeight * uiBitsPerPixel / 8;
        NiStreamLoadBinary(kStream, pucPixels, uiBytes);

        //CreateTexture(pkPixelData);
        m_spTextures.SetAt(uiLoop,
            NiSourceTexture::Create(spPixelData));
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiFont::LegacyLoadFontHeader(NiBinaryStream& kStream, 
    unsigned int uiVersion)
{

    if (uiVersion < 1)
    {
        enum
        {
            NAME_LEN = 128
        };

        char acName[NAME_LEN];
        NiStreamLoadBinary(kStream, acName, NAME_LEN);
    }

    NiStreamLoadBinary(kStream, m_uiHeight);
    NiStreamLoadBinary(kStream, m_uiCharHeight);
    NiStreamLoadBinary(kStream, m_uiFlags);
    NiStreamLoadBinary(kStream, m_uiGlyphCount);

    char* pcGlyphMap = NiAlloc(char, m_uiGlyphCount);
    NIASSERT(pcGlyphMap);
    NiStreamLoadBinary(kStream, pcGlyphMap, m_uiGlyphCount);

    // Allocate a new glyph map

    // Pick a good hash size
    m_pkGlyphMap = NiNew NiTMap<NiWChar, unsigned short>;

    // Insert into the Glyph Map
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < m_uiGlyphCount; uiLoop++)
        m_pkGlyphMap->SetAt(pcGlyphMap[uiLoop], uiLoop);

    // Free the array we used for loading
    NiFree(pcGlyphMap);


    // Load unused values
    unsigned int uiTextureWidth;
    NiStreamLoadBinary(kStream, uiTextureWidth);

    unsigned int uiTextureHeight;
    NiStreamLoadBinary(kStream, uiTextureHeight);

    float fTextureScale;
    NiStreamLoadBinary(kStream, fTextureScale);

    float* pfTextureCoords = NiAlloc(float, 4 * m_uiGlyphCount);
    NiStreamLoadBinary(kStream, pfTextureCoords, 4 * m_uiGlyphCount);

    // Allocate the Texture Coordinates
    m_pusTextureCoordinates = NiAlloc(unsigned short, 4 * m_uiGlyphCount);

    // convert back to pixel instead of UVs
    for (uiLoop = 0; uiLoop < (m_uiGlyphCount * 4); uiLoop++)
    {
        //Width
        if ((uiLoop % 2) == 0)
            m_pusTextureCoordinates[uiLoop] = (unsigned short)
            (pfTextureCoords[uiLoop] * (float)uiTextureWidth);
        else  // Height
            m_pusTextureCoordinates[uiLoop] = (unsigned short)
            (pfTextureCoords[uiLoop] * (float)uiTextureHeight);
    }

    // Free the array we used for loading
    NiFree(pfTextureCoords);

    if (uiVersion < 2)
    {
        NiColorA kDeprecatedColor;
        // Because we're coming from a binary stream rather than an NiStream
        // we have to load each individually.

        NiStreamLoadBinary(kStream, kDeprecatedColor.r);
        NiStreamLoadBinary(kStream, kDeprecatedColor.g);
        NiStreamLoadBinary(kStream, kDeprecatedColor.b);
        NiStreamLoadBinary(kStream, kDeprecatedColor.a);
    }

    // Assume a standard max width and height
    m_usMaxPixelWidth = (unsigned short)m_uiCharHeight;
    m_usMaxPixelHeight = (unsigned short)m_uiCharHeight;
    return true;
}
//---------------------------------------------------------------------------
void NiFont::LegacyLoadPixelData(NiBinaryStream& kStream)
{
    unsigned int uiWidth;
    unsigned int uiHeight;
    unsigned int uiBitsPerPixel;
    
    NiStreamLoadBinary(kStream, uiWidth);
    NiStreamLoadBinary(kStream, uiHeight);
    NiStreamLoadBinary(kStream, uiBitsPerPixel);

    NIASSERT(uiBitsPerPixel == 32);
    const NiPixelFormat& kDesc = NiPixelFormat::RGBA32;

    NiPixelDataPtr spPixelData = NiNew NiPixelData(uiWidth, uiHeight, kDesc);

    unsigned char* pucPixels = spPixelData->GetPixels();
    unsigned int uiBytes = uiWidth * uiHeight * uiBitsPerPixel / 8;
    NiStreamLoadBinary(kStream, pucPixels, uiBytes);

    CreateTexture(spPixelData);
}
void NiFont::CreateTexture(NiPixelDataPtr spPixelData)
{
    NIASSERT(spPixelData != NULL);

    m_spTextures.Add(NiSourceTexture::Create(spPixelData));
}
//---------------------------------------------------------------------------
NiWChar* NiFont::AsciiToUnicode(const char* pcBuffer,
    unsigned int uiBufferSize)
{
    // Simple conversion from ascii to unicode using a static buffer.
    NIASSERT(strlen(pcBuffer) < 2048);

    NiWChar* pkToBuffer = ms_akConversionBuffer;
    char* pcFromBuffer = (char*)pcBuffer;

    while (*pcFromBuffer != NULL)
    {
        *pkToBuffer++ = (NiWChar)*pcFromBuffer++;
    }

    // Add the NULL Terminator.
    *pkToBuffer++ = NULL;

    return ms_akConversionBuffer;
}
//---------------------------------------------------------------------------
void NiFont::AsciiToUnicode(const char* pcAscii, unsigned int uiBufferSize,
    NiWChar* pkUnicode, unsigned int uiUnicodeSize)
{
    NIASSERT(strlen(pcAscii) < uiUnicodeSize);

    unsigned int uiCount = 0;
    while ((*pcAscii != NULL) && (uiBufferSize-- > 0) && 
        (uiCount++ < uiUnicodeSize))
    {
        *pkUnicode++ = (NiWChar)*pcAscii++;
    }

    // Add the NULL Terminator
    *pkUnicode++ = NULL;
}
//---------------------------------------------------------------------------
void NiFont::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    pStrings->Add(NiGetViewerString(NiFont::ms_RTTI.GetName()));
    pStrings->Add(NiGetViewerString("m_acName", m_acName));
    pStrings->Add(NiGetViewerString("m_uiHeight", m_uiHeight));
    pStrings->Add(NiGetViewerString("m_uiCharHeight", m_uiCharHeight));
    pStrings->Add(NiGetViewerString("m_uiFlags", m_uiFlags));
    pStrings->Add(NiGetViewerString("m_uiGlyphCount", m_uiGlyphCount));
    pStrings->Add(NiGetViewerString("Num Textures",
        m_spTextures.GetEffectiveSize()));
}
//---------------------------------------------------------------------------
