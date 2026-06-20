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

#ifndef NISCENEVIEWERWINDOW_H
#define NISCENEVIEWERWINDOW_H

#include "NiSceneChangeInfo.h"
#include "NiSceneEventHandler.h"
//#include "NiSceneViewer.h"

//#include "NiUIMap.h"
//#include <NiUniversalTypes.h>
//#include <NiAnimation.h>
//#include <NiCollision.h>
//#include <NiMain.h>
//#include <NiPortal.h>
//#include <NiSystem.h>
//#include "NiSceneKeyboard.h"
//#include "NiSceneMouse.h"

class NiSceneViewerWindow : public NiRefObject
{
public:
        
    virtual ~NiSceneViewerWindow();
    bool Initialize(NiScenePtr spScene,
        NiSceneChangeInfo* pkChangeInfo = NULL);
    bool Initialize(const char* pcFilename, 
        NiSceneChangeInfo* pkChangeInfo = NULL);
    bool ReinitializeCameras();

    void CloseScene();
    bool Terminate();
    virtual void OnIdle() = 0;
    bool SetUI(NiUIMap* ui);
    NiUIMap* GetUI();
    
    // Get input devices
    NiSceneKeyboard* GetKeyboard() const;
    NiSceneMouse* GetMouse() const;
    
    // window painting
    virtual bool OnWindowDraw(NiContextRef pkDC) = 0;
    virtual bool OnWindowErase(NiContextRef pkDC) = 0;

    // window maintenance
    virtual bool OnWindowCreate(NiCreateStructRef pkCreateStructRef) = 0;
    virtual bool OnWindowResize(int iWidth, int iHeight, 
        unsigned int uiSizeType, NiWindowRef pkWnd) = 0;
    virtual bool OnWindowDestroy(NiWindowRef pkWnd, bool bOption) = 0;
    virtual bool OnWindowMove(int iXPos, int iYPos, NiWindowRef pkWnd) = 0;

    // Events

    // command processing
    bool OnCommand(int iParam1, int iParam2, NiWindowRef pkCtl);

    // input/system processing (returns true to continue, false to exit)
    virtual bool Process(long* plRetVal) = 0;

    // default processing
    virtual bool OnDefault(NiEventRef pkEventRecord) = 0;

    static void SetAcceleratorReference(NiAcceleratorRef pkAccel);
    static NiAcceleratorRef GetAcceleratorReference();
    static void SetInstanceReference(NiInstanceRef pkInstance);
    static NiInstanceRef GetInstanceReference();
    
    void PreMainLoop();
    long MainLoopTick();
    void PostMainLoop();

    // window size
    void SetWidth(unsigned int uiWidth);
    unsigned int GetWidth() const;
    void SetHeight(unsigned int uiHeight);
    unsigned int GetHeight() const;

    void SetParentWidth(unsigned int uiWidth);
    unsigned int GetParentWidth() const;
    void SetParentHeight(unsigned int uiHeight);
    unsigned int GetParentHeight() const;

    void SetWindowReference(NiWindowRef pkWnd);
    NiWindowRef GetWindowReference();
    void SetRenderWindowReference(NiWindowRef pkWnd);
    NiWindowRef GetRenderWindowReference();
    
    void SetFullscreen(bool bOn);
    bool GetFullscreen() const;
    void SetHardware(bool bOn);
    bool GetHardware() const;
    void SetStencil(bool bOn);
    bool GetStencil() const;
    void SetMultitexture(bool bOn);
    bool GetMultitexture() const;
    void SetBackgroundColor(NiColor kColor);
    NiColor GetBackgroundColor();

    void SetD3D10(bool bD3D10);
    bool GetD3D10() const;

    void SetAnimationLooping(bool bLoop);
    bool GetAnimationLooping();

    void SetAnimationLoopEndTime(float fEndTime);
    float GetAnimationLoopEndTime();
    void SetAnimationLoopStartTime(float fStartTime);
    float GetAnimationLoopStartTime();

    void SetFrameRateLimit(unsigned int uiCeiling);
    unsigned int GetFrameRateLimit();

    void SetDefaultCamera(NiNodePtr spCamNode);
    
    NiSceneViewer* GetSceneViewer();

    void CalcCameraFrustum(NiCamera* pkCamera) const;
    void CalcInitialCameraTransform(NiCamera* pkCamera) const;

protected:

    bool CreateScene(NiScenePtr spScene);
    bool CreateCamera(NiScenePtr spScene);
    bool SceneHasLights(NiNode* pkScene);

    virtual bool CreateRenderer()=0;
    float m_fElapsedTime;
    float m_fStartTime;
    float m_fLastTime;

    NiCameraPtr m_spCamera;
    NiNodePtr m_spDefaultCamNode;
    NiNode* m_spScene;
    NiRendererPtr m_spRenderer;
    NiScenePtr m_spTheSceneDatabase;

    NiSceneViewer* m_pkSceneViewer;
    NiSceneEventHandler* m_pkSceneEventHandler;
    NiSceneChangeInfo* m_pkSceneChangeInfo;
    bool m_bFullscreen;
    bool m_bHardware;
    bool m_bStencil;
    bool m_bMultitexture;

    bool m_bD3D10;

    unsigned int m_uiBitDepth;
    NiColor m_kBackground;
    static NiAcceleratorRef ms_pkAccel;
    static NiInstanceRef ms_pkInstance;

    unsigned int m_uiWidth;
    unsigned int m_uiParentWidth;
    unsigned int m_uiHeight;
    unsigned int m_uiParentHeight;

    NiWindowRef m_pkWnd;
    NiWindowRef m_pkRenderWnd;
    NiSceneKeyboard* m_pkKeyboard;
    NiSceneMouse* m_pkMouse;

    bool m_bIsAlive;
    bool m_bInitShadowManager;
};

#include "NiSceneViewerWindow.inl"
#endif //NISCENEVIEWERWINDOW_H
