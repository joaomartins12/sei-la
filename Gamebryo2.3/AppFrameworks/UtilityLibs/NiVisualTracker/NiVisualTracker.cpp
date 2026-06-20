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

// Precmpiled Header
#include "NiVisualTrackerPCH.h"

#include "NiVisualTracker.h"
#include <NiMaterialProperty.h>
#include <NiZBufferProperty.h>
#include <NiWireframeProperty.h>
#include <NiVertexColorProperty.h>
#include <NiCamera.h>
#include <NiSystem.h>
#include <NiRect.h>
#include <NiRenderer.h>
#include <NiScreenTexture.h>
#include <NiTexturingProperty.h>
#include <NiSourceTexture.h>
#include "Courier16.h"

//---------------------------------------------------------------------------
NiVisualTracker::NiVisualTracker(float fMaxValue, 
    unsigned int uiNumDecimalPlaces, NiRect<float> kWindowRect,
    const char* pcName, bool bShow, unsigned int uiNumTrackers) 
    : m_kCallbackData(uiNumTrackers, 1), 
    m_kDimensions(kWindowRect), 
    m_fMaxValue(fMaxValue), 
    m_bShow(bShow), 
    m_kScreenTextures(uiNumTrackers + 1, 1)
{
    NiSprintf(m_acName, 255, "%s", pcName);
    m_acName[255] = '\0';

    Ni2DBuffer* pkBuffer = NiRenderer::GetRenderer()->GetDefaultBackBuffer();
    NIASSERT(pkBuffer);

    unsigned int uiBBWidth = pkBuffer->GetWidth();
    unsigned int uiBBHeight = pkBuffer->GetHeight();
    
    m_pkTextTitle = NiNew ScreenText(32, &m_kScreenTextures, NiColorA::WHITE);

    m_spWindowRoot = NiNew NiNode(uiNumTrackers);
    m_spWindowRoot->SetTranslate(NiPoint3(m_kDimensions.m_left,
        m_kDimensions.m_top, 0.1f));

    // Graph visualization dimensions
    float fOuterWidth = NiAbs(m_kDimensions.m_left - m_kDimensions.m_right);
    float fOuterHeight = NiAbs(m_kDimensions.m_top - m_kDimensions.m_bottom);
   
    unsigned int uiSpacing = m_pkTextTitle->GetCharacterSpacing();
    float fNormTextWidth = ((float)uiSpacing)/((float)uiBBWidth);
    float fNormTextHeight = 
        ((float)m_pkTextTitle->GetHeight())/((float)uiBBHeight);

    m_kGraphLineDimensions.m_left = 0.0f;
    m_kGraphLineDimensions.m_right = fOuterWidth - 0.10f*fOuterWidth;
    m_kGraphLineDimensions.m_top = fNormTextHeight;
    m_kGraphLineDimensions.m_bottom = fOuterHeight - fNormTextHeight;

    float fWidth = NiAbs(m_kGraphLineDimensions.m_left - 
        m_kGraphLineDimensions.m_right);
    float fHeight = NiAbs(m_kGraphLineDimensions.m_top - 
        m_kGraphLineDimensions.m_bottom);
    
    // Create the border line segments
    unsigned int uiNumVerts = 11;
    m_spBorders = GraphCallbackObjectData::CreateLines(
        uiNumVerts, NiColor::WHITE, m_kGraphLineDimensions);

    NiLinesData* pkModelData = (NiLinesData*) m_spBorders->GetModelData();

    // Remember y runs from top (0.0) to bottom (1.0)
    // in Gamebryo screen space.

    if (pkModelData)
    {
        NiPoint3* pkVerts = pkModelData->GetVertices();
        NiBool* pkConnects = pkModelData->GetFlags();
        if (pkVerts)
        {
            unsigned int ui = 0;
            // The frame vertices
            pkVerts[ui] = NiPoint3(0.0f,   0.0f,    0.0f);
            pkConnects[ui++] = true;
            pkVerts[ui] = NiPoint3(0.0f,   fHeight, 0.0f);
            pkConnects[ui++] = true;
            pkVerts[ui] = NiPoint3(fWidth, fHeight, 0.0f);
            pkConnects[ui++] = true;
            pkVerts[ui] = NiPoint3(fWidth, 0.0f, 0.0f);
            pkConnects[ui++] = true;
            pkVerts[ui] = NiPoint3(0.0f, 0.0f, 0.0f);
            pkConnects[ui++] = false;

            // The 0.25% line
            pkVerts[ui] = NiPoint3(0.0f, 0.75f*fHeight, 0.0f);
            pkConnects[ui++] = true;
            pkVerts[ui] = NiPoint3(fWidth, 0.75f*fHeight, 0.0f);
            pkConnects[ui++] = false;

            // The 0.50% line
            pkVerts[ui] = NiPoint3(0.0f, 0.5f*fHeight, 0.0f);
            pkConnects[ui++] = true;
            pkVerts[ui] = NiPoint3(fWidth, 0.5f*fHeight, 0.0f);
            pkConnects[ui++] = false;

            // The 0.75% line
            pkVerts[ui] = NiPoint3(0.0f, 0.25f*fHeight, 0.0f);
            pkConnects[ui++] = true;
            pkVerts[ui] = NiPoint3(fWidth, 0.25f*fHeight, 0.0f);
            pkConnects[ui++] = false;

        }
        pkModelData->MarkAsChanged(NiGeometryData::VERTEX_MASK);
        pkModelData->GetBound().ComputeFromData(uiNumVerts, pkVerts);     
    }

    m_spWindowRoot->AttachChild(m_spBorders);

    unsigned int uiLen = strlen(m_acName);
    

    float fOffset = fWidth - fNormTextWidth*(float)uiLen;
    float fNormX;
    
    if (fOffset > 1.0f)
        fOffset = 0.0f;

    fNormX = fOffset / 2.0f;

    unsigned int uiXPos, uiYPos;
    uiXPos = (unsigned int)((fNormX + m_kDimensions.m_left) *
        (float)uiBBWidth);
    uiYPos = (unsigned int)((m_kDimensions.m_top) * (float)uiBBHeight);
    m_pkTextTitle->SetTextOrigin(uiXPos, uiYPos);
    m_pkTextTitle->SetVisible(true);
    m_pkTextTitle->SetString(m_acName);

    char acNumberString[16];
    uiXPos =  (unsigned int)((m_kDimensions.m_left + 
        m_kGraphLineDimensions.m_right + 0.01f) *
        (float)uiBBWidth);
    uiYPos = (unsigned int)(
        (m_kDimensions.m_top + m_kGraphLineDimensions.m_top - 
        0.5f * fNormTextHeight) *
        (float)uiBBHeight);

    unsigned int uiHeightIncrement = 
        (unsigned int)((0.25f*fHeight)*(float)uiBBHeight);

    char acFormatString[128];
    NiSprintf(acFormatString, 127, "%%.%df", uiNumDecimalPlaces);

    // 100% text
    NiSprintf(acNumberString, 14, acFormatString, m_fMaxValue);
    acNumberString[15] = '\0';
    m_pkTextOneHundred = NiNew ScreenText(15, &m_kScreenTextures,
        NiColorA::WHITE);
    m_pkTextOneHundred->SetTextOrigin(uiXPos, uiYPos);
    m_pkTextOneHundred->SetVisible(true);
    m_pkTextOneHundred->SetString(acNumberString);

    // 75% text
    uiYPos += uiHeightIncrement;
    NiSprintf(acNumberString, 14, acFormatString, 0.75f*m_fMaxValue);
    acNumberString[15] = '\0';
    m_pkTextSeventyFive = NiNew ScreenText(15, &m_kScreenTextures,
        NiColorA::WHITE);
    m_pkTextSeventyFive->SetTextOrigin(uiXPos, uiYPos);
    m_pkTextSeventyFive->SetVisible(true);
    m_pkTextSeventyFive->SetString(acNumberString);

    // 50% text
    uiYPos += uiHeightIncrement;
    NiSprintf(acNumberString, 14, acFormatString, 0.5f*m_fMaxValue);
    acNumberString[15] = '\0';
    m_pkTextFifty = NiNew ScreenText(15, &m_kScreenTextures, 
        NiColorA::WHITE);
    m_pkTextFifty->SetTextOrigin(uiXPos, uiYPos);
    m_pkTextFifty->SetVisible(true);
    m_pkTextFifty->SetString(acNumberString);

    // 25% text
    uiYPos += uiHeightIncrement;
    NiSprintf(acNumberString, 14, acFormatString, 0.25f*m_fMaxValue);
    acNumberString[15] = '\0';
    m_pkTextTwentyFive = NiNew ScreenText(15, &m_kScreenTextures, 
        NiColorA::WHITE);
    m_pkTextTwentyFive->SetTextOrigin(uiXPos, uiYPos);
    m_pkTextTwentyFive->SetVisible(true);
    m_pkTextTwentyFive->SetString(acNumberString);

    // 0.0% text
    uiYPos += uiHeightIncrement;
    NiSprintf(acNumberString, 14, acFormatString, 0.0f);
    acNumberString[15] = '\0';
    m_pkTextZero = NiNew ScreenText(32, &m_kScreenTextures, 
        NiColorA::WHITE);
    m_pkTextZero->SetTextOrigin(uiXPos, uiYPos);
    m_pkTextZero->SetVisible(true);
    m_pkTextZero->SetString(acNumberString);

    // Define legend location
    m_uiLegendX = (unsigned int)((m_kDimensions.m_left) * (float)uiBBWidth);
    m_uiLegendY = (unsigned int)((m_kDimensions.m_bottom - 
        fNormTextHeight) * (float)uiBBHeight);
 }
//---------------------------------------------------------------------------
NiVisualTracker::~NiVisualTracker()
{
    m_spBorders = 0;

    for (unsigned int ui = 0; ui < m_kCallbackData.GetSize(); ui++)
    {
        NiDelete m_kCallbackData.GetAt(ui);
    }

    NiDelete m_pkTextTitle;
    NiDelete m_pkTextZero;
    NiDelete m_pkTextTwentyFive;
    NiDelete m_pkTextFifty;
    NiDelete m_pkTextSeventyFive;
    NiDelete m_pkTextOneHundred;

}
//---------------------------------------------------------------------------
unsigned int NiVisualTracker::AddGraph(GraphCallbackObject* pkObject, 
    const char* pcName, const NiColor& kColor, 
    unsigned int uiNumSamplesToKeep, float fSamplingTime, bool bShow)
{
    
    NiVisualTracker::GraphCallbackObjectData* pkCBData = NiNew
        NiVisualTracker::GraphCallbackObjectData(pkObject, 
        pcName, kColor, uiNumSamplesToKeep, m_fMaxValue, 
        fSamplingTime, bShow, m_kGraphLineDimensions, m_spWindowRoot,
        m_uiLegendX, m_uiLegendY, m_kScreenTextures);

    return m_kCallbackData.AddFirstEmpty(pkCBData);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void NiVisualTracker::RemoveGraph(const char* pcName)
{
    for (unsigned int ui = 0; ui < m_kCallbackData.GetSize(); ui++)
    {
        NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(ui);
        NIASSERT(pkData);
        if (strcmp(pkData->GetName(), pcName) == 0)
        {
            int iXAdjust = -(int)pkData->GetLegendWidth();

            for (unsigned int uj = ui+1; uj < m_kCallbackData.GetSize(); uj++)
            {
                NiVisualTracker::GraphCallbackObjectData* pkData = 
                    m_kCallbackData.GetAt(uj);
                NIASSERT(pkData);
                pkData->AdjustLegend(iXAdjust, 0);
            }

            m_kCallbackData.RemoveAt(ui);
            NiDelete pkData;

            m_kCallbackData.Compact();

            m_uiLegendX += iXAdjust;
            break;
        }
    }   
}
//---------------------------------------------------------------------------
void NiVisualTracker::RemoveAll()
{
    for (unsigned int ui = 0; ui < m_kCallbackData.GetSize(); ui++)
    {
        NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(ui);

        NiDelete pkData;
    }

    m_kCallbackData.RemoveAll();

    // reset legend
    Ni2DBuffer* pkBuffer = NiRenderer::GetRenderer()->GetDefaultBackBuffer();
    NIASSERT(pkBuffer);
    unsigned int uiBBWidth = pkBuffer->GetWidth();

    m_uiLegendX = (unsigned int)((m_kDimensions.m_left) * (float)uiBBWidth);
}
//---------------------------------------------------------------------------
const char* NiVisualTracker::GetName()
{
    return m_acName;
}
//---------------------------------------------------------------------------
void NiVisualTracker::Update()
{
    float fTime = NiGetCurrentTimeInSec();
    for (unsigned int ui = 0; ui < m_kCallbackData.GetSize(); ui++)
    {
        NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(ui);
        NIASSERT(pkData);
        pkData->Update(fTime);
    }   
    m_spWindowRoot->Update(fTime);
}
//---------------------------------------------------------------------------
void NiVisualTracker::Draw()
{
    if (!m_bShow)
        return;

    NiRenderer::GetRenderer()->SetScreenSpaceCameraData();

    m_spBorders->RenderImmediate(NiRenderer::GetRenderer());

    for (unsigned int ui = 0; ui < m_kCallbackData.GetSize(); ui++)
    {
        NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(ui);
        NIASSERT(pkData);
        pkData->Draw();
    }

    for (unsigned int uj = 0; uj < m_kScreenTextures.GetSize(); uj++)
    {
        NiScreenTexture* pkTexture = m_kScreenTextures.GetAt(uj);
        NIASSERT(pkTexture);
        pkTexture->Draw(NiRenderer::GetRenderer());
    }
}
//---------------------------------------------------------------------------
void NiVisualTracker::SetShow(bool bShow)
{
    m_bShow = bShow;
}
//---------------------------------------------------------------------------
bool NiVisualTracker::GetShow()
{
    return m_bShow;
}
//---------------------------------------------------------------------------
unsigned int NiVisualTracker::GetGraphCount()
{
    return m_kCallbackData.GetSize();
}
//---------------------------------------------------------------------------
void NiVisualTracker::SetName(const char* pcName, unsigned int uiWhichGraph)
{
    NIASSERT(uiWhichGraph < m_kCallbackData.GetSize());

    NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(uiWhichGraph);
    NIASSERT(pkData);
    int iXAdjust = -(int)pkData->GetLegendWidth();

    pkData->SetName(pcName);
    iXAdjust += pkData->GetLegendWidth();

    for (unsigned int uj = uiWhichGraph + 1; uj < m_kCallbackData.GetSize();
        uj++)
    {
        NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(uj);
        NIASSERT(pkData);
        pkData->AdjustLegend(iXAdjust, 0);
    }

}
//---------------------------------------------------------------------------
const char* NiVisualTracker::GetName(unsigned int uiWhichGraph)
{
    NIASSERT(uiWhichGraph < m_kCallbackData.GetSize());

    NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(uiWhichGraph);
    NIASSERT(pkData);
    return pkData->GetName();
}
//---------------------------------------------------------------------------
unsigned int NiVisualTracker::GetGraphIndexByName(const char* pcName)
{
    for (unsigned int ui = 0; ui < m_kCallbackData.GetSize(); ui++)
    {
        NiVisualTracker::GraphCallbackObjectData* pkData = 
            m_kCallbackData.GetAt(ui);
        NIASSERT(pkData);

        if (0 == strcmp(pcName, pkData->GetName()))
            return ui;
    }  

    return (unsigned int) -1;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  NiVisualTracker::GraphCallbackObjectData implementation
//---------------------------------------------------------------------------
void NiVisualTracker::GraphCallbackObjectData::SetShow(bool bShow)
{
    m_bShow = bShow;
}
//---------------------------------------------------------------------------
bool NiVisualTracker::GraphCallbackObjectData::GetShow()
{
    return m_bShow;
}
//---------------------------------------------------------------------------
void NiVisualTracker::GraphCallbackObjectData::SetName(const char* pcName)
{
    NiSprintf(m_acName, 255, "%s", pcName);
    m_acName[255] = '\0';
    m_pkText->SetString(m_acName);
}
//---------------------------------------------------------------------------
const char* NiVisualTracker::GraphCallbackObjectData::GetName()
{
    return m_acName;
}
//---------------------------------------------------------------------------
void NiVisualTracker::GraphCallbackObjectData::Draw()
{
    if (m_bShow)
    {
        m_spLines->RenderImmediate(NiRenderer::GetRenderer());
    }
}
//---------------------------------------------------------------------------
void NiVisualTracker::GraphCallbackObjectData::Update(float fTime)
{
    if (m_fLastTime != -NI_INFINITY && 
        fTime - m_fLastTime < m_fSamplingTime)
    {
        return;
    }

    float fDataValue = m_pkCallbackObj->TakeSample(fTime);
    if (fDataValue > m_fMaxValue)
        fDataValue = m_fMaxValue;
   
    float fWidth = NiAbs(m_kDimensions.m_left - m_kDimensions.m_right);
    float fHeight = NiAbs(m_kDimensions.m_top - m_kDimensions.m_bottom);
    
    NiLinesData* pkModelData = 
        (NiLinesData*) m_spLines->GetModelData();

    if (pkModelData)
    {
        NiPoint3* pkVerts = pkModelData->GetVertices();
        NiBool* pkConnects = pkModelData->GetFlags();
        if (pkVerts)
        {
            for (unsigned int ui = 0; ui < m_uiNumSamplesToKeep - 1; ui++)
            {
                pkVerts[ui] = pkVerts[ui+1];
                pkVerts[ui].x = (((float)ui) / (float)m_uiNumSamplesToKeep) *
                    fWidth;
            }

            pkVerts[m_uiNextVertex].x = (float)1.0f * fWidth; 
            pkVerts[m_uiNextVertex].y = fHeight - 
                ((float)fDataValue / m_fMaxValue) * fHeight;
            pkVerts[m_uiNextVertex].z = 0.0f;
            pkConnects[m_uiNextVertex] = false;
            
            pkModelData->MarkAsChanged(
                NiGeometryData::VERTEX_MASK);
            pkModelData->GetBound().ComputeFromData(
                m_uiNumSamplesToKeep, pkVerts);           
        }
    }   

    m_fLastTime = fTime;
}
//---------------------------------------------------------------------------
NiLines* NiVisualTracker::GraphCallbackObjectData::CreateLines(
    unsigned int m_uiNumSamplesToKeep, const NiColor& kColor,
    const NiRect<float>& kDimensions)
{
    // float fWidth = NiAbs(kDimensions.m_left - kDimensions.m_right);
    float fHeight = NiAbs(kDimensions.m_top - kDimensions.m_bottom);
    
    NiBool* pkConnections = NiAlloc(NiBool, m_uiNumSamplesToKeep);
    for (unsigned int ui = 0; ui < m_uiNumSamplesToKeep; ui++)
        pkConnections[ui] = true;
    NiPoint3* pkVertices = NiNew NiPoint3[m_uiNumSamplesToKeep];
    for (unsigned int ui = 0; ui < m_uiNumSamplesToKeep; ui++)
        pkVertices[ui] = NiPoint3(0.0f, fHeight, 0.0f);
    NiColorA* pkColors = NULL;

    NiLines* m_spLines = NiNew NiLines(m_uiNumSamplesToKeep, pkVertices, 
        pkColors, NULL, 0, NiGeometryData::NBT_METHOD_NONE, pkConnections);
    m_spLines->GetModelData()->SetConsistency(NiGeometryData::MUTABLE);
    NiPoint3 kPos = NiPoint3::ZERO;
    kPos.x = kDimensions.m_left;
    kPos.y = kDimensions.m_top;
    m_spLines->SetTranslate(NiPoint3(kDimensions.m_left, kDimensions.m_top, 
        0.0f));
    NiMatrix3 kRot;
    kRot.MakeIdentity();
    m_spLines->SetRotate(kRot);
    m_spLines->SetScale(1.0f);

    NiMaterialProperty* pkMatProp = NiNew NiMaterialProperty();
    pkMatProp->SetEmittance(kColor);

    m_spLines->AttachProperty(pkMatProp);

    NiZBufferProperty* pkZProp = NiNew NiZBufferProperty();
    pkZProp->SetZBufferTest(false);
    pkZProp->SetZBufferWrite(false);
    m_spLines->AttachProperty(pkZProp);

   
    m_spLines->Update(0.0f);
    m_spLines->UpdateProperties();
    m_spLines->UpdateEffects();
    m_spLines->UpdateNodeBound();

    return m_spLines;
}
//---------------------------------------------------------------------------
NiVisualTracker::GraphCallbackObjectData::GraphCallbackObjectData(
    GraphCallbackObject* pkObject, const char* pcName,
    const NiColor& kColor, unsigned int uiNumSamplesToKeep, float fMaxValue,
    float fSamplingTime, bool bShow, const NiRect<float>& kDimensions,
    NiNode* pkParentNode, unsigned int& uiLegendX, unsigned int& uiLegendY,
    NiScreenTextureArray& kTextures)
{
    m_pkCallbackObj = pkObject;
    m_bShow = bShow;
    m_uiNumSamplesToKeep = uiNumSamplesToKeep;
    m_fSamplingTime = fSamplingTime;
    m_kDimensions = kDimensions;
    m_spLines = CreateLines(m_uiNumSamplesToKeep, kColor, kDimensions);
    pkParentNode->AttachChild(m_spLines);
    m_kColor = kColor;
    m_fLastTime = -NI_INFINITY;
    NiSprintf(m_acName, 255, "%s", pcName);
    m_acName[255] = '\0';
    m_fMaxValue = fMaxValue;
    m_uiNextVertex = m_uiNumSamplesToKeep - 1;

    NiColorA kTextColor(m_kColor.r, m_kColor.g, m_kColor.b, 1.0f);
    m_pkText = NiNew ScreenText(15, &kTextures, kTextColor);
    m_pkText->SetTextOrigin(uiLegendX, uiLegendY);
    m_pkText->SetVisible(true);
    m_pkText->SetString(m_acName);

    uiLegendX += m_pkText->GetWidth();
}
//---------------------------------------------------------------------------
NiVisualTracker::GraphCallbackObjectData::~GraphCallbackObjectData()
{
    NiDelete m_pkCallbackObj;
    NiDelete m_pkText;
}
//---------------------------------------------------------------------------
void NiVisualTracker::GraphCallbackObjectData::AdjustLegend(
    int iX, int iY)
{
    unsigned int uiXCurrent, uiYCurrent;
    m_pkText->GetTextOrigin(uiXCurrent, uiYCurrent);
    uiXCurrent += iX;
    uiYCurrent += iY;
    m_pkText->SetTextOrigin(uiXCurrent, uiYCurrent);
}
//---------------------------------------------------------------------------
unsigned int NiVisualTracker::GraphCallbackObjectData::GetLegendWidth()
{
    return m_pkText->GetWidth();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  NiVisualTracker::ScreenText implementation
//---------------------------------------------------------------------------
NiTexturePtr NiVisualTracker::ScreenText::ms_spTextTexture = 0;
unsigned int NiVisualTracker::ScreenText::ms_uiCharWidth = 11;
unsigned int NiVisualTracker::ScreenText::ms_uiCharHeight = 21;
unsigned int NiVisualTracker::ScreenText::ms_uiCharSpacingX = 11;
unsigned int NiVisualTracker::ScreenText::ms_uiCharSpacingY = 21;
const unsigned int NiVisualTracker::ScreenText::ms_uiCharBaseU = 0;
const unsigned int NiVisualTracker::ScreenText::ms_uiCharBaseV = 0;
const unsigned int NiVisualTracker::ScreenText::ms_uiASCIIMin = 33;
const unsigned int NiVisualTracker::ScreenText::ms_uiASCIIMax = 122;
unsigned int NiVisualTracker::ScreenText::ms_uiASCIICols = 23;
int NiVisualTracker::ScreenText::ms_iCount = 0;

//---------------------------------------------------------------------------
void NiVisualTracker::ScreenText::SetString(const char* pcString)
{
    unsigned int uiNewLength = strlen(pcString);

    if (uiNewLength >= m_uiMaxChars)
        uiNewLength = m_uiMaxChars - 1;
    m_uiNumChars = uiNewLength + 1;

    NiStrncpy(m_pcString, m_uiMaxChars, pcString, uiNewLength);
    RecreateText();
}
//---------------------------------------------------------------------------
const char* NiVisualTracker::ScreenText::GetString() const
{
    return m_pcString;
}
//---------------------------------------------------------------------------
void NiVisualTracker::ScreenText::SetTextOrigin(unsigned int uiX, 
    unsigned int uiY)
{
    if (m_uiTextOriginX != uiX || m_uiTextOriginY != uiY)
    {
        m_uiTextOriginX = uiX;
        m_uiTextOriginY = uiY;
        RecreateText();
    }
}
//---------------------------------------------------------------------------
void NiVisualTracker::ScreenText::GetTextOrigin(unsigned int& uiX, 
    unsigned int& uiY) const
{
    uiX = m_uiTextOriginX;
    uiY = m_uiTextOriginY;
}
//---------------------------------------------------------------------------
void NiVisualTracker::ScreenText::SetScrollDown(bool bDown)
{
    if (m_bScrollDown != bDown)
    {
        m_bScrollDown = bDown;
        RecreateText();
    }
}
//---------------------------------------------------------------------------
bool NiVisualTracker::ScreenText::GetScrollDown() const
{
    return m_bScrollDown;
}
//---------------------------------------------------------------------------
const NiColorA& NiVisualTracker::ScreenText::GetColor() const
{
    return m_kColor;
}
//---------------------------------------------------------------------------
short NiVisualTracker::ScreenText::GetHeight() const
{
    return (short)ms_uiCharSpacingY;
}
//---------------------------------------------------------------------------
short NiVisualTracker::ScreenText::GetWidth() const
{
    return (short)(ms_uiCharSpacingX * m_uiNumChars);
}
//---------------------------------------------------------------------------
short NiVisualTracker::ScreenText::GetCharacterSpacing() const
{
    return (short)ms_uiCharSpacingX;
}
//---------------------------------------------------------------------------
void NiVisualTracker::ScreenText::SetMaxLineLength(unsigned int uiColumns)
{
    if (m_uiMaxCols != uiColumns)
    {
        m_uiMaxCols = uiColumns;
        RecreateText();
    }
}
//---------------------------------------------------------------------------
unsigned int NiVisualTracker::ScreenText::GetMaxLineLength() const
{
    return m_uiMaxCols;
}
//---------------------------------------------------------------------------
NiTexture* NiVisualTracker::ScreenText::GetTexture()
{
    return ms_spTextTexture;
}
//---------------------------------------------------------------------------
NiVisualTracker::ScreenText::ScreenText(unsigned int uiMaxChars,
    NiScreenTextureArray* pkScreenTextures, const NiColorA& kColor)
{
    Init(uiMaxChars, pkScreenTextures, kColor);
}
//---------------------------------------------------------------------------
NiVisualTracker::ScreenText::~ScreenText()
{
    m_pkScreenTextures->Remove(m_spScreenTexture);
    m_pkScreenTextures->Compact();

    if (--ms_iCount < 1)
        ms_spTextTexture = 0;

    NiFree(m_pcString);
}
//---------------------------------------------------------------------------
void NiVisualTracker::ScreenText::Init(unsigned int uiMaxChars,
    NiScreenTextureArray* pkScreenTextures, const NiColorA& kColor)
{
    ms_iCount++;

    if (!ms_spTextTexture)
    {
        ms_spTextTexture = NiSourceTexture::Create(
            CreateCourier16PixelData());
    }

    m_uiMaxChars = uiMaxChars;
    m_kColor = kColor;
    
    m_spScreenTexture = NiNew NiScreenTexture(ms_spTextTexture);
    m_spScreenTexture->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
    m_pkScreenTextures = pkScreenTextures;

    m_uiTextOriginX = 0;
    m_uiTextOriginY = 0;
    m_bScrollDown = true;

    // String is _not_ NULL terminated
    m_pcString = NiAlloc(char, m_uiMaxChars);
    m_uiNumChars = 0;

    m_uiMaxCols = 40;

    m_uiNumRects = 0;
    m_uiNumCurrentRows = 0;
    m_uiCurrentColumn = 0;
}
//---------------------------------------------------------------------------
void NiVisualTracker::ScreenText::RecreateText()
{
    m_spScreenTexture->RemoveAllScreenRects();

    m_uiNumCurrentRows = 0;
    m_uiCurrentColumn = 0;

    for (unsigned int i = 0; i < m_uiNumChars; i++)
    {
        char cChar = m_pcString[i];

        // if we are at the end of a line or if the char is '\n' then move
        // to the start of the next line
        if ((m_uiCurrentColumn >= m_uiMaxCols) || (cChar == '\n'))
        {
            m_uiNumCurrentRows++;
            m_uiCurrentColumn = 0;

            if (!m_bScrollDown)
            {
                // Move all characters up one row
                unsigned int uiNumRects = 
                    m_spScreenTexture->GetNumScreenRects();
                for (unsigned int j = 0; j < uiNumRects; j++)
                {
                    NiScreenTexture::ScreenRect& kRect = 
                        m_spScreenTexture->GetScreenRect(j);
                    kRect.m_sPixTop -= ms_uiCharSpacingY;
                }
            }

            // if the extra char is a '\n', skip it
            if (cChar == '\n')
                continue;
        }

        unsigned int uiChar = (unsigned int)cChar;

        // skip whitespace or unprintable character
        if ((uiChar >= ms_uiASCIIMin) && (uiChar <= ms_uiASCIIMax))
        {
            uiChar -= ms_uiASCIIMin;
    
            unsigned short usPixTop = m_uiTextOriginY;
            if (m_bScrollDown)
                usPixTop += (m_uiNumCurrentRows) * ms_uiCharSpacingY;
            unsigned short usPixLeft = m_uiTextOriginX + 
                m_uiCurrentColumn * ms_uiCharSpacingX;

            unsigned short usTexTop = ms_uiCharBaseV + 
                (uiChar / ms_uiASCIICols) * ms_uiCharSpacingY;
            unsigned short usTexLeft = ms_uiCharBaseU + 
                (uiChar % ms_uiASCIICols) * ms_uiCharSpacingX;

            m_spScreenTexture->AddNewScreenRect(usPixTop, usPixLeft, 
                ms_uiCharWidth, ms_uiCharHeight, usTexTop, usTexLeft, 
                m_kColor);
        }

        m_uiCurrentColumn++;
    }
    m_spScreenTexture->MarkAsChanged(NiScreenTexture::EVERYTHING_MASK);
}
//---------------------------------------------------------------------------
void NiVisualTracker::ScreenText::AppendCharacter(char cChar)
{
    if (m_uiNumChars >= (m_uiMaxChars - 1))
        return;
    
    m_pcString[m_uiNumChars] = cChar;

    m_uiNumChars++;
    m_pcString[m_uiNumChars] = '\0';

    m_spScreenTexture->MarkAsChanged(NiScreenTexture::EVERYTHING_MASK);

    // if we are at the end of a line or if the char is '\n' then move
    // to the start of the next line
    if ((m_uiCurrentColumn >= m_uiMaxCols) || (cChar == '\n'))
    {
        m_uiNumCurrentRows++;
        m_uiCurrentColumn = 0;

        if (!m_bScrollDown)
        {
            // Move all characters up one row
            unsigned int uiNumRects = m_spScreenTexture->GetNumScreenRects();
            for (unsigned int i = 0; i < uiNumRects; i++)
            {
                NiScreenTexture::ScreenRect& kRect = 
                    m_spScreenTexture->GetScreenRect(i);
                kRect.m_sPixTop -= ms_uiCharSpacingY;
            }
        }

        // if the extra char is a '\n', skip it
        if (cChar == '\n')
            return;
    }

    unsigned int uiChar = (unsigned int)cChar;

    // skip whitespace or unprintable character
    if ((uiChar >= ms_uiASCIIMin) && (uiChar <= ms_uiASCIIMax))
    {
        uiChar -= ms_uiASCIIMin;
    
        unsigned short usPixTop = m_uiTextOriginY;
        if (m_bScrollDown)
            usPixTop += (m_uiNumCurrentRows + 1) * ms_uiCharSpacingY;
        unsigned short usPixLeft = m_uiTextOriginX + 
            m_uiCurrentColumn * ms_uiCharSpacingX;

        unsigned short usTexTop = ms_uiCharBaseV + 
            (uiChar / ms_uiASCIICols) * ms_uiCharSpacingY;
        unsigned short usTexLeft = ms_uiCharBaseU + 
            (uiChar % ms_uiASCIICols) * ms_uiCharSpacingX;

        m_spScreenTexture->AddNewScreenRect(usPixTop, usPixLeft, 
            ms_uiCharWidth, ms_uiCharHeight, usTexTop, usTexLeft, m_kColor);
    }

    m_uiCurrentColumn++;
}
//---------------------------------------------------------------------------
void NiVisualTracker::ScreenText::DeleteLastCharacter()
{
    if (!m_uiNumChars)
        return;

    m_uiNumChars--;
    char cChar = m_pcString[m_uiNumChars];
    m_pcString[m_uiNumChars] = '\0';

    // if we are at the beginning of a line then return to end of the 
    // previous line - easiest way to do this is to recreate the text
    if (m_uiCurrentColumn == 0)
    {
        RecreateText();
    }
    else
    {
        m_spScreenTexture->MarkAsChanged(NiScreenTexture::EVERYTHING_MASK);

        unsigned int uiChar = (unsigned int)cChar;

        // skip whitespace or unprintable character
        if ((uiChar >= ms_uiASCIIMin) && (uiChar <= ms_uiASCIIMax))
        {
            // Remove character
            m_spScreenTexture->RemoveScreenRect(
                m_spScreenTexture->GetNumScreenRects() - 1);
        }
        m_uiCurrentColumn--;
    }
}
//---------------------------------------------------------------------------
void NiVisualTracker::ScreenText::SetColor(NiColorA& kNewColor)
{
    if (kNewColor == m_kColor)
        return;

    m_kColor = kNewColor;

    // Update all of the Screen Rectangles
    const unsigned int uiQuantity = m_spScreenTexture->GetNumScreenRects();
    for (unsigned int uiLoop = 0; uiLoop < uiQuantity; uiLoop++)
    {
        NiScreenTexture::ScreenRect& kTemp = 
            m_spScreenTexture->GetScreenRect(uiLoop);

        kTemp.m_kColor = m_kColor;
    }

    m_spScreenTexture->MarkAsChanged(NiScreenTexture::COLOR_MASK);
}
//---------------------------------------------------------------------------
void NiVisualTracker::ScreenText::SetVisible(bool bVisible)
{
    if (bVisible)
        m_pkScreenTextures->AddFirstEmpty(m_spScreenTexture);
    else
        m_pkScreenTextures->Remove(m_spScreenTexture);
}
//---------------------------------------------------------------------------

