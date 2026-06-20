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

#include "NiRenderedCubeMap.h"
#include "NiRenderer.h"

NiImplementRTTI(NiRenderedCubeMap, NiRenderedTexture);

//---------------------------------------------------------------------------
NiRenderedCubeMap::NiRenderedCubeMap()
{
    m_eFace = FACE_POS_X;
}
//---------------------------------------------------------------------------
NiRenderedCubeMap* NiRenderedCubeMap::Create(unsigned int uiSize,
    NiRenderer* pkRenderer, FormatPrefs& kPrefs)
{
    if (!(pkRenderer && NiIsPowerOf2(uiSize)))
        return NULL;

    NiRenderedCubeMap* pkThis = NiNew NiRenderedCubeMap;
    pkThis->m_kFormatPrefs = kPrefs;
    for (unsigned int ui = 0; ui < FACE_NUM; ui++)
    {
        pkThis->m_aspFaceBuffers[ui] = Ni2DBuffer::Create(uiSize, uiSize);
    }

    // The creation of the renderer data should also populate
    // the Ni2DBuffer::RendererData
    if (!pkRenderer->CreateRenderedCubeMapRendererData(pkThis))
    {
        NiDelete pkThis;
        return NULL;
    }
    return pkThis;
}
//---------------------------------------------------------------------------
NiRenderedCubeMap* NiRenderedCubeMap::Create(Ni2DBuffer* pkPosXBuffer, 
    Ni2DBuffer* pkNegXBuffer, Ni2DBuffer* pkPosYBuffer, 
    Ni2DBuffer* pkNegYBuffer, Ni2DBuffer* pkPosZBuffer, 
    Ni2DBuffer* pkNegZBuffer, NiRenderer* pkRenderer)
{
    // All Faces of a cube map must be square and the same size
    NIASSERT(pkPosXBuffer->GetWidth() == pkPosXBuffer->GetHeight());
    NIASSERT(pkPosXBuffer->GetWidth() == pkNegXBuffer->GetWidth());
    NIASSERT(pkPosXBuffer->GetWidth() == pkNegXBuffer->GetHeight());
    NIASSERT(pkPosXBuffer->GetWidth() == pkPosYBuffer->GetWidth());
    NIASSERT(pkPosXBuffer->GetWidth() == pkPosYBuffer->GetHeight());
    NIASSERT(pkPosXBuffer->GetWidth() == pkNegYBuffer->GetWidth());
    NIASSERT(pkPosXBuffer->GetWidth() == pkNegYBuffer->GetHeight());
    NIASSERT(pkPosXBuffer->GetWidth() == pkPosZBuffer->GetWidth());
    NIASSERT(pkPosXBuffer->GetWidth() == pkPosZBuffer->GetHeight());
    NIASSERT(pkPosXBuffer->GetWidth() == pkNegZBuffer->GetWidth());
    NIASSERT(pkPosXBuffer->GetWidth() == pkNegZBuffer->GetHeight());

    // All Faces of a cube map must be the same format
    NIASSERT(pkPosXBuffer->GetPixelFormat() == pkNegXBuffer->GetPixelFormat());
    NIASSERT(pkPosXBuffer->GetPixelFormat() == pkPosYBuffer->GetPixelFormat());
    NIASSERT(pkPosXBuffer->GetPixelFormat() == pkNegYBuffer->GetPixelFormat());
    NIASSERT(pkPosXBuffer->GetPixelFormat() == pkPosZBuffer->GetPixelFormat());
    NIASSERT(pkPosXBuffer->GetPixelFormat() == pkNegZBuffer->GetPixelFormat());

    unsigned int uiSize = pkPosXBuffer->GetWidth();
    if (!(pkRenderer && NiIsPowerOf2(uiSize)))
        return NULL;

    NiRenderedCubeMap* pkThis = NiNew NiRenderedCubeMap;

    pkThis->m_aspFaceBuffers[FACE_POS_X] = pkPosXBuffer;
    pkThis->m_aspFaceBuffers[FACE_NEG_X] = pkNegXBuffer;
    pkThis->m_aspFaceBuffers[FACE_POS_Y] = pkPosYBuffer;
    pkThis->m_aspFaceBuffers[FACE_NEG_Y] = pkNegYBuffer;
    pkThis->m_aspFaceBuffers[FACE_POS_Z] = pkPosZBuffer;
    pkThis->m_aspFaceBuffers[FACE_NEG_Z] = pkNegZBuffer;

    if (!pkRenderer->CreateRenderedCubeMapRendererData(pkThis))
    {
        NiDelete pkThis;
        return NULL;
    }

    return pkThis;
}
//---------------------------------------------------------------------------
unsigned int NiRenderedCubeMap::GetWidth() const
{
    NIASSERT(m_aspFaceBuffers[0]);
    return m_aspFaceBuffers[0]->GetWidth();
}
//---------------------------------------------------------------------------
unsigned int NiRenderedCubeMap::GetHeight() const
{
    NIASSERT(m_aspFaceBuffers[0]);
    return m_aspFaceBuffers[0]->GetHeight();
}
//---------------------------------------------------------------------------
