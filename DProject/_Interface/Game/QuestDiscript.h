
#pragma once 

class cQuestDiscript : public cBaseWindow
{
public:
	cQuestDiscript();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_QUEST_DISCRIPT; }
	virtual void			Destroy();
	virtual bool			Close( bool bSound = true );

	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			OnMoveWindow();
	virtual void			OnSetFocus();


	// 메모리잡힌 컨트롤
protected:	

	// 포인터만 있는 컨트롤
protected:
	cButton*				m_pDropButton;
	cButton*				m_pCancelButton;
	cScrollBar*				m_pScrollBar;
	cText*					m_pExpText;
	cMoneyIF*				m_pMoney;

protected:
	CsQuest*				m_pFTQuest;

	int						m_nRequiteItemCount;
	cItemInfo				m_RequiteItemInfo[ 6 ];
	sIFIcon					m_RequiteItemIFIcon[ 6 ];
	cStringList				m_StringBody;

	int						m_OpenDigimonID;
	int						m_OpenBaseDigimonID;
	cButton*				m_pkBTOpenSlot;
	cString					mEvoSlot;

public:
	void			SetQuest( DWORD dwQuestFTID, bool bKeepScroll );
	CsQuest*		GetQuest(){ return m_pFTQuest; }

	int				GetCurScroll(){ return m_pScrollBar->GetCurPosIndex(); }

	void			ProcessFindWay( void* pSender, void* pData );

	cString*		MakeFindWayButton( int nTargetID, CsQuestCompRequire::eTYPE  eQuestCompleteType, const TCHAR* pButtonText, NiColor color = FONT_WHITE );
	bool			MakeFindWayButton( cString* pString, int nTargetID, CsQuestCompRequire::eTYPE eQuestCompleteType, const TCHAR* pButtonText );
	cString*		MakeFindWayButton( CsQuestCompRequire* pCompleteInfo, const TCHAR* pButtonText );

protected:
	TCHAR*			_PacingText( TCHAR* szDest, const int nDestSize, TCHAR* szSrc, CsQuestCompRequireGroup* pApplyGroup );
};


