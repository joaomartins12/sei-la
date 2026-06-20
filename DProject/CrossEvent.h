#pragma once 
#ifdef CROSSWARS_SYSTEM

class cRenderTex;
class CCrossEvent : public NiMemObject  
{



public:
	CCrossEvent(void);
	~CCrossEvent(void);


	enum eAniProcess { eAni_None = 0, eAni_Start, eAni_BG, eAni_Play, eAni_End };	

	struct sTacticsAni : public NiMemObject
	{
	public:
		sTacticsAni() { s_fDigimonAlpha = 0.0f; s_pDigimon = NULL; }

		float				s_fDigimonAlpha;
		NiPoint3			s_vStartPos;	
		NiPoint3			s_vEndPos;	

		float				s_fStartRot;
		float				s_fEndRot;
		int					s_nUpdateSpeed;

		cTacticsObject*		s_pDigimon;
	};

	typedef std::vector<cRenderTex*>	vecTex;
	typedef std::vector<cRenderTex*>::iterator	vecTex_it;

	typedef std::vector<sTacticsAni*>		vecDigimon;
	typedef std::vector<sTacticsAni*>::iterator		vecDigimon_it;

	typedef std::vector<cSprite*>		vecSprite;
	typedef std::vector<cSprite*>::iterator		vecSprite_it;

private:
	cTacticsObject*			_AddDigimon( DWORD dwModelID, NiPoint3 vPos, float fRot );

public:
	void	EventStart( int nDigimonID );
	void	EventEnd();

	void	PlaySound( char* szSound ) { g_pSoundMgr->PlaySystemSound( szSound ); }
	void	PlayMusic( char* szMusic ) { g_pSoundMgr->PlayMusic( szMusic ); }
	

public:
	void		Reset();
	void		Delete();
	void		Init();

	void		Update( float fDeltaTime );
	void		Render();	
	void		PostRender();
	void		ResetDevice();	



private:	
	int				m_nDigimonCount;
	DWORD			m_TargetDIgimon;
	float			m_fCutTime;
	float			m_fScenePlayTime;
protected:		

	cSprite*		m_pBG;	

	vecDigimon			m_sDigimon;
	vecTex				m_vecRenderTex;		// 캐릭터 랜더링
	vecSprite			m_vecSprite;

	

};

extern CCrossEvent* g_pCrossEvent;
#endif