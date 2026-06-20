

#pragma once 

class cQuestHelper : public cBaseWindow
{
	struct QuestHelperButtonInfo : public CUSTOMDATA
	{
		QuestHelperButtonInfo( int nIndex ) : nQuestIndex(nIndex)
		{}

		~QuestHelperButtonInfo() {};
		int nQuestIndex;
	};

public:
	cQuestHelper();
	~cQuestHelper();
		

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_QUEST_HELPER; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual bool			IsShowWindow();

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();

	virtual bool			IsInWindow( CsPoint pos );

protected:
	bool					_UpdateStringList();
	void					_Render_Npc();

	// 메모리잡힌 컨트롤
protected:
	cPopUp*			m_pBGWindow;
	cImage*			m_pHelperStateImage;
	cSprite*		m_pLineLeft;
	cSprite*		m_pLineRight;

	// 포인터만 있는 컨트롤
protected:
	cScrollBar*		m_pScrollBar;

protected:
	cStringList		m_StringList;
	bool			m_bView;

public:
	void			ResetHelperString();
	void			SetView( bool bView ){ m_bView = bView; }
	bool			IsView(){ return m_bView; }

	cString*		MakeFindWayButton( int nTargetID, CsQuestCompRequire::eTYPE eQuestCompleteType, const TCHAR* pButtonText, NiColor color = FONT_RED );
	cString*		MakeFindWayButton( CsQuestCompRequire* pCompleteInfo, const TCHAR* pButtonText );

	void			ProcessFindWay( void* pSender, void* pData );
	void			OpenQuestDiscriptionWindow( void* pSender, void* pData );

protected:
	void			_ResetHelperString( CsQuest::eQUEST_TYPE eType );
	void			_AddLine( CsQuest::eQUEST_TYPE eType );
};


