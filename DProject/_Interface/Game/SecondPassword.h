#pragma once 

#if (defined VERSION_TAIWAN || defined VERSION_HONGKONG)
	#define MAX_SECONDPASS 32
#elif defined VERSION_KOR
	#define MAX_SECONDPASS 9
#else
	#define MAX_SECONDPASS 8
#endif

class cSecondPassword : public cScriptUI
{
public:
	cSecondPassword();
	virtual ~cSecondPassword();

	enum ePassType { eSet = 0, eQuestion, eEnd };	// 설정창인지? 확인창인지?
	enum eCaseType { eNone = 0, ePassRegist, ePassRegistCheck, ePassChange, ePassChangeCheck }; // 어떤경우인지?

public:
	static void GlobalInit();
	static void GlobalShotDown();

public:
	void		Delete();	
	void		ResetDevice();

	void		Update();
	void		Render();

	void		Open( ePassType eType, eCaseType eCase = eNone, int nValue = 0 );
	void		Close();
	bool		IsOpen() { return m_bShow; }

	void		SetPassCheck( bool bSuccess );	
	bool		IsRegisted();
	bool		IsCertified(); 	

	void		SendPassCertify();

protected:
	void		_Init();
	void		_SetPassword();

#ifdef USE_DMO_INI
public:
	char		m_szIniPass[ MAX_SECONDPASS + 1 ];
#endif

protected:
	ePassType	m_eType;
	eCaseType	m_eCaseType;
	bool		m_bShow;	
	bool		m_bSuccess; //2차 비밀번화 확인을 했는가??

	struct sPassword_Info : public NiMemObject
	{
		int			s_nData;
		cButton*	s_Btn;	
		cText*		s_Text;
	};
	typedef std::list< sPassword_Info* >					LIST;
	typedef std::list< sPassword_Info* >::iterator			itLIST;	
	LIST	m_BtnInfo; 	

	TCHAR			m_szPassword[ MAX_SECONDPASS + 1 ];	// 현재 창에 입력되어있는 비밀번호
	char			m_szPrePassword[ MAX_SECONDPASS + 1 ]; // 이전 비밀번호
	char			m_szCheckPassword[ MAX_SECONDPASS + 1 ]; // 이전 비밀번호

	cText*			m_Str;
	cString*		m_Text;
	cStringList		m_StringList;

	cButton*		m_pOk;
	cButton*		m_pDelete;
	cButton*		m_pClear;	
	cButton*		m_pCancel;

	int				m_nValue;

};

extern cSecondPassword*		g_pSecondPassword;