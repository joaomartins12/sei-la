#ifndef __SOUND_H__
#define __SOUND_H__

class CSound : public NiMemObject
{
public:
	CSound(){ m_pkSound = NULL; m_pTarget = NULL; }
	CSound( NiAudioSource* pSoundSource ){ m_pkSound = pSoundSource; }

	virtual ~CSound()
	{
		SAFE_NIDELETE( m_pkSound );
	}

	void		Load( char const* path, char const* name, UINT type, bool loop, bool bMusic, float fMinDist, float fMaxDist );
	void		Update(){ m_pkSound->Update( (float)g_fAccumTime ); }

	bool		FadeToVolume( float fGain, float fStartTime, float fDuration ){ return m_pkSound->FadeToGain( fGain, fStartTime, fDuration ); }

	bool		IsSameMusic( char const* cMusic ){ return ( strcmp( cMusic, m_pkSound->GetLocalName() ) == 0 ); }

	void		SetVolume( float i ){ m_pkSound->SetGain( i ); }
	float		GetVolume(){ return m_pkSound->GetGain(); }

	void		Stop();
	void		Play();
	void		Rewind();

	void		SetPos( const NiPoint3 vPos );
	void		SetMinMaxDistance( float min, float max );

	DWORD		GetStatus(){ return m_pkSound->GetStatus(); }
	void		GetPlayLength( float& fLength ){ m_pkSound->GetPlayLength( fLength ); }

	CSound*		Clone();

	bool		IsSource(){ return ( m_pkSound != NULL ); }

	// 접근시간 + 파일의 길이
	void		ConnetFile(){ m_fConnetTime = (float)g_fAccumTime; }
	float		GetLastConnetFileTime(){ return m_fConnetTime; }

	void			SetTarget( CsC_AvObject* pTarget ) { m_pTarget = pTarget; }
	CsC_AvObject*	GetTarget() { return m_pTarget; }

protected:
	NiAudioSource*		m_pkSound;
	float				m_fConnetTime;
	CsC_AvObject*		m_pTarget;	//3D사운드의 대상이되는 포인터만 들고있음
};

#endif
