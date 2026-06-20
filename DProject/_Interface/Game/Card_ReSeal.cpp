#include "StdAfx.h"
#include "Card_ReSeal.h"

#define		IF_REGIST_IDENTITY_POS		CsPoint( 85, 104 )
#define		IF_REGIST_SEALING_POS		CsPoint( 201, 104 )
#define		IF_INVENTORY_ICON_SIZE CsPoint( 32, 32 )

cCardReSeal::sSLOT::sSLOT()
: s_pRenderText(NULL)
, s_TextPos(-1,-1)
, s_IconPos(-1,-1)
, s_InvenIndex(-1)
{	
	memset(&s_ItemInfo, 0, sizeof(s_ItemInfo));
	memset(&s_IFIcon, 0, sizeof(s_IFIcon));
}
void cCardReSeal::sSLOT::DeleteResource()
{		
	s_IFIcon.ReleaseState();
	s_ItemInfo.Reset();	
	SAFE_NIDELETE(s_pRenderText);
}

void cCardReSeal::sSLOT::InitUI(IN CsPoint RegistPos, IN CsPoint RegistItemPos, IN cWindow* pRoot)
{
	s_TextPos = RegistPos;
	s_IconPos = RegistItemPos;
	s_pWindowRoot = pRoot;

	s_IFIcon.Init( CsRect(RegistItemPos, (CsSIZE)IF_INVENTORY_ICON_SIZE));
}

void cCardReSeal::sSLOT::InitFont(IN CFont::eFACE_SIZE eFontSize, IN cText::sTEXTINFO::eBOLD_LEVEL Level)
{
	s_Font.Init( &g_pEngine->m_FontSystem, eFontSize );		
	s_Font.SetBoldLevel( Level );	
}

void cCardReSeal::sSLOT::SetText(TCHAR const* pText)
{
	s_Font.SetText(pText);
	SAFE_NIDELETE(s_pRenderText);
	s_pRenderText = NiNew cText;
	s_pRenderText->Init( s_pWindowRoot, CsPoint::ZERO, &s_Font, false );
}

void cCardReSeal::sSLOT::Render()

{	// 등록된 씰 아이템
	if( s_ItemInfo.IsEnable() )
		g_pIconMng->RenderItem( &s_ItemInfo, s_IconPos + s_pWindowRoot->GetClient(), false );

	// 마스크 렌더링
	if( s_IFIcon.s_eState == sIFIcon::MOUSE_ON )
		s_IFIcon.RenderMask( s_IconPos + s_pWindowRoot->GetClient() , IF_INVENTORY_ICON_SIZE );

	if( s_pRenderText )
		s_pRenderText->Render( s_TextPos, DT_CENTER );
}





cCardReSeal::cCardReSeal(void)
: m_pCancelBtn(NULL)
, m_pEnchantBtn(NULL)
, m_nNeedReSeal(0)
{
}

cCardReSeal::~cCardReSeal(void)
{
}

void cCardReSeal::Destroy()
{
	cBaseWindow::Delete();
}

void cCardReSeal::DeleteResource()
{
	DeleteScript();

	_ClearItemLock();	// 아이템 락 해제

	m_arrSLOT[0].DeleteResource();
	m_arrSLOT[1].DeleteResource();

	g_pDataMng->ReleaseItemLock();
}

void cCardReSeal::_ClearItemLock()
{
	if( !g_pDataMng )
		return;

	if( m_arrSLOT[SLOT_SEAL].s_ItemInfo.IsEnable() )
	{	// 씰 아이템의 락 해제
		uint nSealItemID = m_arrSLOT[SLOT_SEAL].s_ItemInfo.m_nType;
		CsItem FTItem = *nsCsFileTable::g_pItemMng->GetItem(nSealItemID);
		CsItem::sINFO FTInfo = *(FTItem.GetInfo());

		g_pDataMng->GetInven()->ItemUnlock_ItemType(FTInfo.s_nType_L, 0);
	}

	if( m_arrSLOT[SLOT_RESEAL].s_ItemInfo.IsEnable() )
	{	// 밀봉 아이템의 락 해제
		uint nSealItemID = m_arrSLOT[SLOT_RESEAL].s_ItemInfo.m_nType;
		CsItem FTItem = *nsCsFileTable::g_pItemMng->GetItem(nSealItemID);
		CsItem::sINFO FTInfo = *(FTItem.GetInfo());

		g_pDataMng->GetInven()->ItemUnlock_ItemType(FTInfo.s_nType_L, 1);
	}

	g_pDataMng->ReleaseItemLock();
}

void cCardReSeal::Create( int nValue /*= 0 */ )
{
	cBaseWindow::Init();

	int nPosX = ( g_nScreenWidth/2 ) /*- ( 317/2 )*/;
	InitScript( "Accessory\\Enchant\\Enchant_BG.tga",  CsPoint( nPosX, g_nScreenHeight/2 - ( 247/2 ) ) , CsPoint( 317, 247 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );

	m_pCancelBtn = AddButton( CsPoint( 290, 12 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pEnchantBtn = AddButton( CsPoint( 197, 190 ), CsPoint( 76, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );	

	m_arrSLOT[SLOT_SEAL].InitUI( CsPoint(99, 145), CsPoint(85, 104), m_pRoot);
	m_arrSLOT[SLOT_SEAL].InitFont( CFont::FS_10, cText::sTEXTINFO::BL_3 );

	m_arrSLOT[SLOT_RESEAL].InitUI( CsPoint(215, 145), CsPoint(201, 104), m_pRoot);
	m_arrSLOT[SLOT_RESEAL].InitFont( CFont::FS_10, cText::sTEXTINFO::BL_3 );
}

void cCardReSeal::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();		// UI 드래그 이동 가능
}

cBaseWindow::eMU_TYPE cCardReSeal::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cBaseWindow::Update_ForMouse();

	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	//닫기 버튼
	if( m_pCancelBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		Close();
		return muReturn;
	}


	// 확인 버튼(밀봉)
	if( m_pEnchantBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::CASH_CARD_RESEAL ) == false )
			return muReturn;

		// 수량 부족 시 메시지 박스 팝업하지 않고 선 처리
		if( m_bIsRegistReSeal == false 
			|| m_nHaveReSeal < m_nNeedReSeal)	// 밀봉 아이템 수량 부족
		{
			CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_arrSLOT[SLOT_RESEAL].s_ItemInfo.m_nType );	// 밀봉 아이템 INDEX
			CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
			cPrintMsg::PrintMsg(30650, pFTInfo->s_szName);	// 밀봉 아이템이 부족합니다.

			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			CProtocol.m_wStr = _T( "밀봉 아이템의 수량이 부족합니다." );
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
			return muReturn;
		}


		
		if( m_arrSLOT[SLOT_SEAL].s_ItemInfo.IsEnable() && 
			m_arrSLOT[SLOT_RESEAL].s_ItemInfo.IsEnable() &&
			m_nHaveReSeal >= m_nNeedReSeal )	// 필요 개수 이상으로 필요함
		{
			CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_arrSLOT[SLOT_SEAL].s_ItemInfo.m_nType );	// 씰 아이템
			CsItem::sINFO* pFTInfo = pFTItem->GetInfo();

			// 밀봉 하시겠습니까?
			cPrintMsg::PrintMsg(30657, pFTInfo->s_szName);
		}

		return muReturn;
	}

	// 툴팁 체크
	if( _UpdateIcon_ForMouse() == true )
		return muReturn;

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( GetRoot()->GetSize().x, 30 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

bool cCardReSeal::_UpdateIcon_ForMouse()
{
	bool bCheck = false;

	for(int i = SLOT_SEAL; i < SLOT_MAX; ++i)
	{
		if( m_arrSLOT[i].s_ItemInfo.IsEnable() )
		{
			if( m_arrSLOT[i].s_IFIcon.s_Rect.PtInRect( MousePosToWindowPos( CURSOR_ST.GetPos()) ) )
			{
				cTooltip::eSLOT eSlot = cTooltip::CLOSE_SLOT;	// 슬롯 타입(사용 안하는 듯)
				CsPoint pos = m_arrSLOT[i].s_IFIcon.GetPos() + GetRootClient();	// 툴팁이 시작되는 좌표

				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, 
					IF_INVENTORY_ICON_SIZE,				// 아이콘 크기
					TOOLTIP_MAX_SIZE,					// 툴팁 길이
					cTooltip::ITEM,						// 툴팁 유형
					m_arrSLOT[i].s_ItemInfo.m_nType,	// 아이템 ID
					cBaseWindow::WT_CARDRESEAL,			// 중복 툴팁을 방지하는 Key
					eSlot, 0, 
					&m_arrSLOT[i].s_ItemInfo			// ItemInfo
					);	

				bCheck = true;
				m_arrSLOT[i].s_IFIcon.s_eState = sIFIcon::MOUSE_ON;
			}

			else	// 마우스 온 상태가 아님.
			{
				m_arrSLOT[i].s_IFIcon.s_eState = sIFIcon::NONE;
			}
		}
	}

	return bCheck;	
}
void cCardReSeal::Render()
{
	// UI 타이틀
	RenderScript();

	m_arrSLOT[SLOT_SEAL].Render();
	m_arrSLOT[SLOT_RESEAL].Render();
}

void cCardReSeal::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();

	if(m_pCancelBtn)
		m_pCancelBtn->ResetDevice();

	if(m_pEnchantBtn)
		m_pEnchantBtn->ResetDevice();

	for(int i = 0 ; i < SLOT_MAX; ++i)
	{
		if(m_arrSLOT[i].s_pRenderText)
			m_arrSLOT[i].s_pRenderText->ResetDevice();
	}
}

void cCardReSeal::ResetDevice()
{
	ResetDeviceScript();

	if(m_pCancelBtn)
		m_pCancelBtn->ResetDevice();

	if(m_pEnchantBtn)
		m_pEnchantBtn->ResetDevice();

	for(int i = 0 ; i < SLOT_MAX; ++i)
	{
		if(m_arrSLOT[i].s_pRenderText)
			m_arrSLOT[i].s_pRenderText->ResetDevice();
	}
}

bool cCardReSeal::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	return false;
}

bool cCardReSeal::Close( bool bSound /*= true */ )
{
	if( IsEnableWindow() == false )
		return false;

	if( bSound )
	{
		PlayCloseSound();
	}

	DeleteResource();
	SetShowWindow( false );

	return true;
}



void cCardReSeal::SetReSealText(int nSealItemID, int nAmount)
{
	_SetTitle(nSealItemID);				// 제목 설정
	_SetSealText(nAmount);				// "%d"
	_SetSealIcon(nSealItemID, nAmount);	// 씰 아이콘 등록

	{	// 필요 개수 
		int nNeed = nAmount / 50;

		if( (nNeed * 50) < nAmount )
			++nNeed;

		m_nNeedReSeal = nNeed;	
	}

	_SetReSealItem();					// 밀봉 아이템 등록
	cWindow::PlaySound( "System\\interface_person_01.wav" );
}



void cCardReSeal::SendReSeal()
{
	if( g_pDataMng->GetInven()->IsSlot() == false )	// 빈슬롯이 없다면,
	{
		cPrintMsg::PrintMsg(11015);		// 인벤토리에 여유 공간이 부족합니다.
		return;
	}


	int nSealItemID = m_arrSLOT[SLOT_SEAL].s_ItemInfo.m_nType;	// 선택 카드 아이템ID
	int nCardID = nsCsFileTable::g_pMaster_CardMng->GetMasterCard(nSealItemID)->GetInfo()->s_nID;	// 카드ID

	//CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_ResistStone.m_nType )->GetInfo();
	int nValue1 = nSealItemID;									// 카드 아이템ID
	int nValue2 = m_arrSLOT[SLOT_SEAL].s_ItemInfo.GetCount();	// 카드 밀봉량
	int nValue3 = m_nNeedReSeal;								// 밀봉 아이템 소모량

	g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CASH_CARD_RESEAL, 0, &nValue1, &nValue2, &nValue3);	

}

void cCardReSeal::GetINFO( IN int nInvenIndex, OUT CsItem& FTItem, OUT CsItem::sINFO& FTInfo, OUT cItemInfo& ItemINFO )
{
	ItemINFO	= *g_pDataMng->GetInven()->GetData( nInvenIndex );
	FTItem	= *nsCsFileTable::g_pItemMng->GetItem( ItemINFO.m_nType );
	FTInfo	= *FTItem.GetInfo();
}

void cCardReSeal::_SetTitle( int nSealItemID )
{
	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nSealItemID );
	SAFE_POINTER_RET( pFTItem );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RET( pFTInfo );

	std::wstring wsTitle;
	DmCS::StringFn::Format( wsTitle, L"%s %s", pFTInfo->s_szName, UISTRING_TEXT( "CARDRESEAL_TITLE_SEAL" ).c_str() );
	AddTitle( wsTitle.c_str(), CsPoint(0, 4) );
}

void cCardReSeal::_SetSealText( int nAmount )
{
	std::wstring wsCount;
	DmCS::StringFn::Format( wsCount, L"%d %s", nAmount, UISTRING_TEXT( "COMMON_TXT_UNIT" ).c_str() );
	m_arrSLOT[SLOT_SEAL].SetText( wsCount.c_str() );
}

void cCardReSeal::_SetSealIcon( int nSealItemID, int nAmount )
{
	cItemInfo _ItemInfo;
	_ItemInfo.Set(nSealItemID, nAmount);		// 씰 아이템 등록

	m_arrSLOT[SLOT_SEAL].s_ItemInfo = _ItemInfo;
	g_pDataMng->GetInven()->ItemLock_ItemType( nItem::CardMaster, 0 );			// 씰 아이템 Lock
}

void cCardReSeal::_SetReSealItem()
{
	// 인벤토리에 밀봉 아이템이 있는지 찾습니다.
	int nReSealInvenIdx = g_pDataMng->GetInven()->GetFirstSlot_Item_TypeLS( nItem::ReSealCardMaster , 1 );

	if( nReSealInvenIdx != cData_Inven::INVALIDE_INVEN_INDEX )	// 인벤토리에 밀봉 아이템이 있을 때
	{
		m_bIsRegistReSeal = true;

		cItemInfo		rItem;
		CsItem			rFTItem;
		CsItem::sINFO	rFTInfo;

		GetINFO(nReSealInvenIdx, rFTItem, rFTInfo, rItem);
		m_arrSLOT[SLOT_RESEAL].s_ItemInfo = rItem;
		g_pDataMng->GetInven()->ItemLock_ItemType( rFTInfo.s_nType_L, 1 );

		int nCount	= g_pDataMng->GetInven()->GetCount_Item_TypeL(rFTInfo.s_nType_L);
		if( nCount < static_cast<uint>(m_nNeedReSeal) )	// 수량 부족 > Font Red
			m_arrSLOT[SLOT_RESEAL].s_Font.s_Color = FONT_RED;

		m_nHaveReSeal = nCount;
	}

	else
	{	// 밀봉 아이템이 없을 때에도 등록해줍니다.
		int nDispItemID = nsCsFileTable::g_pItemMng->TypeLS_to_Disp( nItem::ReSealCardMaster, 1 );	// 소분류에 등록된 아이템ID 가져오기
		assert_csm1( nDispItemID, "Item_Type : %d은 소분류 대표 아이템에서 찾을 수 없습니다.", nItem::ReSealCardMaster );

		m_arrSLOT[SLOT_RESEAL].s_Font.s_Color = FONT_RED;
		m_arrSLOT[SLOT_RESEAL].s_ItemInfo.Set(nDispItemID, 1);
		g_pDataMng->GetInven()->ItemLock_ItemType( nItem::ReSealCardMaster, 1 );

		m_bIsRegistReSeal = false;
	}

	int nDispItemID = nsCsFileTable::g_pItemMng->TypeLS_to_Disp( nItem::ReSealCardMaster, 1 );

	std::wstring wsCount;
	DmCS::StringFn::Format( wsCount, L"%s %d%s", 
		UISTRING_TEXT( "CARDIDENTIFY_REQUIRED_AMOUNT" ).c_str(), 
		m_nNeedReSeal, UISTRING_TEXT( "COMMON_TXT_UNIT" ).c_str() );
	m_arrSLOT[SLOT_RESEAL].SetText( wsCount.c_str() );
}

void cCardReSeal::RecvReSealProcess( int nSealID, int nSealAmount, int nReSealUse )
{
	// 1. 밀봉 아이템을 차감합니다.(data)
	g_pDataMng->GetInven()->DecreaseItem_TypeLS(nItem::ReSealCardMaster, 1, nReSealUse, false, false);

	// 2. 카드를 소모시킵니다.(data)
	GS2C_RECV_SEALMASTER_RESEAL recv;
	recv.m_nSealItemID = nSealID;
	recv.m_nSealAmount = nSealAmount;

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_RESEALING, &recv );

	{
		CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nSealID );	// 씰 아이템 INDEX
		CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
		cPrintMsg::PrintMsg( 30655, pFTInfo->s_szName );

		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString(_T( "%s이 밀봉되었습니다.") , pFTInfo->s_szName);
		DmCS::StringFn::Format(CProtocol.m_wStr, _T( "%s이 밀봉되었습니다.") , pFTInfo->s_szName);
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
	}	

	// 3. 씰 아이템을 획득합니다.(data)
	g_pDataMng->GetInven()->ItemCrop(nSealID, nSealAmount);
}
