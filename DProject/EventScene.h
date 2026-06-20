#pragma once 


class cRenderTex;
class CEventScene : public NiMemObject  
{
public:
	CEventScene(void);
	~CEventScene(void);

public:
	void	EventStart( int nSceneNum );
	void	EventEnd();
	void	NextStep();

	void	PlaySound( char const* szSound ) { g_pSoundMgr->PlaySystemSound( szSound ); }
	void	PlayMusic( char const* szMusic ) { g_pSoundMgr->PlayMusic( szMusic ); }
	
	void    EventSave( int nSceneNum );

public:
	void		Delete();
	void		Init();

	void		Update( float fDeltaTime );
	void		Render();	

	void		ResetDevice();	

protected:
	char const*		_LoadMusic();
	void			_LoadCheck( float fDeltaTime );
	void			_DeleteTarget();
	void			_InitTarget( DWORD dwIDX, DWORD dwModelID, TCHAR* szName );

	void			_NextStep();

	bool			_IsTarget( DWORD dwIdx );  
	CsC_AvObject*	_GetTarget( DWORD dwIdx );

	void			_SetSceneTarget();	
	void			_SetEffect();	
	void			_SetAni();

	float			_GetHeight();
	float			_GetDist();

protected:	
	std::map< DWORD, CsC_AvObject* >			m_mapTarget;
	CSceneData*									m_pScene;
	std::list< CSceneData::sData* >::iterator	m_it;

	// 사용변수들	
	int			m_nSceneNum;
	bool		m_bIsSound;	
	bool		m_bLoad;

	cText*		m_pTitle;
	bool		m_bAlpha;
	float		m_fTitleAlpha;

	// 캐릭터 랜더링
	cRenderTex*		m_pRenderTex1;
	cRenderTex*		m_pRenderTex2;
	cRenderTex*		m_pRenderTex3;
	cRenderTex*		m_pRenderTex4;
	cRenderTex*		m_pRenderTex5;

	// 그려지는 타겟정보 포인터만 들고있다
	CsSceneDataMng::sTargetData*	m_pTarget1;
	CsSceneDataMng::sTargetData*	m_pTarget2;
	CsSceneDataMng::sTargetData*	m_pTarget3;
	CsSceneDataMng::sTargetData*	m_pTarget4;
	CsSceneDataMng::sTargetData*	m_pTarget5;


	//////////////////////////////////////////////////////////////////////////
	//
	// 이벤트 씬 나올때 쓰는검은바
	//
	//////////////////////////////////////////////////////////////////////////
protected:
	// 이벤트 일때 사용변수
	cSprite*		m_pBack;
	cSprite*		m_pSprite1;
	cSprite*		m_pSprite2;		
	cSprite*		m_pSprite3;

	cButton*		m_pSkip;
	cButton*		m_pNext;

protected:
	enum eALPA_TYPE { ALPA_STOP = 0, ALPA_INCREASE, ALPA_DECREASE };
	void			_SetAlpha( eALPA_TYPE eAlpha, float fAlpha );
	void			_SetText();

protected:	
	cStringList		m_StringList; // 카메라 이벤트 대사
	eALPA_TYPE		m_eAlpa;
	float			m_fAlpaValue;


	//////////////////////////////////////////////////////////////////////////
	//
	//	이벤트 영역 체크
	//
	//////////////////////////////////////////////////////////////////////////
public:
	void						LoadScene( DWORD dwMapId );
	std::map< DWORD, DWORD >*	GetCondMap() { return &m_mapSceneCond; }

protected:
	std::map< DWORD, DWORD > m_mapSceneCond;
	

};

extern CEventScene* g_pEventScene;