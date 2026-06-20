
#include "stdafx.h"
#include "TamerStatus.h"

#define IF_TAMER_EQUIP_ICON_SIZE CsPoint( 32, 32 )
#define ICON_HALF		16

namespace
{
	std::wstring BuildSafeOwnedText( TCHAR const* pText )
	{
		cText::sTEXTINFO textInfo;
		textInfo.SetText( pText );
		return textInfo.s_strText;
	}

	void SetSafeOwnedText( cText::sTEXTINFO& textInfo, TCHAR const* pText )
	{
		std::wstring ownedText = BuildSafeOwnedText( pText );
		textInfo.SetText( ownedText.c_str() );
	}

	void SetSafeOwnedText( cText::sTEXTINFO& textInfo, std::wstring const& text )
	{
		SetSafeOwnedText( textInfo, text.c_str() );
	}
}

void cTamerStatus::_DeleteEquip()
{
	SAFE_NIDELETE( m_pEquipBG );
	SAFE_NIDELETE( m_pRenderTex );

	for( int i=0; i<IF_TAMER_STAT_COUNT; ++i )
		SAFE_NIDELETE( m_pStatText[ i ] );

	SAFE_NIDELETE( m_pNameText );
	SAFE_NIDELETE( m_pGuildText );

#ifdef WEB_INFOMATION
	SAFE_NIDELETE( m_pRank );
#endif

#ifdef UI_REMOVE_RENEWAL_20190109
#else
	for( int i = 0; i < 6; i++ )
	{
		SAFE_NIDELETE( m_pEquipText[ i ] );
	}
#endif
}

void cTamerStatus::_CreateEquip()
{
	float fStartPosY = 45.0f;
	m_IFIcon_Equip[ nTamer::Head ].Init( CsRect( CsPoint( 9, 45 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );
	m_IFIcon_Equip[ nTamer::Coat ].Init( CsRect( CsPoint( 9, 87 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );
	m_IFIcon_Equip[ nTamer::Pants ].Init( CsRect( CsPoint( 9, 129 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );
	m_IFIcon_Equip[ nTamer::Glove ].Init( CsRect( CsPoint( 9, 172 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );
	m_IFIcon_Equip[ nTamer::Shoes ].Init( CsRect( CsPoint( 9, 214 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );
	m_IFIcon_Equip[ nTamer::Costume ].Init( CsRect( CsPoint( 9, 256 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );

	m_IFIcon_Equip[ nTamer::Ring ].Init( CsRect( CsPoint( 215, 45 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );
 	m_IFIcon_Equip[ nTamer::Necklace ].Init( CsRect( CsPoint( 215, 87 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );	
	m_IFIcon_Equip[ nTamer::Earring ].Init( CsRect( CsPoint( 215, 129 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );
	m_IFIcon_Equip[ nTamer::Glass ].Init( CsRect( CsPoint( 215, 172 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );

	m_IFIcon_Equip[ nTamer::EquipAura ].Init( CsRect( CsPoint( 215, 214 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );//캐릭터 장비창 오라 추가 chu8820

	m_IFIcon_Equip_Digivice.Init( CsRect( CsPoint( 215, 256 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );

#ifdef SDM_TAMER_XGUAGE_20180628
	m_IFIcon_Equip[ nTamer::XAI ].Init( CsRect( CsPoint( 215, 299 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );
#endif

#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
	m_IFIcon_Equip[ nTamer::Bracelet ].Init( CsRect( CsPoint( 9, 299 ), (CsSIZE)IF_TAMER_EQUIP_ICON_SIZE ) );
#endif
}

void cTamerStatus::_InitEquip()
{
	int AdjustYPos = 0;
#ifdef SDM_TAMER_XGUAGE_20180628
	AdjustYPos = 22;
#endif
	_InitText_Equip();

	m_pEquipBG = NiNew cSprite;
	m_pEquipBG->Init( m_pRoot, CsPoint::ZERO, CsPoint( 255, 424 + AdjustYPos ), "TamerStatus\\Tamer_Win.tga", false );

	m_pRenderTex = NiNew cRenderTex;
	if( m_pRenderTex->Init( CsPoint( 158, 248 ), "TamerStatus\\TamerBack.tga" ) == false )
		SAFE_NIDELETE( m_pRenderTex );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.s_eTextAlign = DT_RIGHT;
	for( int i=0; i<IF_TAMER_STAT_COUNT; ++i )
	{
		m_pStatText[ i ] = NiNew cText;
		m_pStatText[ i ]->Init( m_pRoot, CsPoint( 132, 341 + 17*i+ AdjustYPos), &ti, false );
		m_rcStatRegion[ i ].SetRect( CsPoint( 12, 340+17*i+ AdjustYPos), CsSIZE( 124, 17 ) );
	}

	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTamerUser );
	CCharOption* pCharOption = pTamerUser->GetCharOption();
	SAFE_POINTER_RET( pCharOption );

	ti.s_eTextAlign = DT_CENTER;
	SetSafeOwnedText( ti, pCharOption->GetName() );
	m_pNameText = NiNew cText;
	m_pNameText->Init( m_pRoot, CsPoint( 127, 45 ), &ti, false );

	cCommunityContent::sGUILD_INFO guildInfo;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDINFO, &guildInfo );
	if( guildInfo.s_szGuildName.size() > 0 )
	{
		TCHAR szGuildName[ Language::pLength::GuildName + 3 ];
		_stprintf_s( szGuildName, Language::pLength::GuildName + 3, _T( "<%s>" ), pCharOption->GetGuildName() );
		ti.s_eTextAlign = DT_CENTER;
		ti.s_eFontSize = CFont::FS_9;
		SetSafeOwnedText( ti, szGuildName );
		m_pGuildText = NiNew cText;
		m_pGuildText->Init( m_pRoot, CsPoint( 127, 296 ), &ti, false );
	}
	else
	{
		ti.s_eTextAlign = DT_CENTER;
		ti.s_eFontSize = CFont::FS_9;
		std::wstring wsText;
		DmCS::StringFn::Format( wsText, L"<%s>", UISTRING_TEXT( "TAMERSTATUS_NONEGUILD_TEXT" ).c_str() );
		SetSafeOwnedText( ti, wsText );
		m_pGuildText = NiNew cText;
		m_pGuildText->Init( m_pRoot, CsPoint( 127, 296 ), &ti, false );
	}

#ifdef WEB_INFOMATION
	m_pRank = NiNew cButton;
	m_pRank->Init( m_pRoot, CsPoint( 172, 258 ), CsPoint( 27, 27 ), "Minimap\\ToggleMapBtn.tga", false );
	m_pRank->SetTexToken( CsPoint( 0, 33 ) );
#endif
	_Update_Equip();
}

void cTamerStatus::_InitText_Equip()
{

	int AdjustYPos = 0;
#ifdef SDM_TAMER_XGUAGE_20180628
	AdjustYPos = 22;
#endif

#ifdef UI_REMOVE_RENEWAL_20190109
#else
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.s_eTextAlign = DT_CENTER;

	ti.SetText( UISTRING_TEXT( "TAMERSTATUS_EQUIP_TAMERABILITY" ).c_str() );
	m_pEquipText[ 0 ] = NiNew cText;
	m_pEquipText[ 0 ]->Init( m_pRoot, CsPoint( 47, 320 + AdjustYPos ), &ti, false );	

	ti.s_eTextAlign = DT_LEFT;

	ti.SetText( UISTRING_TEXT( "TAMERSTATUS_EQUIP_AT" ).c_str() );
	m_pEquipText[ 1 ] = NiNew cText;
	m_pEquipText[ 1 ]->Init( m_pRoot, CsPoint( 17, 341  + AdjustYPos), &ti, false );

	ti.SetText( UISTRING_TEXT( "TAMERSTATUS_EQUIP_DE" ).c_str() );
	m_pEquipText[ 2 ] = NiNew cText;
	m_pEquipText[ 2 ]->Init( m_pRoot, CsPoint( 17, 358  + AdjustYPos), &ti, false );

	ti.SetText( UISTRING_TEXT( "TAMERSTATUS_EQUIP_HP" ).c_str() );
	m_pEquipText[ 3 ] = NiNew cText;
	m_pEquipText[ 3 ]->Init( m_pRoot, CsPoint( 17, 375  + AdjustYPos), &ti, false );

	ti.SetText( UISTRING_TEXT( "TAMERSTATUS_EQUIP_DS" ).c_str() );
	m_pEquipText[ 4 ] = NiNew cText;
	m_pEquipText[ 4 ]->Init( m_pRoot, CsPoint( 17, 392  + AdjustYPos), &ti, false );

	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( UISTRING_TEXT( "TAMERSTATUS_FRIENDSHIP_TEXT" ).c_str() );
	m_pEquipText[ 5 ] = NiNew cText;
	m_pEquipText[ 5 ]->Init( m_pRoot, CsPoint( 202, 348  + AdjustYPos), &ti, false );
#endif
	m_FriendshipPoint.SetRect(CsPoint(174, 376 + AdjustYPos), CsSIZE(62, 32));
}

void cTamerStatus::_Update_Equip()
{
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTamerUser );
	CTamerUser::sUSER_STAT* pUserStat = (CTamerUser::sUSER_STAT*)pTamerUser->GetBaseStat();
	SAFE_POINTER_RET( pUserStat );
	FMTamer::sTINFO TInfo;
	TInfo.s_nTamerID = pTamerUser->GetOrgModelID();
	TInfo.s_nLevel = pTamerUser->GetBaseStat()->GetLevel();


	if( (pTamerUser->GetViewSight() == false) && m_pRenderTex && pTamerUser->IsLoad() )
	{
		// 오브젝트 위치 설정	
		CsNodeObj* pCsNode = pTamerUser->GetCsNode();
		if( pCsNode != NULL )
		{
			// 시야에 보이지 않으면 애니메이션 업데이트 풀로 다해준다.
			float fAniTime = pTamerUser->GetAniTime();
			pTamerUser->GetProp_Animation()->GetActorMng()->Update( fAniTime );
			pCsNode->m_pNiNode->Update( fAniTime );
		}
	}



	int nBase;
	int nEquip;
	for( int i=0; i<IF_TAMER_STAT_COUNT; ++i )
	{
		switch( i )
		{
		case 0:
			nBase = FMTamer::GetBaseAtt( &TInfo );
			nEquip = FMTamer::GetEquipAtt( &TInfo );
			break;
		case 1:
			nBase = FMTamer::GetBaseDef( &TInfo );
			nEquip = FMTamer::GetEquipDef( &TInfo );
			break;
		case 2:
			nBase = FMTamer::GetBaseMaxHP( &TInfo );
			nEquip = FMTamer::GetEquipMaxHP( &TInfo );
			break;
		case 3:
			nBase = FMTamer::GetBaseMaxDS( &TInfo );
			nEquip = FMTamer::GetEquipMaxDS( &TInfo );
			break;
			default:
				assert_cs( false );
			}		

		SAFE_POINTER_CON( m_pStatText[ i ] );
		m_pStatText[ i ]->SetColor( nEquip != 0 ? NiColor( 0, 1, 0 ) : FONT_WHITE );
		switch( i )
		{
		case 0:
//			assert_cs( pUserStat->GetAtt() == nBase + nEquip );
			m_pStatText[ i ]->SetText( pUserStat->GetAtt() );
			break;
		case 1:
//			assert_cs( pUserStat->GetDef() == nBase + nEquip );
			m_pStatText[ i ]->SetText( pUserStat->GetDef() );
			break;
		case 2:
//			assert_cs( pUserStat->GetMaxHP() == nBase + nEquip );
			m_pStatText[ i ]->SetText( pUserStat->GetMaxHP() );
			break;
		case 3:
//			assert_cs( pUserStat->GetMaxDS() == nBase + nEquip );
			m_pStatText[ i ]->SetText( pUserStat->GetMaxDS() );
			break;
		default:
			assert_cs( false );
		}		
	}
}


bool cTamerStatus::_UpdateIcon_ForMouse_Equip()
{
	bool bCheck = false;
	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

#ifdef WEB_INFOMATION
	switch( m_pRank->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:		
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_WEBWIN ) )
			{
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_WEBWIN );
			}												
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_WEBWIN, 0, cWebWin::eRANK );		
			g_pGameIF->GetWebWin()->SetWebControl( cWebWin::eRANK, CsPoint( 50, 200 ), CsPoint( 416, 550 ), g_pCharMng->GetTamerUser()->GetName() );
			bCheck = true;
		}		
		break;
	}
#endif

	switch( eCursor )
	{	
	case CURSOR::LBUTTON_DOWN:		
		{
			for( int i=0; i<nLimit::Equip; ++i )
			{
				m_IFIcon_Equip[ i ].MouseLBtnClick( ptMouseLocalPos );
			}

			m_IFIcon_Equip_Digivice.MouseLBtnClick( ptMouseLocalPos );
		}
		break;	
	case CURSOR::LBUTTON_PRESS:
		{
			_OnMouseOutWindow_Equip();

			for( int i=0; i<nLimit::Equip; ++i )
			{
				if( m_IFIcon_Equip[ i ].s_eState != sIFIcon::MOUSE_LCLICK )
					m_IFIcon_Equip[ i ].MouseOn( ptMouseLocalPos );
			}

			if( m_IFIcon_Equip_Digivice.s_eState != sIFIcon::MOUSE_LCLICK )
				m_IFIcon_Equip_Digivice.MouseOn( ptMouseLocalPos );
		}
		break;
	case CURSOR::LBUTTON_UP:
		{
			for( int i=0; i<nLimit::Equip; ++i )
				m_IFIcon_Equip[ i ].ReleaseState();

			m_IFIcon_Equip_Digivice.ReleaseState();
		}
		break;
	case CURSOR::RBUTTON_DOWN:		
		{
			for( int i=0; i<nLimit::Equip; ++i )
			{
				m_IFIcon_Equip[ i ].MouseRBtnClick( ptMouseLocalPos );
			}

			m_IFIcon_Equip_Digivice.MouseRBtnClick( ptMouseLocalPos );
		}
		break;	
	case CURSOR::RBUTTON_PRESS:
		{
			_OnMouseRButtonOutWindow_Equip();

			for( int i=0; i<nLimit::Equip; ++i )
			{
				if( m_IFIcon_Equip[ i ].s_eState != sIFIcon::MOUSE_RCLICK )
					m_IFIcon_Equip[ i ].MouseOn( ptMouseLocalPos );
			}

			if( m_IFIcon_Equip_Digivice.s_eState != sIFIcon::MOUSE_RCLICK )
				m_IFIcon_Equip_Digivice.MouseOn( ptMouseLocalPos );
		}
		break;
	case CURSOR::RBUTTON_UP:
		{
			for( int i=0; i<nLimit::Equip; ++i )
				m_IFIcon_Equip[ i ].ReleaseState();
			m_IFIcon_Equip_Digivice.ReleaseState();
		}
		break;
	default:		
		for( int i=0; i<nLimit::Equip; ++i )
		{
			if( ( bCheck == false )&&( m_IFIcon_Equip[ i ].MouseOn( ptMouseLocalPos ) == true ) )
				bCheck = true;
			else
				m_IFIcon_Equip[ i ].ReleaseState();
		}

		if( ( bCheck == false )&&( m_IFIcon_Equip_Digivice.MouseOn( ptMouseLocalPos ) == true ) )
			bCheck = true;
		else
			m_IFIcon_Equip_Digivice.ReleaseState();
	}

	// 툴팁용
	{
		for( int i=0; i<nLimit::Equip; ++i )
		{
			cItemInfo* pData = g_pDataMng->GetTEquip()->GetData( i );

			if( pData->IsEnable() == false )
				continue;

			if( m_IFIcon_Equip[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				CsPoint pos = m_IFIcon_Equip[ i ].GetPos() + GetRootClient();
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_TAMER_EQUIP_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pData->GetType(), cBaseWindow::WT_TAMERSTATUS, 0, 0, pData );
				break;
			}
		}

		if( g_pDataMng->GetTEquip()->GetDigiviceItem()->IsEnable() == true )
		{			
			if( m_IFIcon_Equip_Digivice.s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				cItemInfo* pData = g_pDataMng->GetTEquip()->GetDigiviceItem();
				CsPoint pos = m_IFIcon_Equip_Digivice.GetPos() + GetRootClient();
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_TAMER_EQUIP_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pData->GetType(), cBaseWindow::WT_TAMERSTATUS, 0, 0, pData );
			}
		}
	}

	// 스탯 툴팁
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	FMTamer::sTINFO TInfo;
	TInfo.s_nTamerID = pTamerUser->GetOrgModelID();
	TInfo.s_nLevel = pTamerUser->GetBaseStat()->GetLevel();

	int nType;
	int nA, nB;
	for( int i=0; i<IF_TAMER_STAT_COUNT; ++i )
	{
		if( m_rcStatRegion[ i ].PtInRect( ptMouseLocalPos ) == FALSE )
			continue;

		switch( i )
		{
		case 0:
			nType = APPLY_AP;
			nA = FMTamer::GetBaseAtt( &TInfo );
			nB = FMTamer::GetEquipAtt( &TInfo );
			break;
		case 1:
			nType = APPLY_DP;
			nA = FMTamer::GetBaseDef( &TInfo );
			nB = FMTamer::GetEquipDef( &TInfo );
			break;
		case 2:
			nType = APPLY_MAXHP;
			nA = FMTamer::GetBaseMaxHP( &TInfo );
			nB = FMTamer::GetEquipMaxHP( &TInfo );
			break;
		case 3:
			nType = APPLY_MAXDS;
			nA = FMTamer::GetBaseMaxDS( &TInfo );
			nB = FMTamer::GetEquipMaxDS( &TInfo );
			break;
		default:
			assert_cs( false );
		}
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( m_rcStatRegion[ i ].GetPos() + GetRootClient(), m_rcStatRegion[ i ].GetSize(), 190, cTooltip::STAT, nType, nA, nB, 0, g_pCharMng->GetTamerUser() );
		bCheck = true;
		break;		
	}
	return bCheck;
}

void cTamerStatus::_RenderEquip()
{
	m_pEquipBG->Render();

	cItemInfo* pItemArray = g_pDataMng->GetTEquip()->GetDataArray();
	cItemInfo* pItem;
	CsPoint pos;

	for( int i=0; i<nLimit::Equip; ++i )
	{
		pItem = &pItemArray[ i ];

		// 위치
		pos = m_IFIcon_Equip[ i ].GetPos() + GetRootClient();

		if( pItem->IsEnable() == true )
		{
			// 아이템 랜더링
			g_pIconMng->RenderItem( pItem, pos, false );

			// 쿨타임 중이라면 쿨타임 랜더링
			g_pIconMng->RenderTime( pos, ICON_HALF, nsCsFileTable::g_pItemMng->GetItem( pItem->GetType() )->GetInfo() );

			if( g_pDataMng->IsItemLock( TO_TEQUIP_SID( i ) ) == true )
			{
				g_pIconMng->RenderMask( ICONMASK::IF_ICON_MOUSE_DRAG, pos, IF_TAMER_EQUIP_ICON_SIZE );
			}
		}

		// 마스크 있다면 마스크 랜더링
		if( m_IFIcon_Equip[ i ].s_eState != sIFIcon::NONE )
		{			
			m_IFIcon_Equip[ i ].RenderMask( pos, IF_TAMER_EQUIP_ICON_SIZE );
		}
	}

	// 디지바이스
	{
		// 위치
		pos = m_IFIcon_Equip_Digivice.GetPos() + GetRootClient();
		pItem = g_pDataMng->GetTEquip()->GetDigiviceItem();
		if( pItem->IsEnable() == true )
		{
			// 아이템 랜더링
			g_pIconMng->RenderItem( pItem, pos, false );

			// 쿨타임 중이라면 쿨타임 랜더링
			g_pIconMng->RenderTime( pos, ICON_HALF, nsCsFileTable::g_pItemMng->GetItem( pItem->GetType() )->GetInfo() );

			if( g_pDataMng->IsItemLock( TO_DIGIVICE_SID( 0 ) ) == true )
			{
				g_pIconMng->RenderMask( ICONMASK::IF_ICON_MOUSE_DRAG, pos, IF_TAMER_EQUIP_ICON_SIZE );
			}
		}
		// 마스크 있다면 마스크 랜더링
		if( m_IFIcon_Equip_Digivice.s_eState != sIFIcon::NONE )
		{			
			m_IFIcon_Equip_Digivice.RenderMask( pos, IF_TAMER_EQUIP_ICON_SIZE );
		}
	}

	if( m_pRenderTex != NULL )
	{
		m_pRenderTex->BeginRender();
		m_pRenderTex->RenderObject( GET_SUBCAMERA(CAMERA_02), g_pCharMng->GetTamerUser(), true );	//2015-11-26-nova 메인카메라->서브카메라 사용
		m_pRenderTex->EndRender( GET_SUBCAMERA(CAMERA_02), CsPoint( 49, 43 ) + GetRootClient() );
	}

	for( int i=0; i<IF_TAMER_STAT_COUNT; ++i )
		m_pStatText[ i ]->Render();

	m_pNameText->Render();
	m_pGuildText->Render();

#ifdef WEB_INFOMATION
	m_pRank->Render();
#endif

	// 친화력
	int nFriendShip = g_pCharMng->GetDigimonUser( 0 )->GetBaseStat()->GetFriendShip();
	int AdjustYPos = 0;
#ifdef SDM_TAMER_XGUAGE_20180628
	AdjustYPos = 22;
#endif

	g_pIconMng->RenderNumber( DT_CENTER, NUMBERSPRITE::ORANGE, nFriendShip, CsPoint( 198, 376 + AdjustYPos) + GetRootClient(), CsPoint( 30, 30 ), false );

#ifdef	FRIENDSHIP_TOOLTIP
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	if( m_FriendshipPoint.PtInRect( ptMouseLocalPos ) == TRUE )
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 10, 10 ), 0, cTooltip::FRIENDSHIP_POINT, 1, 0, 0, 0, 0);
#endif

#ifdef UI_REMOVE_RENEWAL_20190109
#else
	for( int i = 0; i < 6; i++ )
	{
		m_pEquipText[ i ]->Render();
	}
#endif
}


void cTamerStatus::_OnMouseOutWindow_Equip()
{
	if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_PRESS )
		return;
	if( CURSOR_ST.IsIcon() == true )
		return;

	_UpdateEquipParts(CURSOR_ST.GetPos(), sIFIcon::MOUSE_LCLICK, FALSE);

	_UpdateEquipDigivice(CURSOR_ST.GetPos(), sIFIcon::MOUSE_LCLICK, FALSE);
}

void cTamerStatus::_OnLButtonUp_Equip( CsPoint pos )
{
	_UpdateEquipParts(pos, sIFIcon::MOUSE_LCLICK, TRUE);

	_UpdateEquipDigivice(pos, sIFIcon::MOUSE_LCLICK, TRUE);
}

void cTamerStatus::_UpdateEquipParts(CsPoint Pos, sIFIcon::eSTATE	check, BOOL MouseOn)
{
	CsPoint MousePos = MousePosToWindowPos( Pos );

	cData_TEquip* pDataEquip = g_pDataMng->GetTEquip();
	SAFE_POINTER_RET(pDataEquip);
	for( int i=0; i<nLimit::Equip; ++i )
	{
		if( pDataEquip->GetData( i )->IsEnable() == false )
			continue;

		if( ( m_IFIcon_Equip[ i ].s_eState == check )	&&
			( m_IFIcon_Equip[ i ].s_Rect.PtInRect( MousePos ) == MouseOn ) )
		{
			if( g_pDataMng->IsItemUse( TO_TEQUIP_SID( i ) ) == false )
			{
				return;
			}

			if( g_pDataMng->IsItemLock( TO_TEQUIP_SID( i ) ) == true )
			{
				cPrintMsg::PrintMsg( 10004 );
				return;
			}

			// 동작
			if( check == sIFIcon::MOUSE_RCLICK && g_pDataMng->IsItemUse( TO_TEQUIP_SID( i ) ) == true )
				g_pDataMng->SendItemMoveInven( TO_TEQUIP_SID( i ) );

			else if( check == sIFIcon::MOUSE_LCLICK )	// 이경우에만 아이콘 만들어준다.
				CURSOR_ST.SetIcon( CURSOR_ICON::CI_TEQUIP, i, 1, &m_IFIcon_Equip[ i ] );

			m_IFIcon_Equip[ i ].ReleaseState();
			break;
		}
	}
}

void cTamerStatus::_UpdateEquipDigivice(CsPoint pos, sIFIcon::eSTATE	check, BOOL MouseOn)
{
	CsPoint MousePos = MousePosToWindowPos( pos );

	cData_TEquip* pDataEquip = g_pDataMng->GetTEquip();
	SAFE_POINTER_RET(pDataEquip);
	if( pDataEquip->GetDigiviceItem()->IsEnable() == true )
	{
		if( m_IFIcon_Equip_Digivice.s_eState == check &&
			m_IFIcon_Equip_Digivice.s_Rect.PtInRect( MousePos ) == MouseOn)
		{
			if( g_pDataMng->IsItemUse( TO_DIGIVICE_SID( 0 ) ) == false )
			{
				return;
			}
			else if( g_pDataMng->IsItemLock( TO_DIGIVICE_SID( 0 ) ) == true )
			{
				cPrintMsg::PrintMsg( 10004 );
				return;
			}

			// 동작
			if( check == sIFIcon::MOUSE_RCLICK && g_pDataMng->IsItemUse( TO_DIGIVICE_SID( 0 ) ) == true )
			{
				g_pDataMng->SendItemMoveInven( TO_DIGIVICE_SID( 0 ) );
			}

			if( check == sIFIcon::MOUSE_LCLICK )	// 이경우에만 아이콘 만들어준다.
				CURSOR_ST.SetIcon( CURSOR_ICON::CI_TEQUIP, SERVER_DATA_DIGIVICE_CONSTANT, 1, &m_IFIcon_Equip_Digivice );
			
			m_IFIcon_Equip_Digivice.ReleaseState();
		}
			
	}
}

void cTamerStatus::_OnRButtonUp_Equip( CsPoint pos )
{
	_UpdateEquipParts(pos, sIFIcon::MOUSE_RCLICK, TRUE);

	_UpdateEquipDigivice(pos, sIFIcon::MOUSE_RCLICK, TRUE);
}

void cTamerStatus::_OnMouseRButtonOutWindow_Equip()
{
	if( CURSOR_ST.GetButtonState() != CURSOR::RBUTTON_PRESS )
		return;
	if( CURSOR_ST.IsIcon() == true )
		return;

	_UpdateEquipParts(CURSOR_ST.GetPos(), sIFIcon::MOUSE_RCLICK, FALSE);

	_UpdateEquipDigivice(CURSOR_ST.GetPos(), sIFIcon::MOUSE_RCLICK, FALSE);
}

bool cTamerStatus::_CursorIconLBtnUp_Equip( CURSOR_ICON::eTYPE eIconType, int nIconSlot )
{
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			// 장착 할수 있는 아이템인지 체크
			CsItem::sINFO* pFTItemInfo = g_pDataMng->GetInven()->GetFTData( TO_ID( nIconSlot ) );
			switch( pFTItemInfo->s_nType_L )
			{
			case nItem::Head:
			case nItem::Coat:
			case nItem::Glove:
			case nItem::Pants:
			case nItem::Shoes:
			case nItem::Costume:
			case nItem::Glass:
			case nItem::Digivice:
#ifdef SDM_TAMER_XGUAGE_20180628
			case nItem::XAI:
#endif
			case nItem::EquipAura: //오라 추가 chu8820
				g_pDataMng->SendItemUse( nIconSlot );
				break;
			case nItem::Earring:
			case nItem::Necklace:
			case nItem::Ring:
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
			case nItem::Bracelet:
#endif
				{
					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCHANT_OPTION ) )
						cPrintMsg::PrintMsg( 11014 );
					else
						g_pDataMng->SendItemUse( nIconSlot );
				}
				break;
			default:
				cPrintMsg::PrintMsg( 11014 );
			}
		}
		break;
	case CURSOR_ICON::CI_TEQUIP:
		break;
	case CURSOR_ICON::CI_WAREHOUSE:
		cPrintMsg::PrintMsg( 11018 );
		break;
	default:
		cPrintMsg::PrintMsg( 11014 );
	}
	CURSOR_ST.ReleaseIcon();
	return true;	
}

void cTamerStatus::_ResetDevice_Equip()
{
	if( m_pEquipBG )
		m_pEquipBG->ResetDevice();

	if( m_pRenderTex )
	{
		m_pRenderTex->ResetDevice();
	}

	for( int i=0; i<IF_TAMER_STAT_COUNT; ++i )
	{
		if( m_pStatText[ i ])
			m_pStatText[ i ]->ResetDevice();
	}

	if( m_pNameText )
		m_pNameText->ResetDevice();

	if( m_pGuildText )
		m_pGuildText->ResetDevice();

#ifdef WEB_INFOMATION
	if( m_pRank )
		m_pRank->ResetDevice();
#endif

#ifdef UI_REMOVE_RENEWAL_20190109
#else
	for( int i = 0; i < 6; i++ )
	{
		if( m_pEquipText[ i ] )
			m_pEquipText[ i ]->ResetDevice();
	}
#endif
}
