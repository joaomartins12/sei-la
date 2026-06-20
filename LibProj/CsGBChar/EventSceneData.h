#pragma once

class CEventSceneData : public NiMemObject
{
public:
	CEventSceneData(void);
	~CEventSceneData(void);

public:
	struct sEvent_Condition : public NiMemObject
	{
		DWORD		s_dwMapID;		/**< 이벤트씬이 작동하는 맵번호 */
		bool		s_bArea;		/**< 설정된 영역이 있는가? */
		bool		s_bOnlyArea;    /**< 영역만을 사용하는 이벤트인가? */
		bool		s_bLoop;     	/**< 반복형 이벤트씬인가? */
		NiPoint2	s_ptPos;		/**< 영역 좌표 */
		float		s_fRad;			/**< 영역 반경 */	
		bool		s_bUseCamera;	/**< 이벤트 카메라 사용 */
	};	

public:
	void	Delete();
	void	DeleteEvent( int nIdx );

	void	Init( UINT nSceneNum, sEvent_Condition* pEventCon );
	void	InsertData( CEventData* pEventData ) { m_vpEventData.PushBack( pEventData ); }
	
	UINT	GetSceneNum() { return m_SceneNum; }
	sEvent_Condition* GetCondition() { return &m_sEventCondition; }	
	int		GetEventSize() { return m_vpEventData.Size(); }
	CEventData* GetEventData( UINT EventNum ); //이벤트 번호로 얻기
	CEventData*	GetEventDatabyIdx( int nIdx );	//이벤트 인덱스로 얻기

protected:
	UINT							m_SceneNum;
	sEvent_Condition				m_sEventCondition;
	CsVectorPB< CEventData* >		m_vpEventData;
};
