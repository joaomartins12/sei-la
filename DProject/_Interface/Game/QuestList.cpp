
#include "stdafx.h"
#include "QuestList.h"

void cQuestList::Destroy()
{
	cBaseWindow::Delete();
}

void cQuestList::DeleteResource()
{	
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_DISCRIPT );

	m_ExpandStringMain.RemoveAll();
	m_ExpandStringSub.RemoveAll();
	DeleteScript();

	m_QuestListStringMain.Delete();
	m_QuestListStringSub.Delete();

	SAFE_NIDELETE( m_pSelMask );
	SAFE_NIDELETE( m_pOnMouseMask );
	SAFE_NIDELETE( m_pExpandImage );
	SAFE_NIDELETE( m_pProcessImage );
}

void cQuestList::Create(int nValue /* = 0  */)
{
	if( IsLive() == true )
	{
		SetShowWindow( true );

		if( nValue != 0 )
			InitSelectQuest( nValue );
		else if( m_pSelectString )
			( (cQuestDiscript*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_QUEST_DISCRIPT ) )->SetQuest( m_pSelectString->GetValue2(), true );
		else
		{
			int nFirstQuestID = _GetFirstSelectQuestID();
			InitSelectQuest( nFirstQuestID );
		}
		return;
	}

// 	if( g_pResist->IsTutorial() == false )
// 		SetRootClient( CsPoint( 10, 165 ) );
// 	else
		SetRootClient( CsPoint( 386, 100 ) );

	_Create_QuestList();
	cBaseWindow::Init();

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ti.SetText( (int)0 );
	ti.s_eTextAlign = DT_RIGHT;

	m_rcTitleRegionMain = CsRect( CsPoint( 14, 84 ), CsPoint( 305, 205 ) );
	m_rcTitleRegionSub = CsRect( CsPoint( 14, 235 ), CsPoint( 305, 446 ) );

	InitScript( "QuestList\\Quest_Listwin.tga", m_ptRootClient, CsPoint( 340, 491 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
	AddTitle( UISTRING_TEXT( "QUESTLIST_TITLE_QUEST_LIST" ).c_str() );

	// 진행 상태
	m_pProcessImage = NiNew cImage;
	m_pProcessImage->Init( NULL, CsPoint::ZERO, CsPoint( 31, 18 ), "QuestList\\Quest_Check.tga", false );
	m_pProcessImage->SetTexToken( CsPoint( 0, 21 ) );
	
	// 펼쳐짐 상태
	m_pExpandImage = NiNew cImage;
	m_pExpandImage->Init( NULL, CsPoint::ZERO, CsPoint( 12, 12 ), "QuestList\\Expand.tga", false );
	m_pExpandImage->SetTexToken( CsPoint( 0, 12 ) );

	// 퀘스트 헬퍼 보기
	m_pHelperViewCheck = AddCheckBox( CsPoint( 311, 34 ), CsPoint( 20, 20 ), CsPoint( 0, 20 ), "System\\Check.tga" );
	m_pHelperViewCheck->SetCheck( g_pGameIF->GetQuestHelper()->IsView() );
	
	// 스크롤바
	m_pScrollBarMain = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 307, 84 ), CsPoint( 16, 123 ), cScrollBar::GetDefaultBtnSize(), m_rcTitleRegionMain, 5 );
	m_pScrollBarSub = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 307, 229 ), CsPoint( 16, 223 ), cScrollBar::GetDefaultBtnSize(), m_rcTitleRegionSub, 10 );
	
	// 퀘스트 갯수
	ti.s_eTextAlign = DT_LEFT;
	ti.s_Color = NiColor( 43/255.0f, 158/255.0f, 236/255.0f );
	m_pCurQuestCountText = AddText( &ti, CsPoint( 15, 38 ) );

	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( UISTRING_TEXT( "QUESTLIST_HELPER" ).c_str() ); 
	AddText( &ti, CsPoint( 288, 38 ) );

	//메인
	ti.SetText( UISTRING_TEXT( "QUESTLIST_MAIN" ).c_str() ); 
	ti.s_eFontSize = CFont::FS_11;
	ti.s_Color = NiColor( 245/255.0f, 174/255.0f, 0/255.0f );
	AddText( &ti, CsPoint( 39, 64 ) );
	
	//서브
	ti.SetText( UISTRING_TEXT( "QUESTLIST_SUB" ).c_str() ); 
	ti.s_Color = NiColor( 138/255.0f, 245/255.0f, 12/255.0f );
	AddText( &ti, CsPoint( 39, 211 ) );
	
	// 나가기 버튼
	m_pCancelButton = AddButton( CsPoint( 317, 6 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	
	//마스크
	m_pSelMask = NiNew cSprite;
	m_pSelMask->Init( NULL, CsPoint::ZERO, CsPoint( 275, 22 ), "Talk\\Common_Seletbar.tga", false );

	m_pOnMouseMask = NiNew cSprite;
	m_pOnMouseMask->Init( NULL, CsPoint::ZERO, CsPoint( 275, 22 ), "Talk\\Common_Seletbar.tga", false );
	m_pOnMouseMask->SetAlpha( 0.4f );

	ResetQuestList( false );

	m_pMouseOnString = NULL;

	if( nValue == 0 )
		nValue = _GetFirstSelectQuestID();
	
	InitSelectQuest( nValue );
}

int cQuestList::_GetFirstSelectQuestID()
{
	std::list< cString* >* pList = m_QuestListStringMain.GetList();
	std::list< cString* >::iterator it = pList->begin();
	for( ; it != pList->end(); ++it )
	{
		if( (*it)->GetValue1() != IF_QL_TITLE )
			continue;
		return (*it)->GetValue2();
	}

	pList = m_QuestListStringSub.GetList();
	it = pList->begin();
	for( ; it != pList->end(); ++it )
	{
		if( (*it)->GetValue1() != IF_QL_TITLE )
			continue;
		return (*it)->GetValue2();
	}
	return 0;
}

void cQuestList::InitSelectQuest( int nQuestID )
{
	if( 0 == nQuestID )
		return;

	CsQuest* pSelQuest = nsCsFileTable::g_pQuestMng->GetQuest( nQuestID );
	SAFE_POINTER_RET( pSelQuest );

	// 메인퀘스트인지 서브 퀘스트인지
	bool bQTMain = false;
	NiTStringPointerMap< int >* pExpand = NULL;
	cStringList* pStringList = NULL;
	if( pSelQuest->GetQuestType() == CsQuest::QT_MAIN )
	{
		pExpand = &m_ExpandStringMain;
		pStringList = &m_QuestListStringMain;
		bQTMain = true;
	}
	else
	{
		pExpand = &m_ExpandStringSub;
		pStringList = &m_QuestListStringSub;
	}

	// 해당 String 포인터를 찾자
	bool bFind = false;
	cString* pTabString = NULL;
	std::list< cString* >* pList = pStringList->GetList();
	std::list< cString* >::iterator it = pList->begin();
	std::list< cString* >::iterator itEnd = pList->end();
	for( ; it!=itEnd; ++it )
	{
		switch( (*it)->GetValue1() )
		{
		case IF_QL_TITLE:
			continue;
		case IF_QL_TAB_EXPAND:
			{
				cString::sTEXT* pTab = (cString::sTEXT*)( *it )->GetElement( 1 );
				assert_cs( pTab->GetType() == cString::sELEMENT::TEXT );
				// 이미 펼쳐져 있다면
				if( _tcscmp( pTab->s_Text.GetTextInfo()->GetText(), pSelQuest->m_szTitleTab ) == 0 )
					bFind = true;
			}
			break;
		case IF_QL_TAB_UNEXPAND:
			{
				cString::sTEXT* pTab = (cString::sTEXT*)( *it )->GetElement( 1 );
				assert_cs( pTab->GetType() == cString::sELEMENT::TEXT );
				// 찾았다면 포인터 연결하고 브레이트
				if( _tcscmp( pTab->s_Text.GetTextInfo()->GetText(), pSelQuest->m_szTitleTab ) == 0 )
				{
					bFind = true;
					pTabString = *it;
				}
			}
			break;
		default:
			assert_cs( false );
		}

		if( bFind == true )
			break;
	}

	// 펼치자
	if( pTabString != NULL )
	{
		char cTab[ QUEST_TITLE_LEN ] = {0, };
		W2M( cTab, pSelQuest->m_szTitleTab, QUEST_TITLE_LEN );
		int nTemp = 0;
		pExpand->SetAt( cTab, nTemp );
		pTabString->CalMaxSize();
		pTabString->SetValue1( IF_QL_TAB_EXPAND );		
	}

	// 선택하자
	it = pList->begin();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetValue1() != IF_QL_TITLE )
			continue;

		if( (*it)->GetValue2() == nQuestID )
		{
			m_pSelectString = *it;
			cQuestDiscript* pIF = (cQuestDiscript*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_QUEST_DISCRIPT );
			pIF->SetQuest( nQuestID, 0 );
			break;
		}
	}

	_CalScrollBarRange( bQTMain, m_pSelectString );
}

bool cQuestList::Close(bool bSound /* = true  */)
{
	if( IsEnableWindow() == false )
		return false;

	if( bSound )
	{
		PlayCloseSound();
	}

	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_DISCRIPT );
	SetShowWindow( false );

	return true;
}

void cQuestList::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cQuestList::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	switch( m_pHelperViewCheck->Update_ForMouse() )
	{
	case cCheckBox::ACTION_CLICK:
		g_pGameIF->GetQuestHelper()->SetView( m_pHelperViewCheck->IsCheck() );
	case cCheckBox::ACTION_DOWN:
		return muReturn;
	}

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:		
		Close();
		return muReturn;
	default:
		return muReturn;
	}
	if( m_pScrollBarMain->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;
	if( m_pScrollBarSub->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;

	if( _Update_FM_QuestList() == true )
	{
		// 동작 했음에도 선택한게 없다면 설명창 제거
		return muReturn;
	}

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 305, 27 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

CsPoint cQuestList::_UpdateMoveWindow()
{
	CsPoint ptDelta = cBaseWindow::_UpdateMoveWindow();
	if( ptDelta != CsPoint::ZERO )
	{		
		if( g_pGameIF->GetQuestDiscript() )
		{
			g_pGameIF->GetQuestDiscript()->SetMove( ptDelta );
			g_pGameIF->GetQuestDiscript()->OnMoveWindow();
		}
	}
	return ptDelta;	
}

void cQuestList::Render()
{
	RenderScript();

	_Render_QuestList( true );
	_Render_QuestList( false );
}

void cQuestList::ResetDevice()
{	
	ResetDeviceScript();

	m_QuestListStringMain.ResetDevice();
	m_QuestListStringSub.ResetDevice();

	if( m_pSelMask )
		m_pSelMask->ResetDevice();
}

void cQuestList::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();

	m_QuestListStringMain.ResetDevice();
	m_QuestListStringSub.ResetDevice();

	if( m_pSelMask )
		m_pSelMask->ResetDevice();
}

bool cQuestList::OnEscapeKey()
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_DISCRIPT ) )
	{
		if( g_pGameIF->GetQuestDiscript()->IsShowWindow() == true )
		{
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_DISCRIPT );
		}
	}
	Close();
	return true;
}

void cQuestList::OnSetFocus()
{
	cBaseWindow::OnSetFocus();
	if( g_pGameIF->GetQuestDiscript() )
		g_pGameIF->SetFocusWindow( g_pGameIF->GetQuestDiscript(), false );
}


