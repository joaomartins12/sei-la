
#pragma once 

#define SYSTEM_SOUND_PATH_LEN		64

class cWindow : public NiMemObject, public UI::UIEventMng
{
	//=======================================================================================================================
	//
	//		생성자 소멸자
	//
	//=======================================================================================================================
public:
	cWindow();
	virtual ~cWindow();

	//=======================================================================================================================
	//
	//		static
	//
	//=======================================================================================================================
public:
	static void		GlobalInit();
	static void		GlobalShotDown();

	//	기본 경로
protected:
	static char		m_cWorkingFolder[ MAX_PATH ];
public:
	static void		SetWorkingFolder( char* cPath );

	//	타입
public:
	enum eTYPE{
		T_Window,
		T_Sprite,
		T_Button,
		T_Image,
		T_CheckBox,
		T_RadioButton,
		T_EditBox,
		T_SpriteAni,
		T_Money,
		T_ComboBox,
		T_Scrollbar,
		T_ListBox,
		T_TreeBox,
		T_Progress,
		T_New_Progress,
		T_Timer_Progress,
	};

	virtual eTYPE	GetType(){ return T_Window; }

	//	사운드 
public:
	enum eSOUND{
		SD_Ba1,		SD_Ba2,
		SD_Bu1,		SD_Bu2,		SD_Bu3,		SD_Bu4,		
		SD_Wi1,		SD_Wi2,
		SD_At1,		SD_At2,		SD_At3,
		SD_Np1,		SD_Np2,		SD_Np3,		SD_Np4,
		SD_Wa1,		SD_Wa2,		SD_Wa3,
		SD_Qu1,		SD_Qu2,
		SD_MSS1,	SD_MSS2,
		SD_MAX_COUNT,		
		SD_NONE,
	};
protected:
	static char		m_cSound[ SD_MAX_COUNT ][ SYSTEM_SOUND_PATH_LEN ];
public:
	static void		PlaySound( eSOUND type );
	static void		PlaySound( char* cPath );
	static uint		GetSoundTypeBaseIndex( char* cType );


	//=======================================================================================================================
	//
	//		Base
	//
	//=======================================================================================================================
protected:
	CsPoint				m_ptClient;	
	CsPoint				m_ptDefault;
	CsPoint				m_ptSize;

	cWindow*			m_pParentWindow;
	bool				m_bApplyWindowSize;
	bool				m_bVisible;

public:
	virtual void		Delete(){}
	virtual void		Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize );	
	virtual void		Render(){}
	virtual void		Update(float const& fDeltaTime) {};
	virtual	void		SetEnable( bool bValue ) {};
	virtual void		SetVisible( bool bValue );

	bool				GetVisible() const;

	bool				PtInWindowRect( CsPoint const& pt );

	void				SetParentWindow( cWindow* pParent );

	virtual	void		OnMouseOver() {};
	virtual	void		OnMouseLeave() {};
	virtual	void		OnMouseDown() {};
	virtual	void		OnMouseUp() {};
	virtual void		OnMousePressed() {};
	//=======================================================================================================================
	//
	//		state
	//
	//=======================================================================================================================
public:
	virtual void		SetPos( CsPoint ptPos );
	virtual void		SetSize( CsPoint ptSize );

	CsPoint				GetDefaultPos();

	CsPoint				GetClient() const;
	CsPoint				GetSize() const;
	CsRect				GetRect() const;

	//=======================================================================================================================
	//
	//		동작
	//
	//=======================================================================================================================
public:
	CURSOR::eTYPE		Click();

	void				SetMove( CsPoint ptDelta );
	CsPoint				MousePosToWindowPos( CsPoint ptMousePos );

	//=======================================================================================================================
	//
	//		사운드
	//
	//=======================================================================================================================
protected:
	eSOUND			m_eSoundType;
public:
	void			SetSound( eSOUND type );
	void			PlaySound();

	//======================================================================================
	//
	//		ResetDevice
	//
	//======================================================================================
public:
	virtual void		ResetDevice( CsPoint pos );
	virtual void		ResetDevice();

	void				SetUserData( CUSTOMDATA* pUserData );
	CUSTOMDATA*			GetUserData() const;

	void				SetControlName( std::string const& sName );
	std::string const&	GetControlName() const;

private:
	CUSTOMDATA*			m_pUserData;
	std::string			m_sControlName;
};

