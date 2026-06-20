#pragma once

class cBossScene : public NiMemObject
{
public:
	cBossScene();
	virtual ~cBossScene(){ Delete(); }		

public:
	void			Delete();
	void			DeleteBossMon();
	void			PreResetMap();
	void			Init();	
	
	void			Update(float const& fDeltaTime);
	void			Render();
	void			SetWarning2( float fLifeTime = 6000.0f);
	void			SetWarning( DWORD dwMonType,  DWORD dwSeedMon, int nCount );
	void			SetEmergency( DWORD dwMonType, int nCount );
	void			EndWarning();

	void			ResetDevice();	

protected:
	//메모리 할당변수
	cSprite*			m_pBright;
	cSprite*			m_pBG;		
	CMonster*			m_pBossMon;
	float				m_fBossAlpa;
	cSpriteAni*			m_pAniWarning;
	cSpriteAni*			m_pAniEmergency;

	enum eBOSSSCENE_TYPE { eNONE = 0, eWARNING, eEMERGENCY };
	//필요변수
	float				m_fAccumTime;
	eBOSSSCENE_TYPE		m_eType;
	float				m_fAlpa;
	int					m_nFlag;	
	float				m_fBackupMusic;	

public:
	struct sBossInfo : public NiMemObject
	{
	public:
		sBossInfo() { s_pImage = NULL; s_strCount = NULL; s_nSeedCount = -1; s_nBossCount = 0; s_bRender = false; }
		
	public:
		sCHAR_IMAGE*					s_pImage;
		cText*							s_strCount;	
		DWORD							s_dwSeedMon;		//잡아야할 몬스터 종류
		int								s_nSeedCount;		//잡아야할 몬스터 수량
		int								s_nBossCount;		//출현했을때 보스수량
		bool							s_bRender;
	};	
	void		SetBossInfo( DWORD dwMonType, int nCount, DWORD dwSeedMon, int nSeedCount );
	void		SetBossDie( DWORD dwMonType );	
	void		RenderBossInfo();
	bool		IsBoss(); 

protected:		
	sBossInfo*		_SetCount( DWORD dwMonType, int nCount );	

protected:
	std::map< DWORD, sBossInfo* >		m_mapBossInfo; 
	bool								m_bIncreaseZoomAlpha;

};


