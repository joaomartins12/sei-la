#pragma once

class CEventMng : public NiMemObject
{
public:
	struct sEventData : public NiMemObject
	{
	public:
		sEventData(){ s_fCurTime = 0.0f; s_nProgress = 0; s_pTarget = NULL; s_pMoveProp = NULL; }		

	public:
		void	Delete();		
		void	Init( CEventData* pEvent ) { s_pEventData = pEvent; }
		bool	Update( float fDeltaTime );
		
		bool	PlayEvent( float fDeltaTime );		
		bool	ControlObject( float fDeltaTime );
		void	UpdateObject( float fDeltaTime );
				
		bool	IsLive( float fDeltaTime );
		void	SetTarget();		

		void	SetCurTime( float fCurTime ) { s_fCurTime = s_fCurTime; }	
		float	GetCurTime() { return s_fCurTime; }
		
		CEventData* 	GetEvent() { return s_pEventData; }	

	public:
		CsC_AvObject*				s_pTarget; /**< 이벤트 대상 */		

	protected:
		float						s_fCurTime;	/**< 이벤트 현재 시간 */
		int							s_nProgress;	/**< 이벤트 진행상황 */					
		CsMoveObject*				s_pMoveProp;

	protected:
		// 포인터만 존재 ( 데이터는 매니져에서 삭제해줌 )
		CEventData*					s_pEventData;	/**< 이벤트 데이터 */
	};

public:
	CEventMng();
	virtual ~CEventMng();

public:
	typedef std::list< sEventData* >					LIST_EVENT;
	typedef std::list< sEventData* >::iterator			LIST_EVENT_IT;

public:
	void	Delete();	
	void	Init( UINT SceneNum );

	bool    Update( float fDeltaTime );
	void	Render();	
	
	bool	IsEventData( CEventData* pEventData );
	UINT	GetSceneNum() { return m_SceneNum; }
	LIST_EVENT*		GetListEvent() { return &m_ListEventDataMng; }

protected:
	void	_InsertData( CEventData* pEvent );		
 
protected:		
	UINT							m_SceneNum; /**< 이벤트씬 번호 */
	LIST_EVENT						m_ListEventDataMng; /**< 현재 진행중인 이벤트들  */		

	//////////////////////////////////////////////////////////////////////////
	//
	//	카메라 씬일경우 필요변수
	//
	//////////////////////////////////////////////////////////////////////////
public:
	CsC_AvObject*	GetCameraTarget() { return m_pCameraTarget; }
	void			SetEventCameraTarget( int nCount );  // 카메라 타겟 정하기 오브젝트 저장 순서로.. 임시임..	

protected:
	//포인터만 존재
	CsC_AvObject*					m_pCameraTarget;	
};
