#include "stdafx.h"
#include "Mail.h"

#define IF_MAIL_ICONSIZE	CsPoint( 32, 32 )
#define IF_MAIL_STR_WIDTH	277

cMail::cMail():m_pOkButton(NULL),m_pScrollBar(NULL)
{

}

cMail::~cMail()
{

}

cBaseWindow::eWINDOW_TYPE cMail::GetWindowType()
{ return WT_MAIL; }

void cMail::ResetMap() 
{ 
	Close( false ); 
}

void cMail::Destroy()
{	
	cBaseWindow::Delete();	
}

void cMail::DeleteResource()
{		
	_ReleaseItemInfo();
	m_strMessage.Delete();
	DeleteScript();
}

void cMail::_ReleaseItemInfo()
{
	m_wsCashProductName.clear();
	m_vpItemIdx.Destroy();
	m_vpItemCnt.Destroy();
}

void cMail::Create(int nValue /* = 0  */)
{	
	SetRootClient( CsPoint( g_nScreenWidth / 2 - 176, g_nScreenHeight / 2 - 223 ) );	

	// 아이템 위치
	CsPoint ptStart( 35, 354 );	
	for( int x=0; x<MAIL_ITEM_COUNT; ++x )
	{
		m_IFIcon[ x ].Init( CsRect( ptStart, (CsSIZE)IF_MAIL_ICONSIZE ) );
		ptStart.x += 47;
	}
	
	cBaseWindow::Init();
	InitScript( "Mail\\Letter.tga", m_ptRootClient, CsPoint( 353, 446 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );		
	m_pOkButton = AddButton( CsPoint( 133, 407 ), CsPoint( 78, 22 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str(), cWindow::SD_Ba1 ); 	
	if( m_pOkButton )
		m_pOkButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cMail::SendMessageConfirm);

	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 328, 20 ), CsPoint( 16, 379 ), CsPoint( 16, 15 ), CsRect( CsPoint( 18, 27 ), CsSIZE( 323, 318 ) ), 11 );
	m_strMessage.SetBuddy( m_pScrollBar );
		
	MailContents::sMAILINFO const& info = GetSystem()->GetMailInfo();

	//TCHAR sz[ 64 ];
	cText::sTEXTINFO ti;	
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor::WHITE );
	ti.s_eTextAlign = DT_LEFT;
	ti.s_bOutLine = false; 

	{
		std::wstring wsSender;
		DmCS::StringFn::Format( wsSender, L"%s : %s", UISTRING_TEXT( "MAIL_SENDER" ).c_str(), info.GetSenderName().c_str() );
		g_pStringAnalysis->Cut( &m_strMessage, 277, wsSender.c_str(), &ti );
	}

	{
		tm time;
		nBase::GetTime2TM( info.GetSendTime(), time );
		std::wstring wsSendTime = UISTRING_TEXT( "MAIL_SEND_TIME_YYMMDD" );
		DmCS::StringFn::Replace( wsSendTime, L"#Year#",		time.tm_year + 1900 );
		DmCS::StringFn::Replace( wsSendTime, L"#Month#",	time.tm_mon + 1 );
		DmCS::StringFn::Replace( wsSendTime, L"#Day#",		time.tm_mday );
		DmCS::StringFn::Replace( wsSendTime, L"#Hour#",		time.tm_hour );
		DmCS::StringFn::Replace( wsSendTime, L"#Min#",		time.tm_min );
		g_pStringAnalysis->Cut( &m_strMessage, 277, wsSendTime.c_str(), &ti );
	}

	{
		std::wstring wsDiscript;
		DmCS::StringFn::Format( wsDiscript, L"%s :", UISTRING_TEXT( "MAIL_DESCRIPTION" ).c_str() );
		g_pStringAnalysis->Cut( &m_strMessage, 277, wsDiscript.c_str(), &ti );
		g_pStringAnalysis->Cut_MailParcing( &m_strMessage, 277, info.GetMessage().c_str(), &ti );
	}

	_ItemAnalysis( info.GetMessage().c_str(), &ti );

	if( m_wsCashProductName.empty() )
	{
		ti.s_Color = FONT_RED;
		ti.s_eTextAlign = DT_CENTER;

		CsPoint ptPos = CsPoint( 347 / 2 - 5, 275 );
		// 다음패치 이전까지 아이템을 수령하지 않을 시 삭제됩니다. - usa/thai x
		// 해당 메일은 확인 시 삭제 됩니다.
		cStringList sl;
		g_pStringAnalysis->Cut_Parcing( &sl, 320, UISTRING_TEXT( "MAIL_MESSAGE_AUTO_DELETE" ).c_str(), &ti );
		std::list< cString* >* pkList = sl.GetList();
		std::list< cString* >::iterator kIter = pkList->begin();
		for( ; kIter != pkList->end(); ++kIter )
		{
			cString* pStr = *kIter;
			SAFE_POINTER_CON( pStr );
			cString::sTEXT* pText = (cString::sTEXT*)pStr->GetElement( 0 );
			SAFE_POINTER_CON( pText );
			ti.SetText( pText->s_Text.GetText() );
			AddText( &ti, ptPos );
			ptPos.y += 17;
		}
	}
	else
	{
		cStringList sl;
		g_pStringAnalysis->Cut_Parcing( &sl, 277, m_wsCashProductName.c_str(), &ti );

		std::wstring itemName;
		std::list< cString* >* pkList = sl.GetList();
		std::list< cString* >::iterator kIter = pkList->begin();
		for(; kIter != pkList->end(); ++kIter)
		{	
			cString::sTEXT* pText = (cString::sTEXT*)(*kIter)->GetElement(0);
			itemName += pText->s_Text.GetText();
		}

		std::wstring wsShowMsg = UISTRING_TEXT("CASHSHOP_PRESENT_MAIL_TEXT");
		DmCS::StringFn::ReplaceAll( wsShowMsg, L"#TamerName#", info.GetSenderName() );
		DmCS::StringFn::ReplaceAll( wsShowMsg, L"#ItemName#", itemName );

		g_pStringAnalysis->Cut_Parcing( &m_strMessage, 277, wsShowMsg.c_str(), &ti );
	}

	ti.s_eTextAlign = DT_CENTER;
	ti.s_eFontSize = CFont::FS_12;
	ti.s_Color = FONT_WHITE;
	ti.s_bOutLine = true;
	ti.SetText( UISTRING_TEXT( "MAIL_DATS_CENTER_MAIL" ).c_str() );
	AddText( &ti, CsPoint( 347 / 2, 12 ) );	

	ti.s_eTextAlign = DT_LEFT;
	ti.s_eFontSize = CFont::FS_10;
	ti.SetText( UISTRING_TEXT( "MAIL_ITEM_LIST" ).c_str() );
	AddText( &ti, CsPoint( 36, 328 ) );	
}

void cMail::SendMessageConfirm(void* pSender, void* pData)
{
	GetSystem()->SendMailConfirm();
	Close();
}

cBaseWindow::eMU_TYPE
cMail::Update_ForMouse()
{	
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;	

	if( m_pOkButton && m_pOkButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;
	
	if( _UpdateIcon_ForMouse() )
		return muReturn;

	if( m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;
	
	_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cMail::Update(float const& fDeltaTime)
{
	if( m_pScrollBar )
		m_pScrollBar->SetRange( CsPoint( 0, m_strMessage.GetSize() ) );	
	
	_UpdateMoveWindow();
}

void cMail::Render()
{
	RenderScript();	
	m_strMessage.Render( GetRootClient() + CsPoint( 20, 57 ) , 5 );
	
	CsItem::sINFO* pFTItemInfo;
	CsPoint pos;
	for( int i=0; i<m_vpItemIdx.Size(); ++i )
	{
		// 위치
		pos = m_IFIcon[ i ].GetPos() + GetRootClient();
		pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( m_vpItemIdx[ i ] )->GetInfo();

		g_pIconMng->RenderItem( m_vpItemIdx[ i ], pos, m_vpItemCnt[ i ] );

		// 마스크 있다면 마스크 랜더링
		if( m_IFIcon[ i ].s_eState != sIFIcon::NONE )
		{			
			m_IFIcon[ i ].RenderMask( pos, IF_MAIL_ICONSIZE );
		}
	}
}

void cMail::ResetDevice()
{	
	ResetDeviceScript();

	m_strMessage.ResetDevice();

	if( m_pScrollBar )
		m_pScrollBar->ResetDevice();
}

void cMail::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
	
	m_strMessage.ResetDevice();

	if( m_pScrollBar )
		m_pScrollBar->ResetDevice();
}

bool cMail::_UpdateIcon_ForMouse()
{
	bool bCheck = false;
	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	// 툴팁용	
	for( int i=0; i<MAIL_ITEM_COUNT; ++i )
	{
		if( i >= m_vpItemIdx.Size() )
			break;

		if( m_IFIcon[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
		{
			int nItemType = nsCsFileTable::g_pItemMng->GetItem( m_vpItemIdx[ i ] )->GetInfo()->s_nType_L;
			CsPoint pos = m_IFIcon[ i ].GetPos() + GetRootClient();			

			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_MAIL_ICONSIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_vpItemIdx[ i ], cBaseWindow::WT_MAIL, cTooltip::CLOSE_SLOT, 0, NULL );
		}
	}

	return bCheck;
}

void cMail::_ItemAnalysis( TCHAR const* szStr, cText::sTEXTINFO* ti )
{
	_ReleaseItemInfo();

	int nStrLen = _tcslen( szStr );
	if( nStrLen < 1 )
		return;

	for( int i=0; i<nStrLen; ++i )
	{
		//=================================================================================================
		//	스트링중에 아이템 정보가 있는지 확인한다
		//=================================================================================================

		// 캐쉬샵 메일인지 확인
		if( szStr[ i ] == L'`' && szStr[ i + 9 ] == L'`' )
		{
			int nNo = _ttoi( &szStr[ i+1 ] );

			LIST_PACKAGE_ITEMS itemData;
			GetSystem()->GetCashItemData( nNo, m_wsCashProductName, itemData );
			LIST_PACKAGE_ITEMS_CIT it = itemData.begin();
			for( ; it != itemData.end(); ++it )
				_InsertItemInfo( (*it).s_nItemID, (*it).s_nItemCnt );
		}
		else if( szStr[ i ] == L'`' )// 이벤트 메일인지 확인
		{
			TCHAR szNum = szStr[ i + 1 ];
			int nType = _ttoi( &szNum );
			int nNo = _ttoi( &szStr[ i+2 ] );

			CsEventTable::sEVENT* pItem = nsCsFileTable::g_pEventMng->GetMap( nType*10000,nNo );	
			if( pItem != NULL )
			{
				for( int Item=0; Item<6; ++Item )
					_InsertItemInfo( pItem->s_nItemID[ Item ], pItem->s_nItemCnt[ Item ]);

				g_pStringAnalysis->Cut( &m_strMessage, IF_MAIL_STR_WIDTH, pItem->s_szMessage, ti );	
			}
			return;
		}							
	}
}

void cMail::_InsertItemInfo( int nItemIdx, int nItemCnt )
{
	if(m_vpItemIdx.Size() >= MAIL_ITEM_COUNT)
		return;

	if(m_vpItemCnt.Size() >= MAIL_ITEM_COUNT)
		return;

	if( nItemIdx == 0 )
		return;

	m_vpItemIdx.PushBack( nItemIdx );
	m_vpItemCnt.PushBack( nItemCnt );	
}


bool cMail::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	return true;
}



void cMail::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{

}