#include "stdafx.h"

#include "CapsuleEvol.h"
#ifdef VERSION_USA
#define IF_CAPSULE_EVOL_ITEM_POS		CsPoint( 141, 121 )
#define IF_CAPSULE_EVOL_ITEM_NAME_POS	CsPoint( 152, 71 )
#else
#define IF_CAPSULE_EVOL_ITEM_POS		CsPoint( 116, 96 )
#define IF_CAPSULE_EVOL_ITEM_NAME_POS	CsPoint( 132, 58 )
#endif
cCapsuleEvolution::cCapsuleEvolution()
{
	m_nCapsuleItem = 0;
	m_nInvenIdx = -1;
	m_nEvolSlot = 0;
}
void cCapsuleEvolution::Destroy()
{
	cBaseWindow::Delete();
}

void cCapsuleEvolution::DeleteResource()
{
	//	NISAFE_DELETE( m_pDigimonBG_XL );

	DeleteScript();
}

void cCapsuleEvolution::Create( int nValue /*= 0 */ )
{
	if( g_pGameIF && !g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_DIGIMONSTATUS ) )
		return;

	cBaseWindow::Init();
#ifdef VERSION_USA
	int nPosX = ( g_nScreenWidth/2 ) - ( 315/2 );
	int nPosY = ( g_nScreenHeight/2 ) - ( 270/2 );
	InitScript( "CapsuleEvo\\CapsuleEvo_BG.tga", CsPoint( nPosX, nPosY ) , CsPoint( 315, 270 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP, false );

	m_pCancelBtn = AddButton( CsPoint( 289, 12 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	  
	m_pEvolBtn = AddButton( CsPoint( 120, 209 ) , CsPoint( 74, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );

#else
	int nPosX = ( g_nScreenWidth/2 ) - ( 265/2 );
	int nPosY = ( g_nScreenHeight/2 ) - ( 220/2 );

	InitScript( "CapsuleEvo\\CapsuleEvo_BG.tga", CsPoint( nPosX, nPosY ) , CsPoint( 265, 220 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP, false );

	m_pCancelBtn = AddButton( CsPoint( 241, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	  
	m_pEvolBtn = AddButton( CsPoint( 95, 184 ) , CsPoint( 74, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );

#endif

	m_pItemName = NiNew cString;

	m_nEvolSlot = nValue;

	// 타이틀
	CDigimonEvolveObj* pEvolObj = g_pCharMng->GetDigimonUser( 0 )->GetFTEvol()->GetEvolveObjByEvoSlot( m_nEvolSlot );
	TCHAR szTitle[ ITEM_NAME_LEN ];
	_tcscpy_s( szTitle, ITEM_NAME_LEN, nsCsFileTable::g_pItemMng->GetTypeName( pEvolObj->m_nOpenItemTypeS ) );
	_tcscat( szTitle, _T(" ") );
	_tcscat( szTitle, UISTRING_TEXT( "COMMON_TXT_REGISTER" ).c_str() );

#ifdef VERSION_USA
	AddTitle( szTitle, CsPoint( -10, 5 ) );
#else
	AddTitle( szTitle, CsPoint( 0, 2 ) );
#endif
	//인벤 오픈
	bool bSound = true;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );
}

void cCapsuleEvolution::Update(float const& fDeltaTime)
{
	SAFE_POINTER_RET( g_pGameIF );
	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RET( pUser );
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_DIGIMONSTATUS )
		|| pUser->GetDigimonState() == CDigimon::DIGIMON_CHANGE_PARTNER_START )
	{
		Close();
		return;
	}

	//등록한 아이템 없음
	if( m_nCapsuleItem == 0 )
		m_pEvolBtn->SetEnable( false );
	else
		m_pEvolBtn->SetEnable( true );

	//윈도우 이동 업데이트
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cCapsuleEvolution::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	//닫기 버튼
	if( m_pCancelBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		Close();
		return muReturn;
	}

	//캡슐 아이템 툴팁
	if( m_nCapsuleItem != 0 )//등록한 아이템 없으면 Pass
	{
		//진화 확인 버튼
		if( m_pEvolBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			if( m_nEvolSlot < 0 || m_nEvolSlot > nLimit::EvoUnit	// 진화슬롯 이상하면 안됨
				|| m_nInvenIdx < 0 || m_nInvenIdx > (int)g_pDataMng->GetInven()->GetInvenSlotCount() )// 인벤 최대치보다 큰 인덱스도 안됨
				return muReturn;

			//올바른 캡슐아이템 확인
			cItemInfo* pItemInfo = g_pDataMng->GetInven()->GetData( m_nInvenIdx );
			CsItem::sINFO* pInfo = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() )->GetInfo();

			CDigimonEvolveObj* pEvolObj = g_pCharMng->GetDigimonUser( 0 )->GetFTEvol()->GetEvolveObjByEvoSlot( m_nEvolSlot );			

			int nTypeL, nTypeS;
			CsItem::TypeS_to_TypeLS( pEvolObj->m_nOpenItemTypeS, nTypeL, nTypeS );

			if( pInfo->s_nType_S != nTypeS || pInfo->s_nType_L != nTypeL)
			{
				//올바른 아이템이 아닙니다.
				return muReturn;
			}
			//이 캡슐아이템 써서 진화 할건지 물어봄
			if( net::game )
				net::game->SendOpenEvoSlot((u2)m_nEvolSlot , (n2)m_nInvenIdx);
			return muReturn;
		}

		if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient() + IF_CAPSULE_EVOL_ITEM_POS , CsSIZE( 32, 32) ) ) != CURSOR::BUTTON_OUTWINDOW )
		{
			if(CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_DOWN )//좌클릭
			{
			}
			else if(CURSOR_ST.GetButtonState() == CURSOR::RBUTTON_DOWN )//우클릭
			{
				//아이템 해제
				m_nCapsuleItem = 0;
				//아이템 락해제
				g_pDataMng->ItemUnlock( m_nInvenIdx );
			}
			else//툴팁
			{
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos() + CsPoint( 20, 20 ) , CsPoint::ZERO, TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_nCapsuleItem, cBaseWindow::WT_INVENTORY );
			}

		}
	}
	//윈도우 이동
#ifdef VERSION_USA
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( GetRoot()->GetSize().x, 50 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
#else
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( GetRoot()->GetSize().x, 30 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
#endif
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

bool cCapsuleEvolution::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			if( nIconCount <= 0 )
				return false;
			else if( IsResist( nIconSlot ) )
				SetResist( nIconSlot, 1 );	

			CURSOR_ST.ReleaseIcon();

			return true;
		}
		break;
	}

	return false;
}
void cCapsuleEvolution::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
	ResetDevice();
}

void cCapsuleEvolution::Render()
{
	RenderScript();
	if( m_nCapsuleItem != 0 )//등록된 아이템 있을 때만
	{
		m_pItemName->Render( GetRootClient() + IF_CAPSULE_EVOL_ITEM_NAME_POS, DT_CENTER );
		g_pIconMng->RenderItem( m_nCapsuleItem, GetRootClient() + IF_CAPSULE_EVOL_ITEM_POS, 1);
	}
}

void cCapsuleEvolution::ResetDevice()
{
	ResetDeviceScript();
}

bool cCapsuleEvolution::OnEscapeKey()
{
	Close();
	return true;
}

bool cCapsuleEvolution::Close( bool bSound /*= true */ )
{
	if( m_nInvenIdx != -1 )
		g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nInvenIdx ) );

	return cBaseWindow::Close();
}

bool cCapsuleEvolution::IsResist( int nInvenIdx )
{

	if( IsEnableWindow() == false )
	{
		cPrintMsg::PrintMsg( 10004 );//지금은 사용할 수 없습니다
		return false;
	}

	cItemInfo* pInfo = g_pDataMng->GetInven()->GetData( nInvenIdx );
#ifdef USE_DUMPER		//chu8820 - 인벤 데이터 로드 실패 ( C004 )
	if( pInfo == NULL )//아이템이 없네.. 인벤번호가 잘못되었거나 인벤데이터 못불러옴
	{
		DUMPLOG( " C004 - 1, %d ", nInvenIdx );
		return false;
	}
	if( nsCsFileTable::g_pItemMng->GetItem( pInfo->GetType() ) == NULL )//아이템 로드 실패
	{
		DUMPLOG( " C004 - 2, %d, %d ", nInvenIdx, pInfo->GetType() );
		return false;
	}
#endif
	// 캡슐진화 아이템 체크
	CsItem::sINFO* pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( pInfo->GetType() )->GetInfo();

	CDigimonEvolveObj* pEvolObj = g_pCharMng->GetDigimonUser( 0 )->GetFTEvol()->GetEvolveObjByEvoSlot( m_nEvolSlot );			

	int nTypeL, nTypeS;
	CsItem::TypeS_to_TypeLS( pEvolObj->m_nOpenItemTypeS, nTypeL, nTypeS );

	if( pFTItemInfo->s_nType_S == nTypeS && pFTItemInfo->s_nType_L == nTypeL)
		return true;
	else
	{
		cPrintMsg::PrintMsg( 30659 );	// 등록할 수 없는 아이템입니다	
		return false;
	}
}

void cCapsuleEvolution::SetResist( int nInvenIdx, int nCount )
{
	// - 아이템 등록
	//등록 중인 아이템이 있다면 락 해제해 주고
	if( m_nInvenIdx != -1 )
		g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nInvenIdx ) );

	//등록한 아이템 인벤에서 락
	g_pDataMng->ItemLock( TO_INVEN_SID( nInvenIdx ) );

	//아이템정보 가져옴
	// 	cItemInfo pItemInfo = *g_pDataMng->GetInven()->GetData( nInvenIdx );
	CsItem::sINFO* pFTItemInfo = g_pDataMng->GetInven()->GetFTData( nInvenIdx );
	if( pFTItemInfo == NULL )//인벤 정보 로드 실패
	{
		g_pDataMng->ItemUnlock( TO_INVEN_SID( nInvenIdx ) );
		return;
	}

	//아이템 이름 셋팅용
	// 	TCHAR		szName[ ITEM_NAME_LEN ];
	// 	cStringList* pStrList = NiNew cStringList;
	cText::sTEXTINFO ti;
	ti.Init();
	ti.s_Color = FONT_GREEN;

	m_nCapsuleItem = pFTItemInfo->s_dwItemID;
	m_nInvenIdx = nInvenIdx;

	ti.SetText( pFTItemInfo->s_szName );

	m_pItemName = NiNew cString;
	m_pItemName->AddText( &ti );
}

void cCapsuleEvolution::_RecvResult( bool bSuccess )
{
	//사용 아이템 감소
	g_pDataMng->GetInven()->GetData( m_nInvenIdx )->DecreaseCount(1);

	//결과가 성공이면 슬롯 뚫어줘. 서버에선 알아서 뚫었음
	if( bSuccess )
	{
		CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );			
		// 진화 플래그 재설정
		cEvoUnit* pEvoUnit = pUser->GetAttributeEvoUnit( m_nEvolSlot );
		pEvoUnit->m_nSlotState = 1;
		pUser->SetEvoPlag( false, m_nEvolSlot, m_nEvolSlot+1 );
		// 퀵에볼
		g_pGameIF->GetQuickEvol()->SetTargetObject( pUser );
		// 디지몬 진화창
		GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_DIGIMON_DIGIMONSTATUS, NULL );


		// 메세지
		// 		CDigimonEvolveObj* pEvoObj = pUser->GetFTEvol()->GetEvolveObjByEvoSlot( m_nEvolSlot );
		// 		CsDigimon::sINFO* pDigimonInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( pEvoObj->m_dwID )->GetInfo();
		// 		cPrintMsg::PrintMsg( 30040, nsCsFileTable::g_pItemMng->GetItem( m_nCapsuleItem )->GetInfo()->s_szName, pDigimonInfo->s_szName );

		//진화 이펙트
		DWORD dwMask = nsEFFECT::POS_CHARPOS | nsEFFECT::DIR_CAMERA_AXIS_Z;
		g_pCharMng->GetTamerUser()->GetProp_Effect()->AddEffect( "system\\Evo_Effect.nif", 1.0f, dwMask );
		g_pCharMng->GetTamerUser()->PlaySound( "game\\Evo_Item.wav" );

		// 도감 정보 갱신 - 부화
		if( m_nEvolSlot < nLimit::EvoUnit )
		{
			GS2C_RECV_ENCYCLOPEDIA_EVOLINFO recv;
			recv.iBaseDigimonID = g_pCharMng->GetDigimonUser(0)->GetBaseDigimonFTID();
			recv.iEvolSlot = m_nEvolSlot;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::ENCYCLOPEDIA_SET_EVOLINFO, &recv );
		}
		Close();
	}

	//결과 받았으니 아이템 빼고
	m_nCapsuleItem = 0;
	//락도 풀어주고
	g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nInvenIdx ) );
	//인벤번호도 초기화
	m_nInvenIdx = -1;
}
