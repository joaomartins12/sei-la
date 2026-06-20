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
#include "NiMainPCH.h"

#include "NiAlphaProperty.h"
#include "NiRenderer.h"
#include "NiScreenTexture.h"
#include "NiVertexColorProperty.h"

NiImplementRTTI(NiScreenTexture, NiObject);

// construction and destruction
//---------------------------------------------------------------------------
NiScreenTexture::NiScreenTexture(NiTexture* pkTexture)
{
    m_usDirtyFlags = 0;

    // Texturing property
    m_spTexProp = NiNew NiTexturingProperty;
    m_spTexProp->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
    m_spTexProp->SetBaseTexture(pkTexture);
    m_spTexProp->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
    m_pkBuffData = 0;
}
//---------------------------------------------------------------------------
NiScreenTexture::NiScreenTexture()
{
    m_usDirtyFlags = 0;
    m_pkBuffData = 0;
}
//---------------------------------------------------------------------------
NiScreenTexture::~NiScreenTexture()
{
    NiRenderer::RemoveScreenTextureRendererData(this);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Screen Rect handling
//---------------------------------------------------------------------------
unsigned int NiScreenTexture::AddNewScreenRect(short sPixTop, short sPixLeft, 
    unsigned short usPixWidth, unsigned short usPixHeight, 
    unsigned short usTexTop, unsigned short usTexLeft, NiColorA kColor)
{
    ScreenRect kScreenRect;
    kScreenRect.m_sPixTop = sPixTop;
    kScreenRect.m_sPixLeft = sPixLeft;
    kScreenRect.m_usPixWidth = usPixWidth;
    kScreenRect.m_usPixHeight = usPixHeight;
    kScreenRect.m_usTexTop = usTexTop;
    kScreenRect.m_usTexLeft = usTexLeft;
    kScreenRect.m_kColor = kColor;

    m_kScreenRects.Add(kScreenRect);
    return m_kScreenRects.GetSize() - 1;
}
//---------------------------------------------------------------------------
void NiScreenTexture::RemoveScreenRect(unsigned int uiIndex)
{
    m_kScreenRects.OrderedRemoveAt(uiIndex);
}
//---------------------------------------------------------------------------
void NiScreenTexture::RemoveAllScreenRects()
{
    m_kScreenRects.RemoveAll();
}
//---------------------------------------------------------------------------
void NiScreenTexture::Draw(NiRenderer* pkRenderer)
{
    // Renderer must ignore property state and effect state. The default
    // NiProperties must be used, with these exceptions:
    // NiVertexColorProperty: SOURCE_EMISSIVE / LIGHTING_E
    // NiAlphaProperty: blend enabled, ALPHA_SRCALPHA / ALPHA_INVSRCALPHA
    // NiZBufferProperty: read and write disabled
    // NiTexturingProperty: specified in the NiScreenTexture itself

    pkRenderer->RenderScreenTexture(this);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiScreenTexture);
//---------------------------------------------------------------------------
void NiScreenTexture::CopyMembers(NiScreenTexture* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    for (unsigned int i = 0; i < m_kScreenRects.GetSize(); i++)
    {
        ScreenRect kNewRect;
        const ScreenRect& kRect = m_kScreenRects.GetAt(i);

        kNewRect.m_sPixTop = kRect.m_sPixTop;
        kNewRect.m_sPixLeft = kRect.m_sPixLeft;
        kNewRect.m_usPixWidth = kRect.m_usPixWidth;
        kNewRect.m_usPixHeight = kRect.m_usPixHeight;
        kNewRect.m_usTexTop = kRect.m_usTexTop;
        kNewRect.m_usTexLeft = kRect.m_usTexLeft;
        kNewRect.m_kColor = kRect.m_kColor;

        pkDest->m_kScreenRects.Add(kNewRect);
    }

    pkDest->m_spTexProp = m_spTexProp;

    // Note that a revision ID of 0 implies that the object is static and can
    // be optimized in a renderer - it implies that the application does not
    // intend to morph the data
    pkDest->m_usDirtyFlags = m_usDirtyFlags;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiScreenTexture);
//---------------------------------------------------------------------------
void NiScreenTexture::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    unsigned int uiNumRects;
    NiStreamLoadBinary(kStream, uiNumRects);

    for (unsigned int i = 0; i < uiNumRects; i++)
    {
        ScreenRect kRect;
        NiStreamLoadBinary(kStream, kRect.m_sPixTop);
        NiStreamLoadBinary(kStream, kRect.m_sPixLeft);
        NiStreamLoadBinary(kStream, kRect.m_usPixWidth);
        NiStreamLoadBinary(kStream, kRect.m_usPixHeight);
        NiStreamLoadBinary(kStream, kRect.m_usTexTop);
        NiStreamLoadBinary(kStream, kRect.m_usTexLeft);
        kRect.m_kColor.LoadBinary(kStream);

        m_kScreenRects.Add(kRect);
    }

    kStream.ReadLinkID();   // m_spTexProp
}
//---------------------------------------------------------------------------
void NiScreenTexture::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);

    m_spTexProp = (NiTexturingProperty*) kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiScreenTexture::RegisterStreamables(NiStream& kStream)
{
    if(!NiObject::RegisterStreamables(kStream))
        return false;

    m_spTexProp->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiScreenTexture::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    unsigned int uiNumRects = m_kScreenRects.GetSize();
    NiStreamSaveBinary(kStream, uiNumRects);

    for (unsigned int i = 0; i < uiNumRects; i++)
    {
        const ScreenRect& kRect = m_kScreenRects.GetAt(i);
        NiStreamSaveBinary(kStream, kRect.m_sPixTop);
        NiStreamSaveBinary(kStream, kRect.m_sPixLeft);
        NiStreamSaveBinary(kStream, kRect.m_usPixWidth);
        NiStreamSaveBinary(kStream, kRect.m_usPixHeight);
        NiStreamSaveBinary(kStream, kRect.m_usTexTop);
        NiStreamSaveBinary(kStream, kRect.m_usTexLeft);
        NiColorA kColor = kRect.m_kColor;
        kColor.SaveBinary(kStream);
    }

    kStream.SaveLinkID(m_spTexProp);
}
//---------------------------------------------------------------------------
bool NiScreenTexture::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiScreenTexture* pkTexture = (NiScreenTexture*) pkObject;

    unsigned int uiNumRects = m_kScreenRects.GetSize();
    if (uiNumRects != pkTexture->m_kScreenRects.GetSize())
        return false;

    for (unsigned int i = 0; i < uiNumRects; i++)
    {
        const ScreenRect& kRect = m_kScreenRects.GetAt(i);
        const ScreenRect& kOtherRect = pkTexture->m_kScreenRects.GetAt(i);

        if (kRect.m_sPixTop != kOtherRect.m_sPixTop &&
            kRect.m_sPixLeft != kOtherRect.m_sPixLeft &&
            kRect.m_usPixWidth != kOtherRect.m_usPixWidth &&
            kRect.m_usPixHeight != kOtherRect.m_usPixHeight &&
            kRect.m_usTexTop != kOtherRect.m_usTexTop &&
            kRect.m_usTexLeft != kOtherRect.m_usTexLeft &&
            kRect.m_kColor != kOtherRect.m_kColor)
        {
            return false;
        }
    }

    if (!m_spTexProp->IsEqual(pkTexture->m_spTexProp))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiScreenTexture::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiObject::GetViewerStrings(pStrings);
}
//---------------------------------------------------------------------------
