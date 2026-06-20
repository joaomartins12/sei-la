#include "stdafx.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"
#include "SealMaster.h"

void cSealMaster::sSealCardControl::SetSealDigimon( std::string const& imgFile )
{
	SAFE_POINTER_RET( sDigimonImg );
	sDigimonImg->ChangeTexture( imgFile.c_str(), false );
}

void cSealMaster::sSealCardControl::SetFavoriteOnOff( bool bValue )
{
	if( sFavoriteOn )
		sFavoriteOn->SetVisible( bValue );
	if( sFavoriteOff )
		sFavoriteOff->SetVisible( !bValue );

	if(spItem)
	{
		sSealGridData* pUserData = dynamic_cast<sSealGridData*>(spItem->GetUserData());
		if( pUserData )
			pUserData->sFavorite = bValue;
	}
}

void cSealMaster::sSealCardControl::SetSealCount( int const& nCount )
{
	SAFE_POINTER_RET(sCount);

	std::wstring wsCount = DmCS::StringFn::getNumberFormatW( nCount );	
	sCount->SetText( wsCount.c_str() );
	
	if(spItem)
	{
		sSealGridData* pUserData = dynamic_cast<sSealGridData*>(spItem->GetUserData());
		if( pUserData )
			pUserData->sSealCount = nCount;
	}

	if( 0 == nCount )
	{
		if( sBack )
			sBack->SetColor( NiColor( 0.2f, 0.2f, 0.2f ) );
		if( sDigimonImg )
			sDigimonImg->SetColor( NiColor( 0.2f, 0.2f, 0.2f ) );
		if( sCount )
			sCount->SetColor( NiColor( 0.5f, 0.5f, 0.5f ) );
	}
	else
	{
		if( sBack )
			sBack->SetColor( NiColor::WHITE );
		if( sDigimonImg )
			sDigimonImg->SetColor( NiColor::WHITE );
		if( sCount )
			sCount->SetColor( NiColor::WHITE );
	}
}

void cSealMaster::sSealCardControl::SetSealGrade( int const& nGrade, std::string const& imgFile )
{
	SAFE_POINTER_RET(spItem);
	sSealGridData* pUserData = dynamic_cast<sSealGridData*>(spItem->GetUserData());
	SAFE_POINTER_RET(pUserData);
	
// 	if( pUserData->sGrade == nGrade )
// 		return;

	pUserData->sGrade = nGrade;

	if(sBack)
		sBack->ChangeTexture( imgFile.c_str(), false );
}

//////////////////////////////////////////////////////////////////////////

cSealMaster::cSealMaster():m_pRenderTex(NULL), m_bViewCard(false), m_nCurStateFilter(0),
m_pGridListBox(NULL), m_pMapgroupTreeBox(NULL), m_pCombo(NULL), m_pSelectedTree(NULL), m_nViewGridCount(0), m_pActiveSealCountText(NULL),
m_bCountFilter(false), m_bFavoriteFilter(false), m_bMapAllFilter(false)
, m_bisTextCreated(false),m_fAniTime(0.0f)
{
	for(int i = 0; i < STATEKINDS; ++i)
		m_pPlusValueText[i] = NULL;

	m_pActiveSealCountText = NiNew cStringList;
}

cSealMaster::~cSealMaster()
{
	m_mapSealCardCon.clear();
	
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE cSealMaster::GetWindowType()
{
	return WT_CARDINVENTORY;
}

void cSealMaster::Destroy()
{
	cBaseWindow::Delete();
	cSeparate::DeleteInstance( GetWindowType(), 0 );
	SAFE_NIDELETE( m_pRenderTex );
	SAFE_NIDELETE( m_pActiveSealCountText );
}

void cSealMaster::DeleteResource()
{
	DeleteScript();
}

void cSealMaster::ResetDevice()
{
	ResetDeviceScript();
}

void cSealMaster::Create( int nValue /*= 0*/ )
{
	cBaseWindow::Init();
	InitScript( "SealMaster\\sealmaster_window.tga", CsPoint( (g_nScreenWidth/2) - 410 , (g_nScreenHeight/2) - 320 ), CsPoint( 800, 600 ), true );
	AddTitle( UISTRING_TEXT("SEALMASTER_SEAL_TITLE").c_str(), CsPoint(0, 13) );

	// 닫기 버튼 생성
	m_pButtonClose = AddButton( CsPoint( 759, 13 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "CommonUI\\CommonUI_close.tga" );
	if( m_pButtonClose )
		m_pButtonClose->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cSealMaster::CloseButtonEvent );

	CsPoint ptBackGrid(240, 79);
	for(int i = 0; i < 10; ++i)
	{
		AddSprite(ptBackGrid, CsPoint(102, 173), "SealMaster\\Sealmaster_bg_00.tga");
		ptBackGrid.x += 102;
		if (i == 4)
			ptBackGrid = CsPoint(240, ptBackGrid.y+168);
	}
	
	m_pMapgroupTreeBox = NiNew cTreeBox;
	if( m_pMapgroupTreeBox )
	{
		m_pMapgroupTreeBox->Init( GetRoot(),  CsPoint(23, 61), CsPoint( 202, 470 ), NULL, false ); 
		m_pMapgroupTreeBox->setChildItemGab(CsPoint(0,0));
		m_pMapgroupTreeBox->AddEvent( cTreeBox::TreeEvent_ItemToggle_Changed, this, &cSealMaster::MapTreeClick );
		m_pMapgroupTreeBox->AddEvent( cTreeBox::TreeEvent_Selection_Changed, this, &cSealMaster::MapTreeButtonClick );
		m_pMapgroupTreeBox->setItemHeight(31);
		m_pMapgroupTreeBox->SetOnlyOpenMode(true);
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if( pScrollBar )
		{
			pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint( 0, 0 ), CsPoint( 16, 470 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint(0,0), CsPoint( 520, 340 ) ), 2, false );
			pScrollBar->SetEnableRenderFromEnableScroll(true);
			m_pMapgroupTreeBox->SetScrollBar( pScrollBar );
		}
		AddChildControl(m_pMapgroupTreeBox);

		//전체보기
		MakeAllWatch();
		//즐겨찾기
		MakeFavorit();
		//활성화 카드
		MakeActive();
		//지역 트리
		MakeMapTree();
	}

	m_pGridListBox = NiNew cGridListBox;
	if(m_pGridListBox)
	{
		cGridListBox::RenderType renderType = cGridListBox::LowRightDown;
		cGridListBox::StartPointType startPoinType = cGridListBox::LeftTop;

		m_pGridListBox->Init( GetRoot(), CsPoint(248,94), CsPoint( 520, 336 ), CsPoint(16,16), CsPoint(86, 152), renderType, startPoinType, NULL, false, 0 );		
		m_pGridListBox->SetBackOverAndSelectedImgRender(false);

		cScrollBar* pScrollBar = NiNew cScrollBar;
		if(pScrollBar)
		{
			pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint( 0, 0 ), CsPoint( 16, 362 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint(0,0), CsPoint( 520, 340 ) ), 2, false );
			pScrollBar->SetEnableRenderFromEnableScroll(true);
			m_pGridListBox->SetScrollBar( pScrollBar );
		}

		m_pGridListBox->AddEvent( cGridListBox::GRID_CLICKED_ITEM, this, &cSealMaster::ViewCardEffect );
		m_pGridListBox->AddEvent( cGridListBox::GRID_BUTTON_UP, this, &cSealMaster::ViewButton );
		m_pGridListBox->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &cSealMaster::RButton );

		AddChildControl(m_pGridListBox);
	}

	//Seal 표현 그리드
	MakeSealGrid();
	
	//능력치 필터링 콤보박스
	MakeStateFilterComboBox();

	//능력치 추가량 표시
	MakePlusState();

	//전체 보기 상태로 설정
	{
		m_bMapAllFilter = true;
		SortforSealMaster();
		SetFilter();
	}
}

void cSealMaster::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();

	if( m_cardEffect.m_pNiNode )
	{
		m_fAniTime += fDeltaTime;
		m_cardEffect.m_pNiNode->Update( m_fAniTime );
	}

	cSeparate::UpdateInstance( GetWindowType(), 0 );
}
cBaseWindow::eMU_TYPE cSealMaster::Update_ForMouse()
{
	if (m_bViewCard)
	{
		if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_UP )
		{
			m_cardEffect.Delete();
			// 같으면 제거
			SAFE_NIDELETE( m_pRenderTex );
			if( g_pNpcMng )
				g_pNpcMng->Reset();
			m_bViewCard = false;
		}
		return cBaseWindow::MUT_NONE;
	}

	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( m_pButtonClose && ( m_pButtonClose->Update_ForMouse() == cButton::ACTION_CLICK ) )
		return muReturn;

	if( cSeparate::Update_ForMouseInstance( GetWindowType(), 0 ) != MUT_OUT_WINDOW )
		return muReturn;

	//포커스가 변하면 안되기 때문에 리턴을 MUT_NONE으로 고정
	if ( g_pSeparate != null && g_pSeparate->GetTargetWindowType() == WT_CARDINVENTORY ||
		g_pGameIF->IsActiveWindow( cBaseWindow::WT_CARDRESEAL ) )
		return cBaseWindow::MUT_NONE;

	if(m_pCombo && m_pCombo->Update_ForMouse() != cComboBox::CI_INVALIDE )
		return muReturn;

	if(m_pMapgroupTreeBox && m_pMapgroupTreeBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return muReturn;

	if(m_pGridListBox && m_pGridListBox->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		Update_ToolTip();
		return muReturn;
	}

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( m_pRoot->GetSize().x, 67 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
	{
		_UpdateMoveWindow_ForMouse();
		return muReturn;
	}
	return muReturn;

}
void cSealMaster::Render()
{
	RenderScript();	
	if(m_pActiveSealCountText != null)
		m_pActiveSealCountText->Render(GetRootClient() + CsPoint( 265, 65 ), 1, DT_CENTER);
	EndRenderScript();
	if (m_bViewCard)
		_CardRender();
	cSeparate::RenderInstance( GetWindowType(), 0 );
}

void cSealMaster::ResetMap()
{
	Close(false);
}
void cSealMaster::PlayOpenSound()
{

}

bool cSealMaster::OnEscapeKey()
{
	Close();
	return true;

}
void cSealMaster::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();	
	ResetDevice();

}

void cSealMaster::MakeMapTree()
{
	const cSealMasterContents::MapGroup& groupMap = GetSystem()->GetMapGroupMap();
	cSealMasterContents::CMGItr cmgItr = groupMap.begin();
	//0/1번은 전체 보기와 즐겨찾기 버튼이다.
	for(int i = 3; cmgItr != groupMap.end(); ++cmgItr, ++i )
	{
		if(cmgItr->second.sCount > 0)
		{
			cTreeBoxItem* pAddItem = NiNew cTreeBoxItem(i);
			SAFE_POINTER_BEK( pAddItem );

			cString * pItem = NiNew cString;
			SAFE_POINTER_BEK( pItem );

			cButton* pButton = NiNew cButton;
			SAFE_POINTER_BEK( pButton );
			pButton->Init( NULL, CsPoint::ZERO, CsPoint( 188, 31), "CommonUI\\CommonUI_tap_btn.tga", false );
			pButton->SetTexToken(CsPoint(0,31));

			pItem->AddButton( pButton, 0, CsPoint( 0, 0), CsPoint( 188, 31) );
			cText::sTEXTINFO tiGroup;
			tiGroup.Init( CFont::FS_11, NiColor::WHITE );
			tiGroup.SetText(cmgItr->second.sMapGroupName.c_str());
			pItem->AddText( &tiGroup, CsPoint(25, 8) );

			pAddItem->SetItem( pItem );
			pAddItem->SetUserData(new sMapGroupData(i, cmgItr->first, -1));

			m_pMapgroupTreeBox->AddItem( pAddItem );
			MakeSubMapTree( *pAddItem, cmgItr->first );
		}
	}
}

void cSealMaster::MakeSubMapTree( cTreeBoxItem& Root, DWORD nGroupType )
{
	const cSealMasterContents::MapGroup& groupMap = const_cast<cSealMasterContents::MapGroup&>( GetSystem()->GetMapGroupMap() );
	cSealMasterContents::CMGItr groupItr = groupMap.find(nGroupType);
	
	cSealMasterContents::CMItr mapItr = groupItr->second.sMapInfoMap.begin();
	cSealMasterContents::CMItr mapend = groupItr->second.sMapInfoMap.end();
	for(int i = 0; mapItr != mapend; ++mapItr, ++i )
	{
		cTreeBoxItem* pAddItem = NiNew cTreeBoxItem(i);
		SAFE_POINTER_BEK( pAddItem );

		cString * pItem = NiNew cString;
		SAFE_POINTER_BEK( pItem );

		cButton* pButton = NiNew cButton;
		SAFE_POINTER_BEK( pButton );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_11, NiColor::WHITE );
		
		pButton->Init( NULL, CsPoint::ZERO, CsPoint( 188, 31), "SealMaster\\common_dropdown_glow.tga", false );
		pButton->SetTexToken(CsPoint(0,22));
		pButton->SetUserData(new sMapGroupData(i, nGroupType, mapItr->first));
		pItem->AddButton( pButton, 0, CsPoint( 0, 0), CsPoint( 188, 31) );

		cText::sTEXTINFO tiGroup;
		tiGroup.Init( CFont::FS_11, NiColor::WHITE );
		tiGroup.SetText(mapItr->second.sMapName.c_str());
		pItem->AddText( &tiGroup, CsPoint(25, 8) );

		pAddItem->SetItem( pItem );

		Root.AddChildItem( pAddItem );
	}
}

//맵 그룹 클릭
void cSealMaster::MapTreeClick(void* pSender, void* pData)
{
	cTreeBox* pTreeBox = static_cast< cTreeBox* >(pSender);
	SAFE_POINTER_RET(pTreeBox);
	cTreeBoxItem* pButton = static_cast<cTreeBoxItem*>(pData);
	SAFE_POINTER_RET(pButton);
	sMapGroupData* pUserData = dynamic_cast<sMapGroupData*>(pButton->GetUserData());
	SAFE_POINTER_RET(pUserData);
	int buttonIndex = pUserData->sButtonIndex;
	int groupType = pUserData->sGroupType;
	m_bMapAllFilter = false;
	m_bFavoriteFilter = false;
	m_bCountFilter = false;
	//전체보기
	if(buttonIndex == 0)
	{
		m_bMapAllFilter = true;
		SortforSealMaster();
		//map 조건 리셋
		m_MapFilterPair = make_pair(0, 0);
		SetFilter();
	}
	//즐겨찾기
	else if (buttonIndex == 1)
	{
		m_bFavoriteFilter = true;
		//map 조건 리셋
		m_MapFilterPair = make_pair(0, 0);
		SetFilter();
	}
	//활성화 카드
	else if (buttonIndex == 2)
	{
		m_bCountFilter = true;
		SortforSealMasterCount();
		//map 조건 리셋
		m_MapFilterPair = make_pair(0, 0);
		SetFilter();
	}
}

//맵 클릭
void cSealMaster::MapTreeButtonClick(void* pSender, void* pData)
{
	cTreeBox* pTreeBox = static_cast< cTreeBox* >(pSender);
	SAFE_POINTER_RET(pTreeBox);
	cTreeBoxItem* pTreeItem = static_cast<cTreeBoxItem*>(pData);
	SAFE_POINTER_RET(pTreeItem);
	cString::sBUTTON* psButton = static_cast<cString::sBUTTON*>(pTreeItem->GetItem()->GetElement(0));
	SAFE_POINTER_RET(psButton);
	cButton* pButton = psButton->s_pButton;
	SAFE_POINTER_RET(pButton);

	sMapGroupData* pUserData = dynamic_cast<sMapGroupData*>(pButton->GetUserData());
	SAFE_POINTER_RET(pUserData);
	if( m_pSelectedTree != null)
		m_pSelectedTree->SetMouseOnMode(false);
	m_pSelectedTree = pButton;
	m_pSelectedTree->SetMouseOnMode(true);

	m_MapFilterPair = make_pair(pUserData->sGroupType, pUserData->sMapID);
	SetFilter();
}


void cSealMaster::MakeAllWatch()
{
	cTreeBoxItem* pAddItem = NiNew cTreeBoxItem(0);
	SAFE_POINTER_RET( pAddItem );

	cString * pItem = NiNew cString;
	SAFE_POINTER_RET( pItem );

	cButton* pButton = NiNew cButton;
	SAFE_POINTER_RET( pButton );
	pButton->Init( NULL, CsPoint::ZERO, CsPoint( 188, 31), "CommonUI\\CommonUI_tap_btn.tga", false );
	pButton->SetTexToken(CsPoint(0,31));
	

	pItem->AddButton( pButton, 0, CsPoint( 0, 0), CsPoint( 188, 31) );
	cText::sTEXTINFO tiGroup;
	tiGroup.Init( CFont::FS_11, NiColor::WHITE );


	std::wstring wsText = UISTRING_TEXT("SEALMASTER_SHOW_ALL");
	tiGroup.SetText( wsText.c_str() );
	pItem->AddText( &tiGroup, CsPoint(25, 8) );

	pAddItem->SetItem( pItem );
	//맵 그룹 0 존재 하지 않음 전체 혹은 즐겨찾기
	pAddItem->SetUserData(new sMapGroupData(0, 0, -1));

	m_pMapgroupTreeBox->AddItem( pAddItem );
}

void cSealMaster::MakeFavorit()
{
	cTreeBoxItem* pAddItem = NiNew cTreeBoxItem(1);
	SAFE_POINTER_RET( pAddItem );

	cString * pItem = NiNew cString;
	SAFE_POINTER_RET( pItem );

	cButton* pButton = NiNew cButton;
	SAFE_POINTER_RET( pButton );
	pButton->Init( NULL, CsPoint::ZERO, CsPoint( 188, 31), "CommonUI\\CommonUI_tap_btn.tga", false );
	pButton->SetTexToken(CsPoint(0,31));

	pItem->AddButton( pButton, 0, CsPoint( 0, 0), CsPoint( 188, 31) );
	cText::sTEXTINFO tiGroup;
	tiGroup.Init( CFont::FS_11, NiColor::WHITE );
	std::wstring wsText = UISTRING_TEXT("SEALMASTER_SHOW_FAVORITE");
	tiGroup.SetText( wsText.c_str() );
	pItem->AddText( &tiGroup, CsPoint(25, 8) );

	pAddItem->SetItem( pItem );
	//맵 그룹 0 존재 하지 않음 전체 혹은 즐겨찾기
	pAddItem->SetUserData(new sMapGroupData(1, 0, -1));

	m_pMapgroupTreeBox->AddItem( pAddItem );
}

void cSealMaster::MakeActive()
{
	cTreeBoxItem* pAddItem = NiNew cTreeBoxItem(2);
	SAFE_POINTER_RET( pAddItem );

	cString * pItem = NiNew cString;
	SAFE_POINTER_RET( pItem );

	cButton* pButton = NiNew cButton;
	SAFE_POINTER_RET( pButton );
	pButton->Init( NULL, CsPoint::ZERO, CsPoint( 188, 31),  "CommonUI\\CommonUI_tap_btn.tga", false );
	pButton->SetTexToken(CsPoint(0,31));

	pItem->AddButton( pButton, 0, CsPoint( 0, 0), CsPoint( 188, 31) );
	cText::sTEXTINFO tiGroup;
	tiGroup.Init( CFont::FS_11, NiColor::WHITE );
	std::wstring wsText = UISTRING_TEXT("SEALMASTER_SHOW_ACTIVE");
	tiGroup.SetText( wsText.c_str() );
	pItem->AddText( &tiGroup, CsPoint(25, 8) );

	pAddItem->SetItem( pItem );
	//맵 그룹 0 존재 하지 않음 전체 혹은 즐겨찾기
	pAddItem->SetUserData(new sMapGroupData(2, 0, -1));

	m_pMapgroupTreeBox->AddItem( pAddItem );
}

void cSealMaster::MakeSealGrid()
{
	m_mapSealCardCon.clear();
	SAFE_POINTER_RET( m_pGridListBox );

	cSealMasterContents::SealInfoMap sealInfoMap = GetSystem()->GetSealInfoMap();
	cSealMasterContents::SealInfoMapItr mapItr = sealInfoMap.begin();
	for(int i = 0; mapItr != sealInfoMap.end(); ++mapItr, ++i)
	{
		cString* pString = NiNew cString;
		SAFE_POINTER_RET(pString);

		const cSealMasterContents::sSealInfo& sealInfo = mapItr->second;
	
		sSealCardControl addCon;		

		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );
			FT_Face nameface = g_pEngine->m_FontSystem.GetFace( CFont::FS_12 );
			int nameTextWidth = cText::GetStringWidth(nameface, sealInfo.sSealName);
			if (nameTextWidth > 98)
			{
				std::wstring CutName = sealInfo.sSealName;
				std::wstring Ellipsis = L"...";
				int nWidth = 0;

				for(int i = 0; i < CutName.length(); ++i)
				{
					nWidth += cText::GetCharWidth(nameface, CutName[i]);
					//"..." 길이는 12
					if (nWidth > 86)
					{
						CutName.replace( i-1, CutName.length()-1, Ellipsis );
						nameTextWidth = nWidth - cText::GetCharWidth(nameface, CutName[i]) + 12;
						break;
					}
				}
				ti.SetText( CutName.c_str() );
			}
			else
				ti.SetText( sealInfo.sSealName.c_str() );
			ti.s_Color = GetSystem()->GetSealItemColor( sealInfo.sSealID );			
			pString->AddText( &ti, CsPoint(43 - nameTextWidth/2, 0) );
		}
		
		addCon.sBack = NiNew cSprite;
		if( addCon.sBack )
		{
			addCon.sBack->Init( NULL, CsPoint(0, 22), CsPoint( 86, 128 ), " " , false );
			cString::sSPRITE* sSprite = pString->AddSprite(addCon.sBack);
			sSprite->SetAutoPointerDelete(true);
		}

		addCon.sDigimonImg = NiNew cSprite;
		if( addCon.sDigimonImg )
		{
			addCon.sDigimonImg->Init( NULL, CsPoint(2, 26), CsPoint( 84, 106 ), " ", false );
			cString::sSPRITE* sSprite = pString->AddSprite(addCon.sDigimonImg);
			sSprite->SetAutoPointerDelete(true);
		}

		//즐찾 있음 상태 버튼
		addCon.sFavoriteOn = NiNew cButton;
		if( addCon.sFavoriteOn )
		{
			addCon.sFavoriteOn->Init( NULL, CsPoint::ZERO, CsPoint( 32, 30 ), "SealMaster\\Sealmaster_favorite_on.dds", false );
			addCon.sFavoriteOn->SetTexToken(CsPoint(0,30));
			addCon.sFavoriteOn->SetVisible(true);
			pString->AddButton(addCon.sFavoriteOn, i, CsPoint(-5,17), CsPoint( 32, 30));
		}

		//즐찾 없음 상태 버튼
		addCon.sFavoriteOff = NiNew cButton;
		if( addCon.sFavoriteOff )
		{
			addCon.sFavoriteOff->Init( NULL, CsPoint::ZERO, CsPoint( 32, 30 ), "SealMaster\\Sealmaster_favorite_off.dds", false );
			addCon.sFavoriteOff->SetTexToken(CsPoint(0,30));
			addCon.sFavoriteOff->SetVisible(false);
			pString->AddButton(addCon.sFavoriteOff, i, CsPoint(-5,17), CsPoint( 32, 30));
		}

		cSprite* pCountBack = NiNew cSprite;
		if( pCountBack )// 갯수 배경
		{			
			pCountBack->Init( NULL, CsPoint(0, 132), CsPoint( 86, 14 ), CsRect(0,0,86,14), "SealMaster\\sealmaster_belt_s.dds", false );
			cString::sSPRITE* sSprite = pString->AddSprite(pCountBack);
			sSprite->SetAutoPointerDelete(true);
		}

		{	// 서버 명
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, NiColor::WHITE);
			ti.SetText( 0 );
			ti.s_eTextAlign = DT_CENTER;
			addCon.sCount = pString->AddText( &ti, CsPoint((86 / 2), 132));

		}

		addCon.spItem  = NiNew cGridListBoxItem(i, CsPoint(86, 128),100 );
		SAFE_POINTER_RET(addCon.spItem);
		addCon.spItem->SetUserData( new sSealGridData(sealInfo.sSealID, sealInfo.sModelID, sealInfo.sSealCount, sealInfo.sGrade, sealInfo.sTableCode,
			sealInfo.sEffectType, sealInfo.sFavorite) );
		addCon.spItem->SetItem(pString);

		std::string backImgFile = GetSystem()->GetSealCardObjectFile_GradBack( sealInfo.sSealID );
		addCon.SetSealGrade( sealInfo.sGrade, backImgFile );
		std::string digimonImg = GetSystem()->GetSealCardObjectFile_Digimon( sealInfo.sSealID );
		addCon.SetSealDigimon( digimonImg );
		addCon.SetFavoriteOnOff( sealInfo.sFavorite );
		addCon.SetSealCount( sealInfo.sSealCount );

		m_pGridListBox->AddItem( addCon.spItem );
		m_mapSealCardCon.insert( std::make_pair( mapItr->first, addCon ) );
	}
}

void cSealMaster::_CreateSealCard( DWORD const& dwModelID )
{
	std::string sNiFile = GetSystem()->GetSealCardObjectFile_NifFile( dwModelID );
	NiStream kStream;
	if( !kStream.Load( sNiFile.c_str() ) )
		return;

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	m_cardEffect.SetNiObject( pNode, CGeometry::CharOutLine );	
	NiTimeController::StartAnimations( pNode, 0.0f );
	pNode->UpdateEffects();
	pNode->Update(0.0f);

	m_fAniTime = 0.0f;

	std::string sBackImg = GetSystem()->GetSealCardObjectFile_GradBack( dwModelID );
 	NiAVObject* pObject = pNode->GetObjectByName( "sealmaster_bg" );
	_ChangeTextureNiObject( pObject, sBackImg );

	std::string sElemental = GetSystem()->GetSealCardObjectFile_Elemental( dwModelID );
	pObject = pNode->GetObjectByName( "sealmaster_elemental" );
	_ChangeTextureNiObject( pObject, sElemental );

	std::string sAttribute = GetSystem()->GetSealCardObjectFile_Attribute( dwModelID );
	pObject = pNode->GetObjectByName( "sealmaster_type" );
	_ChangeTextureNiObject( pObject, sAttribute );

	std::string sDigimon = GetSystem()->GetSealCardObjectFile_Digimon( dwModelID );
	pObject = pNode->GetObjectByName( "sealmaster_monster" );
	_ChangeTextureNiObject( pObject,  sDigimon );	


	//렌더텍스 생성
	m_pRenderTex = NiNew cRenderTex;
	if( m_pRenderTex->Init( CsPoint( g_nScreenWidth, g_nScreenHeight ) ) )
		m_pRenderTex->SetHeightConstant(-2.75f);
	else
		SAFE_NIDELETE( m_pRenderTex );

	m_bViewCard = true;
	cWindow::PlaySound( "System\\SealMaster.wav" );
	GET_SUBCAMERA(CAMERA_10)->SetTranslate(NiPoint3::ZERO);
}

void cSealMaster::_ChangeTextureNiObject( NiAVObject* pObj, std::string const& sFileName )
{
	SAFE_POINTER_RET( pObj );
	NiTexturingProperty* pProperty = ( (NiTexturingProperty*)pObj->GetProperty( NiTexturingProperty::GetType() ) );
	SAFE_POINTER_RET( pProperty );
	NiTexture* pTexture = NiSourceTexture::Create( sFileName.c_str() );
	pProperty->SetBaseTexture( pTexture );
}

void cSealMaster::_CardRender()
{
	if( m_pRenderTex == NULL )
		return;

	g_pEngine->m_spRenderer->EndUsingRenderTargetGroup();	
	g_pEngine->m_spRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ZBUFFER );

	GET_SUBCAMERA(CAMERA_10)->SetRotation( 3.14, 0 );
	GET_SUBCAMERA(CAMERA_10)->SetDist( 1000.0f );
	GET_SUBCAMERA(CAMERA_10)->ChangeStandardFov( g_nScreenWidth, g_nScreenHeight );
	GET_SUBCAMERA(CAMERA_10)->_UpdateCamera();
	g_pEngine->m_spRenderer->SetCameraData( GET_SUBCAMERA(CAMERA_10)->GetCameraObj() );	

	m_cardEffect.Render();

	m_pRenderTex->EndRender( GET_SUBCAMERA(CAMERA_10), CsPoint::ZERO );
}

void cSealMaster::ViewCardEffect(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pSelItem = static_cast<cGridListBoxItem*>(pData);
	sSealGridData* pUserData = dynamic_cast<sSealGridData*>( pSelItem->GetUserData() );
	SAFE_POINTER_RET(pUserData);

	if( pUserData->sSealCount > 0 )
		_CreateSealCard( pUserData->sSealID );
}

void cSealMaster::ViewButton(void* pSender, void* pData)
{
	cGridListBox* pGridBox = static_cast< cGridListBox* >(pSender);
	SAFE_POINTER_RET(pGridBox);
	cString::sBUTTON* pButton = static_cast< cString::sBUTTON* >(pData);
	SAFE_POINTER_RET(pButton);
	int gridItemID = pButton->GetValue();
	const cGridListBoxItem* pSeal = m_pGridListBox->GetItemFormPos(gridItemID);
	SAFE_POINTER_RET(pSeal);
	int buttonID = pSeal->GetItem()->GetListIndex(pButton);
	if(buttonID == -1)
		return;

	if( buttonID == 3 ||buttonID == 4 )
	{
		sSealGridData* pUserData = dynamic_cast<sSealGridData*>( pSeal->GetUserData() );
		SAFE_POINTER_RET(pUserData);
		if(buttonID == 4)
			net::game->SendCardBookmark( static_cast<u2>(pUserData->sTableCode), static_cast<u1>(true) );
		else
			net::game->SendCardBookmark( static_cast<u2>(pUserData->sTableCode), static_cast<u1>(false) );
	}	
}

void cSealMaster::SelectStateFilter(void* pSender, void* pData)
{
	m_nCurStateFilter = m_pCombo->GetCurSel_Idx();
	SetFilter();
}

void cSealMaster::RButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pSender);
	SAFE_POINTER_RET(pData);
	cGridListBox* pGridBox = static_cast< cGridListBox* >(pSender);
	cGridListBoxItem* pGrid = static_cast<cGridListBoxItem*>(pData);
	sSealGridData* pUserData = dynamic_cast<sSealGridData*>( pGrid->GetUserData() );
	SAFE_POINTER_RET(pUserData);
	if(pUserData->sSealCount < 1)
		return;

	int nLeader = GetSystem()->GetSealLeader();

	int	nFlag = LEADER_SELECT;			// 0 : 리더 설정,  1 : 리더 해제
	if( nLeader == pUserData->sTableCode )
		nFlag = LEADER_NOTSELECT;		// 같은 디지몬을 리더 설정하려고 할 때에만 "리더 해제"
	else if( nLeader > 0 )				// 선택된 리더가 있고 다른 리더로 선택하려고 할 때 "리더 변경"
		nFlag = LEADER_CHANGE;

	GetSystem()->SetSelectSealID(pUserData->sSealID);

	// 선택한 카드가 리더였는지 여부를 같이 팝업에 보내줍니다.
	g_pGameIF->GetPopup()->SetPopup( CURSOR_ST.GetPos(), CsPoint( 40, 0 ) , cPopUpWindow::SEAL_MENU, nFlag );
}

void cSealMaster::Update_ToolTip()
{
	// 마우스 좌표
	CsPoint ptMouse = m_pGridListBox->MousePosToWindowPos( CURSOR_ST.GetPos() );
	cGridListBoxItem* pOverGrid = m_pGridListBox->getItemAtPoint(ptMouse);
	SAFE_POINTER_RET(pOverGrid);
	if( pOverGrid->GetVisible() == false )
		return;
	sSealGridData* pUserData = dynamic_cast<sSealGridData*>( pOverGrid->GetUserData() );
	SAFE_POINTER_RET(pUserData);
	cSealMasterContents::SealInfoMap sealInfoMap = GetSystem()->GetSealInfoMap();
	cSealMasterContents::SealInfoMapItr mapItr = sealInfoMap.find(pUserData->sSealID);
	const cSealMasterContents::sSealInfo& sealInfo = mapItr->second;
	CsMaster_Card::sINFO	sInfo = *nsCsFileTable::g_pMaster_CardMng->GetMasterCard(sealInfo.sSealID)->GetInfo();
	
	TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip(pOverGrid->GetWorldPos(),pOverGrid->getItemSize(), pOverGrid->getItemSize().x, cTooltip::SEALMASTER, 
						sealInfo.sSealID, cBaseWindow::WT_CARDINVENTORY, sealInfo.sGrade, sealInfo.sSealCount, &sInfo);
}

void cSealMaster::SetFilter()
{
	m_nViewGridCount = 0;
	m_nViewnActiveGridCount = 0;
	for (int gridIndex = 0; gridIndex < m_pGridListBox->GetItemCount(); ++gridIndex)
	{
		cGridListBoxItem* pGrid = const_cast<cGridListBoxItem*>(m_pGridListBox->GetItemFormPos(gridIndex));
		SAFE_POINTER_RET(pGrid);
		sSealGridData* pUserData = dynamic_cast<sSealGridData*>( pGrid->GetUserData() );
		SAFE_POINTER_RET(pUserData);
		int sealID = pUserData->sSealID;

		//맵조건
		bool bMapFilter = false;
		if ( m_bMapAllFilter || GetSystem()->IsHaveSeal(m_MapFilterPair.first, m_MapFilterPair.second, sealID) )
			bMapFilter = true;
		else if(m_bFavoriteFilter && pUserData->sFavorite)
			bMapFilter = true;

		//능력치 조건
		bool bStateFilter = false;
		int gridStateFilter = EffectTypeTransToComboIndex(pUserData->sEffectType);
		if (m_nCurStateFilter == 0 || m_nCurStateFilter == gridStateFilter)
		{
			bStateFilter = true;
		}

		//count 조건 비상설 조건 맵조건/능력치 조건 무시
		bool bCountFilter = false;
		if (m_bCountFilter == false)
		{
			bCountFilter = true;
		}
		else if (m_bCountFilter == true && (pUserData->sSealCount > 0))
		{
			bMapFilter = bStateFilter = bCountFilter = true;
		}

		if ( bMapFilter && bStateFilter && bCountFilter)
		{
			pGrid->SetVisible(true);
			m_nViewGridCount++;
			if(pUserData->sSealCount > 0)
				m_nViewnActiveGridCount++;
		}
		else
		{
			pGrid->SetVisible(false);
		}
	}
	SetActiveSealCountText(m_nViewnActiveGridCount, m_nViewGridCount);
	m_pGridListBox->VisibleSort();

}

//effecttype을 콤보박스 인덱스로 변환
int cSealMaster::EffectTypeTransToComboIndex(int effectType)
{
	switch(effectType)
	{
	case 0:
		return 0;
	case 1:			//		HP	정수
	case 2:			//		HP	%
		return 1;			
	case 3:			//		DS	정수
	case 4:			//		DS	%
		return 2;			
	case 5:			//		AT	정수
	case 6:			//		AT	%
		return 3;			
	case 7:			//		AS	정수
	case 8:			//		AS	%
		return 4;			
	case 9:			//		CT	정수
	case 10:		//		CT	%
		return 5;			
	case 11:		//		HT	정수
	case 12:		//		HT	%
		return 6;			
	case 13:		//		DE	정수
	case 14:		//		DE	%
		return 7;
	case 15:		//		BL	정수
	case 16:		//		BL	%
		return 8;
	case 17:		//		EV	정수
	case 18:		//		EV	%
		return 9;
	}
	return 0;
}

void cSealMaster::CloseButtonEvent(void* pSender, void* pData)
{
	OnEscapeKey();
}

void cSealMaster::MakeStateFilterComboBox()
{
	m_pCombo = NiNew cComboBox;
	SAFE_POINTER_RET(m_pCombo);
	m_pCombo->Init( GetRoot(), cComboBox::CT_NORMAL, CsPoint(542, 55), CsPoint( 204, 22 ), CFont::FS_10, DT_CENTER, false, false, false);
	m_pCombo->ChangeComboBoxImg( "SealMaster\\btn_combobox.tga", CsPoint(0, 22));
	m_pCombo->SetComboListWindow( "SealMaster\\dropdown_list_0" );

	std::wstring combotext = UISTRING_TEXT("SEALMASTER_STATE_COMBO_ALL");
	m_pCombo->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_GREEN );
	combotext =  UISTRING_TEXT("SEALMASTER_STATE_COMBO_HP");
	m_pCombo->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("SEALMASTER_STATE_COMBO_DS");
	m_pCombo->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("SEALMASTER_STATE_COMBO_AT");
	m_pCombo->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("SEALMASTER_STATE_COMBO_AS");
	m_pCombo->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("SEALMASTER_STATE_COMBO_CT");
	m_pCombo->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("SEALMASTER_STATE_COMBO_HT");
	m_pCombo->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("SEALMASTER_STATE_COMBO_DE");
	m_pCombo->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("SEALMASTER_STATE_COMBO_BL");
	m_pCombo->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	combotext =  UISTRING_TEXT("SEALMASTER_STATE_COMBO_EV");
	m_pCombo->AddItem( const_cast<TCHAR*>(combotext.c_str()), 0, FONT_WHITE );
	m_pCombo->SetCurSel( 0 );

	m_pCombo->AddEvent(cComboBox::COMBO_SELECTITEM, this, &cSealMaster::SelectStateFilter);
	AddChildControl(m_pCombo);
}

void cSealMaster::MakePlusState()
{	
	//추가 능력치 보여주는 BG
	AddSprite(CsPoint(299, 415), CsPoint(385, 150), "SealMaster\\Sealmaster_deco.tga");

	CsPoint ptStateText(360,463);
	CsPoint ptValueText(381,463);
	CsPoint ptGap(120,22);

	std::wstring stateText = UISTRING_TEXT("SEALMASTER_STATE_HP");
	std::wstring wsPlusValueText;
	std::wstring plus = L"+";
	int* pPlusValue = GetSystem()->GetPlusValue();
	
	cText::sTEXTINFO HPstateti;
	HPstateti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	HPstateti.SetText(stateText.c_str());
	AddText(&HPstateti, ptStateText);

	cText::sTEXTINFO HPValueti;
	HPValueti.Init( &g_pEngine->m_FontText, CFont::FS_10, FONT_GREEN );
	m_pPlusValueText[0] = AddText(&HPValueti, ptValueText);
	ptStateText.x += ptGap.x;
	ptValueText.x += ptGap.x;

	stateText = UISTRING_TEXT("SEALMASTER_STATE_DS");

	cText::sTEXTINFO DSstateti;
	DSstateti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	DSstateti.SetText(stateText.c_str());
	AddText(&DSstateti, ptStateText);

	cText::sTEXTINFO DSValueti;
	DSValueti.Init( &g_pEngine->m_FontText, CFont::FS_10, FONT_GREEN );
	m_pPlusValueText[1] = AddText(&DSValueti, ptValueText);
	ptStateText.x += ptGap.x;
	ptValueText.x += ptGap.x;

	stateText = UISTRING_TEXT("SEALMASTER_STATE_AT");

	cText::sTEXTINFO ATstateti;
	ATstateti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ATstateti.SetText(stateText.c_str());
	AddText(&ATstateti, ptStateText);

	cText::sTEXTINFO ATValueti;
	ATValueti.Init( &g_pEngine->m_FontText, CFont::FS_10, FONT_GREEN );
	m_pPlusValueText[2] = AddText(&ATValueti, ptValueText);
	ptStateText.x = 360;
	ptValueText.x = 381;
	ptStateText.y += ptGap.y;
	ptValueText.y += ptGap.y;

	stateText = UISTRING_TEXT("SEALMASTER_STATE_AS");

	cText::sTEXTINFO ASstateti;
	ASstateti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ASstateti.SetText(stateText.c_str());
	AddText(&ASstateti, ptStateText);

	cText::sTEXTINFO ASValueti;
	ASValueti.Init( &g_pEngine->m_FontText, CFont::FS_10, FONT_GREEN );
	m_pPlusValueText[3] = AddText(&ASValueti, ptValueText);
	ptStateText.x += ptGap.x;
	ptValueText.x += ptGap.x;

	stateText = UISTRING_TEXT("SEALMASTER_STATE_CT");

	cText::sTEXTINFO CTstateti;
	CTstateti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	CTstateti.SetText(stateText.c_str());
	AddText(&CTstateti, ptStateText);

	cText::sTEXTINFO CTValueti;
	CTValueti.Init( &g_pEngine->m_FontText, CFont::FS_10, FONT_GREEN );
	m_pPlusValueText[4] = AddText(&CTValueti, ptValueText);
	ptStateText.x += ptGap.x;
	ptValueText.x += ptGap.x;

	stateText = UISTRING_TEXT("SEALMASTER_STATE_HT");

	cText::sTEXTINFO HTstateti;
	HTstateti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	HTstateti.SetText(stateText.c_str());
	AddText(&HTstateti, ptStateText);

	cText::sTEXTINFO HTValueti;
	HTValueti.Init( &g_pEngine->m_FontText, CFont::FS_10, FONT_GREEN );
	m_pPlusValueText[5] = AddText(&HTValueti, ptValueText);
	ptStateText.x = 360;
	ptValueText.x = 381;
	ptStateText.y += ptGap.y;
	ptValueText.y += ptGap.y;

	stateText = UISTRING_TEXT("SEALMASTER_STATE_DE");

	cText::sTEXTINFO DEstateti;
	DEstateti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	DEstateti.SetText(stateText.c_str());
	AddText(&DEstateti, ptStateText);

	cText::sTEXTINFO DEValueti;
	DEValueti.Init( &g_pEngine->m_FontText, CFont::FS_10, FONT_GREEN );
	m_pPlusValueText[6] = AddText(&DEValueti, ptValueText);
	ptStateText.x += ptGap.x;
	ptValueText.x += ptGap.x;

	stateText = UISTRING_TEXT("SEALMASTER_STATE_BL");
	cText::sTEXTINFO BLstateti;
	BLstateti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	BLstateti.SetText(stateText.c_str());
	AddText(&BLstateti, ptStateText);

	cText::sTEXTINFO BLValueti;
	BLValueti.Init( &g_pEngine->m_FontText, CFont::FS_10, FONT_GREEN );
	m_pPlusValueText[7] = AddText(&BLValueti, ptValueText);
	ptStateText.x += ptGap.x;
	ptValueText.x += ptGap.x;

	stateText = UISTRING_TEXT("SEALMASTER_STATE_EV");
	cText::sTEXTINFO EVstateti;
	EVstateti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	EVstateti.SetText(stateText.c_str());
	AddText(&EVstateti, ptStateText);

	cText::sTEXTINFO EVValueti;
	EVValueti.Init( &g_pEngine->m_FontText, CFont::FS_10, FONT_GREEN );
	m_pPlusValueText[8] = AddText(&EVValueti, ptValueText);

	SetPlusValueText();
}

void cSealMaster::SetPlusValueText()
{
	if(m_pPlusValueText[0] == null)
		return;
	std::wstring wsPlus = L"+";
	std::wstring persent = L"%";
	int* pPlusValue = GetSystem()->GetPlusValue();
	for(int i = 0; i < STATEKINDS; ++i)
	{
		if (i == 4 || i == 8 || i == 7)
		{
			std::wstring wsPlusValue;
			float fValue = pPlusValue[i] * 0.01f;
			DmCS::StringFn::From( wsPlusValue, fValue );
			wsPlusValue = wsPlus + wsPlusValue;
			int EVdotPos = wsPlusValue.find('.');
			wsPlusValue = wsPlusValue.substr(0, EVdotPos+3);
			wsPlusValue = wsPlusValue + persent;
			m_pPlusValueText[i]->SetText(wsPlusValue.c_str());
		}
		else
		{
			std::wstring wsPlusValue;
			DmCS::StringFn::From( wsPlusValue, pPlusValue[i] );
			wsPlusValue = wsPlus+wsPlusValue;
			m_pPlusValueText[i]->SetText(wsPlusValue.c_str());
		}
	}
}

void cSealMaster::SetActiveSealCountText(int nActiveSealCount, int nViewCount)
{
	if (m_pActiveSealCountText == null)
		return;
	
	if(m_pActiveSealCountText != null)
		m_pActiveSealCountText->Delete();

	cText::sTEXTINFO OnViewSealCountTI;
	OnViewSealCountTI.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	std::wstring countText = UISTRING_TEXT("SEALMASTER_SEAL_COUNT");

	std::wstring wsSealCount;
	DmCS::StringFn::From( wsSealCount, nActiveSealCount );
	DmCS::StringFn::ReplaceAll( countText, L"#sealcount#", wsSealCount );

	std::wstring wsViewSealCount;
	DmCS::StringFn::From( wsViewSealCount, nViewCount );
	DmCS::StringFn::ReplaceAll( countText, L"#viewcount#", wsViewSealCount );

	g_pStringAnalysis->Cut_Parcing( m_pActiveSealCountText, 200, const_cast<TCHAR*>(countText.c_str()) , &OnViewSealCountTI );
}

bool CompCountforSealMaster(cGridListBoxItem* first, cGridListBoxItem* second)
{
	SAFE_POINTER_RETVAL( first, false );
	SAFE_POINTER_RETVAL( second, false );

	//씰 갯수 비교
	cSealMaster::sSealGridData* pFirstUserData = dynamic_cast<cSealMaster::sSealGridData*>(first->GetUserData());
	cSealMaster::sSealGridData* pSecondUserData = dynamic_cast<cSealMaster::sSealGridData*>(second->GetUserData());
	SAFE_POINTER_RETVAL(pFirstUserData, false);
	SAFE_POINTER_RETVAL(pSecondUserData, false);
	if (pFirstUserData->sSealCount > pSecondUserData->sSealCount)
		return true;
	else if (pFirstUserData->sSealCount < pSecondUserData->sSealCount)
		return false;
	//같을때
	else
		return false;
}

bool CompSealIDforSealMaster(cGridListBoxItem* first, cGridListBoxItem* second)
{
	SAFE_POINTER_RETVAL( first, false );
	SAFE_POINTER_RETVAL( second, false );

	cSealMaster::sSealGridData* pFirstUserData = dynamic_cast<cSealMaster::sSealGridData*>(first->GetUserData());
	cSealMaster::sSealGridData* pSecondUserData = dynamic_cast<cSealMaster::sSealGridData*>(second->GetUserData());
	SAFE_POINTER_RETVAL(pFirstUserData, false);
	SAFE_POINTER_RETVAL(pSecondUserData, false);
	//씰 아이디 비교
	if (pFirstUserData->sSealID < pSecondUserData->sSealID)
		return true;
	
	return false;
}

void cSealMaster::SortforSealMaster()
{
	SAFE_POINTER_RET( m_pGridListBox );
	list<cGridListBoxItem*>* listItems = m_pGridListBox->GetListItem();
	listItems->sort(CompSealIDforSealMaster);
	
	m_pGridListBox->RecountItemID(0);
	m_pGridListBox->configureScrollbars();
}

void cSealMaster::SortforSealMasterCount()
{
	SAFE_POINTER_RET( m_pGridListBox );
	list<cGridListBoxItem*>* listItems = m_pGridListBox->GetListItem();
	listItems->sort(CompCountforSealMaster);

	m_pGridListBox->RecountItemID(0);
	m_pGridListBox->configureScrollbars();
}

//////////////////////////////////////////////////////////////////////////

bool cSealMaster::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eSetFavorite, this );
	GetSystem()->Register( SystemType::eSetSealCount, this );
	GetSystem()->Register( SystemType::eSetPlusValu, this );

	return true;
}

void cSealMaster::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
		//즐겨찾기 된 상태와 안된 상태의 버튼을 스위칭 해준다.
	case SystemType::eSetFavorite:
		{
			int serverSealID;
			bool bFavorite;
			kStream >> serverSealID >> bFavorite;

			std::map<DWORD, sSealCardControl>::iterator it = m_mapSealCardCon.find( serverSealID );
			if( it == m_mapSealCardCon.end() )
				break;

			it->second.SetFavoriteOnOff( bFavorite );
			SetFilter();
		}
		break;
		//Seal 숫자가 변경되면 표시되는 숫자를 변경하고 등급 변화가 있으면 이미지를 변경한다. 
	case SystemType::eSetSealCount:
		{
			int nSealID;
			bool bChangeGrade;
			kStream >> nSealID >> bChangeGrade;
			
			std::map<DWORD, sSealCardControl>::iterator it = m_mapSealCardCon.find( nSealID );
			if( it == m_mapSealCardCon.end() )
				break;

			cSealMasterContents::SealInfoMap sealInfoMap = GetSystem()->GetSealInfoMap();
			cSealMasterContents::SealInfoMapItr sealInfoItr = sealInfoMap.find(nSealID);
			if( sealInfoItr == sealInfoMap.end() )
				break;

			it->second.SetSealCount( sealInfoItr->second.sSealCount );
			if( bChangeGrade )
			{
				std::string backImgFile = GetSystem()->GetSealCardObjectFile_GradBack( sealInfoItr->second.sSealID );
				it->second.SetSealGrade( sealInfoItr->second.sGrade, backImgFile );
			}
			SetFilter();
		}
		break;		
	case SystemType::eSetPlusValu:	SetPlusValueText();		break;//등급 변경이 일어나 추가 능력치가 변경되면 반영한다.
	default:
		break;
	}
}
