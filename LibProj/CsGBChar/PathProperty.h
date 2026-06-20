
#pragma once 

#define PATH_IGNORE_DIST		20.0f

class CsC_PathProp : public CsC_Property
{
	THREAD_MEMPOOL_H( CsC_PathProp )

public:
	enum eMOVE_STATE
	{
		MS_RUN, MS_WALK,
	};
	enum eMOVE_TYPE
	{
		USE_KEY, USE_PATHENGINE,
	};

public:
	CsC_PathProp();
	~CsC_PathProp();

	virtual void			Delete();
	virtual void			PreDelete();
	void					Init( CsC_AvObject* pParent, float* pfMoveSpeed, bool bUserControl );
	virtual void			PreUpdate( float fDeltaTime );
	virtual void			CheckModelComp();

	void					SetPointer_MoveSpeed( float* pfMoveSpeed ){ m_pfMoveSpeed = pfMoveSpeed; }

protected:
	void					_PreUpdate_AbsolutePath( float fDeltaTime );

	//============================================================================================
	//	Status
	//============================================================================================
protected:
	float*				m_pfMoveSpeed;
	eMOVE_STATE			m_eMoveState;	

public:
	float				GetMoveSpeed();				// 상태 값을 전부 적용 시킨 스피드
	// 스테이트
	void				SetMoveState( eMOVE_STATE ms ){ m_eMoveState = ms; }
	eMOVE_STATE			GetMoveState(){ return m_eMoveState; }

	//============================================================================================
	//	Path
	//============================================================================================
protected:
	CsPath				m_Path;
	struct sABSOLUTEPATH 
	{
		sABSOLUTEPATH():s_bActive(false),s_fSpeed(0.0f)
		{}

		bool	s_bActive;
		float	s_fSpeed;
	};
	sABSOLUTEPATH		m_AbsolutePathInfo;
	bool				m_bAbsoluteRotation;

public:
	bool				IsMove(){ return ( m_Path.m_iPath != NULL ); }
	CsPath*				GetPath(){ return &m_Path; }

	void				AddAgent( float x, float y ){ g_pCsPathEngine->AddAgent( &m_Path, x, y ); }	
	bool				PathTest( float x, float y ){ return g_pCsPathEngine->PathTest( &m_Path, x, y ); }
	bool				PathTest( NiPoint2 vPos ){ return g_pCsPathEngine->PathTest( &m_Path, vPos.x, vPos.y ); }
	bool				PathTest( NiPoint3 vPos ){ return g_pCsPathEngine->PathTest( &m_Path, vPos.x, vPos.y ); }
	bool				PathTestLine( NiPoint3 vPos ){ return g_pCsPathEngine->PathTestLine( &m_Path, vPos.x, vPos.y ); }
	bool				PathTestLine( NiPoint2 vPos ){ return g_pCsPathEngine->PathTestLine( &m_Path, vPos.x, vPos.y ); }
	virtual bool		SetPath( float x, float y );
	bool				AbsolutePath( float x, float y, float fSpeedSec );

	virtual bool		SetPath( NiPoint2 vPos ){ return SetPath( vPos.x, vPos.y ); }
	virtual bool		SetPath( NiPoint3 vPos ){ return SetPath( vPos.x, vPos.y ); }

	void				DeletePath();

	void				ResetAgent();

	bool				CalTargetPath( NiPoint2& vReturnPos, NiPoint3 vTargetPos, float fConnetLength );

	//============================================================================================
	//	UserControl
	//============================================================================================
protected:
	bool				m_bUserControl;

	//============================================================================================
	//	Arrive Point
	//============================================================================================
protected:
	NiPoint2			m_vArrivePos;
	NiPoint2			m_vArriveDir;
	float				m_fMoveDist;
	float				m_fOrgTotalMoveDist;

public:
	float				GetOrgTotalMoveDist(){ assert_cs( m_bUserControl == true ); return m_fOrgTotalMoveDist; }
	float				GetMoveDist(){ assert_cs( m_bUserControl == true );  return m_fMoveDist; }
	NiPoint2			GetArrivePos(){ return m_vArrivePos; }
	NiPoint2			GetArriveDir(){ return m_vArriveDir; }
};

#include "PathProperty.inl"