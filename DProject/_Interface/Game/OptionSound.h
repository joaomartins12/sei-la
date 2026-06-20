
#pragma once 

class cOptionSound : public cOptionBase
{
public:
	cOptionSound(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_OPTION_SOUND; }
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

	virtual void			Combo_SelChange( sOP_COMBO* pCR );

public:
	enum eVALUE
	{
		BGM	=	1,
		SOUND,
		SYSTEM_SOUND,
		MOVE_SOUND,
		VOICE,
	};

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pDefaultBtn;

protected:
	void			_OnOk();
	void			_OnCancel();
	void			_OnDefault();

	char const*		_LoadMusic();
};

