#pragma once

typedef CsC_AvObject* (*CALLBACK_EventObject)( DWORD dwID );

class CEventSceneMng : public NiMemObject
{
public:
	CEventSceneMng();
	virtual ~CEventSceneMng();

public:	
	void	Delete();	
	void	ResetScene(); 
	void	Init();

	void	Update( float fDeltaTime );
	void	Render();		

	bool	InsertScene( UINT SceneNum ); /**< 실행되길 기다리는 이벤트씬 번호 */	
	void	DeleteScene( UINT SceneNum );
	void	EventCheckCondition( NiPoint2 ptPos, DWORD dwCurMapID );	
		
	bool	IsCameraScene() { if( m_bCameraScene ) assert_cs( m_pScene ); return m_bCameraScene; }  //현재 이벤트가 카메라줌 사용 이벤트인가??
	CEventMng* GetScene() { return m_pScene; }

	CsC_AvObject* GetEventObject( int nEventNum ); // 이벤트 번호로 해당 대상 찾기				

protected:		
	CsVectorPB< UINT >				m_vpSceneNum;	/**< 대기중인 이벤트씬 번호들 */		
	CEventMng*						m_pScene;		/**< 현재 플레이 되고있는 씬 */
	bool							m_bCameraScene; 

	// 콜백
public:
	CALLBACK_EventObject				m_CallBackEventObject;	
	void		ResistCallBack_EventObject( void* func ){ m_CallBackEventObject = (CALLBACK_EventObject)func; }		

public:
	//////////////////////////////////////////////////////////////////////////
	//
	// 툴에서만 쓰이는 함수들
	//
	//////////////////////////////////////////////////////////////////////////
	
	void	DirectScenePlay( UINT SceneNum ); //조건없이 이벤트씬 바로 실행함 툴에서만 사용함
	int		EventProgressCount(); //툴에서 이벤트 몇개도는지 볼라고 쓰는거	
};

extern CEventSceneMng* g_pEventSceneMng;