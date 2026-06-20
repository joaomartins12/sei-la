
#pragma once 

#ifdef USE_REVISION

class cTactics_Charge : public cBaseWindow
{
public:
	cTactics_Charge(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_TACTICS_CHARGE; }
	virtual void			Destroy();	
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	

	virtual void			Update();
	virtual eMU_TYPE		Update_ForMouse();	
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			PreResetMap();
	virtual void			ResetMap(){ Close( false ); }	

	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual void			OnMoveWindow();

	virtual void			PlayOpenSound(){}
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );	

	virtual void			UserDie(){ Close( false ); }
	virtual void			RecvDiedAlready(){ Close( false ); }


	// 메모리잡힌 컨트롤
protected:
	cButton*				m_pBtnOk;
	cButton*				m_pBtnCancel;
	cButton*				m_pBtnExit;	
	cText*					m_pTextResistCount;	

	// 포인터만 있는 컨트롤
protected:
	cButton*				m_pBtnAutoResist;
	cProgressBar*			m_pPBarProcess;
	cButton*				m_pCancelButton;


protected:
	CsC_AvObject*			m_pTarget;
	cButton*				m_pBtnActive;	
	cItemInfo				m_ResistItem;
	CsRect					m_rcResistItem;	

public:
	void			SetTarget( CsC_AvObject* pTarget ){ assert_cs( pTarget != NULL ); m_pTarget = pTarget; }

	void			SendServer();
	void			RecvServer();

	//========================================================================================================
	//
	//	게이지 충전
	//
	//========================================================================================================
protected:
	bool			m_bProcessing;
	float			m_fProcessTime;
	bool			m_bWaitRecvServer;

protected:
	void			_Update_Process();

	void			_ProcessSuccess();
	void			_ProcessCancel();

public:
	void			StartProcess();

	//========================================================================================================
	//
	//	자동 등록
	//
	//========================================================================================================
protected:
	int				m_nSearchInvenIndex;
	int				m_nResistInvenIndex;
protected:
	void			_AutoResist();

public:
	bool			IsResist( int nInvenIndex );
	void			SetResist( int nInvenIndex );
	void			SetResist( int nInvenIndex, int nCount );

};

#endif