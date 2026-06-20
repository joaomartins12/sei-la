#include "stdafx.h"
#include "CSound.h"


//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CSound::Load( char const* path, char const* name, UINT type, bool loop, bool bMusic, float fMinDist, float fMaxDist )
{
	assert_cs( path != NULL );
	assert_cs( m_pkSound == NULL );

	char str[MAX_PATH];

	if( name != NULL )
	{
		if( name[ 0 ] != '.' )
			sprintf_s( str, MAX_PATH, "%s\\%s", path, name );
		else
			sprintf_s( str, MAX_PATH, "%s\\%s", path, &name[ 2 ] );
	}
	else
	{
		strcpy_s( str, MAX_PATH, path );
	}

	NiAudioSystem* pkSS = NiAudioSystem::GetAudioSystem();
    m_pkSound = pkSS->CreateSource( type );
    m_pkSound->SetFilename(str);
    m_pkSound->SetStreamed( true );
    m_pkSound->Load();

	if( type == NiAudioSource::TYPE_3D )
	{
		SetVolume( g_pSoundMgr->GetSoundVolume() );
		SetMinMaxDistance( fMinDist, fMaxDist );
		m_pkSound->SetSelectiveUpdate( true );
		m_pkSound->SetSelectiveUpdateTransforms(true);
		m_pkSound->SetSelectiveUpdatePropertyControllers(false);
		m_pkSound->SetSelectiveUpdateRigid(true);
	}
	else
	{
		SetVolume( bMusic ? 0.0f : g_pSoundMgr->GetSystemVolume() );
	}
	
	if( loop )
	{
		m_pkSound->SetLoopCount(NiAudioSource::LOOP_INFINITE);
	}
	else
	{
		m_pkSound->SetLoopCount(NiAudioSource::LOOP_SINGLE);
	}
	m_fConnetTime = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CSound::Stop()
{
	assert_cs( m_pkSound );
	m_pkSound->Stop();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CSound::Play()
{
	assert_cs( m_pkSound );
    m_pkSound->Play();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CSound::Rewind()
{
	assert_cs( m_pkSound );
    m_pkSound->Rewind();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CSound::SetPos( const NiPoint3 vPos )
{
	assert_cs( m_pkSound );
	m_pkSound->SetTranslate( vPos );	
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CSound::SetMinMaxDistance( float min, float max )
{
	assert_cs( m_pkSound );
	m_pkSound->SetMinMaxDistance( min, max );
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
CSound* CSound::Clone()
{
	ConnetFile();

	assert_cs( m_pkSound );
	CSound* pReturn = NiNew CSound( (NiAudioSource*)m_pkSound->Clone() );	
	return pReturn;
}