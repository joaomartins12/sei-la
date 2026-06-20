
#pragma once 

class cOptionInterface : public cOptionBase
{
public:
	cOptionInterface(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_OPTION_INTERFACE; }
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

protected:
	void					_InitInterfaceControl();

public:
	enum eVALUE
	{
		VIEW_COSTUME	=	1,
		NAMEVIEW_SELF,
		NAMEVIEW_OTHER,
		NAMEVIEW_MONSTER,
		NAMEVIEW_NPC,		
		HPVIEW,
		FIGURE_TAMER,
		FIGURE_DIGIMON,
		FIGURE_EMPLOYMENT,
		FIGURE_PAT,
		BALLOONE_CHAT,
		BALLOONE_DIGIMON,
		BALLOONE_TIP,
#ifdef LANGAUGE_SELECT
		LANGUAGE,
#endif
		MOUSE_HAND,
		CAMERA_MODE,
		CAMERA_DIST,
		CAMERA_AUTO_ROT_SPEED,
		CAMERA_ENEMY_CRITICAL,		
		TRADE_ACTIVE,
		PARTY_ACTIVE,
		DUAL_ACTIVE,
		VIEW_LEVELEFFECT,

#ifdef GUILD_INVITATION
		GUILD_INVITATE,
#endif

#ifdef USE_BARCODE_REDER
		USE_BARCODE,
#endif
		VIEW_ATTACK_EFFECT,
		RIGHT_TOOLTIP,
		BOSS_WARNING,

		CENTER_BAR,
		QUICK_OPEN,
		CENTER_BAR_ALPHA,
#ifdef KEYBOARD_MOVE
		MOVE_MAIN,
		MOVE_INPUT,
#endif
	};

	// 메모리잡힌 컨트롤
protected:

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pDefaultBtn;

protected:
	void			_OnOk();
	void			_OnCancel();
	void			_OnDefault();
};

