
#include "stdafx.h"
#include "Resist.h"

#define NPC_RESIST_PATH		L".\\ToolRgs\\NpcTool.rgs"

cResist	g_Resist;



void cResist::_Reset()
{
	m_WindowInfo.Reset();
	m_StrViewer.Reset();
}

void cResist::Save()
{
	// 폴더가 없다면 폴더 생성
	TCHAR szPath[ MAX_PATH ];
	nsCSFILE::GetFilePathOnly( szPath, MAX_PATH, NPC_RESIST_PATH );
	nsCSFILE::CreateDirectory( szPath );

	DWORD dwVersion = RESIST_VERSION;
	FILE* fp = NULL;
	_tfopen_s( &fp, NPC_RESIST_PATH, L"wb" );

	fwrite( &dwVersion, sizeof( DWORD ), 1, fp );
	fwrite( &m_WindowInfo, sizeof( sWINDOW_INFO ), 1, fp );
	fwrite( &m_StrViewer, sizeof( sSTR_VIEWER ), 1, fp );

	fclose( fp );
}

void cResist::Load()
{
	_Reset();

	if( _taccess_s( NPC_RESIST_PATH, 0 ) != 0 )
	{
		return;
	}

	FILE* fp = NULL;
	_tfopen_s( &fp, NPC_RESIST_PATH, L"rb" );

	DWORD dwVersion;
	fread( &dwVersion, sizeof( DWORD ), 1, fp );
	if( dwVersion != RESIST_VERSION )
	{
		MessageBox( NULL, L"저장된 레지스트 파일은 예전버젼 이므로\n제거 하고 새로 만들겠습니다.", L"Resist", MB_OK );
		fclose( fp );
		return;
	}
	fread( &m_WindowInfo, sizeof( sWINDOW_INFO ), 1, fp );
	fread( &m_StrViewer, sizeof( sSTR_VIEWER ), 1, fp );

	fclose( fp );
}