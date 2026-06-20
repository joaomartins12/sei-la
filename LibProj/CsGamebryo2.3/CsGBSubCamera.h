////////////////////////////////
//2015-11-26-nova
//메인카메라에서 필요기능만 분리
////////////////////////////////
#pragma once

struct sCAMERAINFO : public NiMemObject
{
	sCAMERAINFO()
	{
		s_fDist = 1000.0f;
		s_fFOV = 45.0f;
		s_fNearPlane = 1.0f;
		s_fFarPlane = 200000.0f;		
		s_ptInitPos = NiPoint3::ZERO;
		s_fInitRoll = 0.0f;
		s_fInitPitch = -NI_HALF_PI;
	}
	float		s_fDist;
	float		s_fNearPlane;
	float		s_fFarPlane;
	float		s_fFOV;
	NiPoint3	s_ptInitPos;
	float		s_fInitRoll;
	float		s_fInitPitch;
};


class CsGBSubCamera : public NiMemObject
{
public:
	CsGBSubCamera();
	~CsGBSubCamera();

protected:
	// Pointer
	NiCameraPtr			m_pCameraObj;
	NiCameraPtr			m_pCameraObjBackup;
	NiNodePtr			m_pTargetObj;

	void quatRotationArc( NiQuaternion* p, NiPoint3 v0, NiPoint3 v1);

	// ==== Create
public:
	void				Create( NiDX9Renderer* pRenderer, sCAMERAINFO* ci = NULL );
	void				Reset( sCAMERAINFO* ci = NULL );	

	// ==== Reset
protected:	
	void				_ResetControl( bool bAccelReset = true );

	// ==== Update
protected:
	float				m_fElapsedTime;

public:
	void				_UpdateCamera();

	// ==== Info
protected:
	sCAMERAINFO			m_CameraInfo;

public:
	sCAMERAINFO*		GetCameraInfo(){ return &m_CameraInfo; }

	// Rotation
public:
	float				m_fCurPitch;
	float				m_fCurRoll;
	float				m_fRotationLimitMin;
	float				m_fRotationLimitMax;
	bool				m_bRotationLimit;

	void				SetRotation( NiMatrix3 matRotation );
	void				SetRotation( float fRoll, float fPich );
	void				SetRotation( NiPoint3 ptDir );

	// Translate
protected:
	float				m_fTargetDeltaHeight;

public:
	void				SetTranslate( NiPoint3 pos );
	void				SetDist( float fDist, bool bReleaseRemainDist = false );
	void				SetDeltaDist( float fDeltaDist );

	// ==== Frustum
protected:
	float				m_fAspectRatio;

	NiFrustum			m_Frustum;
	NiFrustum			m_OrthoFrustum;
	NiFrustumPlanes		m_FrustumPlanes;

public:
	void				ChangeStandardFov( float fWidth, float fHeight );
	void				ApplyFrustumPlane( bool bOther );
	void				SetOrthoFrustum( float fWidth, float fHeight );
	bool				UserCulling( NiAVObject* pkObject );

	void				ReleaseDistRange(){ m_bDistRange = false; }

	// ==== Control Camera
	// Dist
protected:
 	float				m_fCurDist;
 	float				m_fDestDist;

	float				m_fDistMin;
	float				m_fDistMax;

	bool				m_bDistRange;

	// Get
public:
	inline NiCamera*	GetCameraObj(){ return m_pCameraObj; }
	inline NiPoint3		GetWorldRightVector(){ return m_pCameraObj->GetWorldRightVector(); }
	inline void			SetDeltaHeight( float fDeltaHeight ){ m_fTargetDeltaHeight = fDeltaHeight; }
	void				ChangeCameraObj( NiCamera* pObj ){ m_pCameraObjBackup = m_pCameraObj; m_pCameraObj = pObj; }

};

