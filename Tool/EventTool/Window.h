
#pragma once 

#define SYSTEM_SOUND_PATH_LEN		64

class cWindow : public NiMemObject
{
	//=======================================================================================================================
	//
	//		생성자 소멸자
	//
	//=======================================================================================================================
public:
	cWindow(){		
		//m_eSoundType = SD_NONE;
		m_pParentWindow = NULL;
	}
	virtual ~cWindow(){}

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

public:
	virtual void		Delete(){}
	void				Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize );	
	virtual void		Render(){}
	

	//=======================================================================================================================
	//
	//		state
	//
	//=======================================================================================================================
public:
	virtual void		SetPos( CsPoint ptPos );
	virtual void		SetSize( CsPoint ptSize );

	CsPoint				GetDefaultPos(){ return m_ptDefault; }

	CsPoint				GetClient(){ return m_ptClient; }
	CsPoint				GetSize(){ return m_ptSize; }

	//=======================================================================================================================
	//
	//		동작
	//
	//=======================================================================================================================
public:
	//CURSOR::eTYPE		Click(){ return g_Cursor.CheckClickBox( m_ptClient.x, m_ptClient.y, m_ptSize.x, m_ptSize.y ); }
	void				SetMove( CsPoint ptDelta );
	CsPoint				MousePosToWindowPos( CsPoint ptMousePos ){ return ptMousePos - m_ptClient; }

	/*
	//=======================================================================================================================
	//
	//		사운드
	//
	//=======================================================================================================================
protected:
	eSOUND			m_eSoundType;
public:
	void			SetSound( eSOUND type ){ m_eSoundType = type; }
	void			PlaySound(){ PlaySound( m_eSoundType ); }
	*/

	//======================================================================================
	//
	//		ResetDevice
	//
	//======================================================================================
public:
	virtual void		ResetDevice( CsPoint pos );
	virtual void		ResetDevice();
};

