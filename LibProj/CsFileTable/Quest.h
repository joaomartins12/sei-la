

#pragma once 


class CsQuest
{
public:
	typedef	std::map< DWORD, CsQuest* >				MAP;
	typedef	std::map< DWORD, CsQuest* >::iterator	MAP_IT;

public:
	CsQuest();

	// ==== Base
public:
	void				Delete();
	void				Init();
	void				InitText();

	// ==== Uniq ID
protected:
	DWORD				m_dwUniqID;
public:
	DWORD				GetUniqID() const { return m_dwUniqID; }
	void				SetUniqID( DWORD dwUniqID ){ m_dwUniqID = dwUniqID; }

	// ==== Model ID
protected:
	DWORD				m_dwModelID_Recv;
	DWORD				m_dwModelID_Comp;
public:
	DWORD				GetModelID_Recv(){ return m_dwModelID_Recv; }
	void				SetModelID_Recv( DWORD dwID ){ m_dwModelID_Recv = dwID; }
	DWORD				GetModelID_Comp(){ return m_dwModelID_Comp; }
	void				SetModelID_Comp( DWORD dwID ){ m_dwModelID_Comp = dwID; }

	// ==== Level
protected:
	USHORT				m_nLevel;
public:
	USHORT				GetLevel(){ return m_nLevel; }
	void				SetLevel( SHORT nLevel ){ m_nLevel = nLevel; }

	// ==== ToolPos
protected:
	CsPoint				m_ptToolPos;
public:
	CsPoint				GetToolPos(){ return m_ptToolPos; }
	void				SetToolPos( CsPoint pos ){ m_ptToolPos = pos; }

	// ==== ManageID
protected:
	DWORD				m_dwManageID;
public:
	DWORD				GetManageID(){ return m_dwManageID; }
	void				SetManageID( DWORD dwMngID ){ m_dwManageID = dwMngID; }

	// ==== Enable
protected:
	bool				m_bActiveQuest;
public:
	bool				IsActive(){ return m_bActiveQuest; }
	void				SetActive( bool bActive ){ m_bActiveQuest = bActive; }

	// ==== 즉시 퀘스트 타입 - 수락과 완료과 동시에
protected:
	bool				m_bImmediate;
public:
	bool				IsImmediate(){ return m_bImmediate; }
	void				SetImmeiate( bool bImmediate ){ m_bImmediate = bImmediate; }

	// ==== 퀘스트를 초기화를 하는가?
#ifdef 	QUEST_RESET_180712
protected:
	bool				m_bResetQuest;
public:
	bool				GetIsResetQuest(){return m_bResetQuest;}
	void				SetResetQuest(bool TOF){m_bResetQuest = TOF;}
#endif

	// ==== type
public:
	enum eQUEST_TYPE{ QT_SUB = 0, QT_MAIN, QT_DAILY, QT_REPEAT, QT_EVENTREPEAT, QT_ACHIEVE,	
		QT_JOINTPROGRESS, 
		QT_TUTORIAL,
		QT_XANTI_JOINTPROGRESS,
//		QT_TIME,
	};
protected:
	eQUEST_TYPE			m_eQuestType;	
public:
	eQUEST_TYPE			GetQuestType(){ return m_eQuestType; }
	void				SetQuestType( eQUEST_TYPE type ){ m_eQuestType = type; }
	bool				IsQuestType( eQUEST_TYPE const& type ) const;

	// ==== Comp Target
public:
	enum eQUEST_TARGET{ QTG_SELF = 0, QTG_NPC, QTG_COMPLETE };
protected:
	eQUEST_TARGET		m_eQuestTargetType;
	int					m_nQuestTargetValue1;
public:
	eQUEST_TARGET		GetQuestTarget(){ return m_eQuestTargetType; }
	void				SetQuestTarget( eQUEST_TARGET target ){ m_eQuestTargetType = target; }
	int					GetQuestTargetValue1(){ return m_nQuestTargetValue1; }
	void				SetQuestTargetValue1( int nValue ){ m_nQuestTargetValue1 = nValue; }

	// ==== Text
public:
	TCHAR				m_szTitleTab[ QUEST_TITLE_LEN ];
	TCHAR				m_szTitleText[ QUEST_TITLE_LEN ];
	TCHAR				m_szBodyText[ QUEST_BODY_LEN ];
	TCHAR				m_szSimpleText[ QUEST_SIMPLE_LEN ];
	TCHAR				m_szHelperText[ QUEST_HELPER_LEN ];
	TCHAR				m_szProcess[ QUEST_PROCESS_LEN ];
	TCHAR				m_szCompleate[ QUEST_COMPLEATE_LEN ];
	TCHAR				m_szExpert[ QUEST_PROCESS_LEN ];

	// ==== StartTarget
public:
	enum eST_TYPE{ ST_NPC, ST_DIGIVICE, };
protected:	
	eST_TYPE			m_eStartTarget_Type;
	DWORD				m_dwStartTarget_ID;
public:
	void				SetStartTarget_Type( eST_TYPE type ){ m_eStartTarget_Type = type; }
	void				SetStartTarget_ID( DWORD dwID ){ m_dwStartTarget_ID = dwID; }
	eST_TYPE			GetStartTarget_Type(){ return m_eStartTarget_Type; }
	DWORD				GetStartTarget_ID(){ return m_dwStartTarget_ID; }

	// ==== ApplyRequite
protected:
	CsQuestApplyRequiteGroup	m_ApplyRequiteGroup;
public:
	void						AddApplyRequite( CsQuestApplyRequite::eTYPE type, UINT nTypeID, int nTypeCount );
	void						DeleteApplyRequite( int nIndex );
	CsQuestApplyRequiteGroup*	GetApplyRequiteGroup(){ return &m_ApplyRequiteGroup; }

	// ==== PreRequire
protected:
	CsQuestPreRequireGroup	m_PreRequireGroup;
public:
	void					AddPreRequire( CsQuestPreRequire::eTYPE type, UINT nTypeID, int nTypeCount );
	void					DeletePreRequire( int nIndex );
	CsQuestPreRequireGroup*	GetPreRequireGroup(){ return &m_PreRequireGroup; }

	// ==== Comp Require
protected:
	CsQuestCompRequireGroup		m_CompRequireGroup;
public:
	void						AddCompRequire( CsQuestCompRequire::eTYPE type, UINT nTypeID, int nTypeCount, UINT nTargetID, int nSubValue1, int nSubValue2 );
	void						DeleteCompRequire( int nIndex );
	CsQuestCompRequireGroup*	GetCompRequireGroup(){ return &m_CompRequireGroup; }

	// ==== Requite
protected:
	CsQuestRequiteGroup			m_RequiteGroup;
public:
	void						AddRequite( CsQuestRequite::eMETHOD method, CsQuestRequite::eTYPE type, UINT nTypeID, int nTypeCount );
	void						DeleteRequite( int nIndex );
	CsQuestRequiteGroup*		GetRequiteGroup(){ return &m_RequiteGroup; }

	// ==== Event
public:
	enum eEVENT{ EVENT_QUEST_START, EVENT_QUEST_PROC, EVENT_QUEST_COMPLEAT, EVENT_QUEST_REQUITE, EVENT_MAX_COUNT, };
	int					m_nEvent[ EVENT_MAX_COUNT ];

	// ==== Clone
public:
	CsQuest*			Clone();
	void				CopyData( CsQuest* pSrcData, bool bIncludeTypeID = false );
};





