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

#include "NiOldString2D.h"
#include "NiFont.h"

#include <NiScreenTexture.h>
#include <NiAlphaProperty.h>
#include <NiTexturingProperty.h>
#include <NiVertexColorProperty.h>
#include <NiZBufferProperty.h>
#include <NiPoint3.h>
#include <NiPoint2.h>

//---------------------------------------------------------------------------
NiOldString2D::NiOldString2D(NiFont* pkFont, 
    unsigned int uiFlags, unsigned int uiMaxStringLen, const char* pcString, 
    const NiColorA& kColor, unsigned int uiX, unsigned int uiY) :
    NiFontString(pkFont, uiFlags, uiMaxStringLen, pcString, kColor)
{
    SetPosition(uiX, uiY);
    CreateScreenTexture();
}
//---------------------------------------------------------------------------
NiOldString2D::~NiOldString2D()
{
}
//---------------------------------------------------------------------------
void NiOldString2D::sprintf(const char* pcFormatString, ...)
{
    char acMessage[2048];

    va_list args;
    va_start(args, pcFormatString);
#if defined(_MSC_VER) && _MSC_VER >= 1400
    vsprintf_s(acMessage, 2048, pcFormatString, args);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    vsprintf(acMessage, pcFormatString, args);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    va_end(args);

    // We duplicate the currently set flags
    SetText(acMessage, m_uiFlags);
}
//---------------------------------------------------------------------------
unsigned int NiOldString2D::SetLimitWidth(unsigned int uiWidth)
{
    unsigned int uiOld = m_uiLimitWidth;
    m_uiLimitWidth = uiWidth;

    if (uiOld != uiWidth)
        m_bModified = true;

    return uiOld;
}
//---------------------------------------------------------------------------
void NiOldString2D::Draw(NiRenderer* pkRenderer)
{
    if (m_spScreenTexture == 0)
        CreateScreenTexture();
    else if (m_bModified)
        UpdateScreenTexture();

    pkRenderer->RenderScreenTexture(m_spScreenTexture);
}
//---------------------------------------------------------------------------
void NiOldString2D::GetTextExtent(float& fWidth, float& fHeight)
{
    NIASSERT(m_spFont);

    // These values should have been updated in NiFontString::SplitText 
    // function. This is done internally!
    fWidth  = m_fStringWidth;
    fHeight = m_fStringHeight;
}
//---------------------------------------------------------------------------
void NiOldString2D::CreateScreenTexture()
{
    // We will use m_uiMaxStringLen to determine the number of rectangles and
    // vertices we will use. This will allow users to set the max length, and
    // not have to toss the ScreenTexture and re-create it everytime the set
    // their text a little longer than it was previously.
    // By modifying the rectangle count on the ScreenTexture, we can cheat
    // w.r.t. the number of rectangles drawn.
    // So, the true rectangle count will reflect the max length of the string,
    // and the currently set rectangle count will reflect the actual text
    // length.
    
    if (m_uiStringLen > 0)
    {        
        m_spScreenTexture = NiNew NiScreenTexture(m_spFont->GetTexture());
        if (m_spScreenTexture)
        {
            m_spScreenTexture->SetApplyMode(
                NiTexturingProperty::APPLY_MODULATE);
            UpdateScreenTexture();
        }
    }
    else
    {
        m_spScreenTexture = 0;
    }
}
//---------------------------------------------------------------------------
void NiOldString2D::UpdateScreenTexture()
{
    if (!m_bModified)
        return;

    NIASSERT(m_spScreenTexture);

    // Clear the 'string'
    m_spScreenTexture->RemoveAllScreenRects();

    NiColorA kColor = NiColorA::WHITE;
    if (m_uiFlags & COLORED)
        kColor = m_kColor;

    // Fill in the data
    char ch;
    unsigned int ui;
    float pfUVSets[4];
    short sW, sH;
    unsigned short usTexLeft, usTexTop;

    unsigned int uiTotalHeight = m_spFont->GetMaxPixelHeight() *
        m_uiStringCount;

    // Setup the starting position of the string
    short sCurrY;
    short sCurrX;

    // Check if it should be centered...
    if (m_uiFlags & CENTERED)
    {
        // Need to center the string about the starting point...
        sCurrX = (short)(m_uiX  - (int)m_pfStringWidth[0] / 2);
        sCurrY  = (short)(m_uiY - uiTotalHeight / 2);
    }
    else
    {
        sCurrX = (short)m_uiX;
        sCurrY = (short)m_uiY;
    }

    sH = (short)(m_spFont->GetMaxPixelHeight());
    unsigned int uiCurrentLine = 0;

    for (ui = 0; ui < m_uiStringLen; ui++)
    {
        ch = m_pcString[ui];

        // Check for end of string.
        if (ch == 0)
            break;

        // Check for new-line.
        if (ch == '\n')
        {
            // Just advance to the 'next line'
            uiCurrentLine++;
            if (m_uiFlags & CENTERED)
            {
                sCurrX = 
                    (short)(m_uiX  - (int)m_pfStringWidth[uiCurrentLine] / 2);
            }
            else
                sCurrX = (short)m_uiX;

            sCurrY += sH;
            continue;
        }

        m_spFont->GetCharUVPair(ch, pfUVSets);
        NIASSERT(pfUVSets);

        sW = (short)((pfUVSets[2] - pfUVSets[0]) * 
            m_spFont->GetTextureWidth());

        // Determine the texture position
        usTexLeft = 
            (unsigned short)(pfUVSets[0] * m_spFont->GetTextureWidth());
        usTexTop = 
            (unsigned short)(pfUVSets[1] * m_spFont->GetTextureHeight());

        m_spScreenTexture->AddNewScreenRect(sCurrY, sCurrX, sW, sH, 
            usTexTop, usTexLeft, kColor);

        sCurrX += sW;
    }

    m_spScreenTexture->MarkAsChanged(NiScreenTexture::EVERYTHING_MASK);

    m_bModified = false;
}
//---------------------------------------------------------------------------
