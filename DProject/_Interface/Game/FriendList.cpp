#include "stdafx.h"

#include "FriendList.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

cFriendList::cFriendList() : m_pCancelButton(null), m_pOnOffCheck(null), m_pRadioBtn(null), m_pFriendCnt(null), m_pPersonInfoListBox(null)
{
}
cFriendList::~cFriendList()
{
}
cBaseWindow::eWINDOW_TYPE cFriendList::GetWindowType()
{
	return WT_FRIEND_LIST;
}
void cFriendList::Destroy()
{
	if( g_pGameIF->GetPopup() )
	{
		g_pGameIF->GetPopup()->ClosePopup( cPopUpWindow::FRIEND_LIST );
		g_pGameIF->GetPopup()->ClosePopup( cPopUpWindow::SHUTOUT_LIST );
	}
	cBaseWindow::Delete();
}

void cFriendList::DeleteResource()
{
	cCreateName::DeleteInstance( cBaseWindow::WT_FRIEND_MEMO, 0 );
	cCreateName::DeleteInstance( cBaseWindow::WT_SHUTOUT_MEMO, 0 );	

	DeleteScript();
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cFriendList::Create( int nValue /*= 0 */ )
{
	cBaseWindow::Init();

	InitScript( "FriendList\\FriendWin.tga", CsPoint( (g_nScreenWidth/2) - 206 , (g_nScreenHeight/2) - 141 ), CsPoint( 413, 252 ), true );	
	
	// 닫기버튼
	m_pCancelButton = AddButton( CsPoint( 390, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pCancelButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cFriendList::CloseClick);

	// 친구/차단 탭
	m_pRadioBtn = NiNew cRadioButton;	
	m_pRadioBtn->Init( m_pRoot, CsPoint( 19, -23 ), CsPoint( 68, 24 ), cCheckBox::IMAGE_RADIO_4, UISTRING_TEXT( "COMMON_TXT_FRIEND" ).c_str(), false );
	m_pRadioBtn->AddNode( CsPoint( 87, -23/*15*/ ), CsPoint( 68, 24 ), cCheckBox::IMAGE_RADIO_4, UISTRING_TEXT( "FRIENDLIST_BTN_BLOCK" ).c_str() );
	
	//event 등록 라디오 버튼의 경우 체크박스 여러개로 이루어지기 때문에 각각의 이벤트 등록을 각 체크박스에 해주어야 한다.
	m_pRadioBtn->GetNode(0)->AddEvent(cCheckBox::BUTTON_LBUP_EVENT, this, &cFriendList::TabClick);
	m_pRadioBtn->GetNode(1)->AddEvent(cCheckBox::BUTTON_LBUP_EVENT, this, &cFriendList::TabClick);
	AddChildControl(m_pRadioBtn);

	// 이름 버튼( 동작 x ) 메모 버튼( 동작 x )
	AddButton( CsPoint( 22, 36 ), CsPoint( 98, 18 ), cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMON_TXT_NAME" ).c_str() );
	AddButton( CsPoint( 120, 36 ), CsPoint( 257, 18 ), cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMON_TXT_MEMO" ).c_str() );
	
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE * 0.7f );			
	ti.s_eTextAlign = DT_CENTER;
	// 친구 수 텍스트
	ti.s_Color = FONT_WHITE;
	ti.SetText( L"" );
	m_pFriendCnt = AddText( &ti, CsPoint(357, 12) );

	// 오프라인 인원 표시 텍스트
	cText::sTEXTINFO tiOfflinDislplay;
	tiOfflinDislplay.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );			
	tiOfflinDislplay.s_eTextAlign = DT_LEFT;
	tiOfflinDislplay.SetText( UISTRING_TEXT( "COMMUNITY_OFFLINE_MEMBER_INDICATOR" ).c_str() );
	AddText( &tiOfflinDislplay, CsPoint( 40, 12 ) );

	// 오프라인 인원 표시 체크박스
	m_pOnOffCheck = AddCheckBox( CsPoint( 20, 12 ), CsPoint( 16, 16 ), CsPoint( 0, 20 ), "System\\Check.tga" );
	if(GetSystem() != null)
		m_pOnOffCheck->SetCheck( GetSystem()->GetOnOff() );
	m_pOnOffCheck->AddEvent(cCheckBox::BUTTON_LBUP_EVENT, this, &cFriendList::OfflineDisplayClick);

	cText::sTEXTINFO tilistbox;
	tilistbox.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	m_pPersonInfoListBox = NiNew cListBox;
	if(m_pPersonInfoListBox != null)
	{
		m_pPersonInfoListBox->Init( m_pRoot, CsPoint(20,60), CsPoint( 387, 180 ), NULL, false );		
		m_pPersonInfoListBox->SetItemGab(CsPoint(0,1));
		m_pPersonInfoListBox->SetItemHeight( 15 );
		m_pPersonInfoListBox->SetMouseOverImg( "Talk\\Common_Seletbar.tga" );
		m_pPersonInfoListBox->SetSelectedImg( "Talk\\Common_Seletbar.tga" );
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if( pScrollBar )
		{
			pScrollBar->Init( cScrollBar::TYPE_ENCY, NULL, CsPoint( 0, 0 ), CsPoint( 16, 150 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint(70,50), CsPoint( 216, 180 ) ), 5, false );
			pScrollBar->SetEnableRenderFromEnableScroll(true);
			pScrollBar->ChangeGBImg( "Item_Make\\Make_WheelBar_Bag_Mid.tga", false );
			m_pPersonInfoListBox->SetScrollBar( pScrollBar );
		}
		m_pPersonInfoListBox->AddEvent(cListBox::LIST_SELECTED_ITEM_RIGHT_UP, this, &cFriendList::TamerMenuPop);
		AddChildControl(m_pPersonInfoListBox);
	}

	m_eRadioIndex = TAB_FRIEND;	// 첫 탭은 친구탭

	MakeFriend();	// 친구 정보 초기화
	MakeShutOut();	// 차단 정보 초기화
}

void cFriendList::MakeFriend()
{
	SAFE_POINTER_RET(m_pPersonInfoListBox);
	// 차단인원이 들어있을 수 있으니 리스트 날려줌
	m_pPersonInfoListBox->RemoveAllItem();

	// 친구 목록 받아오기
	if(GetSystem() == null)
		return;
	const cFriendListContents::FRIENDLIST & pVector = GetSystem()->GetFriendList();
	cFriendListContents::FRIENDLIST::const_iterator fItr = pVector.begin();

	for(int i = 0; fItr != pVector.end(); ++fItr, ++i )
	{
		AddFriend( &(*fItr), i );
	}
	// 인원 수 텍스트
	std::wstring txt;
	DmCS::StringFn::Format( txt, _T( "%d / %d" ), m_pPersonInfoListBox->GetItemCount(), neFriend::nMaxFriendCount );
	m_pFriendCnt->SetText( txt.c_str() );
}

void cFriendList::AddFriend( const cFriendListContents::FRIEND_INFO* pInfo, int nRealDataIdx )
{
	SAFE_POINTER_RET(m_pPersonInfoListBox);
	// nRealDataIdx : 저장시킬 인덱스
	// 저장할 곳에 친구정보가 있다면 삭제 함. 현재 목록보다 큰걸 넣으면 목록 사이즈가 나옴
	int nIdx = DeletePerson( nRealDataIdx );
	if( GetSystem() == null )
		return;

	// 오프라인 인원 보기가 꺼져있는데 오프라인이면 안넣음
	if( GetSystem()->GetOnOff() == false && pInfo->s_eOnlineState != cFriendListContents::ONLINE )
		return;	

	CsPoint ptPos;
//	int nBase;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );

	cString* pString = NiNew cString;	
	//pString->SetValue1( nIdx );
	//pString->SetValue2( nRealDataIdx );

	switch( pInfo->s_eOnlineState )
	{
		case cFriendListContents::OFFLINE:
			ti.s_Color = OFFLINE_COLOR;
			break;
		case cFriendListContents::ONLINE:
			ti.s_Color = ONLINE_COLOR;
			break;
		case cFriendListContents::NO_EXIST:
			ti.s_Color = NiColor( 1.0f, 0.2f, 0.2f );
			break;
		default:
			assert_cs( false );
	}	
	// 이름
	ti.SetText( pInfo->s_szName.c_str() );	
	pString->AddText( &ti );	

	// 메모
	cText::sTEXTINFO timemo;
	timemo.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	timemo.SetText( pInfo->s_szMemo.c_str() );	
	pString->AddText( &timemo, CsPoint(103, 0) );	

	cListBoxItem * addItem  = NiNew cListBoxItem;
	addItem->SetItem( pString );
	addItem->SetUserData( new sListBoxIndexInfo( nRealDataIdx, nIdx ) );
	m_pPersonInfoListBox->InsertItem( nIdx, addItem );
}

void cFriendList::MakeShutOut()
{
	SAFE_POINTER_RET(m_pPersonInfoListBox);
	// 친구인원이 들어있을 수 있으니 리스트 날려줌
	m_pPersonInfoListBox->RemoveAllItem();
	if (GetSystem() == null)
		return;
	// 차단 목록 받아오기
	const cFriendListContents::SHUTOUTLIST& pList = GetSystem()->GetShutoutList();
	cFriendListContents::SHUTOUTLIST::const_iterator fItr = pList.begin();

	for(int i = 0; fItr != pList.end(); ++fItr, ++i )
	{
		AddShutOut( &(*fItr), i );		
	}

	// 인원 수 텍스트
	std::wstring txt;
	DmCS::StringFn::Format( txt, _T( "%d / %d" ), m_pPersonInfoListBox->GetItemCount(), neFriend::nMaxShutOutCount );
	m_pFriendCnt->SetText( txt.c_str() );
}

void cFriendList::AddShutOut( const cFriendListContents::SHUTOUT_INFO* pInfo, int nRealDataIdx )
{
	SAFE_POINTER_RET(m_pPersonInfoListBox);

	// nRealDataIdx : 저장시킬 인덱스
	// 저장할 곳에 차단정보가 있다면 삭제 함. 현재 목록보다 큰걸 넣으면 목록 사이즈가 나옴
	int nIdx = DeletePerson( nRealDataIdx );

	CsPoint ptPos;
//	int nBase;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ti.s_Color = FONT_RED;

	cString* pString = NiNew cString;	
	//pString->SetValue1( nIdx );
	//pString->SetValue2( nRealDataIdx );		
	// 이름
	ti.SetText( pInfo->s_szName.c_str() );	
	pString->AddText( &ti );	
	// 메모
	cText::sTEXTINFO tiMemo;
	tiMemo.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	tiMemo.SetText( pInfo->s_szMemo.c_str() );	
	pString->AddText( &tiMemo, CsPoint(103, 0) );	


	cListBoxItem * addItem  = NiNew cListBoxItem;;
	addItem->SetItem( pString );
	addItem->SetUserData( new sListBoxIndexInfo( nRealDataIdx, nIdx ) );
	m_pPersonInfoListBox->InsertItem( nIdx, addItem );

}

int cFriendList::DeletePerson( int nDataIdx )
{
	// 인덱스에 정보가 있다면 삭제 하고 해당 인덱스 리턴.
	// 목록에 정보가 없을 때 목록 사이즈 리턴
	int nSize = m_pPersonInfoListBox->GetItemCount();

	if( nSize == 0 )
		return 0;

	for( int i = 0; i < nSize; i++ )
	{
		
		sListBoxIndexInfo* pUserData = dynamic_cast<sListBoxIndexInfo*>( m_pPersonInfoListBox->GetItemFormPos(i)->GetUserData() );
		assert_cs( pUserData != NULL );
		if( pUserData->sdataIndex == nDataIdx )
		{
			m_pPersonInfoListBox->RemoveItem(i);
			return i;
		}
	}		
	return m_pPersonInfoListBox->GetItemCount();
}

void cFriendList::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();

	if( m_eRadioIndex != (eTAB)m_pRadioBtn->GetCheckIndex() )
		_SetTab( (eTAB)m_pRadioBtn->GetCheckIndex() );
	cCreateName::UpdateInstance( cBaseWindow::WT_FRIEND_MEMO, 0 );
	cCreateName::UpdateInstance( cBaseWindow::WT_SHUTOUT_MEMO, 0 );
}

cBaseWindow::eMU_TYPE 
cFriendList::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn;

	// 메모 윈도우
	muReturn = cCreateName::Update_ForMouseInstance( cBaseWindow::WT_FRIEND_MEMO, 0 );
	if( muReturn != MUT_OUT_WINDOW )
		return muReturn;
	// 차단 윈도우
	muReturn = cCreateName::Update_ForMouseInstance( cBaseWindow::WT_SHUTOUT_MEMO, 0 );
	if( muReturn != MUT_OUT_WINDOW )
		return muReturn;
	// 탭 버튼 -> 목록 변경
	if( m_pRadioBtn->Update_ForMouse() != cRadioButton::INVALIDE_RADIO_CLICK )
	{
		return MUT_NONE;
	}
	
	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;
	// 닫기버튼
	if ( m_pCancelButton->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	// OnOff 체크버튼 -> 인원 수 변경 및 리스트 새로고침
	if( m_pOnOffCheck->Update_ForMouse() == cButton::ACTION_CLICK  )
	{
		return muReturn;
	}

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( m_pRoot->GetSize().x, 67 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();
	
	if( m_pPersonInfoListBox != null && m_pPersonInfoListBox->Update_ForMouse(CURSOR_ST.GetPos()) == true )
	{
		return muReturn;
	}
		

	return muReturn;
}

void cFriendList::_SetPopup( int nDataIdx )
{
	// 선택한 인원의 인덱스(보여지는 목록이 아닌 실제 저장된 인덱스)
	GetSystem()->SetCurSelect_DataIndex(nDataIdx);

	if( m_eRadioIndex == TAB_FRIEND && GetSystem() != null)	// 친구
	{
		g_pGameIF->GetPopup()->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::FRIEND_LIST );
	}
	else if( m_eRadioIndex == TAB_SHUTOUT && GetSystem() != null )	// 차단
	{
		g_pGameIF->GetPopup()->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::SHUTOUT_LIST );
	}
}

void cFriendList::_SetTab( eTAB eCurTab )
{
	m_eRadioIndex = eCurTab; 
	m_pRadioBtn->SetCheckIndex( (int)eCurTab ); 	

	switch( m_eRadioIndex )
	{
	case TAB_FRIEND:
		MakeFriend();
		break;
	case TAB_SHUTOUT:
		MakeShutOut();
		break;
	default:
		assert_cs( false );
		break;
	}
}
void cFriendList::Render()
{
	RenderScript();	

	cCreateName::RenderInstance( cBaseWindow::WT_FRIEND_MEMO, 0 );
	cCreateName::RenderInstance( cBaseWindow::WT_SHUTOUT_MEMO, 0 );
}

void cFriendList::ResetDevice()
{
	ResetDeviceScript();
}

bool cFriendList::OnEscapeKey()
{
	Close();
	return true;
}

void cFriendList::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();	
	ResetDevice();
}
bool cFriendList::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;
	GetSystem()->Register( SystemType::RenewTab_Friend, this );
	GetSystem()->Register( SystemType::RenewTab_Shutout, this );
	GetSystem()->Register( SystemType::AddMemo_Friend, this );
	GetSystem()->Register( SystemType::AddMemo_Shutout, this );
	GetSystem()->Register( SystemType::Friend_OnOffline, this );

	return true;
}
void cFriendList::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case cFriendListContents::RenewTab_Friend:	_SetTab( cFriendList::TAB_FRIEND );		break;
	case cFriendListContents::RenewTab_Shutout:	_SetTab( cFriendList::TAB_SHUTOUT );	break;
	case cFriendListContents::AddMemo_Friend:
		{
			int nVecterIndex;
			kStream >> nVecterIndex;
			if( m_eRadioIndex == cFriendList::TAB_FRIEND )
			{
				const cFriendListContents::FRIEND_INFO* pInfo = GetSystem()->GetFriendInfo(nVecterIndex);

				if(pInfo != null)
					AddFriend(pInfo, nVecterIndex);
			}
		}
		break;
	case cFriendListContents::AddMemo_Shutout:
		{
			int nVecterIndex;
			kStream >> nVecterIndex;
			if( m_eRadioIndex == cFriendList::TAB_SHUTOUT )
			{
				const cFriendListContents::SHUTOUT_INFO* pInfo = GetSystem()->GetShutInfo(nVecterIndex);
				if(pInfo != null)
					AddShutOut( pInfo, nVecterIndex );
			}

		}
		break;
	case cFriendListContents::Friend_OnOffline:
		{
			int nVecterIndex;
			kStream >> nVecterIndex;
			if( m_eRadioIndex == cFriendList::TAB_FRIEND )
			{
				const cFriendListContents::FRIEND_INFO* pInfo = GetSystem()->GetFriendInfo(nVecterIndex);
				if(pInfo != null)
					AddFriend( pInfo, nVecterIndex );
			}
		}
		break;
	default:
		break;

	}

}
void cFriendList::TabClick(void* pSender, void* pData)
{
	cWindow::PlaySound( cWindow::SD_At2 );		
	_SetTab((eTAB)m_pRadioBtn->GetCheckIndex());
}
void cFriendList::CloseClick(void* pSender, void* pData)
{
	Close();
}
void cFriendList::OfflineDisplayClick(void* pSender, void* pData)
{
	if (GetSystem() != null)
		GetSystem()->SetOnOff( m_pOnOffCheck->IsCheck() );
	switch( (eTAB)m_pRadioBtn->GetCheckIndex() )
	{
	case TAB_FRIEND:
		MakeFriend();
		break;
	case TAB_SHUTOUT:
		MakeShutOut();
		break;
	default:
		assert_cs( false );
		break;
	}
}
void cFriendList::TamerMenuPop(void* pSender, void* pData)
{
	if(m_pPersonInfoListBox->GetSelectedItem() != null)
	{
		sListBoxIndexInfo* pUserData = dynamic_cast<sListBoxIndexInfo*>( m_pPersonInfoListBox->GetSelectedItem()->GetUserData() );
		if( pUserData->sUIIndex < m_pPersonInfoListBox->GetItemCount() )
		{
			_SetPopup( pUserData->sdataIndex );
		}
	}
}
