

#pragma once

class CLoading : public NiMemObject
{
public:
	CLoading(void);
   ~CLoading(void);

public:
	enum eSTATE
	{
		NORMAL, WAIT_MATCH, EXIT_WAIT_MATCH,
	};
	static eSTATE		g_eState;
	static int			g_nStateValue;

public:
	void	Destroy();
	void	Init_Normal();
	void	Init_WaitMatch();

protected:
	bool			m_bUseStartFadeIn;
	cSprite*		m_pBack;
	cProgressBar*	m_pLoadBar;
	int				m_nStep;
	
	cSprite*	m_pSprLoadingBar;
	cSprite*	m_pBarTail;
	cSprite*	m_pPcBang;

#ifdef LOADING_SUB_NOTICE
	cSprite*	m_pSubNotice;
#endif

	NiColorA	m_OrgBackColor;
	
	cSprite*		m_pTip;
	cText*			m_pLoading;
	cStringList		m_pTipString;
	CsPoint			m_TextSize;
	CsPoint			m_DefaultPoint;	

public:
	void		ChangeState( eSTATE next );
	void		Render();		
	void		NextStep(int nStep);						// 100%로를 기준으로 로딩바 다음 단계 설정

	bool		DataProcess();

protected:
	void		_Render_Normal();
	void		_Render_WaitMatch();

	bool		_DataProcess_Normal();
	bool		_DataProcess_WaitMatch();
	bool		_Load_Data_Normal();
	bool		_Load_Data_WaitMatch();

	//========================================================================================================
	//
	//	매칭
	//
	//========================================================================================================
protected:	
	cSprite*		m_pMatchBG;
	cSprite*		m_pMatch_UserBG;

	struct sUSER
	{
		CsPoint		s_ptBGPos;
		cText		s_Name;
		bool		s_bEnable;
		bool		s_bLoadComp;
	};
	sUSER			m_UserArray[ 2 ][ 4 ];	

public:
	void			LoadComp( TCHAR* szLoadCompUser );
};

extern CLoading* g_pLoading;

