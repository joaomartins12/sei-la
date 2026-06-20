
#include "stdafx.h"
#include "Talk.h"

#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"

#define TALK_ICON_SIZE				CsPoint( 22, 22 )

#define IF_TALK_STRLIST_POS		CsPoint( 16, 59 )

cTalk::cTalk() :
m_pSelMask(NULL),
m_pBGWindow(NULL),
m_pCancelButton(NULL),
m_pName(NULL),	
m_pScrollBar(NULL),
m_pSelectedString(NULL),
m_pTarget(NULL),
m_eNpcType(nsCsNpcTable::NT_NONE),
m_bRenderSelMark(false),
m_bClickMask(false),
m_bUseTamerDistCancel(false)
{
	memset( m_nPortalDisableID, 0, sizeof(m_nPortalDisableID) );
	memset( m_nPortalDisableCount, 0, sizeof(m_nPortalDisableCount) );
	for( int i = 0; i < 10; ++i )
		memset( m_szPortalDisableDiscript[i], 0, sizeof(TCHAR)*64 );
}

cTalk::~cTalk()
{
	memset( m_nPortalDisableID, 0, sizeof(m_nPortalDisableID) );
	memset( m_nPortalDisableCount, 0, sizeof(m_nPortalDisableCount) );
	for( int i = 0; i < 10; ++i )
		memset( m_szPortalDisableDiscript[i], 0, sizeof(TCHAR)*64 );
}

void cTalk::Destroy()
{
	if( g_pServerMoveOwner )
		g_pServerMoveOwner->SetMoveLock( 0 );

	//2017-01-10-nova 대화창 종료시 비활성화된 인벤토리 활성화 (#4308)

	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_COMP ) && 
		!g_pGameIF->IsActiveWindow( cBaseWindow::WT_SPIRIT_EVOLUTION ) )	// 2017-01-11-nova 퀘스트 완료창이 있을경우는 인벤토리를 비활성화 상태로 둔다
	{
		bool bIsEnable = true;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable );
	}

	cBaseWindow::Delete(); 
}

void cTalk::DeleteResource()
{
	// 이름 짓는 윈도우 닫아준다.
	cCreateName::DeleteInstance( cBaseWindow::WT_CREATE_GUILD_NAME, 0 );
	
	m_pTarget = NULL;
	DeleteScript();

	m_StringList.Delete();
	SAFE_NIDELETE( m_pSelMask );
	SAFE_NIDELETE( m_pBGWindow );
}

void cTalk::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	m_pTarget = NULL;

	InitScript( NULL, CsPoint( 272, 11 ), CsPoint( 558, 236 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::CENTER );

	m_pBGWindow = NiNew cPopUp;
	m_pBGWindow->Init( m_pRoot, CsPoint::ZERO, CsPoint( 558, 236 ), CsPoint( 32, 32 ), "Control_G\\Popup\\WDlg", "tga", false );

	m_pCancelButton = AddButton( CsPoint( 520, 20 ), CsPoint( 19, 19 ), CsPoint( 0, 19 ), "QuestRev\\Quest_X_But.tga" );

	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_2, CsPoint( 521, 60 ), CsPoint( 16, 153 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 11, 60 ), CsPoint( 505, 230 ) ), 6 );

	m_pSelMask = NiNew cSprite;
	m_pSelMask->Init( NULL, CsPoint::ZERO, CsPoint( 500, 22 ), "Talk\\Common_Seletbar.tga", false );

	//제목 위치
	CsPoint ptNamePos = CsPoint( 40, 10 );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_20, NiColor( 0.2f, 0.6f, 0.8f ) );	
	ti.s_bOutLine = false;
	m_pName = AddText( &ti, ptNamePos );

	m_pScrollBar->SetEnableRenderFromEnableScroll( true );


	m_bUseTamerDistCancel = true;


	//길드 이름짓기창이 있으면 닫는다.
	cCreateName::DeleteInstance( cBaseWindow::WT_CREATE_GUILD_NAME, 0 );

	//2017-01-10-nova 대화창 생성시 활성화된 인벤토리 비활성화 (#4308)
	bool bIsEnable = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable );

	SAFE_POINTER_RET( g_pServerMoveOwner );
	g_pServerMoveOwner->SetMoveLock( CUserServerMove::eKeyboardLock | CUserServerMove::eMouseLock );
}

bool cTalk::OnEscapeKey()
{ 
// #ifdef CRASHDUMP_0305
// 	DUMPLOG("04.talk001] t : %d", m_eNpcType);
// #endif

	if( m_pTarget )
	{
		CsC_AniProp* pAni = m_pTarget->GetProp_Animation();
		if( pAni && pAni->IsSequence( ANI::NPC_CLOSE ) )
			m_pTarget->SetAnimation( ANI::NPC_CLOSE );
	}

	Close(); 
	return true; 
}

bool cTalk::OnMacroKey(const MSG& p_kMsg)
{
	switch( p_kMsg.message )
	{
	case WM_KEYDOWN:	// Enter, Esc 는 여기로 안 들어옴
		{
			uint nVkCode = p_kMsg.wParam;
			// 한글은 vk_processkey 가 넘어옴
			if( p_kMsg.wParam == VK_PROCESSKEY )
			{
				// scan code 를 이용해 vk_code 얻어올 수 있음
				uint nScanCode = (p_kMsg.lParam >> 16) & 0xff;
				nVkCode = ::MapVirtualKey( nScanCode, 3/*MAPVK_VSC_TO_VK_EX*/ );
			}
			return _CheckInputKey( nVkCode );
		}
		break;
	}

	return false;
}

bool cTalk::_CheckInputKey(const WPARAM& wParam)
{
	if( wParam == DMKEYBOARD::KEY_W )
	{
		return _SelectList( true );
	}
	else if( wParam == DMKEYBOARD::KEY_S )
	{
		return _SelectList( false );
	}
	else if( GLOBALINPUT_ST.GetEqualSnKey( wParam, DMKEY::NPC_ACTION ) )
	{
		SAFE_POINTER_RETVAL( m_pSelectedString, false );
		m_bRenderSelMark = false;
		_SelectedStringList( m_pSelectedString, m_pSelectedString->GetValue1(), m_pSelectedString->GetValue2() );
		return true;
	}

	return false;
}

void cTalk::Update(float const& fDeltaTime)
{
	if( m_eNpcType == nsCsNpcTable::NT_GUILD )
		cCreateName::UpdateInstance( cBaseWindow::WT_CREATE_GUILD_NAME, 0 );

	g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_TALK );

	if( m_bUseTamerDistCancel && m_pTarget )
	{
		assert_cs( m_pTarget->GetLeafRTTI() == RTTI_NPC );
		// 테이머와의 거리 체크
		if( ( (CNpc*)m_pTarget )->CheckTamerDist() == false )
		{	
			( (CNpc*)m_pTarget )->SetCloseAnimation();

			Close();
			cMessageBox::DelMsg(20067,false);
			return;
		}
	}	
}

cBaseWindow::eMU_TYPE
cTalk::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cCreateName::Update_ForMouseInstance( cBaseWindow::WT_CREATE_GUILD_NAME, 0 );
	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;

	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;	

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:		
		{
			if( m_pTarget )
			{
				CsC_AniProp* pAni = m_pTarget->GetProp_Animation();
				if( pAni && pAni->IsSequence( ANI::NPC_CLOSE ) )
					m_pTarget->SetAnimation( ANI::NPC_CLOSE );	
			}
			Close();
		}		
		return muReturn;
	default:
		return muReturn;
	}

	if( m_pScrollBar )
		m_pScrollBar->Update_ForMouse();

	if( _UpdateStringList() == true )
	{
		m_pSelectedString = NULL;
		return muReturn;
	}

	return muReturn;
}

bool cTalk::_UpdateStringList()
{
	int nActiveIndex1, nActiveIndex2;
	m_bRenderSelMark = true;
	m_ptSelMarkPos = IF_TALK_STRLIST_POS - CsPoint( 0, 1 ) + GetRootClient();
	switch( m_StringList.Update_ForMouse( &m_pSelectedString, nActiveIndex1, nActiveIndex2, m_ptSelMarkPos, TALK_LINE_DELTA, CsPoint( 500, 22 ) ) )
	{
	case CURSOR::BUTTON_OUTWINDOW:
		m_bRenderSelMark = false;
		m_bClickMask = false;
		return false;
	case CURSOR::LBUTTON_DOWN:
		m_bClickMask = true;
		break;
	case CURSOR::LBUTTON_UP:
		if( m_bClickMask == true )
			m_bRenderSelMark = _SelectedStringList( m_pSelectedString, nActiveIndex1, nActiveIndex2 );
		break;
	}

#ifdef SIMPLE_TOOLTIP
	if( nActiveIndex1 == cBaseWindow::WT_DIGITAMA_SCAN )
	{
		CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1017 );
	}
	else if( nActiveIndex1 == cBaseWindow::WT_DIGITAMA_SELL )
	{
		CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1018 );
	}
#endif
	
	return true;
}

bool cTalk::_SelectedStringList(cString* pSelectString, int nValue1, int nValue2)
{
	// return false 하면 SelectMark 표시 안 한다

	SAFE_POINTER_RETVAL( pSelectString, false );

	if( !pSelectString->GetEnabled() )
		return false;

	cWindow::PlaySound( cWindow::SD_Bu2 );

	switch( nValue1 )
	{
	case cBaseWindow::WT_STORE:
		{				
			std::pair<CsC_AvObject*, int> pairStoreInfo;
			pairStoreInfo.first = m_pTarget;
			pairStoreInfo.second = nValue2;

			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_STORE_WINDOW, &pairStoreInfo );
		}
		break;			
	case cBaseWindow::WT_ITEM_PRODUCTION_SHOP:// 아이템 제작 창 오픈
		GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_ITEM_PRODUCTION_SHOP, m_pTarget );
		break;
	case cBaseWindow::WT_DIGITAMA_SELL:
		{
			bool bSound = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );

			UINT nIDX = m_pTarget->GetIDX();
			UINT nWindowType = cBaseWindow::WT_DIGITAMA_SELL;
			ContentsStream kTmp;
			kTmp << nWindowType << nIDX;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_OPEN_NPC, &kTmp );
		}
		break;
	case cBaseWindow::WT_DIGITAMA_SCAN:
		{
			bool bSound = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );
			
			UINT nIDX = m_pTarget->GetIDX();
			UINT nWindowType = cBaseWindow::WT_DIGITAMA_SCAN;
			ContentsStream kTmp;
			kTmp << nWindowType << nIDX;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_OPEN_NPC, &kTmp );
		}
		break;
	case cBaseWindow::WT_BATTLE_MATCHING_SYSTEM:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_BATTLEMATCH_REGIST_WINDOW, m_pTarget );
		}
		break;
	case cBaseWindow::WT_QUEST_REV:
		{
			// 즉시 퀘스트가 아닌가
			assert_cs( nsCsFileTable::g_pQuestMng->IsQuest( nValue2 ) );
			CsQuest* pFTQuest = nsCsFileTable::g_pQuestMng->GetQuest( nValue2 );
			if( pFTQuest->IsImmediate() == false )
			{
				( (cQuestRev*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_QUEST_REV ) )->SetTarget( m_pTarget, nValue2 );
				if( pFTQuest->GetModelID_Recv() != 0 )
				{
					g_pNpcMng->SetZoomNpc( g_pNpcMng->LoadZoomNpcTemp( pFTQuest->GetModelID_Recv() ) );
				}
			}
			else
			{
				// 퀘스트를 받을 공간이 존재 하는가
				if( g_pDataMng->GetQuest()->GetInfo()->GetExecuteCount() < nQuest::MaxExecute )
				{
					// 현재 이 퀘스트는 완료가 가능한가?
					cData_Quest::sPROCESS* pProcess = g_pDataMng->GetQuest()->InsertProcess( pFTQuest );
					bool bComp = pProcess->s_bCompleate;
					pProcess->Delete();
					cData_Quest::sPROCESS::DeleteInstance( pProcess );
					if( bComp )
					{
						( (cQuestComp*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_QUEST_COMP ) )->SetTarget( m_pTarget, nValue2 );
						if( pFTQuest->GetModelID_Comp() != 0 )
						{
							g_pNpcMng->SetZoomNpc( g_pNpcMng->LoadZoomNpcTemp( pFTQuest->GetModelID_Comp() ) );
						}
					}
					else
					{
						SetTarget( m_pTarget, false, pFTQuest->m_szExpert );
						return false;
					}
				}
				else
				{
					cPrintMsg::PrintMsg( 14002 );
				}
			}
		}
		break;
	case cBaseWindow::WT_QUEST_COMP:
		{					
			( (cQuestComp*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_QUEST_COMP ) )->SetTarget( m_pTarget, nValue2 );
			CsQuest* pFTQuest = nsCsFileTable::g_pQuestMng->GetQuest( nValue2 );
			if( pFTQuest->GetModelID_Comp() != 0 )
			{
				g_pNpcMng->SetZoomNpc( g_pNpcMng->LoadZoomNpcTemp( pFTQuest->GetModelID_Comp() ) );
			}
		}
		break;
	case cBaseWindow::WT_PORTAL_ENABLE:
		{
			assert_cs( pSelectString );
			// 이벤트 있다면 이벤트 연결

			// 포탈 사용시 돈이 소비 된다면 메세지 박스 출력					
			sNPC_TYPE_PORTAL::sPORTAL* pPortal = ( (sNPC_TYPE_PORTAL*)((CNpc*)m_pTarget)->GetFTNpc()->GetType() )->GetPortal( pSelectString->GetValue2() );
			CsMapPortal* pPortalFT = nsCsMapTable::g_pMapPortalMng->GetPortal_ByPortalID( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID, pSelectString->GetValue3() );
			if( pPortal )
			{
				u8 reqMoney = 0;
				u8 reqItem = 0;
				u8 reqCount = 0;

				for( int p=0; p<NPC_TYPE_PORTAL_MAX_REQ_COUNT; ++p )
				{
					if( pPortal->s_ReqArray[ p ].s_eEnableType == sNPC_TYPE_PORTAL::None )
						continue;

					if( pPortal->s_ReqArray[ p ].s_eEnableType == sNPC_TYPE_PORTAL::Money )
					{
						reqMoney += pPortal->s_ReqArray[ p ].s_nEnableID;
					}

					if( pPortal->s_ReqArray[ p ].s_eEnableType == sNPC_TYPE_PORTAL::Item )
					{
						reqItem = pPortal->s_ReqArray[ p ].s_nEnableID;
						reqCount = pPortal->s_ReqArray[ p ].s_nEnableCount;
					}

				}

				if( reqItem != 0 )
				{
					cData_Inven* pInven = g_pDataMng->GetInven();

					CsMapPortal* pPotalFT = nsCsMapTable::g_pMapPortalMng->GetPortal_ByPortalID( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID, pSelectString->GetValue3() );
					assert_cs( pPotalFT != NULL );

					if( pPotalFT->GetInfo()->s_dwDestMapID == 0 )
						return false;

					if(CsMapPortal::DG == pPotalFT->GetInfo()->s_dwPortalType)
					{
						CsMapList::sINFO* pMapFT = nsCsMapTable::g_pMapListMng->GetList( pPortalFT->GetInfo()->s_dwDestMapID )->GetInfo();
						int nSlot = pInven->GetFirstSlot_Item_ID( (int)reqItem );
						cPrintMsg::PrintMsg( 20067, const_cast<TCHAR*>(pMapFT->s_szMapDiscript.c_str()), g_pDataMng->GetInven()->GetFTData( nSlot )->s_szName );
						cMessageBox::GetFirstMessageBox()->SetValue1( nSlot );
						cMessageBox::GetFirstMessageBox()->SetValue2( pSelectString->GetValue3() );
						return false;
					}
					else
					{
						for(uint i = 0 ; i < reqCount ; i++)
						{	
							int nSlot = pInven->GetFirstSlot_Item_ID( (int)reqItem );
							cItemInfo* pItem = g_pDataMng->GetInven()->GetData( nSlot );
							pItem->DecreaseCount( 1 );
						}

					}
				}

				if( reqMoney != 0 )
				{
					sMONEY money( reqMoney );
					CsMapList::sINFO* pMapFT = nsCsMapTable::g_pMapListMng->GetList( pPortalFT->GetInfo()->s_dwDestMapID )->GetInfo();
					cPrintMsg::PrintMsg( 20066, const_cast<TCHAR*>(pMapFT->s_szMapDiscript.c_str()), &money );
					cMessageBox::GetFirstMessageBox()->SetValue1( pSelectString->GetValue3() );
					return false;
				}
			}
			// 포탈 태우자
			DWORD dwValue = pSelectString->GetValue3();
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PORTAL_MOVE, &dwValue );
		}
		break;
	case cBaseWindow::WT_PORTAL_DISABLE:
		{
			int nErrorIdx = pSelectString->GetValue2();
			switch( m_nPortalDisableID[ nErrorIdx ] )
			{
			case 30101:
			case 30102:
				cPrintMsg::PrintMsg( m_nPortalDisableID[ nErrorIdx ], m_szPortalDisableDiscript[ nErrorIdx ], &m_nPortalDisableCount[ nErrorIdx ] );
				break;
			case 30100:
			case 30104:
			case 30105:
			case 30106:
				cPrintMsg::PrintMsg( m_nPortalDisableID[ nErrorIdx ], &m_nPortalDisableCount[ nErrorIdx ] );						
				break;
			case 30103:
				{
					sMONEY money( m_nPortalDisableCount[ nErrorIdx ] );
					cPrintMsg::PrintMsg( m_nPortalDisableID[ nErrorIdx ], &money );
				}						
				break;
			default:
				assert_cs( false );						
			}
		}
		break;
	case cBaseWindow::WT_MAKE_TACTICS:
		{					
			bool bSound = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );

			cMakeTactics* pWin = dynamic_cast<cMakeTactics*>(g_pGameIF->GetDynamicIF( cBaseWindow::WT_MAKE_TACTICS ));
			if( pWin )
			{
				CsPoint csPos = pWin->GetRootSize();
				GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_POS_DEPENDON_INVEN, &csPos );

				pWin->SetPos( csPos );
				pWin->ResetDevice();
				pWin->SetTarget( m_pTarget );
			}
		}
		break;
#ifdef LJW_EQUIPSYSTEM_190724
	case cBaseWindow::WT_EQUIPMENT_SYSTEM:
		GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_EQUIPSYSTEM_NPC, m_pTarget );
		break;
#else
	case cBaseWindow::WT_ELEMENTITEM_CHECK:
	case cBaseWindow::WT_ELEMENTITEM_ATTACH:
		{
			std::pair< int, CsC_AvObject* > pairSend( nValue1, m_pTarget );
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_ELEMENT_ITEM_NPC, &pairSend );
		}
		break;
#endif
	case cBaseWindow::WT_WAREHOUSE:
		{
			bool bSound = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );

			cWarehouse* Warehouse = (cWarehouse*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_WAREHOUSE );
			if( Warehouse )
				Warehouse->SetTarget( m_pTarget );
		}
		break;
	case cBaseWindow::WT_TACTICSHOUSE:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_SHOWVIEWER_BY_NPC, m_pTarget);
		}
		break;
	case cBaseWindow::WT_CREATE_GUILD_NAME:
		{
			cCommunityContent::sGUILD_INFO guildInfo;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDINFO, &guildInfo );
			if( guildInfo.s_szGuildName.size() > 0 )
			{
				Close( false );
				cPrintMsg::PrintMsg( 30146 );
				break;
			}

			if( g_pDataMng->GetInven()->GetCount_Item_TypeLS( ITEM_TYPELS_GUILD_CREATE ) < nsCsFileTable::g_pBaseMng->GetGuildInfo( 1 )->s_nItemCount1 )
			{
				Close( false );
				int nCount = nsCsFileTable::g_pBaseMng->GetGuildInfo( 1 )->s_nItemCount1;
				cPrintMsg::PrintMsg( 30149, ITEM_STR_GUILD_CREATE, &nCount );
				break;
			}						
#ifdef GUILD_RENEWAL
			// 레벨 체크
			u4 limiteLv = nsCsFileTable::g_pBaseMng->GetGuildBase()->s_nLimitLv;
			if( g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel() < limiteLv )
			{
				cPrintMsg::PrintMsg( 30218 , &limiteLv );	// @<tc:900>테이머 레벨 [int] 미만@</tc>은\n길드를 창설할 수 없습니다.%
				break;
			}
#endif
			cCreateName::DeleteInstance();					
			cCreateName::NewInstance( cBaseWindow::WT_CREATE_GUILD_NAME, 0, CsPoint( -1, -1 ) );
		}
		break;
	case cBaseWindow::WT_UPGRADE_GUILD:
		{					

		} 
		break;			
	case cBaseWindow::WT_CAPSULE_MACHINE:
		{
			if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::CASH_CAPSULE_COIN_COMPLEATE ) )
			{
				int nCoinInvenIndex = g_pDataMng->GetInven()->GetFirstSlot_ItemType( nItem::Capsule_Coin, 0 );
				if( nCoinInvenIndex != cData_Inven::INVALIDE_INVEN_INDEX )
				{
					// 인벤이 가득찼다면
					if( g_pDataMng->GetInven()->GetEmptySlotCount() == 0 )
					{
						cPrintMsg::PrintMsg( 11015 );
					}
					else
					{								
						// 코인 아이템					
						cPrintMsg::PrintMsg( 20022, g_pDataMng->GetInven()->GetFTData( nCoinInvenIndex )->s_szName );
						cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
						pMBox->SetValue1( nCoinInvenIndex );
						pMBox->SetValuePointer1( m_pTarget );
						return false;
					}							
				}
				else
				{
					int nItemCount = 1;
					cPrintMsg::PrintMsg( 30102, ITEM_STR_CAPSULE_COIN, &nItemCount );
				}
			}					
		}
		break;
	case cBaseWindow::WT_ENCHANT_TACTICS:
		{
#ifdef CROSSWARS_SYSTEM
			// 크로스 테이블 뒤져서 자기가 데리고있는디지몬이 크로스 디지몬이면..
			DWORD dwMyDigimon = g_pCharMng->GetDigimonUser(0)->GetBaseDigimonFTID();
			bool bCross = nsCsFileTable::g_pDigimonMng->IsCrossCombine(dwMyDigimon);	

			if(bCross)
			{
				// 크로스 디지몬은 강화를 할수없습니다. 메시지
				cPrintMsg::PrintMsg( 30591 );
				return false;
			}
#endif
			FLOWMGR_ST.PushFlow( Flow::CFlow::FLW_DATSCENTER );				
		}
		break;
	case cBaseWindow::WT_DATA_TRADE_SELECT:
		{
			FLOWMGR_ST.PushFlow( Flow::CFlow::FLW_DATATRADECENTER );
		}
		break;
	case cBaseWindow::WT_CARD_EVENT:
		{
			SAFE_POINTER_BEK( m_pTarget );
			CsNpc* pFTNpc = ( (CNpc*)m_pTarget )->GetFTNpc();
			SAFE_POINTER_BEK( pFTNpc );
			CsNpc::sINFO* pInfo = pFTNpc->GetInfo();
			SAFE_POINTER_BEK( pInfo );
			DWORD dwTargetNpcID = pInfo->s_dwNpcID;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_CARD_EVENT_UI, &dwTargetNpcID );
		}
		break;
	case cBaseWindow::WT_NEW_GOTCHA_MACHINE:
		{
			if( nValue2 == nsCsNpcTable::NT_GOTCHA_MACHINE )	// 레어머신
			{
				GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_OPEN_GOTCHAMACHINE, m_pTarget );
			}
			else if( nValue2 == nsCsNpcTable::NT_MYSTERY_MACHINE )	// 수수께끼머신
			{
				SAFE_POINTER_BEK( pSelectString );
				std::pair< CsC_AvObject*, int > pairMachine( m_pTarget, pSelectString->GetValue3() );
				GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_MYSTERYMACHINE, &pairMachine );
			}
		}
		break;
	case cBaseWindow::WT_MASTERS_MATCHING:
		{
			//타겟 셋팅
			g_pGameIF->SetMastersMatchTarget( m_pTarget );

			//팀 미정인지 확인
			if( g_pGameIF->m_sMyInfo.s_eMyTeam == NONE )//팀 아직 미정인 아이 ( 이벤트 미참여 )
			{
				//팀 결정하고 이벤트 참가 할건지 물어봐
				cPrintMsg::PrintMsg( 30621 );
			}
			else//이벤트 참여 중인 애는 기부창 오픈 해줘야해
			{
				//인벤 오픈
				bool bSound = false;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );

				//리셋 시간 체크
				u4 nNextTime = g_pGameIF->m_sMyInfo.s_nNextResetTime;
				u4 nCurTime = _TIME_TS;

				if( nNextTime < nCurTime ) // 리셋시간 지났으면 서버에 오픈 요청
				{
					//유저인덱스
					u4 nUserID =  g_pCharMng->GetTamerUser()->GetUniqID();

					//Npc번호
					DWORD nNpcID = ((CNpc*)m_pTarget)->GetFTNpc()->GetInfo()->s_dwNpcID;

					//요청
					net::game->SendMastersMatchOpen( nUserID, (u4)nNpcID );

					//데이터 수신중 메세지
					cPrintMsg::PrintMsg( 10019 );
				}
				else // 리셋 시간 아니면 가지고 있는 데이터로 열어
				{
					// 기본 데이터로 여는지는 Create때 체크 ( nValue = 1 )
					cMastersMatching* pMastersMatching = (cMastersMatching*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_MASTERS_MATCHING, 0, 1 );
					if( pMastersMatching )
						pMastersMatching->SetTarget( m_pTarget );
				}
			}
		}
		break;
	case cBaseWindow::WT_INFINITEWAR_START_WINDOW:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INFINITEWAR_START_WINDOW, m_pTarget );
		}
		break;
	case cBaseWindow::WT_INFINITEWAR_GIVEUP_WINDOW:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INFINITEWAR_GIVEUP_WINDOW, m_pTarget );
		}
		break;
	case cBaseWindow::WT_INFINITEWAR_NOTICEBOARD_WINDOW:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INFINITEWAR_NOTICEBOARD_WINDOW, m_pTarget );
		}
		break;
	case cBaseWindow::WT_SPIRIT_EVOLUTION:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_EXTRA_EVOLUTION_STORE, m_pTarget );
		}
		break;
	case cBaseWindow::WT_EVENT_PINOKIMON:
		{
			switch( nValue2 )
			{
			case 1:		// 안내창 보여주기
				cPrintMsg::PrintMsg( 30740 );
				break;
			case 2:		// 카드 투표창 보여주기
				{
					cEvent_Pinokimon* pEvPinoki = (cEvent_Pinokimon*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_EVENT_PINOKIMON );
					if( pEvPinoki )
						pEvPinoki->SetTarget( m_pTarget );

					// 갱신 시간이 지났으면 목록 요청
					if( g_pGameIF->m_sEventInfo.s_nNextResetTime < _TIME_TS )
						net::game->SendPinokimonInfoReq();
				}
				break;
			default:
				break;
			}
		}
		break;
	default:
		assert_cs( false );
	}

	if( nValue1 != cBaseWindow::WT_CREATE_GUILD_NAME )
		Close( false );

	return true;
}

void cTalk::Render()
{
	m_pBGWindow->Render();

	RenderScript();

	if( m_bRenderSelMark == true )
		m_pSelMask->Render( m_ptSelMarkPos );

	m_StringList.Render( IF_TALK_STRLIST_POS + GetRootClient(), TALK_LINE_DELTA );

	if( m_eNpcType == nsCsNpcTable::NT_GUILD )
		cCreateName::RenderInstance( cBaseWindow::WT_CREATE_GUILD_NAME, 0 );
}


void cTalk::SetTarget( CsC_AvObject* pTarget, bool bNew /*=true*/, TCHAR* szTalk /*=NULL*/ )
{
	assert_cs( pTarget != NULL );
	m_pTarget = pTarget;

	assert_cs( m_pTarget->GetLeafRTTI() == RTTI_NPC );
	CNpc* pNpc = (CNpc*)m_pTarget;
	if( pNpc->GetNpcRenderType() != CNpc::NRT_NORMAL )
		DisableTamerDistCancel();

	pNpc->Try_Talk();

	ContentsStream kTmp;
	UINT nFTID = 0;
	UINT nObjType = 0;
	kTmp << nFTID << nObjType;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ACTIONKEY_NPC, &kTmp );

	CsNpc::sINFO* pFTNpc = pNpc->GetFTNpc()->GetInfo();

	// 이름	
	m_pName->SetText( pNpc->GetCharOption()->GetName() );

	// 본문
	m_StringList.Delete();

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_13, NiColor( 0.8f, 0.8f, 0.8f ) );	
	ti.s_bOutLine = false;

	// 말풍선 셋팅 
	if( szTalk == NULL )
	{
		TCHAR* szBody = g_pDataMng->GetQuest()->NpcTalk(pFTNpc->s_dwNpcID);	// npc ID 로 NPC 테이블의 대화문자를 얻어온다.
		if (szBody == NULL)	// NPC 테이블에 대화가 할당되지 않으면?
			szBody = pFTNpc->s_szTalk;
		TCHAR sz[QUEST_PROCESS_LEN + 64] = { 0, };
		g_pStringAnalysis->StringTransfer(sz, QUEST_PROCESS_LEN + 64, szBody);
		g_pStringAnalysis->Cut_Parcing(&m_StringList, IF_TALK_STRING_CUT, sz, &ti);	// 말풍선에 대화를  길이맞춰서 적절히 파싱한다.

		ti.s_Color = NiColor::WHITE;

		// 추가 항목
		std::list< cString* >	listType;

		// 퀘스트 체크
		_CheckQuest( &listType );

		m_eNpcType = pFTNpc->s_eType;
		switch( pFTNpc->s_eType )
		{
		case nsCsNpcTable::NT_NONE:
			break;
		case nsCsNpcTable::NT_TRADE:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, STORE, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT( "TALK_MENU_STORE" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_STORE );				
				pStringType->SetValue2( nsCsNpcTable::NT_TRADE );

				listType.push_back( pStringType );
			}
			break;
		case nsCsNpcTable::NT_ITEM_PRODUCTION_NPC:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, STORE, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT( "TALKNAME_ITEMPRODUCTION_SHOP" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_ITEM_PRODUCTION_SHOP);
				pStringType->SetValue2( nsCsNpcTable::NT_ITEM_PRODUCTION_NPC );

				listType.push_back( pStringType );
			}
			break;
		case nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, TALK_DIGIMON_BATTLE, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT( "TALKNAME_BATTLEMATCH_NPC" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_BATTLE_MATCHING_SYSTEM );
				pStringType->SetValue2( nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC );

				listType.push_back( pStringType );
			}
			break;
		case nsCsNpcTable::NT_DIGITAMA_TRADE:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, DIGITAMA_SCAN, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				std::wstring strItemScan = UISTRING_TEXT("ITEM_SCAN_OUTPUT_TEXT");
				ti.SetText( strItemScan.c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_DIGITAMA_SCAN );
				listType.push_back( pStringType );


				pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, DIGITAMA_SELL, 0, CsPoint( 0, -2 ) );
				std::wstring strItemRecovery = UISTRING_TEXT("ITEM_RECOVERY_OUTPUT_TEXT");
				ti.SetText( strItemRecovery.c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_DIGITAMA_SELL );
				listType.push_back( pStringType );			
			}
			break;
		case nsCsNpcTable::NT_PORTAL:
			{
				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;

				DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
				sNPC_TYPE_PORTAL* pType = (sNPC_TYPE_PORTAL*)pNpc->GetFTNpc()->GetType();

				assert_csm( pType->s_nPortalCount <= 10, L"배열 갯수를 10으로 잡아놨기 때문에 포탈 갯수는 10 이하여야 한다. 원치 않을시에는 프로그래머에게 말하세요" );

				// 조건 체크				
				NiColor colorBackup = ti.s_Color;
				for( int p=0; p<pType->s_nPortalCount; ++p )
				{
					sNPC_TYPE_PORTAL::sPORTAL* pPortal = pType->GetPortal( p );
					cBaseWindow::eWINDOW_TYPE wt = cBaseWindow::WT_PORTAL_ENABLE;
					for( int q=0; q<NPC_TYPE_PORTAL_MAX_REQ_COUNT; ++q )
					{
						switch( pPortal->s_ReqArray[ q ].s_eEnableType )
						{
						case sNPC_TYPE_PORTAL::None:
							break;
						case sNPC_TYPE_PORTAL::Quest:
							if( g_pDataMng->GetQuest()->IsCompleate( pPortal->s_ReqArray[ q ].s_nEnableID ) == false )
							{
								ti.s_Color = NiColor( 1, 0.1f, 0.1f );
								wt = cBaseWindow::WT_PORTAL_DISABLE;

								m_nPortalDisableID[ p ] = 30101;
								_tcscpy_s( m_szPortalDisableDiscript[ p ], 64, nsCsFileTable::g_pQuestMng->GetQuest( pPortal->s_ReqArray[ q ].s_nEnableID )->m_szTitleText );
								m_nPortalDisableCount[ p ] = 0;
							}
							break;
						case sNPC_TYPE_PORTAL::Item:
							{
								int nCount = g_pDataMng->GetInven()->GetCount_Item_ID( pPortal->s_ReqArray[ q ].s_nEnableID );
								nCount += g_pDataMng->GetTEquip()->GetCount_Item_ID( pPortal->s_ReqArray[ q ].s_nEnableID );
								if( nCount < (int)pPortal->s_ReqArray[ q ].s_nEnableCount )
								{
									ti.s_Color = NiColor( 1, 0.1f, 0.1f );
									wt = cBaseWindow::WT_PORTAL_DISABLE;

									m_nPortalDisableID[ p ] = 30102;
									_tcscpy_s( m_szPortalDisableDiscript[ p ], 64, nsCsFileTable::g_pItemMng->GetItem( pPortal->s_ReqArray[ q ].s_nEnableID )->GetInfo()->s_szName );
									m_nPortalDisableCount[ p ] = pPortal->s_ReqArray[ q ].s_nEnableCount;
								}
							}
							break;
						case sNPC_TYPE_PORTAL::TMinLevel:// 최소 레벨
							{
								if( g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel() < (int)pPortal->s_ReqArray[ q ].s_nEnableID )
								{
									ti.s_Color = NiColor( 1, 0.1f, 0.1f );
									wt = cBaseWindow::WT_PORTAL_DISABLE;

									m_nPortalDisableID[ p ] = 30100;
									m_szPortalDisableDiscript[ p ][ 0 ] = NULL;
									m_nPortalDisableCount[ p ] = pPortal->s_ReqArray[ q ].s_nEnableID;
								}
							}
							break;
						case sNPC_TYPE_PORTAL::TMaxLevel:// 최대 레벨
							{
								if( g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel() > (int)pPortal->s_ReqArray[ q ].s_nEnableID )
								{
									ti.s_Color = NiColor( 1, 0.1f, 0.1f );
									wt = cBaseWindow::WT_PORTAL_DISABLE;

									m_nPortalDisableID[ p ] = 30104;
									m_szPortalDisableDiscript[ p ][ 0 ] = NULL;
									m_nPortalDisableCount[ p ] = pPortal->s_ReqArray[ q ].s_nEnableID;
								}
							}
							break;
						case sNPC_TYPE_PORTAL::DMinLevel:// 최소 레벨
							{
								CDigimonUser* pDi = g_pCharMng->GetDigimonUser(0);
								if( NULL == pDi )
								{
									ti.s_Color = NiColor( 1, 0.1f, 0.1f );
									wt = cBaseWindow::WT_PORTAL_DISABLE;
									m_nPortalDisableID[ p ] = 30105;
									m_szPortalDisableDiscript[ p ][ 0 ] = NULL;
									m_nPortalDisableCount[ p ] = pPortal->s_ReqArray[ q ].s_nEnableID;
									break;
								}

								CsC_AvObject::sBASE_STAT* pBaseStat = pDi->GetBaseStat();
								if( NULL == pBaseStat )
								{
									ti.s_Color = NiColor( 1, 0.1f, 0.1f );
									wt = cBaseWindow::WT_PORTAL_DISABLE;
									m_nPortalDisableID[ p ] = 30105;
									m_szPortalDisableDiscript[ p ][ 0 ] = NULL;
									m_nPortalDisableCount[ p ] = pPortal->s_ReqArray[ q ].s_nEnableID;
									break;
								}

								if( pBaseStat->GetLevel() < (int)pPortal->s_ReqArray[ q ].s_nEnableID )
								{
									ti.s_Color = NiColor( 1, 0.1f, 0.1f );
									wt = cBaseWindow::WT_PORTAL_DISABLE;

									m_nPortalDisableID[ p ] = 30105;
									m_szPortalDisableDiscript[ p ][ 0 ] = NULL;
									m_nPortalDisableCount[ p ] = pPortal->s_ReqArray[ q ].s_nEnableID;
								}
							}
							break;
						case sNPC_TYPE_PORTAL::DMaxLevel:// 최대 레벨
							{
								CDigimonUser* pDi = g_pCharMng->GetDigimonUser(0);
								if( NULL == pDi )
								{
									ti.s_Color = NiColor( 1, 0.1f, 0.1f );
									wt = cBaseWindow::WT_PORTAL_DISABLE;
									m_nPortalDisableID[ p ] = 30106;
									m_szPortalDisableDiscript[ p ][ 0 ] = NULL;
									m_nPortalDisableCount[ p ] = pPortal->s_ReqArray[ q ].s_nEnableID;
									break;
								}

								CsC_AvObject::sBASE_STAT* pBaseStat = pDi->GetBaseStat();
								if( NULL == pBaseStat )
								{
									ti.s_Color = NiColor( 1, 0.1f, 0.1f );
									wt = cBaseWindow::WT_PORTAL_DISABLE;
									m_nPortalDisableID[ p ] = 30106;
									m_szPortalDisableDiscript[ p ][ 0 ] = NULL;
									m_nPortalDisableCount[ p ] = pPortal->s_ReqArray[ q ].s_nEnableID;
									break;
								}

								if( pBaseStat->GetLevel() > (int)pPortal->s_ReqArray[ q ].s_nEnableID )
								{
									ti.s_Color = NiColor( 1, 0.1f, 0.1f );
									wt = cBaseWindow::WT_PORTAL_DISABLE;

									m_nPortalDisableID[ p ] = 30106;
									m_szPortalDisableDiscript[ p ][ 0 ] = NULL;
									m_nPortalDisableCount[ p ] = pPortal->s_ReqArray[ q ].s_nEnableID;
								}
							}
							break;
						case sNPC_TYPE_PORTAL::Money:
							{
								if( g_pDataMng->GetInven()->GetMoney() < (int)pPortal->s_ReqArray[ q ].s_nEnableID )
								{
									ti.s_Color = NiColor( 1, 0.1f, 0.1f );
									wt = cBaseWindow::WT_PORTAL_DISABLE;

									m_nPortalDisableID[ p ] = 30103;
									m_szPortalDisableDiscript[ p ][ 0 ] = NULL;
									m_nPortalDisableCount[ p ] = pPortal->s_ReqArray[ q ].s_nEnableID;
								}
							}
							break;
						default:
							assert_cs( false );
						}

						if( 0 != m_nPortalDisableID[ p ] )
							break;
					}

					CsMapPortal* pFTPortal = nsCsMapTable::g_pMapPortalMng->GetNpcPortal_ByObjectID( dwMapID, pFTNpc->s_dwNpcID, p );
					if( pFTPortal == NULL )
						continue;

					cString* pStringType = NiNew cString;
					pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, PORTAL, 0, CsPoint( 0, -2 ) );
					ti.SetText( _T( "  " ) );
					pStringType->AddText( &ti );

					if( pFTPortal->GetInfo()->s_dwDestMapID != nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID )
					{
						CsMapList::sINFO* pFTMapInfo = nsCsMapTable::g_pMapListMng->GetList( pFTPortal->GetInfo()->s_dwDestMapID )->GetInfo();
						ti.SetText( pFTMapInfo->s_szMapDiscript.c_str() );
					}
					else
					{
						ti.SetText( UISTRING_TEXT( "COMMON_TXT_MOVE" ).c_str() );
					}

					pStringType->AddText( &ti );
					pStringType->SetValue1( wt );
					pStringType->SetValue2( p );
					pStringType->SetValue3( pFTPortal->GetInfo()->s_dwPortalID );
					pStringType->SetValue4( pPortal->s_dwEventID );
					listType.push_back( pStringType );

					ti.s_Color = colorBackup;
				}
			}
			break;
		case nsCsNpcTable::NT_MAKE_TACTICS:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, TACTICS, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT( "TALK_MENU_CREATE_MERCENARY" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_MAKE_TACTICS );

				listType.push_back( pStringType );
			}
			break;
		case nsCsNpcTable::NT_ELEMENT_ITEM:
			{
#ifdef LJW_EQUIPSYSTEM_190724
				// Equipment System
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, EI_ATTACH, 0, CsPoint( 0, -2 ) );

				ti.SetText( UISTRING_TEXT( "TALK_MENU_EQUIP_SYSTEM" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_EQUIPMENT_SYSTEM );
				listType.push_back( pStringType );
#else
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, EI_CHECK, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT( "TALK_MENU_ATTRIBUTE_ANALYSIS" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_ELEMENTITEM_CHECK );
				listType.push_back( pStringType );


				pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, EI_ATTACH, 0, CsPoint( 0, -2 ) );

				ti.SetText( UISTRING_TEXT( "TALK_MENU_EQUIPMENT_MERGE" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_ELEMENTITEM_ATTACH );
				listType.push_back( pStringType );
#endif
			}
			break;
		case nsCsNpcTable::NT_WAREHOUSE:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, STORE, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT( "TALK_MENU_STORAGE" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_WAREHOUSE );
				listType.push_back( pStringType );
			}
			break;
		case nsCsNpcTable::NT_TACTICSHOUSE:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, STORE, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT( "TALK_MENU_MERCENARY_STORAGE" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_TACTICSHOUSE );
				listType.push_back( pStringType );
			}
			break;
		case nsCsNpcTable::NT_GUILD:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, STORE, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT( "TALK_MENU_ITEM_SHOP" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_STORE );
				pStringType->SetValue2( nsCsNpcTable::NT_GUILD );
				listType.push_back( pStringType );

				pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_CREATE_GUILD, 0, CsPoint( 0, -2 ) );

				int nInvenIndex = g_pDataMng->GetInven()->GetFirstSlot_Item_TypeLS( ITEM_TYPELS_GUILD_CREATE );
				if( nInvenIndex == cData_Inven::INVALIDE_INVEN_INDEX )
				{
					ti.s_Color = NiColor( 1.0f, 0.0f, 0.0f );
				}
#ifdef GUILD_RENEWAL
				// 레벨 체크
				if( g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel() < nsCsFileTable::g_pBaseMng->GetGuildBase()->s_nLimitLv )
				{
					// 레벨이 안되면 빨갛게
					ti.s_Color = NiColor( 1.0f, 0.0f, 0.0f );
				}
#endif

				ti.SetText( UISTRING_TEXT( "TALK_MENU_CREATE_GUILD" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );	
				pStringType->SetValue1( cBaseWindow::WT_CREATE_GUILD_NAME );
				listType.push_back( pStringType );	

				ti.s_Color = NiColor::WHITE;
			}
			break;
		case nsCsNpcTable::NT_DIGICORE:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, STORE, 0, CsPoint( 0, -2 ) );				

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT( "STORE_TITLE_DIGICORE_SHOP" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_STORE );						
				pStringType->SetValue2( nsCsNpcTable::NT_DIGICORE );

				listType.push_back( pStringType );			
			}
			break;
		case nsCsNpcTable::NT_CAPSULE_MACHINE:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, CAPSULE, 0, CsPoint( 0, -2 ) );

				ti.s_Color = FONT_RED;
				if( g_pDataMng )
				{
					cData_Inven* pInven = g_pDataMng->GetInven();
					if( pInven && pInven->GetFirstSlot_ItemType( nItem::Capsule_Coin, 0 ) != cData_Inven::INVALIDE_INVEN_INDEX )
						ti.s_Color = NiColor::WHITE;
				}
				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT( "TALK_MENU_USE_CAPSULE_MACHINE" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_CAPSULE_MACHINE );
				listType.push_back( pStringType );
			}
			break;
		case nsCsNpcTable::NT_EVENT_STORE:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, STORE, 0, CsPoint( 0, -2 ) );				

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT( "STORE_TITLE_EVENT_SHOP" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_STORE );						
				pStringType->SetValue2( nsCsNpcTable::NT_EVENT_STORE );

				listType.push_back( pStringType );					
			}
			break;
		case nsCsNpcTable::NT_DATS_PORTAL:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_CREATE_GUILD, 0, CsPoint( 0, -2 ) );

				ti.SetText( UISTRING_TEXT( "TALK_MENU_GO_TO_DATS_CENTER" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_ENCHANT_TACTICS );
				listType.push_back( pStringType );

				{
					pStringType = NiNew cString;
					pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_CREATE_GUILD, 0, CsPoint( 0, -2 ) );

					ti.SetText( (TCHAR*)UISTRING_TEXT( "DATA_TRADE" ).c_str());	//데이터 교환
					pStringType->TailAddSizeX( 10 );
					pStringType->AddText( &ti );
					pStringType->SetValue1( cBaseWindow::WT_DATA_TRADE_SELECT );
					listType.push_back( pStringType );
				}
			}
			break;
		case nsCsNpcTable::NT_PROPERTY_STORE:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, STORE, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT( "TALK_MENU_ATTRIBUTE_STORE" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_STORE );				
				pStringType->SetValue2( nsCsNpcTable::NT_PROPERTY_STORE );

				listType.push_back( pStringType );
			}
			break;
		case nsCsNpcTable::NT_GOTCHA_MACHINE:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, CAPSULE/*여기에 가챠머신 아이콘? 넣어말어?*/, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText(((CNpc*)m_pTarget)->GetFTNpc()->GetInfo()->s_szName );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_NEW_GOTCHA_MACHINE );
				pStringType->SetValue2( nsCsNpcTable::NT_GOTCHA_MACHINE );

				listType.push_back( pStringType );
			}
			break;
		case nsCsNpcTable::NT_MYSTERY_MACHINE:
			{
				cString* pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, CAPSULE/*여기에 가챠머신 아이콘? 넣어말어?*/, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText(((CNpc*)m_pTarget)->GetFTNpc()->GetInfo()->s_szName );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_NEW_GOTCHA_MACHINE );
				pStringType->SetValue2( nsCsNpcTable::NT_MYSTERY_MACHINE );

				listType.push_back( pStringType );

#ifdef EGG_PREFERMENT

				// 3단
				pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, CAPSULE, 0, CsPoint( 0, -2 ) );

				ti.SetText( L"4단 용병알 뽑기" );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_NEW_GOTCHA_MACHINE );
				pStringType->SetValue2( nsCsNpcTable::NT_MYSTERY_MACHINE );
				pStringType->SetValue3( 3 );

				listType.push_back( pStringType );

				// 4단
				pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, CAPSULE, 0, CsPoint( 0, -2 ) );

				ti.SetText( L"5단 용병알 뽑기 1" );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_NEW_GOTCHA_MACHINE );
				pStringType->SetValue2( nsCsNpcTable::NT_MYSTERY_MACHINE );
				pStringType->SetValue3( 4 );

				listType.push_back( pStringType );

				// 5단
				pStringType = NiNew cString;
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, CAPSULE, 0, CsPoint( 0, -2 ) );

				ti.SetText( L"5단 용병알 뽑기 2" );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				pStringType->SetValue1( cBaseWindow::WT_NEW_GOTCHA_MACHINE );
				pStringType->SetValue2( nsCsNpcTable::NT_MYSTERY_MACHINE );
				pStringType->SetValue3( 5 );

				listType.push_back( pStringType );
#endif
			}
			break;
		case nsCsNpcTable::NT_MASTERS_MATCHING:
			{
				cString* pStringType = NiNew cString;
				//아이콘
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, DIGIBALL, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT( "TALK_MENU_MASTERS_MATCH" ).c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );
				//양옆에 아이콘 넣으라고 해서 한번 더 넣음
//				pStringType->TailAddSizeX( 10 );
//				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, DIGIBALL, 0, CsPoint( 0, -2 ) );

				pStringType->SetValue1( cBaseWindow::WT_MASTERS_MATCHING );
				listType.push_back( pStringType );
			}
			break;

		case nsCsNpcTable::NT_INFINITEWAR_MANAGER_NPC:
			{
				cString* pStringType = NiNew cString;
				//아이콘
				if( pStringType )
				{
					pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, INFINITE_WAR_STAT, 0, CsPoint( 0, -2 ) );
					ti.s_pFont = &g_pEngine->m_FontSystem;
					ti.s_bOutLine = false;
					ti.s_eFontSize = CFont::FS_14;
					ti.SetText( UISTRING_TEXT("INFINITEWAR_TALK_START").c_str() );
					pStringType->TailAddSizeX( 10 );
					pStringType->AddText( &ti );
					pStringType->SetValue1( cBaseWindow::WT_INFINITEWAR_START_WINDOW );
					listType.push_back( pStringType );
				}
				cString* pStringType2 = NiNew cString;
				//아이콘
				if( pStringType2 )
				{
					pStringType2->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, INFINITE_WAR_END, 0, CsPoint( 0, -2 ) );
					ti.s_pFont = &g_pEngine->m_FontSystem;
					ti.s_bOutLine = false;
					ti.s_eFontSize = CFont::FS_14;
					ti.SetText( UISTRING_TEXT("INFINITEWAR_TALK_GIVEUP").c_str() );
					pStringType2->TailAddSizeX( 10 );
					pStringType2->AddText( &ti );
					pStringType2->SetValue1( cBaseWindow::WT_INFINITEWAR_GIVEUP_WINDOW );
					listType.push_back( pStringType2 );
				}
			}
			break;
		case nsCsNpcTable::NT_INFINITEWAR_NOTICEBOARD_NPC:
			{
				cString* pStringType = NiNew cString;
				//아이콘
				if( pStringType )
				{
					pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, INFINITE_NOTICEBOARD, 0, CsPoint( 0, -2 ) );
					ti.s_pFont = &g_pEngine->m_FontSystem;
					ti.s_bOutLine = false;
					ti.s_eFontSize = CFont::FS_14;
					ti.SetText( UISTRING_TEXT("INFINITEWAR_TALK_NOTICEBOARD").c_str() );
					pStringType->TailAddSizeX( 10 );
					pStringType->AddText( &ti );
					pStringType->SetValue1( cBaseWindow::WT_INFINITEWAR_NOTICEBOARD_WINDOW );
					listType.push_back( pStringType );
				}
			}
			break;
		case nsCsNpcTable::NT_SPIRIT_EVO:
			{
				cString* pStringType = NiNew cString;
				//아이콘
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, DIGIBALL, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT("NPC_TALK_SPRITI_EVOLUTION").c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );

				pStringType->SetValue1( cBaseWindow::WT_SPIRIT_EVOLUTION );
				listType.push_back( pStringType );
			}
			break;
		case nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC:
			{
				cString* pStringType = NiNew cString;
				//아이콘
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, DIGIBALL, 0, CsPoint( 0, -2 ) );

				ti.s_pFont = &g_pEngine->m_FontSystem;
				ti.s_bOutLine = false;
				ti.s_eFontSize = CFont::FS_14;
				ti.SetText( UISTRING_TEXT("NPC_TALK_EXTRA_EVOLUTION").c_str() );
				pStringType->TailAddSizeX( 10 );
				pStringType->AddText( &ti );

				pStringType->SetValue1( cBaseWindow::WT_SPIRIT_EVOLUTION );
				listType.push_back( pStringType );
			}
			break;
		case nsCsNpcTable::NT_SPECIAL_EVENT:	// 이벤트 NPC
			{
				// 서브 타입에 따라 동작
				sNPC_TYPE_SPECIAL_EVENT* pType = (sNPC_TYPE_SPECIAL_EVENT*)pNpc->GetFTNpc()->GetType();

				int nEvType = pType->GetSubType();
				switch( nEvType )
				{
				case sNPC_TYPE_SPECIAL_EVENT::EVENT_CARDGAME:		// 피에몬 카드 게임
					{
						cString* pStringType = NiNew cString;
						pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, STORE, 0, CsPoint( 0, -2 ) );

						ti.s_pFont = &g_pEngine->m_FontSystem;
						ti.s_bOutLine = false;
						ti.s_eFontSize = CFont::FS_14;

						pStringType->TailAddSizeX( 10 );
						pStringType->AddText( &ti );
						pStringType->SetValue1( cBaseWindow::WT_CARD_EVENT );				

						listType.push_back( pStringType );
					}
					break;
				case sNPC_TYPE_SPECIAL_EVENT::EVENT_PINOKIMON:		// 피노키몬의 카드게임(2014 겨울이벤트)
					{
						// 이벤트 룰 설명
						cString* pStringType = NiNew cString;
						pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, DIGIBALL, 0, CsPoint( 0, -2 ) );

						ti.s_pFont = &g_pEngine->m_FontSystem;
						ti.s_bOutLine = false;
						ti.s_eFontSize = CFont::FS_14;

						wstring msg = UISTRING_TEXT( "PINOKIMON_GAME_TALK_MESSAGE_1" );
	 					ti.SetText( const_cast<TCHAR*>(msg.c_str()) );

						pStringType->TailAddSizeX( 10 );
						pStringType->AddText( &ti );
						pStringType->SetValue1( cBaseWindow::WT_EVENT_PINOKIMON );
						pStringType->SetValue2( 1 );
						listType.push_back( pStringType );

						// 이벤트 참여
						pStringType = NiNew cString;
						pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, DIGIBALL, 0, CsPoint( 0, -2 ) );

						ti.s_pFont = &g_pEngine->m_FontSystem;
						ti.s_bOutLine = false;
						ti.s_eFontSize = CFont::FS_14;
						msg = UISTRING_TEXT( "PINOKIMON_GAME_TALK_MESSAGE_2" );
						ti.SetText( const_cast<TCHAR*>(msg.c_str()) );

						pStringType->TailAddSizeX( 10 );
						pStringType->AddText( &ti );
						pStringType->SetValue1( cBaseWindow::WT_EVENT_PINOKIMON );
						pStringType->SetValue2( 2 );
						listType.push_back( pStringType );
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			assert_cs( false );
		}	

		// 추가 되는 항목이 있다면
		if( listType.empty() == false )
		{
			// 여백 넣어 주자
			for( int i=0; i<1; ++i )
			{
				cString* pString = NiNew cString;

				ti.s_eFontSize = CFont::FS_10;
				ti.SetText( _T( "" ) );
				pString->AddText( &ti );
				m_StringList.AddTail( pString );
			}

			// 추가 항목 넣어주자
			std::list< cString* >::iterator it = listType.begin();
			std::list< cString* >::iterator itEnd = listType.end();
			for( ; it!=itEnd; ++it )
			{
				m_StringList.AddTail( *it );
			}
			listType.clear();
		}

		( (CNpc*)m_pTarget )->SetOpenAnimation();

		// 만약 퀘스트를 완료받지 못하였을겨우에는 에니메이션 플래이
		assert_cs( m_pTarget->GetLeafRTTI() == RTTI_NPC );
		if( ( (CNpc*)m_pTarget )->GetQuestOwner() )
		{
			if( ( (CNpc*)m_pTarget )->GetQuestOwner()->GetDisplayRev() == cData_QuestOwner::sNpcOwner::REQUITE )
			{
				( (CNpc*)m_pTarget )->Try_No();
			}
		}
	}
	else
	{
		g_pStringAnalysis->Cut_Parcing( &m_StringList, IF_TALK_STRING_CUT, szTalk, &ti );
		assert_cs( m_pTarget->GetLeafRTTI() == RTTI_NPC );
		( (CNpc*)m_pTarget )->Try_Yes();
	}

	// 추가 여백 2줄 무조건
	m_pScrollBar->SetRange( CsPoint( 0, m_StringList.GetSize() + 3 ) );	
	m_StringList.SetBuddy( m_pScrollBar );

	if( bNew == true )
	{
		g_pNpcMng->SetZoomNpc( m_pTarget );

		//==========================================================================================================
		// 업적 체크
		//==========================================================================================================
		GS2C_RECV_CHECKTYPE recv;
		recv.nType = AchieveContents::CA_NpcTalk;
		recv.nValue1 = pFTNpc->s_dwNpcID;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
	}
}

void cTalk::ResetDevice()
{	
	ResetDeviceScript();

	m_pBGWindow->ResetDevice();
	m_pSelMask->ResetDevice();
}

bool cTalk::IsHaveQuest( CNpc* pTargetNpc )
{
	if( ( pTargetNpc->GetQuestOwner() == NULL )&&( pTargetNpc != g_pNpcMng->GetDigivice() ) )
		return false;

	cData_QuestOwner::sNpcOwner* pQuestOwner = pTargetNpc->GetQuestOwner();
	if( pQuestOwner )
	{
		std::list< cData_QuestOwner::sNpcOwner::sQuestInfo* > listQuestInfo;
		// 보상받을 퀘스트
		pQuestOwner->GetQuestInfoList( cData_QuestOwner::sNpcOwner::REQUITE, &listQuestInfo );

		std::list< cData_QuestOwner::sNpcOwner::sQuestInfo* >::iterator it = listQuestInfo.begin();
		for( ; it != listQuestInfo.end(); ++it )
		{
			SAFE_POINTER_CON( *it );
			SAFE_POINTER_CON( (*it)->s_pProcess );
			
			if( (*it)->s_pProcess->s_bCompleate )
				return true;
		}

		listQuestInfo.clear();
		// 시작할 퀘스트
		DWORD dwPlag =	cData_QuestOwner::sNpcOwner::ENABLE_REV_MAIN | cData_QuestOwner::sNpcOwner::ENABLE_REV_SUB |
			cData_QuestOwner::sNpcOwner::ENABLE_REV_REPEAT | cData_QuestOwner::sNpcOwner::ENABLE_REV_EVENTREPEAT;
		pQuestOwner->GetQuestInfoList( dwPlag, &listQuestInfo );
		if( !listQuestInfo.empty() )
			return true;

		return false;
	}
	else// 디지바이스 퀘스트 체크
	{
		// 보상받을 퀘스트
		std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >* pQuestMap = g_pDataMng->GetQuestOwner()->GetReadyRevDigiviceOwner()->GetMap();
		if( !pQuestMap->empty() )
			return true;
		// 보상받을 퀘스트
		pQuestMap = g_pDataMng->GetQuestOwner()->GetProcDigiviceOwner()->GetMap();		
		std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >::iterator it = pQuestMap->begin();
		for( ; it!=pQuestMap->end(); ++it )
		{
			SAFE_POINTER_CON( it->second );
			SAFE_POINTER_CON( it->second->s_pProcess );
			if( it->second->s_pProcess->s_bCompleate )
				return true;
		}

		return false;
	}
}

void cTalk::_CheckQuest( std::list< cString* >* pList )
{
	// 퀘스트
	CNpc* pNpc = (CNpc*)m_pTarget;
	if( ( pNpc->GetQuestOwner() == NULL )&&( pNpc != g_pNpcMng->GetDigivice() ) )
		return;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_14, NiColor( 1, 0.8f, 0 ) );	//퀘스트 리스트 색상
	ti.s_bOutLine = false;

	if( pNpc->GetQuestOwner() )
	{
		cData_QuestOwner::sNpcOwner* pQuestOwner = pNpc->GetQuestOwner();
		std::list< cData_QuestOwner::sNpcOwner::sQuestInfo* > listQuestInfo;
		// 보상받을 퀘스트
		pQuestOwner->GetQuestInfoList( cData_QuestOwner::sNpcOwner::REQUITE, &listQuestInfo );

		std::list< cData_QuestOwner::sNpcOwner::sQuestInfo* >::iterator it = listQuestInfo.begin();
		std::list< cData_QuestOwner::sNpcOwner::sQuestInfo* >::iterator itEnd = listQuestInfo.end();
		for( ; it!=itEnd; ++it )
		{
			cString* pStringType = NiNew cString;

			switch( (*it)->s_pFTQuest->GetQuestType() )
			{
			case CsQuest::QT_MAIN:
			case CsQuest::QT_TUTORIAL:
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_MAIN_COMP, 0, CsPoint( 0, -2 ) );
				break;
			case CsQuest::QT_SUB:
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_SUB_COMP, 0, CsPoint( 0, -2 ) );
				break;
			case CsQuest::QT_DAILY:
			case CsQuest::QT_REPEAT:
			case CsQuest::QT_JOINTPROGRESS:
#ifdef SDM_TAMER_XGUAGE_20180628
			case CsQuest::QT_XANTI_JOINTPROGRESS:
#endif
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_REPEAT_COMP, 0, CsPoint( 0, -2 ) );
				break;
			case CsQuest::QT_EVENTREPEAT:
			//case CsQuest::QT_TIME:
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_EVENTREPEAT_COMP, 0, CsPoint( 0, -2 ) );
				break;
			default:
				assert_cs( false );
			}
			pStringType->TailAddSizeX( 10 );

			ti.SetText( (*it)->s_pFTQuest->m_szTitleText );

			pStringType->AddText( &ti );

			pStringType->SetValue1( (*it)->s_pProcess->s_bCompleate ? cBaseWindow::WT_QUEST_COMP : cBaseWindow::WT_QUEST_REV );
			pStringType->SetValue2( (*it)->s_pFTQuest->GetUniqID() );

			pList->push_back( pStringType );
		}

		listQuestInfo.clear();
		// 시작할 퀘스트
		DWORD dwPlag =	cData_QuestOwner::sNpcOwner::ENABLE_REV_MAIN | cData_QuestOwner::sNpcOwner::ENABLE_REV_SUB |
						cData_QuestOwner::sNpcOwner::ENABLE_REV_REPEAT | cData_QuestOwner::sNpcOwner::ENABLE_REV_EVENTREPEAT;
		pQuestOwner->GetQuestInfoList( dwPlag, &listQuestInfo );
		it = listQuestInfo.begin();
		itEnd = listQuestInfo.end();
		for( ; it!=itEnd; ++it )
		{
			cString* pStringType = NiNew cString;

			switch( (*it)->s_pFTQuest->GetQuestType() )
			{
			case CsQuest::QT_MAIN:
			case CsQuest::QT_TUTORIAL:
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_MAIN_RECV, 0, CsPoint( 0, -2 ) );
				break;
			case CsQuest::QT_SUB:
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_SUB_RECV, 0, CsPoint( 0, -2 ) );
				break;
			case CsQuest::QT_DAILY:
			case CsQuest::QT_REPEAT:
			case CsQuest::QT_JOINTPROGRESS:
#ifdef SDM_TAMER_XGUAGE_20180628
			case CsQuest::QT_XANTI_JOINTPROGRESS:
#endif
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_REPEAT_RECV, 0, CsPoint( 0, -2 ) );
				break;
			case CsQuest::QT_EVENTREPEAT:
			//case CsQuest::QT_TIME:
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_EVENTREPEAT_RECV, 0, CsPoint( 0, -2 ) );
				break;
			default:
				assert_cs( false );
			}
			pStringType->TailAddSizeX( 10 );

			ti.SetText( (*it)->s_pFTQuest->m_szTitleText );

			if( (*it)->s_pFTQuest->IsImmediate() == false )
			{
				ti.s_Color = NiColor( 1, 0.8f, 0 );	// 서브 퀘스트 색상
			}
			else
			{
				// 현재 이 퀘스트는 완료가 가능한가?
				cData_Quest::sPROCESS* pProcess = g_pDataMng->GetQuest()->InsertProcess( (*it)->s_pFTQuest );
				if( pProcess->s_bCompleate == false )
				{
					ti.s_Color = FONT_RED;
				}
				else
				{
#ifdef UI_TXTCOLOR_RENEWAL_20190109
					ti.s_Color = NiColor::WHITE;
#else
					ti.s_Color = NiColor::BLACK;
#endif
				}
				pProcess->Delete();
				cData_Quest::sPROCESS::DeleteInstance( pProcess );
			}

			pStringType->AddText( &ti );

			pStringType->SetValue1( cBaseWindow::WT_QUEST_REV );
			pStringType->SetValue2( (*it)->s_pFTQuest->GetUniqID() );

			pList->push_back( pStringType );
		}
	}
	else
	{
		assert_cs( pNpc == g_pNpcMng->GetDigivice() );

		std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >* pQuestMap = g_pDataMng->GetQuestOwner()->GetReadyRevDigiviceOwner()->GetMap();
		std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >::iterator it = pQuestMap->begin();
		std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >::iterator itEnd = pQuestMap->end();
		for( ; it!=itEnd; ++it )
		{
			cString* pStringType = NiNew cString;
			switch( it->second->s_pFTQuest->GetQuestType() )
			{
			case CsQuest::QT_MAIN:
			case CsQuest::QT_TUTORIAL:
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_MAIN_RECV, 0, CsPoint( 0, -2 ) );
				break;
			case CsQuest::QT_SUB:
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_SUB_RECV, 0, CsPoint( 0, -2 ) );
				break;
			case CsQuest::QT_DAILY:
			case CsQuest::QT_REPEAT:
			case CsQuest::QT_JOINTPROGRESS:
#ifdef SDM_TAMER_XGUAGE_20180628
			case CsQuest::QT_XANTI_JOINTPROGRESS:
#endif
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_REPEAT_RECV, 0, CsPoint( 0, -2 ) );
				break;
			case CsQuest::QT_EVENTREPEAT:
			//case CsQuest::QT_TIME:
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_EVENTREPEAT_RECV, 0, CsPoint( 0, -2 ) );
				break;
			default:
				assert_cs( false );
			}

			pStringType->TailAddSizeX( 10 );

			ti.SetText( it->second->s_pFTQuest->m_szTitleText );
			pStringType->AddText( &ti );

			pStringType->SetValue1( cBaseWindow::WT_QUEST_REV );
			pStringType->SetValue2( it->second->s_pFTQuest->GetUniqID() );

			pList->push_back( pStringType );
		}

		// 보상받을 퀘스트
		pQuestMap = g_pDataMng->GetQuestOwner()->GetProcDigiviceOwner()->GetMap();
		it = pQuestMap->begin();
		itEnd = pQuestMap->end();
		for( ; it!=itEnd; ++it )
		{
			if( it->second->s_pProcess->s_bCompleate == false )
				continue;

			cString* pStringType = NiNew cString;
			switch( it->second->s_pFTQuest->GetQuestType() )
			{
			case CsQuest::QT_MAIN:
			case CsQuest::QT_TUTORIAL:
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_MAIN_COMP, 0, CsPoint( 0, -2 ) );
				break;
			case CsQuest::QT_SUB:
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_SUB_COMP, 0, CsPoint( 0, -2 ) );
				break;
			case CsQuest::QT_DAILY:
			case CsQuest::QT_REPEAT:
			case CsQuest::QT_JOINTPROGRESS:
#ifdef SDM_TAMER_XGUAGE_20180628
			case CsQuest::QT_XANTI_JOINTPROGRESS:
#endif
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_REPEAT_COMP, 0, CsPoint( 0, -2 ) );
				break;
			case CsQuest::QT_EVENTREPEAT:
			//case CsQuest::QT_TIME:
				pStringType->AddIcon( TALK_ICON_SIZE, ICONITEM::Talk, QT_EVENTREPEAT_COMP, 0, CsPoint( 0, -2 ) );
				break;
			default:
				assert_cs( false );
			}
			pStringType->TailAddSizeX( 10 );

			ti.SetText( it->second->s_pFTQuest->m_szTitleText );
			pStringType->AddText( &ti );

			pStringType->SetValue1( cBaseWindow::WT_QUEST_COMP );
			pStringType->SetValue2( it->second->s_pFTQuest->GetUniqID() );

			pList->push_back( pStringType );
		}
	}
	
	if( pList->size() > 0 )
	{
		std::list< cString* >::iterator itEnd = pList->end();
		(*(--itEnd))->AddSizeY( 16 );
	}
}

bool cTalk::_SelectList(bool bUpArrow)
{
	if( !m_pSelectedString )
	{
		int nIndex = 0;
		// 맨 위에 목록 하나 선택
		std::list< cString* > lStringList = *m_StringList.GetList();
		std::list< cString* >::iterator it = lStringList.begin();
		std::list< cString* >::iterator itEnd = lStringList.end();
		for( ; it != itEnd; ++it, ++nIndex )
		{
			if( (*it)->GetValue1() == cString::INVALIDE_STRING_VALUE )
				continue;
			m_pSelectedString = *it;
			break;
		}

		// Selected Mark PosY 갱신
		m_ptSelMarkPos = IF_TALK_STRLIST_POS - CsPoint( 0, 1 ) + GetRootClient();
		m_ptSelMarkPos.y = m_StringList.GetStringPosY( m_ptSelMarkPos.y, nIndex, TALK_LINE_DELTA );

		m_bRenderSelMark = true;

		return true;
	}

	SAFE_POINTER_RETVAL( m_pSelectedString, false );
	
	int nStrIndex = 0;
	std::vector< stSelectList > vSelectList;
	std::list< cString* > lStringList = *m_StringList.GetList();
	std::list< cString* >::iterator it = lStringList.begin();
	std::list< cString* >::iterator itEnd = lStringList.end();
	for( ; it != itEnd; ++it, ++nStrIndex )
	{
		if( (*it)->GetValue1() == cString::INVALIDE_STRING_VALUE )
			continue;

		stSelectList selList;
		selList.pString = *it;
		selList.nStringIndex = nStrIndex;
		
		// 선택 항목
		vSelectList.push_back( selList );
	}

	// 이전에 선택했던 목록이랑 체크
	int nListIndex = 0;
	int nSize = vSelectList.size();
	for( int i = 0; i < nSize; ++i )
	{
		stSelectList pSelList = vSelectList.at( i );
		cString* pCurString = pSelList.pString;
		SAFE_POINTER_BEK( pCurString );

		if( pCurString->GetValue1() == m_pSelectedString->GetValue1() &&
			pCurString->GetValue2() == m_pSelectedString->GetValue2() &&
			pCurString->GetValue3() == m_pSelectedString->GetValue3() )
		{
			nListIndex = i;
			break;
		}
	}

	if( bUpArrow )
	{
		nListIndex--;
		if( nListIndex < 0 )
			nListIndex = 0;
	}
	else
	{
		nListIndex++;
		if( nListIndex >= nSize )
			nListIndex = nSize - 1;
	}

	// Selected String 갱신
	stSelectList pSelList = vSelectList.at( nListIndex );
	cString* pSelString = pSelList.pString;
	SAFE_POINTER_RETVAL( pSelString, false );
	m_pSelectedString = pSelString;

	// Selected Mark PosY 갱신
	if( m_pScrollBar )
	{
		int nRenderCurPos = m_pScrollBar->GetCurPosIndex();
		int nRenderMaxPos = nRenderCurPos + m_pScrollBar->GetRenderCount() - 1;
		if( pSelList.nStringIndex >= nRenderMaxPos )
		{
			int nScrolling = ( pSelList.nStringIndex - nRenderMaxPos ) + 1/*Scroll Gab*/;
			m_pScrollBar->SetCurPosIndex( nRenderCurPos + nScrolling );
		}
		else if( pSelList.nStringIndex < nRenderCurPos )
		{
			m_pScrollBar->SetCurPosIndex( pSelList.nStringIndex );
		}
	}

	m_ptSelMarkPos = IF_TALK_STRLIST_POS - CsPoint( 0, 1 ) + GetRootClient();
	m_ptSelMarkPos.y = m_StringList.GetStringPosY( m_ptSelMarkPos.y, pSelList.nStringIndex, TALK_LINE_DELTA );

	m_bRenderSelMark = true;

	return true;
}

void cTalk::EnableString( int nNPCIdx ,int nTalkIdx, bool bNpcIDCheck )
{
	if( m_StringList.GetSize() <= 0 )
		return;

	if( bNpcIDCheck )
	{
		if(m_pTarget->GetFTID() != nNPCIdx)
			return;
	}

	std::list< cString* >::iterator it = m_StringList.GetList()->begin();
	std::list< cString* >::iterator itEnd = m_StringList.GetList()->end();

	for( ; it != itEnd ; ++it )
	{
		int nValue = (*it)->GetValue1();
		if( cString::INVALIDE_STRING_VALUE == nValue || nTalkIdx == nValue )
		{
			(*it)->SetEnable(true);
		}
		else
		{
			(*it)->SetEnable(false);
		}
	}
}

CsRect cTalk::GetControlPos( int nTalkIdx, int nSubValue )
{
	std::list< cString* >::iterator it = m_StringList.GetList()->begin();
	std::list< cString* >::iterator itEnd = m_StringList.GetList()->end();
	for( ; it != itEnd ; ++it )
	{
		if( nTalkIdx != (*it)->GetValue1() )
			continue;

		if( 0 != nSubValue && nSubValue != (*it)->GetValue2() )
			continue;
		
		cString::sELEMENT* pEl = (*it)->GetElement(0);
		SAFE_POINTER_RETVAL( pEl, NULL );
		return pEl->GetWorldRect();
	}
	return CsRect::ZERO;
}