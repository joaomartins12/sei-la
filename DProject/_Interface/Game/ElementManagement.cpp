#include "stdafx.h"
#include "ElementManagement.h"

#define IF_ELEMENT_PROCESS_TIME 3.5f

cElementManagement::cElementManagement() :
m_pListBox( NULL ), m_pHelpButton( NULL )
{
	Construct();
}

cElementManagement::~cElementManagement()
{
	Destroy();
}

bool cElementManagement::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eRegistEquip, this );
	GetSystem()->Register( SystemType::eRegistElem, this );
	GetSystem()->Register( SystemType::eSelectSocket, this );
	GetSystem()->Register( SystemType::eReleaseSocket, this );
	GetSystem()->Register( SystemType::eSuccessAttach, this );
	GetSystem()->Register( SystemType::eSuccessDetach, this );

	GetSystem()->Register( SystemType::eRegistCheckItem, this );
	GetSystem()->Register( SystemType::eSuccessAnalysis, this );

	GetSystem()->Register( SystemType::eResetWindow, this );

	return true;
}

void cElementManagement::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eRegistEquip:
		{
			uint nInvenIndex = 0;
			uint nItemType = 0;
			int nSocketCount = 0;
			std::wstring wsItemName;
			kStream >> nInvenIndex >> nSocketCount >> wsItemName >> nItemType;
			_SetRegistEquip( nInvenIndex, nItemType, nSocketCount, wsItemName );
		}
		break;
	case SystemType::eRegistElem:
		{
			int nSocketIndex = 0;
			u8 nNeedAttachMoney = 0;
			bool bEnable = false;
			kStream >> nSocketIndex >> nNeedAttachMoney >> bEnable;
			_SetRegistSocket( nSocketIndex, nNeedAttachMoney, bEnable );
		}
		break;
	case SystemType::eSelectSocket:
		{
			int nSocketIndex = 0;
			u8 nNeedMoney = 0;
			bool bEnable = false;
			kStream >> nSocketIndex >> nNeedMoney >> bEnable;
			_SelectSocket( nSocketIndex, nNeedMoney, bEnable );
		}
		break;
	case SystemType::eReleaseSocket:
		{
			int nSocketIndex = 0;
			u8 nNeedMoney = 0;
			bool bEnable = false;
			bool bReleaseAll = false;
			kStream >> nSocketIndex >> nNeedMoney >> bEnable >> bReleaseAll;
			_ReleaseSocket( nSocketIndex, nNeedMoney, bEnable, bReleaseAll );
		}
		break;
	case SystemType::eSuccessAttach:
		_SuccessAttach();
		break;
	case SystemType::eSuccessDetach:
		_SuccessDetach();
		break;

	case SystemType::eRegistCheckItem:
		{
			uint nInvenIndex = 0;
			uint nItemType = 0;
			u8 nMoney = 0;
			bool bEnable = false;
			std::wstring wsItemName;
			kStream >> nItemType >> nMoney >> bEnable >> wsItemName >> nInvenIndex;
			_SetRegistElement( nInvenIndex, nItemType, wsItemName, nMoney, bEnable );
		}
		break;
	case SystemType::eSuccessAnalysis:
		{
			u1 nRate = 0;
			kStream >> nRate;
			_SuccessAnalysis( nRate );
		}
		break;

	case SystemType::eResetWindow:
		{
			m_sRegistItem.Reset();
			m_sEquipCon.ResetUI();
			m_sCheckCon.ResetUI();
		}
		break;
	}
}

void cElementManagement::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	DeleteResource();
}

void cElementManagement::DeleteResource()
{
	DeleteScript();
}

void cElementManagement::Create(cWindow* pkRoot, int nValue /*= 0 */)
{
	CsPoint ptPos( 20, 140 );
	CsPoint ptSize( 600, 500 );

	InitScript( pkRoot, NULL, ptPos, ptSize, false );

	m_pHelpButton = AddButton( CsPoint( 570, -15 ), CsPoint( 24, 32 ), CsPoint( 0, 32 ), "DigimonStatus_New\\Digimon_help_btn.tga" );
	if( m_pHelpButton )
		m_pHelpButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cElementManagement::_OnClickHelpButton );

	{	// 등록창 설명
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_EXPLAIN_REGIST" ).c_str() );
		AddText( &ti, CsPoint( ptSize.x * 0.5f, ptSize.y * 0.4f ) );
	}

	_MakeEmptyRegistUI();
	_MakeEmptyEquipUI();
	_MakeEmptyCheckUI();
}

void cElementManagement::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );
}

BOOL cElementManagement::Update_ForMouse()
{
	if( m_pListBox && m_pListBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		_SetRegistItemTooltip( m_pListBox->GetMouseOverItem() );
		return TRUE;
	}

	if( m_sEquipCon.Update_ForMouse() )
		return TRUE;

	if( m_sCheckCon.Update_ForMouse() )
		return TRUE;

	if( m_pHelpButton && m_pHelpButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	return FALSE;
}

void cElementManagement::RenderScript()
{
	cScriptUI::RenderScript();
}

void cElementManagement::ResetDevice()
{
	ResetDeviceScript();
}

void cElementManagement::OnLButtonUp(CsPoint pos)
{

}

bool cElementManagement::CursorIconLBtnUp(CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount)
{
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			GetSystem()->RegisterItem_ElementManage( nIconSlot );
			CURSOR_ST.ReleaseIcon();
			return true;
		}
		break;
	default:
		cPrintMsg::PrintMsg( 11018 );
	}
	return false;
}

void cElementManagement::UpdateViewer()
{
	m_sRegistItem.Reset();
	m_sEquipCon.ResetUI();
	m_sCheckCon.ResetUI();
}

void cElementManagement::_MakeEmptyRegistUI()
{
	m_pListBox = NiNew cListBox;
	SAFE_POINTER_RET( m_pListBox );

	m_pListBox->Init( m_pRoot, CsPoint( 200, 90 ), CsPoint( 200, 100 ), false );
	m_pListBox->AddEvent( cListBox::LIST_ITEM_RIGHT_UP, this, &cElementManagement::_OnRClickRegistItem );
	//m_pListBox->SetBackGorundImg( "" );
	AddChildControl( m_pListBox );

	m_pListBox->RemoveAllItem();

	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	{	// 등록 아이템 이름
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor( 84.f/255.f, 189.f/225.f, 1 ) );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( L"" );
		m_sRegistItem.pItemName = pControl->AddText( &ti, CsPoint( 100, -30 ) );
		if( m_sRegistItem.pItemName )
			m_sRegistItem.pItemName->SetAutoPointerDelete( true );
	}

	// 등록 아이템 슬롯
	cSprite* pSlot = NiNew cSprite;
	if( pSlot )
	{
		pSlot->Init( NULL, CsPoint::ZERO, CsPoint( 188, 88 ), "EquipSystem\\Recharge_slot.tga", false );
		cString::sSPRITE* pSlotImg = pControl->AddSprite( pSlot, CsPoint( 12, 6 ) );
		if( pSlotImg )
			pSlotImg->SetAutoPointerDelete( true );
	}

	// 등록 아이템 아이콘
	m_sRegistItem.pItemIcon = pControl->AddIcon( CsPoint( 42, 42 ), ICONITEM::ITEM_ICON, 0, 1, CsPoint( 85, 29 ) );
	if( m_sRegistItem.pItemIcon )
		m_sRegistItem.pItemIcon->SetAutoPointerDelete( true );

	m_sRegistItem.pItemControl = NiNew cListBoxItem;
	if( m_sRegistItem.pItemControl )
	{
		m_sRegistItem.pItemControl->SetItem( pControl );
		m_sRegistItem.pItemControl->SetUserData( new sRegistInfo( eRegistType::eRegistNone, -1 ) );
		m_pListBox->AddItem( m_sRegistItem.pItemControl );
	}
}

void cElementManagement::_MakeEmptyEquipUI()
{
	m_sEquipCon.pElemEquip = NiNew cScriptUI;
	SAFE_POINTER_RET( m_sEquipCon.pElemEquip );

	m_sEquipCon.pElemEquip->InitScript( m_pRoot, NULL, CsPoint( 150, 240 ), CsPoint( 310, 250 ), false );

	// 합성 비용 배경
	m_sEquipCon.pElemEquip->AddSprite( CsPoint( 42, 90 ), CsPoint( 200, 35 ), "TacticsHouse\\Storage_M_field.tga", true );

	{	// 합성 비용
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_ATTACH_MONEY" ).c_str() );
		m_sEquipCon.pElemEquip->AddText( &ti, CsPoint( -30, 103 ) );
	}

	// 돈 표시
	m_sEquipCon.pAttachCost = NiNew cMoneyIF;
	if( m_sEquipCon.pAttachCost )
	{
		m_sEquipCon.pAttachCost->Init( cMoneyIF::DISP_SIMPLE_1, (u8)0, false );
		m_sEquipCon.pAttachCost->SetVisible( false );
		m_sEquipCon.pAttachCost->SetPos( CsPoint( 107, 100 ) );
		m_sEquipCon.AddChildControl( m_sEquipCon.pAttachCost );
	}

	{	// 합성 버튼
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_BTN_ATTACH" ).c_str() );
		m_sEquipCon.pAttachBtn = m_sEquipCon.pElemEquip->AddButton( CsPoint( 250, 94 ), CsPoint( 128, 32 ), cButton::IMAGE_NOR_13, &ti );
		if( m_sEquipCon.pAttachBtn )
		{
			m_sEquipCon.pAttachBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cElementManagement::_OnClickAttachButton );
			m_sEquipCon.pAttachBtn->SetEnable( false );
		}
	}


	// 해제 비용 배경
	m_sEquipCon.pElemEquip->AddSprite( CsPoint( 42, 130 ), CsPoint( 200, 35 ), "TacticsHouse\\Storage_M_field.tga", true );

	{	// 해제 비용
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_DETACH_MONEY" ).c_str() );
		m_sEquipCon.pElemEquip->AddText( &ti, CsPoint( -30, 143 ) );
	}

	// 돈 표시
	m_sEquipCon.pDetachCost = NiNew cMoneyIF;
	if( m_sEquipCon.pDetachCost )
	{
		m_sEquipCon.pDetachCost->Init( cMoneyIF::DISP_SIMPLE_1, (u8)0, false );
		m_sEquipCon.pDetachCost->SetVisible( false );
		m_sEquipCon.pDetachCost->SetPos( CsPoint( 107, 140 ) );
		m_sEquipCon.AddChildControl( m_sEquipCon.pDetachCost );
	}

	{	// 해제 버튼
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_BTN_DETACH" ).c_str() );
		m_sEquipCon.pDetachBtn = m_sEquipCon.pElemEquip->AddButton( CsPoint( 250, 134 ),
			CsPoint( 128, 32 ), cButton::IMAGE_NOR_13, &ti );
		if( m_sEquipCon.pDetachBtn )
		{
			m_sEquipCon.pDetachBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cElementManagement::_OnClickDetachButton );
			m_sEquipCon.pDetachBtn->SetEnable( false );
		}
	}

	// 아이템 등록 전 비활성화
	m_sEquipCon.SetEnable( false );

	AddChildScriptUI( m_sEquipCon.pElemEquip );
}

void cElementManagement::_MakeEmptyCheckUI()
{
	m_sCheckCon.pElemCheck = NiNew cScriptUI;
	SAFE_POINTER_RET( m_sCheckCon.pElemCheck );

	m_sCheckCon.pElemCheck->InitScript( m_pRoot, NULL, CsPoint( 50, 220 ), CsPoint( 500, 300 ), false );

	// 확인 비용 배경
	m_sCheckCon.pElemCheck->AddSprite( CsPoint( 150, 97 ), CsPoint( 200, 35 ), "TacticsHouse\\Storage_M_field.tga", true );

	{	// 확인 비용
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_RIGHT;
		ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_ANALYSIS_MONEY" ).c_str() );
		m_sCheckCon.pElemCheck->AddText( &ti, CsPoint( 150, 111 ) );
	}

	// 돈 표시
	m_sCheckCon.pAnalysisCost = NiNew cMoneyIF;
	if( m_sCheckCon.pAnalysisCost )
	{
		m_sCheckCon.pAnalysisCost->Init( cMoneyIF::DISP_SIMPLE_1, (u8)0, false );
		m_sCheckCon.pAnalysisCost->SetVisible( false );
		m_sCheckCon.pAnalysisCost->SetPos( CsPoint( 200, 108 ) );
		m_sCheckCon.AddChildControl( m_sCheckCon.pAnalysisCost );
	}

	{	// 분석 버튼
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_BTN_ANALYSIS" ).c_str() );
		m_sCheckCon.pAnalysisBtn = m_sCheckCon.pElemCheck->AddButton( CsPoint( 350, 102 ), CsPoint( 128, 32 ), cButton::IMAGE_NOR_13, &ti );
		if( m_sCheckCon.pAnalysisBtn )
		{
			m_sCheckCon.pAnalysisBtn->SetUserData( new sButtonInfo() );
			m_sCheckCon.pAnalysisBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cElementManagement::_OnClickAnalysisButton );
		}
	}

	{	// 프로그레스 바 배경
		m_sCheckCon.pElemCheck->AddSprite( CsPoint( 164, 25 ), CsPoint( 192, 47 ), "EquipSystem\\Recharge_gauge.tga" );

		// 프로그레스 바
		m_sCheckCon.pProgressBar = NiNew cUI_TimerProgressBar;
		if( m_sCheckCon.pProgressBar )
		{
			m_sCheckCon.pProgressBar->Init( m_sCheckCon.pElemCheck->GetRoot(), CsPoint( 180, 41 ), CsPoint( 160, 15 ), false );
			m_sCheckCon.pProgressBar->SetViewTextType( cUI_ProgressBar::NONE );
			m_sCheckCon.pProgressBar->SetGuageImage( "DigitamaTrade\\Digitama_ScanBar.tga" );
			m_sCheckCon.pProgressBar->SetStepSize( 0.01f );
			m_sCheckCon.pProgressBar->SetTimer( 0.0f, IF_ELEMENT_PROCESS_TIME );
			m_sCheckCon.pProgressBar->Stop();
			m_sCheckCon.pProgressBar->AddEvent( cUI_ProgressBar::eProgressbar_Done, this, &cElementManagement::_ProcessSuccess );
			m_sCheckCon.AddChildControl( m_sCheckCon.pProgressBar );
		}
	}

	// 아이템 등록 전 비활성화
	m_sCheckCon.SetEnable( false );

	AddChildScriptUI( m_sCheckCon.pElemCheck );
}

void cElementManagement::_MakeEmptySocketGrid(int const& nSlotCount)
{
	const CsPoint ptItemSize( 32, 32 );
	const CsPoint ptItemGab( 48, 0 );
	CsPoint ptSize = ptItemSize;
	ptSize.x = ( ptItemSize.x + ptItemGab.x ) * nSlotCount - ptItemGab.x;
	CsPoint ptPos( 0, 15 );
	ptPos.x = ( m_sEquipCon.GetRootSize().x - ptSize.x ) >> 1;

	if( NULL == m_sEquipCon.pGridBox )
	{
		m_sEquipCon.pGridBox = NiNew cGridListBox;
		if( m_sEquipCon.pGridBox )
		{
			m_sEquipCon.pGridBox->Init( NULL, ptPos, ptSize, ptItemGab, ptItemSize, 
				cGridListBox::LowRightDown, cGridListBox::LeftTop, false, nSlotCount );
			m_sEquipCon.pGridBox->SetMouseOverImg( "Icon\\Mask_Over.tga" );
			m_sEquipCon.pGridBox->SetBackOverAndSelectedImgRender( false );
			m_sEquipCon.pGridBox->SetAutoSelection( false );
			m_sEquipCon.pGridBox->AddEvent( cGridListBox::GRID_CLICKED_ITEM, this, &cElementManagement::_OnClickElementSocket );
			m_sEquipCon.pGridBox->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &cElementManagement::_OnRClickElementSocket );
			m_sEquipCon.AddChildControl( m_sEquipCon.pGridBox );
		}
	}
	else
	{
		m_sEquipCon.pGridBox->SetPos( ptPos );
		m_sEquipCon.pGridBox->SetSize( ptSize );
		m_sEquipCon.pGridBox->ResetItemExtendSize();
		m_sEquipCon.pGridBox->AutoSetExtendSize();
 		m_sEquipCon.pGridBox->ResetDevice();
	}

	SAFE_POINTER_RET( m_sEquipCon.pGridBox );
	m_sEquipCon.pGridBox->RemoveAllItem();

	m_sEquipCon.mapSocket.clear();
	for( int i = 0; i < nSlotCount; ++i )
	{
		SystemType::sSocketInfo* pSocketInfo = GetSystem()->GetSocketInfo( i );
		SAFE_POINTER_CON( pSocketInfo );
		bool bAttach = ( SystemType::eSocketState::eAttached == pSocketInfo->eState ) ? true : false;
		_AddElementSocket( i, pSocketInfo->nItemType, pSocketInfo->nItemRate, bAttach );
	}
}

void cElementManagement::_AddElementSocket(int const& nSlotIndex, int const& nItemType, u1 const& nRate, bool const& bAttach)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	const CsPoint ptItemSize( 32, 32 );

	// 소켓 슬롯 배경
	cSprite* pSlot = NiNew cSprite;
	if( pSlot )
	{
		std::string strPath = GetSystem()->GetSlotImagePath();
		pSlot->Init( NULL, CsPoint::ZERO, CsPoint( 40, 40 ), strPath.c_str(), false );
		cString::sSPRITE* pSprite = pControl->AddSprite( pSlot, CsPoint( -4, -4 ) );
		if( pSprite )
		{
			pSprite->SetAutoPointerDelete( true );
			if( strPath.empty() )
				pSprite->SetVisible( false );
		}
	}

	sSocketItem sSocket;
	// 소켓 아이템 아이콘
	sSocket.pItemIcon = pControl->AddIcon( ptItemSize, ICONITEM::ITEM_ICON, nItemType, 1, CsPoint::ZERO );
	if( sSocket.pItemIcon )
	{
		sSocket.pItemIcon->s_nIndex_3 = nRate;
		sSocket.pItemIcon->SetAutoPointerDelete( true );
	}

	// 장착된 소켓 표시
	cSprite* pAttach = NiNew cSprite;
	if( pAttach )
	{
		pAttach->Init( NULL, CsPoint::ZERO, ptItemSize, "Icon\\Mask_Socket.tga", false );
		sSocket.pAttachedImg = pControl->AddSprite( pAttach );
		if( sSocket.pAttachedImg )
		{
			sSocket.pAttachedImg->SetAutoPointerDelete( true );
			sSocket.pAttachedImg->SetVisible( bAttach );
		}
	}

	// 선택된 소켓 표시
	cSprite* pSelect = NiNew cSprite;
	if( pSelect )
	{
		pSelect->Init( NULL, CsPoint::ZERO, ptItemSize, "Scan_New\\Scan_glow_Y02.tga", false );
		sSocket.pSelectedImg = pControl->AddSprite( pSelect );
		if( sSocket.pSelectedImg )
		{
			sSocket.pSelectedImg->SetAutoPointerDelete( true );
			sSocket.pSelectedImg->SetVisible( false );
		}
	}

	sSocket.pItemControl = NiNew cGridListBoxItem( nSlotIndex, ptItemSize );
	if( sSocket.pItemControl )
	{
		sSocket.pItemControl->SetItem( pControl );
		sSocket.pItemControl->SetUserData( new sElemInfo( nItemType, nRate ) );
		m_sEquipCon.pGridBox->AddItem( sSocket.pItemControl );
		m_sEquipCon.mapSocket.insert( std::make_pair( nSlotIndex, sSocket ) );
	}
}

void cElementManagement::_SetRegistEquip(uint const& nInvenIndex, uint const& nItemType, int const& nSocketCount, std::wstring const& wsItemName)
{
	// 분석기 disable, 합성기 enable
	m_sCheckCon.SetEnable( false );
	m_sEquipCon.SetEnable( true );

	cWindow::PlaySound( cWindow::SD_At1 );

	// 장비 아이템 아이콘
	m_sRegistItem.ChangeRegistItem( eRegistType::eRegistEquip, nItemType, nInvenIndex, wsItemName );

	// 소켓 개수만큼 그리드 아이템 생성
	_MakeEmptySocketGrid( nSocketCount );

	SAFE_POINTER_RET( m_sEquipCon.pGridBox );
	m_sEquipCon.pGridBox->RemoveAllItem();

	// 소켓 정보로 갱신
	m_sEquipCon.mapSocket.clear();
	for( int i = 0; i < nSocketCount; ++i )
	{
		SystemType::sSocketInfo* pSocketInfo = GetSystem()->GetSocketInfo( i );
		SAFE_POINTER_CON( pSocketInfo );
		bool bAttach = ( SystemType::eSocketState::eAttached == pSocketInfo->eState ) ? true : false;
		_AddElementSocket( i, pSocketInfo->nItemType, pSocketInfo->nItemRate, bAttach );
	}

	// 비용 ui disable
	m_sEquipCon.SetAttachUI( false, false );
	m_sEquipCon.SetDetachUI( false, false );
}

void cElementManagement::_SetRegistSocket(int const& nSocketIndex, u8 const& nNeedMoney, bool const& bEnable)
{
	std::map< int, sSocketItem >::iterator it = m_sEquipCon.mapSocket.find( nSocketIndex );
	if( it == m_sEquipCon.mapSocket.end() )
		return;

	cWindow::PlaySound( cWindow::SD_At1 );

	// 소켓 정보 갱신
	SystemType::sSocketInfo* pSocketInfo = GetSystem()->GetSocketInfo( nSocketIndex );
	if( pSocketInfo )
		it->second.ChangeSocketItem( pSocketInfo->nItemType, pSocketInfo->nItemRate );

	// 합성 비용 갱신
	m_sEquipCon.SetAttachUI( true, bEnable, nNeedMoney );
}

void cElementManagement::_SetRegistElement(uint const& nInvenIndex, uint const& nItemType, std::wstring const& wsItemName,
										   u8 const& nMoney, bool const& bEnable)
{
	// 분석기 enable, 합성기 disable
	m_sCheckCon.SetEnable( true );
	m_sEquipCon.SetEnable( false );

	cWindow::PlaySound( "System\\interface_person_01.wav" );
	// 속성 아이템 아이콘
	m_sRegistItem.ChangeRegistItem( eRegistType::eRegistElem, nItemType, nInvenIndex, wsItemName );

	// 비용 ui enable
	m_sCheckCon.SetAnalysisUI( true, bEnable, nMoney );
	m_sCheckCon.SetAnalysisButton( eButtonType::eBtnAnalysis );
}

void cElementManagement::_SetRegistItemTooltip(cListBoxItem const* pOverItem)
{
	SAFE_POINTER_RET( pOverItem );

	sRegistInfo* pUserData = dynamic_cast< sRegistInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RET( pUserData );

	cString::sICON* pItemIcon = m_sRegistItem.pItemIcon;
	SAFE_POINTER_RET( pItemIcon );

	if( !pItemIcon->m_MouseOverCheckRect.PtInRect( CURSOR_ST.GetPos() ) )
		return;

	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );

	switch( pUserData->eType )
	{
	case eRegistType::eRegistElem:
		{
			cItemInfo* pElem = GetSystem()->GetRegistItem_Check();
			SAFE_POINTER_RET( pElem );
			pTooltip->SetTooltip( pItemIcon->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pElem->GetType(),
				cBaseWindow::WT_EQUIPMENT_SYSTEM, cTooltip::OPEN_SLOT, pElem->m_nRate, pElem );
		}
		break;
	case eRegistType::eRegistEquip:
		{
			cItemInfo* pEquip = GetSystem()->GetRegistItem_Equip();
			SAFE_POINTER_RET( pEquip );
			pTooltip->SetTooltip( pItemIcon->GetWorldPos(), CsPoint( 42, 42 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pEquip->GetType(),
				cBaseWindow::WT_EQUIPMENT_SYSTEM, cTooltip::OPEN_SLOT, 0, pEquip );
		}
		break;
	}
}

void cElementManagement::_SuccessAnalysis(u1 const& nRate)
{
	// 분석 완료 -> rate 값 변경
	m_sRegistItem.ChangeRate( nRate );

	// 프로그레스바 리셋
	m_sCheckCon.SetProgressBar( false );

	// 취소 -> 완료
	m_sCheckCon.SetAnalysisButton( eButtonType::eBtnSuccess );
}

void cElementManagement::_SuccessAttach()
{
	std::map< int, sSocketItem >::iterator it = m_sEquipCon.mapSocket.begin();
	std::map< int, sSocketItem >::iterator itEnd = m_sEquipCon.mapSocket.end();
	for( ; it != itEnd; ++it )
	{
		SystemType::sSocketInfo* pSocketInfo = GetSystem()->GetSocketInfo( it->first );
		SAFE_POINTER_CON( pSocketInfo );
		if( SystemType::eSocketState::eAttached == pSocketInfo->eState )
			it->second.SetAttached( true );
	}

	m_sEquipCon.SetAttachUI( false, false );
}

void cElementManagement::_SuccessDetach()
{
	std::map< int, sSocketItem >::iterator it = m_sEquipCon.mapSocket.begin();
	std::map< int, sSocketItem >::iterator itEnd = m_sEquipCon.mapSocket.end();
	for( ; it != itEnd; ++it )
	{
		SystemType::sSocketInfo* pSocketInfo = GetSystem()->GetSocketInfo( it->first );
		SAFE_POINTER_CON( pSocketInfo );
		if( SystemType::eSocketState::eNoneRegist == pSocketInfo->eState )
			it->second.Reset();
	}

	m_sEquipCon.SetDetachUI( false, false );
}

void cElementManagement::_SelectSocket(int const& nSocketIndex, u8 const& nNeedMoney, bool const& bEnable)
{
	std::map< int, sSocketItem >::iterator it = m_sEquipCon.mapSocket.begin();
	std::map< int, sSocketItem >::iterator itEnd = m_sEquipCon.mapSocket.end();
	for( ; it != itEnd; ++it )
	{
		if( nSocketIndex != it->first )
			it->second.SetSelected( false );
		else
			it->second.SetSelected( true );
	}

	m_sEquipCon.SetDetachUI( true, bEnable, nNeedMoney );
}

void cElementManagement::_ReleaseSocket(int const& nSocketIndex, u8 const& nNeedMoney, bool const& bEnable, bool const& bReleaseAll)
{
	std::map< int, sSocketItem >::iterator it = m_sEquipCon.mapSocket.find( nSocketIndex );
	if( it != m_sEquipCon.mapSocket.end() )
		it->second.Reset();

	bool bAttach = bReleaseAll ? false : bEnable;
	m_sEquipCon.SetAttachUI( !bReleaseAll, bAttach, nNeedMoney );
}

void cElementManagement::_OnClickAnalysisButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	cButton* pAnalysis = static_cast< cButton* >( pSender );
	sButtonInfo* pBtnInfo = static_cast< sButtonInfo* >( pAnalysis->GetUserData() );
	SAFE_POINTER_RET( pBtnInfo );

	switch( pBtnInfo->eBtnState )
	{
	case eButtonType::eBtnAnalysis:
		{
			if( !GetSystem()->StartAnalysis() )
				return;

			m_sCheckCon.SetProgressBar( true );

			m_sCheckCon.SetAnalysisButton( eButtonType::eBtnCancel );

			cWindow::PlaySound( "System\\System_return_01.wav" );
		}
		break;
	case eButtonType::eBtnSuccess:
		{
			GetSystem()->ResetRegistItem_Check();

			m_sCheckCon.SetAnalysisUI( false, false );
			m_sCheckCon.SetAnalysisButton( eButtonType::eBtnAnalysis );

			cWindow::PlaySound( cWindow::SD_Np1 );
		}
		break;
	case eButtonType::eBtnCancel:
		{
			if( !GetSystem()->CancelAnalysis() )
				return;

			m_sCheckCon.SetProgressBar( false );
			m_sCheckCon.SetAnalysisButton( eButtonType::eBtnAnalysis );
		}
		break;
	}
}

void cElementManagement::_OnClickAttachButton(void* pSender, void* pData)
{
	GetSystem()->AttachElementItem();
}

void cElementManagement::_OnClickDetachButton(void* pSender, void* pData)
{
	GetSystem()->DetachElementItem();
}

void cElementManagement::_OnClickElementSocket(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pClicked = static_cast< cGridListBoxItem* >( pData );
	GetSystem()->SelectAttachedSocket( pClicked->getID() );
}

void cElementManagement::_OnRClickElementSocket(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pClicked = static_cast< cGridListBoxItem* >( pData );
	GetSystem()->ReleaseRegistSocket( pClicked->getID() );
}

void cElementManagement::_OnRClickRegistItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cListBoxItem* pClicked = static_cast< cListBoxItem* >( pData );
	sRegistInfo* pInfo = dynamic_cast< sRegistInfo* >( pClicked->GetUserData() );
	SAFE_POINTER_RET( pInfo );
	switch( pInfo->eType )
	{
	case eRegistType::eRegistEquip:
		GetSystem()->ResetRegistItem_Equip();
		break;
	case eRegistType::eRegistElem:
		GetSystem()->ResetRegistItem_Check();
		break;
	}
}

void cElementManagement::_OnClickHelpButton(void* pSender, void* pData)
{
	GetSystem()->HelpWindow();
}

void cElementManagement::_ProcessSuccess(void* pSender, void* pData)
{
	GetSystem()->EndAnalysis();
}

///////////////////////////////////////////////////////////////////////////////////////////////
bool cElementManagement::sEquipControl::Update_ForMouse()
{
	if( !GetEnable() )
		return false;

	SetExplainBtnTooltip();

	if( pGridBox && pGridBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		SetSocketTooltip( pGridBox->GetMouseOverItem() );
		return true;
	}

	if( pAttachBtn && pAttachBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return true;

	if( pDetachBtn && pDetachBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return true;

	return false;
}

CsPoint cElementManagement::sEquipControl::GetRootSize() const
{
	SAFE_POINTER_RETVAL( pElemEquip, CsPoint::ZERO );
	return pElemEquip->GetRootSize();
}

bool cElementManagement::sEquipControl::GetEnable() const
{
	SAFE_POINTER_RETVAL( pElemEquip, false );
	if( !pElemEquip->IsVisible() )
		return false;
	return pElemEquip->IsEnableWindow();
}

void cElementManagement::sEquipControl::SetEnable(bool bEnable)
{
	SAFE_POINTER_RET( pElemEquip );
	pElemEquip->SetEnableWindow( bEnable );
	pElemEquip->SetVisible( bEnable );
}

void cElementManagement::sEquipControl::SetAttachUI(bool bVisible, bool bEnable, u8 nMoney)
{
	if( pAttachCost )
	{
		pAttachCost->SetVisible( bVisible );

		if( !bEnable )
			pAttachCost->SetColor_BeforeSetMoney( NiColor( 1, 0, 0 ) );
		else
			pAttachCost->SetColor_BeforeSetMoney( NiColor::WHITE );

		pAttachCost->SetMoney( nMoney );
	}

	if( pAttachBtn )
		pAttachBtn->SetEnable( bEnable );
}

void cElementManagement::sEquipControl::SetDetachUI(bool bVisible, bool bEnable, u8 nMoney)
{
	if( pDetachCost )
	{
		pDetachCost->SetVisible( bVisible );

		if( !bEnable )
			pDetachCost->SetColor_BeforeSetMoney( NiColor( 1, 0, 0 ) );
		else
			pDetachCost->SetColor_BeforeSetMoney( NiColor::WHITE );

		pDetachCost->SetMoney( nMoney );
	}

	if( pDetachBtn )
		pDetachBtn->SetEnable( bEnable );
}

void cElementManagement::sEquipControl::SetSocketTooltip(cGridListBoxItem const* pOverItem)
{
	SAFE_POINTER_RET( pOverItem );
	sElemInfo* pUserData = dynamic_cast< sElemInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RET( pUserData );

	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );

	if( 0 == pUserData->nItemType )
	{
#ifdef SIMPLE_TOOLTIP
		pTooltip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), 100, cTooltip::UIHELP, 1035 );
#endif
		return;
	}

	cItemInfo socketItem;
	socketItem.Set( pUserData->nItemType, 1, pUserData->nItemRate, 0 );
	pTooltip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM,
		socketItem.m_nType, cBaseWindow::WT_EQUIPMENT_SYSTEM, cTooltip::OPEN_SLOT, pOverItem->getID(), &socketItem );
}

void cElementManagement::sEquipControl::SetExplainBtnTooltip()
{
#ifdef SIMPLE_TOOLTIP
	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );

	// Attach button
	CsRect rcCheckBox( pAttachBtn->GetPos(), CsSIZE( pAttachBtn->GetSize() ) );
	if( rcCheckBox.PtInRect( CURSOR_ST.GetPos() ) )
		pTooltip->SetTooltip( pAttachBtn->GetPos(), pAttachBtn->GetSize(), 100, cTooltip::UIHELP, 1037 );

	// Detach button
	rcCheckBox = CsRect( pDetachBtn->GetPos(), CsSIZE( pDetachBtn->GetSize() ) );
	if( rcCheckBox.PtInRect( CURSOR_ST.GetPos() ) )
		pTooltip->SetTooltip( pDetachBtn->GetPos(), pDetachBtn->GetSize(), 100, cTooltip::UIHELP, 1039 );

	// Detach cost text
	rcCheckBox = CsRect( pDetachCost->GetClient(), CsSIZE( pDetachCost->GetSize() ) );
	if( rcCheckBox.PtInRect( CURSOR_ST.GetPos() ) )
		pTooltip->SetTooltip( pDetachCost->GetClient(), pDetachCost->GetSize(), 100, cTooltip::UIHELP, 1038 );
#endif
}

void cElementManagement::sEquipControl::ResetUI()
{
	mapSocket.clear();

	if( pGridBox )
		pGridBox->RemoveAllItem();

	SetAttachUI( false, false );
	SetDetachUI( false, false );
	SetEnable( false );
}

void cElementManagement::sEquipControl::AddChildControl(cWindow* pControl)
{
	SAFE_POINTER_RET( pElemEquip );
	pElemEquip->AddChildControl( pControl );
}

///////////////////////////////////////////////////////////////////////////////////////////////
bool cElementManagement::sCheckControl::Update_ForMouse()
{
	if( !GetEnable() )
		return false;

	if( pAnalysisBtn && pAnalysisBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return true;

	return false;
}

bool cElementManagement::sCheckControl::GetEnable() const
{
	SAFE_POINTER_RETVAL( pElemCheck, false );
	if( !pElemCheck->IsVisible() )
		return false;
	return pElemCheck->IsEnableWindow();
}

void cElementManagement::sCheckControl::SetEnable(bool bEnable)
{
	SAFE_POINTER_RET( pElemCheck );
	pElemCheck->SetEnableWindow( bEnable );
	pElemCheck->SetVisible( bEnable );
}

void cElementManagement::sCheckControl::SetAnalysisUI(bool bVisible, bool bEnable, u8 nMoney /*= 0 */)
{
	if( pAnalysisCost )
	{
		pAnalysisCost->SetVisible( bVisible );

		if( !bEnable )
			pAnalysisCost->SetColor_BeforeSetMoney( NiColor( 1, 0, 0 ) );
		else
			pAnalysisCost->SetColor_BeforeSetMoney( NiColor::WHITE );

		pAnalysisCost->SetEnable( bEnable );
		pAnalysisCost->SetMoney( nMoney );
	}

	if( pAnalysisBtn )
		pAnalysisBtn->SetEnable( bEnable );
}

void cElementManagement::sCheckControl::SetProgressBar(bool bPlay)
{
	SAFE_POINTER_RET( pProgressBar );

	pProgressBar->SetTimer( 0.0f, IF_ELEMENT_PROCESS_TIME );

	if( bPlay )
		pProgressBar->Start();
	else
		pProgressBar->Stop();
}

void cElementManagement::sCheckControl::SetAnalysisButton(eButtonType eType)
{
	SAFE_POINTER_RET( pAnalysisBtn );

	switch( eType )
	{
	case eButtonType::eBtnAnalysis:
		pAnalysisBtn->SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_BTN_ANALYSIS" ).c_str() );
		break;
	case eButtonType::eBtnCancel:
		pAnalysisBtn->SetText( UISTRING_TEXT( "EQUIPSYSTEM_NORMAL_CHARGE_BTN_CANCEL" ).c_str() );
		break;
	case eButtonType::eBtnSuccess:
		pAnalysisBtn->SetText( UISTRING_TEXT( "EQUIPSYSTEM_ELEM_MANAGE_BTN_COMPLETE" ).c_str() );
		break;
	}

	sButtonInfo* pBtnInfo = static_cast< sButtonInfo* >( pAnalysisBtn->GetUserData() );
	if( pBtnInfo )
		pBtnInfo->eBtnState = eType;
}

void cElementManagement::sCheckControl::ResetUI()
{
	SetProgressBar( false );
	SetAnalysisUI( false, false );
	SetAnalysisButton( eButtonType::eBtnAnalysis );
	SetEnable( false );
}

void cElementManagement::sCheckControl::AddChildControl(cWindow* pControl)
{
	SAFE_POINTER_RET( pElemCheck );
	pElemCheck->AddChildControl( pControl );
}
/////////////////////////////////////////////////////////////////////////////////////

void cElementManagement::sRegistItem::ChangeRegistItem(eRegistType eType, uint nItemType, int nInvenIndex, std::wstring wsItemName)
{
	if( pItemIcon )
		pItemIcon->ChangeIcon( ICONITEM::ITEM_ICON, nItemType, 1 );

	if( pItemControl )
	{
		sRegistInfo* pInfo = dynamic_cast< sRegistInfo* >( pItemControl->GetUserData() );
		if( pInfo )
		{
			pInfo->nInvenIndex = nInvenIndex;
			pInfo->eType = eType;
		}
	}

	if( pItemName )
		pItemName->SetText( wsItemName.c_str() );
}

void cElementManagement::sRegistItem::ChangeRate(u1 nRate)
{
	if( pItemIcon )
		pItemIcon->s_nIndex_3 = nRate;
}

void cElementManagement::sRegistItem::Reset()
{
	ChangeRegistItem( eRegistType::eRegistNone, 0, -1, L"" );
	ChangeRate( 0 );
}
///////////////////////////////////////////////////////////////////////////////////////
void cElementManagement::sSocketItem::ChangeSocketItem(uint nItemType, u1 nRate)
{
	if( pItemIcon )
		pItemIcon->ChangeIcon( ICONITEM::ITEM_ICON, nItemType, 1, nRate );

	if( pItemControl )
	{
		sElemInfo* pInfo = dynamic_cast< sElemInfo* >( pItemControl->GetUserData() );
		if( pInfo )
			pInfo->SetItem( nItemType, nRate );
	}
}

void cElementManagement::sSocketItem::SetAttached(bool bVisible)
{
	if( pAttachedImg )
		pAttachedImg->SetVisible( bVisible );
}

void cElementManagement::sSocketItem::SetSelected(bool bVisible)
{
	if( pSelectedImg )
		pSelectedImg->SetVisible( bVisible );
}

void cElementManagement::sSocketItem::Reset()
{
	ChangeSocketItem( 0, 0 );
	SetAttached( false );
	SetSelected( false );
}
