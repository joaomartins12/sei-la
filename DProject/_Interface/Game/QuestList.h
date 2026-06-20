

#pragma once 

#define IF_QUESTLIST_DELTA_Y		4

#define	IF_QL_TAB_UNEXPAND			1
#define	IF_QL_TAB_EXPAND			2
#define IF_QL_TITLE					3

class cQuestList : public cBaseWindow
{
public:
	cQuestList(){ 
		m_pSelectString = NULL;
	}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_QUEST_LIST; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual bool			Close( bool bSound = true );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			OnMoveWindow();

	virtual bool			OnEscapeKey();
	virtual void			OnSetFocus();

public:
	void					InitSelectQuest( int nQuestID );

protected:	
	CsPoint					_UpdateMoveWindow();	

private:
	int						_GetFirstSelectQuestID();

	// 메모리잡힌 컨트롤
protected:
	cSprite*				m_pSelMask;
	cSprite*				m_pOnMouseMask;
	cImage*					m_pProcessImage;
	cImage*					m_pExpandImage;

	// 포인터만 있는 컨트롤
protected:
	cButton*				m_pCancelButton;
	cText*					m_pCurQuestCountText;
	cScrollBar*				m_pScrollBarMain;
	cScrollBar*				m_pScrollBarSub;
	cCheckBox*				m_pHelperViewCheck;

public:
	CsPoint			GetDiscriptPos(){ return GetRootClient() + CsPoint( 340, 0 ); }

	//================================================================================================================
	//
	//		Title
	//
	//================================================================================================================
protected:
	int								m_nQuestListTabExpandSizeX;
	cStringList						m_QuestListStringMain;
	cStringList						m_QuestListStringSub;
	CsRect							m_rcTitleRegionMain;
	CsRect							m_rcTitleRegionSub;

	cString*						m_pMouseOnString;
	cString*						m_pSelectString;
	NiTStringPointerMap< int >		m_ExpandStringMain;
	NiTStringPointerMap< int >		m_ExpandStringSub;

protected:
	// Create
	void			_Create_QuestList();

	// Read
public:
	void			ResetQuestList( bool bKeepDiscript = false );
protected:
	int				_CreateQuestTab( TCHAR* szTab, bool bQTMain );

	// Add, Drop
public:
	void			AddQuest( int nQuestExeIndex, DWORD dwQuestFTID, bool bInitial = false );
protected:
	void			_DropQuest( DWORD dwQuestFTID );

	// Update
protected:
	enum eQL_UFM_STATE{ QL_ACTION_CLICK, QL_ACTION_NONE, QL_END, };
	bool			_Update_FM_QuestList();
	eQL_UFM_STATE	_Update_FM_QuestTab( CsPoint& pos, int& nRemainPos, int& nRenderCount, std::list< cString* >::iterator& itTab, CsPoint ptLocal );
	eQL_UFM_STATE	_Update_FM_QuestTitleExpand(	CsPoint& pos, int& nRemainPos, int& nRenderCount, 
													std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd, CsPoint ptLocal );
	eQL_UFM_STATE	_Update_FM_QuestTitleUnExpand( std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd );


	// Render
	void			_Render_QuestList( bool bQTMain );
	bool			_Render_QuestTab( CsPoint& pos, int& nRemainPos, int& nRenderCount, std::list< cString* >::iterator& itTab );
	bool			_Render_QuestTitleExpand(	CsPoint& pos, int& nRemainPos, int& nRenderCount, 
												std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd );
	bool			_Render_QuestTitleUnExpand( std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd );

	// 동작
	void			_QL_ClickTab( cString* pString, bool bToExpand, bool bQTMain );
	void			_QL_ClickTitle( cString* pString, bool bQTMain );

	bool			_QL_String_Update_Control( bool bQTMain );
	bool			_QL_String_Update_Control_Tab( CsPoint& pos, int& nRemainPos, int& nRenderCount, std::list< cString* >::iterator& itTab );
	bool			_QL_String_Update_Control_TitleExpand(	CsPoint& pos, int& nRemainPos, int& nRenderCount, 
															std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd );
	void			_QL_String_Update_Control_TitleUnExpand( std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd );

	// QuestCount
	void			_CalQuestCount();

	// ScrollBar
	void			_CalScrollBarRange( bool bQTMain, cString* pEnsureString = NULL );

	void			_CalScrollBarRange_Tab( CsPoint& ptRange, std::list< cString* >::iterator& itTab );
	int				_CalScrollBarRange_TitleExpand(	CsPoint& ptRange, std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd, cString* pEnsureString );
	void			_CalScrollBarRange_TitleUnExpand( std::list< cString* >::iterator& itTitle, std::list< cString* >::iterator itEnd );


public:
	void			ReleaseSelectQuest(){ m_pSelectString = NULL; }
	DWORD			GetSelectQuestID(){ return m_pSelectString ? m_pSelectString->GetValue2() : 0; }
};


