#pragma once

class CEventCall;
class cAxisRotation;
class CEventScene : public NiMemObject
{
public:
	CEventScene(void);
	~CEventScene(void);

public:
	static void	CALLBACK_EventSound( char* szFileName ); 

public:
	void	Delete();	
	void	Init();

	void	Update();
	void	Render();
	
	void	EventSetComplete( CEventData* pEvent );	

	void    CreateEvent();
	void	SetEventInit( CEventData::sEvent_Info pEventInfo, CEventData::sEventObject_Info pObjectInfo );
	void	SetEvent( CEventData::sBase_Info* sEventInfo );	
	
	void	ResetFreeCamera( bool bResetPos = true );
	void	ResetCharCamera();	

	void	ReSetPlayTime() { m_fPlayTime = 0.0f; m_nProgress = 0; }
	void	SetDeltaTime( float fTime ) { m_fPlayTime = fTime; }

	bool	IsTempEvent() { return ( m_pEventData == NULL ) ? false : true; }

	//==============================================================================
	//
	//		캐릭터 카메라
	//
	//==============================================================================
public:
	void			SetCameraTargetPos( NiPoint3 vPos );
	CsC_AvObject*	GetCameraTarget() { return m_pCameraTarget; }		
	void			DeleteCameraTarget();

protected:
	CsC_AvObject*	m_pCameraTarget;


	//==============================================================================
	//
	//		이벤트 플레이
	//
	//==============================================================================
public:	
	bool			PlayEvent();
	void			EndEvent();

	void			EventDelete();	
	void			DeleteMoveData();
	void			DeleteEventTarget();

	void			SetEventTarget();
	CsC_AvObject*	GetEventTarget() { return m_pEventTarget; }		

protected:		
	CEventData*		m_pEventData;	//이벤트 임시저장
	CsC_AvObject*	m_pEventTarget;	 // 포인터만 존재
	CsMoveObject*	m_pMoveProp;

	CEventCall*		m_pEventCall;


	float	m_fPlayTime;
	float	m_fEndTime;
	int		m_nProgress;

	//==============================================================================
	//
	//		이벤트 임시 저장
	//
	//==============================================================================
public:
	typedef std::map< UINT, CEventData* >				MAP_EvntData;
	typedef std::map< UINT, CEventData* >::iterator		MAP_EvntData_IT;
		
	void	DeleteTempEventAll();
	void	DeleteTempEvent( UINT EventNum ); 	
	void	SaveTempEventData();	
	CEventData* GetTempEventData( UINT EventNum );

protected:
	void	_InsertTempEvent( DWORD dwMapID );	
	void	_LoadTempEventALL();
	void	_LoadTempEventData( TCHAR* szFilename );

public:	
	std::map< DWORD, MAP_EvntData >	m_mapTempData;	

	//==============================================================================
	//
	//		렌더러
	//
	//==============================================================================
public:
	D3DXMATRIX			m_matView;
	D3DXMATRIX			m_matProj;
	D3DXMATRIX			m_matViewProj;
	D3DVIEWPORT9		m_ViewPort;

public:
	D3DXMATRIX*			GetD3DView(){ return &m_matView; }
	D3DXMATRIX*			GetD3DProj(){ return &m_matProj; }
	D3DXMATRIX*			GetD3DViewProj(){ return &m_matViewProj; }
	D3DVIEWPORT9*		GetD3DViewPort(){ return &m_ViewPort; }
	NiPoint3			Cal3D_To_2D( NiPoint3 vPos );

	//=====================================================================================	
	//	무브 포인트	
	//=====================================================================================
protected:
	CsNodeObj		m_MovePoint;
	bool			m_bMovePointRender;
	NiPoint3		m_vMovePoint;
protected:
	void			_CreateMovePoint();
	void			_DeleteMovePoint();
	void			_RenderMovePoint();
	void			_RenderPlayAreaPoint();
public:
	void			SetMovePoint( NiPoint3 vMovePoint ); 
	void			ReleaseMovePoint(){ m_bMovePointRender = false; }

	//==============================================================================
	//
	//		폰트
	//
	//==============================================================================
public:
	CFreeFont			m_FontSystem;
	CFreeFont			m_FontText;
	LPD3DXFONT			m_pD3DXFont;

public:
	bool			CreateFont();
	void			DeleteFont();

	//=====================================================================================	
	//	영역 표시
	//=====================================================================================
protected:
	CsNodeObj		m_AreaPoint;
	bool			m_bAreaPointRender;
	NiPoint3		m_vAreaPoint;
	NiPoint3		m_vPlayAreaPoint;
	float			m_fPlayAreaRad;
protected:
	void			_CreateAreaPoint();
	void			_DeleteAreaPoint();
	void			_RenderAreaPoint();
public:
	void			SetAreaPoint( NiPoint3 vAreaPoint ); 
	void			SetPlayAreaPoint( NiPoint3 vAreaPoint, float fRad );
	void			ReleaseAreaPoint(){ m_bAreaPointRender = false; }
	NiPoint2		GetAreaPoint(){ return ( NiPoint2( m_vAreaPoint.x, m_vAreaPoint.y ) ); }

	//=====================================================================================	
	//	이펙트영역 표시
	//=====================================================================================
public:
	CsNodeObj		m_pAxisNiNode;	
	CsNodeObj		m_pAxisPlaneNiNode;
	NiPoint3		m_vEffectPoint;	
	
protected:
	void			_CreateEffectPoint();
	void			_DeleteEffectPoint();
	void			_RenderEffectPoint();

};

extern CEventScene g_EventScene;