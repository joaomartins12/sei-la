#pragma once

#include "CsGBAttachCamera.h"
#include "CsGBSubCamera.h"
#include "NiOcclusionCuller.h"

#include "..\..\DProject\Singleton.h"

typedef void(*CALLBACK_KeyAutoMapping)(void);

//2015-12-02-nova
enum eSubCamera
{
	CAMERA_01,	// 그림자							CsGBShadow
	CAMERA_02,	// 캐릭터							cTamerStatus
	CAMERA_03,	// 디지몬							cTamerStatus
	CAMERA_04,	// 캐쉬샵							cCashShop_Char
	CAMERA_05,	// <용병 디지몬 부화> 인큐베이터	cMaOkeTactics
	CAMERA_06,	// 용병보관소						cTacticsHouseO
	CAMERA_07,	// 테이머->디지바이스				cTamerStatus::_RenderDigivice
	CAMERA_08,	// 크로스인벤						cTamerStatus::_RenderCrossInven()
	CAMERA_09,	// 									CCrossEvent::PostRender()
	CAMERA_10,	// NPC 카메라 줌					CNpcMng::RenderZoomNpc()
	CAMERA_11,	// 인큐베이터 -> 용병생성 이벤트신 / 스피릿 디지몬 부화 연출 cTacticsAni
	CAMERA_12,	// 튜토리얼							CEventScene
	CAMERA_13,	// 보스몬스터						cBossScene
	CAMERA_14,	// 악세사리 변경 옵션 이펙트		cAccessory
	CAMERA_CNT
};

class CsGBCamera : public CSingleton< CsGBCamera >
{
public:
	CsGBCamera();
	~CsGBCamera();

protected:
	// Pointer
	NiCameraPtr				m_pCameraObj;
	NiCameraPtr				m_pCameraObjBackup;
	NiNodePtr				m_pTargetObj;
	NiOcclusionCuller*		m_spCuller;
	bool					m_bOcclusionCullingUse;

	// ==== Delete
public:
	void				Delete();
	// ==== Create
public:
	void				Create( NiDX9Renderer* pRenderer, sCAMERAINFO* ci = NULL );
	void				Reset( sCAMERAINFO* ci = NULL );	
	void				FindOcclusionGeometry( NiNode* pSceneNode );
	void				AddOcclusionGeometry(NiNode* pSceneNode);
	void				RestOcclusionGeometry();
	void				SetUseOcclusionCulling( bool bValue );
	void				SetShowOccluder( bool bValue );

	// ==== Reset
protected:	
	void				_ResetControl( bool bAccelReset = true );

	// ==== Update
protected:
	bool				m_bAttachActor_ByTool;
	float				m_fElapsedTime;
public:
	void				Update( float fElapsedTime );
	void				Update_ByTool( float fElapsedTime, bool bUpdateTargetDist_ByTool = true );

	void				SetAttachActor_ByTool( bool bAttach );
protected:
	void				_CalCollision();

	void				_UpdateDistAni( float fElapsedTime );
	void				_UpdateDistAni_ByTool( float fElapsedTime );
	//void				_UpdateStaticCamera();

	//void				_UpdateTerrainCollision();
	void				_UpdateTerrainCollision_ByTool();
	void				_UpdateObjectPick( float fElapsedTime );
	void				_UpdateTargetDist_ByTool();

	void				_UpdateAutoDirection( float fElapsedTime );

public:
	inline void			_UpdateCamera();

	// ==== Info
protected:
	sCAMERAINFO			m_CameraInfo;
	sCAMERAINFO			m_BackupCameraInfo;

	float				m_fBackupTargetDeltaHeight;
	bool				m_bBackupRotationLimit;
	bool				m_bBackupDistRange;
	float				m_fBackupAspectRatio;

public:
	sCAMERAINFO*		GetCameraInfo(){ return &m_CameraInfo; }
	void				SaveCameraState();
	void				LoadCameraState();

	// ==== Frustum
protected:
	NiFrustum			m_Frustum;
	NiFrustum			m_OrthoFrustum;
	float				m_fAspectRatio;
	float				m_fBackupFar;

	NiFrustumPlanes		m_FrustumPlanes;
public:	
	void				SetStandardFrustum();
	void				SetFarFrustumPlane( float fFar );		// 업데이트 - 컬링시 사용
	void				SetDefaultFarFrustumPlane();			// 업데이트 - 컬링시 사용
	bool				SetFarFrustum( float fFar );			// 렌더 - 랜더링시 사용
	bool				SetDefaultFarFrustum();					// 랜더 - 랜더링시 사용
	void				ChangeStandardFov( float fWidth, float fHeight, float fFOV );
	void				ChangeStandardFov( float fWidth, float fHeight );
	void				ChangeStandardFov( float fFOV );
	void				SetOrthoFrustum( float fHeightSize );
	void				SetOrthoFrustum( float fWidth, float fHeight );
	float				GetAspectRatio(){ return m_fAspectRatio; }
	NiFrustum			GetFrustum(){ return m_pCameraObj->GetViewFrustum(); }
	NiFrustumPlanes*	GetFrustumPlane(){ return &m_FrustumPlanes; }

	void				ApplyFrustumPlane( bool bOther );

	bool				UserCulling( NiAVObject* pkObject );
	/*void				SetUserFrustum_UserCliping( float fFarPlane );
	void				SetDefaultFrustum_UserCliping();*/

	void				SetCameraDist( float fDistRange );

	void				SetUsingTerrainCollition( bool bValue );
	// ==== Fog
public:
	inline float		FogStartDistToFogDepth( float fRealDist );
	inline float		FogEndDistToMaxFogFactor( float fRealDist, float fDepth );

	// ==== Capsule  오브젝트와의 충돌 바운드 박스
protected:
	NiCapsuleBV*		m_pBoundCapsule;
	NiNodePtr			m_pBoundNode;
protected:
	void				_DeleteCollision();
	void				_CreateCollision();
	
	// ==== Control Camera
	// Dist
protected:
	float				m_fDistMin;
	float				m_fDistMax;
	bool				m_bDistRange;
	float				m_fDistAccel;

	float				m_fCurDist;
	float				m_fRemainDist;
	bool				m_bDestDistInit;
	float				m_fDestDist;
	float				m_fDistAniAccel;
	float				m_fDAAConstant;

	bool				m_bTerrainCollision;
	float				m_fTerrainCollitionDist;
	bool				m_bUsingTerrainCollision;
	float				m_fBeforeCollisionDist;

	// Static Camera
protected:
	bool				m_bStaticCamera;
	int					m_nDistLevel;

public:
	void				SetStaticCamera( bool bStatic ){ m_bStaticCamera = bStatic; }
	int					GetSetDistLevel(){ return m_nDistLevel; }


public:
	void				SetDAAConstant( float fConstant ){ m_fDAAConstant = fConstant; }
	void				SetDistAccel( float fAccel ){ m_fDistAccel = fAccel; m_fDistAniAccel = fAccel*m_fDAAConstant; }
	void				SetDistRange( float fMin, float fMax );
	void				SetDistRange(){ m_bDistRange = true; }
	float				GetDistRange_Min(){ return m_fDistMin; }
	float				GetDistRange_Max(){ return m_fDistMax; }
	void				ReleaseDistRange(){ m_bDistRange = false; }
	void				ReleaseRemainDist() { m_fRemainDist = 0.0f; }
	void				SetDist( float fDist, bool bReleaseRemainDist = false );
	void				SetDeltaDist( float fDeltaDist );
	void				SetDirectDist( float zDelta );

	float				GetDist(){ return m_fCurDist; }
	float				GetDistLevel( int nLevel );
	void				SetDistLevel( int nLevel, bool bSetDist );

	// Translate
protected:
	float				m_fTranslateAccel;
	float				m_fTargetDeltaHeight;
protected:
	void				_SetDeltaTranslate_OnCamera( float fCross, float fUp );
public:
	void				SetTranslateAccel( float fAccel ){ m_fTranslateAccel = fAccel; }		
	void				SetTranslate( NiPoint3 pos );
	void				SetDeltaTranslate( NiPoint3 pos );
	NiPoint3			GetViewerPos(){ return m_pTargetObj->GetWorldTranslate(); }
	inline void			SetDeltaHeight( float fDeltaHeight ){ m_fTargetDeltaHeight = fDeltaHeight; }
	float				GetDeltaHeight(){ return m_fTargetDeltaHeight; }
	inline NiPoint3		GetWorldTranslate(){ return m_pCameraObj->GetWorldTranslate(); }
	inline NiTransform	GetWorldTransform(){ return m_pCameraObj->GetWorldTransform(); }

	// Rotation
protected:
	float				m_fRotationAccel;
	float				m_fCurPitch;
	float				m_fCurRoll;
	float				m_fRotationLimitMin;
	float				m_fRotationLimitMax;
	bool				m_bRotationLimit;


public:

	void				SetRotationAccel( float fAccel ){ m_fRotationAccel = fAccel; }
	void				SetRotationLimit( float fMin, float fMax );
	void				ReleaseRotationLimit(){ m_bRotationLimit = false; }
	void				SetRotation( NiMatrix3 matRotation );
	void				SetRotation( float fRoll, float fPich );
	void				SetRotation( NiPoint3 ptDir );
	void				SetDeltaRotation( float fRoll, float fPich );

	float				GetRotationLimitMax(){ return m_fRotationLimitMax; }
	float				GetRotationLimitMin(){ return m_fRotationLimitMin; }

	void				SetDirection_FormDirectionLight( NiDirectionalLight* pDirectionLight );

	// Get
public:
	bool				IsAttachCamera(){ return ( m_pAttachCamera != NULL )&&( m_pAttachCamera->GetCameraObj() != NULL ); }
	NiCamera*			GetCameraObj();		//2016-03-29-nova
	inline NiCamera*	GetCameraObj_Org(){ return m_pCameraObj; }				

	inline NiNode*		GetTargetObj(){ return m_pTargetObj; }
	inline NiPoint3		GetWorldLocation(){ return m_pCameraObj->GetWorldLocation(); }
	inline NiPoint3		GetWorldDirection(){ return m_pCameraObj->GetWorldDirection(); }
	inline NiPoint3		GetWorldRightVector(){ return m_pCameraObj->GetWorldRightVector(); }
	inline NiPoint3		GetWorldUpVector(){ return m_pCameraObj->GetWorldUpVector(); }

	float				GetCurPitch(){ return m_fCurPitch; }
	float				GetCurRoll(){ return m_fCurRoll; }

	// Backup
public:
	void				ChangeCameraObj( NiCamera* pObj );
	void				RollBackCameraObj();

	// ==== Ani Rotation
protected:
	struct sANI_ROTAION
	{
		enum eSETTING{ NONE, REMAIN_TIME, ACTIVE, };

		void				Release(){ s_eSetting = NONE; }
		void				Set( float fActiveTime, float fRot ){ s_eSetting = REMAIN_TIME; s_fActiveTime = fActiveTime; s_fAniRotation = fRot; }
		eSETTING			GetSetting(){ return s_eSetting; }

		eSETTING			s_eSetting;
		float				s_fActiveTime;
		float				s_fAniRotation;
	};
	sANI_ROTAION		m_CurAniRoll;
	float				m_fAutoRotSpeed;

public:
	void				SetAniRotation_Roll( float fActiveTime, float fRot, float fSpeed );
	void				ReleaseAniRotation_Roll(){ m_CurAniRoll.Release(); }

	// ==== Attach Camera
protected:
	CsGBAttachCamera*	m_pAttachCamera;
public:
	void				AttachCamera( char* cName, bool bOwner );
	void				DeleteDetachCamera();

	// ==== Sub Camera
protected:
	CsGBSubCamera*	mSubCamera[CAMERA_CNT];							//2015-11-26-nova

public:
	CsGBSubCamera*	getSubCamera(int num);
	
	// ==== MouseInput
protected:
	// Mouse
	bool				m_bLBtnDown;
	POINT				m_ptLBtnDown;
	bool				m_bMBtnDown;
	POINT				m_ptMBtnDown;
	bool				m_bRBtnDown;
	POINT				m_ptRBtnDown;
public:
	void				OnLButtonDown( POINT pt );
	void				OnLButtonUp( POINT pt );
	void				OnMButtonDown( POINT pt );
	void				OnMButtonUp( POINT pt );
	void				OnRButtonDown( POINT pt );
	void				OnRButtonUp( POINT pt );
	void				OnMouseMove( POINT pt );
	void				OnMouseMove( POINT pt, float fAccel );
	void				OnMouseWheel( short zDelta );
	inline bool			WindowPointToRay( int nWindowX, int nWindowY, NiPoint3& ptOrigin, NiPoint3& ptDir );
public:
	void				OnRButtonPressed( int x, int y );

protected:
	void				_SetRotationOnMouse( POINT dest );
	void				_SetTranslateOnMouse( POINT dest );
	void				_SetTranslateOnMouse( POINT dest, float fAccel );



	// 콜백
protected:
	CALLBACK_KeyAutoMapping		m_CallBackKeyAutoMapping;
public:
	void		ResistCallBack_KeyAutoMapping( void* func ){ m_CallBackKeyAutoMapping = (CALLBACK_KeyAutoMapping)func; }
};

#include "CsGBCamera.inl"

//---------------------------------------------------------------------------
// Singleton	//2015-12-02-nova
#define CAMERA_ST			CsGBCamera::GetSingleton()			
#define CAMERA_STPTR		CsGBCamera::GetSingletonPtr()		
#define GET_SUBCAMERA(x)	CAMERA_ST.getSubCamera(x)
//---------------------------------------------------------------------------
