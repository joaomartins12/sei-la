
#include "stdafx.h"
#include "Card_Identify.h"

#define		IF_REGIST_IDENTITY_POS		CsPoint( 85, 104 )
#define		IF_REGIST_SEALING_POS		CsPoint( 201, 104 )
#define IF_INVENTORY_ICON_SIZE CsPoint( 32, 32 )

cCardIdentify::sSLOT::sSLOT()
: s_pRenderText(NULL)
, s_TextPos(-1,-1)
, s_IconPos(-1,-1)
, s_InvenIndex(-1)
{	
	memset(&s_ItemInfo, 0, sizeof(s_ItemInfo));
	memset(&s_IFIcon, 0, sizeof(s_IFIcon));
}
void cCardIdentify::sSLOT::DeleteResource()
{		
	s_IFIcon.ReleaseState();
	s_ItemInfo.Reset();	
	SAFE_NIDELETE(s_pRenderText);
}

void cCardIdentify::sSLOT::InitUI(IN CsPoint RegistPos, IN CsPoint RegistItemPos, IN cWindow* pRoot)
{
	s_TextPos = RegistPos;
	s_IconPos = RegistItemPos;
	s_pWindowRoot = pRoot;

	s_IFIcon.Init( CsRect(RegistItemPos, (CsSIZE)IF_INVENTORY_ICON_SIZE));
}

void cCardIdentify::sSLOT::InitFont(IN CFont::eFACE_SIZE eFontSize, IN cText::sTEXTINFO::eBOLD_LEVEL Level)
{
	s_Font.Init( &g_pEngine->m_FontSystem, eFontSize );		
	s_Font.SetBoldLevel( Level );	
}

void cCardIdentify::sSLOT::SetText(IN TCHAR const* pText)
{
	s_Font.SetText(pText);
	SAFE_NIDELETE(s_pRenderText);
	s_pRenderText = NiNew cText;
	s_pRenderText->Init( s_pWindowRoot, CsPoint::ZERO, &s_Font, false );
}

void cCardIdentify::sSLOT::Render()

{	// 등록된 씰 아이템
	if( s_ItemInfo.IsEnable() )
		g_pIconMng->RenderItem( &s_ItemInfo, s_IconPos + s_pWindowRoot->GetClient(), false );

	// 마스크 렌더링
	if( s_IFIcon.s_eState == sIFIcon::MOUSE_ON )
		s_IFIcon.RenderMask( s_IconPos + s_pWindowRoot->GetClient() , IF_INVENTORY_ICON_SIZE );

	if( s_pRenderText )
		s_pRenderText->Render( s_TextPos, DT_CENTER );
}





cCardIdentify::cCardIdentify()
: m_nNeedUnSealAmount(0)
, m_nHaveUnSealAmount(0)
, m_bIsRegistUnSeal(false)
{
}
void cCardIdentify::Destroy()
{
	cBaseWindow::Delete();
}
void cCardIdentify::DeleteResource()
{
	DeleteScript();
	_ClearItemLock();	// 아이템 락 해제

	m_arrSLOT[SLOT_SEAL].DeleteResource();
	m_arrSLOT[SLOT_UNSEAL].DeleteResource();

	m_nNeedUnSealAmount = 0;
	m_nHaveUnSealAmount = 0;
}

void cCardIdentify::_ClearItemLock()
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

	if( m_arrSLOT[SLOT_UNSEAL].s_ItemInfo.IsEnable() )
	{	// 봉인 해제 아이템의 락 해제
		uint nSealItemID = m_arrSLOT[SLOT_UNSEAL].s_ItemInfo.m_nType;
		CsItem FTItem = *nsCsFileTable::g_pItemMng->GetItem(nSealItemID);
		CsItem::sINFO FTInfo = *(FTItem.GetInfo());

		g_pDataMng->GetInven()->ItemUnlock_ItemType(FTInfo.s_nType_L, 1);
	}

	g_pDataMng->ReleaseItemLock();
}


void cCardIdentify::Create( int nValue )
{
	cBaseWindow::Init();

	int nPosX = ( g_nScreenWidth/2 ) /*- ( 317/2 )*/;
	InitScript( "Accessory\\Enchant\\Enchant_BG.tga",  CsPoint( nPosX, g_nScreenHeight/2 - ( 247/2 ) ) , CsPoint( 317, 247 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );

	m_pCancelBtn = AddButton( CsPoint( 290, 12 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pEnchantBtn = AddButton( CsPoint( 197, 190 ), CsPoint( 76, 26 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );	
	
	m_arrSLOT[SLOT_SEAL].InitUI( CsPoint(99, 145), CsPoint(85, 104), m_pRoot);
	m_arrSLOT[SLOT_SEAL].InitFont( CFont::FS_10, cText::sTEXTINFO::BL_3 );

	m_arrSLOT[SLOT_UNSEAL].InitUI( CsPoint(215, 145), CsPoint(201, 104), m_pRoot);
	m_arrSLOT[SLOT_UNSEAL].InitFont( CFont::FS_10, cText::sTEXTINFO::BL_3 );

	SetResist(nValue);		// 씰 아이템 자동 등록
	SetTitleText(nValue);
	SetUnSealItem();
}
void cCardIdentify::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();		// UI 드래그 이동 가능
}

cBaseWindow::eMU_TYPE cCardIdentify::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( m_pCancelBtn->Update_ForMouse() == cButton::ACTION_CLICK ) 	//닫기 버튼
	{
		Close();
		return muReturn;
	}

	if( m_pEnchantBtn->Update_ForMouse() == cButton::ACTION_CLICK ) // 확인 버튼
	{
		if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::CASH_IDENTIFY_CARD ) == false )
			return muReturn;


		if( m_bIsRegistUnSeal == false )	// 봉인 해제 아이템 등록되지 않았거나 수량 부족
		{
			CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_arrSLOT[SLOT_UNSEAL].s_ItemInfo.m_nType );	// 봉인 해제 아이템 INDEX
			CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
			cPrintMsg::PrintMsg(30650, pFTInfo->s_szName);	// 봉인 해제 아이템이 부족합니다.
			return muReturn;
		}

		// 아이템이 모두 정상 등록
		if( m_arrSLOT[SLOT_SEAL].s_ItemInfo.IsEnable() && m_arrSLOT[SLOT_UNSEAL].s_ItemInfo.IsEnable() )
		{
			if( IsEnableWindow() == false )
			{
				cPrintMsg::PrintMsg( 10004 );	// 지금은 사용할 수 없습니다.
				return muReturn;
			}

			if( m_nNeedUnSealAmount < 0 )
			{
				ST_CHAT_PROTOCOL	CProtocol;
				CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
				CProtocol.m_wStr = _T( "아이템이 부족합니다." );
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
				CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_arrSLOT[SLOT_UNSEAL].s_ItemInfo.m_nType );
				CsItem::sINFO* pFTInfo = pFTItem->GetInfo();

				cPrintMsg::PrintMsg( 30650, pFTInfo->s_szName );

				return muReturn;
			}
			//아이템 ID와 수량
			std::pair<int, int> openSealInfo;// = make_pair(m_arrSLOT[SLOT_SEAL].s_ItemInfo.m_nType, m_arrSLOT[SLOT_SEAL].s_ItemInfo.GetCount());
			openSealInfo.first = m_arrSLOT[SLOT_SEAL].s_ItemInfo.m_nType;
			openSealInfo.second = m_arrSLOT[SLOT_SEAL].s_ItemInfo.GetCount();
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::CHECK_SEALMASTER_UNSEAL, &openSealInfo);
		}

		return muReturn;
	}

	if( _UpdateIcon_ForMouse() == true ) // 툴팁 체크
		return muReturn;

	// 드래그
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( GetRoot()->GetSize().x, 30 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();
	return muReturn;
}

bool cCardIdentify::_UpdateIcon_ForMouse()
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
					cBaseWindow::WT_CARDIDENTIFY,		// 중복 툴팁을 방지하는 Key
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

void cCardIdentify::Render()
{
	// UI 타이틀
	RenderScript();

	m_arrSLOT[SLOT_SEAL].Render();
	m_arrSLOT[SLOT_UNSEAL].Render();
}

void cCardIdentify::ResetDevice()
{
	ResetDeviceScript();

#ifndef CRASHDUMP_0305
	// ResetDevieScript() 함수에서 리셋 하므로 필요없음	lks007
	if(m_pCancelBtn)
		m_pCancelBtn->ResetDevice();

	if(m_pEnchantBtn)
		m_pEnchantBtn->ResetDevice();
#endif

	for(int i = 0 ; i < SLOT_MAX; ++i)
	{
		if(m_arrSLOT[i].s_pRenderText)
			m_arrSLOT[i].s_pRenderText->ResetDevice();
	}
}

bool cCardIdentify::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
// 			if( IsResist( nIconSlot ) == true )
// 				SetResist( nIconSlot );
// 
// 			CURSOR_ST.ReleaseIcon();

			return true;
		}
		break;
	}

	return false;
}
// 봉인 해제가 가능한지 여부를 판단한다.
int cCardIdentify::IsEnableUnseal()
{
	cPrintMsg::PrintMsg(30651);
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = _T("같은 종류의 씰은 최대 3000개까지만 등록할 수 있습니다.");
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
	return MSG_HAVEOVERSEAL;
}

void cCardIdentify::SetResist( int nInvenIndex )
{
	cItemInfo		rItem;
	CsItem			rFTItem;
	CsItem::sINFO	rFTInfo;
	GetINFO(nInvenIndex, rFTItem, rFTInfo, rItem);

	switch(rFTInfo.s_nType_L)
	{
	case nItem::CardMaster:
		{
			m_arrSLOT[SLOT_SEAL].s_ItemInfo = rItem;
			m_nCardIndex = nInvenIndex;				// 사용하려는 씰의 인벤토리 인덱스 저장
			g_pDataMng->GetInven()->ItemLock_ItemType( rFTInfo.s_nType_L, 0 );
		}
		break;

	case nItem::UnSealCardMaster:
		{
			m_arrSLOT[SLOT_UNSEAL].s_ItemInfo = rItem;
			g_pDataMng->GetInven()->ItemLock_ItemType( rFTInfo.s_nType_L, 1 );
			SetUnSealText(nInvenIndex);
		}
		break;
	}

	cWindow::PlaySound( "System\\interface_person_01.wav" );
}

void cCardIdentify::OnMoveWindow()
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

void cCardIdentify::SetTitleText( int nInvenIndex )
{
	cItemInfo		rItem;
	CsItem			rFTItem;
	CsItem::sINFO	rFTInfo;
	GetINFO(nInvenIndex, rFTItem, rFTInfo, rItem);

	int				nCount	= rItem.GetCount();
	
	{	// UI Title 등록
		std::wstring wsTitle;
		DmCS::StringFn::Format( wsTitle, L"%s %s", rFTInfo.s_szName, UISTRING_TEXT( "CARDRESEAL_TITLE_OPENED" ).c_str() );
		AddTitle( wsTitle.c_str(), CsPoint(0, 4) );
	}

// 	cText::sTEXTINFO ti;
// 	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );		
// 	ti.SetBoldLevel( cText::sTEXTINFO::BL_3 );					


	{	// 보유한 Seal 아이템 개수
		std::wstring wsCount;
		DmCS::StringFn::Format( wsCount, L"%d%s", nCount, UISTRING_TEXT( "COMMON_TXT_UNIT" ).c_str() );
		m_arrSLOT[SLOT_SEAL].SetText( wsCount.c_str() );
	}

	// UnSeal Item 
	int nNeed = nCount / 50;

	// nCount가 270이었다면, nNeed * 50은 250이 되므로 무조건 올림 처리를 하여 + 1이 된다.
	if( (nNeed * 50) < nCount )
		++nNeed;	// 50으로 나누어 떨어지지 않으면 무조건 올림 처리

	m_nNeedUnSealAmount = nNeed;	// 우측 하단의 "필요 개수 n개"에 표기할 변수입니다.
}

void cCardIdentify::SetUnSealText( int nInvenIndex )
{
	cItemInfo					rItem;
	CsItem						rFTItem;
	CsItem::sINFO				rFTInfo;
	GetINFO(nInvenIndex, rFTItem, rFTInfo, rItem);

	int				nCount	= g_pDataMng->GetInven()->GetCount_Item_TypeL(rFTInfo.s_nType_L);
	m_nHaveUnSealAmount = nCount;		// 현재 보유한 봉인해제 아이템 총 수량을 보관합니다.

	int nTextCount = m_nNeedUnSealAmount;		// 필요 개수 보관 
	if( m_nNeedUnSealAmount > nCount)
	{
		m_arrSLOT[SLOT_UNSEAL].s_Font.s_Color = FONT_RED;
		m_nNeedUnSealAmount = -1;		// 확인 버튼을 누를 수 없도록 -1로 지정해줍니다.	
		
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		CProtocol.m_wStr = _T( "봉인 해제 아이템 수량이 부족합니다." );
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
	}

	{
		std::wstring wsCount;
		DmCS::StringFn::Format( wsCount, L"%s %d%s", 
			UISTRING_TEXT( "CARDIDENTIFY_REQUIRED_AMOUNT" ).c_str(), 
			nTextCount, UISTRING_TEXT( "COMMON_TXT_UNIT" ).c_str() );
		m_arrSLOT[SLOT_UNSEAL].SetText( wsCount.c_str() );
	}
}

void cCardIdentify::SetUnSealItem()
{
	// 인벤토리에 봉인 해제 아이템이 있는지 찾습니다.
	int nUnSealInvenIdx = g_pDataMng->GetInven()->GetFirstSlot_Item_TypeLS( nItem::UnSealCardMaster , 1 );

	// 봉인 해제 아이템이 있으면 그 아이템을 등록합니다.
	if( nUnSealInvenIdx != cData_Inven::INVALIDE_INVEN_INDEX )
	{
		m_bIsRegistUnSeal = true;
		SetResist(nUnSealInvenIdx);
		return;
	}

	// 없으면, 보여주기 용으로 아이템을 등록만 해줍니다.

	int nDispItemID = nsCsFileTable::g_pItemMng->TypeLS_to_Disp( nItem::UnSealCardMaster, 1 );
	assert_csm1( nDispItemID, "Item_Type : %d은 소분류 대표 아이템에서 찾을 수 없습니다.", nItem::UnSealCardMaster );

	m_arrSLOT[SLOT_UNSEAL].s_ItemInfo.Set(nDispItemID, 1);

	m_bIsRegistUnSeal = false;
	m_arrSLOT[SLOT_UNSEAL].s_Font.s_Color = FONT_RED;

	std::wstring wsCount;
	DmCS::StringFn::Format( wsCount, L"%s %d%s", 
		UISTRING_TEXT( "CARDIDENTIFY_REQUIRED_AMOUNT" ).c_str(), 
		m_nNeedUnSealAmount, UISTRING_TEXT( "COMMON_TXT_UNIT" ).c_str() );
	m_arrSLOT[SLOT_UNSEAL].SetText( wsCount.c_str() );
}

bool cCardIdentify::Close( bool bSound /*= true */ )
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

void cCardIdentify::SendUnSeal()
{
	int nValue1 = m_nCardIndex;			// 씰 위치
	int nValue2 = m_arrSLOT[SLOT_SEAL].s_ItemInfo.GetCount();	// 씰 사용량
	int nValue3 = m_nNeedUnSealAmount;	// 봉인 해제 사용량

	g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CASH_IDENTIFY_CARD, 0, &nValue1, &nValue2, &nValue3 );	
}

void cCardIdentify::GetINFO( IN int nInvenIndex, OUT CsItem& FTItem, OUT CsItem::sINFO& FTInfo, OUT cItemInfo& ItemINFO )
{
	ItemINFO	= *g_pDataMng->GetInven()->GetData( nInvenIndex );
	FTItem	= *nsCsFileTable::g_pItemMng->GetItem( ItemINFO.m_nType );
	FTInfo	= *FTItem.GetInfo();
}

void cCardIdentify::RecvUnSealProcess( int nSealInvenIdx, int nSealUse, int nUnSealUse )
{
	assert_cs(nSealInvenIdx == m_nCardIndex);
	assert_cs(m_arrSLOT[SLOT_SEAL].s_ItemInfo.GetCount() == nSealUse);		// 씰 사용량 체크

	// 1. 봉인 해제 아이템을 차감합니다.(data)
	g_pDataMng->GetInven()->DecreaseItem_TypeLS(nItem::UnSealCardMaster, 1, nUnSealUse, false, false);

	{
		CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_arrSLOT[SLOT_SEAL].s_ItemInfo.m_nType );
		CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
		cPrintMsg::PrintMsg( 30652, pFTInfo->s_szName );
		
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString(_T( "%s이 적용되었습니다.") , pFTInfo->s_szName);
		DmCS::StringFn::Format(CProtocol.m_wStr, _T( "%s이 적용되었습니다.") , pFTInfo->s_szName); 
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
	}	

	// 2. 씰 아이템 모두 차감
	cItemInfo* pItem = g_pDataMng->GetInven()->GetData( nSealInvenIdx );
	pItem->DecreaseCount(nSealUse);
	
	// 3. 씰 마스터 데이터를 수정합니다.(data)
	GS2C_RECV_SEALMASTER_OPENSEAL recv;
	recv.m_nSealItemID =  m_arrSLOT[SLOT_SEAL].s_ItemInfo.m_nType;
	recv.m_nSealAmount = nSealUse;			// 씰 사용량
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_SEALOPEN, &recv );
}