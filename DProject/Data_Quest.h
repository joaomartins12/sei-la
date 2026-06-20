
#pragma once 

class cData_Quest : public NiMemObject
{
public:
	static bool	CheckPreRequire( CsQuest* pFT );

	//===========================================================================
	//
	//		퀘스트 진행 구조체
	//
	//===========================================================================
public:
	struct sPROCESS : public NiMemObject
	{
		CSGBMEMPOOL_H( cData_Quest::sPROCESS );
		sPROCESS(){ s_pFT = NULL; s_bCompleate = false; }
		void	Delete(){ s_CurCompRequireGroup.Delete(); s_pFT = NULL; s_bCompleate = false; }
		CsQuest*				s_pFT;
		CsQuestCompRequireGroup	s_CurCompRequireGroup;
		bool					s_bCompleate;
	};

	//===========================================================================
	//
	//		전역
	//
	//===========================================================================
public:
	static void		GlobalInit();
	static void		GlobalShotDown();
	static bool		CALLBACK_QuestCheck( int nQuest );

	//===========================================================================
	//
	//		베이스
	//
	//===========================================================================
public:
	void		Delete();
	void		Init(){}

	void		InitNetOff();
	void		PostRevPostLoad( DWORD dwMapID );
	void		PostRevAddQuest( CsQuest* pQuest );

	void		QuestReLoad();
	//===========================================================================
	//
	//		상태
	//
	//===========================================================================
protected:
	std::map< DWORD, sPROCESS* >	m_mapProcQuest;						// 퀘스트 수행 정보
	nQuest::cInfo					m_Info;								// 퀘스트 전체 정보

public:
	nQuest::cInfo*	GetInfo(){ return &m_Info; }
	int				GetInfoArrayIndex( DWORD dwFTID );

	sPROCESS*		GetProcess( DWORD dwFTID );
	bool			IsProcess( DWORD dwFTID ) const;
	bool			IsCompleate( DWORD dwFTID );
	bool			IsNoAccept( DWORD dwFTID );// 아직 받은 지은 퀘스트 체크
	void			UpdateQuestCondition( DWORD const& dwQuestID, int const& nDepth, int const& nValue );

	//int				GetQuestIDX() const;
	//std::string		GetTutorial_TooltipImage_Path(int nMainIndex=0, int nSubIndex=0) const;

	//===========================================================================
	//
	//		퀘스트 시작
	//
	//===========================================================================
public:
	void				RevQuestCheck();

	bool				RevQuest_ByNpc( CsQuest* pFT, CsC_AvObject* pNpc );
	bool				RevQuest_ByDigivice( CsQuest* pFT );
	
protected:
	bool				_RevQuest_ApplyRequite( CsQuest* pFT );								// 퀘스트 시작 지급 아이템

	//===========================================================================
	//
	//		Process
	//
	//===========================================================================
protected:	
	void				_DeleteProcQuest();

public:
	sPROCESS*			InsertProcess( CsQuest* pFT );
	bool				CalProcess(int nCheckType = -1);
	TCHAR*				NpcTalk( DWORD dwNpcFTID );
	DWORD				UseItem( CsItem* pFTUseItem );
	void				CheckRegion( DWORD const & dwMapId, int const& dwRegionID );

protected:
	//int					_CheckProcessCondition( sPROCESS* pProcess );				// 퀘스트별 갯수 체크
	int					_CheckProcessCondition( sPROCESS* pProcess, int nCheckType = -1);// 퀘스트별 갯수 체크
	CsC_AvObject*		_GetOwner( CsQuest* pFT );

	//===========================================================================
	//
	//		퀘스트 포기
	//
	//===========================================================================
public:
	void				DropProcAllQuest();
	void				DropProcQuest();
	void				DropProcQuest( DWORD nQuestID );

	//===========================================================================
	//
	//		퀘스트 완료
	//
	//===========================================================================
protected:
	void				_CompReadyQuest( bool bCompleate, CsQuest* pQuestFT );
	void				_NewBiQuestCheck( CsQuest* pQuestFT );

	void				_CompQuest_PostCheck( CsQuest* pQuestFT );
public:
	bool				CompQuest( CsQuest* pQuestFT, CsC_AvObject* pNpc );
	bool				IsJointProgressQuest( DWORD dwQuestID );

	//===========================================================================
	//
	//		부가
	//
	//===========================================================================
public:
	NiColor				GetTitleColor( CsQuest* pQuestFT );
	//int					GetQuestExp( CsQuest* pQuestFT, int nExp );
	void				ResetDailyQuest();

	bool				IsProcessQuestAllComplete( DWORD const& dwQuestID );
};