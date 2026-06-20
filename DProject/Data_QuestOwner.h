
#pragma once 


class cData_QuestOwner : public NiMemObject
{
public:
	void		Delete();
	void		Init();
	void		LoadMap();

	void		Update();

	void		AddQuest_New( nQuest::cInfo* pOrgInfo, CsQuest* pFT );
	void		AddQuest_Process( nQuest::cInfo* pOrgInfo, CsQuest* pFT, cData_Quest::sPROCESS* pProcess );

	void		RevQuestCheck( nQuest::cInfo* pOrgInfo );

	bool		IF_DigiviceQuest_Exist();		// 디지바이스 퀘 정보 있는지 체크
	void		IF_DigiviceQuest_Open();		// 디지바이스 IF 오픈

public:
	// 엔피씨당 가진 퀘스트 리스트
	struct sNpcOwner : public NiMemObject
	{
	public:
		// 엔피씨 한테 받는 타입 ( 순서 바뀌면 안됨. 엔피씨 디스플레이 타입이 결정됨 )
		enum eREV_TYPE{
			TALK					=	0x00000001,			//	대화해야하는 퀘를 가지고 있다.
			REQUITE					=	0x00000002,			//	보상 받아야 함
			ENABLE_REV_MAIN			=	0x00000010,			//	메인퀘 수행가능
			ENABLE_REV_SUB			=	0x00000020,			//	서브퀘 수행가능
			ENABLE_REV_EVENTREPEAT	=	0x00000040,			//	반복퀘 수행가능
			ENABLE_REV_REPEAT		=	0x00000080,			//	반복퀘 수행가능
			PROCESS					=	0x00000100,			//	진행중			
			SOON_REV_LEVEL			=	0x00001000,			//	곧 수행가능	- 레벨

			_DISPLAY_LIMIT			=	0x00010000,			// 여까이만 디스플레이됨
			
			DISABLE_REV				=	0x01000000,			//	수행불가	- 레벨 or 퀘스트 or Item

			_DISPLAY_NONE			=	0x08000000,

			// 디스플레이와 무관하게 항상 보여야 할 마크
			TRAIN					=	0x10000000,			// 포탈 엔피씨
			NORMAL					=	0x10000001,			// 포탈 엔피씨
		};

		struct sQuestInfo : public NiMemObject
		{
			sQuestInfo(){ s_eType = _DISPLAY_NONE; s_pFTQuest = NULL; s_pProcess = NULL; }
			eREV_TYPE				s_eType;
			CsQuest*				s_pFTQuest;
			cData_Quest::sPROCESS*	s_pProcess;
		};

		void		Delete();
		void		Init();
		void		Add_New( nQuest::cInfo* pOrgInfo, CsQuest* pFT );
		void		Add_Process( nQuest::cInfo* pOrgInfo, CsQuest* pFT, cData_Quest::sPROCESS* pProcess );		

		void		DeleteQuest( uint nQuestFTID );

		void		CalDisplayRev();
		bool		CheckRevType( nQuest::cInfo* pOrgInfo ); 
		bool		IsHaveType( DWORD nRevPlag );												// 해당 타입의 퀘스트가 있는지 체크

		bool		IsDisplayRev(){ return ( s_eDisplayRevType < _DISPLAY_LIMIT ); }			// 마크 랜더링 되는가
		eREV_TYPE	GetDisplayRev(){ return s_eDisplayRevType; }								// 랜더링할 마크

		void		GetQuestInfoList( DWORD nRevPlag, std::list< sQuestInfo* >* pList );		// 해당 타입의 퀘스트 목록

	protected:
		// 선행조건 체크
		eREV_TYPE	_CheckPreRequire( nQuest::cInfo* pOrgInfo, CsQuest* pFT );
	protected:
		eREV_TYPE						s_eDisplayRevType;
		std::map< DWORD, sQuestInfo* >	s_mapQuest;
	};

	// 디지바이스 퀘스트 리스트
	struct sDigiviceOwner : public NiMemObject
	{
		struct sQuestInfo : public NiMemObject
		{
			sQuestInfo(){ s_pFTQuest = NULL; s_pProcess = NULL; }
			CsQuest*				s_pFTQuest;
			cData_Quest::sPROCESS*	s_pProcess;
		};		

		void		Delete();
		void		DeleteQuest( CsQuest* pFT );
		void		Init( bool bUseDigiviceIF );
		void		Add_New( nQuest::cInfo* pOrgInfo, CsQuest* pFT, cData_Quest::sPROCESS* pProcess );
		void		Add_Pointer( sDigiviceOwner::sQuestInfo* pQuestInfo );
		
		std::map< DWORD, sQuestInfo* >*	GetMap(){ return &s_mapQuest; }

	protected:
		bool							s_bUseDigiviceIF;
		std::map< DWORD, sQuestInfo* >	s_mapQuest;
	};

	typedef std::map<DWORD, sNpcOwner*>		QuestOwnerCont;

protected:
	// npcID, 퀘스트 정보
	QuestOwnerCont					m_mapNpcOwner;
	

	// 디지바이스	
	sDigiviceOwner					m_ProcDigiviceOwner;		// 진행 중인 디지바이스 퀘스트
	sDigiviceOwner					m_ReadyRevDigiviceOwner;	// 받을 수 있는 디지바이스 퀘스트 목록
	sDigiviceOwner					m_CheckDigiviceOwner;		// 테이머 레벨은 되나 수행 조건을 만족하지 못해 받을 수 없는 디지바이스 퀘스트 목록
	sDigiviceOwner					m_HighLevelDigiviceOwner;	// 테이머 레벨이 부족해 받을 수 없는 디지바이스 퀘스트 목록


public:
	// 체크 해야 하는 퀘스트 목록
	sDigiviceOwner*		GetCheckDigiviceOwner(){ return &m_CheckDigiviceOwner; }
	sDigiviceOwner*		GetReadyRevDigiviceOwner(){ return &m_ReadyRevDigiviceOwner; }
	sDigiviceOwner*		GetProcDigiviceOwner(){ return &m_ProcDigiviceOwner; }


protected:
	sNpcOwner*			_GetNpcOwner( DWORD const& dwNpcFTID );

public:
	void				Check_DigimonTalk( bool const& bPostLoad );
};