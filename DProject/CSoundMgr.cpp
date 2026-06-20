#include "stdafx.h"
#include "CSoundMgr.h"

CSoundMgr* g_pSoundMgr = NULL;

CSoundMgr::CSoundMgr()
{
	m_pMusicAmbient		=	NULL;
	m_pMusicOldAmbient	=	NULL;
	m_pmapSound			=	NULL;
	m_bActiveFocus		=	true;
}

CSoundMgr::~CSoundMgr()
{
}

void CSoundMgr::GlobalInit()
{
	assert_cs( g_pSoundMgr == NULL );
	g_pSoundMgr = NiNew CSoundMgr;
	g_pSoundMgr->Init();

	nsCsGBTerrain::g_pCsSoundMng = g_pSoundMgr;
}

void CSoundMgr::GlobalShotDown()
{
	if( g_pSoundMgr != NULL )
		g_pSoundMgr->Destroy();
	SAFE_NIDELETE( g_pSoundMgr );
}

void CSoundMgr::Destroy()
{
	if( m_pmapSound != NULL )
	{
		CSound* pSound = NULL;
		NiTMapIterator itObj = m_pmapSound->GetFirstPos();
		while( itObj != NULL )
		{
			const char* pcName;
			m_pmapSound->GetNext( itObj, pcName, pSound );
			SAFE_NIDELETE( pSound );
		}
		m_pmapSound->RemoveAll();
		SAFE_NIDELETE( m_pmapSound );
	}

	SAFE_NIDELETE( m_pMusicAmbient );
	SAFE_NIDELETE( m_pMusicOldAmbient );

	std::list< CSound* >::iterator it = m_listCloneSound.begin();
	std::list< CSound* >::iterator itEnd = m_listCloneSound.end();
	for( ; it!=itEnd; ++it )
		NISAFE_DELETE(*it);
	m_listCloneSound.clear();

	std::list< CSound* >::iterator it2 = m_listCloneSound_Move.begin();
	std::list< CSound* >::iterator it2End = m_listCloneSound_Move.end();
	for( ; it2!=it2End; ++it2 )
		NISAFE_DELETE(*it2);
	m_listCloneSound_Move.clear();

	std::list< CSound* >::iterator it3 = m_listCloneSound_System.begin();
	std::list< CSound* >::iterator it3End = m_listCloneSound_System.end();
	for( ; it3!=it3End; ++it3 )
		NISAFE_DELETE(*it3);
	m_listCloneSound_System.clear();

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

	pkSS->SetHWnd(GAMEAPP_ST.GetHWnd());
	if (!pkSS->Startup("Data\\NP\\Sound") )
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
	pkSS->SetUnitsPerMeter( 1.0f );

	m_bEnableSound = true;
	m_bDeleteMusic = false;

	m_fMusicVolume = g_pResist->GetMusic();
	m_fSoundVolume = g_pResist->GetSound();
	m_fSystemVolume = g_pResist->GetSystemSound();

	assert_cs( m_pmapSound == NULL );
	m_pmapSound = NiNew NiTStringPointerMap< CSound* >;

	// 1분 30초마다 한번씩 리소스 쓰인지 오래된거 검사
	m_CheckSoundResTimeSeq.SetDeltaTime( 90*1000 );
	m_CheckSoundResTimeSeq.OffReset();
}

void CSoundMgr::Update(float fTime)
{
	if( m_bEnableSound == false )
		return;

	if( g_pServerMoveOwner )
	{
		NiMatrix3 mat;
		mat.MakeZRotation( g_pServerMoveOwner->GetParent()->GetCurRot() + NI_PI );
		GetListener()->SetWorldTranslate( g_pServerMoveOwner->GetParent()->GetPos() );
		GetListener()->SetWorldRotate( mat );
	}
	else
	{
		GetListener()->SetWorldTranslate( CAMERA_ST.GetCameraObj()->GetWorldTranslate() );
	}

	NiAudioSystem::GetAudioSystem()->Update( fTime, true );

	if( m_pMusicAmbient )
	{
		if( ( m_bDeleteMusic == true )&&( m_pMusicAmbient->GetVolume() == 0.0f ) )
		{
			m_pMusicAmbient->Stop();
			SAFE_NIDELETE( m_pMusicAmbient );
			m_bDeleteMusic = false;
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

	std::list< CSound* >::iterator it = m_listCloneSound.begin();
	std::list< CSound* >::iterator itEnd = m_listCloneSound.end();		
	while( it!=itEnd )
	{
		if( (*it)->GetStatus() == NiAudioSource::DONE )
		{
			NISAFE_DELETE(*it);
			it = m_listCloneSound.erase( it );
		}
		else
		{
			++it;
		}
	}

	std::list< CSound* >::iterator it2 = m_listCloneSound_Move.begin();
	std::list< CSound* >::iterator it2End = m_listCloneSound_Move.end();		
	while( it2!=it2End )
	{
		if( (*it2)->GetStatus() == NiAudioSource::DONE )
		{
			NISAFE_DELETE(*it2);
			it2 = m_listCloneSound_Move.erase( it2 );
		}
		else
		{
			++it2;
		}
	}

	std::list< CSound* >::iterator it3 = m_listCloneSound_System.begin();
	std::list< CSound* >::iterator it3End = m_listCloneSound_System.end();		
	while( it3!=it3End )
	{
		if( (*it3)->GetStatus() == NiAudioSource::DONE )
		{
			NISAFE_DELETE(*it3);
			it3 = m_listCloneSound_System.erase( it3 );
		}
		else
		{
			++it3;
		}
	}

	if( m_CheckSoundResTimeSeq.IsEnable() )
	{
		// 시작한지 5분 지나야만 체크 시작
		if( fTime > 300.0f )
		{
			CSound* pSound = NULL;
			NiTMapIterator itObj = m_pmapSound->GetFirstPos();
			while( itObj != NULL )
			{
				const char* pcName;
				m_pmapSound->GetNext( itObj, pcName, pSound );
				if( pSound->GetLastConnetFileTime() < fTime - 300.0f )
				{					
					m_pmapSound->RemoveAt( pcName );
					SAFE_NIDELETE( pSound );
				}
			}
		}
	}
}

void CSoundMgr::OnActive()
{
	m_bActiveFocus = true;

	if( m_bEnableSound == false )
		return;	

	if( ( m_pMusicAmbient )&&( m_pMusicAmbient->IsSource() == true ) )
		m_pMusicAmbient->SetVolume( m_fMusicVolume );
	if( ( m_pMusicOldAmbient )&&( m_pMusicOldAmbient->IsSource() == true ) )
		m_pMusicOldAmbient->SetVolume( m_fMusicVolume );
}

void CSoundMgr::OnInactive()
{
	m_bActiveFocus = false;

	if( m_bEnableSound == false )
		return;

	if( ( m_pMusicAmbient )&&( m_pMusicAmbient->IsSource() == true ) )
		m_pMusicAmbient->SetVolume( 0 );
	if( ( m_pMusicOldAmbient )&&( m_pMusicOldAmbient->IsSource() == true ) )
		m_pMusicOldAmbient->SetVolume( 0 );
}



//==========================================================================================================
//
//		배경음
//
//==========================================================================================================

void CSoundMgr::PlayMusic( char const* cMusic, float fFadeTime /*=1.5f*/ )
{
	SAFE_POINTER_RET( cMusic );

	if( m_bEnableSound == false )
		return;

	if( g_pResist == NULL )
		return;

	if( g_pResist->IsMusic() == false )
		return;

	// 기존 사운드와 같지 않다면
	if( ( m_pMusicAmbient == NULL )||( m_pMusicAmbient->IsSameMusic( cMusic ) == false ) )
	{
		// 사운드 제거
		if( m_pMusicAmbient != NULL )
		{
			if( m_pMusicOldAmbient != NULL )
			{
				m_pMusicOldAmbient->Stop();
				SAFE_NIDELETE( m_pMusicOldAmbient );
			}

			m_pMusicOldAmbient = m_pMusicAmbient;
			m_pMusicOldAmbient->FadeToVolume( 0.0f, (float)g_fAccumTime, fFadeTime );
			m_pMusicAmbient = NULL;
		}
		// 사운드 로드
		if( cMusic[ 0 ] != NULL )
		{
			m_pMusicAmbient = NiNew CSound;
			m_pMusicAmbient->Load( "Data\\Music", cMusic, NiAudioSource::TYPE_AMBIENT, true, true, 0.0f, 0.0f );


			m_pMusicAmbient->FadeToVolume( m_bActiveFocus ? m_fMusicVolume : 0, (float)g_fAccumTime, fFadeTime );
			m_pMusicAmbient->Play();
		}
	}
}

void CSoundMgr::DeleteMusic( bool bJustDelete )
{
	if( m_bEnableSound == false )
		return;

	if( m_pMusicAmbient != NULL )
	{
		if( bJustDelete == false )
		{
			m_pMusicAmbient->FadeToVolume( 0.0f, (float)g_fAccumTime, 0.8f );
			m_bDeleteMusic = true;
		}
		else
		{
			m_pMusicAmbient->Stop();
			SAFE_NIDELETE( m_pMusicAmbient );
			m_bDeleteMusic = false;
		}
	}
	if( m_pMusicOldAmbient != NULL )
	{
		if( bJustDelete == false )
		{
			m_pMusicOldAmbient->FadeToVolume( 0.0f, (float)g_fAccumTime, 0.8f );
		}
		else
		{
			m_pMusicOldAmbient->Stop();
			SAFE_NIDELETE( m_pMusicOldAmbient );
		}
	}
}

void CSoundMgr::Set_BGM_Volume( float nVol )
{
	if( m_bEnableSound == false )
		return;

	m_fMusicVolume = nVol;

	if(m_pMusicAmbient)
	{
		m_pMusicAmbient->SetVolume( m_bActiveFocus ? m_fMusicVolume : 0 );
	}
	if( m_pMusicOldAmbient )
	{
		m_pMusicOldAmbient->SetVolume( m_bActiveFocus ? m_fMusicVolume : 0 );
	}
}

void CSoundMgr::Set_BGM_FadeVolume( float nVol )
{
	if( m_bEnableSound == false )
		return;

	m_fMusicVolume = nVol;

	if(m_pMusicAmbient)
	{
		m_pMusicAmbient->FadeToVolume( m_bActiveFocus ? m_fMusicVolume : 0, (float)g_fAccumTime, 1.6f );
	}
	if( m_pMusicOldAmbient )
	{
		m_pMusicOldAmbient->FadeToVolume( m_bActiveFocus ? m_fMusicVolume : 0, (float)g_fAccumTime, 1.6f );
	}
}

//==========================================================================================================
//
//		효과음
//
//==========================================================================================================

void CSoundMgr::Update_MoveSound( void* pTarget, NiPoint3 vPos )
{
	if( g_pResist->m_Global.s_bEnableSound_3DMove == false )
		return;

	std::list< CSound* >::iterator it = m_listCloneSound_Move.begin();
	std::list< CSound* >::iterator itEnd = m_listCloneSound_Move.end();		
	while( it!=itEnd )
	{		
		assert_cs( (*it)->GetTarget() != NULL );
		if( (CsC_AvObject*)pTarget == (*it)->GetTarget() )
		{
			(*it)->SetPos( vPos );
		}
		++it;		
	}
}

void* CSoundMgr::PlaySound( char const* cSound, NiPoint3 vPos, CsC_AvObject* pTarget )
{
	if( m_bEnableSound == false )
		return NULL;

	if( g_pResist == NULL )
		return NULL;

	if( g_pResist->IsSound() == false )
		return NULL;

	CSound* pSound = GetSound( cSound, true );
	pSound->SetTarget( pTarget );
	pSound->SetVolume( m_bActiveFocus ? m_fSoundVolume : 0 );
	pSound->SetPos( vPos );
	pSound->Play();
	m_listCloneSound_Move.push_back( pSound );

	return pSound;
}

void* CSoundMgr::PlaySound( char const* cSound, NiPoint3 vPos, float fMinDist, float fMaxDist )
{
	if( m_bEnableSound == false )
		return NULL;

	if( g_pResist == NULL )
		return NULL;

	if( g_pResist->IsSound() == false )
		return NULL;

	CSound* pSound = GetSound( cSound, true, fMinDist, fMaxDist );
	pSound->SetVolume( m_bActiveFocus ? m_fSoundVolume : 0 );
	pSound->SetPos( vPos );
	pSound->Play();
	m_listCloneSound.push_back( pSound );

	return pSound;
}

void CSoundMgr::PlaySound( char const* szSound )
{
	if( m_bEnableSound == false )
		return;

	if( g_pResist == NULL )
		return;

	if( g_pResist->IsSound() == false )
		return;

	CSound* pSound = GetSound( szSound, false );
	pSound->SetVolume( m_bActiveFocus ? m_fSoundVolume : 0 );
	pSound->Play();
	m_listCloneSound.push_back( pSound );
}


void* CSoundMgr::PlayStaticSound( char const* cSound, NiPoint3 vPos )
{
	if( m_bEnableSound == false )
		return NULL;

	if( g_pResist == NULL )
		return NULL;

	if( g_pResist->IsSound() == false )
		return NULL;

	CSound* pSound = GetStaticSound( cSound, true );
	pSound->SetVolume( m_bActiveFocus ? m_fSoundVolume : 0 );
	pSound->SetPos( vPos );
	pSound->Play();
	m_listCloneSound.push_back( pSound );

	return pSound;
}

bool CSoundMgr::IsSound( CSound* pSound )
{
	if( m_bEnableSound == false )
		return false;

	std::list< CSound* >::iterator it = m_listCloneSound.begin();
	std::list< CSound* >::iterator itEnd = m_listCloneSound.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it) == pSound )
		{
			return true;
		}
	}

	std::list< CSound* >::iterator it2 = m_listCloneSound_Move.begin();
	std::list< CSound* >::iterator it2End = m_listCloneSound_Move.end();
	for( ; it2!=it2End; ++it2 )
	{
		if( (*it2) == pSound )
		{
			return true;
		}
	}

	std::list< CSound* >::iterator it3 = m_listCloneSound_System.begin();
	std::list< CSound* >::iterator it3End = m_listCloneSound_System.end();
	for( ; it3!=it3End; ++it3 )
	{
		if( (*it3) == pSound )
		{
			return true;
		}
	}
	return false;
}

void CSoundMgr::Play_Stop_Sound()
{
	std::list< CSound* >::iterator it = m_listCloneSound.begin();
	std::list< CSound* >::iterator itEnd = m_listCloneSound.end();		
	for( ; it!=itEnd; ++it )
	{
		(*it)->Stop();
	}

	std::list< CSound* >::iterator it2 = m_listCloneSound_Move.begin();
	std::list< CSound* >::iterator it2End = m_listCloneSound_Move.end();		
	for( ; it2!=it2End; ++it2 )
	{
		(*it2)->Stop();
	}
}

void CSoundMgr::StopSound( CSound* pSound )
{
	if( m_bEnableSound == false )
		return;

	if( IsSound( pSound ) == true )
		pSound->Stop();
}

void CSoundMgr::Set_EFM_Volume(float nVol)
{
	m_fSoundVolume = nVol;
}

//=====================================================================================
//
//	System
//
//=====================================================================================

CSound* CSoundMgr::PlaySystemSound( char const* cSound )
{
	if( m_bEnableSound == false )
		return NULL;

	if( g_pResist == NULL )
		return NULL;

	if( g_pResist->IsSystemSound() == false )
		return NULL;

	CSound* pSound = GetSound( cSound, false );
	pSound->SetVolume( m_bActiveFocus ? m_fSystemVolume : 0 );
	pSound->Play();
	m_listCloneSound_System.push_back( pSound );
	return pSound;
}

void CSoundMgr::PlaySystemSound( char const* cSound, float& fLength )
{		
	if( m_bEnableSound == false )
		return;

	if( g_pResist == NULL )
		return;

	if( g_pResist->IsSystemSound() == false )
		return;

	CSound* pSound = GetSound( cSound, false );
	pSound->SetVolume( m_bActiveFocus ? m_fSystemVolume : 0 );
	pSound->Play();

	pSound->GetPlayLength( fLength );
	m_listCloneSound_System.push_back( pSound );
}

CSound* CSoundMgr::PlaySystemSound( char const* cSound, NiPoint3 vPos, CsC_AvObject* pTarget )
{		
	if( m_bEnableSound == false )
		return NULL;

	if( g_pResist == NULL )
		return NULL;

	if( g_pResist->IsSystemSound() == false )
		return NULL;


	CSound* pSound = GetSound( cSound, true );
	pSound->SetTarget( pTarget );
	pSound->SetVolume( m_bActiveFocus ? m_fSystemVolume : 0 );
	pSound->SetPos( vPos );
	pSound->Play();
	m_listCloneSound_System.push_back( pSound );

	return pSound;
}

CSound* CSoundMgr::PlayOtherSystemSound( char const* cSound )
{
	if( m_bEnableSound == false )
		return NULL;

	if( g_pResist == NULL )
		return NULL;

	if( g_pResist->IsSystemSound() == false )
		return NULL;

	CSound* pSound = GetOtherSound( cSound, false );
	pSound->SetVolume( m_bActiveFocus ? m_fSystemVolume : 0 );
	pSound->Play();
	m_listCloneSound_System.push_back( pSound );
	return pSound;
}

void CSoundMgr::Set_System_Volume( float nVol)
{
	m_fSystemVolume = nVol;
}

void CSoundMgr::Play_Stop_SystemSound()
{
	std::list< CSound* >::iterator it3 = m_listCloneSound_System.begin();
	std::list< CSound* >::iterator it3End = m_listCloneSound_System.end();		
	for( ; it3!=it3End; ++it3 )
	{
		(*it3)->Stop();
	}
}

//=====================================================================================
//
//	Sound
//
//=====================================================================================
CSound* CSoundMgr::GetSound( char const* cSound, bool b3DSound, float fMinDist /*=300.0f*/, float fMaxDist /*=4000.0f*/ )
{
	CSound* pSound = NULL;
	bool bFind = m_pmapSound->GetAt( cSound, pSound );

	if( bFind == false )
	{
		pSound = NiNew CSound;
		pSound->Load( "Data\\Sound", cSound,
			b3DSound ? NiAudioSource::TYPE_3D : NiAudioSource::TYPE_AMBIENT,
			false,
			false,
			fMinDist,
			fMaxDist );

		m_pmapSound->SetAt( cSound, pSound );
	}
	return pSound->Clone();
}

CSound* CSoundMgr::GetOtherSound( char const* cSound, bool b3DSound, float fMinDist /*=300.0f*/, float fMaxDist /*=4000.0f*/ )
{
	CSound* pSound = NULL;
	bool bFind = m_pmapSound->GetAt( cSound, pSound );

	if( bFind == false )
	{
		pSound = NiNew CSound;
		pSound->Load( cSound, NULL,
			b3DSound ? NiAudioSource::TYPE_3D : NiAudioSource::TYPE_AMBIENT,
			false,
			false,
			fMinDist,
			fMaxDist );

		m_pmapSound->SetAt( cSound, pSound );
	}
	return pSound->Clone();
}

CSound* CSoundMgr::GetStaticSound( char const* cSound, bool b3DSound, float fMinDist /*=300.0f*/, float fMaxDist /*=4000.0f*/ )
{
	CSound* pSound = NULL;
	bool bFind = m_pmapSound->GetAt( cSound, pSound );

	if( bFind == false )
	{
		pSound = NiNew CSound;
		pSound->Load( "Data\\StaticSound", cSound,
			b3DSound ? NiAudioSource::TYPE_3D : NiAudioSource::TYPE_AMBIENT,
			false,
			false,
			fMinDist,
			fMaxDist );

		m_pmapSound->SetAt( cSound, pSound );
	}
	return pSound->Clone();
}


