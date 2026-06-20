#include "stdafx.h"
#include "CSound.h"


//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CSound::Load( char* path, char* name, UINT type, bool loop, bool bMusic )
{
	assert_cs( path != NULL );
	assert_cs( m_pkSound == NULL );

	char str[MAX_PATH];
	sprintf_s( str, MAX_PATH, "%s/%s", path, name );
	assert_csm1( _access_s( str, 0 ) == 0, "음악 파일이 존재 하지 않습니다.\n%s", str );

	NiAudioSystem* pkSS = NiAudioSystem::GetAudioSystem();
    m_pkSound = pkSS->CreateSource( type );
    m_pkSound->SetFilename(str);
    m_pkSound->SetStreamed( true );
    m_pkSound->Load();

	if( type == NiAudioSource::TYPE_3D )
	{
		SetVolume( 1.0f );
		SetMinMaxDistance( 500.0f, 4000.f );
		m_pkSound->SetSelectiveUpdate( true );
		m_pkSound->SetSelectiveUpdateTransforms(true);
		m_pkSound->SetSelectiveUpdatePropertyControllers(false);
		m_pkSound->SetSelectiveUpdateRigid(true);
	}
	else
	{
		SetVolume( bMusic ? 1.0f : 1.0f );
	}

	if( loop )
	{
		m_pkSound->SetLoopCount(NiAudioSource::LOOP_INFINITE);
	}
	else
	{
		m_pkSound->SetLoopCount(NiAudioSource::LOOP_SINGLE);
	}
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
	//m_pkSound->Update(0.0f);	
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
	assert_cs( m_pkSound );

	CSound* pReturn = NiNew CSound( (NiAudioSource*)m_pkSound->Clone() );	
	return pReturn;
}