
#pragma once 
#define IF_OP_MACRO_SEL_COUNT		15

class cOptionMacro : public cOptionBase
{
public:
	cOptionMacro(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_OPTION_MACRO; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			OnMoveWindow();
	virtual void			ResetMap(){ Close( false ); }

	virtual bool			OnEscapeKey(){ _OnCancel(); return true; }
	virtual bool			OnMacroKey(const MSG& p_kMsg);

	// 메모리잡힌 컨트롤
protected:
	cCheckBox*		m_pSelectCheck;
	int				m_nSelectIndex;
	int				m_nOldScrollIndex;

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pDefaultBtn;
	cButton*		m_pReleaseBtn;

protected:
	void			_InitMacroKeyText();
	void			_MakeKey( DMKEY::eTYPE type, TCHAR const* szOrder );		//2016-01-27-nova
	

protected:
	void			_OnOk();
	void			_OnCancel();
	void			_OnDefault();
	void			_OnRelease();

protected:
	HotKey			mHotKey;						//2015-02-01-nova
	HotKey::sINPUT	hInput;							//2015-02-01-nova
	int				iSameKey;						//2015-02-01-nova	
	int				iKeyVal;						//2015-02-01-nova

	sOP_CHECK_RADIO*			mBackupOP;			//2015-02-01-nova
	DMKEYBOARD::KeyCode			mBackupKey;			//2015-02-01-nova
	uint						mBackupModifier;	//2015-02-01-nova

public:
	void			SetBackupKey();				

public:
	DWORD			m_dwTick;
};

