#pragma once 

class CGameEventMng : public CEventCall_Base
{
public:
	CGameEventMng(void);
	~CGameEventMng(void);

public:
	virtual void	EventStart();
	virtual void	EventEnd();

	virtual void	PlaySound( char* szSound ) { g_pSoundMgr->PlaySound( szSound ); }
	virtual void	PlayMusic( char* szMusic ) { g_pSoundMgr->PlayMusic( szMusic ); }

	virtual void	EventTalk( UINT nTargetUID, TCHAR* szMsg, bool bExistDelete, int ObjType );
	virtual void    EventSave( int nSceneNum );

public:
	void		Delete();
	void		Init();
	
	void		Update( float fDeltaTime );
	void		Render();
	void		RenderText();

	void		ResetDevice();
	void		EventSkip();

protected:
	// 사용변수들
	NiPoint3	m_vDelta;
	float		m_fCameraRot;
	float		m_fCameraPitch;
	bool		m_bSkip;	
	float		m_fAccumTime;
	bool		m_bIsSound;

	//////////////////////////////////////////////////////////////////////////
	//
	// 이벤트 씬 나올때 쓰는검은바
	//
	//////////////////////////////////////////////////////////////////////////
protected:
	// 이벤트 일때 사용변수
	cSprite*		m_pSprite1;
	cSprite*		m_pSprite2;		
	cButton*		m_pSkip;

protected:
	enum eALPA_TYPE { ALPA_STOP = 0, ALPA_INCREASE, ALPA_DECREASE };
	void			_SetAlpha( eALPA_TYPE eAlpha, float fAlpha );
	void			_SetText( uint nTargetUID, TCHAR* szText );

protected:
	CsTimeSeq		m_LifeTimeSeq;
	cStringList		m_StringList; // 카메라 이벤트 대사
	eALPA_TYPE		m_eAlpa;
	float			m_fAlpaValue;


	////////////////////////////////////////////////////////////////////////////////////
	//
	//	영역만 사용하는 이벤트씬 저장
	//
	////////////////////////////////////////////////////////////////////////////////////
public:
	struct sAreaEventScene : public NiMemObject
	{
	public:
		void Delete() { m_vpAreaScene.Destroy(); }
		void AddAreaScene( CEventSceneData* nSceneData ) { m_vpAreaScene.PushBack( nSceneData ); }
		void DeleteAreaScene( CEventSceneData* nSceneData ) { m_vpAreaScene.DeleteElement_FromData( nSceneData ); }

		CsVectorPB< CEventSceneData* > m_vpAreaScene;
	};

public:
	void	LoadAreaScene();
	void	ReLoadScene( DWORD dwMapID );
	sAreaEventScene* GetAreaScene( UINT MapID );

protected:
	std::map< UINT, sAreaEventScene* > m_mapAreaSceneData; /**< 영역만 사용하는 이벤트 전체 데이터 */
	sAreaEventScene* m_CurMapAreaData;

};

extern CGameEventMng* g_pGameEventMng;