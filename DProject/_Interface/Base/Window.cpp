
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
	strcpy_s( m_cSound[ SD_MSS1 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_message_01.wav" );
	strcpy_s( m_cSound[ SD_MSS2 ], SYSTEM_SOUND_PATH_LEN, "System\\Interface_message_02.wav" );
	cSprite::GlobalInit();
	cButton::GlobalInit();
}

void cWindow::GlobalShotDown()
{
	cSprite::GlobalShotDown();	
}

cWindow::cWindow():m_pUserData(NULL),m_eSoundType(SD_NONE),m_pParentWindow(NULL),m_bVisible(true)
{		
}

cWindow::~cWindow()
{
	SAFE_DELETE(m_pUserData);
}

void cWindow::SetVisible( bool bValue )
{
	m_bVisible = bValue;
}

bool cWindow::GetVisible() const
{
	return m_bVisible;
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

void cWindow::PlaySound( eSOUND type )
{
	assert_cs( type != SD_NONE );
	if( g_pSoundMgr )
		g_pSoundMgr->PlaySystemSound( m_cSound[ type ] );
}

void cWindow::PlaySound( char* cPath )
{
	if( g_pSoundMgr )
		g_pSoundMgr->PlaySystemSound( cPath );
}

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
		float fRate = (float)m_ptDefault.x/(float)DEFAULT_SCREEN_WIDTH;
		xy.x = (int)( fRate*g_nScreenWidth );
		fRate = (float)m_ptDefault.y/(float)DEFAULT_SCREEN_HEIGHT;
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
		float fRate = (float)m_ptDefault.x/(float)DEFAULT_SCREEN_WIDTH;
		xy.x = (int)( fRate*g_nScreenWidth );
		fRate = (float)m_ptDefault.y/(float)DEFAULT_SCREEN_HEIGHT;
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

CsPoint cWindow::GetDefaultPos()
{ 
	return m_ptDefault; 
}

CsPoint cWindow::GetClient() const 
{ 
	return m_ptClient; 
}

CsPoint cWindow::GetSize() const 
{ 
	return m_ptSize; 
}

CsRect cWindow::GetRect() const
{
	CsRect value;
	value.left = m_ptClient.x;
	value.top = m_ptClient.y;

	value.right = m_ptClient.x + m_ptSize.x;
	value.bottom = m_ptClient.y + m_ptSize.y;
	return value;
}


//======================================================================================
//
//		동작
//
//======================================================================================
CURSOR::eTYPE cWindow::Click()
{ 
	return CURSOR_ST.CheckClickBox( m_ptClient.x, m_ptClient.y, m_ptSize.x, m_ptSize.y ); 
}

void cWindow::SetMove( CsPoint ptDelta )
{
	m_ptDefault += ptDelta;

	/*if( m_ptDefault.x < 0 )									m_ptDefault.x = 0;
	else if( m_ptDefault.x+m_ptSize.x > g_nScreenWidth )	m_ptDefault.x = g_nScreenWidth - m_ptSize.x;
	if( m_ptDefault.y < 0 )									m_ptDefault.y = 0;
	else if( m_ptDefault.y+m_ptSize.y > g_nScreenHeight )	m_ptDefault.y = g_nScreenHeight - m_ptSize.y;*/	
}

CsPoint cWindow::MousePosToWindowPos( CsPoint ptMousePos )
{ 
	return ptMousePos - m_ptClient; 
}

//======================================================================================
//
//		사운드
//
//======================================================================================

void cWindow::SetSound( eSOUND type )
{ 
	m_eSoundType = type; 
}

void cWindow::PlaySound()
{ 
	PlaySound( m_eSoundType ); 
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
		float fRate = (float)m_ptDefault.x/(float)DEFAULT_SCREEN_WIDTH;
		xy.x = (int)( fRate*g_nScreenWidth );
		fRate = (float)m_ptDefault.y/(float)DEFAULT_SCREEN_HEIGHT;
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



void cWindow::SetParentWindow( cWindow* pParent )
{
	m_pParentWindow = pParent;
	CsPoint xy = m_ptDefault;
	if( m_bApplyWindowSize == true )
	{
		float fRate = (float)m_ptDefault.x/(float)DEFAULT_SCREEN_WIDTH;
		xy.x = (int)( fRate*g_nScreenWidth );
		fRate = (float)m_ptDefault.y/(float)DEFAULT_SCREEN_HEIGHT;
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

bool cWindow::PtInWindowRect( CsPoint const& pt )
{
	if( m_ptClient.x > pt.x )
		return false;
	
	if( m_ptClient.y > pt.y )
		return false;

	if( (m_ptSize.x + m_ptClient.x) < pt.x )
		return false;

	if( (m_ptSize.y + m_ptClient.y) < pt.y )
		return false;

	return true;
}


void cWindow::SetUserData( CUSTOMDATA* pUserData )
{
	SAFE_DELETE( m_pUserData );
	m_pUserData = pUserData;
}

CUSTOMDATA* cWindow::GetUserData() const
{
	return m_pUserData;
}

void cWindow::SetControlName( std::string const& sName )
{
	m_sControlName = sName;
}

std::string const&	cWindow::GetControlName() const
{
	return m_sControlName;
}

