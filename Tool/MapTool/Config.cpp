
#include "stdafx.h"
#include "Config.h"

cConfig			g_Config;


#define SIZE_SET	( s_dwStructSize = sizeof( *this ) - sizeof( sSTRUCT_HEADER ) )

void cConfig::sHEADER::Reset()
{
	SIZE_SET;

	s_dwVersion = CONFIG_VERSION;
	s_dwStructCount = CONFIG_STRUCT_COUNT;
}

void cConfig::sKEYSET::Reset()
{
	SIZE_SET;

	s_KeyInfo[ KS_OBJ_DEL ].s_btKey = 'D';

	s_KeyInfo[ KS_OBJ_DETACH ].s_btKey = 'X';
	s_KeyInfo[ KS_OBJ_DETACH ].s_dwPlag = KP_CONTROL;

	s_KeyInfo[ KS_OBJ_COPY ].s_btKey = 'C';
	s_KeyInfo[ KS_OBJ_COPY ].s_dwPlag = KP_CONTROL;

	s_KeyInfo[ KS_OBJ_P_ROT ].s_btKey = 'M';

	s_KeyInfo[ KS_OBJ_M_ROT ].s_btKey = 'B';

	s_KeyInfo[ KS_OBJ_INIT_ROT ].s_btKey = 'N';

	s_KeyInfo[ KS_OBJ_RENDERING ].s_btKey = 'O';

	s_KeyInfo[ KS_TER_NODE_CHILD ].s_btKey = 'C';

	s_KeyInfo[ KS_TER_NODE_PARENT ].s_btKey = 'V';

	s_KeyInfo[ KS_RB_UNDO ].s_btKey = 'Z';
	s_KeyInfo[ KS_RB_UNDO ].s_dwPlag = KP_CONTROL;

	s_KeyInfo[ KS_RB_REDO ].s_btKey = 'Y';
	s_KeyInfo[ KS_RB_REDO ].s_dwPlag = KP_CONTROL;

	s_KeyInfo[ KS_TAB_SEL1 ].s_btKey = VK_F1;
	s_KeyInfo[ KS_SLIDE_B_LEFT ].s_btKey = VK_NUMPAD1;
	s_KeyInfo[ KS_SLIDE_B_RIGHT ].s_btKey = VK_NUMPAD3;
	s_KeyInfo[ KS_SLIDE_S_LEFT ].s_btKey = VK_NUMPAD2;
	s_KeyInfo[ KS_SLIDE_S_RIGHT ].s_btKey = VK_NUMPAD5;

	s_KeyInfo[ KS_FILE_SAVEAS ].s_btKey = 'S';
	s_KeyInfo[ KS_FILE_SAVEAS ].s_dwPlag = KP_ALT;

	s_KeyInfo[ KS_FILE_LOADAS ].s_btKey = 'L';
	s_KeyInfo[ KS_FILE_LOADAS ].s_dwPlag = KP_ALT;

	s_KeyInfo[ KS_OBJ_SAVE1 ].s_btKey = '1';
	s_KeyInfo[ KS_OBJ_SAVE1 ].s_dwPlag = KP_CONTROL;
	s_KeyInfo[ KS_OBJ_SAVE2 ].s_btKey = '2';
	s_KeyInfo[ KS_OBJ_SAVE2 ].s_dwPlag = KP_CONTROL;
	s_KeyInfo[ KS_OBJ_SAVE3 ].s_btKey = '3';
	s_KeyInfo[ KS_OBJ_SAVE3 ].s_dwPlag = KP_CONTROL;
	s_KeyInfo[ KS_OBJ_SAVE4 ].s_btKey = '4';
	s_KeyInfo[ KS_OBJ_SAVE4 ].s_dwPlag = KP_CONTROL;
	s_KeyInfo[ KS_OBJ_SAVE5 ].s_btKey = '5';
	s_KeyInfo[ KS_OBJ_SAVE5 ].s_dwPlag = KP_CONTROL;
	s_KeyInfo[ KS_OBJ_SAVE6 ].s_btKey = '6';
	s_KeyInfo[ KS_OBJ_SAVE6 ].s_dwPlag = KP_CONTROL;
	s_KeyInfo[ KS_OBJ_SAVE7 ].s_btKey = '7';
	s_KeyInfo[ KS_OBJ_SAVE7 ].s_dwPlag = KP_CONTROL;
	s_KeyInfo[ KS_OBJ_SAVE8 ].s_btKey = '8';
	s_KeyInfo[ KS_OBJ_SAVE8 ].s_dwPlag = KP_CONTROL;
	s_KeyInfo[ KS_OBJ_SAVE9 ].s_btKey = '9';
	s_KeyInfo[ KS_OBJ_SAVE9 ].s_dwPlag = KP_CONTROL;
	s_KeyInfo[ KS_OBJ_SAVE10 ].s_btKey = '0';
	s_KeyInfo[ KS_OBJ_SAVE10 ].s_dwPlag = KP_CONTROL;
	s_KeyInfo[ KS_OBJ_LOAD1 ].s_btKey = '1';
	s_KeyInfo[ KS_OBJ_LOAD2 ].s_btKey = '2';
	s_KeyInfo[ KS_OBJ_LOAD3 ].s_btKey = '3';
	s_KeyInfo[ KS_OBJ_LOAD4 ].s_btKey = '4';
	s_KeyInfo[ KS_OBJ_LOAD5 ].s_btKey = '5';
	s_KeyInfo[ KS_OBJ_LOAD6 ].s_btKey = '6';
	s_KeyInfo[ KS_OBJ_LOAD7 ].s_btKey = '7';
	s_KeyInfo[ KS_OBJ_LOAD8 ].s_btKey = '8';
	s_KeyInfo[ KS_OBJ_LOAD9 ].s_btKey = '9';
	s_KeyInfo[ KS_OBJ_LOAD10 ].s_btKey = '0';
	s_KeyInfo[ KS_OBJ_SELECTALL ].s_btKey = 'A';
	s_KeyInfo[ KS_OBJ_SELECTALL ].s_dwPlag = KP_CONTROL;
	s_KeyInfo[ KS_OBJ_TERRAIN_ATTACH ].s_btKey = 'H';
	s_KeyInfo[ KS_FULLSCREEN ].s_btKey = 'W';
	s_KeyInfo[ KS_FULLSCREEN ].s_dwPlag = KP_ALT;
}

void cConfig::sAUTOSAVE::Reset()
{
	SIZE_SET;

	s_bAutoSave = false;
	s_fAutoSaveTime = 0.0f;
	s_nSaveIndex = 0;
}

void cConfig::sROLLBACK::Reset()
{
	SIZE_SET;

	s_nRollBackCount = 50;
}

void cConfig::sENVIRONMENT::Reset()
{
	SIZE_SET;

	s_nSleepTick = 0;
	s_bWriteExcel = false;
	s_bWriteList = false;
	s_bWritePortal = false;
}

//==============================================================================================
//==============================================================================================
//==============================================================================================

void cConfig::Reset()
{
	m_Header.Reset();
	m_KeySet.Reset();
	m_AutoSave.Reset();
	m_RollBack.Reset();
	m_Environment.Reset();
}

void cConfig::Apply()
{	
}

void cConfig::Save()
{
	TCHAR szFolder[ MAX_PATH ] = {0, };
	nsCSFILE::GetFilePathOnly( szFolder, MAX_PATH, CONFIG_PATH );
	// 폴더가 존재 하지 않는다면 폴더 생성
	nsCSFILE::CreateDirectory( szFolder );

	assert_cs( m_Header.s_dwVersion == CONFIG_VERSION );
	m_Header.s_dwStructCount = CONFIG_STRUCT_COUNT;

	FILE* fp = NULL;
	_tfopen_s( &fp, CONFIG_PATH, L"wb" );

	fwrite( &m_Header, sizeof( sHEADER ), 1, fp );
	fwrite( &m_KeySet, sizeof( sKEYSET ), 1, fp );
	fwrite( &m_AutoSave, sizeof( sAUTOSAVE ), 1, fp );
	m_RollBack.s_nRollBackCount = 50;
	fwrite( &m_RollBack, sizeof( sROLLBACK ), 1, fp );
	fwrite( &m_Environment, sizeof( sENVIRONMENT ), 1, fp );

	fclose( fp );
}

void cConfig::Load()
{
	Reset();
	if( _taccess_s( CONFIG_PATH, 0 ) != 0 )
	{
		return;
	}

	// 파일이 존재 한다면 값을 읽어 온다
	FILE* fp = NULL;
	_tfopen_s( &fp, CONFIG_PATH, L"rb" );

	
#define READ_CONFIG_FILE( ClassMember )	{\
	if( --nStructCount == 0 )\
	{\
		fclose( fp );\
		return;\
	}\
	fread( &sh, sizeof( sSTRUCT_HEADER ), 1, fp );\
	fread( ( (BYTE*)(&##ClassMember ) ) + sizeof( sSTRUCT_HEADER ), sh.s_dwStructSize, 1, fp );\
}

	sSTRUCT_HEADER sh;
	int nStructCount = CONFIG_STRUCT_COUNT;
	READ_CONFIG_FILE( m_Header );
	nStructCount = m_Header.s_dwStructCount;
	// 버젼 체크
	if( m_Header.s_dwVersion != CONFIG_VERSION )
	{
		MessageBox( NULL, L"저장된 콘피그 파일은 예전버젼 이므로\n제거 하고 새로 만들겠습니다.", L"Config", MB_OK );
		m_Header.Reset();
		fclose( fp );
		return;
	}
	
	READ_CONFIG_FILE( m_KeySet );
	READ_CONFIG_FILE( m_AutoSave );
	READ_CONFIG_FILE( m_RollBack );
	m_RollBack.s_nRollBackCount = 50;
	READ_CONFIG_FILE( m_Environment );
	fclose( fp );
}