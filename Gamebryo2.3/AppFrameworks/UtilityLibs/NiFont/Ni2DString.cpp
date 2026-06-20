// NUMERICAL DESIGN LIMITED PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Numerical Design Limited and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2007 Numerical Design Limited.
//      All Rights Reserved.
//
// Numerical Design Limited, Chapel Hill, North Carolina 27514
// http://www.ndl.com
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiFontPCH.h"

#include "Ni2DString.h"

#include <NiScreenElements.h>
#include <NiAlphaProperty.h>
#include <NiTexturingProperty.h>
#include <NiVertexColorProperty.h>
#include <NiZBufferProperty.h>
#include <NiPoint3.h>
#include <NiPoint2.h>
#include <NiRenderTargetGroup.h>

//---------------------------------------------------------------------------
Ni2DString::Ni2DString(NiFont* pkFont, unsigned int uiFlags,
    unsigned int uiMaxStringLen, const NiWChar* pkString,
    const NiColorA& kColor, short sX, short sY,
    unsigned char ucDirection,  // = NIFONTSTRING_LEFT_TO_RIGHT
    unsigned short usPointSize, // = DEFAULT_POINT_SIZE
    const NiRenderTargetGroup* pkTarget) : // = NULL
    NiFontString(pkFont, uiFlags, uiMaxStringLen, pkString, kColor,
    ucDirection)
{
    m_usPointSize = usPointSize;
    m_pkTarget = pkTarget;
    SetPosition(sX, sY);
    CreateScreenElements();
}
//---------------------------------------------------------------------------
Ni2DString::Ni2DString(NiFont* pkFont, unsigned int uiFlags,
    unsigned int uiMaxStringLen, const char* pcString, const NiColorA& kColor,
    short sX, short sY,
    unsigned char ucDirection,  // = NIFONTSTRING_LEFT_TO_RIGHT
    unsigned short usPointSize, // = DEFAULT_POINT_SIZE
    const NiRenderTargetGroup* pkTarget) :  // = NULL
    NiFontString(pkFont, uiFlags, uiMaxStringLen, pcString, kColor, 
    ucDirection)
{
    m_usPointSize = usPointSize;
    m_pkTarget = pkTarget;
    SetPosition(sX, sY);
    CreateScreenElements();
}
//---------------------------------------------------------------------------
Ni2DString::~Ni2DString()
{
}
//---------------------------------------------------------------------------
void Ni2DString::sprintf(const char* pcFormatString, ...)
{
    const unsigned int uiDestSize = 2048;
    char acMessage[uiDestSize];

    va_list args;
    va_start(args, pcFormatString);

    NiVsprintf(acMessage, uiDestSize, pcFormatString, args);

    va_end(args);

    // We duplicate the currently set flags
    SetText(acMessage, m_uiFlags);
}
//---------------------------------------------------------------------------
void Ni2DString::sprintf(const NiWChar* pkFormatString, ...)
{
    const unsigned int uiDestSize = 2048;
    NiWChar akDest[uiDestSize];

    va_list kArgs;
    va_start(kArgs, pkFormatString);

#if defined(_PS3)
    vswprintf((wchar_t*)akDest, uiDestSize, (const wchar_t*)pkFormatString,
        kArgs);
#else   // #if defined(_PS3)
#if _MSC_VER >= 1400
    vswprintf_s((wchar_t*)akDest, uiDestSize, (const wchar_t*)pkFormatString, 
        kArgs);
#else   // _MSC_VER >= 1400
    vswprintf((wchar_t*)akDest, uiDestSize, (const wchar_t*)pkFormatString, 
        kArgs);
#endif   // _MSC_VER >= 1400
#endif  // #if defined(_PS3)

    akDest[uiDestSize - 1] = '\0';  // Force last character to be null.

    va_end(kArgs);

    SetText(akDest, m_uiFlags); // Preserve the current flags.
}
//---------------------------------------------------------------------------
void Ni2DString::Draw(NiRenderer* pkRenderer)
{
    if (m_spScreenElements.GetAt(0) == NULL)
        CreateScreenElements();
    else if (m_bModified)
        UpdateScreenElements();

    // Draw the NiScreenElements
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < m_spScreenElements.GetEffectiveSize(); uiLoop++)
        m_spScreenElements.GetAt(uiLoop)->Draw(pkRenderer);
}
//---------------------------------------------------------------------------
void Ni2DString::GetTextExtent(float& fWidth, float& fHeight)
{
    NIASSERT(m_spFont);

    // These values should have been updated in NiFontString::SplitText 
    // function. This is done internally! All of these computations assume
    // the point size found in the font. These values must be scaled to 
    // compensate for the new point size.
    float fScale = 1.0f;

    // Compute a scale to convert from the point height desired from the point
    // height found in the NiFont
    if (m_usPointSize != DEFAULT_POINT_SIZE)
        fScale = (float)m_usPointSize / (float)m_spFont->GetHeight();


    fWidth  = m_fStringWidth * fScale;
    fHeight = m_fStringHeight * fScale;
}
//---------------------------------------------------------------------------
void Ni2DString::CreateScreenElements()
{
    // We will use m_uiMaxStringLen to determine the number of rectangles and
    // vertices we will use. This will allow users to set the max length, and
    // not have to modify the ScreenElements and re-create it everytime the set
    // their text a little longer than it was previously.
    
    if (m_uiStringLen > 0)
    {        

        // Create a new screen elements for each texture
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < m_spFont->GetNumTextures(); uiLoop++)
        {
            // Create the New Screen Elements Data. A grow by size of 20
            // was chosen because the we will be adding string where each 
            // glyph is a polygon.
            NiScreenElementsData* pkData = NiNew NiScreenElementsData(true,
                true, 1, m_uiMaxStringLen, 20, m_uiMaxStringLen * 4, 80,
                m_uiMaxStringLen * 2, 40);

            NIASSERT(pkData);

            // Create the new screen elements
            NiScreenElements* pkNewElements = NiNew NiScreenElements(pkData);
            NIASSERT(pkNewElements);

            m_spScreenElements.Add(pkNewElements);

            // Attach the font's texture
            NiTexturingProperty* pkTexture = NiNew NiTexturingProperty();
            pkTexture->SetBaseTexture(m_spFont->GetTextureAtIndex(uiLoop));
            m_spScreenElements.GetAt(uiLoop)->AttachProperty(pkTexture);
        }

        // Create the default alpha property
        NiAlphaProperty* pkDefaultAlphaProperty = NiNew NiAlphaProperty;
        pkDefaultAlphaProperty->SetAlphaBlending(true);
        pkDefaultAlphaProperty->SetSrcBlendMode(
            NiAlphaProperty::ALPHA_SRCALPHA);
        pkDefaultAlphaProperty->SetDestBlendMode(
            NiAlphaProperty::ALPHA_INVSRCALPHA);

        // Create the default vertex color property
        NiVertexColorProperty* pkDefaultVertexColorsProperty = 
            NiNew NiVertexColorProperty();
        pkDefaultVertexColorsProperty->SetSourceMode(
            NiVertexColorProperty::SOURCE_EMISSIVE);
        pkDefaultVertexColorsProperty->SetLightingMode(
            NiVertexColorProperty::LIGHTING_E_A_D);

        // Create the default Z Buffer property
        NiZBufferProperty* pkDefaultZBufferProperty = NiNew NiZBufferProperty;
        pkDefaultZBufferProperty->SetZBufferTest(false);
        pkDefaultZBufferProperty->SetZBufferWrite(false);
        pkDefaultZBufferProperty->SetTestFunction(
            NiZBufferProperty::TEST_LESSEQUAL);

        // Attach the properties
        for (uiLoop = 0; uiLoop < m_spFont->GetNumTextures(); uiLoop++)
        {
            m_spScreenElements.GetAt(uiLoop)->AttachProperty(
                pkDefaultAlphaProperty);

            m_spScreenElements.GetAt(uiLoop)->AttachProperty(
                pkDefaultVertexColorsProperty);

            m_spScreenElements.GetAt(uiLoop)->AttachProperty(
                pkDefaultZBufferProperty);

            m_spScreenElements.GetAt(uiLoop)->UpdateProperties();
            m_spScreenElements.GetAt(uiLoop)->UpdateEffects();
        }
    }
    else
    {
        unsigned int uiLoop;
        for (uiLoop = 0; uiLoop < m_spFont->GetNumTextures(); uiLoop++)
            m_spScreenElements.SetAt(uiLoop, NULL);
    }
}
//---------------------------------------------------------------------------
void Ni2DString::UpdateScreenElements()
{
    if (!m_bModified)
        return;

    // TURN BACK TO NORMALIZED DISPLAY COORDINATES
    float fScreenWidth;
    float fScreenHeight;

    // Determine the width and height of the expected render target
    if (m_pkTarget)
    {
        fScreenWidth = (float)m_pkTarget->GetWidth(0);
        fScreenHeight = (float)m_pkTarget->GetHeight(0);
    }
    else
    {
        // Use the default render target because one hasn't been defined
        fScreenWidth = (float)NiRenderer::GetRenderer()->
           GetDefaultRenderTargetGroup()->GetWidth(0);

        fScreenHeight = (float)NiRenderer::GetRenderer()->
            GetDefaultRenderTargetGroup()->GetHeight(0);
    }


    NIASSERT(m_spScreenElements.GetAt(0));

    // Clear the 'string'
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < m_spScreenElements.GetEffectiveSize(); uiLoop++)
        m_spScreenElements.GetAt(uiLoop)->RemoveAll();


    // Default to white if a color hasn't been specified
    NiColorA kColor = NiColorA::WHITE;
    if (m_uiFlags & COLORED)
        kColor = m_kColor;

    // Fill in the data
    NiWChar ch;
    unsigned int ui;
    float pfUVSets[4];
    short sW, sH;
    unsigned int uiTotalHeight;
    float fScale = 1.0f;

    // Compute a scale to convert from the point height desired from the point
    // height found in the NiFont
    if (m_usPointSize != DEFAULT_POINT_SIZE)
        fScale = (float)m_usPointSize / (float)m_spFont->GetHeight();

    // Determine the final height of the string in pixels
    uiTotalHeight = (unsigned int)((float)m_spFont->GetCharHeight() * fScale) *
        m_uiStringCount;

    // Setup the starting position of the string
    short sStartX = m_sX;
    short sStartY = m_sY;
    short sCurrX = m_sX;
    short sCurrY = m_sY;

    // Check if it should be centered...
    if (m_uiFlags & CENTERED)
    {
        float fLWidth;
        float fLHeight;
        m_spFont->GetTextLineExtent(m_pkString, fLWidth, fLHeight, 
            m_ucDirection);

        switch (m_ucDirection)
        {
        case NIFONTSTRING_LEFT_TO_RIGHT:
            // Need to center the string about the starting point...
            sStartX = (m_sX  - (short)(fLWidth * fScale) / 2);
            sCurrY  = m_sY - (uiTotalHeight / 2);
            break;
        case NIFONTSTRING_RIGHT_TO_LEFT:
            // Need to center the string about the starting point...
            sStartX = (m_sX  + (short)(fLWidth * fScale) / 2);
            sCurrY  = m_sY - (uiTotalHeight / 2);
            break;
        case NIFONTSTRING_TOP_TO_BOTTOM:
            // Need to center the string about the starting point...
            sStartX = m_sX - (short)
                ((m_uiStringCount * m_spFont->GetMaxPixelWidth()) / 2);
            sCurrY  = (m_sY  - (short)(fLHeight * fScale) / 2);
            break;
        case NIFONTSTRING_BOTTOM_TO_TOP:
            // Need to center the string about the starting point...
            sStartX = m_sX - (short)
                ((m_uiStringCount * m_spFont->GetMaxPixelWidth()) / 2);
            sCurrY  = (m_sY  + (short)(fLHeight * fScale) / 2);
            break;
        }

        sCurrX = sStartX;
    }

    short sCharH = (short)((float)m_spFont->GetCharHeight() * fScale);

    for (ui = 0; ui < m_uiStringLen; ui++)
    {
        ch = m_pkString[ui];

        // Check for end of string.
        if (ch == 0)
            break;

        m_spFont->GetCharUVPair(ch, pfUVSets);

        // Determine the width of this character in pixels
        sW = (short)((pfUVSets[2] - pfUVSets[0]) * 
            m_spFont->GetTextureWidth(ch) * fScale);

        // Determine the width of this character in pixels
        sH = (short)((pfUVSets[3] - pfUVSets[1]) * 
            m_spFont->GetTextureHeight(ch) * fScale);

        switch (m_ucDirection)
        {
        case NIFONTSTRING_LEFT_TO_RIGHT:
            // Check for new-line.
            if (ch == '\n')
            {
                    // Check if it should be centered...
                if (m_uiFlags & CENTERED)
                {
                    float fLWidth;
                    float fLHeight;
                    m_spFont->GetTextLineExtent(&m_pkString[ui + 1], fLWidth,
                        fLHeight, m_ucDirection);

                    // Need to center the string about the starting point...
                    sCurrX = (m_sX  - (short)(fLWidth * fScale) / 2);
                    sCurrY += sCharH;
                    continue;
                }
                else
                {
                    // Just advance to the 'next line'
                    sCurrX = sStartX;
                    sCurrY += sCharH;
                }
                continue;
            }

            CreateScreenPolygon(ch, sCurrX, sCurrY, sW, sH, fScreenWidth, 
                fScreenHeight, kColor, pfUVSets);

            sCurrX += sW;
            break;
        case NIFONTSTRING_RIGHT_TO_LEFT:
            // Check for new-line.
            if (ch == '\n')
            {

                    // Check if it should be centered...
                if (m_uiFlags & CENTERED)
                {
                    float fLWidth;
                    float fLHeight;
                    m_spFont->GetTextLineExtent(&m_pkString[ui + 1], fLWidth,
                        fLHeight, m_ucDirection);

                    // Need to center the string about the starting point...
                    sCurrX = (m_sX  + (short)(fLWidth * fScale) / 2);
                    sCurrY += sCharH;
                    continue;
                }
                else
                {
                    // Just advance to the 'next line'
                    sCurrX = sStartX;
                    sCurrY += sCharH;
                    continue;
                }
            }

            sCurrX -= sW;

            CreateScreenPolygon(ch, sCurrX, sCurrY, sW, sH, fScreenWidth, 
                fScreenHeight, kColor, pfUVSets);

            break;
        case NIFONTSTRING_TOP_TO_BOTTOM:
            // Check for new-line.
            if (ch == '\n')
            {
                    // Check if it should be centered...
                if (m_uiFlags & CENTERED)
                {
                    float fLWidth;
                    float fLHeight;
                    m_spFont->GetTextLineExtent(&m_pkString[ui + 1], fLWidth,
                        fLHeight, m_ucDirection);

                    // Need to center the string about the starting point...
                    sCurrX += sCharH;
                    sCurrY = (m_sY  - (short)(fLHeight * fScale) / 2);
                    continue;
                }
                else
                {
                    // Just advance to the 'next line'
                    sCurrX += sCharH;
                    sCurrY = sStartY;
                    continue;
                }
            }

            CreateScreenPolygon(ch, sCurrX, sCurrY, sW, sH, fScreenWidth, 
                fScreenHeight, kColor, pfUVSets);

            sCurrY += sCharH;
            break;
        case NIFONTSTRING_BOTTOM_TO_TOP:
            // Check for new-line.
            if (ch == '\n')
            {
                    // Check if it should be centered...
                if (m_uiFlags & CENTERED)
                {
                    float fLWidth;
                    float fLHeight;
                    m_spFont->GetTextLineExtent(&m_pkString[ui + 1], fLWidth,
                        fLHeight, m_ucDirection);

                    // Need to center the string about the starting point...
                    sCurrX += sCharH;
                    sCurrY = (m_sY  + (short)(fLHeight * fScale) / 2);
                    continue;
                }
                else
                {
                    // Just advance to the 'next line'
                    sCurrX += sCharH;
                    sCurrY = sStartY;
                    continue;
                }
            }

            sCurrY -= sCharH;

            CreateScreenPolygon(ch, sCurrX, sCurrY, sW, sH, fScreenWidth, 
                fScreenHeight, kColor, pfUVSets);

            break;
        default:
            // You should never get here
            NIASSERT(false);
        }
    }

    for (uiLoop = 0; uiLoop < m_spScreenElements.GetEffectiveSize(); uiLoop++)
        m_spScreenElements.GetAt(uiLoop)->Update(0.0f);

    m_bModified = false;
}
//---------------------------------------------------------------------------
