

#pragma once 

class cQuestRev : public cBaseWindow
{
public:
	cQuestRev(void);

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_QUEST_REV; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }

	virtual bool			OnMacroKey(const MSG& p_kMsg);
	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual void			PlayOpenSound(){}

	DWORD					GetQuestUIDX() const;

protected:
	bool					_UpdateStringList();
	void					_Render_Npc();
	void					_NewBiQuestCheck();
	void					_AcceptQuest();

	// 메모리잡힌 컨트롤
protected:
	cPopUp*			m_pBGWindow;
	cSprite*		m_pExpSprite;

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pCancelButton;
	cButton*		m_pAcceptButton;
	cText*			m_pName;	
	cScrollBar*		m_pScrollBar;

protected:
	cStringList		m_StringList;
	cString			m_RequiteString;
	CsC_AvObject*	m_pTarget;
	CsQuest*		m_pQuestFT;	

	int				m_nRequiteItemIconCount;
	int				m_nRequiteItemType[ 6 ];		//chu8820 이벤트 보상 6개로 주려고 임시로 수정 5 -> 6
	sIFIcon			m_RequiteItemIFIcon[ 6 ];				// 5개를 넘지 않을것이기 때문에

	int				m_OpenSlotIdx;
	cButton*		m_pkBTOpenSlot;
	bool			m_bUseTamerDistCancel;

public:
	void			SetTarget( CsC_AvObject* pTarget, int nQuestFTID );
	CsC_AvObject*	GetTarget(){ return m_pTarget; }

	void			DisableTamerDistCancel(){ m_bUseTamerDistCancel = false; }
};


