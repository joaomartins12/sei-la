
#pragma once 

class cOptionGraphic : public cOptionBase
{
public:
	cOptionGraphic(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_OPTION_GRAPHIC; }
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
	enum eCOMBO_VALUE
	{
		VIEW_MODE	=	1,
		VIEW_RESOLUTION,
		TEX_DETAIL,
		SHADOW,
		ANTIALICING,
		CATOON,
		CELL_OBJECT,
		H_SYNC,
		USE_MY_SHADER,
		WEATHER,
		USE_BRIGHT,
		POST_EFFECT_BLUR,
	};

	enum eSLIDER_VALUE
	{
		TR_SIGHT,
		GAME_BRIGHT,
	};

	// 메모리잡힌 컨트롤
protected:
	cRadioButton*	m_pDefaultSet;	

	// 원래값
protected:
	int				m_nOptionDefIndex;

protected:
	void			_OnOk();
	void			_OnCancel();
	void			_OnDefault( int nDefault );	
};

