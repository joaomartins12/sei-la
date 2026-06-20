#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

class CSoundMgr : public CsSoundMng
{
public:
	CSoundMgr();		
	virtual ~CSoundMgr();

public:
	static void		GlobalInit();
	static void		GlobalShotDown();


public:
	NiAudioListener* CSoundMgr::GetListener();
	void		Destroy();
	void		Init();	

protected:	
	bool		_InitMiles();

public:
	void		Update( float fTime );

	void		OnActive();
	void		OnInactive();

protected:
	bool		m_bEnableSound;
	bool		m_bActiveFocus;


	//==========================================================================================================
	//
	//		배경음
	//
	//==========================================================================================================
protected:
	bool		m_bDeleteMusic;
	CSound*		m_pMusicAmbient;
	CSound*		m_pMusicOldAmbient;	
	float		m_fMusicVolume;
public:			
	void		PlayMusic( char const* cMusic, float fFadeTime = 1.5f );
	void		DeleteMusic( bool bJustDelete );
	void		Set_BGM_Volume( float nVol );
	void		Set_BGM_FadeVolume( float nVol );

	CSound*		GetCurMusic(){ return m_pMusicAmbient; }

	float		GetMusicVolume(){ return m_fMusicVolume; }

	//==========================================================================================================
	//
	//		환경음	// Resigt.h의	s_bSound 변수가 환경음 입니다.
	//
	//==========================================================================================================
protected:
	std::list< CSound* >	m_listCloneSound;
	std::list< CSound* >	m_listCloneSound_Move;
	std::list< CSound* >	m_listCloneSound_System;
	float					m_fSoundVolume;

public:
	virtual void		Update_MoveSound( void* pTarget, NiPoint3 vPos );
	virtual void*		PlaySound( char const* cSound, NiPoint3 vPos, float fMinDist, float fMaxDist );
	virtual void		PlaySound( char const* szSound );
	void*				PlaySound( char const* cSound, NiPoint3 vPos, CsC_AvObject* pTarget );
	void*				PlayStaticSound( char const* cSound, NiPoint3 vPos );

	void		StopSound( CSound* pSound );
	void		Set_EFM_Volume(float nVol);

	bool		IsSound( CSound* pSound );

	float		GetSoundVolume(){ return m_fSoundVolume; }

	void		Play_Stop_Sound();	

	//==========================================================================================================
	//
	//		효과음	// Resigt.h의	s_bSystem 변수가 효과음 입니다.
	//
	//==========================================================================================================
protected:
	float		m_fSystemVolume;
public:
	CSound*		PlaySystemSound( char const* cSound );
	void		PlaySystemSound( char const* cSound, float& fLength );
	CSound* 	PlaySystemSound( char const* cSound, NiPoint3 vPos, CsC_AvObject* pTarget );

	CSound*		PlayOtherSystemSound( char const* cSound );

	void		Set_System_Volume(float nVol);

	float		GetSystemVolume(){ return m_fSystemVolume; }

	void		Play_Stop_SystemSound();

	//=====================================================================================
	//
	//	Sound Object
	//
	//=====================================================================================
protected:
	NiTStringPointerMap< CSound* >*		m_pmapSound;
	CsTimeSeq							m_CheckSoundResTimeSeq;
public:
	CSound*			GetSound( char const* cSound, bool b3DSound, float fMinDist = 300.0f, float fMaxDist = 4000.0f );
	CSound*			GetOtherSound( char const* cSound, bool b3DSound, float fMinDist = 300.0f, float fMaxDist = 4000.0f );
	CSound*			GetStaticSound( char const* cSound, bool b3DSound, float fMinDist = 300.0f, float fMaxDist = 4000.0f );
};

extern CSoundMgr* g_pSoundMgr;

#endif