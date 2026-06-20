
#include "stdafx.h"
#include "Data_FriendList.h"

void cData_FriendList::Delete()
{
	int nCount = m_vpFriend.Size();
	for( int i=0; i<nCount; ++i )
		NiDelete m_vpFriend[ i ];
	m_vpFriend.Destroy();

	nCount = m_vpShutout.Size();
	for( int i=0; i<nCount; ++i )
		NiDelete m_vpShutout[ i ];
	m_vpShutout.Destroy();
}

void cData_FriendList::Init()
{
	m_bSendCheck = false;
#ifdef GUILD_RENEWAL
	m_bOnOffCheck = false;
#endif
}

void cData_FriendList::SendFriendInfo()
{
	if( m_bSendCheck == true )
		return;

	m_bSendCheck = true;

	if( net::game )
	{
		net::game->SendBuddyList();		
	}
}

void cData_FriendList::InitFirend( TCHAR const* szName, TCHAR const* szMemo, int nOnline )
{
	// 이미 존재 .. 이럴순 없지..
	int nCount = m_vpFriend.Size(); 
	for( int i=0; i<nCount; ++i )
	{
		if( _tcscmp( m_vpFriend[ i ]->s_szName, szName ) == 0 )
		{
			assert_cs( false );
			return;
		}
	}

	sFRIEND_INFO* fi = NiNew sFRIEND_INFO;
	_tcscpy_s( fi->s_szName, Language::pLength::name, szName );
	_tcscpy_s( fi->s_szMemo, Language::pLength::str, szMemo );
	fi->s_eOnlineState = (eONLINE_STATE)nOnline;

	m_vpFriend.PushBack( fi );	
}

void cData_FriendList::AddFriend( TCHAR const* szName, int bOnline )
{
	sFRIEND_INFO* fi = NiNew sFRIEND_INFO;
	_tcscpy_s( fi->s_szName, Language::pLength::name, szName );		
	fi->s_eOnlineState = (eONLINE_STATE)bOnline;	
	m_vpFriend.PushBack( fi );

	if( g_pGameIF )
	{
#ifdef GUILD_RENEWAL
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_FRIEND_LIST ) == true )
		{
			g_pGameIF->GetFriendList()->_SetTab( cFriendList::TAB_FRIEND );
		}
#else
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) == true )
		{
			g_pGameIF->GetCommunity()->SetTab( cCommunity::TAB_FRIEND );			
		}
#endif
	}

	//==========================================================================================================
	// 업적 체크
	//==========================================================================================================
	g_pDataMng->GetAchieve()->CheckType( cData_Achieve::CA_FriendCount, m_vpFriend.Size() );
}

void cData_FriendList::AddFriendMemo( TCHAR const* szName, TCHAR const* szMemo )
{	
	sFRIEND_INFO* pInfo = NULL;
	int nCount = m_vpFriend.Size(); 	
	for( int i=0; i<nCount; ++i )
	{
		if( _tcscmp( m_vpFriend[ i ]->s_szName, szName ) == 0 )
		{			
			pInfo = m_vpFriend[ i ];
			_tcscpy_s( pInfo->s_szMemo, Language::pLength::str, szMemo );
			
#ifdef GUILD_RENEWAL
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_FRIEND_LIST ) == true )
			{	
				if( g_pGameIF->GetFriendList()->_GetTab() == cFriendList::TAB_FRIEND )
					g_pGameIF->GetFriendList()->AddFriend( pInfo, i );
			}
#else
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) == true )
			{	
				if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_FRIEND )
					g_pGameIF->GetCommunity()->AddFriend( pInfo, i );
			}
#endif
			break;
		}		
	}			
}

void cData_FriendList::FriendOnOff( TCHAR const* szName, int nOnline )
{
	// 이미 존재 한다면 리턴
	sFRIEND_INFO* pInfo = NULL;	
	int nCount = m_vpFriend.Size(); 
	for( int i=0; i<nCount; ++i )
	{
		if( _tcscmp( m_vpFriend[ i ]->s_szName, szName ) == 0 )
		{			
			pInfo = m_vpFriend[ i ];			
			pInfo->s_eOnlineState = (eONLINE_STATE)nOnline;			

#ifdef GUILD_RENEWAL
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_FRIEND_LIST ) == true )
			{
				if( g_pGameIF->GetFriendList()->_GetTab() == cFriendList::TAB_FRIEND )
					g_pGameIF->GetFriendList()->AddFriend( pInfo, i );
			}
#else
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) == true )
			{
				if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_FRIEND )
					g_pGameIF->GetCommunity()->AddFriend( pInfo, i );
			}
#endif
			break;
		}
	}			
}

cData_FriendList::sFRIEND_INFO* cData_FriendList::GetFriendInfo( int nIndex )
{
	assert_cs( m_vpFriend.IsExistElement( nIndex ) );

	if( m_vpFriend.IsExistElement( nIndex ) == false )
		return NULL;

	return m_vpFriend[ nIndex ];
}

cData_FriendList::sFRIEND_INFO* cData_FriendList::GetFriendInfo( const TCHAR* szName )
{
	for( int i = 0; i < m_vpFriend.Size(); ++i )
	{
		if( _tcscmp( m_vpFriend[ i ]->s_szName, szName) == 0 )
			return m_vpFriend[ i ];
	}
	return NULL;
}

void cData_FriendList::DelFriend( int nIndex )
{
	if( m_vpFriend.IsExistElement( nIndex ) == false )
		return;

	NiDelete m_vpFriend[ nIndex ];
	m_vpFriend.DeleteElement( nIndex );
#ifdef GUILD_RENEWAL
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_FRIEND_LIST ) == true )
	{
		g_pGameIF->GetFriendList()->_SetTab( cFriendList::TAB_FRIEND );
	}
#else
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) == true )
	{
		g_pGameIF->GetCommunity()->SetTab( cCommunity::TAB_FRIEND );
	}
#endif
}

bool cData_FriendList::CheckFriend( TCHAR const* szName )
{
	if( m_vpFriend.Size() >= FRIENDLIST_MAX_FRIENDCOUNT )
	{
		cPrintMsg::PrintMsg( 30035 );
		return true;
	}

	if( _tcscmp( g_pCharMng->GetTamerUser()->GetName(), szName ) == 0 )
	{
		cPrintMsg::PrintMsg( 11101 );
		return true;
	}

	int nCount = m_vpFriend.Size(); 
	for( int i=0; i<nCount; ++i )
	{
		if( _tcscmp( m_vpFriend[ i ]->s_szName, szName ) == 0 )
		{
			cPrintMsg::PrintMsg( 30036 );
			return true;
		}
	}

	nCount = m_vpShutout.Size(); 
	for( int i=0; i<nCount; ++i )
	{
		if( _tcscmp( m_vpShutout[ i ]->s_szName, szName ) == 0 )
		{
			NiDelete m_vpShutout[ i ];
			m_vpShutout.DeleteElement( i );			
		}
	}

	return false;
}

void cData_FriendList::InitShutout( TCHAR const* szName, TCHAR const* szMemo, bool bOnline )
{
	// 이미 존재 .. 이럴순 없지..
	int nCount = m_vpShutout.Size(); 
	for( int i=0; i<nCount; ++i )
	{
		if( _tcscmp( m_vpShutout[ i ]->s_szName, szName ) == 0 )
		{
			assert_cs( false );
			return;
		}
	}

	sSHUTOUT_INFO* si = NiNew sSHUTOUT_INFO;
	_tcscpy_s( si->s_szName, Language::pLength::name, szName );
	_tcscpy_s( si->s_szMemo, Language::pLength::str, szMemo );
	m_vpShutout.PushBack( si );	
}

cData_FriendList::sSHUTOUT_INFO* cData_FriendList::GetShutInfo( int nIndex )
{
	assert_cs( m_vpShutout.IsExistElement( nIndex ) );
	return m_vpShutout[ nIndex ];
}

cData_FriendList::sSHUTOUT_INFO* cData_FriendList::GetShutInfo( const TCHAR* szName )
{
	for( int i = 0; i < m_vpShutout.Size(); ++i )
	{
		if( _tcscmp( m_vpShutout[ i ]->s_szName, szName) == 0 )
			return m_vpShutout[ i ];
	}
	return NULL;
}

void cData_FriendList::AddShutout( TCHAR const* szName, bool bOnline )
{
	sSHUTOUT_INFO* si = NiNew sSHUTOUT_INFO;
	_tcscpy_s( si->s_szName, Language::pLength::name, szName );	
	m_vpShutout.PushBack( si );

	if( g_pGameIF )
	{
#ifdef GUILD_RENEWAL
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_FRIEND_LIST ) == true )
		{
			g_pGameIF->GetFriendList()->_SetTab( cFriendList::TAB_SHUTOUT );
		}
#else
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) == true )
		{
			g_pGameIF->GetCommunity()->SetTab( cCommunity::TAB_SHUTOUT );				
		}
#endif
	}		
}

void cData_FriendList::AddShutoutMemo( TCHAR const* szName, TCHAR const* szMemo )
{
	sSHUTOUT_INFO* pInfo = NULL;	
	int nCount = m_vpShutout.Size(); 
	for( int i=0; i<nCount; ++i )
	{
		if( _tcscmp( m_vpShutout[ i ]->s_szName, szName ) == 0 )
		{
			pInfo = m_vpShutout[ i ];
			_tcscpy_s( pInfo->s_szMemo, Language::pLength::str, szMemo );

#ifdef GUILD_RENEWAL
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_FRIEND_LIST ) == true )
			{
				if( g_pGameIF->GetFriendList()->_GetTab() == cFriendList::TAB_SHUTOUT )
					g_pGameIF->GetFriendList()->AddShutOut( pInfo, i );
			}
#else
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) == true )
			{
				if( g_pGameIF->GetCommunity()->GetTab() == cCommunity::TAB_SHUTOUT )
					g_pGameIF->GetCommunity()->AddShutOut( pInfo, i );
			}
#endif
			break;
		}
	}			
}	

void cData_FriendList::DelShutout( int nIndex )
{
	if( m_vpShutout.IsExistElement( nIndex ) == false )
		return;

	NiDelete m_vpShutout[ nIndex ];
	m_vpShutout.DeleteElement( nIndex );

#ifdef GUILD_RENEWAL
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_FRIEND_LIST ) == true )
	{
		g_pGameIF->GetFriendList()->_SetTab( cFriendList::TAB_SHUTOUT );
	}
#else
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) == true )
	{
		g_pGameIF->GetCommunity()->SetTab( cCommunity::TAB_SHUTOUT );
	}
#endif
}

bool cData_FriendList::CheckShutout( TCHAR const* szName )
{	
	if( m_vpShutout.Size() >= FRIENDLIST_MAX_SHUTOUTCOUNT )
	{
		cPrintMsg::PrintMsg( 30038 );
		return true;
	}

	if( _tcscmp( g_pCharMng->GetTamerUser()->GetName(), szName ) == 0 )
	{
		cPrintMsg::PrintMsg( 11101 );
		return true;
	}

	int nCount = m_vpShutout.Size(); 
	for( int i=0; i<nCount; ++i )
	{
		if( _tcscmp( m_vpShutout[ i ]->s_szName, szName ) == 0 )
		{
			cPrintMsg::PrintMsg( 30036 );
			return true;
		}
	}

	nCount = m_vpFriend.Size(); 
	for( int i=0; i<nCount; ++i )
	{
		if( _tcscmp( m_vpFriend[ i ]->s_szName, szName ) == 0 )
		{
			NiDelete m_vpFriend[ i ];
			m_vpFriend.DeleteElement( i );		
		}
	}

	return false;
}

bool cData_FriendList::IsShutout( TCHAR const* szName )
{	
	int nCount = m_vpShutout.Size(); 
	for( int i=0; i<nCount; ++i )
	{
		if( _tcscmp( m_vpShutout[ i ]->s_szName, szName ) == 0 )
		{	
			return true;
		}
	}
	return false;
}

void cData_FriendList::ChangeTamerName( const std::wstring& BefoleName, const std::wstring& NewName )
{
	cData_FriendList::sFRIEND_INFO*	pFriendInfo  = GetFriendInfo( BefoleName.data() );
	if( pFriendInfo )
	{
		pFriendInfo->SetName( NewName );
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
		{	
			if( g_pGameIF->GetCommunity()->GetTab() == 2 )
				g_pGameIF->GetCommunity()->_MakeFriend();
		}
	}

	cData_FriendList::sSHUTOUT_INFO*	pShutOutInfo = GetShutInfo( BefoleName.data() );
	if( pShutOutInfo )
	{
		pShutOutInfo->SetName( NewName );
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
		{	
			if( g_pGameIF->GetCommunity()->GetTab() == 3 )
				g_pGameIF->GetCommunity()->_MakeShutOut();
		}
	}
}


void cData_FriendList::sFRIEND_INFO::SetName( const std::wstring& NewName )
{
	if( NewName.empty() )
		return;

	wmemset( s_szName, 0, sizeof( s_szName ) );
	_tcscpy_s( s_szName, Language::pLength::name, NewName.data() );
}

void cData_FriendList::sSHUTOUT_INFO::SetName( const std::wstring& NewName )
{
	if( NewName.empty() )
		return;

	wmemset( s_szName, 0, sizeof( s_szName ) );
	_tcscpy_s( s_szName, Language::pLength::name, NewName.data() );
}
