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
#include "NiApplicationPCH.h"

#include "NiApplication.h"
#include "NiApplicationMetrics.h"
#include <NiFragmentMaterial.h>
#include <NiTNodeTraversal.h>

NiApplication* NiApplication::ms_pkApplication = 0;

NiInstanceRef NiApplication::ms_pInstance = 0;
NiAcceleratorRef NiApplication::ms_pAccel = 0;
NiCommand* NiApplication::ms_pkCommand = 0;


//---------------------------------------------------------------------------
void NiApplication::SetCommandLine(char* pcCommandLine)
{
    if (pcCommandLine && strlen(pcCommandLine) > 0)
        ms_pkCommand = NiNew NiCommand(pcCommandLine);
}
//---------------------------------------------------------------------------
void NiApplication::SetCommandLine(int iArgc, char** ppcArgv)
{
    ms_pkCommand = NiNew NiCommand(iArgc, ppcArgv);
}
//---------------------------------------------------------------------------
bool NiApplication::CreateScene()
{
    m_spScene = NiNew NiNode;
    return true;
}
//---------------------------------------------------------------------------
struct FindFirstCameraFunctor
{
    NiCamera* m_pkCamera;

    FindFirstCameraFunctor() : m_pkCamera(NULL) {}

    bool operator() (NiAVObject* pkObject)
    {
        m_pkCamera = NiDynamicCast(NiCamera, pkObject);
        return m_pkCamera == NULL; // if no camera, continue traversal
    }
};
//---------------------------------------------------------------------------
NiCamera* NiApplication::FindFirstCamera(NiNode* pkNode)
{
    FindFirstCameraFunctor kFindFirstCameraFunctor;
    NiTNodeTraversal::DepthFirst_FirstStop(
        pkNode,
        kFindFirstCameraFunctor);

    return kFindFirstCameraFunctor.m_pkCamera;
}
//---------------------------------------------------------------------------
bool NiApplication::CreateFrame()
{
    // Create a standard NiRenderFrame using m_spCamera, m_spScene, and 
    // the default render target.

    // Initialize shadow click generator and active it.
    NiShadowManager::SetActiveShadowClickGenerator(
        "NiDefaultShadowClickGenerator");

    // Initialize shadow manager parameters.
    NiShadowManager::SetSceneCamera(m_spCamera);
    NiShadowManager::SetCullingProcess(m_spCuller);

    // Create shadow render step.
    NiDefaultClickRenderStep* pkShadowRenderStep = NiNew
        NiDefaultClickRenderStep;
    pkShadowRenderStep->SetName(m_kShadowRenderStepName);
    pkShadowRenderStep->SetPreProcessingCallbackFunc(ShadowRenderStepPre);

    // Create 3D render view.
    m_spMainRenderView = NiNew Ni3DRenderView(m_spCamera, m_spCuller);
    m_spMainRenderView->SetName(m_kMainRenderViewName);
    NIASSERT(m_spScene);
    m_spMainRenderView->AppendScene(m_spScene);

    // Create main render click.
    NiViewRenderClick* pkMainRenderClick = NiNew NiViewRenderClick;
    pkMainRenderClick->SetName(m_kMainRenderClickName);
    pkMainRenderClick->AppendRenderView(m_spMainRenderView);
    pkMainRenderClick->SetViewport(m_spCamera->GetViewPort());
    pkMainRenderClick->SetClearAllBuffers(true);

    // Create accumulator processor and add it to render click.
    m_spAccumulatorProcessor = NiNew NiAccumulatorProcessor;
    pkMainRenderClick->SetProcessor(m_spAccumulatorProcessor);

    // Create screen element render click.
    NIASSERT(m_spScreenElementsRenderView);
    NiViewRenderClick* pkScreenElementsRenderClick = NiNew NiViewRenderClick;
    pkScreenElementsRenderClick->SetName(m_kScreenElementsRenderClickName);
    pkScreenElementsRenderClick->AppendRenderView(
        m_spScreenElementsRenderView);

    // Create main scene render step.
    NiDefaultClickRenderStep* pkMainRenderStep = NiNew
        NiDefaultClickRenderStep;
    pkMainRenderStep->SetName(m_kMainRenderStepName);
    pkMainRenderStep->AppendRenderClick(pkMainRenderClick);

    NIASSERT(m_spScreenTextureRenderClick && m_spVisualTrackerRenderClick);

    // Create screen space render step.
    NiDefaultClickRenderStep* pkScreenSpaceRenderStep = NiNew
        NiDefaultClickRenderStep;
    pkScreenSpaceRenderStep->SetName(m_kScreenSpaceRenderStepName);
    pkScreenSpaceRenderStep->AppendRenderClick(pkScreenElementsRenderClick);
    pkScreenSpaceRenderStep->AppendRenderClick(m_spScreenTextureRenderClick);
    pkScreenSpaceRenderStep->AppendRenderClick(m_spVisualTrackerRenderClick);

    // Create render frame that contains the render steps.
    m_spFrame = NiNew NiRenderFrame;
    m_spFrame->SetName(m_kRenderFrameName);
    m_spFrame->AppendRenderStep(pkShadowRenderStep);
    m_spFrame->AppendRenderStep(pkMainRenderStep);
    m_spFrame->AppendRenderStep(pkScreenSpaceRenderStep);

    return true;
}
//---------------------------------------------------------------------------
bool NiApplication::ShadowRenderStepPre(NiRenderStep* pkCurrentStep,
    void* pvCallbackData)
{
    // Get the list of render clicks from the shadow manager.
    const NiTPointerList<NiRenderClick*>& kShadowClicks =
        NiShadowManager::GenerateRenderClicks();

    // Replace the render clicks in the shadow render step with those provided
    // by the shadow manager.
    NIASSERT(NiIsKindOf(NiDefaultClickRenderStep, pkCurrentStep));
    NiDefaultClickRenderStep* pkClickRenderStep = (NiDefaultClickRenderStep*)
        pkCurrentStep;
    pkClickRenderStep->GetRenderClickList().RemoveAll();
    NiTListIterator kIter = kShadowClicks.GetHeadPos();
    while (kIter)
    {
        pkClickRenderStep->AppendRenderClick(kShadowClicks.GetNext(kIter));
    }

    return true;
}
//---------------------------------------------------------------------------
void NiApplication::ConfigureInputDevices()
{
    m_spInputSystem->ConfigureDevices();
}
//---------------------------------------------------------------------------
bool NiApplication::MeasureTime()
{
    if (!m_bUseFixedTime)
    {
        // start performance measurements
        if (m_fLastTime == -1.0f)
        {
            m_fLastTime = NiGetCurrentTimeInSec();
            m_fAccumTime = 0.0f;
            m_iClicks = 0;
        }

        // measure time
        m_fCurrentTime = NiGetCurrentTimeInSec();
        float fDeltaTime = m_fCurrentTime - m_fLastTime;

#if defined(WIN32)
        // NVPerfHUD support!
        if (m_bNVPerfHUD && fDeltaTime == 0.0f)
            return true;
#endif

        if (fDeltaTime < 0.0f)
            fDeltaTime = 0.0f;
        m_fLastTime = m_fCurrentTime;
        m_fAccumTime += fDeltaTime;

        // frame rate limiter
        if (m_fAccumTime < (m_fLastFrame + m_fMinFramePeriod))
            return false;

        m_fFrameTime = m_fAccumTime - m_fLastFrame;
        m_fLastFrame = m_fAccumTime;

        return true;
    }
    else
    {
        m_fCurrentTime += m_fFixedTimeIncrement;
        m_fAccumTime = ((float)m_iClicks) * m_fFixedTimeIncrement;      
        m_fFrameTime = m_fAccumTime - m_fLastFrame;
        m_fLastFrame = m_fAccumTime;

        return true;
    }
}
//---------------------------------------------------------------------------
void NiApplication::DrawFrameRate()
{
    if (--m_iTimer == 0)
    {
        if (m_fAccumTime > 0.0f)
        {
            if (m_fAccumTime > m_fLastFrameRateTime)
            {
                m_fFrameRate = (m_iClicks - m_iLastFrameRateClicks) / 
                    (m_fAccumTime - m_fLastFrameRateTime);
            }
        }
        else
        {
            m_fFrameRate = 0.0f;
        }

        char acMsg[32];
        NiSprintf(acMsg, 32, "fps: %.1f", m_fFrameRate);

#if defined(WIN32)
        unsigned int uiPane = 0;
        m_pkAppWindow->WriteToStatusWindow(uiPane, acMsg);
#else
        printf("%s\n", acMsg);
#endif

        m_iTimer = m_iMaxTimer;
        m_fLastFrameRateTime = m_fAccumTime;
        m_iLastFrameRateClicks = m_iClicks;
    }
}
//---------------------------------------------------------------------------
void NiApplication::Terminate()
{
    if (m_pkFrameRate)
    {
        m_pkFrameRate->Shutdown();
        NiDelete m_pkFrameRate;
        m_pkFrameRate = 0;
    }

    m_spCuller = NULL;

    m_spMainRenderView = NULL;
    m_spAccumulatorProcessor = NULL;
    m_spScreenElementsRenderView = NULL;
    m_spScreenTextureRenderClick = NULL;
    m_spVisualTrackerRenderClick = NULL;
    m_spFrame = NULL;

    m_spInputSystem = 0;
    m_spScene = 0;
    m_spCamera = 0;

    // The shadow manager needs to be shutdown after the scene has been 
    // destroyed, but before the renderer is destroyed.
    if (m_bUseFrameSystem)
    {
        NiShadowManager::Shutdown();
    }

    m_spRenderer = 0;
}
//---------------------------------------------------------------------------
void NiApplication::CreateMenuBar()
{
}
//---------------------------------------------------------------------------
void NiApplication::EnableFrameRate(bool bEnable)
{
    if (m_pkFrameRate)
    {
        if (m_bFrameRateEnabled != bEnable)
        {
            if (bEnable)
            {
                GetScreenElements().AddTail(m_pkFrameRate->GetElements());
            }
            else
            {
                GetScreenElements().Remove(m_pkFrameRate->GetElements());
            }
        }
           
        m_bFrameRateEnabled = bEnable;
    }
}
//---------------------------------------------------------------------------
void NiApplication::SetShowAllTrackers(bool bShow)
{
    const NiTPointerList<NiVisualTrackerPtr>& kVisualTrackers =
        GetVisualTrackers();
    NiTListIterator kIter = kVisualTrackers.GetHeadPos();
    while (kIter)
    {
        NiVisualTracker* pkTracker = kVisualTrackers.GetNext(kIter);
        NIASSERT(pkTracker);
        pkTracker->SetShow(bShow);
    }

    m_bShowAllTrackers = bShow;
}
//---------------------------------------------------------------------------
const char* NiApplication::GetMediaPath()
{
    return ms_acMediaPath;
}
//---------------------------------------------------------------------------
const char* NiApplication::ConvertMediaFilename(const char* pcFilename)
{
    NiSprintf(ms_acTempMediaFilename, NI_MAX_PATH, "%s%s", ms_acMediaPath, 
        pcFilename);
    NiPath::Standardize(ms_acTempMediaFilename);

    return ms_acTempMediaFilename;
}
//---------------------------------------------------------------------------
void NiApplication::UpdateInput()
{
    if (!m_spInputSystem)
    {
        return;
    }

    // If there are action maps defined the update with action maps
    // if not then just update all of the devices
    if (m_spInputSystem->GetMode() == NiInputSystem::MAPPED)
    {
        if (m_spInputSystem->UpdateActionMap() == NIIERR_DEVICELOST)
            m_spInputSystem->HandleDeviceChanges();
    }
    else
    {
        if (m_spInputSystem->UpdateAllDevices() == NIIERR_DEVICELOST)
            m_spInputSystem->HandleDeviceChanges();
    }
}
//---------------------------------------------------------------------------
void NiApplication::ProcessVisualTrackerInput()
{
    NiInputKeyboard* pkKeyboard = GetInputSystem()->GetKeyboard();
    if (pkKeyboard)
    {
        if (pkKeyboard->KeyWasPressed(NiInputKeyboard::KEY_F11))
        {
            SetShowAllTrackers(!m_bShowAllTrackers);
        }
    }

    // Allow ANY gamepad to show vis trackers
    NiInputGamePad* pkGamePad;
    for (unsigned int uiPort = 0; uiPort < NiInputSystem::MAX_GAMEPADS; 
        uiPort++)
    {
        pkGamePad = m_spInputSystem->GetGamePad(uiPort);
        if (pkGamePad)
        {
            if (pkGamePad->ButtonWasPressed(NiInputGamePad::NIGP_B))
            {
                SetShowAllTrackers(!m_bShowAllTrackers);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiApplication::OnIdle(void)
{
    // MeasureTime returns false if the frame rate is over the pre-set limit
    if (!MeasureTime())
        return;

    ResetFrameTimings();
    
    BeginUpdate();
    UpdateFrame();
    EndUpdate();

    BeginFrame();
    RenderFrame();
    EndFrame();

    DisplayFrame();

    UpdateVisualTrackers();
    UpdateMetrics();

    m_iClicks++;

    if ((m_fAccumTime >= m_fAppEndTime) && (m_fAppEndTime != NI_INFINITY))
    {
        QuitApplication();
    }
}
//--------------------------------------------------------------------------
void NiApplication::UpdateVisualTrackers()
{
    const NiTPointerList<NiVisualTrackerPtr>& kVisualTrackers =
        GetVisualTrackers();
    NiTListIterator kIter = kVisualTrackers.GetHeadPos();
    while (kIter)
    {
        NiVisualTracker* pkVisualTracker = kVisualTrackers.GetNext(kIter);
        NIASSERT(pkVisualTracker);
        pkVisualTracker->Update();
    }
}
//--------------------------------------------------------------------------
void NiApplication::ResetFrameTimings()
{
    m_fUpdateTime = 0.0f;
    m_fCullTime = 0.0f;
    m_fRenderTime = 0.0f;
}
//---------------------------------------------------------------------------
void NiApplication::BeginUpdate()
{
    NIASSERT(m_fBeginUpdate == 0.0f);
    m_fBeginUpdate = NiGetCurrentTimeInSec();
}
//---------------------------------------------------------------------------
void NiApplication::EndUpdate()
{
    float fEndUpdate = NiGetCurrentTimeInSec();
    m_fUpdateTime += fEndUpdate - m_fBeginUpdate;
    m_fBeginUpdate = 0.0f;
}
//---------------------------------------------------------------------------
void NiApplication::BeginCull()
{
    NIASSERT(m_fBeginCull == 0.0f);
    m_fBeginCull = NiGetCurrentTimeInSec();
}
//---------------------------------------------------------------------------
void NiApplication::EndCull()
{
    float fEndCull = NiGetCurrentTimeInSec();
    m_fCullTime += fEndCull - m_fBeginCull;
    m_fBeginCull = 0.0f;
}
//---------------------------------------------------------------------------
void NiApplication::BeginRender()
{
    NIASSERT(m_fBeginRender == 0.0f);
    m_fBeginRender = NiGetCurrentTimeInSec();
}
//---------------------------------------------------------------------------
void NiApplication::EndRender()
{
    float fEndRender = NiGetCurrentTimeInSec();
    m_fRenderTime += fEndRender - m_fBeginRender;
    m_fBeginRender = 0.0f;
}
//---------------------------------------------------------------------------
void NiApplication::UpdateFrame()
{
    // Update the input system
    UpdateInput();
    // Handle user input
    ProcessInput();

    if (m_bFrameRateEnabled && m_pkFrameRate)
    {
        m_pkFrameRate->TakeSample( );
        m_pkFrameRate->Update( );
    }

    m_fFrameRate = 1.0f / GetFrameTime();
}
//---------------------------------------------------------------------------
void NiApplication::BeginFrame()
{
    if (!m_bUseFrameSystem)
    {
        m_spRenderer->BeginFrame();
    }
}
//---------------------------------------------------------------------------
void NiApplication::CullFrame()
{
    BeginCull();
    NIASSERT(m_spCuller);
    NiCullScene(m_spCamera, m_spScene, *m_spCuller, m_kVisible);
    EndCull();
}
//---------------------------------------------------------------------------
void NiApplication::RenderFrame()
{
    if (m_bUseFrameSystem)
    {
        // Set renderer's sorter on the accumulator processor.
        if (m_spAccumulatorProcessor)
        {
            m_spAccumulatorProcessor->SetAccumulator(
                m_spRenderer->GetSorter());
        }

        // Ensure that the camera used for the shadowing system and the main
        // render view is m_spCamera (it may have been changed).
        if (m_spCamera)
        {
            if (NiShadowManager::GetShadowManager())
            {
                NiShadowManager::SetSceneCamera(m_spCamera);
            }
            if (m_spMainRenderView)
            {
                m_spMainRenderView->SetCamera(m_spCamera);
            }
        }

        // Draw the frame.
        m_spFrame->Draw();

        // Gather rendering statistics.
        m_spFrame->GatherStatistics(m_uiNumObjectsDrawn, m_fCullTime,
            m_fRenderTime);
    }
    else
    {
        m_spRenderer->BeginUsingDefaultRenderTargetGroup(
            NiRenderer::CLEAR_ALL);
        CullFrame();

        BeginRender();
        NiDrawVisibleArray(m_spCamera, m_kVisible);

        RenderScreenItems();
        EndRender();

        RenderVisualTrackers();
        m_spRenderer->EndUsingRenderTargetGroup();
    }
}
//---------------------------------------------------------------------------
void NiApplication::RenderScreenItems()
{
    m_spRenderer->SetScreenSpaceCameraData();
    const NiTPointerList<NiScreenElementsPtr>& kScreenElements =
        GetScreenElements();
    NiTListIterator kIter = kScreenElements.GetHeadPos();
    while (kIter)
    {
        NiScreenElements* pkScreenElement = kScreenElements.GetNext(kIter);
        pkScreenElement->Draw(m_spRenderer);
    }

    const NiTPointerList<NiScreenTexturePtr>& kScreenTextures =
        GetScreenTextures();
    kIter = kScreenTextures.GetHeadPos();
    while (kIter)
    {
        NiScreenTexture* pkScreenTexture = kScreenTextures.GetNext(kIter);
        NIASSERT(pkScreenTexture);
        pkScreenTexture->Draw(m_spRenderer);
    }
}
//---------------------------------------------------------------------------
void NiApplication::RenderVisualTrackers()
{
    const NiTPointerList<NiVisualTrackerPtr>& kVisualTrackers =
        GetVisualTrackers();
    NiTListIterator kIter = kVisualTrackers.GetHeadPos();
    while (kIter)
    {
        NiVisualTracker* pkVisualTracker = kVisualTrackers.GetNext(kIter);
        NIASSERT(pkVisualTracker);
        pkVisualTracker->Draw();
    }
}
//---------------------------------------------------------------------------
void NiApplication::EndFrame()
{
    if (!m_bUseFrameSystem)
    {
        m_spRenderer->EndFrame();
    }
}
//---------------------------------------------------------------------------
void NiApplication::DisplayFrame()
{
    OnDisplayFrame();

    if (m_bUseFrameSystem)
    {
        m_spFrame->Display();
    }
    else
    {
        m_spRenderer->DisplayFrame();
    }
}
//---------------------------------------------------------------------------
void NiApplication::OnDisplayFrame()
{
    // Take a screenshot if desired
    if (m_bDumpShotAtFixedInterval && (m_fDumpInterval != -NI_INFINITY) && 
        ((m_fLastDumpTime == -NI_INFINITY) || 
         (m_fLastDumpTime + m_fDumpInterval <= m_fAccumTime)))
    {
        SaveScreenshot();
        m_fLastDumpTime = m_fAccumTime;
    }
}
//---------------------------------------------------------------------------
void NiApplication::SaveScreenshot()
{
    // Pick the extension string based on the output format
    char* pcScreenshotSuffix = "";
    switch (ms_eScreenshotFormat)
    {
    case NiRenderer::FORMAT_TARGA:
        pcScreenshotSuffix = ".tga";
        break;
    case NiRenderer::FORMAT_PNG:
        pcScreenshotSuffix = ".png";
        break;
    case NiRenderer::FORMAT_JPEG:
        pcScreenshotSuffix = ".jpg";
        break;
    default:
        NIASSERT(false && "Unknown screenshot format!");
    }

    char acImageFilename[NI_MAX_PATH];
    GetScreenshotFilename(acImageFilename, NI_MAX_PATH, pcScreenshotSuffix);

    if (m_spRenderer->SaveScreenShot(acImageFilename, ms_eScreenshotFormat))
    {
        m_uiScreenShotId++;
    }
}
//---------------------------------------------------------------------------
void NiApplication::GetScreenshotFilename(char* pcBuffer,
    unsigned int pcBufferSize, const char* pcSuffix)
{
    NiSprintf(pcBuffer, pcBufferSize, 
#ifdef _XENON
        "D:\\Screenshots\\%s_%d%s",
#else
        "Screenshots/%s_%d%s",
#endif
        m_pkAppWindow->GetWindowCaption(), m_uiScreenShotId, pcSuffix);
}
//---------------------------------------------------------------------------
