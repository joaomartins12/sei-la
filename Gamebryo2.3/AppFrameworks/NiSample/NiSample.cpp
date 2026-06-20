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
#include "NiSamplePCH.h"

#include "NiSample.h"

#include <NiApplicationMetrics.h>

#include <NiUIManager.h>
#include <NiSystemCursor.h>
#include <NiNavManager.h>
#include <NiNavFlyController.h>
#include <NiNavOrbitController.h>
#include <NiUIGroup.h>

#if defined(WIN32)
#include <NiDX9Renderer.h>
#include <NiD3D10Renderer.h>
#endif

//---------------------------------------------------------------------------
NiSample::NiSample(const char* pcWindowCaption, 
    unsigned int uiWidth, unsigned int uiHeight, bool bUseFrameSystem,
    unsigned int uiMenuID, unsigned int uiNumStatusPanes,
    unsigned int uiBitDepth) : NiApplication(pcWindowCaption, uiWidth,
        uiHeight, bUseFrameSystem, uiMenuID, uiNumStatusPanes, uiBitDepth),
    m_uiMaxOutputLogLines(3),
    m_fUIElementHeight(0.0f),
    m_fUIElementWidth(0.0f),
    m_kUINavHelpStart(0.0f, 0.0f),
    m_kUIElementGroupOffset(0.0f, 0.0f),
    m_uiNumLogChars(0),
    m_uiNavControllerLine(UINT_MAX),
    m_bShowNavHelp(false),
    m_kNavUpAxis(0.0f, 0.0f, 1.0f),
    m_fNavDefaultScale(300.0f),
    m_bUseNavSystem(true)
{
    m_kHideAll.Initialize(this, &NiSample::HideAllPressed);
    m_kToggleNavHelp.Initialize(this, &NiSample::ToggleNavHelp);
    m_kChangeController.Initialize(this, &NiSample::ChangeController);
    m_kQuit.Initialize(this, &NiSample::QuitApplication);

    // Initialize frame rendering system object names.
    m_kUIManagerRenderStepName = "NiSample UI Manager Render Step";
    m_kUIManagerRenderClickName = "NiSample UI Manager Render Click";
    m_kCursorRenderStepName = "NiSample Cursor Render Step";
    m_kCursorRenderClickName = "NiSample Cursor Render Click";

#if defined(WIN32)
    // Disable system cursor since we're using NiCursor.
    m_bExclusiveMouse = true;

    // Create the cursor render click.
    m_spCursorRenderClick = NiNew NiCursorRenderClick;
    m_spCursorRenderClick->SetName(m_kCursorRenderClickName);
#endif // WIN32
}
//---------------------------------------------------------------------------
bool NiSample::Initialize()
{
    // The ShadowManager needs to be initialized before the renderer is
    //created.
    if (m_bUseFrameSystem)
    {
        NiShadowManager::Initialize();
    }

    if (!CreateRenderer())
        return false;

    if (!CreateCamera())
        return false;

    if (!CreateShaderSystem())
        return false;

    if (!CreateInputSystem())
        return false;

    if (!CreateCursor())
        return false;

    NIMETRICS_APPLICATION_TIMER(NiMetricsClockTimer, kTimer, 
        CREATE_SCENE_TIME);
    NIMETRICS_APPLICATION_STARTTIMER(kTimer);
    if (!CreateScene())
    {
        NIMETRICS_APPLICATION_ENDTIMER(kTimer);
        return false;
    }
    NIMETRICS_APPLICATION_ENDTIMER(kTimer);

    if (m_bUseFrameSystem && !CreateFrame())
        return false;

    if (m_spScene)
    {
        m_spScene->Update(0.0f);
        m_spScene->UpdateProperties();
        m_spScene->UpdateEffects();
    }

    if (m_spCamera)
    {
        m_spCamera->Update(0.0f);
    }

    if (!CreateUISystem())
        return false;

    if (!CreateNavigationControllers())
        return false;

    if (!CreateUIElements())
        return false;

    if (!CompleteUISystem())
        return false;

    if (!CreateVisualTrackers())
        return false;

    NIMETRICS_APPLICATION_EVENT(INITIALIZED, 1.0f);

    return true;
}
//---------------------------------------------------------------------------
#if defined(WIN32)
MessageBoxFunction g_pfnGlobalFunction;

unsigned int NiSample::NiSampleMessageBoxFunc(const char* pcText, 
    const char* pcCaption, void* pvExtraData)
{
    bool bExclusiveMouse = NiApplication::ms_pkApplication->IsExclusiveMouse();
    if (bExclusiveMouse)
        ::ShowCursor(TRUE);

    unsigned int uiValue = g_pfnGlobalFunction(pcText, pcCaption, pvExtraData);
    
    if (bExclusiveMouse)
        ::ShowCursor(FALSE);
    return uiValue;
}
#endif
//---------------------------------------------------------------------------
bool NiSample::CreateInputSystem()
{
#if defined(WIN32)
    g_pfnGlobalFunction = NiMessageBox::GetMessageBoxFunction();
    NiMessageBox::SetMessageBoxFunction(&NiSampleMessageBoxFunc);
#endif

    return NiApplication::CreateInputSystem();
}
//---------------------------------------------------------------------------
// NiShader Functions
//---------------------------------------------------------------------------
bool NiSample::CreateShaderSystem()
{
    NiShaderFactory::RegisterErrorCallback(ShaderErrorCallback);

#if defined(WIN32)
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    assert(pkRenderer);
    const char* pcShaderProgramDir;
    if (NiIsKindOf(NiDX9Renderer, pkRenderer))
        pcShaderProgramDir = "Shaders/DX9";
    else if (NiIsKindOf(NiD3D10Renderer, pkRenderer))
        pcShaderProgramDir = "Shaders/D3D10";
    else
        pcShaderProgramDir = NULL;
    assert(pcShaderProgramDir);
#elif defined(_XENON)
    const char* pcShaderProgramDir = "Shaders\\Xenon";
#elif defined(_PS3)
    const char* pcShaderProgramDir = "Shaders/PS3";
#else
    #error "Unknown platform";
#endif

    const char* pcShaderDir = "Shaders";

    NiShaderFactory::AddShaderProgramFileDirectory(
        ConvertMediaFilename("Shaders/Common"));
    NiShaderFactory::AddShaderProgramFileDirectory(
        ConvertMediaFilename(pcShaderProgramDir));

    // First, we will run the parsers. This is done to make sure that
    // any text-based shader files that have been modified are re-compiled
    // to binary before loading all the binary representations.
    if (!RunShaderParsers(pcShaderDir))
    {
        NiMessageBox("Failed to run shader parsers!", "ERROR");
        return false;
    }

    if (!RunShaderLibraries(pcShaderDir))
    {
        NiMessageBox("Failed to load shader libraries!", "ERROR");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiSample::RunShaderParsers(const char* pcShaderDir)
{
#if !defined(_USRDLL)
    NiShaderFactory::RegisterRunParserCallback(RunParser);
#else
    // Use the default DLL creation callback in DLL builds
#endif

    if (!RegisterShaderParsers())
        return false;

    for (m_uiActiveCallbackIdx = 0; 
        m_uiActiveCallbackIdx < m_kParserLibraries.GetSize(); 
        m_uiActiveCallbackIdx++)
    {
        const char* pcLibName = NULL;
#if defined(_USRDLL)
        pcLibName = m_kParserLibraries.GetAt(m_uiActiveCallbackIdx);
#endif
        NiShaderFactory::LoadAndRunParserLibrary(pcLibName, 
            ConvertMediaFilename(pcShaderDir), true);
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiSample::RunShaderLibraries(const char* pcShaderDir)
{
#if !defined(_USRDLL)
    NiShaderFactory::RegisterClassCreationCallback(LibraryClassCreate);
#else
    // Use the default DLL creation callback in DLL builds
#endif

    if (!RegisterShaderLibraries())
        return false;

    unsigned int uiCount = 1;
    char* apcDirectories[1];
    apcDirectories[0] = (char*)ConvertMediaFilename(pcShaderDir);

    for (m_uiActiveCallbackIdx = 0; 
        m_uiActiveCallbackIdx < m_kShaderLibraries.GetSize(); 
        m_uiActiveCallbackIdx++)
    {
        const char* pcLibName = NULL;
#if defined(_USRDLL)
        pcLibName = m_kShaderLibraries.GetAt(m_uiActiveCallbackIdx);
#endif
        if (!NiShaderFactory::LoadAndRegisterShaderLibrary(pcLibName, uiCount, 
            apcDirectories, true))
        {
            NiMessageBox("Failed to load shader library!", "ERROR");
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
#if !defined(_USRDLL)
// Only defined in non-DLL builds
bool NiSample::LibraryClassCreate(const char* pcLibFile, 
    NiRenderer* pkRenderer, int iDirectoryCount, char* apcDirectories[], 
    bool bRecurseSubFolders, NiShaderLibrary** ppkLibrary)
{
    *ppkLibrary = NULL;

    NiSample* pkSample = (NiSample*)NiApplication::ms_pkApplication;

    if (pkSample->m_uiActiveCallbackIdx >= 
        pkSample->m_kShaderLibraries.GetSize())
    {
        return false;
    }

    NiShaderFactory::NISHADERLIBRARY_CLASSCREATIONCALLBACK pkCallback =
        pkSample->m_kShaderLibraries.GetAt(pkSample->m_uiActiveCallbackIdx);
    if (pkCallback)
    {
        if (!pkCallback(pkRenderer, iDirectoryCount, apcDirectories, 
            bRecurseSubFolders, ppkLibrary))
        {
            return false;
        }
    }

    return true;
}
#endif
//---------------------------------------------------------------------------
#if !defined(_USRDLL)
// Only defined in non-DLL builds
unsigned int NiSample::RunParser(const char* pcLibFile, 
    NiRenderer* pkRenderer, const char* pcDirectory, 
    bool bRecurseSubFolders)
{
    unsigned int uiCount = 0;
    NiSample* pkSample = (NiSample*)NiApplication::ms_pkApplication;
    if (pkSample->m_uiActiveCallbackIdx >= 
        pkSample->m_kParserLibraries.GetSize())
    {
        return false;
    }

    NiShaderFactory::NISHADERLIBRARY_RUNPARSERCALLBACK pkCallback =
        pkSample->m_kParserLibraries.GetAt(pkSample->m_uiActiveCallbackIdx);
    if (pkCallback)
    {
        uiCount += pkCallback(pcDirectory, bRecurseSubFolders);
    }

    return uiCount;
}
#endif
//---------------------------------------------------------------------------
unsigned int NiSample::ShaderErrorCallback(const char* pcError, 
    NiShaderError eError, bool bRecoverable)
{
    NiOutputDebugString("ERROR: ");
    NiOutputDebugString(pcError);
    return 0;
}
//---------------------------------------------------------------------------
bool NiSample::CreateCursor()
{
#if defined(WIN32)
    const NiRenderTargetGroup* pkRTGroup = m_spRenderer->
        GetDefaultRenderTargetGroup();
    NIASSERT(pkRTGroup);

    NiRect<int> kScreenBounds(0, pkRTGroup->GetWidth(0), 
        0, pkRTGroup->GetHeight(0));
    m_spCursor = NiSystemCursor::Create(m_spRenderer, kScreenBounds, 
        NiCursor::IMMEDIATE, NiSystemCursor::STANDARD, 
        "../../../../Data/SystemCursors.tga", pkRTGroup);
    if (m_spCursor == NULL)
        return false;

    unsigned int uiMouseStartX = 320;
    unsigned int uiMouseStartY = 240;
    POINT kPoint;
    if (GetCursorPos(&kPoint) == TRUE && 
        ScreenToClient(GetRenderWindowReference(), &kPoint) == TRUE)
    {
        uiMouseStartX = kPoint.x;
        uiMouseStartY = kPoint.y;
    }
    
    m_spCursor->SetPosition(0.0f, uiMouseStartX, uiMouseStartY);
    m_spCursor->Show(true);
    ShowCursor(FALSE);

    m_spCursorRenderClick->AppendCursor(m_spCursor);
#endif
    
    return true;
}
//---------------------------------------------------------------------------
#if defined(WIN32)
bool NiSample::OnDefault(NiEventRef pEventRecord)
{
    return NiApplication::OnDefault(pEventRecord);
}
#endif //#if defined(WIN32)
//---------------------------------------------------------------------------
bool NiSample::CreateUISystem()
{
    NiUIManager::Create();
    NiUIManager* pkUIManager = NiUIManager::GetUIManager();
    if (pkUIManager == NULL)
        return false;

    if (!pkUIManager->Initialize(
#if defined(_XENON)
        GetInputSystem(), "D:\\Data\\UISkinFull.dds", NULL))
#elif defined(WIN32)
        GetInputSystem(), "../../../../Data/UISkinFull.dds", m_spCursor))
#elif defined(_PS3)
        GetInputSystem(), "../../../../../Data/UISkinFull_ps3.dds", NULL))
#endif
    {
        return false;
    }

    m_fUIElementHeight = pkUIManager->GetMaxCharHeightInNSC() * 3.0f;
    m_fUIElementWidth = NiMin(0.40f, 
        pkUIManager->GetMaxCharWidthInNSC() * 25.0f);
    m_fUIGroupHeaderHeight = pkUIManager->GetMaxCharHeightInNSC() * 2.75f;
    m_kUIElementGroupOffset.x = pkUIManager->GetMaxCharWidthInNSC() * 1.5f;
    m_kUIElementGroupOffset.y = pkUIManager->GetMaxCharHeightInNSC() * 0.5f +
        m_fUIGroupHeaderHeight;

    if (m_bUseNavSystem)
    {
        if (!NiNavManager::Create())
            return false;
    }

    NiUIManager::GetUIManager()->ReserveGamePadButton(
        NiInputGamePad::NIGP_A, &m_kHideAll, NiUIManager::WASPRESSED);
    NiUIManager::GetUIManager()->ReserveKeyboardButton(
        NiInputKeyboard::KEY_Z, &m_kHideAll, NiUIManager::WASPRESSED);

    return true;
}
//---------------------------------------------------------------------------
bool NiSample::CreateNavigationControllers()
{
    if (m_bUseNavSystem)
    {
        if (!NiNavManager::GetNavManager())
            return false;

        // These need to be after create scene because there are occasions 
        // where the camera (m_spCamera) was not initialized before CreateScene
        m_spFlyController = NiNew NiNavFlyController(
            m_spCamera, m_spCamera, "\"Flying\" Mode", m_fNavDefaultScale,
            m_kNavUpAxis);
        m_spOrbitController = NiNew NiNavOrbitController(
            m_spCamera, "\"Orbiting\" Mode", m_fNavDefaultScale,
            m_kNavUpAxis);

        NiNavManager::GetNavManager()->AddNavigationController(
            m_spFlyController);
        NiNavManager::GetNavManager()->AddNavigationController(
            m_spOrbitController);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiSample::AdjustNavigationControllers(
    NiAVObject* pkControlledObject,
    NiAVObject* pkReferenceObject,
    const NiPoint3& kUpDir,
    float fScale)
{
    if (m_bUseNavSystem)
    {
        if (m_spFlyController != NULL)
        {
            m_spFlyController->SetUpDir(kUpDir);
            m_spFlyController->SetScale(fScale);
            m_spFlyController->SetControlledObject(pkControlledObject);
            m_spFlyController->SetReferenceObject(pkReferenceObject);
        }
        if (m_spOrbitController != NULL)
        {
            m_spOrbitController->SetRadius(fScale);
            m_spOrbitController->SetObject(pkControlledObject);
            m_spOrbitController->SetUpDir(kUpDir);
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiSample::CompleteUISystem()
{
    NiUIManager* pkUIManager = NiUIManager::GetUIManager();
    pkUIManager->PrepareRenderer();

    if (pkUIManager->GetUIGroupCount() > 1)
    {
        NiUIGroup* pkGroup = pkUIManager->SetSelectedUIGroup(1);
        // if the UI group exists, advance the focus to the first real element
        // if possible.
        if (pkGroup)
            pkGroup->AdvanceFocus();
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiSample::CreateFrame()
{
    if (!NiApplication::CreateFrame())
    {
        return false;
    }

    // Create UI manager render step.
    NiUIManagerRenderClick* pkUIManagerRenderClick =
        NiNew NiUIManagerRenderClick;
    pkUIManagerRenderClick->SetName(m_kUIManagerRenderClickName);
    NiDefaultClickRenderStep* pkUIManagerRenderStep = NiNew
        NiDefaultClickRenderStep;
    pkUIManagerRenderStep->SetName(m_kUIManagerRenderStepName);
    pkUIManagerRenderStep->AppendRenderClick(pkUIManagerRenderClick);

    // Append UI manager render step to frame.
    NIASSERT(m_spFrame);
    m_spFrame->AppendRenderStep(pkUIManagerRenderStep);

#if defined(WIN32)
    // Create cursor render step.
    NiDefaultClickRenderStep* pkCursorRenderStep = NiNew
        NiDefaultClickRenderStep;
    pkCursorRenderStep->SetName(m_kCursorRenderStepName);
    pkCursorRenderStep->AppendRenderClick(m_spCursorRenderClick);

    // Append cursor render step to frame.
    m_spFrame->AppendRenderStep(pkCursorRenderStep);
#endif  // #if defined(WIN32)

    return true;
}
//---------------------------------------------------------------------------
void NiSample::RenderFrame()
{
    if (m_bUseFrameSystem)
    {
        NiApplication::RenderFrame();
    }
    else
    {
        m_spRenderer->BeginUsingDefaultRenderTargetGroup(
            NiRenderer::CLEAR_ALL);
        CullFrame();

        BeginRender();
        NiDrawVisibleArray(m_spCamera, m_kVisible);

        RenderScreenItems();

        RenderUIElements();

        EndRender();

        RenderVisualTrackers();
        m_spRenderer->EndUsingRenderTargetGroup();
    }
}
//---------------------------------------------------------------------------
void NiSample::RenderUIElements()
{
    NiUIManager::GetUIManager()->Draw(m_spRenderer);

#if defined(WIN32)
    m_spCursor->Draw();
#endif
}
//---------------------------------------------------------------------------
void NiSample::Terminate()
{
    for (unsigned int ui = 0; ui < m_kLogLines.GetSize(); ui++)
        NiDelete m_kLogLines.GetAt(ui);
    m_kLogLines.RemoveAll();

    m_spOrbitController = NULL;
    m_spFlyController = NULL;
    m_spOutputLog = NULL;
    m_spFrameRateLabel = NULL;
    m_kNavHelpRenderGroups.RemoveAll();

#if defined(WIN32)
    m_spCursorRenderClick = NULL;
    m_spCursor = NULL;
    NiSystemCursor::Shutdown();
#endif // #if defined(WIN32)

    // The Navigation Manager must shut down before the UI Manager
    NiNavManager::Shutdown();
    NiUIManager::Shutdown();

    NiApplication::Terminate();
}
//---------------------------------------------------------------------------
void NiSample::UpdateFrame()
{
    NiApplication::UpdateFrame();

#if defined(WIN32)
    NiInputMouse* pkMouse = GetInputSystem()->GetMouse();
    if ((pkMouse != NULL) && (m_spCursor != NULL))
    {
        int iX, iY, iZ = 0;
        if (pkMouse->GetPositionDelta(iX, iY, iZ))
        {
            if ((iX != 0) || (iY != 0))
                m_spCursor->Move(0.0f, iX, iY);
        }
    }
#endif
}
//---------------------------------------------------------------------------
void NiSample::EndUpdate()
{
    // UI system updated after application updates scene,
    // allowing cameras to be placed correctly.
    if (NiUIManager::GetUIManager())
    {
        NiUIManager::GetUIManager()->UpdateUI();
        if (m_bUseNavSystem)
        {
            NiNavManager::GetNavManager()->Update();
            if (m_spCamera != NULL)
                m_spCamera->Update(m_fAccumTime);
        }
    }

    NiApplication::EndUpdate();

    if (m_spFrameRateLabel && (m_iClicks % 30 == 0))
    {
        char acString[32];
        NiSprintf(acString, 32, "Frame Rate: %.1f", m_fFrameRate);
        m_spFrameRateLabel->SetText(acString);
    }
}
//---------------------------------------------------------------------------
void NiSample::HideAllPressed(unsigned char ucState)
{
    NiUIManager* pkManager = NiUIManager::GetUIManager();
    if (pkManager)
    {
        pkManager->SetVisible(!pkManager->IsVisible());
    }
}
//---------------------------------------------------------------------------
bool NiSample::CreateUIElements()
{
    unsigned int uiWidth, uiHeight;
    NiRenderer::GetRenderer()->ConvertFromNDCToPixels(1.0f, 1.0f, 
        uiWidth, uiHeight);

    NiPoint2 kDimensions(0.0f, 0.0f);

    NiUIGroup* pkLogGroup = NiNew NiUIGroup("Output Log", 
        m_fUIGroupHeaderHeight, true);
    
    float fLogWidth = NiMin(0.45f,
        NiUIManager::GetUIManager()->GetMaxCharWidthInNSC() * 35.0f);
    float fCharHeight = NiUIManager::GetUIManager()->GetMaxCharHeightInNSC();
    float fFRHeight = fCharHeight;
    float fLogHeight = fFRHeight * (float)m_uiMaxOutputLogLines;
    kDimensions.x += fLogWidth + 2.0f * m_kUIElementGroupOffset.x;
    kDimensions.y += m_kUIElementGroupOffset.y;

    m_spFrameRateLabel = NiNew NiUILabel("", NiColorA::BLACK, NiColor::WHITE);
    m_spFrameRateLabel->SetOffset(m_kUIElementGroupOffset.x, kDimensions.y);
    m_spFrameRateLabel->SetDimensions(fLogWidth, fFRHeight);
    m_spFrameRateLabel->SetAlignment(NiUILabel::TOP, NiUILabel::LEFT);
    pkLogGroup->AddChild(m_spFrameRateLabel);
    kDimensions.y += fFRHeight + 0.5f * fCharHeight;

    m_spOutputLog = NiNew NiUILabel( "", NiColorA::BLACK, NiColor::WHITE);
    m_spOutputLog->SetOffset(m_kUIElementGroupOffset.x, kDimensions.y);
    m_spOutputLog->SetDimensions(fLogWidth, fLogHeight);
    m_spOutputLog->SetAlignment(NiUILabel::TOP, NiUILabel::LEFT);
    pkLogGroup->AddChild(m_spOutputLog);
    kDimensions.y += fLogHeight + fCharHeight;
    
    AddLogEntry("To hide the log and all UI elements, press \'Z\' on a "
        "keyboard or left analog stick press on a Gamepad.");

    pkLogGroup->SetOffset(0.0f, 0.0f);
    pkLogGroup->SetDimensions(kDimensions.x, kDimensions.y);
    pkLogGroup->UpdateRect();
    NiUIManager::GetUIManager()->AddUIGroup(pkLogGroup);

    m_kUINavHelpStart.x = 0.0f;
    m_kUINavHelpStart.y = kDimensions.y + fCharHeight;

    if (m_bUseNavSystem)
    {
        m_uiNavControllerLine = AddLogEntry("");

        NiNavBaseController* pkController = 
            NiNavManager::GetNavManager()->GetCurrentNavController();
        UpdateControllerLog(pkController);
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiSample::AddDefaultUIElements(NiUIGroup* pkGroup, 
    float& fLeftOffset, float& fTopOffset, float fElementWidth, 
    float fElementHeight)
{
    NIASSERT(pkGroup);

    NiUIButton* pkButton;
    if (m_bUseNavSystem)
    {
        if (NiNavManager::GetNavManager()->GetNumNavigationControllers() > 1)
        {
            pkButton = NiNew NiUIButton("Change Control Mode");
            pkButton->SetOffset(fLeftOffset, fTopOffset);
            pkButton->SetDimensions(fElementWidth, fElementHeight);
            pkButton->AddKeyboardHotkey(NiInputKeyboard::KEY_C);
            pkButton->AddGamePadHotkey(NiInputGamePad::NIGP_RLEFT);
            pkButton->SubscribeToPressEvent(&m_kChangeController);
            pkGroup->AddChild(pkButton);
            fTopOffset += fElementHeight;
        }
        pkButton = NiNew NiUIButton("Display Control Help");
        pkButton->SetOffset(fLeftOffset, fTopOffset);
        pkButton->SetDimensions(fElementWidth, fElementHeight);
        pkButton->AddKeyboardHotkey(NiInputKeyboard::KEY_V);
        pkButton->SubscribeToPressEvent(&m_kToggleNavHelp);
        pkGroup->AddChild(pkButton);
        fTopOffset += fElementHeight;
    }

    pkButton = NiNew NiUIButton("Quit");
    pkButton->SetOffset(fLeftOffset, fTopOffset);
    pkButton->SetDimensions(fElementWidth, fElementHeight);
    pkButton->AddKeyboardHotkey(NiInputKeyboard::KEY_ESCAPE);
    pkButton->AddGamePadHotkey(NiInputGamePad::NIGP_START, 
        NiInputGamePad::NIGP_MASK_SELECT);
    pkButton->SubscribeToPressEvent(&m_kQuit);
    pkGroup->AddChild(pkButton);
    fTopOffset += fElementHeight;

    AddNavigationHelpUIElements(m_kUINavHelpStart.x, m_kUINavHelpStart.y, 
        fElementWidth, fElementHeight, 0.1f, 0.4f);
}
//---------------------------------------------------------------------------
void NiSample::AddNavigationHelpUIElements(float fLeftOffset, float fTopOffset,
    float fElementWidth, float fElementHeight, float fImageWidth, 
    float fMinLabelWidth)
{
    if (NiUIManager::GetUIManager() == NULL)
        return;

    if (!m_bUseNavSystem)
        return;

    // Note that this code does not attempt to reserve the controls before
    // setting up the images.  This is because navigation controllers need
    // to be swappable.  If they can't swap around, then only one controller
    // could bind to a key or axis (WASD for instance).  However, it is left
    // up to the application programmer to ensure that the Ni*NavController
    // actually gets these controls (i.e. that they aren't bound by something
    // else.  If there are problems, the application programmer can always
    // set the Ni*NavController's bindings to something else.

    NiNavManager* pkNavManager = NiNavManager::GetNavManager();
    if (!pkNavManager)
        return;

    unsigned int uiNumNavControllers = 
        pkNavManager->GetNumNavigationControllers();
    unsigned int uiCurrentNavIdx = 
        pkNavManager->GetCurrentNavControllerIndex();
        
    for (unsigned int uiNavIdx = 0; uiNavIdx < uiNumNavControllers; uiNavIdx++)
    {
        const NiNavBaseController* pkNavController = 
            pkNavManager->GetNavControllerAt(uiNavIdx);
        
        if (!pkNavController)
            continue;

        NiUIRenderGroupPtr spRenderGroup = NiNew NiUIRenderGroup();

        unsigned int uiIdx;
        float fCurTop = 0.0f;
        float fRemainingWidth;
        float fWidth = fElementWidth;

        NiUILabelPtr spLabel;
        NiUIHotkeyImagePtr spHotkeyImg;
        unsigned int uiNumControls = pkNavController->GetControlCount();

        for (uiIdx = 0; uiIdx < uiNumControls; ++uiIdx)
        {
            if (!pkNavController->IsControlActive(uiIdx))
                continue;

            // First, we add the hotkey images where appropriate
            spHotkeyImg = NiNew NiUIHotkeyImage();

            const NiNavBaseController::InputBinding& kPosKey = 
                pkNavController->GetBinding(uiIdx, 
                NiNavBaseController::POSITIVE_AXIS);
            const NiNavBaseController::InputBinding& kNegKey = 
                pkNavController->GetBinding(uiIdx, 
                NiNavBaseController::NEGATIVE_AXIS);
            if (NiUIManager::GetUIManager()->KeyboardExists())
            {
                if (kPosKey.SupportsKeyboard())
                {
                    spHotkeyImg->SetKeyboardHotkey(kPosKey.eKeyboardKey,
                        kPosKey.eKeyboardModifier);
                }
                if (kNegKey.SupportsKeyboard())
                {
                    spHotkeyImg->SetKeyboardHotkey(kNegKey.eKeyboardKey,
                        kNegKey.eKeyboardModifier);
                }
            }
            if (NiUIManager::GetUIManager()->MouseExists())
            {
                // Note that the assert makes sure that the negative direction 
                // also uses a mouse axis and that they both use the same axis
                if (kPosKey.SupportsMouseAxis())
                {
                    NIASSERT(kNegKey.SupportsMouseAxis());
                    NIASSERT(kNegKey.eMouseAxis == kPosKey.eMouseAxis);

                    if (kPosKey.eMouseAxis == NiInputMouse::NIM_AXIS_Z)
                        spHotkeyImg->SetMouseHotkey(NiInputMouse::NIM_MIDDLE);
                    else
                        spHotkeyImg->SetMouseMotion();
                }
                else
                {
                    if (kPosKey.SupportsMouseButton())
                    {
                        spHotkeyImg->SetMouseHotkey(kPosKey.eMouseButton,
                            kPosKey.eMouseModifier);
                    }
                    if (kNegKey.SupportsMouseButton())
                    {
                        spHotkeyImg->SetMouseHotkey(kNegKey.eMouseButton,
                            kNegKey.eMouseModifier);
                    }
                }
            }
            if (NiUIManager::GetUIManager()->GamePadExists())
            {
                // Note that the assert makes sure that the negative direction
                // also uses the Gamebryo axis and that they use the same axis.
                if (kPosKey.SupportsGamePadAxis())
                {
                    NIASSERT(kNegKey.SupportsGamePadAxis());
                    NIASSERT(kNegKey.eGamePadAxis == kPosKey.eGamePadAxis);
                    spHotkeyImg->SetGamePadAxis(kPosKey.eGamePadAxis);
                }
                else
                {
                    if (kPosKey.SupportsGamePadButton())
                    {
                        spHotkeyImg->SetGamePadHotkey(kPosKey.eGamePadButton,
                            kPosKey.eGamePadModifier);
                    }
                    if (kNegKey.SupportsGamePadButton())
                    {
                        spHotkeyImg->SetGamePadHotkey(kNegKey.eGamePadButton,
                            kNegKey.eGamePadModifier);
                    }
                }
            }
            // Then we decide how much space is needed for the hotkey images
            fRemainingWidth = fWidth - (spHotkeyImg->GetTotalWidth() * 
                fImageWidth * fWidth);
            fRemainingWidth = 
                (fRemainingWidth < (fMinLabelWidth * fWidth)) ? 
                (fMinLabelWidth * fWidth) : fRemainingWidth;

            // Then we set the label and images where they belong
            spHotkeyImg->SetOffset(fRemainingWidth, 
                fCurTop);
            spHotkeyImg->SetDimensions(fWidth - fRemainingWidth,
                fElementHeight);

            spLabel = NiNew NiUILabel(pkNavController->GetControlName(uiIdx), 
                NiColorA(0.0f, 0.0f, 0.0f, 0.5f), NiColor::WHITE);
            spLabel->SetOffset(0.0f, fCurTop);
            spLabel->SetDimensions(fRemainingWidth, fElementHeight);
            spLabel->SetAlignment(NiUILabel::VERT_MIDDLE);
            fCurTop += fElementHeight;

            // Add them to the list and then decrement their smart pointer
            // count
            spRenderGroup->AddChild(spLabel);
            spRenderGroup->AddChild(spHotkeyImg);
            spHotkeyImg = NULL;
            spLabel = NULL;
        }

        // If there are no controls, add a simple label
        if (uiNumControls == 0)
        {
            spLabel = NiNew NiUILabel(
                "This navigation mode has no controls.", 
                NiColorA(0.0f, 0.0f, 0.0f, 0.5f), 
                NiColor::WHITE);
            spLabel->SetOffset(0.0f, fCurTop);
            spLabel->SetDimensions(fElementWidth, fElementHeight);
            spLabel->SetAlignment(NiUILabel::VERT_MIDDLE);
            fCurTop += fElementHeight;

            // Add them to the list and then decrement their smart pointer
            // count
            spRenderGroup->AddChild(spLabel);
            spLabel = NULL;
        }

        m_kNavHelpRenderGroups.SetAtGrow(uiNavIdx, spRenderGroup);
        NiUIManager::GetUIManager()->RegisterUIRenderGroup(spRenderGroup);
        spRenderGroup->SetOffset(fLeftOffset, fTopOffset);
        spRenderGroup->UpdateRect();
        spRenderGroup->InitializeScreenElements();

        if (uiNavIdx == uiCurrentNavIdx)
            spRenderGroup->SetVisible(m_bShowNavHelp);
        else
            spRenderGroup->SetVisible(false);
    }
}
//---------------------------------------------------------------------------
unsigned int NiSample::AddLogEntry(const char* pcNewLine)
{
    // This code adds the new line to my array
    unsigned int uiNewLineLen = strlen(pcNewLine);
    
    unsigned int uiRetVal = m_kLogLines.Add(NiNew NiString(pcNewLine));
    m_uiNumLogChars += uiNewLineLen + 2;
    ReinitializeLogText();
    
    return uiRetVal;
}
//---------------------------------------------------------------------------
bool NiSample::ChangeLogEntry(unsigned int uiLineNum, 
    const char* pcNewLine)
{
    if (uiLineNum >= m_kLogLines.GetSize())
        return false;

    unsigned int uiOldLineLen = 0;
    unsigned int uiNewLineLen = strlen(pcNewLine);
    
    NiString* pkLine = m_kLogLines.GetAt(uiLineNum);
    if (pkLine)
    {
        uiOldLineLen = pkLine->Length();
        *(pkLine) = pcNewLine;
    }
    m_uiNumLogChars += uiNewLineLen - uiOldLineLen;
    ReinitializeLogText();

    return true;
}
//---------------------------------------------------------------------------
void NiSample::ReinitializeLogText()
{
    char acLogText[1024];
    NIASSERT(m_uiNumLogChars <= 1024);

    char* pcLogText = acLogText;
    unsigned int uiSize, uiIdx, uiCurChar, uiCurLineLen;
    uiSize = m_kLogLines.GetSize();

    uiCurChar = 0;
    for (uiIdx = 0; uiIdx < uiSize; ++uiIdx)
    {
        NiString* pkLine = m_kLogLines.GetAt(uiIdx);
        if (!pkLine)
            continue;

        uiCurLineLen = pkLine->Length();
        const char* pcSrc = *(pkLine);
        
        memcpy(pcLogText + uiCurChar, pcSrc, uiCurLineLen);
        uiCurChar += uiCurLineLen;
        pcLogText[uiCurChar] = '\n';
        ++uiCurChar;
    }
    if (uiCurChar == 0)
        pcLogText[uiCurChar] = '\0';
    else
        pcLogText[uiCurChar - 1] = '\0';

    unsigned int uiNumLines = 0;
    for (unsigned int ui = 0; ui < uiCurChar; ui++)
    {
        if (pcLogText[ui] == '\n')
            uiNumLines++;
    }
    NIASSERT(uiNumLines <= m_uiMaxOutputLogLines);

    NIASSERT(m_spOutputLog != NULL);
    m_spOutputLog->SetText(pcLogText);
    NIASSERT(uiNumLines <= m_spOutputLog->GetVisualLineCount());
    NIASSERT(m_spOutputLog->GetVisualLineCount() <= m_uiMaxOutputLogLines);
}
//---------------------------------------------------------------------------
void NiSample::UpdateControllerLog(NiNavBaseController* pkNewController)
{
    if (pkNewController && UINT_MAX != m_uiNavControllerLine)
    {
        char acString[256];
        NiSprintf(acString, 256, "Navigation Controller Type: %s",
            (const char*) pkNewController->GetName());
        NIVERIFY(ChangeLogEntry(m_uiNavControllerLine, acString));
    }
}
//---------------------------------------------------------------------------
void NiSample::ToggleNavHelp()
{
    if (m_bUseNavSystem)
    {
        NiNavManager* pkManager = NiNavManager::GetNavManager();
        m_bShowNavHelp = !m_bShowNavHelp;

        if (pkManager)
        {
            unsigned int uiIdx = pkManager->GetCurrentNavControllerIndex();
            NiUIRenderGroup* pkGroup = m_kNavHelpRenderGroups.GetAt(uiIdx);
            if (pkGroup)
                pkGroup->SetVisible(m_bShowNavHelp);
        }
    }
}
//---------------------------------------------------------------------------
void NiSample::ChangeController()
{
    if (!m_bUseNavSystem)
        return;

    NiNavManager* pkManager = NiNavManager::GetNavManager();
    NIASSERT(pkManager != NULL);

    unsigned int uiOldIdx = pkManager->GetCurrentNavControllerIndex();
    pkManager->AdvanceController();
    NiNavBaseController* pkNewController =
        pkManager->GetCurrentNavController();
    unsigned int uiNewIdx = pkManager->GetCurrentNavControllerIndex();

    NiUIRenderGroup* pkOldGroup = m_kNavHelpRenderGroups.GetAt(uiOldIdx);
    if (pkOldGroup)
        pkOldGroup->SetVisible(false);

    NiUIRenderGroup* pkNewGroup = m_kNavHelpRenderGroups.GetAt(uiNewIdx);
    if (pkNewGroup)
        pkNewGroup->SetVisible(m_bShowNavHelp);

    UpdateControllerLog(pkNewController);
}
//---------------------------------------------------------------------------
