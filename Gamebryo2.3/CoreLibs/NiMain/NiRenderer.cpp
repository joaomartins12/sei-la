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

#include "NiRenderer.h"
#include "NiAccumulator.h"
#include "NiNode.h"
#include "NiCamera.h"
#include "NiGeometry.h"
#include "NiMaterialNodeLibrary.h"
#include "NiMaterialProperty.h"
#include "NiRenderTargetGroup.h"
#include "NiStandardMaterial.h"
#include "StandardMaterialNodeLibrary/NiStandardMaterialNodeLibrary.h"
#include "NiTriShape.h"
#include "NiTriStrips.h"
#include "NiScreenElements.h"

NiImplementRTTI(NiRenderer, NiObject);

char NiRenderer::ms_acErrorString[256] = "";
NiRenderer* NiRenderer::ms_pkRenderer = NULL;

//---------------------------------------------------------------------------
NiRenderer::NiRenderer() :
    m_bRenderTargetGroupActive(false),
    m_eFrameState(FRAMESTATE_OUTSIDE_FRAME),
    m_uiFrameID(0),
    m_bBatchRendering(false)
{
    NIASSERT(ms_pkRenderer == NULL);
    ms_pkRenderer = this;

    NiMaterialNodeLibraryPtr spMaterialNodeLibrary = 
        NiStandardMaterialNodeLibrary::CreateMaterialNodeLibrary();
    NiStandardMaterial* pkStandardMaterial =
        NiNew NiStandardMaterial(spMaterialNodeLibrary, false);
    pkStandardMaterial->AddDefaultFallbacks();
    m_spInitialDefaultMaterial = pkStandardMaterial;
}
//---------------------------------------------------------------------------
NiRenderer::~NiRenderer()
{
    ms_pkRenderer = NULL;
}
//---------------------------------------------------------------------------
void NiRenderer::RemoveEffect(NiDynamicEffect* pkEffect)
{
    if (ms_pkRenderer)
        ms_pkRenderer->PurgeEffect(pkEffect);
}
//---------------------------------------------------------------------------
void NiRenderer::RemoveGeometryData(NiGeometryData* pkData)
{
    if (ms_pkRenderer)
        ms_pkRenderer->PurgeGeometryData(pkData);
}
//---------------------------------------------------------------------------
void NiRenderer::RemoveMaterial(NiMaterialProperty* pkMaterial)
{
    if (ms_pkRenderer)
        ms_pkRenderer->PurgeMaterial(pkMaterial);
}
//---------------------------------------------------------------------------
void NiRenderer::RemoveScreenTextureRendererData(
    NiScreenTexture* pkScreenTexture)
{
    if (ms_pkRenderer)
        ms_pkRenderer->PurgeScreenTextureRendererData(pkScreenTexture);
}
//---------------------------------------------------------------------------
void NiRenderer::RemoveSkinPartitionRendererData(
    NiSkinPartition* pkSkinPartition)
{
    if (ms_pkRenderer)
        ms_pkRenderer->PurgeSkinPartitionRendererData(pkSkinPartition);
}
//---------------------------------------------------------------------------
void NiRenderer::RemoveSkinInstance(NiSkinInstance* pkSkinInstance)
{
    if (ms_pkRenderer)
        ms_pkRenderer->PurgeSkinInstance(pkSkinInstance);
}
//---------------------------------------------------------------------------
void NiRenderer::PurgeAllRendererData(NiAVObject* pkScene)
{
    PurgeAllTextures(true);
    if (pkScene)
        pkScene->PurgeRendererData(this);
};
//---------------------------------------------------------------------------
bool NiRenderer::MapWindowPointToBufferPoint(unsigned int uiX,
    unsigned int uiY, float& fX, float& fY, 
    const NiRenderTargetGroup* pkTarget)
{
    //const NiRenderTargetGroup* pkCurrentRTGroup = pkTarget;

    if (pkTarget == NULL)
        pkTarget = GetDefaultRenderTargetGroup();

    unsigned int uiXSize = pkTarget->GetWidth(0);
    unsigned int uiYSize = pkTarget->GetHeight(0);

    if (uiXSize && uiYSize)
    {
        fX = (float)(uiX) / (float)uiXSize;
        fY = 1.0f - (float)(uiY) / (float)uiYSize;

        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
void NiRenderer::Do_BeginBatch(NiPropertyState* pkPropertyState,
    NiDynamicEffectState* pkEffectState)
{
    SetPropertyState(pkPropertyState);
    SetEffectState(pkEffectState);
}
//---------------------------------------------------------------------------
void NiRenderer::Do_EndBatch()
{
}
//---------------------------------------------------------------------------
void NiRenderer::Do_BatchRenderShape(NiTriShape* pkTriShape)
{
    // Property and effect states need to be set before object is drawn
    SetPropertyState(pkTriShape->GetPropertyState());
    SetEffectState(pkTriShape->GetEffectState());

    RenderShape(pkTriShape);
}
//---------------------------------------------------------------------------
void NiRenderer::Do_BatchRenderStrips(NiTriStrips* pkTriStrips)
{
    // Property and effect states need to be set before object is drawn
    SetPropertyState(pkTriStrips->GetPropertyState());
    SetEffectState(pkTriStrips->GetEffectState());

    RenderTristrips(pkTriStrips);
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiRenderer::FindClosestDepthStencilFormat( 
    const NiPixelFormat* pkFrontBufferFormat) const
{
    return FindClosestDepthStencilFormat(pkFrontBufferFormat, 32, 8);
}
//---------------------------------------------------------------------------
void NiRenderer::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiRenderer::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
void NiRenderer::SetDefaultProgramCache(NiFragmentMaterial* pkMaterial)
{
    SetDefaultProgramCache(pkMaterial, 
        NiFragmentMaterial::GetDefaultAutoSaveProgramCache(),
        NiFragmentMaterial::GetDefaultWriteDebugProgramData(),
        NiFragmentMaterial::GetDefaultLoadProgramCacheOnCreation(),
        NiFragmentMaterial::GetDefaultLockProgramCache(),
        NiMaterial::GetDefaultWorkingDirectory());
}
//---------------------------------------------------------------------------
void NiRenderer::Message(const char* pcFormat, ...)
{
#ifdef _DEBUG
    char acOutput[1024] = "NiRenderer MESSAGE: ";
    int iIndex = strlen(acOutput);

    va_list kArgs;
    va_start(kArgs, pcFormat);
    iIndex += NiVsprintf(&acOutput[iIndex], 1024 - iIndex, pcFormat, kArgs);
    va_end(kArgs);

    // Newline and null terminate
    iIndex = NiMin(iIndex, sizeof(acOutput) - 2);
    if (acOutput[iIndex-1] != '\n')
    {
        acOutput[iIndex  ] = '\n';
        acOutput[iIndex+1] = NULL;
    }
    else
    {
        acOutput[iIndex] = NULL;
    }

    NiOutputDebugString(acOutput);
#endif  //#ifdef _DEBUG
}
//---------------------------------------------------------------------------
void NiRenderer::Warning(const char* pcFormat, ...)
{
#ifdef _DEBUG
    char acOutput[1024] = "NiRenderer WARNING: ";
    int iIndex = strlen(acOutput);

    va_list kArgs;
    va_start(kArgs, pcFormat);
    iIndex += NiVsprintf(&acOutput[iIndex], 1024 - iIndex, pcFormat, kArgs);
    va_end(kArgs);

    // Newline and null terminate
    iIndex = NiMin(iIndex, sizeof(acOutput) - 2);
    if (acOutput[iIndex-1] != '\n')
    {
        acOutput[iIndex  ] = '\n';
        acOutput[iIndex+1] = NULL;
    }
    else
    {
        acOutput[iIndex] = NULL;
    }

    NiOutputDebugString(acOutput);
#endif  //#ifdef _DEBUG
}
//---------------------------------------------------------------------------
void NiRenderer::Error(const char* pcFormat, ...)
{
#ifdef _DEBUG
    char acOutput[1024] = "NiRenderer ERROR: ";
    int iIndex = strlen(acOutput);

    va_list kArgs;
    va_start(kArgs, pcFormat);
    iIndex += NiVsprintf(&acOutput[iIndex], 1024 - iIndex, pcFormat, kArgs);
    va_end(kArgs);

    // Newline and null terminate
    iIndex = NiMin(iIndex, sizeof(acOutput) - 2);
    if (acOutput[iIndex-1] != '\n')
    {
        acOutput[iIndex  ] = '\n';
        acOutput[iIndex+1] = NULL;
    }
    else
    {
        acOutput[iIndex] = NULL;
    }

    NiOutputDebugString(acOutput);
#endif  //#ifdef _DEBUG
}
//---------------------------------------------------------------------------
void NiRenderer::_SDMShutdown()
{
    ms_pkRenderer = NULL;
}
//---------------------------------------------------------------------------
void NiRenderer::SetCameraData(const NiCamera* pkCamera)
{
    SetCameraData(pkCamera->GetWorldLocation(),
        pkCamera->GetWorldDirection(), pkCamera->GetWorldUpVector(),
        pkCamera->GetWorldRightVector(), pkCamera->GetViewFrustum(),
        pkCamera->GetViewPort());
}
//---------------------------------------------------------------------------
void NiRenderer::GetCameraData(NiCamera& kCamera)
{
    NiPoint3 kWorldLoc, kWorldDir, kWorldUp, kWorldRight;
    NiFrustum kFrustum;
    NiRect<float> kPort;
    GetCameraData(kWorldLoc, kWorldDir, kWorldUp, kWorldRight, kFrustum,
        kPort);

    // Build world-space camera transform.
    NiTransform kWorldTransform;
    kWorldTransform.m_Translate = kWorldLoc;
    kWorldTransform.m_Rotate = NiMatrix3(kWorldDir, kWorldUp, kWorldRight);
    kWorldTransform.m_fScale = 1.0f;

    // Compute local camera transform.
    NiTransform kLocalTransform;
    NiNode* pkParent = kCamera.GetParent();
    if (pkParent)
    {
        // If camera has a parent, un-apply parent transform.
        NiTransform kInverseParent;
        pkParent->GetWorldTransform().Invert(kInverseParent);
        kLocalTransform = kInverseParent * kWorldTransform;
    }
    else
    {
        kLocalTransform = kWorldTransform;
    }

    // Set both local and world transform on camera so that it does not need
    // to be updated here.
    kCamera.SetTranslate(kLocalTransform.m_Translate);
    kCamera.SetRotate(kLocalTransform.m_Rotate);
    kCamera.SetScale(kLocalTransform.m_fScale);
    kCamera.SetWorldTranslate(kWorldTransform.m_Translate);
    kCamera.SetWorldRotate(kWorldTransform.m_Rotate);
    kCamera.SetWorldScale(kWorldTransform.m_fScale);

    // Set frustum and viewport on camera.
    kCamera.SetViewFrustum(kFrustum);
    kCamera.SetViewPort(kPort);
}
//---------------------------------------------------------------------------
void NiRenderer::SetDefaultMaterial(NiMaterial* pkMaterial)
{
    if (pkMaterial == NULL)
        pkMaterial = m_spInitialDefaultMaterial;
    m_spCurrentDefaultMaterial = pkMaterial;
    NIASSERT(m_spCurrentDefaultMaterial);
}
//---------------------------------------------------------------------------
void NiRenderer::ApplyDefaultMaterial(NiGeometry* pkGeometry)
{
    pkGeometry->ApplyAndSetActiveMaterial(GetDefaultMaterial());
}
//---------------------------------------------------------------------------
void NiRenderer::GetOnScreenCoord(
    const float fXOffset, const float fYOffset,
    const unsigned int uiWidth, const unsigned int uiHeight,
    unsigned int& uiOutX, unsigned int& uiOutY,
    const DisplayCorner eCorner, 
    const bool bForceSafeZone)
{
    unsigned int uiX = 0;
    unsigned int uiY = 0;
    unsigned int uiXOffset, uiYOffset;

    unsigned int uiScreenWidth = GetDefaultBackBuffer()->GetWidth();
    unsigned int uiScreenHeight = GetDefaultBackBuffer()->GetHeight();

    ConvertFromNDCToPixels(fXOffset, fYOffset, uiXOffset, uiYOffset);

    // Use given ScreenCorner to determine which direction the
    // given offset will be moved.
    switch (eCorner)
    {
    case CORNER_TOP_LEFT:
        uiX = uiXOffset;
        uiY = uiYOffset;
        break;
    case CORNER_TOP_RIGHT:
        uiX = uiScreenWidth - uiXOffset;
        uiY = uiYOffset;
        break;
    case CORNER_BOTTOM_LEFT:
        uiX = uiXOffset;
        uiY = uiScreenHeight - uiYOffset;
        break;
    case CORNER_BOTTOM_RIGHT:
        uiX = uiScreenWidth - uiXOffset;
        uiY = uiScreenHeight - uiYOffset;
        break;
    }

    unsigned int uiRight = uiScreenWidth;
    unsigned int uiBottom = uiScreenHeight;

    if (bForceSafeZone)
    {
        ForceInSafeZone(uiRight, uiBottom);
        ForceInSafeZone(uiX, uiY);
    }

    // Ensure an object of uiWidth and uiHeight will be able to fully 
    // fit onto the display.
    int iXDiff = uiRight - (uiX + uiWidth); 
    int iYDiff = uiBottom - (uiY + uiHeight);
    iXDiff = NiMin(0, iXDiff);
    iYDiff = NiMin(0, iYDiff);

    uiOutX = uiX + iXDiff;
    uiOutY = uiY + iYDiff;
}
//---------------------------------------------------------------------------
NiScreenElements* NiRenderer::CreateScreenElement(
    const float fXOffset, 
    const float fYOffset,
    const unsigned int uiWidth, 
    const unsigned int uiHeight,
    const DisplayCorner eCorner, 
    const bool bForceSafeZone)
{
    unsigned int uiX, uiY;

    GetOnScreenCoord(fXOffset, fYOffset, uiWidth, uiHeight, 
        uiX, uiY, eCorner, bForceSafeZone);

    NiScreenElements* pkScreenElements = NiNew NiScreenElements(
        NiNew NiScreenElementsData(false, true, 1));

    float fX, fY;
    float fWidth, fHeight;
    ConvertFromPixelsToNDC(uiX, uiY, fX, fY);
    ConvertFromPixelsToNDC(uiWidth, uiHeight, fWidth, fHeight);

    // We know that the polygon handle is zero and will use it directly in
    // the vertex and texture coordinate assignments.
    pkScreenElements->Insert(4);
    pkScreenElements->SetRectangle(0, fX, fY, fWidth, fHeight);
    pkScreenElements->SetTextures(0, 0, 0.0f, 0.0f, 1.0f, 1.0f);
    pkScreenElements->UpdateBound();

    return pkScreenElements;
}
//---------------------------------------------------------------------------
bool NiRenderer::BeginInternalFrame()
{
    if (m_eFrameState == FRAMESTATE_INTERNAL_FRAME)
        return false;

    m_eSavedFrameState = m_eFrameState;
    m_eFrameState = FRAMESTATE_INTERNAL_FRAME;
    return true;
}
//---------------------------------------------------------------------------
bool NiRenderer::EndInternalFrame()
{
    if (!Check_FrameState("EndInternalFrame", FRAMESTATE_INTERNAL_FRAME))
        return false;
    m_eFrameState = m_eSavedFrameState;
    return true;
}
//---------------------------------------------------------------------------
bool NiRenderer::SaveScreenShot(const char* pcFilename,
    EScreenshotFormat eFormat)
{
    return false;
}
//---------------------------------------------------------------------------
