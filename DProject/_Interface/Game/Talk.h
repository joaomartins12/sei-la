

#pragma once 

#define TALK_LINE_DELTA				5

class cTalk : public cBaseWindow
{
public:
	cTalk();
	~cTalk();

	// 전역
	enum eTALK_TYPE{	DIGITAMA_SCAN, DIGITAMA_SELL, EI_ATTACH, EI_CHECK, QT_CREATE_GUILD, PORTAL, QT_EVENTREPEAT_COMP, QT_MAIN_COMP,
						QT_REPEAT_COMP, QT_SUB_COMP, QT_EVENTREPEAT_RECV, QT_MAIN_RECV, QT_REPEAT_RECV, QT_SUB_RECV, STORE, TACTICS,
						CAPSULE,
						DIGIBALL,
						SKILL_FORSTER,
						TALK_DIGIMON_BATTLE,
						INFINITE_WAR_STAT,
						INFINITE_WAR_END,
						INFINITE_NOTICEBOARD,
						MAX_TYPE, };

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_TALK; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }

	virtual bool			OnEscapeKey();
	virtual bool			OnMacroKey(const MSG& p_kMsg);
	static bool				IsHaveQuest( CNpc* pTargetNpc );

protected:
	bool					_UpdateStringList();
	void					_Render_Npc();

	void					_CheckQuest( std::list< cString* >* pList );

private:
	bool					_SelectedStringList( cString* pSelectString, int nValue1, int nValue2 );
	bool					_SelectList( bool bUpArrow );
	bool					_CheckInputKey( const WPARAM& wParam );

	// 메모리잡힌 컨트롤
protected:
	cSprite*		m_pSelMask;
	cPopUp*			m_pBGWindow;

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pCancelButton;
	cText*			m_pName;	
	cScrollBar*		m_pScrollBar;

private:
	struct stSelectList 
	{
		stSelectList() : pString(NULL), nStringIndex(0) {}
		~stSelectList() {}

		cString*	pString;
		int			nStringIndex;
	};

protected:
	cString*					m_pSelectedString;
	cStringList					m_StringList;
	CsC_AvObject*				m_pTarget;
	nsCsNpcTable::eNPC_TYPE		m_eNpcType;
	CsPoint						m_ptSelMarkPos;
	bool						m_bRenderSelMark;
	bool						m_bClickMask;
	bool						m_bUseTamerDistCancel;

	TCHAR			m_szPortalDisableDiscript[ 10 ] [ 64 ];
	int				m_nPortalDisableCount[ 10 ];
	int				m_nPortalDisableID[ 10 ];


public:
	void			SetTarget( CsC_AvObject* pTarget, bool bNew = true, TCHAR* szTalk = NULL );

	CsC_AvObject*	GetTarget(){ return m_pTarget; }

	void			DisableTamerDistCancel(){ m_bUseTamerDistCancel = false; }

	void			EnableString(int nNPCIdx ,int nIdx, bool bNpcIDCheck = true);
	cStringList		GetStringList() const {return m_StringList;}
	CsRect			GetControlPos( int nTalkIdx, int nSubValue = 0 );
};



