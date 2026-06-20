#include "stdafx.h"
#include "TamerStatusUI_Tamer.h"


cTamerStatusUI_Tamer::cTamerStatusUI_Tamer()
: m_pLEquipList(NULL),
m_pREquipList(NULL),
m_pTamerAbilList(NULL),
m_pApplyAbilList(NULL),
#ifdef WEB_INFOMATION
	m_pWebInfoButton(NULL),
#endif
m_pLevelText(NULL),
m_pNameText(NULL),
m_pGuildText(NULL),
m_pTamerAbilText(NULL),
m_pApplyAbilText(NULL),
m_pFriendShipText(NULL),
m_pTamerRenderTex(NULL)
{
	Construct();
}

cTamerStatusUI_Tamer::~cTamerStatusUI_Tamer()
{
	if( m_pTamerRenderTex )
		SAFE_NIDELETE( m_pTamerRenderTex );

	Destroy();
}

bool cTamerStatusUI_Tamer::Construct(void)
{
	if( !cTamerStatusUI_SubViewer::Construct() )
		return false;
	return true;
}

void cTamerStatusUI_Tamer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

void cTamerStatusUI_Tamer::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	DeleteResource();
}

void cTamerStatusUI_Tamer::DeleteResource()
{
	DeleteScript();
}

void cTamerStatusUI_Tamer::Create(cWindow* pkRoot, int nValue /*= 0 */)
{
	InitScript( pkRoot, NULL, CsPoint::ZERO, CsPoint( 406, 676 ), true );

	CsPoint ptPos = CsPoint( 34, 143 );
	CsPoint ptSize = CsPoint( 32, 310 );
	CsPoint ptGab = CsPoint( 0, 14 );
	CsPoint ptItemSize = CsPoint( 32, 32 );

	m_pLEquipList = NiNew cGridListBox;
	if( m_pLEquipList )
	{
		m_pLEquipList->Init( GetRoot(), ptPos, ptSize, ptGab, ptItemSize, cGridListBox::LowLeftDown, cGridListBox::LeftTop, NULL, false, 1 );
		m_pLEquipList->SetUsePressedMoveEvent( true );
		m_pLEquipList->SetMouseOverImg( "Icon\\Mask_Over.tga" );
		m_pLEquipList->SetBackOverAndSelectedImgRender( false );
		m_pLEquipList->SetAutoSelection( false );
		m_pLEquipList->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT,	this, &cTamerStatusUI_Tamer::_OnRClickEquipItem );
		m_pLEquipList->AddEvent( cGridListBox::GRID_MOUSE_DOWN_ON_ITEM,		this, &cTamerStatusUI_Tamer::_OnLClickDownEquipItem );
		AddChildControl( m_pLEquipList );
	}
	m_pREquipList = NiNew cGridListBox;
	if( m_pREquipList )
	{
		ptPos.x = 340;
		m_pREquipList->Init( GetRoot(), ptPos, ptSize, ptGab, ptItemSize, cGridListBox::LowLeftDown, cGridListBox::LeftTop, NULL, false, 1 );
		m_pREquipList->SetUsePressedMoveEvent( true );
		m_pREquipList->SetMouseOverImg( "Icon\\Mask_Over.tga" );
		m_pREquipList->SetBackOverAndSelectedImgRender( false );
		m_pREquipList->SetAutoSelection( false );
		m_pREquipList->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT,	this, &cTamerStatusUI_Tamer::_OnRClickEquipItem );
		m_pREquipList->AddEvent( cGridListBox::GRID_MOUSE_DOWN_ON_ITEM,		this, &cTamerStatusUI_Tamer::_OnLClickDownEquipItem );
		AddChildControl( m_pREquipList );
	}
	_MakeEquipGrid();

#ifdef WEB_INFOMATION
	m_pWebInfoButton = AddButton( CsPoint( 342, 84 ), CsPoint( 27, 27 ), CsPoint( 0, 27 ), "TamerStatus_New\\TamerStatus_Info.tga" );
	m_pWebInfoButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cTamerStatusUI_Tamer::_OnClickWebInfo );
#endif

	m_pTamerRenderTex = NiNew cRenderTex;
	if( m_pTamerRenderTex )
		m_pTamerRenderTex->Init( CsPoint( 386, 363 ), "TamerStatus_New\\TamerStatus_Win_01_bg.tga" );

	// 카메라 위치 조정
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );

	m_pTamerAbilList = NiNew cListBox;
	if( m_pTamerAbilList )
	{
		m_pTamerAbilList->Init( GetRoot(), CsPoint( 11, 553 ), CsPoint( 140, 108 ), false );
		m_pTamerAbilList->SetMargin( CsPoint( 70, 8 ) );
		m_pTamerAbilList->SetItemGab( CsPoint( 0, 12 ) );
		AddChildControl( m_pTamerAbilList );
	}
	m_pApplyAbilList = NiNew cListBox;
	if( m_pApplyAbilList )
	{
		m_pApplyAbilList->Init( GetRoot(), CsPoint( 255, 553 ), CsPoint( 140, 108 ), false );
		m_pApplyAbilList->SetMargin( CsPoint( 70, 8 ) );
		m_pApplyAbilList->SetItemGab( CsPoint( 0, 12 ) );
		AddChildControl( m_pApplyAbilList );
	}
	_MakeAbilList();

	{	// 테이머 레벨
		std::wstring wsLevel;
		int nLevel = GetSystem()->GetTamerStat( SystemType::eAbilType::eLV );
		DmCS::StringFn::Format( wsLevel, L"%s %d", L"Lv.", nLevel );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor( 76.f/255.f, 242.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( wsLevel.c_str() );
		m_pLevelText = AddText( &ti, CsPoint( 30, 88 ) );
	}

	{	// 테이머명
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor( 76.f/255.f, 242.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( pUser->GetName() );
		m_pNameText = AddText( &ti, CsPoint( 203, 88 ) );
	}

	{	// 길드명
		cCommunityContent::sGUILD_INFO guildInfo;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDINFO, &guildInfo );

		std::wstring wsGuild;

		if( guildInfo.s_szGuildName.size() > 0 )
			DmCS::StringFn::Format( wsGuild, L"<%s>", guildInfo.s_szGuildName.c_str() );
		else
			DmCS::StringFn::Format( wsGuild, L"<%s>", UISTRING_TEXT( "TAMERSTATUS_NONEGUILD_TEXT" ).c_str() );

		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_11, NiColor( 247.f/255.f, 83.f/255.f, 0 ) );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( wsGuild.c_str() );
		m_pGuildText = AddText( &ti, CsPoint( 203, 491 ) );
	}

	{	// 테이머 능력치
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor( 1, 174.f/255.f, 0 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "TAMERSTATUS_TAMERABIL_TEXT" ).c_str() );
		m_pTamerAbilText = AddText( &ti, CsPoint( 19, 534 ) );
	}

	{	// 적용 능력치
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor( 122.f/255.f, 254.f/255.f, 0 ) );
		ti.s_eTextAlign = DT_RIGHT;
		ti.SetText( UISTRING_TEXT( "TAMERSTATUS_TAMERAPPLY_TEXT" ).c_str() );
		m_pApplyAbilText = AddText( &ti, CsPoint( 388, 534 ) );
	}

	{	// 친화력
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor( 122.f/255.f, 254.f/255.f, 0 ) );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "TAMERSTATUS_FRIENDSHIP_TEXT" ).c_str() );
		m_pFriendShipText = AddText( &ti, CsPoint( 203, 583 ) );
	}

	m_rcFriendShip.SetRect( CsPoint( 156, 516 ), CsSIZE( 90, 145 ) );
}

void cTamerStatusUI_Tamer::Update(float const& fDeltaTime)
{
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );
	if( !pUser->GetViewSight() && pUser->IsLoad() && m_pTamerRenderTex )
	{
		// 오브젝트 위치 설정	
		CsNodeObj* pCsNode = pUser->GetCsNode();
		if( pCsNode != NULL )
		{
			// 시야에 보이지 않으면 애니메이션 업데이트 풀로 다해준다.
			float fAniTime = pUser->GetAniTime();
			CsC_AniProp* pAniProp = pUser->GetProp_Animation();
			SAFE_POINTER_RET( pAniProp );
			NiActorManager* pActorMng = pAniProp->GetActorMng();
			SAFE_POINTER_RET( pActorMng );
			pActorMng->Update( fAniTime );
			pCsNode->m_pNiNode->Update( fAniTime );
		}
	}
}

BOOL cTamerStatusUI_Tamer::Update_ForMouse()
{
	if( m_pLEquipList && m_pLEquipList->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return _SetEquipTooltip( m_pLEquipList->GetMouseOverItem() );

	if( m_pREquipList && m_pREquipList->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return _SetEquipTooltip( m_pREquipList->GetMouseOverItem() );

#ifdef WEB_INFOMATION
	if( m_pWebInfoButton && m_pWebInfoButton->Update_ForMouse() )
		return TRUE;
#endif

	// 툴팁 필요
	if( m_pTamerAbilList && m_pTamerAbilList->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return _SetStatTooltip( m_pTamerAbilList->GetMouseOverItem() );

	CsPoint ptLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	if( m_rcFriendShip.PtInRect( ptLocalPos ) )
	{
		SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR, FALSE );
		cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL( pTooltip, FALSE );
		pTooltip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 10, 0 ), 0, cTooltip::FRIENDSHIP_POINT, 1, 0, 0, 0, 0);
		return TRUE;
	}

	return FALSE;
}

void cTamerStatusUI_Tamer::RenderScript()
{
	if( IsVisible() )
	{
		if( m_pTamerRenderTex )
		{
			SAFE_POINTER_RET( g_pCharMng );
			CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
			SAFE_POINTER_RET( pTamerUser );

			m_pTamerRenderTex->BeginRender();
			m_pTamerRenderTex->RenderObject( GET_SUBCAMERA(CAMERA_02), pTamerUser, true );
			m_pTamerRenderTex->EndRender( GET_SUBCAMERA(CAMERA_02), GetTotalPosition() + CsPoint( 10, 115 ) );
		}

		if( g_pIconMng )
		{
			g_pIconMng->RenderNumber( DT_CENTER, NUMBERSPRITE::RED, GetSystem()->GetFriendShip(), 
				GetTotalPosition() + CsPoint( 200, 607 ), CsPoint( 32, 34 ), 1.2f, false );
		}
	}

	cScriptUI::RenderScript();
}

void cTamerStatusUI_Tamer::ResetDevice()
{
	ResetDeviceScript();
	if( m_pTamerRenderTex )
		m_pTamerRenderTex->ResetDevice();
}

void cTamerStatusUI_Tamer::ReleaseSelect()
{

}

void cTamerStatusUI_Tamer::UpdateViewer()
{
	for( int i = 0; i <= nLimit::Equip; ++i )
	{
		int nItemType = GetSystem()->GetEquipItemType( i );
		CsCoolTimeSeq* pCoolTime = GetSystem()->GetFTItemCoolTime( nItemType );
		std::map< int, sEquipList >::iterator it = m_mapEquip.find( i );
		if( it != m_mapEquip.end() )
			it->second.ChangeItem( nItemType, pCoolTime );
	}

	_UpdateAbilList();
	_UpdateTamerInfo();
}

void cTamerStatusUI_Tamer::OnLButtonUp(CsPoint pos)
{
	if( m_pLEquipList )
	{
		cGridListBoxItem const* pOverItem = m_pLEquipList->GetMouseOverItem();
		if( _OnClickEquipItem( pOverItem ) )
			return;
	}

	if( m_pREquipList )
	{
		cGridListBoxItem const* pOverItem = m_pREquipList->GetMouseOverItem();
		if( _OnClickEquipItem( pOverItem ) )
			return;
	}

	CURSOR_ST.ReleaseIcon();
}

bool cTamerStatusUI_Tamer::CursorIconLBtnUp(CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount)
{
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		GetSystem()->WearEquipItem( nIconSlot );
		return true;
	case CURSOR_ICON::CI_TEQUIP:
	case CURSOR_ICON::CI_CHIPSET:
	case CURSOR_ICON::CI_TSKILL:
		break;
	case CURSOR_ICON::CI_WAREHOUSE:
		cPrintMsg::PrintMsg( 11018 );
		break;
	default:
		cPrintMsg::PrintMsg( 11014 );
		break;
	}
	CURSOR_ST.ReleaseIcon();
	return false;
}

BOOL cTamerStatusUI_Tamer::_SetEquipTooltip(cGridListBoxItem const * pOverItem)
{
	SAFE_POINTER_RETVAL( pOverItem, FALSE );

	sEquipInfo* pEquipInfo = dynamic_cast< sEquipInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RETVAL( pEquipInfo, FALSE );

	if( 0 == pEquipInfo->nItemType )
		return FALSE;

	cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RETVAL( pToolTip, FALSE );

	pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pEquipInfo->nItemType, 
		cBaseWindow::WT_NEW_TAMERSTATUS, 0, 0, GetSystem()->GetEquipItem( pEquipInfo->nPartsType ) );
	
	return TRUE;
}

BOOL cTamerStatusUI_Tamer::_SetStatTooltip(cListBoxItem const* pOverItem)
{
	SAFE_POINTER_RETVAL( pOverItem, FALSE );

	sAbilInfo* pAbilInfo = dynamic_cast< sAbilInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RETVAL( pAbilInfo, FALSE );

	int nAbilType = _ChangeToSystemAbilType( pAbilInfo->eAbilType );
	if( -1 == nAbilType )
		return FALSE;

	int nType = 0, nBaseStat = 0;
	GetSystem()->GetBaseStat( nType, nBaseStat, static_cast<SystemType::eAbilType>( nAbilType ) );

	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RETVAL( pTooltip, FALSE );

	pTooltip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 20, 0 ), 190, cTooltip::STAT, nType, nBaseStat, pAbilInfo->nAbilPoint - nBaseStat, 0, g_pCharMng->GetTamerUser() );

	return TRUE;
}

void cTamerStatusUI_Tamer::_MakeEquipGrid()
{
	if( m_pLEquipList )
	{
		int nIndex = 0;
		m_pLEquipList->RemoveAllItem();
		_AddEquipGridItem( m_pLEquipList, nIndex++, nTamer::Head );
		_AddEquipGridItem( m_pLEquipList, nIndex++, nTamer::Glass );
		_AddEquipGridItem( m_pLEquipList, nIndex++, nTamer::Coat );
		_AddEquipGridItem( m_pLEquipList, nIndex++, nTamer::Pants );
		_AddEquipGridItem( m_pLEquipList, nIndex++, nTamer::Glove );
		_AddEquipGridItem( m_pLEquipList, nIndex++, nTamer::Shoes );
		_AddEquipGridItem( m_pLEquipList, nIndex++, nTamer::Costume );
	}

	if( m_pREquipList )
	{
		int nIndex = 0;
		m_pREquipList->RemoveAllItem();
		_AddEquipGridItem( m_pREquipList, nIndex++, nTamer::Earring );
		_AddEquipGridItem( m_pREquipList, nIndex++, nTamer::Necklace );
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
		_AddEquipGridItem( m_pREquipList, nIndex++, nTamer::Bracelet );
#endif
		_AddEquipGridItem( m_pREquipList, nIndex++, nTamer::Ring );
		_AddEquipGridItem( m_pREquipList, nIndex++, nTamer::EquipAura );
		_AddEquipGridItem( m_pREquipList, nIndex++, nTamer::MaxParts );	// 디지바이스
#ifdef SDM_TAMER_XGUAGE_20180628
		_AddEquipGridItem( m_pREquipList, nIndex++, nTamer::XAI );
#endif
	}
}

void cTamerStatusUI_Tamer::_AddEquipGridItem(cGridListBox* pGrid, int nGridIndex, int nEquipType)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	uint nItemType = GetSystem()->GetEquipItemType( nEquipType );

	sEquipList sEquip;
	sEquip.pItemIcon = pControl->AddIcon( CsPoint( 32, 32 ), ICONITEM::ITEM_ICON, nItemType, 1 );
	if( sEquip.pItemIcon )
	{
		sEquip.pItemIcon->SetAutoPointerDelete( true );
		if( 0 == nItemType )
			sEquip.pItemIcon->SetEnable( false );
		else
		{
			CsCoolTimeSeq* pCoolTime = GetSystem()->GetFTItemCoolTime( nItemType );
			sEquip.pItemIcon->SetCoolTimeMode( true );
			sEquip.pItemIcon->SetCoolTimeSeq( pCoolTime );
		}
	}

	sEquip.pGridItem = NiNew cGridListBoxItem( nGridIndex, CsPoint( 32, 32 ) );
	sEquip.pGridItem->SetUserData( new sEquipInfo( nEquipType, nItemType ) );
	if( sEquip.pGridItem )
	{
		sEquip.pGridItem->SetItem( pControl );
		if( pGrid )
			pGrid->AddItem( sEquip.pGridItem );
	}

	m_mapEquip.insert( std::make_pair( nEquipType, sEquip ) );
}

void cTamerStatusUI_Tamer::_MakeAbilList()
{
	SAFE_POINTER_RET( m_pTamerAbilList );
	m_pTamerAbilList->RemoveAllItem();

	int nAT = GetSystem()->GetTamerStat( SystemType::eAbilType::eAT );
	int nDE = GetSystem()->GetTamerStat( SystemType::eAbilType::eDE );
	int nHP = GetSystem()->GetTamerStat( SystemType::eAbilType::eHP );
	int nDS = GetSystem()->GetTamerStat( SystemType::eAbilType::eDS );
	int nFS = GetSystem()->GetFriendShip();

	_AddTamerAbilListItem( m_pTamerAbilList, eAT, nAT );
	_AddTamerAbilListItem( m_pTamerAbilList, eDE, nDE );
	_AddTamerAbilListItem( m_pTamerAbilList, eHP, nHP );
	_AddTamerAbilListItem( m_pTamerAbilList, eDS, nDS );

	SAFE_POINTER_RET( m_pApplyAbilList );
	m_pApplyAbilList->RemoveAllItem();

	_AddApplyAbilListItem( m_pApplyAbilList, eAT, (nAT * nFS) * 0.01f );
	_AddApplyAbilListItem( m_pApplyAbilList, eDE, (nDE * nFS) * 0.01f );
	_AddApplyAbilListItem( m_pApplyAbilList, eHP, (nHP * nFS) * 0.01f );
	_AddApplyAbilListItem( m_pApplyAbilList, eDS, (nDS * nFS) * 0.01f );
}

void cTamerStatusUI_Tamer::_AddTamerAbilListItem(cListBox* pList, eAbilityType eAbilType, int nAbilPoint)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	sAbility sAbil;

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_11, NiColor( 202.f/255.f, 158.f/255.f, 3.f/255.f ) );
	ti.s_eTextAlign = DT_RIGHT;
	ti.SetText( nAbilPoint );
	sAbil.pAbilText = pControl->AddText( &ti, CsPoint( 60, 0 ) );
	if( sAbil.pAbilText )
		sAbil.pAbilText->SetAutoPointerDelete( true );

	sAbil.pAbilListItem = NiNew cListBoxItem();
	if( sAbil.pAbilListItem )
	{
		sAbil.pAbilListItem->SetItem( pControl );
		sAbil.pAbilListItem->SetUserData( new sAbilInfo( eAbilType, nAbilPoint ) );
		if( pList )
			pList->AddItem( sAbil.pAbilListItem );
	}

	m_mapTamerAbil.insert( std::make_pair( eAbilType, sAbil ) );
}

void cTamerStatusUI_Tamer::_AddApplyAbilListItem(cListBox* pList, eAbilityType eAbilType, int nAbilPoint)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	sAbility sAbil;

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_11, NiColor( 122.f/255.f, 254.f/255.f, 0 ) );
	ti.s_eTextAlign = DT_RIGHT;
	ti.SetText( nAbilPoint );
	sAbil.pAbilText = pControl->AddText( &ti, CsPoint( 60, 0 ) );
	if( sAbil.pAbilText )
		sAbil.pAbilText->SetAutoPointerDelete( true );

	sAbil.pAbilListItem = NiNew cListBoxItem();
	if( sAbil.pAbilListItem )
	{
		sAbil.pAbilListItem->SetItem( pControl );
		sAbil.pAbilListItem->SetUserData( new sAbilInfo( eAbilType, nAbilPoint ) );
		if( pList )
			pList->AddItem( sAbil.pAbilListItem );
	}

	m_mapApplyAbil.insert( std::make_pair( eAbilType, sAbil ) );
}

void cTamerStatusUI_Tamer::_UpdateAbilList()
{
	int nAT = GetSystem()->GetTamerStat( SystemType::eAbilType::eAT );
	int nDE = GetSystem()->GetTamerStat( SystemType::eAbilType::eDE );
	int nHP = GetSystem()->GetTamerStat( SystemType::eAbilType::eHP );
	int nDS = GetSystem()->GetTamerStat( SystemType::eAbilType::eDS );
	int nFS = GetSystem()->GetFriendShip();

	_ChangeAbilPoint( eAT, nAT, nFS );
	_ChangeAbilPoint( eDE, nDE, nFS );
	_ChangeAbilPoint( eHP, nHP, nFS );
	_ChangeAbilPoint( eDS, nDS, nFS );
}

void cTamerStatusUI_Tamer::_ChangeAbilPoint(eAbilityType eAbilType, int nPoint, int nFSPoint)
{
	std::map< int, sAbility >::iterator it = m_mapTamerAbil.find( eAbilType );
	if( it != m_mapTamerAbil.end() )
		it->second.SetText( nPoint );

	it = m_mapApplyAbil.find( eAbilType );
	if( it != m_mapApplyAbil.end() )
		it->second.SetText( (nPoint * nFSPoint) * 0.01f );
}

int cTamerStatusUI_Tamer::_ChangeToSystemAbilType(eAbilityType eAbilType)
{
	switch( eAbilType )
	{
	case eAT:
		return SystemType::eAT;
	case eDE:
		return SystemType::eDE;
	case eHP:
		return SystemType::eHP;
	case eDS:
		return SystemType::eDS;
	}
	return -1;
}

void cTamerStatusUI_Tamer::_UpdateTamerInfo()
{
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );

	if( m_pNameText )
		m_pNameText->SetText( pUser->GetName() );

	if( m_pLevelText )
	{
		std::wstring wsLevel;
		int nLevel = GetSystem()->GetTamerStat( SystemType::eAbilType::eLV );
		DmCS::StringFn::Format( wsLevel, L"%s %d", L"Lv.", nLevel );
		m_pLevelText->SetText( wsLevel.c_str() );
	}
}

#ifdef WEB_INFOMATION
void cTamerStatusUI_Tamer::_OnClickWebInfo(void* pSender, void* pData)
{
	SAFE_POINTER_RET( g_pGameIF );

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_WEBWIN ) )
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_WEBWIN );
	}												
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_WEBWIN, 0, cWebWin::eRANK );		
	cWebWin* pWeb = g_pGameIF->GetWebWin();
	if( pWeb )
	{
		SAFE_POINTER_RET( g_pCharMng );
		CTamerUser* pUser = g_pCharMng->GetTamerUser();
		SAFE_POINTER_RET( pUser );
		pWeb->SetWebControl( cWebWin::eRANK, CsPoint( 50, 200 ), CsPoint( 416, 550 ), pUser->GetName() );
	}
}
#endif

void cTamerStatusUI_Tamer::_OnLClickDownEquipItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pOverItem = static_cast< cGridListBoxItem* >( pData );

	if( _OnClickEquipItem( pOverItem ) )
		return;

	CURSOR_ST.ReleaseIcon();
}

void cTamerStatusUI_Tamer::_OnRClickEquipItem(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pEquip = static_cast< cGridListBoxItem* >( pData );
	sEquipInfo* pEquipInfo = dynamic_cast< sEquipInfo* >( pEquip->GetUserData() );
	SAFE_POINTER_RET( pEquipInfo );

	pEquipInfo->nItemType = 0;
	GetSystem()->MoveEquipItem( pEquipInfo->nPartsType );
}

bool cTamerStatusUI_Tamer::_OnClickEquipItem(cGridListBoxItem const* pOverItem)
{
	bool bSetItem = false;
	if( !pOverItem )
	{
		CURSOR_ST.ReleaseIcon();
		return bSetItem;
	}
	
	sEquipInfo* pEquipInfo = dynamic_cast< sEquipInfo* >( pOverItem->GetUserData() );
	if( pEquipInfo )
	{
		if( nTamer::MaxParts == pEquipInfo->nPartsType )
			CURSOR_ST.SetIcon( CURSOR_ICON::CI_TEQUIP, SERVER_DATA_DIGIVICE_CONSTANT, 1, NULL );
		else
			CURSOR_ST.SetIcon( CURSOR_ICON::CI_TEQUIP, pEquipInfo->nPartsType, 1, NULL );
		bSetItem = true;
	}

	if( !bSetItem )
		CURSOR_ST.ReleaseIcon();

	return bSetItem;
}
///////////////////////////////////////////////////////////////////////////////////////////////
void cTamerStatusUI_Tamer::sEquipList::ChangeItem(uint nItemType, CsCoolTimeSeq* pCoolTime)
{
	SAFE_POINTER_RET( pGridItem );
	SAFE_POINTER_RET( pItemIcon );

	pItemIcon->ChangeIcon( ICONITEM::ITEM_ICON, nItemType, 1 );

	sEquipInfo* pEquipInfo = dynamic_cast< sEquipInfo* >( pGridItem->GetUserData() );
	SAFE_POINTER_RET( pEquipInfo );
	pEquipInfo->nItemType = nItemType;

	if( 0 == nItemType )
	{
		pGridItem->SetEnable( false );
		pItemIcon->SetVisible( false );
	}
	else
	{
		pGridItem->SetEnable( true );
		pItemIcon->SetVisible( true );
	}

	pItemIcon->SetCoolTimeSeq( pCoolTime );
}

void cTamerStatusUI_Tamer::sAbility::SetText(int nAbilPoint)
{
	sAbilInfo* pAbilInfo = dynamic_cast< sAbilInfo* >( pAbilListItem->GetUserData() );
	if( pAbilInfo )
	{
		pAbilInfo->nAbilPoint = nAbilPoint;
		std::wstring wsPoint;
		DmCS::StringFn::From( wsPoint, nAbilPoint );
		pAbilText->SetText( wsPoint.c_str() );
	}
}

