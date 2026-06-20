#include "stdafx.h"
#include "Community.h"

//////////////////////////////////////////////////////////////////////////
//
//	친구
//
//////////////////////////////////////////////////////////////////////////

void cCommunity::_MakeFriend()
{	
	_DeletePbButton();
	m_strPersonInfo.Delete();

	cButton* pName = NiNew cButton;	
	pName->Init( m_pRoot, CsPoint( 23, 275 ), CsPoint( 99, 20 ), cButton::IMAGE_NOR_12, _LAN( "이름" ), false );
	cButton* pMemo = NiNew cButton;	
	pMemo->Init( m_pRoot, CsPoint( 121, 275 ), CsPoint( 260, 20 ), cButton::IMAGE_NOR_12, _LAN( "메모" ), false );
	pName->SetTextColor( FONT_WHITE * 0.7f );
	m_pbButton.PushBack( pName );

	pMemo->SetTextColor( FONT_WHITE * 0.7f );
	m_pbButton.PushBack( pMemo );	

	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::MAKE_FRIEND_LIST, null);

	// 길드 인원수 체크
	TCHAR szNum[ 32 ]={0};
	_stprintf_s( szNum, 32, _T( "%d / %d" ), m_strPersonInfo.GetSize(), cFriendListContents::FRIENDLIST_MAX_FRIENDCOUNT );	
	m_pGuildPersonNum->SetText( szNum );
	// 스크롤바 길이 설정
	m_pScrollBar->SetRange( CsPoint( 0, m_strPersonInfo.GetSize() ) );
}

void cCommunity::AddFriend( cFriendListContents::FRIEND_INFO* pInfo, int nRealDataIdx )
{	
	int nIdx = DeletePerson( nRealDataIdx );

	if( g_pDataMng->GetGuild()->IsOnOff() == false && pInfo->s_eOnlineState != cFriendListContents::ONLINE )
		return;	

	CsPoint ptPos;
	int nBase;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );

	cString* pString = NiNew cString;	
	pString->SetValue1( nIdx );
	pString->SetValue2( nRealDataIdx );

	switch( pInfo->s_eOnlineState )
	{
	case cFriendListContents::OFFLINE:		ti.s_Color = OFFLINE_COLOR;					break;
	case cFriendListContents::ONLINE:		ti.s_Color = ONLINE_COLOR;					break;
	case cFriendListContents::NO_EXIST:	ti.s_Color = NiColor( 1.0f, 0.2f, 0.2f );	break;
	default:							assert_cs( false );
	}	

	ti.SetText( pInfo->s_szName.c_str() );	
	pString->AddText( &ti );	
	cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
	pString->TailAddSizeX( 98 - ptPos.x );	
	
	ti.SetText( pInfo->s_szMemo.c_str() );	
	pString->AddText( &ti );	
	cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );	

	if( nIdx == 0 )
		m_strPersonInfo.AddHead( pString );
	else
		m_strPersonInfo.AddIndexAfter( pString, nIdx - 1 );	
}

cButton::eACTION
cCommunity::_UpdateMouseFriend()
{	
	return cButton::ACTION_NONE;
}

void cCommunity::_FriendPersonPopup( int nDataIdx )
{
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::SET_REALDATAIDX, &nDataIdx);

	g_pGameIF->GetPopup()->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::FRIEND_LIST );											
}

//////////////////////////////////////////////////////////////////////////
//
//	차단
//
//////////////////////////////////////////////////////////////////////////

void cCommunity::_MakeShutOut()
{
	_DeletePbButton();
	m_strPersonInfo.Delete();

	cButton* pName = NiNew cButton;	
	pName->Init( m_pRoot, CsPoint( 23, 275 ), CsPoint( 99, 20 ), cButton::IMAGE_NOR_12, _LAN( "이름" ), false );
	cButton* pMemo = NiNew cButton;	
	pMemo->Init( m_pRoot, CsPoint( 121, 275 ), CsPoint( 260, 20 ), cButton::IMAGE_NOR_12, _LAN( "메모" ), false );

	pName->SetTextColor( FONT_WHITE * 0.7f );
	m_pbButton.PushBack( pName );

	pMemo->SetTextColor( FONT_WHITE * 0.7f );
	m_pbButton.PushBack( pMemo );

	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::MAKE_SHUTOUT_LIST,null);

	// 길드 인원수 체크
	TCHAR szNum[ 32 ]={0};
	_stprintf_s( szNum, 32, _T( "%d / %d" ), m_strPersonInfo.GetSize(), cFriendListContents::FRIENDLIST_MAX_SHUTOUTCOUNT );	
	m_pGuildPersonNum->SetText( szNum );
	// 스크롤바 길이 설정
	m_pScrollBar->SetRange( CsPoint( 0, m_strPersonInfo.GetSize() ) );
}

void cCommunity::AddShutOut( cFriendListContents::SHUTOUT_INFO* pInfo, int nRealDataIdx )
{
	int nIdx = DeletePerson( nRealDataIdx );	

	CsPoint ptPos;
	int nBase;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ti.s_Color = FONT_RED;

	cString* pString = NiNew cString;	
	pString->SetValue1( nIdx );
	pString->SetValue2( nRealDataIdx );		

	ti.SetText( pInfo->s_szName.c_str() );	
	pString->AddText( &ti );	
	cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
	pString->TailAddSizeX( 98 - ptPos.x );	

	ti.SetText( pInfo->s_szMemo.c_str() );	
	pString->AddText( &ti );	
	cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );	

	if( nIdx == 0 )
		m_strPersonInfo.AddHead( pString );
	else
		m_strPersonInfo.AddIndexAfter( pString, nIdx - 1 );
}

cButton::eACTION	
cCommunity::_UpdateMouseShutOut()
{
	return cButton::ACTION_NONE;
}

void cCommunity::_ShutOutPersonPopup( int nDataIdx )
{
	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::SET_REALDATAIDX, &nDataIdx);
	g_pGameIF->GetPopup()->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::SHUTOUT_LIST );											
}