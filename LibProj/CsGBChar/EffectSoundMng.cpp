#include "StdAfx.h"
#include "EffectSoundMng.h"

CsEffectSoundMng*		g_EffectSoundMng = NULL;

#define EFFECTSOUND_FILENAME		_T( "Data\\EffectSound.dat" )

CsEffectSoundMng::CsEffectSoundMng(void)
{
}

CsEffectSoundMng::~CsEffectSoundMng(void)
{
}

void CsEffectSoundMng::Delete()
{
	std::map< size_t, CsEffectSound* >::iterator it = m_mapEffectSound.begin();
	std::map< size_t, CsEffectSound* >::iterator itEnd = m_mapEffectSound.end();
	for( ; it != itEnd ; it++ )
	{			
		SAFE_NIDELETE( it->second );
	}	
	m_mapEffectSound.clear();
}

void CsEffectSoundMng::Init()
{
	
	if( CsFPS::CsFPSystem::IsExistOnlyPack( 0, "Data\\EffectSound.dat" ) )
	{
		LoadFilePack();
	}
	else
	{
		LoadFile();
	}
}

void CsEffectSoundMng::SaveFile()
{
	if( m_mapEffectSound.empty() )
	{
		CsMessageBox( MB_OK, _T( "저장할게 없네요." ) );
		return;			
	}

	FILE* fp = NULL;
	_tfopen_s( &fp, EFFECTSOUND_FILENAME, _T( "wb" ) );

	int nCount = (int)m_mapEffectSound.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	std::map< size_t, CsEffectSound* >::iterator it = m_mapEffectSound.begin();
	std::map< size_t, CsEffectSound* >::iterator itEnd = m_mapEffectSound.end();
	for( ; it != itEnd; it++ )
	{	
		fwrite( &(it->second->m_sESInfo), sizeof( CsEffectSound::sES_Info), 1, fp );		
		fwrite( &(it->second->m_sEffectOptin), sizeof( CsEffectSound::sEffectOption ), 1, fp );

		std::list< CsEffectSound::sSound_Info* >::iterator it2 = it->second->m_listSound.begin();
		std::list< CsEffectSound::sSound_Info* >::iterator it2End = it->second->m_listSound.end();
		for(  ; it2 != it2End; it2++ )			
		{
			fwrite( *it2, sizeof( CsEffectSound::sSound_Info ), 1, fp );
		}
	}	
	
	fclose( fp );

	CsMessageBox( MB_OK, _T( "저장완료." ) );
}

void CsEffectSoundMng::LoadFile()
{	
	//파일 로드	
	if( !m_mapEffectSound.empty() )
	{
		m_mapEffectSound.clear();
	}

	if( _taccess_s( EFFECTSOUND_FILENAME, 0 ) != 0 )
		return;
	
	FILE* fp = NULL;

	_tfopen_s( &fp, EFFECTSOUND_FILENAME, _T( "rb" ) );
	assert_cs( fp != NULL );

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );	

	for( int i = 0; i < nCount; i++ )
	{
		CsEffectSound* cEs = NiNew CsEffectSound;
		
		fread( &cEs->m_sESInfo , sizeof( CsEffectSound::sES_Info ), 1, fp );
		fread( &cEs->m_sEffectOptin, sizeof( CsEffectSound::sEffectOption ), 1, fp );
				
		for( int j = 0 ; j < cEs->m_sESInfo.nSoundNum ; j++ )			
		{
			CsEffectSound::sSound_Info* s_Info = NiNew CsEffectSound::sSound_Info;
			fread( s_Info, sizeof( CsEffectSound::sSound_Info ), 1, fp );

			cEs->m_listSound.push_back( s_Info );
		}

		assert_cs( m_mapEffectSound.find( cEs->m_sESInfo.HashCode ) == m_mapEffectSound.end() );
		m_mapEffectSound[ cEs->m_sESInfo.HashCode ] = cEs;
	}	


	fclose( fp );
	
}

void CsEffectSoundMng::LoadFilePack()
{
	//파일 로드	
	assert_cs( m_mapEffectSound.empty() == true );

	if( !CsFPS::CsFPSystem::SeekLock(0) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( 0, "Data\\EffectSound.dat" );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	for( int i=0; i<nCount; ++i )
	{
		CsEffectSound* cEs = NiNew CsEffectSound;

		_read( nHandle, &cEs->m_sESInfo , sizeof( CsEffectSound::sES_Info ) );
		_read( nHandle, &cEs->m_sEffectOptin, sizeof( CsEffectSound::sEffectOption ) );

		for( int j = 0 ; j < cEs->m_sESInfo.nSoundNum ; j++ )			
		{
			CsEffectSound::sSound_Info* s_Info = NiNew CsEffectSound::sSound_Info;
			_read( nHandle, s_Info, sizeof( CsEffectSound::sSound_Info ) );

			cEs->m_listSound.push_back( s_Info );
		}

		assert_cs( m_mapEffectSound.find( cEs->m_sESInfo.HashCode ) == m_mapEffectSound.end() );
		m_mapEffectSound[ cEs->m_sESInfo.HashCode ] = cEs;
	}

	CsFPS::CsFPSystem::SeekUnLock(0);
}

void CsEffectSoundMng::InsertData( CsEffectSound* cs_EsData )
{
	if( m_mapEffectSound.size() == 0 )
		return;

	CsEffectSound* EsData;

	if( m_mapEffectSound.find( cs_EsData->m_sESInfo.HashCode ) != m_mapEffectSound.end() )
	{
		std::map< size_t, CsEffectSound* >::iterator it = m_mapEffectSound.find( cs_EsData->m_sESInfo.HashCode );
		EsData = it->second;

		/* assert_csm2( strcmp( m_mapEffectSound[ cs_EsData->m_sESInfo.HashCode ]->m_sESInfo.szFilePath, cs_EsData->m_sESInfo.szFilePath ) == 0,
			"%s, %s", m_mapEffectSound[ cs_EsData->m_sESInfo.HashCode ]->m_sESInfo.szFilePath, cs_EsData->m_sESInfo.szFilePath );
		
		SAFE_NIDELETE( m_mapEffectSound[ cs_EsData->m_sESInfo.HashCode ] );	*/			
	}
	else
	{
		EsData = NiNew CsEffectSound;			
	}

	memcpy( &EsData->m_sESInfo, &cs_EsData->m_sESInfo, sizeof( CsEffectSound::sES_Info ) );
	memcpy( &EsData->m_sEffectOptin, &cs_EsData->m_sEffectOptin, sizeof( CsEffectSound::sEffectOption ) );

	EsData->DeleteSoundList();
	std::list< CsEffectSound::sSound_Info* >::iterator it = cs_EsData->m_listSound.begin();
	std::list< CsEffectSound::sSound_Info* >::iterator itEnd = cs_EsData->m_listSound.end();
	for( ; it != itEnd; it++ )
	{
		CsEffectSound::sSound_Info* s_Info = NiNew CsEffectSound::sSound_Info;
		memcpy( s_Info, *it, sizeof( CsEffectSound::sSound_Info ) );			
		EsData->m_listSound.push_back( s_Info );
	}
	
	m_mapEffectSound[ EsData->m_sESInfo.HashCode ] = EsData;
}
