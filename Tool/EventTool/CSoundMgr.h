#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

class CSoundMgr
{
public:
	CSoundMgr();		
	virtual ~CSoundMgr();		


public:
	NiAudioListener* CSoundMgr::GetListener();
	void		Init();

	bool		m_bEnableSound;

protected:
	void		_Shutdown();
	bool		_InitMiles();

public:
	void		Update( float fTime );

	void		OnActive();
	void		OnInactive();


	//==========================================================================================================
	//
	//		배경음
	//
	//==========================================================================================================
protected:
	CSound*		m_pMusicAmbient;
	CSound*		m_pMusicOldAmbient;	
public:
	void		PlayMusic( char* cMusic );
	void		DeleteMusic();
	void		Set_BGM_Volume(float nVol);

	//==========================================================================================================
	//
	//		효과음
	//
	//==========================================================================================================
	protected:
	std::list< CSound* >		m_listSound;
public:
	void		PlaySound( char* cSound, NiPoint3 vPos );
	void		PlaySound( char* cSound );
	void		Set_EFM_Volume(float nVol);
};

extern CSoundMgr* g_pSoundMgr;

#endif