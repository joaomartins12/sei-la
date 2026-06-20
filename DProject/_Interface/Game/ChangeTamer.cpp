
#include "stdafx.h"
#include "ChangeTamer.h"

#define IF_CHANGETAMER_DISCRIPT_WIDTH 255

cChangeTamer::cChangeTamer() :
m_pChangeList(NULL),
m_pCancelButton(NULL),
m_pOkButton(NULL)
{
}

cChangeTamer::~cChangeTamer()
{
}

bool cChangeTamer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eOpenChangeTamer,	this );
	GetSystem()->Register( SystemType::eChangeDiscript,		this );

	return true;
}

void cChangeTamer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eOpenChangeTamer:
		{
			int nInvenIndex = 0;
			std::list< int > lChangeList;
			kStream >> nInvenIndex >> lChangeList;
			_InitChangeListData( nInvenIndex, lChangeList );
		}
		break;
	case SystemType::eChangeDiscript:
		{
			std::wstring wsName;
			std::wstring wsComment;
			kStream >> wsName >> wsComment;
			_ChangeTamerDiscript( wsName, wsComment );
		}
		break;
	}
}

void cChangeTamer::Destroy()
{
	cBaseWindow::Delete();
}

void cChangeTamer::DeleteResource()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	DeleteScript();
}

void cChangeTamer::Create( int nValue /*=0*/ )
{
	if( cBaseWindow::Init() == false )
		return;

	if( 1 == nValue ) // 어드벤쳐로
		_CreateChangeAdvanture();
	else	// 세이버즈로
		_CreateChangeSavers( nValue );

	AddTitle( UISTRING_TEXT( "CHANGETAMER_TITLE_CHANGE_TAMER" ).c_str() );
	m_pCancelButton = AddButton( CsPoint( 196, 6 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	if( m_pCancelButton )
		m_pCancelButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cChangeTamer::_OnClickClose );

	m_sCommentUI.pCommentBg = AddSprite( CsPoint( 217, 5 ), CsPoint( 272, 208 ), "Control_G\\Popup\\NDlg5.tga" );
	m_sCommentUI.pNameBg = AddSprite( CsPoint( 217, 5 ), CsPoint( 272, 208 ), "CharCreate\\Discript.tga" );

	{	// 이름
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, FONT_GOLD );
		ti.s_eTextAlign = DT_CENTER;
		m_sCommentUI.pNameTxt = AddText( &ti, CsPoint( 353, 11 ) );
	}

	{	// 설명
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, FONT_WHITE );
		m_sCommentUI.pCommentTxt = AddText( &ti, CsPoint( 224, 49 ) );
	}

	m_sCommentUI.SetOnOff( false );
}

void cChangeTamer::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cChangeTamer::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( m_pChangeList && m_pChangeList->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return muReturn;

	if( m_pCancelButton && m_pCancelButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if( m_pOkButton && m_pOkButton->Update_ForMouse() == cButton::ACTION_CLICK )	
		return muReturn;

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( m_pRoot->GetSize().x, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cChangeTamer::Render()
{
	RenderScript();
}

void cChangeTamer::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

void cChangeTamer::ResetDevice()
{	
	ResetDeviceScript();
}

void cChangeTamer::_MakeChangeList(int const& nCount)
{
	if( NULL == m_pChangeList )
	{
		m_pChangeList = NiNew cListBox;
		SAFE_POINTER_RET( m_pChangeList );

		m_pChangeList->Init( m_pRoot, CsPoint( 2, 29 ), CsPoint( 214, 49 * nCount ), false );
		m_pChangeList->SetBackOverAndSelectedImgRender( true );
		m_pChangeList->SetMargin( CsPoint( 1, 1 ) );
		m_pChangeList->SetItemHeight( 49 );
		m_pChangeList->SetSelectedImg( "Talk\\Common_Seletbar.tga" );
		m_pChangeList->AddEvent( cListBox::LIST_SELECT_CHANGE_ITEM, this, &cChangeTamer::_OnClickChangeList );
		AddChildControl( m_pChangeList );
	}
	else
	{
		m_pChangeList->Init( m_pRoot, CsPoint( 2, 29 ), CsPoint( 214, 49 * nCount ), false );
		m_pChangeList->ResetDevice();
	}

	m_pChangeList->RemoveAllItem();
}

void cChangeTamer::_AddChangeInfo(int const& nIndex, int const& nTamerID)
{
	SAFE_POINTER_RET( m_pChangeList );

	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	cSprite* pTamerImg = NiNew cSprite;
	if( pTamerImg )
	{
		std::string strImgPath = GetSystem()->GetTamerIamgeS( nTamerID );
		pTamerImg->Init( NULL, CsPoint::ZERO, CsPoint( 47, 47 ), strImgPath.c_str(), false, NiColor::WHITE, false );
		cString::sSPRITE* pImg = pControl->AddSprite( pTamerImg );
		if( pImg )
			pImg->SetAutoPointerDelete( true );
	}

	{	// 테이머 이름
		std::wstring wsName = GetSystem()->GetTamerName( nTamerID );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.SetText( wsName.c_str() );
		cString::sTEXT* pText = pControl->AddText( &ti, CsPoint( 57, 10 ) );
		if( pText )
			pText->SetAutoPointerDelete( true );
	}

	{	// 테이머 소속
		std::wstring wsGroup;
		DmCS::StringFn::Format( wsGroup, L"%s : %s", UISTRING_TEXT( "CHANGETAMER_MEMBER_OF" ).c_str(), GetSystem()->GetTamerGroup( nTamerID ).c_str() );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.SetText( wsGroup.c_str() );
		cString::sTEXT* pText = pControl->AddText( &ti, CsPoint( 57, 25 ) );
		if( pText )
			pText->SetAutoPointerDelete( true );
	}

	cListBoxItem* pListItem = NiNew cListBoxItem( nIndex );
	if( pListItem )
	{
		pListItem->SetItem( pControl );
		pListItem->SetUserData( new sUIInfo( nTamerID, nIndex ) );
		m_pChangeList->AddItem( pListItem );
	}
}

//////////////////////////////////////////////////////////////////////////

void cChangeTamer::_CreateChangeAdvanture()
{
	InitScript( "_Etc\\ChangeTaeil_Win.tga", CsPoint( 200, 262 ), CsPoint( 217, 118 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );

	m_pOkButton = AddButton( CsPoint( 133, 86 ), CsPoint( 76, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );
	if( m_pOkButton )
	{
		m_pOkButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cChangeTamer::_OnClickChangeOK );
		m_pOkButton->SetEnable( false );
	}
}

void cChangeTamer::_CreateChangeSavers(int const& nValue)
{
	if( 3 == nValue )
	{
		InitScript( "_Etc\\ChangeTamer_Win.tga", CsPoint( 200, 262 ), CsPoint( 217, 218 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
		m_pOkButton = AddButton( CsPoint( 133, 184 ), CsPoint( 76, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );
	}
	else
	{
		InitScript( "_Etc\\AdChangeTamer_Win.tga", CsPoint( 200, 262 ), CsPoint( 217, 269 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
		m_pOkButton = AddButton( CsPoint( 133, 235 ), CsPoint( 76, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );
	}

	if( m_pOkButton )
	{
		m_pOkButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cChangeTamer::_OnClickChangeOK );
		m_pOkButton->SetEnable( false );
	}
}

void cChangeTamer::_InitChangeListData(int const& nInvenIndex, std::list< int >& lChangeList)
{
	if( lChangeList.empty() )
		return;

	if( m_pOkButton )
		m_pOkButton->SetUserData( new sUIInfo( nInvenIndex, 0/*Selected TamerID*/ ) );

	int nListCount = lChangeList.size();
	_MakeChangeList( nListCount );

	std::list< int >::iterator it = lChangeList.begin();
	for( int i = 0; it != lChangeList.end(); ++it, ++i )
		_AddChangeInfo( i, *it );
}

void cChangeTamer::_ChangeTamerDiscript(std::wstring const& wsName, std::wstring const& wsComment)
{
	m_sCommentUI.SetUI( wsName, wsComment );

	if( m_pOkButton )
		m_pOkButton->SetEnable( true );
}

void cChangeTamer::_OnClickChangeList(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cListBoxItem* pSelectItem = static_cast< cListBoxItem* >( pData );

	sUIInfo* pListInfo = dynamic_cast< sUIInfo* >( pSelectItem->GetUserData() );
	SAFE_POINTER_RET( pListInfo );

	if( m_pOkButton )
	{
		sUIInfo* pButtonInfo = dynamic_cast< sUIInfo* >( m_pOkButton->GetUserData() );
		if( pButtonInfo )
			pButtonInfo->nValue2 = pListInfo->nValue1;	// 선택된 테이머의 ID 전달
	}

	GetSystem()->SelectChangeList( pListInfo->nValue1 );
}

void cChangeTamer::_OnClickChangeOK(void* pSender, void* pData)
{
	SAFE_POINTER_RET( m_pOkButton );

	sUIInfo* pButtonInfo = dynamic_cast< sUIInfo* >( m_pOkButton->GetUserData() );
	SAFE_POINTER_RET( pButtonInfo );

	GetSystem()->ChangeTamer( pButtonInfo->nValue1, pButtonInfo->nValue2 );
}

void cChangeTamer::_OnClickClose(void* pSender, void* pData)
{
	SAFE_POINTER_RET( m_pOkButton );

	sUIInfo* pButtonInfo = dynamic_cast< sUIInfo* >( m_pOkButton->GetUserData() );
	SAFE_POINTER_RET( pButtonInfo );

	GetSystem()->CloseChangeWindow( pButtonInfo->nValue1 );
}

bool cChangeTamer::OnEscapeKey()
{
	SAFE_POINTER_RETVAL( m_pOkButton, false );

	sUIInfo* pButtonInfo = dynamic_cast< sUIInfo* >( m_pOkButton->GetUserData() );
	SAFE_POINTER_RETVAL( pButtonInfo, false );

	GetSystem()->CloseChangeWindow( pButtonInfo->nValue1 );

	return true;
}

//////////////////////////////////////////////////////////////////////////
void cChangeTamer::sCommentUI::SetOnOff(bool bIsOn)
{
	if( pCommentBg )
		pCommentBg->SetVisible( bIsOn );
	if( pCommentTxt )
		pCommentTxt->SetVisible( bIsOn );

	if( pNameBg )
		pNameBg->SetVisible( bIsOn );
	if( pNameTxt )
		pNameTxt->SetVisible( bIsOn );
}

void cChangeTamer::sCommentUI::SetUI(std::wstring const& wsName, std::wstring const& wsComment)
{
	if( pNameTxt )
		pNameTxt->SetText( wsName.c_str() );

	if( pCommentTxt )
		pCommentTxt->SetText( wsComment.c_str(), IF_CHANGETAMER_DISCRIPT_WIDTH );

	SetOnOff( true );
}
