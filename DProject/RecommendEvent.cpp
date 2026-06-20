#include "StdAfx.h"
#include "RecommendEvent.h"

cRecommendEvent::cRecommendEvent() :
m_pBtnClose( NULL ),
m_pRecommendCategory( NULL ),
mpkAchieves( NULL ),
m_pSelectedTamerNameItem( NULL ),
m_pSelectedSprite( NULL ),
m_pMouseOverSprite( NULL )
//,m_pMarkImage( NULL )
{
	mColor_Process = NiColor::WHITE;
	mColor_Receivable = NiColor( 1.0f, 1.0f, 0.0f);
	mColor_ReceiveComplete = NiColor( 192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f );
}

cRecommendEvent::~cRecommendEvent()
{
	Destroy();

	if( GetSystem() )	
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE cRecommendEvent::GetWindowType()
{
	return WT_RECOMMENDEVENT;
}

void cRecommendEvent::Destroy()
{
	cBaseWindow::Delete();
}

void cRecommendEvent::DeleteResource()
{
	DeleteScript();

//	SAFE_DELETE( m_pMarkImage );
}

void cRecommendEvent::Create(int nValue)
{
	cBaseWindow::Init();
	SetRootClient( CsPoint( 150, 165 ) );

	InitScript("RecommendEvent\\Win.tga", CsPoint(DEFAULT_SCREEN_WIDTH - 540, 200), CsPoint(384, 345), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false);
	AddTitle( UISTRING_TEXT( "RECOMMENDEVENT_TITLE_RECOMMEND_EVENT" ).c_str() );
	
	m_pBtnClose = AddButton( CsPoint( 360, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	m_pBtnClose->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cRecommendEvent::PressCloseButton );

	//m_pMarkImage = NiNew cImage;
	//m_pMarkImage->Init( NULL, CsPoint::ZERO, CsPoint( 15, 15 ), "System\\Check.tga", false );
	//m_pMarkImage->SetTexToken( CsPoint( 0, 20 ) );

	m_pSelectedSprite = AddSprite( CsPoint::ZERO, CsPoint( 200, 18 ), "Talk\\Common_Seletbar.tga" );
	m_pSelectedSprite->SetAlpha( 0.f );

	m_pMouseOverSprite = AddSprite( CsPoint::ZERO, CsPoint( 200, 18 ), "Talk\\Common_Seletbar.tga" );
	m_pMouseOverSprite->SetAlpha( 0.f );

	m_pRecommendCategory = NiNew cTreeBox;
	m_pRecommendCategory->Init( GetRoot(), CsPoint(25, 55), CsPoint( 335, 230 ), NULL, false );
	m_pRecommendCategory->setItemGab( CsPoint(0, 2) );
	
	m_pRecommendCategory->AddEvent( cTreeBox::TreeEvent_Item_MouseUp, this, &cRecommendEvent::OnClickBTInTreeItem );
	m_pRecommendCategory->AddEvent( cTreeBox::TreeEvent_ItemToggle_Changed, this, &cRecommendEvent::ClickTopCategory );
	m_pRecommendCategory->AddEvent( cTreeBox::TreeEvent_Selection_Changed, this, &cRecommendEvent::ClickTreeItem );
	m_pRecommendCategory->AddEvent( cTreeBox::TreeEvent_MouseOverItem_Change, this, &cRecommendEvent::ChangeMouseOverCategory );
	m_pRecommendCategory->AddEvent( cTreeBox::TreeEvent_MouseOverItem_Release, this, &cRecommendEvent::ReleaseMouseOverCategory );
	AddChildControl(m_pRecommendCategory);

	cScrollBar* pScrollBar = NiNew cScrollBar;
	pScrollBar->Init( cScrollBar::TYPE_1, NULL, CsPoint( 348, 45 ), CsPoint( 16, 252 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 10, 37 ), CsPoint( 280, 403 ) ), 11, false );
	m_pRecommendCategory->SetScrollBar( pScrollBar );

	MakeRecommendMainCategory( Recommend::Type::Sent );
	MakeRecommendMainCategory( Recommend::Type::Received );
	MakeAchieveTreeBox();
}

void cRecommendEvent::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();

	if( GetSystem() && GetSystem()->IsPossibleRecommend() )
		cCreateName::UpdateInstance( cBaseWindow::WT_RECOMMEND_NAME, 1 );
}

void cRecommendEvent::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

cBaseWindow::eMU_TYPE cRecommendEvent::Update_ForMouse()
{
	SAFE_POINTER_RETVAL( GetSystem(), MUT_OUT_WINDOW );

	cBaseWindow::eMU_TYPE muReturn;
	if( GetSystem()->IsPossibleRecommend() ) // 추천 가능한 상태이면
	{
		muReturn = cCreateName::Update_ForMouseInstance( cBaseWindow::WT_RECOMMEND_NAME, 1 );
			
		if( muReturn != MUT_OUT_WINDOW )
			return muReturn;
	}
	else
	{
		muReturn = cBaseWindow::Update_ForMouse();
	
		if( muReturn == MUT_OUT_WINDOW )	
			return muReturn;

		if( m_pBtnClose && m_pBtnClose->Update_ForMouse() == cButton::ACTION_CLICK )
			return muReturn;
		
		if( m_pRecommendCategory && m_pRecommendCategory->Update_ForMouse( CURSOR_ST.GetPos() ) )
			return muReturn;

		_UpdateMoveWindow_ForMouse();
	}
	return muReturn;
}

void cRecommendEvent::Render()
{
	SAFE_POINTER_RET( GetSystem() );
	if( GetSystem()->IsPossibleRecommend() )
	{
		cCreateName::RenderInstance( cBaseWindow::WT_RECOMMEND_NAME, 1 );
	}
	else
	{
		RenderScript();

		cBaseWindow::EndRenderScript();
	}
}

void cRecommendEvent::ResetDevice()
{
	ResetDeviceScript();
}

bool cRecommendEvent::Construct( void )
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eChange_PurposeState, this );
	GetSystem()->Register( SystemType::eChange_TamerName, this );
	return true;
}

void cRecommendEvent::Notify( int const& iNotifiedEvt, ContentsStream const& kStream )
{
	switch( iNotifiedEvt )
	{
	case CRecommendEvent_Contents::eChange_PurposeState:
		{
			int nMarkIndex( 0 );
			int nMarkValue( 0 );
			int nRecommendType( 0 );
			std::wstring strTamerName;
			kStream >> nMarkIndex >> nMarkValue >> nRecommendType >> strTamerName;

			int TreeItemViewerIdx = nMarkIndex - 1;
			cString* pString = GetPurposeString( TreeItemViewerIdx );
			SAFE_POINTER_RET( pString );

			UpdateAchieveTreeItem( pString, nRecommendType, TreeItemViewerIdx, nMarkValue, strTamerName );
		}break;
	case CRecommendEvent_Contents::eChange_TamerName:
		{
			std::wstring BefoleName;
			std::wstring NewName;
			kStream >> BefoleName >> NewName;

			ChangeTamerNameString( BefoleName, NewName );
			ResetAchieveTreeItem( NewName );
			mpkAchieves->setIsOpen( true );
		}break;
	}
}
// 메인 카테고리
void cRecommendEvent::MakeRecommendMainCategory( int nRecommendType )
{
	cText::sTEXTINFO tiGroup;
	tiGroup.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	tiGroup.SetText( nRecommendType == Recommend::Type::Sent ? UISTRING_TEXT( "RECOMMENDEVENT_RECOMMEND_BY_FRIENDLY" ).c_str() : UISTRING_TEXT( "RECOMMENDEVENT_RECOMMEND_BY_ME" ).c_str() );

	cString* pTitleString = NiNew cString;
	SAFE_POINTER_RET( pTitleString );
	pTitleString->TailAddSizeX( 5 );
	pTitleString->AddText( &tiGroup, CsPoint(20, 0) );

	cTreeBoxItem* pAddItem = NiNew cTreeBoxItem;
	SAFE_POINTER_RET( pAddItem );
	pAddItem->SetItem( pTitleString );
	pAddItem->SetUserData( new sRecommendData( Recommend::ItemType::TamerNameCategory ) );

	cButton* pOpenBtn = NiNew cButton;
	SAFE_POINTER_RET( pOpenBtn );
	pOpenBtn->Init( NULL, CsPoint::ZERO, CsPoint( 12, 12), "Item_Make\\Make_Slot_Plus.tga", false );
	pOpenBtn->SetTexToken(CsPoint(0,14));
	pAddItem->SetOpenButton( pOpenBtn, CsPoint(5, 0) );

	cButton* pCloseBtn = NiNew cButton;
	SAFE_POINTER_RET( pCloseBtn );
	pCloseBtn->Init( NULL, CsPoint::ZERO, CsPoint( 12, 12), "Item_Make\\Make_Slot_Minus.tga", false );
	pCloseBtn->SetTexToken(CsPoint(0,14));
	pAddItem->SetCloseButton( pCloseBtn, CsPoint(5, 0) );

	std::list< CRecommendEvent_Contents::ST_RecommendInfo > RecommendInfoList = GetSystem()->GetRecommendInfoList();
	std::list< CRecommendEvent_Contents::ST_RecommendInfo >::const_iterator iter = RecommendInfoList.begin();
	for( ; iter != RecommendInfoList.end(); ++iter )
	{
		CRecommendEvent_Contents::ST_RecommendInfo info = (*iter);

		if( nRecommendType == info.m_nRecommendType )
			MakeRecommendSubCategory( *pAddItem, info.m_nRecommendType, info.m_strTamerName );
	}

	if(m_pRecommendCategory)
		m_pRecommendCategory->AddItem( pAddItem );
}

void cRecommendEvent::MakeRecommendSubCategory( cTreeBoxItem& Root, const int nRecommendType, const std::wstring strName )
{
	cText::sTEXTINFO tiGroup;
	tiGroup.Init( CFont::FS_10 );
	tiGroup.SetText( strName.data() );

	cString* pString = NiNew cString;
	SAFE_POINTER_RET( pString );
	pString->AddText( &tiGroup, CsPoint(25, 0) );

	cTreeBoxItem* pAddItem = NiNew cTreeBoxItem;
	SAFE_POINTER_RET( pAddItem );
	pAddItem->SetItem( pString );
	pAddItem->SetUserData( new sRecommendData( Recommend::ItemType::TamerName, strName ) );

	Root.AddChildItem( pAddItem );
}

void cRecommendEvent::MakeAchieveTreeBox()
{
	cText::sTEXTINFO tiGroup;
	tiGroup.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	tiGroup.SetText( UISTRING_TEXT( "RECOMMENDEVENT_PURPOSE_INFO" ).c_str() );
	
	cString* pTitleString = NiNew cString;
	SAFE_POINTER_RET( pTitleString );
	pTitleString->TailAddSizeX( 5 );
	pTitleString->AddText( &tiGroup, CsPoint(20, 0) );

	mpkAchieves = NiNew cTreeBoxItem;
	SAFE_POINTER_RET( mpkAchieves );
	mpkAchieves->SetItem( pTitleString );
	mpkAchieves->SetUserData( new sRecommendData( Recommend::ItemType::PurposeCategory ) );

	cButton* pOpenBtn = NiNew cButton;
	SAFE_POINTER_RET( pOpenBtn );
	pOpenBtn->Init( NULL, CsPoint::ZERO, CsPoint( 12, 12), "Item_Make\\Make_Slot_Plus.tga", false );
	pOpenBtn->SetTexToken(CsPoint(0,14));
	mpkAchieves->SetOpenButton( pOpenBtn, CsPoint(5, 0) );

	cButton* pCloseBtn = NiNew cButton;
	SAFE_POINTER_RET( pCloseBtn );
	pCloseBtn->Init( NULL, CsPoint::ZERO, CsPoint( 12, 12), "Item_Make\\Make_Slot_Minus.tga", false );
	pCloseBtn->SetTexToken(CsPoint(0,14));
	mpkAchieves->SetCloseButton( pCloseBtn, CsPoint(5, 0) );

	if(m_pRecommendCategory)
		m_pRecommendCategory->AddItem( mpkAchieves );
}

cString* cRecommendEvent::MakeAchieveContents(int TableIdx,  int nMarkIndex, int nMarkValue, int nRecommendType, const std::wstring& strTamerName )
{
	cString* pString = NiNew cString;
	SAFE_POINTER_RETVAL( pString, NULL );

	AddPurposeText( pString, nMarkIndex, nRecommendType, strTamerName );

	MakePurposeElement( pString, nMarkIndex + 1, nMarkValue );
 	
	return pString;
}
cTreeBoxItem* cRecommendEvent::InitAchieveTreeItem( cString* pkContents, int TreeItemIdx )
{
	SAFE_POINTER_RETVAL( pkContents, NULL );

	cTreeBoxItem* pItem = NiNew cTreeBoxItem( TreeItemIdx );
	SAFE_POINTER_RETVAL( pItem, NULL );

	pItem->SetItem( pkContents );
	mpkAchieves->AddChildItem( pItem );
	return pItem;
}
void cRecommendEvent::UpdateAchieveTreeItem( cString* pPurposeString, int RecommendType, int nMarkIndex, int nMarkValue, std::wstring strTamerName )
{
	SAFE_POINTER_RET( GetSystem() );
	SAFE_POINTER_RET( pPurposeString );

	cString::sELEMENT* pElement = pPurposeString->FindElement( cString::sELEMENT::TEXT );
	SAFE_POINTER_RET(pElement);
	cString::sTEXT* pText = dynamic_cast< cString::sTEXT* >( pElement );
	SAFE_POINTER_RET(pText);
	std::wstring pTCHAR = MakePurposeText( RecommendType, nMarkIndex, strTamerName);
	pText->s_Text.SetText(pTCHAR.c_str());

	switch( nMarkValue )
	{
	case Recommend::MarkValue::Uncompleted:
		{
			//cString::sELEMENT* pImageElement = pPurposeString->FindElement( cString::sELEMENT::IMAGE );
			//SAFE_POINTER_RET( pImageElement );
			//cString::sIMAGE* pImage = dynamic_cast< cString::sIMAGE* >( pImageElement );
			//SAFE_POINTER_RET(pImage);
			//pImage->SetImangeIndex( 0 );
			//pImage->SetAlpha( 1.f );

			cString::sELEMENT* pButtonElement = pPurposeString->FindElement( cString::sELEMENT::BUTTON );
			SAFE_POINTER_RET( pButtonElement );

			cString::sBUTTON* pButton = dynamic_cast< cString::sBUTTON* >( pButtonElement );
			SAFE_POINTER_RET( pButton );
			SAFE_POINTER_RET( pButton->s_pButton );
			pButton->s_pButton->SetTextColor(mColor_Process);
			pButton->s_pButton->SetText( UISTRING_TEXT( "RECOMMENDEVENT_ONGOING" ).c_str() );
			pButton->s_pButton->SetEnable( false );
		}
		break;
	case Recommend::MarkValue::Completed:
		{
			//cString::sELEMENT* pImageElement = pPurposeString->FindElement( cString::sELEMENT::IMAGE );
			//SAFE_POINTER_RET( pImageElement );
			//cString::sIMAGE* pImage = dynamic_cast< cString::sIMAGE* >( pImageElement );
			//SAFE_POINTER_RET(pImage);
			//pImage->SetImangeIndex( 3 );
			//pImage->SetAlpha( 1.f );

			cString::sELEMENT* pButtonElement = pPurposeString->FindElement( cString::sELEMENT::BUTTON );
			SAFE_POINTER_RET( pButtonElement );

			cString::sBUTTON* pButton = dynamic_cast< cString::sBUTTON* >( pButtonElement );
			SAFE_POINTER_RET( pButton );
			SAFE_POINTER_RET( pButton->s_pButton );
			pButton->s_pButton->SetTextColor(mColor_Receivable);
			pButton->s_pButton->SetText( UISTRING_TEXT( "RECOMMENDEVENT_RECEIVE_REWARD" ).c_str() );
			pButton->s_pButton->SetEnable( true );
		}
		break;
	case Recommend::MarkValue::ReceivedReward:
		{
			//cString::sELEMENT* pImageElement = pPurposeString->FindElement( cString::sELEMENT::IMAGE );
			//SAFE_POINTER_RET( pImageElement );
			//cString::sIMAGE* pImage = dynamic_cast< cString::sIMAGE* >( pImageElement );
			//SAFE_POINTER_RET(pImage);
			//pImage->SetAlpha( 0.f );
			
			cString::sELEMENT* pButtonElement = pPurposeString->FindElement( cString::sELEMENT::BUTTON );
			SAFE_POINTER_RET( pButtonElement );
			cString::sBUTTON* pButton = dynamic_cast< cString::sBUTTON* >( pButtonElement );
			SAFE_POINTER_RET( pButton );
			SAFE_POINTER_RET( pButton->s_pButton );
			pButton->s_pButton->SetTextColor(mColor_ReceiveComplete);
			pButton->s_pButton->SetText( UISTRING_TEXT( "RECOMMENDEVENT_RECEIVED" ).c_str() );
			pButton->s_pButton->SetEnable( false );
		}
		break;

	}
}

void cRecommendEvent::ResetAchieveTreeItem( const std::wstring& strTamerName )
{
	if( strTamerName.empty() )
		return;

	SAFE_POINTER_RET( GetSystem() );
	CRecommendEvent_Contents::ST_RecommendInfo RecommendInfo;
	if( GetSystem()->GetRecommendInfo( RecommendInfo, strTamerName ) == false )
		return;

	std::list< CRecommendEvent_Contents::ST_RecommendTableData > TableDataList;
	if( GetSystem()->GetTableDataList( TableDataList, RecommendInfo.m_nRecommendType ) == false )
		return;

	std::list< CRecommendEvent_Contents::ST_RecommendTableData >::iterator kIter = TableDataList.begin();
	int PurposeViewerIdx = 0;
	for(; kIter != TableDataList.end(); ++kIter, ++PurposeViewerIdx)
	{
		cString* pPurposeString = GetPurposeString( PurposeViewerIdx );
		SAFE_POINTER_RET( pPurposeString );

		CRecommendEvent_Contents::ST_RecommendTableData* pkRTData = &(*kIter);
		SAFE_POINTER_RET( pkRTData );

		if(pkRTData->m_nIndex >= nLimit::MAX_FRIENDLY_MARK)
			continue;

		UpdateAchieveTreeItem( pPurposeString, RecommendInfo.m_nRecommendType, PurposeViewerIdx, RecommendInfo.AchieveState[ pkRTData->m_nIndex ], RecommendInfo.m_strTamerName );

	}
}

bool cRecommendEvent::AddPurposeText( cString* pPurposeString, int nMarkIndex, int nRecommendType, const std::wstring strTamerName )
{
	SAFE_POINTER_RETVAL( pPurposeString, false );

	std::wstring pPurposeText = MakePurposeText( nRecommendType, nMarkIndex, strTamerName );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ti.SetText( pPurposeText.c_str() );
	pPurposeString->AddText( &ti, CsPoint(25, 0) );
	return true;
}

std::wstring cRecommendEvent::MakePurposeText( int nRecommendType, int nMarkIndex, std::wstring strTamerName /*= L"" */ )
{
	SAFE_POINTER_RETVAL( GetSystem(), NULL );
	CRecommendEvent_Contents::ST_RecommendTableData TableData;
	if( GetSystem()->GetRecommendTableData( TableData, nRecommendType, nMarkIndex ) == false )
		return NULL;

	std::wstring wsText;
	if( nRecommendType == Recommend::Type::Sent )
	{
		switch( TableData.m_nSuccessType )
		{
		case Recommend::SuccessType::LevelUp:
			{
				wsText = UISTRING_TEXT( "RECOMMENDEVENT_ACHIEVE_LEVEL" );
				DmCS::StringFn::Replace( wsText, L"#Level#", TableData.m_nSuccessValue );
			}
			break;
		case Recommend::SuccessType::NewArea:
			{
				wsText = UISTRING_TEXT( "RECOMMENDEVENT_NEW_AREA" );
				DmCS::StringFn::Replace( wsText, L"#Num#", TableData.m_nSuccessValue );	
			}
			break;
		case Recommend::SuccessType::Achieve:
			{
				wsText = UISTRING_TEXT( "RECOMMENDEVENT_SUCCESS_ACHIEVE" );
				DmCS::StringFn::Replace( wsText, L"#Num#", TableData.m_nSuccessValue );	
			}
			break;
		}
	}
	else if( nRecommendType == Recommend::Type::Received )
	{
		switch( TableData.m_nSuccessType )
		{
		case Recommend::SuccessType::LevelUp:
			{
				wsText = UISTRING_TEXT( "RECOMMENDEVENT_TAMER_LEVELUP" );
				DmCS::StringFn::Replace( wsText, L"#Name#", strTamerName.c_str() );
				DmCS::StringFn::Replace( wsText, L"#Level#", TableData.m_nSuccessValue );
			}
			break;
		case Recommend::SuccessType::NewArea:
			{
				wsText = UISTRING_TEXT( "RECOMMENDEVENT_TAMER_NEW_AREA" );
				DmCS::StringFn::Replace( wsText, L"#Name#", strTamerName.c_str() );
				DmCS::StringFn::Replace( wsText, L"#Num#", TableData.m_nSuccessValue );
			}
			break;
		case Recommend::SuccessType::Achieve:
			{
				wsText = UISTRING_TEXT( "RECOMMENDEVENT_TAMER_SUCCESS_ACHIEVE" );
				DmCS::StringFn::Replace( wsText, L"#Name#", strTamerName.c_str() );
				DmCS::StringFn::Replace( wsText, L"#Num#", TableData.m_nSuccessValue );
			}
			break;
		}
	}
	return wsText;
}

bool cRecommendEvent::MakePurposeElement( cString* pPurposeString, int nPurposeIndex, int nMarkValue )
{
	SAFE_POINTER_RETVAL( pPurposeString, false );
	//SAFE_POINTER_RETVAL( m_pMarkImage, false );
	
	CsPoint ptSize;
	int nBase;
	cButton* pRewardButton( NULL );

	//pPurposeString->AddImage( m_pMarkImage, 0, CsPoint( 210, 2) ); // 보상 상태 마크 이미지

	switch( nMarkValue )
	{
	case Recommend::MarkValue::Uncompleted:
		{
			cText::sTEXTINFO ButtonTextInfo;
			ButtonTextInfo.Init( &g_pEngine->m_FontText, CFont::FS_10, mColor_Process );
			ButtonTextInfo.SetText( UISTRING_TEXT( "RECOMMENDEVENT_ONGOING" ).c_str() );
			cText::GetStringSize( &ButtonTextInfo, ptSize, nBase, ButtonTextInfo.GetText() );

			pRewardButton = NiNew cButton;
			SAFE_POINTER_RETVAL( pRewardButton, false );
			pRewardButton->Init( NULL, CsPoint::ZERO, CsPoint( 55, 18 ), cButton::IMAGE_NOR_1, &ButtonTextInfo, false, CsPoint( 0, 0) );
			pRewardButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cRecommendEvent::PressRewardButton );
			pRewardButton->SetUserData( new sAchieveTreeItemInfo( nPurposeIndex ) );
			pRewardButton->SetEnable( false );
		}break;

	case Recommend::MarkValue::Completed:
		{
			cText::sTEXTINFO ButtonTextInfo;
			ButtonTextInfo.Init( &g_pEngine->m_FontText, CFont::FS_10, mColor_Receivable);
			ButtonTextInfo.SetText( UISTRING_TEXT( "RECOMMENDEVENT_RECEIVE_REWARD" ).c_str() );
			cText::GetStringSize( &ButtonTextInfo, ptSize, nBase, ButtonTextInfo.GetText() );

			pRewardButton = NiNew cButton;
			SAFE_POINTER_RETVAL( pRewardButton, false );
			pRewardButton->Init( NULL, CsPoint::ZERO, CsPoint( 55, 18 ), cButton::IMAGE_NOR_1, &ButtonTextInfo, false, CsPoint( 0, 0) );
			pRewardButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cRecommendEvent::PressRewardButton );
			pRewardButton->SetUserData( new sAchieveTreeItemInfo( nPurposeIndex ) );
			pRewardButton->SetEnable( true );

		}break;

	case Recommend::MarkValue::ReceivedReward:
		{
			cText::sTEXTINFO ButtonTextInfo;
			ButtonTextInfo.Init( &g_pEngine->m_FontText, CFont::FS_10, mColor_ReceiveComplete );
			ButtonTextInfo.SetText( UISTRING_TEXT( "RECOMMENDEVENT_RECEIVED" ).c_str() );
			cText::GetStringSize( &ButtonTextInfo, ptSize, nBase, ButtonTextInfo.GetText() );

			pRewardButton = NiNew cButton;
			SAFE_POINTER_RETVAL( pRewardButton, false );
			pRewardButton->Init( NULL, CsPoint::ZERO, CsPoint( 55, 18 ), cButton::IMAGE_NOR_13_R, &ButtonTextInfo, false, CsPoint( 0, 0) );
			pRewardButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cRecommendEvent::PressRewardButton );
			pRewardButton->SetUserData( new sAchieveTreeItemInfo( nPurposeIndex ) );
			pRewardButton->SetEnable( false );
		}break;
	}

	cString::sBUTTON* pButton = pPurposeString->AddButton( pRewardButton, nMarkValue,  CsPoint( 235, 0 ), CsPoint( 55, 18 ) );
	SAFE_POINTER_RETVAL( pButton, NULL );
	pButton->m_ptTextPos = pRewardButton->GetClient() + ( CsPoint(55, 18 ) / 2 ) - (ptSize / 2) ;

	return true;
}
void cRecommendEvent::OnClickBTInTreeItem(void* pSender, void* pData )
{
	SAFE_POINTER_RET( pSender);
	SAFE_POINTER_RET( pData );
	cString::sBUTTON* pRewardButton = static_cast< cString::sBUTTON* >(pData);
	SAFE_POINTER_RET( pRewardButton );
	SAFE_POINTER_RET( pRewardButton->s_pButton );
	
	pRewardButton->s_pButton->SetEnable( false );
	sAchieveTreeItemInfo* pkInfo = dynamic_cast<sAchieveTreeItemInfo*>(pRewardButton->s_pButton->GetUserData());
	GetSystem()->SendMarkItemInfo( pkInfo->index  );
}

void cRecommendEvent::ClickTreeItem( void* pSender, void* pData )
{
	SAFE_POINTER_RET( pSender);
	SAFE_POINTER_RET( pData );

	cTreeBoxItem* pCurrSelectedItem = static_cast <cTreeBoxItem* >( pData );
	SAFE_POINTER_RET( pCurrSelectedItem );
	sRecommendData* pUserData = dynamic_cast< sRecommendData* >( pCurrSelectedItem->GetUserData() );
	SAFE_POINTER_RET(pUserData);

	switch( pUserData->GetItemType() )
	{
	case 	Recommend::ItemType::TamerName:
		{
			GetSystem()->SetSelectedTamerName( pUserData->m_strTamerName );

			if(mpkAchieves->getItemList().empty()) // 최초 생성이면
				CreateAchieveTreeItem( pUserData->m_strTamerName );
			ResetAchieveTreeItem( pUserData->m_strTamerName );

			ShowSelectedImage( pCurrSelectedItem );				// 아이템에 선택 이미지를 설정
			SetSelectedTamerNameItem( pCurrSelectedItem );

			if( mpkAchieves )						// 목표 카테고리를 오픈
				mpkAchieves->setIsOpen( true );
		}break;
	}
}

void cRecommendEvent::ClearAchieveTreeBox()
{
	SAFE_POINTER_RET( mpkAchieves );
	mpkAchieves->DeleteAllChildItem();
}

void cRecommendEvent::CreateAchieveTreeItem( const std::wstring& strTamerName )
{
	if( strTamerName.empty() )
		return;

	SAFE_POINTER_RET( GetSystem() );
	CRecommendEvent_Contents::ST_RecommendInfo RecommendInfo;
	if( GetSystem()->GetRecommendInfo( RecommendInfo, strTamerName ) == false )
		return;

	std::list< CRecommendEvent_Contents::ST_RecommendTableData > TableDataList;
	if( GetSystem()->GetTableDataList( TableDataList, RecommendInfo.m_nRecommendType ) == false )
		return;

	// 목표 생성
	std::list< CRecommendEvent_Contents::ST_RecommendTableData >::const_iterator iter = TableDataList.begin();
	for( int i = 0; iter != TableDataList.end(); ++iter, ++i )
	{
		cString*		pAchieveContents= MakeAchieveContents(iter->m_nIndex,  i, RecommendInfo.AchieveState[ iter->m_nIndex ], RecommendInfo.m_nRecommendType, RecommendInfo.m_strTamerName );
		cTreeBoxItem*	pAchieveTreeItem	= InitAchieveTreeItem( pAchieveContents, i );
	}
}

void cRecommendEvent::PressCloseButton( void* pSender, void* pData )
{
	Close();
}

void cRecommendEvent::PressRewardButton( void* pSender, void* pData )
{
	SAFE_POINTER_RET( GetSystem() );
	SAFE_POINTER_RET( pSender );
	cButton* pRewardButton = static_cast<cButton*>(pSender);
	pRewardButton->SetEnable( false );

	sAchieveTreeItemInfo* pksAchieveTreeItemInfo = dynamic_cast<sAchieveTreeItemInfo*>(pRewardButton->GetUserData());
	GetSystem()->SendMarkItemInfo( pksAchieveTreeItemInfo->index );
}

void cRecommendEvent::ChangeMouseOverCategory( void* pSender, void* pData )
{
	SAFE_POINTER_RET( pData );

	cTreeBoxItem* pCurrOverItem = static_cast< cTreeBoxItem* >( pData );
	SAFE_POINTER_RET( pCurrOverItem );
	cString* pItemString = pCurrOverItem->GetItem();
	SAFE_POINTER_RET( pItemString );

	cString::sELEMENT* pElement = pItemString->FindElement( cString::sELEMENT::TEXT );
	cString::sTEXT* pText = dynamic_cast<cString::sTEXT*>( pElement );
	SAFE_POINTER_RET( pText );

	CsPoint TextPos = pText->s_Text.GetSpritePos();
	SAFE_POINTER_RET( m_pMouseOverSprite );
 	m_pMouseOverSprite->SetPos( TextPos + CsPoint( -5, -3 ) - GetRootClient() );
 	m_pMouseOverSprite->SetAlpha( 0.5f );
}

void cRecommendEvent::ReleaseMouseOverCategory( void* pSender, void* pData )
{
	SAFE_POINTER_RET( m_pMouseOverSprite );
	m_pMouseOverSprite->SetAlpha( 0.f );
}

void cRecommendEvent::ChangeTamerNameString( std::wstring BefoleName, std::wstring NewName )
{
	std::list<cTreeBoxItem*> AllItemList;
	m_pRecommendCategory->GetItemList( AllItemList );

	std::list<cTreeBoxItem*>::iterator iter = AllItemList.begin();
	for(; iter != AllItemList.end(); ++iter )
	{
		sRecommendData* RecommendData = dynamic_cast<sRecommendData*>( (*iter)->GetUserData() );
		SAFE_POINTER_CON( RecommendData );

		if( RecommendData->m_strTamerName.compare( BefoleName ) == 0 )
		{
			cString* pString = (*iter)->GetItem();
			SAFE_POINTER_CON( pString );

			cString::sELEMENT* pElement = pString->FindElement( cString::sELEMENT::TEXT );
			SAFE_POINTER_CON( pElement );

			cString::sTEXT* pText = dynamic_cast<cString::sTEXT* >( pElement );
			SAFE_POINTER_CON( pText );

			pText->SetText( NewName.data() );
		}
	}
}

void cRecommendEvent::ShowSelectedImage( cTreeBoxItem* pSelectedItem )
{
	SAFE_POINTER_RET( pSelectedItem );
	cString* pItemString = pSelectedItem->GetItem();
	SAFE_POINTER_RET( pItemString );

	cString::sELEMENT* pElement = pItemString->FindElement( cString::sELEMENT::TEXT );
	cString::sTEXT* pText = dynamic_cast<cString::sTEXT*>( pElement );
	SAFE_POINTER_RET( pText );

	CsPoint TextPos = pText->s_Text.GetSpritePos();
	SAFE_POINTER_RET( m_pMouseOverSprite );
	m_pSelectedSprite->SetPos( TextPos + CsPoint( -5, -3 ) - GetRootClient());
	m_pSelectedSprite->SetAlpha( 0.5f );
}

void cRecommendEvent::HideSelectedImage()
{
	SAFE_POINTER_RET( m_pSelectedSprite );
	m_pSelectedSprite->SetAlpha( 0.f );
}

void cRecommendEvent::SetSelectedTamerNameItem( cTreeBoxItem* pItem )
{
	SAFE_POINTER_RET( pItem );
	m_pSelectedTamerNameItem = pItem;
}

std::wstring cRecommendEvent::GetSelectedItemText()
{
	SAFE_POINTER_RETVAL( m_pSelectedTamerNameItem, std::wstring() );
	return m_pSelectedTamerNameItem->GetItemText();
}

cString* cRecommendEvent::GetPurposeString( int nIndex )
{
	std::list<cTreeBoxItem*> BoxItemList = mpkAchieves->getItemList();
	std::list<cTreeBoxItem*>::const_iterator iter = BoxItemList.begin();
	for( ; iter != BoxItemList.end(); ++iter )
	{
		if( (*iter)->getID() == nIndex )
			return (*iter)->GetItem();
	}
	return NULL;
}

void cRecommendEvent::ClickTopCategory( void* pSender, void* pData )
{
	SAFE_POINTER_RET( pSender);
	SAFE_POINTER_RET( pData );

	cTreeBoxItem* pCurrSelectedItem = static_cast < cTreeBoxItem* >( pData );
	SAFE_POINTER_RET( pCurrSelectedItem );
	sRecommendData* pUserData = dynamic_cast< sRecommendData* >( pCurrSelectedItem->GetUserData() );
	SAFE_POINTER_RET(pUserData);

	switch( pUserData->GetItemType() )
	{
	case 	Recommend::ItemType::TamerNameCategory:
		{
			std::list<cTreeBoxItem*> TreeItemList = pCurrSelectedItem->getItemList();
			std::list<cTreeBoxItem*>::const_iterator it = TreeItemList.begin();
			for( ; it != TreeItemList.end(); ++it )
			{
				if( (*it)->GetItemText() == GetSelectedItemText() )
				{
					HideSelectedImage();
					break;
				}
			}
		}break;
	case 	Recommend::ItemType::PurposeCategory:
		{
		}break;
	}
}

const int cRecommendEvent::sRecommendData::GetItemType() const
{
	assert_cs( Recommend::ItemType::None < m_nTreeItemType && m_nTreeItemType < Recommend::ItemType::Max );
	return m_nTreeItemType;
}
