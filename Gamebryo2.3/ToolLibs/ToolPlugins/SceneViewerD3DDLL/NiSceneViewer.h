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

#ifndef NISCENEVIEWER_H
#define NISCENEVIEWER_H

#define IS_USER_CAM     "SCENEVIEWER_USERCAM"
#define FRUSTUM_LEFT    "FRUSTUM_LEFT"
#define FRUSTUM_RIGHT   "FRUSTUM_RIGHT"
#define FRUSTUM_TOP     "FRUSTUM_TOP"
#define FRUSTUM_BOTTOM  "FRUSTUM_BOTTOM"
#define FRUSTUM_NEAR    "FRUSTUM_NEAR"
#define FRUSTUM_FAR     "FRUSTUM_FAR"

#include "NiViewerCamera.h"
#include "NiScreenConsole.h"
#include "NiScene.h"
#include <NiScreenElements.h>
#include <NiCriticalSection.h>
#include "NiUIMap.h"

class NiSceneCommand;

class NiSceneViewer : public NiMemObject
{
public:
    NiSceneViewer(NiRenderer* renderer);
    ~NiSceneViewer();

    NiRenderer* GetRenderer();

    // Scene-graph manipulation methods
    bool SetScene(NiNodePtr scene);// Pre-made scene
    bool SetScene(char* pcFilename); // Open the file and read in the scene
    NiNode* GetScene();            // Get the scene minus nodes 
                                    // created as a convenience 
                                    // to the viewer
    NiNode* GetMasterScene();
    float GetWorldRadius();
    void SetWorldRadius(float f);

    bool SetSceneManager(NiScene* pkScene);
    bool SetMasterScene(NiNode* pkMasterScene);

    void SetBackgroundColor(NiColor kColor);

    // Camera manipulation methods
    bool SelectCamera(unsigned int uiWhichCamera);
    bool AddCamera(NiCamera* pkCamera);
    NiCamera* GetCamera(unsigned int uiWhichCamera, bool bReadonly = true);
    NiMatrix3& GetCameraInitialRotation(unsigned int uiWhichCamera);
    NiPoint3&  GetCameraInitialLocation(unsigned int uiWhichCamera);     
    NiViewerCamera* GetCurrentCameraOrientation();
    void SetCurrentCameraOrientation(NiViewerCamera* pkCameraOrientation);

    unsigned int GetCurrentCameraID();
    unsigned int GetCameraCount();
    void CollectCameras(NiNode* pkScene);
    void EmptyCameraList();
    void GetCameraList(NiTPrimitiveArray<NiCamera*>& kCameras);
    
    // Main sequence methods
    void Begin();
    void MainLoop();
    void End();
    void UpdateStats();
    void SubmitCommands(NiSceneCommandQueue* pkCommands);

    // Animation manipulation methods
    void SetAnimationMode(bool bIsAnimated);// Turn on/off animation
    bool GetAnimationMode();                // Are we currently animating?
    void ResetAnimations();                 // Reset the clock
    void SetAnimationSpeed(float fSpeed);   // Set animation rate
    float GetAnimationSpeed();              // Get current animation rate
    void SetExternalTimeMode(bool bIsAnimatedExternally);
    void SetCurrentTime(float fCurrentTime);
    float GetCurrentTime();

    // Metric-related manipulation methods
    void EnableTextureStats(bool bOn);   // Gather text info on textures?
    void EnableGeometryStats(bool bOn);  // Gather text info on geometry?
    void EnableFrameRateStats(bool bOn); // Gather text info on framerate?
    void EnableCameraConsoleInfo(bool bOn);//Show name of the current cam?
    void EnableAllStats(bool bOn);
    bool IsTextureStatsEnabled();       
    bool IsGeometryStatsEnabled();
    bool IsFrameRateStatsEnabled();
    bool IsAllStatsEnabled();
    bool IsCameraConsoleInfoEnabled();

    // Screen Console manipulation methods
    void EnableScreenConsole(bool bOn);
    bool IsScreenConsoleEnabled();
    void SetConsoleDimensions(int iWidth, int iHeight); 
    void SetConsoleLocation(int iX, int iY);
    NiScreenConsole* GetScreenConsole();
    
    void SetLoopEndTime(float fEndTime);
    float GetLoopEndTime();
    void SetLoopStartTime(float fStartTime);
    float GetLoopStartTime();
    void SetAnimationLooping(bool bLoop);
    bool GetAnimationLooping();

    void SetCameraClickCeiling(unsigned int uiCeiling);
    unsigned int GetCameraClickCeiling();

    enum LockTarget 
    {
        CAMERA_LOCK, 
        COMMAND_LOCK,
        RENDERER_LOCK,
        SCENE_LOCK,
        NUM_LOCK_TARGETS
    };

    bool isUnlocked(LockTarget eTarget);
    bool Lock(LockTarget eTarget);
    bool Unlock(LockTarget eTarget);

    static void SetupCameraExtraData(bool bIsUserCam, NiCamera* pkCamera);

    // frame rendering
    void CreateFrame();
    Ni3DRenderViewPtr m_spMainRenderView;
    NiRenderFramePtr m_spFrame;
    NiFixedString m_kShadowRenderStepName;
    NiDefaultClickRenderStepPtr m_spShadowRenderStep;
    
    void Precache(NiAVObject* pkObject);
    static bool ShadowRenderStepPre(NiRenderStep* pkCurrentStep, 
        void* pvCallbackData);

protected:

    // Internal class for holding camera information
    class Camera : public NiMemObject
    {
    public:
        NiCameraPtr m_spCamera;
        NiMatrix3 m_kInitialRot;
        NiPoint3 m_kInitialPos;
        NiViewerCamera* m_pkCameraOrientation;
    };

    void AttachSceneToMasterRoot(NiNode* pkScene);
    unsigned int m_uiNumCameraClicks;
    unsigned int m_uiCameraClickCeiling;
    bool m_bAnimationLooping;
    float m_fAnimLoopEndTime;
    float m_fAnimLoopStartTime;

    NiSceneCommandQueue* m_pkCommandQueue;
    NiTPrimitiveArray<Camera*>* m_pkCameras;
    NiCameraPtr m_spCamera;
    NiVisibleArray m_kVisible;
    NiScene* m_pkSceneDB;
    unsigned int m_uiInsertionIndex;

    NiScreenConsolePtr m_spScreenConsole;
    int m_iCameraIndex;
    NiNode* m_pkScene;
    NiNodePtr m_pkMasterScene;
    float m_fWorldRadius;

    NiRendererPtr m_spRenderer;

    NiColor m_kBackgroundColor;
    
    bool  m_bAnimationMode;
    bool  m_bAnimateExternally;
    float m_fAnimCurrentTime;
    float m_fAnimationSpeed;
    float m_fAnimLastTime;
    float m_fAnimAccumTime;

    bool m_bGeometryStats;
    bool m_bTextureStats;
    bool m_bFrameRateStats;
    bool m_bCameraConsoleInfo;
    bool m_bForceConsoleRedraw;
    float m_fFrameRate;
    float m_fLastSampleTime;
    float m_fAccumTime;
    float m_fCurrentTime;

    unsigned int m_uiCameraConsoleInfoIndex;
    unsigned int m_uiFrameRateStatsIndex;
    unsigned int m_uiGeometryStatsIndex;
    unsigned int m_uiTextureStatsIndex;

    void PrintOutSceneGraph(NiNode* pkScene);
    
    void HandleSceneCommands();

    void RunUpScene();
    float m_fMinStartTime;

private:
    bool m_abLocks[NUM_LOCK_TARGETS];
    NiCriticalSection m_akCritSecs[NUM_LOCK_TARGETS];

// support for overdraw metering
public:
    void AttachScreenElements(NiScreenElements* pkElements);
    void DetachScreenElements(NiScreenElements* pkElements);
private:
    NiScreenElementsArray m_kScreenElements;
};

#endif //NISCENEVIEWER_H
