#include "stdafx.h"
#include "CSoundMgr.h"

CSoundMgr* g_pSoundMgr = NULL;


CSoundMgr::CSoundMgr()
{
	m_pMusicAmbient		=	NULL;
	m_pMusicOldAmbient	=	NULL;
}

CSoundMgr::~CSoundMgr()
{
	_Shutdown();
}

void CSoundMgr::_Shutdown()
{
	if( m_bEnableSound == false )
		return;

	SAFE_NIDELETE( m_pMusicAmbient );
	SAFE_NIDELETE( m_pMusicOldAmbient );

	std::list< CSound* >::iterator it = m_listSound.begin();
	for( ; it!=m_listSound.end(); ++it )
		SAFE_NIDELETE(*it);
	m_listSound.clear();

	NiAudioSystem::GetAudioSystem()->Shutdown();
}

NiAudioListener* CSoundMgr::GetListener()
{
	return NiAudioSystem::GetAudioSystem()->GetListener();
}


bool CSoundMgr::_InitMiles()
{
    NiMilesAudioSystem* pkSS = 
        (NiMilesAudioSystem*)NiAudioSystem::GetAudioSystem();

    pkSS->SetHWnd( g_pMainWnd->GetSafeHwnd() );
	
	if (!pkSS->Startup("Data/Sound") )
    {        
        return false;
    }

	return true;
}

void CSoundMgr::Init()
{
    if ( _InitMiles() == false )
	{
        m_bEnableSound = false;
		return;
	}

    NiAudioSystem* pkSS = NiAudioSystem::GetAudioSystem();
    pkSS->SetBestSpeakerTypeAvailable();
    pkSS->SetUnitsPerMeter( 1.0f );

	m_bEnableSound = true;
}

void CSoundMgr::Update(float fTime)
{
	if( m_bEnableSound == false )
		return;

	GetListener()->SetWorldTranslate( g_Camera.GetCameraObj()->GetWorldTranslate() );
	
    NiAudioSystem::GetAudioSystem()->Update( fTime, true );

	if( m_pMusicAmbient )
	{
		if( m_pMusicAmbient->GetVolume() == 0.0f )
		{
			m_pMusicAmbient->Stop();
			SAFE_NIDELETE( m_pMusicAmbient );			
		}
	}
	if( m_pMusicOldAmbient )
	{
		if( m_pMusicOldAmbient->GetVolume() == 0.0f )
		{
			m_pMusicOldAmbient->Stop();
			SAFE_NIDELETE( m_pMusicOldAmbient );			
		}
	}

	std::list< CSound* >::iterator it = m_listSound.begin();
	std::list< CSound* >::iterator itDelete;
	while( it!=m_listSound.end() )
	{
		itDelete = it;
		++it;
		if( (*itDelete)->GetStatus() == NiAudioSource::DONE )
		{
			SAFE_NIDELETE(*itDelete);
			
			m_listSound.erase( itDelete );
		}
	}
}

void CSoundMgr::OnActive()
{
	/*if( m_bEnableSound == false )
		return;

	if( ( m_pMusicAmbient )&&( m_pMusicAmbient->IsSource() == true ) )
		m_pMusicAmbient->SetVolume( g_pResist->GetMusic_Result() );
	if( ( m_pMusicOldAmbient )&&( m_pMusicOldAmbient->IsSource() == true ) )
		m_pMusicOldAmbient->SetVolume( g_pResist->GetMusic_Result() );*/
}

void CSoundMgr::OnInactive()
{
	/*if( m_bEnableSound == false )
		return;

	if( ( m_pMusicAmbient )&&( m_pMusicAmbient->IsSource() == true ) )
	{
		m_pMusicAmbient->SetVolume( g_pResist->GetMusic_Result() );
	}
	if( ( m_pMusicOldAmbient )&&( m_pMusicOldAmbient->IsSource() == true ) )
	{
		m_pMusicOldAmbient->SetVolume( g_pResist->GetMusic_Result() );
	}*/
}



//==========================================================================================================
//
//		배경음
//
//==========================================================================================================

void CSoundMgr::PlayMusic( char* cMusic )
{
	if( m_bEnableSound == false )
		return;

	// 기존 사운드와 같지 않다면
	if( ( m_pMusicAmbient == NULL )||( m_pMusicAmbient->IsSameMusic( cMusic ) == false ) )
	{
		// 사운드 제거
		if( m_pMusicAmbient != NULL )
		{
			m_pMusicOldAmbient = m_pMusicAmbient;
			m_pMusicOldAmbient->FadeToVolume( 0.0f, (float)g_fAccumTime, 1.3f );
			m_pMusicAmbient = NULL;
		}
		// 사운드 로드
		if( cMusic[ 0 ] != NULL )
		{
			m_pMusicAmbient = NiNew CSound;
			m_pMusicAmbient->Load( "Data/Music", cMusic );

			m_pMusicAmbient->FadeToVolume( 1.0f, (float)g_fAccumTime, 1.3f );
			m_pMusicAmbient->Play();
		}
	}
}

void CSoundMgr::DeleteMusic()
{
	if( m_bEnableSound == false )
		return;

	if( m_pMusicAmbient != NULL )
	{
		m_pMusicAmbient->FadeToVolume( 0.0f, (float)g_fAccumTime, 0.6f );
	}
	if( m_pMusicOldAmbient != NULL )
	{
		m_pMusicOldAmbient->FadeToVolume( 0.0f, (float)g_fAccumTime, 0.6f );
	}
}

void CSoundMgr::Set_BGM_Volume( float nVol )
{
	if( m_bEnableSound == false )
		return;

	if(m_pMusicAmbient)
	{
		m_pMusicAmbient->SetVolume( 1.0f );
	}
	if( m_pMusicOldAmbient )
	{
		m_pMusicOldAmbient->SetVolume( 1.0f );
	}
}

//==========================================================================================================
//
//		효과음
//
//==========================================================================================================
void CSoundMgr::PlaySound( char* cSound, NiPoint3 vPos )
{
	if( m_bEnableSound == false )
		return;

	CSound* pSound = NiNew CSound;
	pSound->Load( "Data/Sound", cSound, NiAudioSource::TYPE_3D, false, false );

	pSound->SetVolume( 1.0f );
	pSound->SetPos( vPos );
	pSound->Play();
	m_listSound.push_back( pSound );
}

void CSoundMgr::PlaySound( char* cSound )
{
	if( m_bEnableSound == false )
		return;

	CSound* pSound = NiNew CSound;
	pSound->Load( "Data/Sound", cSound, NiAudioSource::TYPE_AMBIENT, false, false );
	pSound->SetVolume( 1.0f );
	pSound->Play();
	m_listSound.push_back( pSound );
}

void CSoundMgr::Set_EFM_Volume(float nVol)
{
}


