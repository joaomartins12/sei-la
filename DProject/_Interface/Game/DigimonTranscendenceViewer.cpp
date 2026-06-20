
#include "stdafx.h"
#include "DigimonTranscendenceViewer.h"

//////////////////////////////////////////////////////////////////////////
void CDigimonTanscendenceViewer::sMaterialCtr::SetIcon( DWORD const& dwIdx, int const& nSlot )
{
	dwDigimonIDX = dwIdx;
	nArchiveSlot = nSlot;
	SAFE_POINTER_RET( pkSlotIcon );

	if( 0 == dwDigimonIDX )
	{
		pkSlotIcon->SetVisible(false);
		return;
	}

	CsDigimon* pkTableInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDigimonIDX );
	if(pkTableInfo != NULL &&  pkTableInfo->GetInfo())
	{
		std::string kPath = g_pModelDataMng->GetSmallModelIconFile( pkTableInfo->GetInfo()->s_dwModelID );
		pkSlotIcon->ChangeTexture( kPath.c_str(), false );
		pkSlotIcon->SetVisible(true);
	}
	else
		pkSlotIcon->SetVisible(false);
}

void CDigimonTanscendenceViewer::sMaterialCtr::SetEmpty()
{
	SetIcon( 0, 0 );
}

bool CDigimonTanscendenceViewer::sMaterialCtr::IsRegist() const
{
	if( 0 == dwDigimonIDX )
		return false;
	if( 0 == nArchiveSlot )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////

void CDigimonTanscendenceViewer::sSubMaterialCtr::SetIcon( DWORD const& dwIdx, int const& nHaveCount, int const& nNeedCount )
{
	dwItemIDX = dwIdx;
	sHaveCount = nHaveCount;
	sNeedCount = nNeedCount;
	bool bShow = (dwIdx == 0)?false : true;

	if( pkItemIcon )
		pkItemIcon->SetVisible( bShow );
	
	if( pCountText )
		pCountText->SetVisible( bShow );

	if( bShow )
	{
		pkItemIcon->ChangeIcon( ICONITEM::ITEM_ICON, dwIdx, 1);
		std::wstring msg;
		DmCS::StringFn::Format( msg, L"%d / %d", sHaveCount, sNeedCount );
		pCountText->SetText( msg.c_str() );
		if( nHaveCount < nNeedCount )
			pCountText->SetColor( FONT_RED );
		else
			pCountText->SetColor( FONT_GREEN );
	}
}

void CDigimonTanscendenceViewer::sSubMaterialCtr::SetEmpty()
{
	dwItemIDX = 0;
	sHaveCount = 0;
	sNeedCount = 0;

	if( pkItemIcon )
		pkItemIcon->SetVisible( false );

	if( pCountText )
		pCountText->SetVisible( false );
}

DWORD CDigimonTanscendenceViewer::sSubMaterialCtr::GetItemCode() const
{
	return dwItemIDX;
}

void CDigimonTanscendenceViewer::sSubMaterialCtr::UpdateMaterialCount( int const& nMaterialCount)
{
	if( 0 == dwItemIDX || 0 == sNeedCount )
		return;

	SAFE_POINTER_RET( pCountText );

	int nCount = (nMaterialCount == 0)?1:nMaterialCount;

	std::wstring msg;
	DmCS::StringFn::Format( msg, L"%d / %d", sHaveCount, sNeedCount * nCount );
	pCountText->SetText( msg.c_str() );
	if( sHaveCount < sNeedCount * nCount )
		pCountText->SetColor( FONT_RED );
	else
		pCountText->SetColor( FONT_GREEN );
}
//////////////////////////////////////////////////////////////////////////
CDigimonTanscendenceViewer::CDigimonTanscendenceViewer()
:m_pkPlayer(NULL),m_pkAccompanies(NULL),m_pkGridMaterial(NULL)
,m_pNormalExpAddBtn(NULL),m_pAdvancedExpAddBtn(NULL),m_pTranscendBtn(NULL),m_pkInsertDigimon(NULL)
,m_pSpendMoney(NULL),m_pProgressbar(NULL),m_pOverProgressbar(NULL),m_pEffect(NULL),m_pResultEffect(NULL)
,m_pExpGuageText(NULL),m_pSuccessEffect(NULL),m_pExcellentEffect(NULL), m_pHelpBtn(NULL)
{
	Construct();
}

CDigimonTanscendenceViewer::~CDigimonTanscendenceViewer()
{
	if(GetSystem() )
		GetSystem()->UnRegisterAll( this );	
}

void CDigimonTanscendenceViewer::Destroy()
{
	DeleteResource();
}

void CDigimonTanscendenceViewer::DeleteResource()
{
	DeleteScript();
}

void CDigimonTanscendenceViewer::DeleteScript()
{
	cScriptUI::DeleteScript();
}

void CDigimonTanscendenceViewer::Create(  cWindow* pkRoot, int nValue /*= 0 */ )
{
	InitScript( pkRoot, NULL, CsPoint( 35,115 ), CsPoint( 348, 467 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP);
	AddSprite(CsPoint(0,0), CsPoint(348,402), "TacticsHouse\\Storage_info_03.tga" );
	AddSprite(CsPoint(2,415), CsPoint(200,35), "TacticsHouse\\Storage_M_field.tga" );
	m_pSpendMoney = NiNew cMoneyIF;
	if( m_pSpendMoney )
	{
		m_pSpendMoney->Init( cMoneyIF::DISP_FULL, (u8)250000, false, FONT_WHITE, DT_RIGHT, CFont::FS_12 );
		m_pSpendMoney->SetPos(CsPoint( 30, 425 ));
		AddChildControl(m_pSpendMoney);
	}
	CreateTaticsDigimonUI();


	m_pEffect = NiNew cSpriteAni;
	m_pEffect->Init( cSpriteAni::ONCE, m_pRoot, CsPoint(74,26), CsPoint( 200, 200 ), "TacticsHouse\\Storage_effect_", "dds", 20, false, CsPoint::ZERO, cSpriteAni::SPRITE_PAGE, 0 );
	m_pEffect->AddEvent( cSpriteAni::SPRITEANI_END, this, &CDigimonTanscendenceViewer::_EffectEnd );
	m_pEffect->SetAniTime( 0.1f );
	m_pEffect->PlayOff();
	m_pEffect->SetVisible(false);
	m_pEffect->SetUserData( new sEffectType(0) );	
	AddChildControl( m_pEffect );


	m_pkInsertDigimon = NiNew cGridListBox;
	if( m_pkInsertDigimon )
	{
		m_pkInsertDigimon->Init( GetRoot(), CsPoint(151, 104), CsPoint(50, 60), CsPoint::ZERO, CsPoint( 46, 46), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		AddChildControl(m_pkInsertDigimon);
		cString * pItem = NiNew cString;
		cSprite* pImage = NiNew cSprite;	// 1
		pImage->Init( NULL, CsPoint::ZERO, CsPoint( 46, 46), "TacticsHouse\\Storage_lock.tga", false );
		cString::sSPRITE* sSprite = pItem->AddSprite( pImage, CsPoint::ZERO, CsPoint( 46, 46)  );	
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);
		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(0, CsPoint( 46, 46));
		addItem->SetItem( pItem );
		m_pkInsertDigimon->AddItem( addItem );

	}

	m_pkGridMaterial = NiNew cGridListBox;
	if( m_pkGridMaterial )
	{
		m_pkGridMaterial->Init( GetRoot(), CsPoint(35, 177),  CsPoint(290, 110), CsPoint(12,11), CsPoint( 46, 46), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		m_pkGridMaterial->SetMouseOverImg( "Lobby\\CharacterCreate\\digimon_glow.tga" );
		m_pkGridMaterial->SetAutoSelection(false);
		m_pkGridMaterial->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &CDigimonTanscendenceViewer::OnMouseRClickMaterialRemove );
		m_pkGridMaterial->AddEvent( cGridListBox::GRID_CHANGED_MOUSEON_ITEM, this, &CDigimonTanscendenceViewer::OnMouseMoveOnMaterialDigimon );
		m_pkGridMaterial->SetBackOverAndSelectedImgRender(false);
		AddChildControl(m_pkGridMaterial);
		_MakeEmptyMaterialSlot(MAX_TRANSCENDENCE_MAX_MATERIAL_COUNT);
	}

	m_pSubMaterial = NiNew cGridListBox;
	if( m_pSubMaterial )
	{
		m_pSubMaterial->Init( GetRoot(), CsPoint(30, 310),  CsPoint(290, 23), CsPoint::ZERO, CsPoint( 145, 23), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		m_pSubMaterial->SetAutoSelection(false);
		m_pSubMaterial->AddEvent( cGridListBox::GRID_CHANGED_MOUSEON_ITEM, this, &CDigimonTanscendenceViewer::OnMouseMoveOnSubMaterialItem );
		AddChildControl(m_pSubMaterial);
		_MakeEmptySubMaterialSlot(2);
	}

	m_pNormalExpAddBtn = AddButton( CsPoint(26, 346), CsPoint(137, 46), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_s.tga", cWindow::SD_Bu3  );
	if( m_pNormalExpAddBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText( UISTRING_TEXT("DIGIMONTRANSCEND_NORMAL_BTN_TEXT").c_str() );
		ti.s_eTextAlign = DT_CENTER;
		m_pNormalExpAddBtn->SetText( &ti );
		m_pNormalExpAddBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CDigimonTanscendenceViewer::PressNormalAddExpBtn);
	}

	m_pAdvancedExpAddBtn = AddButton( CsPoint(185, 346), CsPoint(137, 46), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_s.tga", cWindow::SD_Bu3  );
	if( m_pAdvancedExpAddBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText( UISTRING_TEXT("DIGIMONTRANSCEND_ADVANCED_BTN_TEXT").c_str() );
		ti.s_eTextAlign = DT_CENTER;
		m_pAdvancedExpAddBtn->SetText( &ti );
		m_pAdvancedExpAddBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CDigimonTanscendenceViewer::PressAdvancedAddExpBtn);
	}

	m_pTranscendBtn = AddButton( CsPoint(187, 412), CsPoint(135, 44), CsPoint( 0, 44 ), "CommonUI\\Simple_btn_Y_S.tga", cWindow::SD_Bu3  );
	if( m_pTranscendBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_14 );
		ti.SetText( UISTRING_TEXT("DIGIMONTRANSCEND_TRANS_BTN_TEXT").c_str() );
		ti.s_eTextAlign = DT_CENTER;
		m_pTranscendBtn->SetText( &ti );
		m_pTranscendBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CDigimonTanscendenceViewer::PressTranscendBtn);
	}

	m_pProgressbar = NiNew cUI_ProgressBar;
	if( m_pProgressbar )
	{
		m_pProgressbar->Init( GetRoot(), CsPoint(36, 294 ), CsPoint(277, 5 ), false );
		m_pProgressbar->SetGuageImage( "TargetWindow\\DsBar1.tga" );
		m_pProgressbar->SetProgressRect( CsRect( 0, 0, 277, 5 ) );
		m_pProgressbar->SetRange( 100.0f );
		m_pProgressbar->SetViewTextType( cUI_ProgressBar::NONE );
		AddChildControl(m_pProgressbar);
	}

	m_pOverProgressbar = NiNew cUI_ProgressBar;
	if( m_pOverProgressbar )
	{
		m_pOverProgressbar->Init( GetRoot(), CsPoint(36, 294 ), CsPoint(277, 5 ), false );
		m_pOverProgressbar->SetGuageImage( "TargetWindow\\HpBar1.tga" );
		m_pOverProgressbar->SetProgressRect( CsRect( 0, 0, 277, 5 ) );
		m_pOverProgressbar->SetRange( 100.0f );
		m_pOverProgressbar->SetViewTextType( cUI_ProgressBar::NONE );
		AddChildControl(m_pOverProgressbar);
	}

	cText::sTEXTINFO ti;
	ti.Init(CFont::FS_10);
	ti.SetText( L"%d / %d ");// 현재 획득 포인드
	ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
	m_pExpGuageText = AddText( &ti, CsPoint( 174, 296 ) );

	_UpdateExpProgressbar(NULL);

	m_pResultEffect = NiNew cSpriteAni;
	m_pResultEffect->Init( cSpriteAni::ONCE, m_pRoot, CsPoint(74,26), CsPoint( 200, 200 ), "TacticsHouse\\Storage_effect_R_", "dds", 8, false, CsPoint::ZERO, cSpriteAni::SPRITE_PAGE, 0 );
	m_pResultEffect->SetAniTime( 0.1f );
	m_pResultEffect->PlayOff();
	m_pResultEffect->SetVisible(false);
	AddChildControl( m_pResultEffect );

	m_pSuccessEffect = NiNew cSpriteAni;
	m_pSuccessEffect->Init( cSpriteAni::ONCE, m_pRoot, CsPoint((348-196)>>1,26), CsPoint( 196, 54 ), "TacticsHouse\\Storage_exp_success.tga", NULL, 1, false, CsPoint::ZERO, cSpriteAni::SPRITE_POS, 0 );
	m_pSuccessEffect->SetAniTime( 1.0f );
	m_pSuccessEffect->PlayOff();
	m_pSuccessEffect->SetVisible(false);
	AddChildControl( m_pSuccessEffect );

	m_pExcellentEffect = NiNew cSpriteAni;
	m_pExcellentEffect->Init( cSpriteAni::ONCE, m_pRoot, CsPoint((348-208)>>1,26), CsPoint( 208, 90 ), "TacticsHouse\\Storage_exp_Excellent.tga", NULL, 1, false, CsPoint::ZERO, cSpriteAni::SPRITE_POS, 0 );
	m_pExcellentEffect->SetAniTime( 1.0f );
	m_pExcellentEffect->PlayOff();
	m_pExcellentEffect->SetVisible(false);
	AddChildControl( m_pExcellentEffect );

	m_pHelpBtn = AddButton( CsPoint( 220, 112 ), CsPoint( 24, 32 ), CsPoint( 0, 32 ), "DigimonStatus_New\\Digimon_help_btn.tga" );
	if( m_pHelpBtn )
		m_pHelpBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CDigimonTanscendenceViewer::_OnClickHelpButton );
}

BOOL CDigimonTanscendenceViewer::UpdateMouse()
{
	if(m_pkPlayer && m_pkPlayer->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;

	if( m_pkInsertDigimon && m_pkInsertDigimon->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;

	if(m_pkAccompanies && m_pkAccompanies->Update_ForMouse(CURSOR_ST.GetPos()) )
		return TRUE;

	if( m_pkGridMaterial && m_pkGridMaterial->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return TRUE;

	if( m_pSubMaterial && m_pSubMaterial->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return TRUE;	

	if( m_pNormalExpAddBtn && m_pNormalExpAddBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return TRUE;

	if( m_pAdvancedExpAddBtn && m_pAdvancedExpAddBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return TRUE;

	if( m_pTranscendBtn && m_pTranscendBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return TRUE;

	if( m_pHelpBtn && m_pHelpBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return TRUE;

	return FALSE;
}

void CDigimonTanscendenceViewer::ResetDevice()
{
	ResetDeviceScript();
}

void CDigimonTanscendenceViewer::CreateTaticsDigimonUI()
{
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Init My Digimon
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	m_pkPlayer = NiNew cGridListBox;
	if( m_pkPlayer )
	{
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
		m_pkPlayer->Init( GetRoot(), CsPoint(23, 31), CsPoint(50, 60), CsPoint::ZERO, CsPoint( 46, 46), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
#else
		m_pkPlayer->Init( GetRoot(), CsPoint(41, 31), CsPoint(50, 60), CsPoint::ZERO, CsPoint( 46, 46), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
#endif
		m_pkPlayer->AddEvent( cGridListBox::GRID_SELECT_CHANGE_ITEM, this, &CDigimonTanscendenceViewer::OnItemClicked_UsingDigimon );
		m_pkPlayer->AddEvent( cGridListBox::GRID_CHANGED_MOUSEON_ITEM, this, &CDigimonTanscendenceViewer::OnMouseMoveOnItem );
		m_pkPlayer->SetMouseOverImg( "Lobby\\CharacterCreate\\digimon_glow.tga" );
		m_pkPlayer->SetSelectedImg( "TacticsHouse\\Storage_Insert.tga", CsPoint( -2, -21 ), CsPoint( 50,68 ));
		m_pkPlayer->SetBackOverAndSelectedImgRender(false);
		AddChildControl(m_pkPlayer);
		cString * pItem = NiNew cString;
		cSprite* pImage = NiNew cSprite;	// 1
		pImage->Init( NULL, CsPoint::ZERO, CsPoint( 46, 46), "TacticsHouse\\Storage_lock.tga", false );
		cString::sSPRITE* sSprite = pItem->AddSprite( pImage, CsPoint::ZERO, CsPoint( 46, 46)  );	
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);
		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(0, CsPoint( 46, 46));
		addItem->SetItem( pItem );
		m_pkPlayer->AddItem( addItem );

	}
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Init Accompany Digimon
	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
	const CsPoint kAccomGridPos = CsPoint(99, 31);
#else
	const CsPoint kAccomGridPos = CsPoint(118, 31);
#endif
	const CsPoint kAccomGridSize = CsPoint(230, 60);
	const CsPoint kAccomGap = CsPoint(14, 0);
	m_pkAccompanies = NiNew cGridListBox;
	if( m_pkAccompanies )
	{
		m_pkAccompanies->Init( GetRoot(), kAccomGridPos, kAccomGridSize, 
			kAccomGap, CsPoint( 46, 46), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		AddChildControl(m_pkAccompanies);
		m_pkAccompanies->AddEvent( cGridListBox::GRID_SELECT_CHANGE_ITEM, this, &CDigimonTanscendenceViewer::OnItemClicked_AccompanyDigimon );
		m_pkAccompanies->AddEvent( cGridListBox::GRID_CHANGED_MOUSEON_ITEM, this, &CDigimonTanscendenceViewer::OnMouseMoveOnAccompanyItem );
		m_pkAccompanies->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &CDigimonTanscendenceViewer::OnMouseRClickInAccompany );
		m_pkAccompanies->AddEvent( cGridListBox::GRID_MOUSE_DOWN_ON_ITEM, this, &CDigimonTanscendenceViewer::OnMouseDownOnAccompanyItem );
		m_pkAccompanies->AddEvent( cGridListBox::GRID_MOUSE_UP_ON_GRIDLIST, this, &CDigimonTanscendenceViewer::OnMouseUpOnAccompanyItem );
		m_pkAccompanies->SetUsePressedMoveEvent(true);
		m_pkAccompanies->SetMouseOverImg( "Lobby\\CharacterCreate\\digimon_glow.tga" );
		m_pkAccompanies->SetSelectedImg( "TacticsHouse\\Storage_Insert.tga", CsPoint( -2, -21 ), CsPoint( 50,68 ));
		m_pkAccompanies->SetBackOverAndSelectedImgRender(false);	

		for(int i =0 ; i < nLimit::DigimonBaseSlot - 1; ++i)
		{	
			cString * pItem = NiNew cString;
			cSprite* pImage = NiNew cSprite;	// 1
			pImage->Init( NULL, CsPoint::ZERO,CsPoint( 24, 34),  "TacticsHouse\\Storage_lock.tga", false );
			cString::sSPRITE* sSprite = pItem->AddSprite( pImage, CsPoint( 11, 5 ), CsPoint( 24, 34)  );	
			if( sSprite )
				sSprite->SetAutoPointerDelete(true);
			cGridListBoxItem * addItem  = NiNew cGridListBoxItem(i, CsPoint( 33, 41));
			addItem->SetItem( pItem );
			m_pkAccompanies->AddItem( addItem );
		}
	}
}

void CDigimonTanscendenceViewer::_MakeEmptyMaterialSlot( int const& nEmptyCount )
{
	m_materialList.clear();
	SAFE_POINTER_RET( m_pkGridMaterial );
	for(int i =0 ; i < nEmptyCount; ++i)
	{	
		cString * pItem = NiNew cString;
		sMaterialCtr addItems;
		addItems.pkSlotIcon = NiNew cSprite;
		addItems.pkSlotIcon->Init( NULL, CsPoint::ZERO,CsPoint( 42, 42),  " ", false );
		cString::sSPRITE* sSprite = pItem->AddSprite( addItems.pkSlotIcon, CsPoint(2,2), CsPoint( 42, 42)  );	
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);
		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(i, CsPoint( 46, 46 ));
		addItem->SetItem( pItem );
		m_pkGridMaterial->AddItem( addItem );

		addItems.SetEmpty();
		m_materialList.insert( std::make_pair(i, addItems ) );
	}
}

void CDigimonTanscendenceViewer::_MakeEmptySubMaterialSlot( int const& nEmptyCount )
{	
	m_subMaterialList.clear();
	SAFE_POINTER_RET( m_pSubMaterial );
	for(int i =0 ; i < nEmptyCount; ++i)
	{	
		cString * pItem = NiNew cString;
		sSubMaterialCtr addItems;
		addItems.pkItemIcon = pItem->AddIcon( CsPoint(21,21), ICONITEM::ITEM_ICON, 0, 1 );
		cText::sTEXTINFO ti2;
		ti2.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		addItems.pCountText = pItem->AddText( &ti2, CsPoint(50,5) );
		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(i, CsPoint( 145, 23 ));
		addItem->SetItem( pItem );
		m_pSubMaterial->AddItem( addItem );

		addItems.SetEmpty();
		m_subMaterialList.push_back( addItems );
	}
}

void CDigimonTanscendenceViewer::UpdateViewer(void)
{
	SAFE_POINTER_RET(m_pkPlayer);
	SAFE_POINTER_RET(m_pkAccompanies);
	DWORD UsingDigimonID = GetSystem()->GetUsingDigimon();
	cGridListBoxItem* pkItem = const_cast<cGridListBoxItem*>(m_pkPlayer->GetItemFormPos(USING));
	UpdateSlotIcon(UsingDigimonID, pkItem);

	for(int i= ACCOMPANY1; i <= GetSystem()->GetOpenedAccompanySlotCnt(); ++i)
	{
		pkItem = const_cast<cGridListBoxItem*>( m_pkAccompanies->GetItemFormPos(i - 1) );

		DWORD dwCompanyID = GetSystem()->GetCompanyID(i);
		UpdateSlotIcon(dwCompanyID, pkItem );
		UpdateLevelIcon(dwCompanyID, pkItem);
	}

	const cData_PostLoad::sDATA* pkDATA = GetSystem()->GetSelectedDigimon();

	if( NULL == pkDATA )
		_SetSubMaterialItem( 0 );
	else
		_SetSubMaterialItem( pkDATA->s_dwBaseDigimonID );	

	std::map<int,sMaterialCtr>::iterator it = m_materialList.begin();
	for( ; it != m_materialList.end(); ++it )
		it->second.SetEmpty();

	CDigimonArchiveContents::TransMaterialInfo const& pSlotInfo = GetSystem()->GetTransMaterialInfo();
	CDigimonArchiveContents::TransMaterialConstIter it2 = pSlotInfo.begin();
	for( int n = 0; it2 != pSlotInfo.end(); ++it2, ++n )
		_SetMaterialDigimon( n, (*it2) );

	int nMaterialCount = pSlotInfo.size();
	_UpdateMaterialCount( nMaterialCount );

	_UpdateExpProgressbar(pkDATA);
	_UpdateTranscendNeedMoney( pkDATA );
}

void CDigimonTanscendenceViewer::_SetMaterialDigimon( int const& nIdx, CDigimonArchiveContents::sTranscendMaterialInfo const& transData )
{
	std::map<int,sMaterialCtr>::iterator it = m_materialList.find(nIdx);
	if( it == m_materialList.end() )
		return;
	it->second.SetIcon( transData.RegisteredDigimonID, transData.RegisteredDigimonArchiveSlotIdx );
}

void CDigimonTanscendenceViewer::UpdateSlotIcon(DWORD dwDigimonID, cGridListBoxItem* pkItem)
{
	SAFE_POINTER_RET( pkItem );
	cString * pItems = pkItem->GetItem();
	SAFE_POINTER_RET( pItems );
	pItems->Delete();

	if( 0 == dwDigimonID )
		return;

	const CsPoint kSlotSizeTest = CsPoint( 44, 44 );
	const CsPoint kSlotDelta = CsPoint( 1, 1);
	//디지몬 Icon
	CsDigimon* pkTableInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDigimonID );
	if(pkTableInfo != NULL &&  pkTableInfo->GetInfo())
	{
		std::string kPath = g_pModelDataMng->GetSmallModelIconFile( pkTableInfo->GetInfo()->s_dwModelID );
		cSprite* pkSlotIcon = NiNew cSprite;	// 1
		pkSlotIcon->Init( NULL, CsPoint::ZERO, kSlotSizeTest, kPath.c_str(), false, NiColor::WHITE, false  );
		cString::sSPRITE* sSprite = pkItem->GetItem()->AddSprite( pkSlotIcon, kSlotDelta, kSlotSizeTest  );	
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);
	}
}

void CDigimonTanscendenceViewer::UpdateLevelIcon(DWORD dwDigimonID,cGridListBoxItem* pkItem)
{
	if( 0 == dwDigimonID )
		return;

	const CsPoint kSlotSizeTest = CsPoint( 44, 44 );
	const CsPoint kSlotDelta = CsPoint( 1, 1);
	//디지몬 LV 커버
	cData_PostLoad::sDATA* pkAccomInfo = GetSystem()->GetDigimonDataInAccompany(pkItem->getID() + 1);
	if(pkAccomInfo != NULL)
	{
		std::string kPath = CDigimonArchiveViewer::GetLVToPath(pkAccomInfo->s_nLevel);
		cSprite* pGrade = NiNew cSprite;	// 1
		pGrade->Init( NULL, CsPoint::ZERO, kSlotSizeTest, kPath.c_str(), false );
		cString::sSPRITE* sSprite = pkItem->GetItem()->AddSprite( pGrade, kSlotDelta, kSlotSizeTest  );	
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);
	}
}

void CDigimonTanscendenceViewer::OnMouseRClickInAccompany(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	SAFE_POINTER_RET(pItem);
	if(pItem->GetVisible()== false)
		return;

	GetSystem()->OnRClick_AccompanyDigimon(pItem->getID());
}

void CDigimonTanscendenceViewer::OnMouseMoveOnItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;

	GetSystem()->OnShowToolTip(CDigimonArchiveContents::USING_DIGIMON, -1);
}

void CDigimonTanscendenceViewer::OnMouseDownOnAccompanyItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
	if(GetSystem()->GetNowDrag())
	{
		DWORD dwDigimonID = GetSystem()->GetCompanyID(pItem->getID()+1);
		if(dwDigimonID != 0)
		{
			DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDigimonID )->GetInfo()->s_dwModelID;	
			sCHAR_IMAGE* pkCharImage = g_pCharResMng->CharImageResLoad( dwModelID );
			if(pkCharImage != NULL)
			{
				CURSOR_ST.SetIcon( CURSOR_ICON::CI_TACTICSHOUSE, 1, pItem->getID(), NULL );
				CURSOR_ST.SetIconSubInfo( (int)pkCharImage, 0, 0 );
				GetSystem()->ResetDragInfo();
				GetSystem()->SetDragInfo(CDigimonArchiveContents::SRC_ACCOMPANY, pItem->getID()+1);
			}
			g_pCharResMng->CharImageResDelete(&pkCharImage);
		}
	}
	else
	{
		GetSystem()->SetNowDrag(true);
		//m_bisDragable = true;
		CURSOR_ST.SetButtonState(CURSOR::eTYPE::BUTTON_NONE);
	}
}

void CDigimonTanscendenceViewer::OnMouseUpOnAccompanyItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;
	if(GetSystem()->GetNowDrag() == true)
	{
		//m_bisDragable= false;
		GetSystem()->SetDragInfo(CDigimonArchiveContents::DEST_ACCOMPANY, pItem->getID()+1);
		GetSystem()->ExcuteDragInfo();
	}
	GetSystem()->SetNowDrag(false);

}

void CDigimonTanscendenceViewer::OnMouseMoveOnAccompanyItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	if(pItem->GetVisible()== false)
		return;

	GetSystem()->OnShowToolTip(CDigimonArchiveContents::ACCOMPANY_DIGIMON, pItem->getID()+1);
}

void CDigimonTanscendenceViewer::ReleaseSelect(void)
{
	if(m_pkPlayer != NULL)
		m_pkPlayer->releaseSelection();
	if(m_pkAccompanies != NULL)
		m_pkAccompanies->releaseSelection();
}

void CDigimonTanscendenceViewer::ReleaseSelect(CDigimonArchiveContents::SELECT_PART ePart)
{
	switch(ePart)
	{
	case CDigimonArchiveContents::ACCOMPANY_DIGIMON:
		if(m_pkPlayer != NULL)
			m_pkPlayer->releaseSelection();
		break;
	case CDigimonArchiveContents::USING_DIGIMON:
		if(m_pkAccompanies != NULL)
			m_pkAccompanies->releaseSelection();	
		break;
	}
}

void CDigimonTanscendenceViewer::OnItemClicked_UsingDigimon(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	SAFE_POINTER_RET(pItem);
	if(pItem->GetVisible()== false)
		return;

	GetSystem()->OnShowPartnerDigimonInfo();
}

void CDigimonTanscendenceViewer::OnItemClicked_AccompanyDigimon(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	SAFE_POINTER_RET(pItem);
	if(pItem->GetVisible()== false)
		return;

	int CurrSlotIdx = pItem->getID();
	GetSystem()->OnShowDigimonInfoInAccompany(CurrSlotIdx);
}

void CDigimonTanscendenceViewer::_SetChargeExpStart( int const& nChargeType )
{
	SAFE_POINTER_RET( m_pEffect );
	sEffectType* pEffectType = dynamic_cast<sEffectType*>(m_pEffect->GetUserData());
	SAFE_POINTER_RET( pEffectType );
	pEffectType->nChargeType = nChargeType;

	m_pEffect->SetPlayType( cSpriteAni::ONCE );
	m_pEffect->PlayOn(true);

	cWindow::PlaySound( "System\\transcendental.wav" );

	if( m_pTranscendBtn )
		m_pTranscendBtn->SetEnable(false);
	if( m_pNormalExpAddBtn )
		m_pNormalExpAddBtn->SetEnable(false);
	if( m_pAdvancedExpAddBtn )
 		m_pAdvancedExpAddBtn->SetEnable( false );

	if( m_pkPlayer )
		m_pkPlayer->SetEnable(false);

	if( m_pkAccompanies )
		m_pkAccompanies->SetEnable(false);

	if( m_pkGridMaterial )
		m_pkGridMaterial->SetEnable(false);

	if( m_pkInsertDigimon )
		m_pkInsertDigimon->SetEnable(false);
}

void CDigimonTanscendenceViewer::_SetChargeExpEnd()
{
	if( m_pEffect )
	{
		sEffectType* pEffectType = dynamic_cast<sEffectType*>(m_pEffect->GetUserData());
		SAFE_POINTER_RET( pEffectType );
		pEffectType->nChargeType = 0;

		m_pEffect->SetPlayType( cSpriteAni::ONCE );
		m_pEffect->PlayOff();
		m_pEffect->SetVisible(false);
	}

	if( m_pTranscendBtn )
		m_pTranscendBtn->SetEnable(true);
	if( m_pNormalExpAddBtn )
		m_pNormalExpAddBtn->SetEnable(true);
	if( m_pAdvancedExpAddBtn )
		m_pAdvancedExpAddBtn->SetEnable( true );

	if( m_pkPlayer )
		m_pkPlayer->SetEnable(true);

	if( m_pkAccompanies )
		m_pkAccompanies->SetEnable(true);

	if( m_pkGridMaterial )
		m_pkGridMaterial->SetEnable(true);

	if( m_pkInsertDigimon )
		m_pkInsertDigimon->SetEnable(true);
}

void CDigimonTanscendenceViewer::PressNormalAddExpBtn(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );

	if( !GetSystem()->IsEnableExpCharge( eDigimonTranscendenceFlag::eGeneralChargeEXP ) )
		return;

	_SetChargeExpStart( eDigimonTranscendenceFlag::eGeneralChargeEXP );
	GetSystem()->SetWindowLock(true);
}

void CDigimonTanscendenceViewer::PressAdvancedAddExpBtn(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	if( !GetSystem()->IsEnableExpCharge( eDigimonTranscendenceFlag::eHighChargeEXP ) )
		return;

	_SetChargeExpStart( eDigimonTranscendenceFlag::eHighChargeEXP );
	GetSystem()->SetWindowLock(true);
}

void CDigimonTanscendenceViewer::PressTranscendBtn(void* pSender, void* pData)
{
	GetSystem()->SendDigimonTranscendence();
}

void CDigimonTanscendenceViewer::OnChangeSelectedDigimon( const cData_PostLoad::sDATA* pkDATA, int SelectedPart )
{
	switch( SelectedPart )
	{
	case CDigimonArchiveContents::ARCHIVE_DIGIMOM:
//	case CDigimonArchiveContents::PART_END:
		return;
	}

	SAFE_POINTER_RET( m_pkInsertDigimon );
	cGridListBoxItem* pkItem = const_cast<cGridListBoxItem*>(m_pkInsertDigimon->GetItemFormPos(USING));
	SAFE_POINTER_RET( pkItem );

	_RegistDigimonUpdate(0);
	GetSystem()->UnRegistAllTranscend();

	if( NULL == pkDATA || 0 == pkDATA->s_dwBaseDigimonID )
	{
		cString* pString = pkItem->GetItem();
		pString->Delete();
	}
	else
		UpdateSlotIcon(pkDATA->s_dwBaseDigimonID, pkItem);
}

void CDigimonTanscendenceViewer::_RegistDigimonUpdate( DWORD const& dwSelDigimonIDX )
{
	bool bRegisted = dwSelDigimonIDX == 0 ? false : true;	
	
	_SetSubMaterialItem( dwSelDigimonIDX );	
	_UpdateExpProgressbar(NULL);
}

void CDigimonTanscendenceViewer::_UpdateTranscendNeedMoney( const cData_PostLoad::sDATA* pkDATA  )
{
	SAFE_POINTER_RET( m_pSpendMoney );
	DWORD dwNeedMoney = 0;
	if( pkDATA && 0 != pkDATA->s_dwBaseDigimonID )
		dwNeedMoney = GetSystem()->GetTranscendOriginNeedMoney( pkDATA );
	m_pSpendMoney->SetMoney( (u8)dwNeedMoney );
}

void CDigimonTanscendenceViewer::_SetSubMaterialItem( DWORD const& dwSelDigimonIDX )
{
	CDigimonArchiveContents::sSubMaterialInfo sNomarlItem = GetSystem()->GetSubMaterial( dwSelDigimonIDX, 1 );
	_UpdateSubMaterial( 0, sNomarlItem.dwShowItemCode, sNomarlItem.nHaveCount, sNomarlItem.nNeedCount );
	CDigimonArchiveContents::sSubMaterialInfo sAdvanItem = GetSystem()->GetSubMaterial( dwSelDigimonIDX, 2 );
	_UpdateSubMaterial( 1, sAdvanItem.dwShowItemCode, sAdvanItem.nHaveCount, sAdvanItem.nNeedCount );	
}


void CDigimonTanscendenceViewer::OnMouseRClickMaterialRemove(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	SAFE_POINTER_RET(pItem);
	if(pItem->GetVisible()== false)
		return;

	int CurrSlotIdx = pItem->getID();
	std::map<int,sMaterialCtr>::iterator it = m_materialList.find( CurrSlotIdx );
	if( it != m_materialList.end() )
		GetSystem()->OnRClick_Archive_Digimon_In_TranscendMode(it->second.nArchiveSlot - 1000);
}

void CDigimonTanscendenceViewer::OnMouseMoveOnMaterialDigimon(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	SAFE_POINTER_RET(pItem);
	if(pItem->GetVisible()== false)
		return;

	int CurrSlotIdx = pItem->getID();
	std::map<int,sMaterialCtr>::iterator it = m_materialList.find( CurrSlotIdx );
	if( it == m_materialList.end() )
		return;

	if( !it->second.IsRegist() )
		return;

	GetSystem()->OnShowToolTip(CDigimonArchiveContents::ARCHIVE_DIGIMOM, it->second.nArchiveSlot );
}

void CDigimonTanscendenceViewer::OnMouseMoveOnSubMaterialItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	SAFE_POINTER_RET(pItem);
	if(pItem->GetVisible()== false)
		return;

 	int CurrSlotIdx = pItem->getID();
	std::list<sSubMaterialCtr>::const_iterator it = m_subMaterialList.begin();
	for( int n = 0; it != m_subMaterialList.end(); ++n, ++it )
	{
		if( n != CurrSlotIdx )
			continue;

		if( 0 == (*it).GetItemCode() )
			break;

		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos() + CsPoint( 20, 20 ), CsPoint::ZERO, TOOLTIP_MAX_SIZE, cTooltip::ITEM, (*it).GetItemCode(), cBaseWindow::WT_INVENTORY, 0, 0, NULL );
		break;
	}
}

void CDigimonTanscendenceViewer::_UpdateExpProgressbar( const cData_PostLoad::sDATA* pkDATA )
{
	SAFE_POINTER_RET( m_pProgressbar );
	SAFE_POINTER_RET( m_pOverProgressbar );

	DWORD dwCurrentExp= 0;
	DWORD dwMaxExp= 0;
	GetSystem()->GetCurrentDigimonMaxNExp( pkDATA, dwCurrentExp, dwMaxExp );

	if( !GetSystem()->IsEnableTranscendenceOverExp() )
	{
		if( dwCurrentExp >= dwMaxExp )
			dwMaxExp = dwCurrentExp;
	}

	std::wstring msg;
	if( 0 == dwMaxExp )
	{
		m_pProgressbar->SetProgressPos( 0.0 );
		m_pOverProgressbar->SetProgressPos( 0.0 );
	}
	else
	{
		float fCurrentPer = 0.0f;
		float fMaxPer = 100.0f;
		if( dwMaxExp >= dwCurrentExp )
		{
			fCurrentPer = ((float)dwCurrentExp / (float)dwMaxExp) * 100.0f;
			m_pProgressbar->SetProgressPos( fCurrentPer );		
			m_pOverProgressbar->SetProgressPos( 0.0f );
			m_pOverProgressbar->SetVisible( false );
			DmCS::StringFn::Format( msg, L"%.2f %% / %.2f %%", fCurrentPer, fMaxPer );
		}
		else
		{
			DWORD dwRemain = dwCurrentExp - dwMaxExp;
			fCurrentPer = ((float)dwRemain / (float)dwMaxExp) * 100.0f;
			m_pProgressbar->SetProgressPos( fMaxPer );
			m_pOverProgressbar->SetProgressPos( fCurrentPer );
			m_pOverProgressbar->SetVisible( true );
			DmCS::StringFn::Format( msg, L"%.2f %% / %.2f %%", fCurrentPer + fMaxPer, fMaxPer*2 );
		}
	}

	if( m_pExpGuageText )
		m_pExpGuageText->SetText( msg.c_str() );
}

void CDigimonTanscendenceViewer::_UpdateSubMaterial( int const& nIdx, DWORD const& dwItemIDX, int const& nHaveCount, int const& nNeedCount )
{
	std::list<sSubMaterialCtr>::iterator it = m_subMaterialList.begin();
	for( int n = 0; it != m_subMaterialList.end(); ++it, ++n )
	{
		if( n != nIdx )
			continue;
		(*it).SetIcon( dwItemIDX, nHaveCount, nNeedCount );
		break;
	}
}

void CDigimonTanscendenceViewer::_UpdateMaterialCount( int const& nMaterialCount )
{
	std::list<sSubMaterialCtr>::iterator it = m_subMaterialList.begin();
	for( int n = 0; it != m_subMaterialList.end(); ++it, ++n )
		(*it).UpdateMaterialCount( nMaterialCount );
}

int CDigimonTanscendenceViewer::_GetRegistMaterialDigimonCount()
{
	int nTotalCount = 0;
	std::map<int,sMaterialCtr>::const_iterator it = m_materialList.begin();
	for( ; it != m_materialList.end(); ++it )
	{
		if( it->second.IsRegist() )
			nTotalCount++;
	}	

	return nTotalCount;
}

void CDigimonTanscendenceViewer::_EffectEnd(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	cSpriteAni* pEffect = static_cast<cSpriteAni*>(pSender);
	sEffectType* pChargeType = dynamic_cast<sEffectType*>(pEffect->GetUserData());
	SAFE_POINTER_RET( pChargeType );

	if( !GetSystem()->SendExpCharge( pChargeType->nChargeType ) )
	{
		GetSystem()->SetWindowLock(false);
		return;
	}

	pEffect->SetPlayType( cSpriteAni::LOOP );
	pEffect->PlayOn(true);
}

void CDigimonTanscendenceViewer::_ResultExpCharge( bool const& bSuccess, bool const& bOverExp )
{
	if( !bSuccess )
	{
		if( m_pEffect )
			m_pEffect->PlayOff();
		return;
	}

	if( m_pResultEffect )
		m_pResultEffect->PlayOn(true);

	if( bOverExp )
	{
		if( m_pExcellentEffect )
			m_pExcellentEffect->PlayOn(true);
		cWindow::PlaySound( "System\\Storage_Excellent.wav" );
	}
	else
	{
		if( m_pSuccessEffect )
			m_pSuccessEffect->PlayOn(true);
		cWindow::PlaySound( "System\\Storage_success.wav" );
	}
}

//////////////////////////////////////////////////////////////////////////

bool CDigimonTanscendenceViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::Recv_TranscendChargeExpResult, this );

	return true;
}

void CDigimonTanscendenceViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::Recv_TranscendChargeExpResult:
		{
			_SetChargeExpEnd();

			bool bSuccess = true;
			bool bOverExp = false;
			kStream >> bSuccess >> bOverExp;
			_ResultExpCharge( bSuccess, bOverExp );			
		}
		break;
	}
}

void CDigimonTanscendenceViewer::_OnClickHelpButton(void* pSender, void* pData)
{
	GetSystem()->OnClickTranscendenceHelp();
}
