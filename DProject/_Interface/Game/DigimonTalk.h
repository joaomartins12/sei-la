
#pragma once 

class cDigimonTalk : public cBaseWindow
{
protected:
	static CsTimeSeq		m_SoundTimeSeq;
public:
	struct sTalkEle
	{
		sTalkEle() { s_dwQuest = s_dwNpc = s_dwSkill = s_dwDigimon = 0; }
		DWORD		s_dwQuest;
		DWORD		s_dwNpc;
		DWORD		s_dwSkill;	
		DWORD		s_dwDigimon;
	};

	static bool		Print( DWORD dwMessageID, sTalkEle* pEle = NULL );
	static TCHAR*	Parcing_Talk( TCHAR* szMsg, int nLen, sTalkEle* pEle );

public:
	cDigimonTalk(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_DIGIMON_TALK; }
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetDevice();
	virtual void			ResetMap();

	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();		

	virtual void			Render();	
	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual bool			OnEnterKey() { return ( m_pEdit == NULL ) ? false : true; }

protected:
	void					_SetMacroButton( int nWordIdx );
	void					_SetEditBox( int nWordIdx );
	void					_SetDefaultTalk();
	void					_DigimonSelect( DWORD dwID );	
	void					_LoadEditDigimonTalk();	
	
protected:	
	cSprite*				m_pMouseOnMask;
	bool					m_bMask;
	int						m_nSelectMask;
	int						m_nMouseOnMaskIndex;
	int						m_nSelectTalk;

	cStringList				m_TalkList;
	cStringList				m_strTalk;
	cEditBox*				m_pEdit;
	TCHAR					m_szTalk[ MAX_PATH ];
	TCHAR					m_szDigimonName[ MAX_PATH ];

	cComboBox*				m_pCombo;
	sCHAR_IMAGE*			m_pImage;

	cComboBox*				m_pPersent;

protected:
	cResist::sDIGIMON_TALK		m_EditTalk;
	

	// 포인터만 있는 컨트롤
protected:
	cButton*				m_pCancelButton;	
	cScrollBar*				m_pScrollBar;

	cButton*				m_pDefault;	
	cButton*				m_pSave;	
	cButton*				m_pDelete;

	cString*				m_pSelectText;
	cText*					m_pSize;		
	cText*					m_pSound;		

	CsVectorPB< cButton* >	m_pbMacro;	
};

