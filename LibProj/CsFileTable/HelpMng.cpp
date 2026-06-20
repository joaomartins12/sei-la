
#include "stdafx.h"
#include "HelpMng.h"

CsHelpMng::CsHelpMng()
{
	m_nTitleUniqID = 0;
}

void CsHelpMng::Delete()
{
	int nCount = m_vpData.Size();
	for( int i=0; i<nCount; ++i )
	{
		m_vpData[ i ]->Delete();
		delete m_vpData[ i ];
	}
	m_vpData.Destroy();
	m_vpCheck_TLevel.Destroy();
	m_vpCheck_PartnerDLevel.Destroy();
	m_vpCheck_MapArrive.Destroy();
}

bool CsHelpMng::Init()
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		Load();
		break;
	case nsCsFileTable::FT_BIN:		
		if( Load() == false )
			return false;
		break;
	case nsCsFileTable::FT_FILEPACK:
		_LoadFilePack();
		break;
	default:
		assert_cs( false );
	}
	return true;
}

bool CsHelpMng::IsTitleID( int nUniqID )
{
	int nCount = m_vpData.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpData[ i ]->GetTitle()->s_nUniqID == nUniqID )
			return true;
	}
	return false;
}

CsHelp* CsHelpMng::GetData( int nUniqID )
{
	int nCount = m_vpData.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpData[ i ]->GetTitle()->s_nUniqID == nUniqID )
			return m_vpData[ i ];
	}
	return NULL;
}

void CsHelpMng::Save()
{
	if( _access_s( nsCsFileTable::GetPath_OT_Help(), 0 ) == 0 )
	{
		if( nsCsFileTable::g_eLanguage != nsCsFileTable::ENGLISH &&
			nsCsFileTable::g_eLanguage != nsCsFileTable::ENGLISH_A && 
			nsCsFileTable::g_eLanguage != nsCsFileTable::THAILAND  &&
			nsCsFileTable::g_eLanguage != nsCsFileTable::TAIWAN &&
			nsCsFileTable::g_eLanguage != nsCsFileTable::HONGKONG )
		{
			// 폴더가 존재 하지 않는다면 폴더 생성
			nsCSFILE::CreateDirectory( _T( ".\\GameData\\Korea\\Help_Backup" ) );

			char cNewFile[ MAX_PATH ];
			SYSTEMTIME st;
			GetLocalTime( &st );
			sprintf_s( cNewFile, MAX_PATH, ".\\GameData\\Korea\\Help_Backup\\Help_%.2d%.2d%.2d_%.2d%.2d%.2d.bin", st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
			// 이미 파일이 존재 한다면
			if( _access_s( cNewFile, 0 ) == 0 )
				DeleteFileA( cNewFile );
			rename( nsCsFileTable::GetPath_OT_Help(), cNewFile );
		}
		else
		{
			// 폴더가 존재 하지 않는다면 폴더 생성
			nsCSFILE::CreateDirectory( _T( ".\\GameData\\English\\Help_Backup" ) );

			char cNewFile[ MAX_PATH ];
			SYSTEMTIME st;
			GetLocalTime( &st );
			sprintf_s( cNewFile, MAX_PATH, ".\\GameData\\English\\Help_Backup\\Help_%.2d%.2d%.2d_%.2d%.2d%.2d.bin", st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
			// 이미 파일이 존재 한다면
			if( _access_s( cNewFile, 0 ) == 0 )
				DeleteFileA( cNewFile );
			rename( nsCsFileTable::GetPath_OT_Help(), cNewFile );
		}
	}

	FILE *fp = NULL;
	fopen_s( &fp, nsCsFileTable::GetPath_OT_Help(), "wb" );
	assert_cs( fp != NULL );
	fwrite( &m_nTitleUniqID, sizeof( UINT ), 1, fp );

	int nCount = m_vpData.Size();
	fwrite( &nCount, sizeof( int ), 1, fp );
	for( int i=0; i<nCount; ++i )
	{
		m_vpData[ i ]->Save( fp );
	}
	fclose( fp );
}

bool CsHelpMng::Load()
{
	FILE *fp = NULL;
	fopen_s( &fp, nsCsFileTable::GetPath_OT_Help(), "rb" );
	if( fp == NULL )
	{
		fopen_s( &fp, "Data\\Interface\\Help\\Help.bin", "rb" );
		if( fp == NULL )
			return false;
	}		

	fread( &m_nTitleUniqID, sizeof( UINT ), 1, fp );
	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );
	for( int i=0; i<nCount; ++i )
	{
		CsHelp* pNew = csnew CsHelp;
		pNew->Load( fp );
		m_vpData.PushBack( pNew );

		switch( pNew->GetTitle()->s_eReqType )
		{
		case CsHelp::RT_NONE:															break;
		case CsHelp::RT_TLEVEL:				m_vpCheck_TLevel.PushBack( pNew );			break;
		case CsHelp::RT_PARTNERMON_LEVEL:	m_vpCheck_PartnerDLevel.PushBack( pNew );	break;
		case CsHelp::RT_MAP_ARRIVE:			m_vpCheck_MapArrive.PushBack( pNew );		break;
		default:							assert_cs( false );
		}
	}

	fclose( fp );
	return true;
}

void CsHelpMng::_LoadFilePack()
{
	if( !CsFPS::CsFPSystem::IsExistOnlyPack( FT_RESOURCE_HANDLE, nsCsFileTable::GetPath_OT_Help() ) )
	{
		Load();
		return;
	}

	// HelpBin 만 예외적으로 0번 인덱스에서 읽어 오겠다.

	if( !CsFPS::CsFPSystem::SeekLock( FT_RESOURCE_HANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_RESOURCE_HANDLE, nsCsFileTable::GetPath_OT_Help() );

	_read( nHandle, &m_nTitleUniqID, sizeof( UINT ) );
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );
	for( int i=0; i<nCount; ++i )
	{
		CsHelp* pNew = csnew CsHelp;
		pNew->LoadFilePack( nHandle );
		m_vpData.PushBack( pNew );

		switch( pNew->GetTitle()->s_eReqType )
		{
		case CsHelp::RT_NONE:															break;
		case CsHelp::RT_TLEVEL:				m_vpCheck_TLevel.PushBack( pNew );			break;
		case CsHelp::RT_PARTNERMON_LEVEL:	m_vpCheck_PartnerDLevel.PushBack( pNew );	break;
		case CsHelp::RT_MAP_ARRIVE:			m_vpCheck_MapArrive.PushBack( pNew );		break;
		default:							assert_cs( false );
		}
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_RESOURCE_HANDLE );
}

void CsHelpMng::ToEng()
{
	int nSize = m_vpData.Size();
	for( int i=0; i<nSize; ++i )
	{
		m_vpData[ i ]->ToEng();
	}
}

CsHelp* CsHelpMng::AddData()
{
	CsHelp* pNew = csnew CsHelp;
	pNew->GetTitle()->s_nUniqID = GetUniqID();
	m_vpData.PushBack( pNew );
	return pNew;
}

void CsHelpMng::DelData( CsHelp* pDel )
{
	int nCount = m_vpData.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( pDel == m_vpData[ i ] )
		{
			m_vpData[ i ]->Delete();
			delete m_vpData[ i ];
			m_vpData.DeleteElement( i );
			return;
		}
	}
	assert_cs( false );
}

void CsHelpMng::SwapUp( CsHelp* pSrc )
{
	int nCount = m_vpData.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( pSrc == m_vpData[ i ] )
		{
			assert_cs( i != 0 );
			m_vpData[ i ] = m_vpData[ i - 1 ];
			m_vpData[ i - 1 ] = pSrc;
			return;
		}
	}
	assert_cs( false );
}

void CsHelpMng::SwapDown( CsHelp* pSrc )
{
	int nCount = m_vpData.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( pSrc == m_vpData[ i ] )
		{
			assert_cs( i != nCount - 1 );
			m_vpData[ i ] = m_vpData[ i + 1 ];
			m_vpData[ i + 1 ] = pSrc;
			return;
		}
	}
	assert_cs( false );
}