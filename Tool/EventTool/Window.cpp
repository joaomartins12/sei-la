
#include "stdafx.h"
#include "Window.h"


//=======================================================================================================================
//
//		static
//
//=======================================================================================================================

char	cWindow::m_cSound[ SD_MAX_COUNT ][ SYSTEM_SOUND_PATH_LEN ];
char	cWindow::m_cWorkingFolder[ MAX_PATH ] = { 0, };

void cWindow::GlobalInit()
{
	cWindow::SetWorkingFolder( "data\\interface\\" );

	strcpy_s( m_cSound[ SD_Ba1 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_bag_01.wav" );
	strcpy_s( m_cSound[ SD_Ba2 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_bag_02.wav" );

	strcpy_s( m_cSound[ SD_Bu1 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_button_01.wav" );
	strcpy_s( m_cSound[ SD_Bu2 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_button_02.wav" );
	strcpy_s( m_cSound[ SD_Bu3 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_button_03.wav" );
	strcpy_s( m_cSound[ SD_Bu4 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_button_04.wav" );

	strcpy_s( m_cSound[ SD_Wi1 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_window_01.wav" );
	strcpy_s( m_cSound[ SD_Wi2 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_window_02.wav" );

	strcpy_s( m_cSound[ SD_At1 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_Attach_01.wav" );
	strcpy_s( m_cSound[ SD_At2 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_Attach_02.wav" );
	strcpy_s( m_cSound[ SD_At3 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_Attach_03.wav" );	

	strcpy_s( m_cSound[ SD_Np1 ], SYSTEM_SOUND_PATH_LEN, "System\\System_npc_01.wav" );
	strcpy_s( m_cSound[ SD_Np2 ], SYSTEM_SOUND_PATH_LEN, "System\\System_npc_02.wav" );
	strcpy_s( m_cSound[ SD_Np3 ], SYSTEM_SOUND_PATH_LEN, "System\\System_npc_03.wav" );
	strcpy_s( m_cSound[ SD_Np4 ], SYSTEM_SOUND_PATH_LEN, "System\\System_npc_03.wav" );

	strcpy_s( m_cSound[ SD_Wa1 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_warning_01.wav" );
	strcpy_s( m_cSound[ SD_Wa2 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_warning_02.wav" );
	strcpy_s( m_cSound[ SD_Wa3 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_warning_03.wav" );

	strcpy_s( m_cSound[ SD_Qu1 ], SYSTEM_SOUND_PATH_LEN, "System\\System_quest_01.wav" );
	strcpy_s( m_cSound[ SD_Qu2 ], SYSTEM_SOUND_PATH_LEN, "System\\System_quest_02.wav" );

	cSprite::GlobalInit();
}

void cWindow::GlobalShotDown()
{
	cSprite::GlobalShotDown();	
}

void cWindow::SetWorkingFolder( char* cPath )
{
	size_t len = strlen( cPath );
	if( cPath[ len - 1 ] == '\\' )
	{
		strcpy_s( m_cWorkingFolder, MAX_PATH, cPath );
	}
	else
	{
		sprintf_s( m_cWorkingFolder, MAX_PATH, "%s\\", cPath );
	}	
}

/*
void cWindow::PlaySound( eSOUND type )
{
	assert_cs( type != SD_NONE );
	if( g_pSoundMgr )
		g_pSoundMgr->PlaySound( m_cSound[ type ] );
}

void cWindow::PlaySound( char* cPath )
{
	if( g_pSoundMgr )
		g_pSoundMgr->PlaySound( cPath );
}
*/

uint cWindow::GetSoundTypeBaseIndex( char* cType )
{
	/*if( strcmp( cType, "Ba" ) == 0 )
	{
		return SD_Ba_Index;
	}
	else if( strcmp( cType, "Bu" ) == 0 )
	{
		return SD_Bu_Index;
	}
	else if( strcmp( cType, "Wi" ) == 0 )
	{
		return SD_Wi_Index;
	}
	else if( strcmp( cType, "At" ) == 0 )
	{
		return SD_At_Index;
	}
	else if( strcmp( cType, "Np" ) == 0 )
	{
		return SD_Np_Index;
	}
	else if( strcmp( cType, "Wa" ) == 0 )
	{
		return SD_Wa_Index;
	}
	else if( strcmp( cType, "Qu" ) == 0 )
	{
		return SD_Qu_Index;
	}
	assert_cs( false );*/
	return 0;
}

//=======================================================================================================================
//
//		Base
//
//=======================================================================================================================

void cWindow::Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize )
{
	m_pParentWindow = pParent;
	m_bApplyWindowSize = bApplyWindowSize;
	m_ptSize = size;
	m_ptDefault = pos;
	CsPoint xy = m_ptDefault;

	if( m_bApplyWindowSize == true )
	{
		float fRate = (float)m_ptDefault.x/(float)1024;
		xy.x = (int)( fRate*g_nScreenWidth );
		fRate = (float)m_ptDefault.y/(float)768;
		xy.y = (int)( fRate*g_nScreenHeight );
	}

	if( m_pParentWindow != NULL )
	{
		m_ptClient = xy + m_pParentWindow->GetClient();
	}
	else
	{
		m_ptClient = xy;
	}
}

//======================================================================================
//
//		state
//
//======================================================================================
void cWindow::SetPos( CsPoint ptPos )
{
	m_ptDefault = ptPos;
	CsPoint xy = m_ptDefault;

	if( m_bApplyWindowSize == true )
	{
		float fRate = (float)m_ptDefault.x/(float)1024;
		xy.x = (int)( fRate*g_nScreenWidth );
		fRate = (float)m_ptDefault.y/(float)768;
		xy.y = (int)( fRate*g_nScreenHeight );
	}

	if( m_pParentWindow != NULL )
	{
		m_ptClient = xy + m_pParentWindow->GetClient();
	}
	else
	{
		m_ptClient = xy;
	}
}

void cWindow::SetSize( CsPoint ptSize )
{
	m_ptSize = ptSize;
}

//======================================================================================
//
//		µ¿ÀÛ
//
//======================================================================================
void cWindow::SetMove( CsPoint ptDelta )
{
	m_ptDefault += ptDelta;

	/*if( m_ptDefault.x < 0 )									m_ptDefault.x = 0;
	else if( m_ptDefault.x+m_ptSize.x > g_nScreenWidth )	m_ptDefault.x = g_nScreenWidth - m_ptSize.x;
	if( m_ptDefault.y < 0 )									m_ptDefault.y = 0;
	else if( m_ptDefault.y+m_ptSize.y > g_nScreenHeight )	m_ptDefault.y = g_nScreenHeight - m_ptSize.y;*/	
}

//======================================================================================
//
//		ResetDevice
//
//======================================================================================
void cWindow::ResetDevice( CsPoint pos )
{
	m_ptDefault = pos;
	cWindow::ResetDevice();
}

void cWindow::ResetDevice()
{
	CsPoint xy = m_ptDefault;

	if( m_bApplyWindowSize == true)
	{
		float fRate = (float)m_ptDefault.x/(float)1024;
		xy.x = (int)( fRate*g_nScreenWidth );
		fRate = (float)m_ptDefault.y/(float)768;
		xy.y = (int)( fRate*g_nScreenHeight );
	}

	if( m_pParentWindow != NULL )
	{
		m_ptClient = xy + m_pParentWindow->GetClient();
	}
	else
	{
		m_ptClient = xy;
	}
}



