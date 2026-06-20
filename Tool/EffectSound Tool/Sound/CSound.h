#ifndef __SOUND_H__
#define __SOUND_H__


class CSound : public NiMemObject
{
public:
	CSound(){ m_pkSound = NULL; }
	CSound( NiAudioSource* pSoundSource ){ m_pkSound = pSoundSource; }

	virtual ~CSound()
	{
		SAFE_NIDELETE( m_pkSound );
	}

	void		Load( char* path, char* name, UINT type = NiAudioSource::TYPE_AMBIENT, bool loop = true, bool bMusic = true );
	void		Update(){ m_pkSound->Update( g_fAccumTime ); }

	bool		FadeToVolume( float fGain, float fStartTime, float fDuration ){ return m_pkSound->FadeToGain( fGain, fStartTime, fDuration ); }
	bool		IsSameMusic( char* cMusic ){ return ( strcmp( cMusic, m_pkSound->GetLocalName() ) == 0 ); }

	void		SetVolume( float i ){ m_pkSound->SetGain( i ); }
	float		GetVolume(){ return m_pkSound->GetGain(); }

	void		Stop();
	void		Play();
	void		Rewind();

	void		SetPos( const NiPoint3 vPos );
	void		SetMinMaxDistance( float min, float max );

	DWORD		GetStatus(){ return m_pkSound->GetStatus(); }

	CSound*		Clone();

	bool		IsSource(){ return ( m_pkSound != NULL ); }


protected:
	NiAudioSource*		m_pkSound;
};

#endif
