#include "stdafx.h"
#include "Centerbar.h"

cCenterbar::cCenterbar()
{	
	for( int i=0; i<CENTERBAR_X * CENTERBAR_Y; ++i )
	{
		m_CenterInfo[ i ].Delete();		
	}
}

void cCenterbar::Destroy()
{
	cBaseWindow::Delete();
}

void cCenterbar::DeleteResource()
{
	DeleteScript();

	SAFE_NIDELETE( m_pBg );

	for( int i=0; i<CENTERBAR_X * CENTERBAR_Y; ++i )
	{
		m_CenterInfo[ i ].Delete();		
	}
}

void cCenterbar::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	m_nValue = nValue;
	if( m_nValue == 0 )
		InitScript( NULL, CsPoint( 317, 395 ), CsPoint( 29 * CENTERBAR_X + 12, 29 * CENTERBAR_Y ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );
	else
		InitScript( NULL, CsPoint( 612, 395 ), CsPoint( 29 * CENTERBAR_X + 12, 29 * CENTERBAR_Y ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );

	CsSIZE size( 21, 21 );
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_9 );
	ti.s_eTextAlign = DT_RIGHT;

	m_pBg = NiNew cSprite;
	m_pBg->Init( NULL, CsPoint::ZERO, CsPoint( 29, 29 ), "QuickSlot\\Evol.tga", false );	

	int nFlag;
	for( int i=0; i<CENTERBAR_X; ++i )
	{
		for( int j=0; j<CENTERBAR_Y; ++j )
		{	
			if( m_nValue == 0 )
			{
				nFlag = ( (j%CENTERBAR_Y) == 0 ) ? 0 : 1;				
			}
			else
			{
				nFlag = ( (j%CENTERBAR_Y) == 0 ) ? 1 : 0;				
			}
			m_CenterInfo[ i + (j*CENTERBAR_X) ].s_IFIcon.Init( CsRect( CsPoint( 29*(i%CENTERBAR_X)+4 + nFlag*12, 29*(j%CENTERBAR_Y)+4 ), size ) );
			m_CenterInfo[ i + (j*CENTERBAR_X) ].s_bRender =	
				( g_pResist->m_CenterBar.s_CenterBarInfo[ i + (j*CENTERBAR_X) + ( m_nValue * CENTERBAR_X * CENTERBAR_Y ) ].s_eType == cResist::ITEM );	
		}
	}
}

cBaseWindow::eMU_TYPE cCenterbar::Update_ForMouse()
{
	if( g_pResist->m_CenterBar.s_bInitial == false )
		return MUT_OUT_WINDOW;

	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;	

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );	
	bool	bIn = false;

	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();
	if( eCursor == CURSOR::LBUTTON_PRESS )
		OnMouseOutWindow();

	//
	for( int i=0; i<CENTERBAR_X * CENTERBAR_Y; ++i )
	{
		if( m_CenterInfo[ i ].s_bRender == false && CURSOR_ST.IsIcon() == false )
			continue;

		switch(eCursor)
		{
		case CURSOR::LBUTTON_DOWN:
			{
				if(	m_CenterInfo[ i ].s_IFIcon.MouseLBtnClick( ptMouseLocalPos ) )
					bIn = true;					
				else
					m_CenterInfo[ i ].s_IFIcon.ReleaseState();

			}
			break;
		case CURSOR::RBUTTON_DOWN:
			{
				if( m_CenterInfo[ i ].s_IFIcon.MouseRBtnClick( ptMouseLocalPos ) )
					bIn = true;
				else
					m_CenterInfo[ i ].s_IFIcon.ReleaseState();

			}			
			break;
		case CURSOR::LBUTTON_PRESS:
			{
				if( m_CenterInfo[ i ].s_IFIcon.s_eState != sIFIcon::MOUSE_LCLICK && m_CenterInfo[ i ].s_IFIcon.MouseOn( ptMouseLocalPos ) == false)
					m_CenterInfo[ i ].s_IFIcon.ReleaseState();
				else
					bIn = true;
			}
			break;
		case CURSOR::RBUTTON_PRESS:
			{
				if( m_CenterInfo[ i ].s_IFIcon.s_eState != sIFIcon::MOUSE_RCLICK && m_CenterInfo[ i ].s_IFIcon.MouseOn( ptMouseLocalPos ) == false )
					m_CenterInfo[ i ].s_IFIcon.ReleaseState();
				else
					bIn = true;
			}
			break;
		default:
			{
				if( m_CenterInfo[ i ].s_IFIcon.MouseOn( ptMouseLocalPos ) )
					bIn = true;
				else
					m_CenterInfo[ i ].s_IFIcon.ReleaseState();
			}
			break;
		}
	}
// 	return muReturn;	// 센터바 내부에서 카메라 좌클릭 드래그 카메라 회전 동작 안되도록 수정 chu8820
	return (bIn == true) ? muReturn : MUT_OUT_WINDOW;
}

void cCenterbar::Render()
{
	if( g_pResist->m_CenterBar.s_bInitial == false )
		return;

	RenderScript();

	CsPoint pos; 
	for( int i=0; i<CENTERBAR_X * CENTERBAR_Y; ++i )
	{
		pos = m_CenterInfo[ i ].s_IFIcon.GetPos() + GetRootClient();	

		m_pBg->SetAlpha( g_pResist->m_CenterBar.s_fAlpha );
		if( CURSOR_ST.IsIcon() == true )		
		{	
			m_pBg->SetAlpha( 0.7f );						
		}
		m_pBg->Render( pos - CsPoint( 4, 4 ) );

		CsItem::sINFO* pFTItemInfo;

		int CenterBarNum = i + ( m_nValue * CENTERBAR_X * CENTERBAR_Y );

		if( CenterBarNum >= nLimit::CenterBar )
			continue;

		cResist::sCHAR_CENTERBAR::sINFO* pInfo = &g_pResist->m_CenterBar.s_CenterBarInfo[ CenterBarNum ];
		switch( pInfo->s_eType )
		{
		case cResist::NONE:
			break;
		case cResist::ITEM:
			{
				cItemInfo Item;
// 				Item.m_nType = nID;
// 				Item.m_nCount = nCount;
				Item.m_nType = pInfo->s_nID;
				Item.m_nCount = pInfo->s_nCount;

				g_pIconMng->RenderItem( &Item, pos, CsPoint( 21, 21 ), true );

// 				pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( nID )->GetInfo();
				pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( pInfo->s_nID )->GetInfo();

				// 쿨타임 중이라면 쿨타임 랜더링
				g_pIconMng->RenderTime( pos, 11, pFTItemInfo );

				// 전투중에 못먹는 아이템이라면 레드 마스크 랜더링
				if( ( pFTItemInfo->s_nUseMode == 1 )&&( g_pCharMng->GetDigimonUser( 0 )->IsBattle() ) )
				{
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, pos, CsPoint( 21, 21 ) );
				}
			}
			break;		
		}

		// 마스크 있다면 마스크 랜더링
		if( m_CenterInfo[ i ].s_IFIcon.s_eState != sIFIcon::NONE )
		{			
			m_CenterInfo[ i ].s_IFIcon.RenderMask( pos, CsPoint( 21, 21 ) );
		}
	}
}

void cCenterbar::OnMouseOutWindow()
{
	if( g_pResist->m_CenterBar.s_bInitial == false )
		return;

	if( IsEnableWindow() == false )
		return;

	cBaseWindow::OnMouseOutWindow();

	if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_PRESS )
	{
		for( int i=0; i<CENTERBAR_X * CENTERBAR_Y; ++i )
		{
			m_CenterInfo[ i ].s_IFIcon.ReleaseState( true );
		}
		return;
	}
	if( CURSOR_ST.IsIcon() == true )
		return;

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	for( int i=0; i<CENTERBAR_X * CENTERBAR_Y; ++i )
	{
		if( m_CenterInfo[ i ].s_IFIcon.s_eState == sIFIcon::MOUSE_LCLICK )
		{
			if( m_CenterInfo[ i ].s_IFIcon.s_Rect.PtInRect( ptMouseLocalPos ) == FALSE )
			{
				int CenterBarNum = i + ( m_nValue * CENTERBAR_X * CENTERBAR_Y );

				if( CenterBarNum >= nLimit::CenterBar )
					continue;

				cResist::sCHAR_CENTERBAR::sINFO* pInfo = &g_pResist->m_CenterBar.s_CenterBarInfo[ CenterBarNum ];
				switch( pInfo->s_eType )
				{
				case cResist::NONE:
					m_CenterInfo[ i ].Delete();
					break;			
				case cResist::ITEM:
					{
						// 퀵슬롯에서의 아이콘 갯수는 의미가 없다
						CURSOR_ST.SetIcon( CURSOR_ICON::CI_CENTERBAR, i, 1, &m_CenterInfo[ i ].s_IFIcon );
						CURSOR_ST.SetIconSubInfo( CURSOR_ICON::CI_CENTERBAR, pInfo->s_eType, pInfo->s_nID );
						pInfo->Reset();
						m_CenterInfo[ i ].Delete();
					}
					break;
				default:
					assert_cs( false );
				}
			}
			break;
		}
	}
}

void cCenterbar::ResetDevice()
{
	if( g_pResist->m_CenterBar.s_bInitial == false )
		return;

	ResetDeviceScript();
	m_pBg->ResetDevice();
}

void cCenterbar::OnLButtonUp( CsPoint pos )
{
	if( g_pResist->m_CenterBar.s_bInitial == false )
		return;

	CsPoint ptLocal = MousePosToWindowPos( pos );
	sIFIcon* pIFIcon;
	for( int i=0; i<CENTERBAR_X * CENTERBAR_Y; ++i )
	{
		pIFIcon = &m_CenterInfo[ i ].s_IFIcon;

		int CenterBarNum = i + ( m_nValue * CENTERBAR_X * CENTERBAR_Y );

		if( CenterBarNum >= nLimit::CenterBar )
			continue;

		if( ( ( pIFIcon->s_eState == sIFIcon::MOUSE_LCLICK )&&( pIFIcon->s_Rect.PtInRect( ptLocal ) ) )
			|| ( ( pIFIcon->s_eState == sIFIcon::MOUSE_RCLICK )&&( pIFIcon->s_Rect.PtInRect( ptLocal ) ) )	)
		{			
			cResist::sCHAR_CENTERBAR::sINFO* pInfo = &g_pResist->m_CenterBar.s_CenterBarInfo[ CenterBarNum ];

			// 동작
			cData_Inven* pInven = g_pDataMng->GetInven();
			int nSlot = pInven->GetFirstSlot_Item_ID( pInfo->s_nID );
			assert_cs( nSlot != cData_Inven::INVALIDE_INVEN_INDEX );
			g_pDataMng->SendItemUse( TO_INVEN_SID( nSlot ) );	
		}
		pIFIcon->s_eState = sIFIcon::NONE;
	}
}

void cCenterbar::OnRButtonUp( CsPoint pos )
{
	if( g_pResist->m_CenterBar.s_bInitial == false )
		return;

	OnLButtonUp( pos );	
}

bool cCenterbar::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	if( g_pResist->m_CenterBar.s_bInitial == false )
		return false;

	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );
	for( int i=0; i<CENTERBAR_X * CENTERBAR_Y; ++i )
	{
		if( m_CenterInfo[ i ].s_IFIcon.s_Rect.PtInRect( ptLocal ) )
		{
			int nInsertReturn = -1;

			int CenterBarNum = i + ( m_nValue * CENTERBAR_X * CENTERBAR_Y );

			if( CenterBarNum >= nLimit::CenterBar )
				continue;

			cResist::sCHAR_CENTERBAR::sINFO* pInfo = &g_pResist->m_CenterBar.s_CenterBarInfo[ CenterBarNum ];
			switch( eIconType )
			{
			case CURSOR_ICON::CI_INVEN:	
				nInsertReturn = _InvenToCenterBar( nIconSlot, pInfo );
				break;
			case CURSOR_ICON::CI_CENTERBAR:
				nInsertReturn = _CenterBarToCenterBar( nIconSlot + ( m_nValue * CENTERBAR_X * CENTERBAR_Y ), pInfo );
				break;
			default:
				nInsertReturn = 0;
				cPrintMsg::PrintMsg( 11014 );
				break;
			}
			switch( nInsertReturn )
			{
			case 0:				// 커서 내비두기
				return true;
			case 1:				// 커서 교체
				CURSOR_ST.ReleaseIcon();
				CURSOR_ST.SetIcon( CURSOR_ICON::CI_CENTERBAR, i, 1, &m_CenterInfo[ i ].s_IFIcon );				
				return true;
			case 2:				// 커서 해제
				CURSOR_ST.ReleaseIcon();
				m_CenterInfo[ i ].s_bRender = true;
				return true;
			}						
		}
	}	
	return false;
}

int cCenterbar::_InvenToCenterBar( int nInvenIndex, cResist::sCHAR_CENTERBAR::sINFO* pData )
{
	int nReturn = 0;

	cItemInfo* pItem = g_pDataMng->GetInven()->GetData( nInvenIndex );
	CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType )->GetInfo();

	// 퀵슬롯 장착 가능한 물품인지 확인
	switch( pFTInfo->s_nType_L )
	{
	case nItem::UseQuest:
	case nItem::Capsule:		// 캡슐
	case nItem::Potion:			// 포션, 카드, 소모품
	case nItem::Card:
	case nItem::Consume:
	case nItem::Buff: 
	case nItem::SKILL_Card:
	case nItem::Fire_Work:
#ifdef CROSSWARS_SYSTEM
	case nItem::CodeCrown:
#endif
#ifdef DEBUFF_ITEM
	case nItem::Debuff:
#endif
	// 퀵슬롯 등록 타입 추가
	case nItem::Portal:
	case nItem::Cash_ChangeDName:
	case nItem::Cash_ChangeTName:
	case nItem::Cash_ResetSkillPoint:
	case nItem::Cash_ExtraInven:
	case nItem::Cash_ExtraWarehouse:
	case nItem::Cash_JumpBuster:
	case nItem::DigimonSlotInc:
	case nItem::Cash_DigimonCareSlot:	
	case nItem::ScanUse_Item:
	case nItem::Item_TamerSKillConsume:
	case nItem::PersonStore:
#ifdef CROSSWARS_SYSTEM
	case nItem::Cash_CrossInvenSlot:
#endif
#ifdef LEVELUP_EFFECT_LOOP
	case nItem::NoticeItem:
#endif

	// 스킬 등록 아이템 센터바에 등록 가능하도록...
	case nItem::DigimonSkillChipAST:
	case nItem::DigimonSkillChipATK:
	case nItem::DigimonSkillChipDEF:
#ifdef SDM_TAMER_XGUAGE_20180628
	case nItem::XAI:
#endif
		break;
	default:
		{
			cPrintMsg::PrintMsg( 11014 );
		}
		return nReturn;
	}

	nReturn = 2;

	// 대상이 비여 있는지 체크
	// 비여 있지 않다면 커서아이콘에 셋팅
	if( pData->s_eType != cResist::NONE )
	{
		CURSOR_ST.SetIconSubInfo( CURSOR_ICON::CI_CENTERBAR, pData->s_eType, pData->s_nID );
		nReturn = 1;
	}

	pData->s_eType = cResist::ITEM;
	pData->s_nID = pItem->m_nType;
	pData->s_nCount = g_pDataMng->GetInven()->GetCount_Item_ID( pItem->m_nType );
	pData->s_nCount += g_pDataMng->GetTEquip()->GetCount_Item_ID( pItem->m_nType );
	assert_cs( pData->s_nCount > 0 );
	return nReturn;
}

int cCenterbar::_CenterBarToCenterBar( int nInvenIndex, cResist::sCHAR_CENTERBAR::sINFO* pData )
{
	int nReturn = 0;
	nReturn = 2;	

	// 현재 들고있는거
	cResist::sCHAR_CENTERBAR::sINFO sInfo;
	sInfo.s_eType = (cResist::eTYPE)CURSOR_ST.GetIconSubInfo2();
	sInfo.s_nID = CURSOR_ST.GetIconSubInfo3();
	sInfo.s_nCount = g_pDataMng->GetInven()->GetCount_Item_ID( CURSOR_ST.GetIconSubInfo3() );
	sInfo.s_nCount += g_pDataMng->GetTEquip()->GetCount_Item_ID( CURSOR_ST.GetIconSubInfo3() );
	// 대상이 비여 있는지 체크
	// 비여 있지 않다면 커서아이콘에 셋팅
	if( pData->s_eType != cResist::NONE )
	{
		CURSOR_ST.SetIconSubInfo( CURSOR_ICON::CI_CENTERBAR, pData->s_eType, pData->s_nID );
		nReturn = 1;
	}	
	memcpy( pData, &sInfo, sizeof( cResist::sCHAR_CENTERBAR::sINFO ) ); 
	
	return nReturn;
}

void cCenterbar::CheckAllSlot()
{
	for( int i = 0; i < nLimit::CenterBar; i++ )
	{
		cResist::sCHAR_CENTERBAR::sINFO* pInfo = &g_pResist->m_CenterBar.s_CenterBarInfo[ i ];
		pInfo->s_nCount = g_pDataMng->GetInven()->GetCount_Item_ID( pInfo->s_nID );
		pInfo->s_nCount += g_pDataMng->GetTEquip()->GetCount_Item_ID( pInfo->s_nID );
		if( pInfo->s_nCount == 0 )
		{
			pInfo->Reset();		//m_CenterInfo[ i ].Delete();
		}
	}
}

