
#include "stdafx.h"
#include "Data_ServerSync.h"

void cData_ServerSync::Reset()
{
	for( int i=0; i<cData_ServerSync::MAX_SYNC_COUNT; ++i )
	{
		m_Sync[ i ].Reset();
	}
}

void cData_ServerSync::Update()
{
	// 캡슐 코인
	if( IsEmptyRefCount( CASH_CAPSULE_COIN_WAIT ) == false )
	{
		// 클라이언트와 서버 전부 완료 되었다면
		if( IsEmptyRefCount( CASH_CAPSULE_COIN_CLIENT ) && IsEmptyRefCount( CASH_CAPSULE_COIN_SERVER ) )
		{
			// ?전부 풀고
			g_pDataMng->ReleaseItemLock();
			// 쿠폰아이템 감소
			int nSlot = g_pDataMng->GetInven()->GetFirstSlot_ItemType( nItem::Capsule_Coin, 0 );
			assert_cs( nSlot != cData_Inven::INVALIDE_INVEN_INDEX );
			g_pDataMng->GetInven()->GetData( nSlot )->DecreaseCount( 1 );

			// 인벤토리에 등록되어 있는 예약되어 있는 캡슐아이템을 집어 넣자
			cItemData* pCapsuleItem = g_pDataMng->GetInven()->GetCapsuleItem();
			g_pDataMng->GetInven()->ItemCrop( pCapsuleItem );			

			RecvServer( CASH_CAPSULE_COIN_WAIT, 0 );
			cBGSprite* pBGSprite = (cBGSprite*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_CAPSULE_COMP, cBGSprite::CAPSULE_COMP );
			pBGSprite->SetValue1( pCapsuleItem->GetType() );

			g_pDataMng->GetInven()->ReleaseCapsuleItem();
		}
	}
}

void cData_ServerSync::SendServer( eSYNC sync, int nRefConstant, void* pValue1 /*=NULL*/, void* pValue2 /*=NULL*/, void* pValue3 /*=NULL*/, void* pValue4 /*=NULL*/ )
{
#ifdef LEVELUP_EFFECT_LOOP
	if( sync == CHANGE_PARTNER )
	{
		std::list< CsC_AttackProp::sHD_BASE* > EventList = g_pCharMng->GetDigimonUser( 0 )->GetProp_Attack()->GetEventList();

		std::list< CsC_AttackProp::sHD_BASE* >::iterator it_Level = EventList.begin();
		std::list< CsC_AttackProp::sHD_BASE* >::iterator itEnd_Level = EventList.end();

		for( ; it_Level != itEnd_Level; ++it_Level )
		{
			CsC_AttackProp::sEVENT_INFO* pInfo = (CsC_AttackProp::sEVENT_INFO*)*it_Level;
			if( pInfo->s_eEventType == CsC_AttackProp::EVENT_LEVEL_UP )
			{
				cPrintMsg::PrintMsg( 30058 );	// 현재 파트너 디지몬을 교체 할 수 없습니다.
				return;
			}
		}
	}
#endif

	// 선행 체크
	switch( sync )
	{
		// 동시에 2개 보낼수 없다. ( 경고 있음 )
	case DIGIMON_EVOLUTION_OPEN:
	case DIGIMON_RIDE_OPEN:
	case ELEMENT_ITEM_CHECK:
	case CASH_REFRESH:
	case CASH_EXTRA_INVEN:
	case CASH_EXTRA_WAREHOUSE:
	case CASH_EXTRA_TACTICSHOUSE:
	case POSTAL_REFRESH:
	case CHANGE_PARTNER:
	case CHANGE_PARTNER_ING:	
#ifdef CROSSWARS_SYSTEM
	case CASH_EXTRA_CROSSINVEN:
	case CROSSCHANGE_PARTNER:
	case CROSSCHANGE_PARTNER_ING:
	case CROSSRECOVER_PARTNER:
	case CROSSRECOVER_PARTNER_ING:
#endif
	case CHANGE_PARTNER_WITHOUTSCENE:
	case DELETE_TACTICS:
	case CASH_CHANGE_TNAME:
	case GUILD_CREATE:
	case GUILD_INVATE:
	case GUILD_LEAVE:
	case GUILD_DESTROY:
	case GUILD_BAN:
	case GUILD_AP_MASTER:
	case GUILD_AP_2MASTER:	
	case GUILD_AP_DAT:
	case GUILD_AP_NORMAL:
	case GUILD_AP_NEWBI:
	case CASH_CAPSULE_COIN_COMPLEATE:
	case CASH_CAPSULE_COIN_WAIT:
	case CASH_CAPSULE_COIN_SERVER:
	case CASH_CAPSULE_COIN_CLIENT:
	case FRIEND_LIST:
	case SHUTOUT_LIST:
	case CASH_ITEM_BUY:	
	case CASH_ITEM_PRESENT:	

		// 동시에 2개 보낼 수 없다. ( 경고 없음 )
	case TACTICSHOUSE_LIST:
	case USE_DIGIMON_SKILL:	
	case RIDE_ON:
	case RIDE_OFF:
	case REWARD_REFRESH:	
	case CASH_EXTRA_CARDINVEN:
	case CASH_IDENTIFY_CARD:
	case CASH_CARD_RESEAL:
	case TACTICS_OPEN:	
	case MEMORY_SKILL_DELETE:
		if( IsEmptyRefCount( sync ) == false )
		{
			return;
		}
	break;

	case CHANGE_KFM:
	case CHANGE_KFM_ING:
		{
			if( nsCsGBTerrain::g_nSvrLibType != nLIB::SVR_BATTLE )
			{
				// 테이머 와의 거리를 체크하여 너무 멀면(2600) 진화 불가 메세지 띄움.
				NiPoint3 v = g_pCharMng->GetTamerUser()->GetPos() - g_pCharMng->GetDigimonUser(0)->GetPos();
				v.z = 0;
				float fTamerDist = v.Length();

				if( fTamerDist > DIGIMONUSER_FOLLOW_RANGE )
				{
#ifdef VERSION_KOR
					cPrintMsg::PrintMsg(13008 );
#else
					cPrintMsg::PrintMsg(13005 );
#endif
					return;
				}
			}

			if( IsEmptyRefCount( sync ) == false )
			{
				return;
			}
			break;
		}

		// 체크 없음	
	case TEMP_FRIEND_LOCAL_TO_SERVER:
	case GET_REWARD:
		break;

	default:
		assert_cs( false );
	}

	if( net::game )
		_EnableNet( sync, nRefConstant, pValue1, pValue2, pValue3, pValue4 );
	else
		_DisableNet( sync, nRefConstant, pValue1, pValue2, pValue3, pValue4 );	
}

void cData_ServerSync::_EnableNet( eSYNC sync, int nRefConstant, void* pValue1 /*=NULL*/, void* pValue2 /*=NULL*/, void* pValue3 /*=NULL*/, void* pValue4 /*=NULL*/ )
{
	m_Sync[ sync ].Increase( nRefConstant );

	switch( sync )
	{
	case CASH_IDENTIFY_CARD:
		{
			int nInvenIndex = *static_cast<int*>(pValue1);				// 씰 인벤토리 인덱스 값
			m_Sync[ sync ].s_nEtcValue1 = *static_cast<int*>(pValue2);	// 씰 사용량
			m_Sync[ sync ].s_nEtcValue2 = *static_cast<int*>(pValue3);	// 봉인 해제 아이템 사용 수량

			// 서버 파트와 협의로 최소한 인벤의 씰 위치만 알려줘도 됩니다.
			net::game->SendCardUnSeal( nInvenIndex );

			RecvServer( sync, nRefConstant, pValue1 , pValue2, pValue3);
		}
		break;

	case CASH_CARD_RESEAL:
		{
			m_Sync[ sync ].s_nEtcValue1 = *static_cast<int*>(pValue1);	// 카드 아이템ID
			m_Sync[ sync ].s_nEtcValue2 = *static_cast<int*>(pValue2);	// 카드 밀봉량
			int nDisCountReSealItem = *static_cast<int*>(pValue3);		// 밀봉 아이템 소모량

			// 보내기 전 카드ID를 구합니다.
			int nSendCardID = nsCsFileTable::g_pMaster_CardMng->GetMasterCard(m_Sync[ sync ].s_nEtcValue1)->GetInfo()->s_nID;

			net::game->SendReCardSeal( nSendCardID , m_Sync[ sync ].s_nEtcValue2 );

			RecvServer( sync, nRefConstant, pValue1, pValue2, pValue3 );
		}
		break;
	case DIGIMON_EVOLUTION_OPEN:
		{
			// 사용되는 오픈아이템 번호를 저장 시켜 놓는다.
			m_Sync[ sync ].s_nEtcValue1 = *(int*)pValue3;
			// 사용되는 오픈아이템 갯수를 저장 시켜 놓는다.
			m_Sync[ sync ].s_nEtcValue2 = *(int*)pValue2;

			int nEvoUnitArrIDX = *(int*)pValue1;
			// 아이템 闡렝?이미 걸려 있다
			net::game->SendOpenEvoSlot( nEvoUnitArrIDX );

			RecvServer( sync, nRefConstant, &nEvoUnitArrIDX );
		}		
		break;

	case DIGIMON_RIDE_OPEN:
		{
			// 사용되는 오픈아이템 번호를 저장 시켜 놓는다.
			m_Sync[ sync ].s_nEtcValue1 = *(int*)pValue3;
			// 사용되는 오픈아이템 갯수를 저장 시켜 놓는다.
			m_Sync[ sync ].s_nEtcValue2 = *(int*)pValue2;

			int nEvoUnitArrIDX = *(int*)pValue1;
			// 아이템 闡렝?이미 걸려 있다
			net::game->SendOpenRideMode( nEvoUnitArrIDX, m_Sync[ sync ].s_nEtcValue1 );

			RecvServer( sync, nRefConstant, &nEvoUnitArrIDX );
		}
		break;

	case ELEMENT_ITEM_CHECK:
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_ELEMENT_ITEM_CHECK, NULL );
		break;

	case CASH_REFRESH:
		g_pDataMng->GetCash()->SetRefreshDisable();
		net::game->SendReqCashShop();
		break;	
	case POSTAL_REFRESH:
		g_pDataMng->GetCash()->SetRefreshDisable();
		net::game->SendReqGiftShop();
		break;
	case REWARD_REFRESH:
		g_pDataMng->GetCash()->SetRefreshDisable();
		net::game->SendRequestRewardList();		
		break;
	case CHANGE_KFM:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
			{
				cPrintMsg::PrintMsg( 30028 );
				m_Sync[ sync ].Decrease( nRefConstant );
				return ;
			}
			uint nUniqID = *(uint*)pValue1;
			uint nEvoIndex = *(uint*)pValue2;
			net::game->SendEvolution( nUniqID, nEvoIndex );
		}		
		break;
#ifdef CROSSWARS_SYSTEM
	case CROSSCHANGE_PARTNER:
		{			
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
			{
				cPrintMsg::PrintMsg( 30028 );
				m_Sync[ sync ].Decrease( nRefConstant );
				return ;
			}

			int nCPIndex = *(int*)pValue1;		
			int	*nCPIndex2 = (int*)pValue2;

			if(1 == nCPIndex)
			{

// 				if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
// 				{
// 					int nDestLv = g_pDataMng->GetCrossTactics()->GetTactics( nCPIndex )->s_nLevel;
// 					int nSrcLv = g_pDataMng->GetBattle()->GetBattle_BaseDigimonLv();
// 					if( ( nDestLv - nSrcLv ) > 5 )
// 					{
// 						cPrintMsg::PrintMsg( 30474 );
// 						m_Sync[ sync ].Decrease( nRefConstant );
// 						return;
// 					}
// 				}
				
				net::game->SendCrossChangePartner( *nCPIndex2 );

				cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();
				pCrossTactics->ResetCrossIdx();
			}
			else
			{

				for(int i =0; i < nCPIndex; i++)
				{
					cData_CrossTactics::sCross* pData = g_pDataMng->GetCrossTactics()->GetTactics( nCPIndex2[i] );
					pData->s_nEnable = 0;

				}

				net::game->SendCrossChangePartner2( nCPIndex, nCPIndex2 );

				cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();
				pCrossTactics->ResetCrossIdx();
				
			}
		}		
		break;
#endif
	case CHANGE_PARTNER:
		{
			uint nCPIndex = *(int*)pValue1;
// 			if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
// 			{
// 				int nDestLv = g_pDataMng->GetTactics()->GetTactics( nCPIndex - 1 )->s_nLevel;
// 				int nSrcLv = g_pDataMng->GetBattle()->GetBattle_BaseDigimonLv();
// 				if( ( nDestLv - nSrcLv ) > 5 )
// 				{
// 					cPrintMsg::PrintMsg( 30474 );
// 					m_Sync[ sync ].Decrease( nRefConstant );
// 					return;
// 				}
// 			}

			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
			{
				cPrintMsg::PrintMsg( 30028 );
				m_Sync[ sync ].Decrease( nRefConstant );
				return ;
			}

			// 배틀 신청 중에는 디지몬 교체 못함.
			bool bValue = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_BATTLEMATCH_REGISTED, &bValue );
			if( bValue )
			{
				cPrintMsg::PrintMsg( 40014 );
				m_Sync[ sync ].Decrease( nRefConstant );
				return;
			}

			bool bBattlePlaying = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_BATTLE_PLAYING, &bBattlePlaying );
			if( bBattlePlaying )
			{
				cPrintMsg::PrintMsg( 40014 );
				m_Sync[ sync ].Decrease( nRefConstant );
				return;
			}
			
			// 최신 용병 강화 정보를 갱신한다. 데이터 교환에 필요한 최신 정보 갱신.
			// 용병 슬롯으로 돌아갈 경우 최신 정보가 갱신 되지 않아 추가함.	
			CDigimon* pCDigimon = g_pCharMng->GetDigimonUser( 0 );
			if(pCDigimon)
			{
				cData_PostLoad::sDATA* pCurData = g_pDataMng->GetPostLoad()->GetDigimonData();
				g_pDataMng->GetTactics()->ChangeStatus( pCurData, pCDigimon );
			}

			net::game->SendChangePartner( nCPIndex );
		}		
		break;
#ifdef CROSSWARS_SYSTEM
	case CROSSRECOVER_PARTNER:
		{			
			
			net::game->SendCrossWarsRecover();
		}
		break;
#endif
	case CHANGE_PARTNER_WITHOUTSCENE:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
			{
				cPrintMsg::PrintMsg( 30028 );
				m_Sync[ sync ].Decrease( nRefConstant );
				return ;
			}

			if(g_pCharMng->GetDigimonUser( 0 )->IsJointProgressing())
			{
				uint nUniqID = g_pGameIF->GetQuickEvol()->GetTargetObject()->GetUniqID();
				uint nEvoIndex = g_pGameIF->GetQuickEvol()->GetQuickEvolIndex();
				DWORD dwDestDigimonID = g_pGameIF->GetQuickEvol()->GetDestDigimonID();				

				g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CHANGE_KFM, 0, &nUniqID, &nEvoIndex, &dwDestDigimonID );	// 고유아이디, 퀵슬롯의 인덱스, 진화할아이디
			}			
		}
		break;
	case TACTICSHOUSE_LIST:
		{
#ifdef ITEM_USE_TIME_PASS	
			if(pValue1 != NULL)
			{
				int targetidx = *static_cast<int*>(pValue1);
				int invenidx = *static_cast<int*>(pValue2);
				if( -1 == invenidx )
					net::game->SendDigimonCareSlotList( targetidx, 0, true );
				else
					net::game->SendDigimonCareSlotList( targetidx, invenidx, false );
			}
			else
			{
				net::game->SendDigimonCareSlotList( 0 );
			}
#else
			net::game->SendDigimonCareSlotList( 0 );
#endif			
		}
		break;

	case CASH_CHANGE_TNAME:		
		// 사용 인벤 인덱스를 저장 시켜 놓는다.
		m_Sync[ sync ].s_nEtcValue1 = TO_INVEN_SID( *(int*)pValue1 );
		break;

	case GUILD_CREATE:
		{
			m_Sync[ sync ].s_nEtcValue1 = g_pDataMng->GetInven()->GetFirstSlot_Item_TypeLS( ITEM_TYPELS_GUILD_CREATE );
			if( ( m_Sync[ sync ].s_nEtcValue1 != cData_Inven::INVALIDE_INVEN_INDEX )&&
				g_pDataMng->GetInven()->GetCount_Item_TypeLS( ITEM_TYPELS_GUILD_CREATE ) >= nsCsFileTable::g_pBaseMng->GetGuildInfo( 1 )->s_nItemCount1
				&& g_pGameIF->GetTalk()->GetTarget() )
			{
				net::game->SendGuildCreate( (TCHAR*)pValue1, m_Sync[ sync ].s_nEtcValue1, g_pGameIF->GetTalk()->GetTarget()->GetModelID() );
			}
			else
			{
				int nCount = nsCsFileTable::g_pBaseMng->GetGuildInfo( 1 )->s_nItemCount1;
				cPrintMsg::PrintMsg( 30149, ITEM_STR_GUILD_CREATE, &nCount );
				bool bSuccess = false;
				RecvServer( sync, nRefConstant, &bSuccess );
			}
		}
		break;

	case GUILD_LEAVE:
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_GUILD_LEAVE, NULL );
		break;

	case GUILD_INVATE:
		{
			// 서버에 전송
			net::game->SendGuildReqAdd( (TCHAR*)pValue1 );
		}
		break;

	case GUILD_DESTROY:
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_GUILD_DESTROY, NULL );
		break;	

	case GUILD_BAN:
		//길드원 강퇴
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_GUILD_BAN, pValue1);
		break;
	case GUILD_AP_MASTER:
	case GUILD_AP_2MASTER:
	case GUILD_AP_NORMAL:
	case GUILD_AP_DAT:
	case GUILD_AP_NEWBI:
		{
			std::pair<cData_ServerSync::eSYNC, TCHAR*> sendPair;
			sendPair.first = sync;
			sendPair.second = (TCHAR*)pValue1;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::SEND_GUILDRANK_SET, &sendPair);
			break;
		}
	case USE_DIGIMON_SKILL:
		// 사용 스킬 인덱스를 저장 시켜 놓는다
		m_Sync[ sync ].s_nEtcValue1 = TO_INVEN_SID( *(int*)pValue1 );
		break;	

	case FRIEND_LIST:		
		{
			std::wstring friendName = (TCHAR*)pValue1;
			if( friendName.empty() )
			{
				m_Sync[ sync ].Decrease( nRefConstant );
				break;
			}
			std::pair<bool, std::wstring> addData = std::pair<bool, std::wstring>(true, friendName);
			GAME_EVENT_ST.OnEvent(EVENT_CODE::SEND_ADD_FRIEND_SHUTOUT, &addData);
		}
		break;

	case SHUTOUT_LIST:
		{
			std::wstring banName = (TCHAR*)pValue1;
			if( banName.empty() )
			{
				m_Sync[ sync ].Decrease( nRefConstant );
				break;
			}
			std::pair<bool, std::wstring> addData = std::pair<bool, std::wstring>(false, banName);
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::SEND_ADD_FRIEND_SHUTOUT, &addData);
		}
		break;

	case CASH_ITEM_BUY:
		{
			DWORD dwItemIdx = *static_cast<DWORD*>(pValue2);
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::SEND_CASHSHOP_ITEM_PURCHASE, &dwItemIdx);
		}
		break;

	case CASH_ITEM_PRESENT:
		{
			//CsCashShop* pItemInfo = nsCsFileTable::g_pCashShopMng->GetCashShop( g_pDataMng->GetCashShop()->m_dwPresentID );
			//assert_cs( pItemInfo != NULL );

			//if( g_pDataMng->GetCashShop()->GetCashMoney() < pItemInfo->GetInfo()->s_nPrice )
			//{
			//	// 캐쉬 모잘라서 아이템 못사요..
			//	RecvServer( sync, nRefConstant );
			//	cPrintMsg::PrintMsg( 30310 );
			//}
			//else
			//{
			//	net::game->SendGiftCashItem( g_pDataMng->GetCashShop()->m_dwPresentID, pItemInfo->GetInfo()->s_nPrice,
			//			g_pDataMng->GetCashShop()->m_szName, g_pDataMng->GetCashShop()->m_szMessage );										
			//}
		}
		break;

	case RIDE_ON:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
			{
				cPrintMsg::PrintMsg( 30028 );
				m_Sync[ sync ].Decrease( nRefConstant );
				return ;
			}
			net::game->SendRideOn();
			//RecvServer( cData_ServerSync::RIDE_ON, 0 );
		}
		break;

	case RIDE_OFF:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
			{
				cPrintMsg::PrintMsg( 30028 );
				m_Sync[ sync ].Decrease( nRefConstant );
				return ;
			}

			net::game->SendRideOff();
			//RecvServer( cData_ServerSync::RIDE_OFF, 0 );
		}break;
	case CASH_EXTRA_CARDINVEN:
		// 별다른 처리 없다	
	case CASH_EXTRA_INVEN:
	case CASH_EXTRA_WAREHOUSE:
	case CASH_EXTRA_TACTICSHOUSE:
	case CHANGE_KFM_ING:
	case CHANGE_PARTNER_ING:
#ifdef CROSSWARS_SYSTEM
	case CASH_EXTRA_CROSSINVEN:
	case CROSSCHANGE_PARTNER_ING:		
	case CROSSRECOVER_PARTNER_ING:
#endif
	case DELETE_TACTICS:
	case CASH_CAPSULE_COIN_COMPLEATE:
	case CASH_CAPSULE_COIN_WAIT:
	case CASH_CAPSULE_COIN_SERVER:
	case CASH_CAPSULE_COIN_CLIENT:
	case TEMP_FRIEND_LOCAL_TO_SERVER:
	case GET_REWARD:
	case TACTICS_OPEN:
		break;

	case MEMORY_SKILL_DELETE:
		break;

	default:
		assert_cs( false );
	}
}

void cData_ServerSync::_DisableNet( eSYNC sync, int nRefConstant, void* pValue1 /*=NULL*/, void* pValue2 /*=NULL*/, void* pValue3 /*=NULL*/, void* pValue4 /*=NULL*/ )
{
	m_Sync[ sync ].Increase( nRefConstant );

	switch( sync )
	{
		case CASH_IDENTIFY_CARD:
		case CASH_CARD_RESEAL:
			RecvServer( sync, nRefConstant, pValue1 , pValue2, pValue3);
			break;
	case DIGIMON_EVOLUTION_OPEN:
		{
			// 사용되는 오픈아이템 번호를 저장 시켜 놓는다.
			m_Sync[ sync ].s_nEtcValue1 = *(int*)pValue3;
			// 사용되는 오픈아이템 갯수를 저장 시켜 놓는다.
			m_Sync[ sync ].s_nEtcValue2 = *(int*)pValue2;

			int nEvoUnitArrIDX = *(int*)pValue1;
			RecvServer( sync, nRefConstant, &nEvoUnitArrIDX );
		}		
		break;

	case DIGIMON_RIDE_OPEN:
		{
			// 사용되는 오픈아이템 번호를 저장 시켜 놓는다.
			m_Sync[ sync ].s_nEtcValue1 = *(int*)pValue3;
			// 사용되는 오픈아이템 갯수를 저장 시켜 놓는다.
			m_Sync[ sync ].s_nEtcValue2 = *(int*)pValue2;

			int nEvoUnitArrIDX = *(int*)pValue1;
			RecvServer( sync, nRefConstant, &nEvoUnitArrIDX );
		}		
		break;
	
	case CHANGE_KFM:
		{
			uint nUniqID = *(uint*)pValue1;
			uint nEvoIndex = *(uint*)pValue2;
			DWORD nDestDigimonID = *(DWORD*)pValue3;

			CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );

			// 진화
			if( nEvoIndex < 5 )
			{				
				pDUser->Scene_Evol( nDestDigimonID, false );
				g_pCharMng->GetTamerUser()->Scene_Evol( pDUser, nDestDigimonID );
			}
			// 퇴화
			else
			{
				pDUser->Scene_Degenerate( nDestDigimonID );
			}
#ifdef CROSSWARS_SYSTEM
			g_pCrossEvent->EventStart( 71080 );
#endif

		}
		break;

	case TACTICSHOUSE_LIST:
		RecvServer( sync, 0 );
		break;

	case USE_DIGIMON_SKILL:
		// 사용 스킬 인덱스를 저장 시켜 놓는다
		m_Sync[ sync ].s_nEtcValue1 = TO_INVEN_SID( *(int*)pValue1 );
		break;
	
		// 별다른 처리 없다	
	case CHANGE_KFM_ING:
	case CASH_CHANGE_TNAME:
	case GUILD_CREATE:
	case GUILD_LEAVE:
	case GUILD_INVATE:
	case GUILD_DESTROY:
	case GUILD_BAN:
	case GUILD_AP_MASTER:
	case GUILD_AP_2MASTER:
	case GUILD_AP_DAT:
	case GUILD_AP_NORMAL:
	case GUILD_AP_NEWBI:
	case CASH_REFRESH:
	case POSTAL_REFRESH:
	case CASH_CAPSULE_COIN_COMPLEATE:
	case CASH_CAPSULE_COIN_WAIT:
	case CASH_CAPSULE_COIN_SERVER:
	case CASH_CAPSULE_COIN_CLIENT:
	case TEMP_FRIEND_LOCAL_TO_SERVER:
	case CASH_ITEM_BUY:	
	case CASH_ITEM_PRESENT:
		break;

		// 클라이언트 용으로 구현 안함
	case DELETE_TACTICS:	
	case CHANGE_PARTNER:
	case ELEMENT_ITEM_CHECK:
	case CHANGE_PARTNER_ING:
#ifdef CROSSWARS_SYSTEM
	case CROSSRECOVER_PARTNER:
	case CROSSCHANGE_PARTNER_ING:	
	case CROSSRECOVER_PARTNER_ING:
#endif
	case REWARD_REFRESH:
	case GET_REWARD:
		assert_cs( false );
		break;
	default:
		assert_cs( false );
	}
}

void cData_ServerSync::RecvServer( eSYNC sync, int nRefConstant, void* pValue1 /*=NULL*/, void* pValue2 /*=NULL*/, void* pValue3 /*=NULL*/, void* pValue4 /*=NULL*/ )
{
	if( IsEmptyRefCount( sync ) == true )
		return;

	assert_cs( g_pDataMng );
	assert_cs( g_pGameIF );

	switch( sync )
	{
	case CASH_IDENTIFY_CARD:
		{
			int nInvenIndex		=	*static_cast<int*>(pValue1);	// 깍아야하는 씰 아이템 인벤토리 인덱스
			int nUseSeal		=	*static_cast<int*>(pValue2);	// 씰 사용량
			int nUseUnSeal		=	*static_cast<int*>(pValue3);	// 봉인 해제 아이템 사용 수량

			assert_cs( g_pGameIF->IsActiveWindow(cBaseWindow::WT_CARDIDENTIFY) );
			// 카드 UI 처리, 인벤토리 처리
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CARDIDENTIFY ) )
			{
				g_pGameIF->GetCardIdentify()->RecvUnSealProcess(nInvenIndex, nUseSeal, nUseUnSeal);	
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CARDIDENTIFY );
			}
		}
		break;

	case CASH_CARD_RESEAL:
		{
			int nSealItemID = *static_cast<int*>(pValue1);	// 씰 아이템 ID
			int nCardAmount = *static_cast<int*>(pValue2);	// 씰 밀봉량
			int nDecreaseReSealAmount = *static_cast<int*>(pValue3);	// 봉인 아이템 사용량

			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CARDRESEAL ) )
			{
				g_pGameIF->GetCardReSeal()->RecvReSealProcess(nSealItemID, nCardAmount, nDecreaseReSealAmount);
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CARDRESEAL );
			}
		}
		break;
	case DIGIMON_EVOLUTION_OPEN:
		{
			// nRefConstant	:	0
			// pValue1		:	갱신된 진화슬롯 플래그
			int nOpenSlot = *(int*)pValue1;
			int nTypeL, nTypeS;
			CsItem::TypeS_to_TypeLS( m_Sync[ sync ].s_nEtcValue1, nTypeL, nTypeS );
			int nNeedCount = m_Sync[ sync ].s_nEtcValue2;

			CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );			

			// 진화 플래그 재설정
			cEvoUnit* pEvoUnit = pUser->GetAttributeEvoUnit( nOpenSlot );
			pEvoUnit->m_nSlotState = 1;
			pUser->SetEvoPlag( false, nOpenSlot, nOpenSlot+1 );

			// 퀵에볼
			g_pGameIF->GetQuickEvol()->SetTargetObject( pUser );


			// 디지몬 진화창
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_DIGIMON_DIGIMONSTATUS, NULL );

			// 사용된 아이템의 감소
			int nOpenItemID = g_pDataMng->GetInven()->GetData( g_pDataMng->GetInven()->GetFirstSlot_Item_TypeLS( nTypeL, nTypeS ) )->GetType();
			g_pDataMng->GetInven()->DecreaseItem_TypeLS( nTypeL, nTypeS, nNeedCount, false, false );

			// 메세지
			CDigimonEvolveObj* pEvoObj = pUser->GetFTEvol()->GetEvolveObjByEvoSlot( nOpenSlot );
			CsDigimon::sINFO* pDigimonInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( pEvoObj->m_dwID )->GetInfo();
			cPrintMsg::PrintMsg( 30040, nsCsFileTable::g_pItemMng->GetItem( nOpenItemID )->GetInfo()->s_szName, pDigimonInfo->s_szName );

			if( nTypeS == 2 )
			{
				DWORD dwMask = nsEFFECT::POS_CHARPOS | nsEFFECT::DIR_CAMERA_AXIS_Z | nsEFFECT::SPOS_BOUND_CENTER;
				switch( pEvoObj->m_dwID )
				{
				case CODE_AGUMON_BUST: 					
					g_pCharMng->GetDigimonUser( 0 )->GetProp_Effect()->AddEffect( "Shinegrey_BM\\Evo_Effect.nif", 1.0f, dwMask );					
					break;
				case CODE_LALAMON_BUST:
					g_pCharMng->GetDigimonUser( 0 )->GetProp_Effect()->AddEffect( "RosemonBM\\Evo_Effect.nif", 1.0f, dwMask );
					break;
				case CODE_GAOMON_BUST:
					g_pCharMng->GetDigimonUser( 0 )->GetProp_Effect()->AddEffect( "Mirage_BM\\Evo_Effect.nif", 1.0f, dwMask );
					break;
				case CODE_FALCOMON_BUST:
					g_pCharMng->GetDigimonUser( 0 )->GetProp_Effect()->AddEffect( "Ravemon_BM\\Evo_Effect.nif", 1.0f, dwMask );
					break;
				default:
					assert_cs( false );
					break;					
				}
				g_pCharMng->GetDigimonUser( 0 )->PlaySound( "game\\bust_evo.wav" );
				CAMERA_ST.AttachCamera( "Bustcamera.nif", false );
			}
			else // 에보류터 이펙트		
			{			
				DWORD dwMask = nsEFFECT::POS_CHARPOS | nsEFFECT::DIR_CAMERA_AXIS_Z;
				g_pCharMng->GetTamerUser()->GetProp_Effect()->AddEffect( "system\\Evo_Effect.nif", 1.0f, dwMask );
				g_pCharMng->GetTamerUser()->PlaySound( "game\\Evo_Item.wav" );
			}	

			// 퀘스트 체크
			// 합체 진화를위해 슬롯 열때마다 체크. 계속 열어서 선행조건이 맞으면 퀘스트 진행			
			g_pDataMng->GetQuest()->CalProcess();
			g_pDataMng->GetQuest()->RevQuestCheck();			

			// 도감 정보 갱신
			GS2C_RECV_ENCYCLOPEDIA_EVOLINFO recv;
			recv.iBaseDigimonID = pUser->GetBaseDigimonFTID();
			recv.iEvolSlot = nOpenSlot;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::ENCYCLOPEDIA_SET_EVOLINFO, &recv );			

			// 상점을 열고 슬롯을 열었을 때 할인 가격 변경
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::STORE_APPLY_DISCOUNT, pUser );
		}
		break;

	case DIGIMON_RIDE_OPEN:
		{
			// nRefConstant	:	0
			// pValue1		:	갱신된 진화슬롯 플래그
			int nOpenSlot = *(int*)pValue1;
			int nTypeL, nTypeS;
			CsItem::TypeS_to_TypeLS( m_Sync[ sync ].s_nEtcValue1, nTypeL, nTypeS );
			int	nOpenItem = nsCsFileTable::g_pItemMng->TypeT_to_Disp( m_Sync[ sync ].s_nEtcValue1 );
			int nNeedCount = m_Sync[ sync ].s_nEtcValue2;

			CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );			

			// 진화 플래그 재설정
			cEvoUnit* pEvoUnit = pUser->GetAttributeEvoUnit( nOpenSlot );
			pEvoUnit->m_nSlotState |= 8;
			pUser->SetEvoPlag( false, nOpenSlot, nOpenSlot+1 );

			// 퀵에볼
			g_pGameIF->GetQuickEvol()->SetTargetObject( pUser );

			// 디지몬 진화창
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_DIGIMON_DIGIMONSTATUS, NULL );

			// 사용된 아이템의 감소
			g_pDataMng->GetInven()->DecreaseItem_TypeLS( nTypeL, nTypeS, nNeedCount, false, false );			

			CTamerUser* pTamer = g_pCharMng->GetTamerUser();
			pTamer->SetAnimation( ANI::NPC_YES );
			pTamer->GetProp_Effect()->AddEffect( "System\\Tactics_success.nif", 1.0f, nsEFFECT::POS_CHARPOS );
			pTamer->PlaySound( "system\\Tactics_yes.wav" );

			DWORD dwOpenDigimonID = pUser->GetFTDigimon()->GetInfo()->s_dwDigimonID;

			CDigimonEvolveInfo* pEvInfo = pUser->GetFTEvol();
			if( pEvInfo )
			{
				CDigimonEvolveObj* EvoObj =pEvInfo->GetEvolveObjByEvoSlot( nOpenSlot );	//	슬롯 번호로 테이블에 저장된 진화 값을 얻는다.
				if( EvoObj )
					dwOpenDigimonID = EvoObj->m_dwID;
			}
			
			CsDigimon* pDigimonInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( dwOpenDigimonID );
			if( pDigimonInfo )
			{
				CsDigimon::sINFO* pInfo = pDigimonInfo->GetInfo();
				if( pInfo )// 메세지					
					cPrintMsg::PrintMsg( 30011, nsCsFileTable::g_pItemMng->GetItem( nOpenItem )->GetInfo()->s_szName, pInfo->s_szName );	
			}		
		}
		break;

	case ELEMENT_ITEM_CHECK:
		{
			// nRefConstant 값을 0으로 바꾸어 놓자. ( 0으로 다 집어 넣었다 )
			// pValue1 = nRate ( u1 )
			// pValue2 = nRemainMoney( n8 )

			u1 nRate = *(u1*)pValue1;
			n8 nRemainMoney = *(n8*)pValue2;

			ContentsStream kSend;
			kSend << nRate << nRemainMoney;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_ELEMENT_ITEM_CHECK, &kSend );
		}
		break;

	case CASH_REFRESH:
		{
			// pValue1 = nkind (n2)
			// pValue2 = cItemData Array
			n2 nKind = *(n2*)pValue1;
			cItemData* pArray = (cItemData*)pValue2;
			if( g_pDataMng )
			{
				cData_Cash* pCashWarehouse = g_pDataMng->GetCash();
				if( pCashWarehouse )
				{
					pCashWarehouse->ResetCash();
					pCashWarehouse->SetItemCashCount( nKind );
					cItemInfo* pCashData = pCashWarehouse->GetCashDataArray();
					for( int i=0; i<nKind; ++i )
						pCashData[ i ] = pArray[ i ];

					pCashWarehouse->UpdateTradeLimiteTime();
				}
			}

// 			g_pDataMng->GetCash()->ResetCash();
// 			g_pDataMng->GetCash()->SetItemCashCount( nKind );
// 			cItemInfo* pCashData = g_pDataMng->GetCash()->GetCashDataArray();
// 			for( int i=0; i<nKind; ++i )
// 			{
// 				pCashData[ i ] = pArray[ i ];
// 			}
		}
		break;
	case POSTAL_REFRESH:
		{
			n2 nKind = *(n2*)pValue1;
			cItemInfo* pInfo = (cItemInfo*)pValue2;

			//2017-03-15-nova
			GS2C_RECV_GIFT_DATA recv;
			recv.SetData(pInfo, nKind);
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REWARD_RECEIVE_GIFT, &recv );
		}
		break;

	case REWARD_REFRESH:
		{			
		}
		break;
	case CHANGE_KFM:
		{
			int nSuccess = *(int*)pValue1;
			// 성공일 시에만
			if( nSuccess == 1 )
			{
				SendServer( cData_ServerSync::CHANGE_KFM_ING, 0 );
			}
		}
		break;
#ifdef CROSSWARS_SYSTEM
	case CROSSCHANGE_PARTNER:
		{
			sCHANGE_PARTNER* pSync = (sCHANGE_PARTNER*)pValue1;

			// 교체 성공
			if( pSync != NULL )
			{
				CsC_AvObject* pDigimon = g_pMngCollector->GetObject( pSync->nUID );
				if( pDigimon == NULL )
				{
					assert_cs( false );
					return;
				}

				assert_cs( pDigimon->GetLeafRTTI() == RTTI_DIGIMON_USER );

				( (CDigimon*)pDigimon )->GetBuff()->ReleaseBuff( nsBuff::BK_CANDY_SIZE_CHANGE );

				bool IsCrossCombine = nsCsFileTable::g_pDigimonMng->IsCrossCombine(pSync->nNextType);
				// 합체체면
				if(IsCrossCombine)
				{
					// 크로스 디지몬 교체 전 데이터 설정
					( (CDigimon*)pDigimon )->Scene_CrossChangePartner2( pSync->nNextType, pSync->nNextArrIDX, pSync->szNextDigimonName, pSync->nNextScale*0.0001f, pSync->nNextLevel );
					//( (CDigimon*)pDigimon )->WithoutScene_CrossChangePartner2( pSync->nNextType, pSync->nNextArrIDX, pSync->szNextDigimonName, pSync->nNextScale*0.0001f, pSync->nNextLevel );
				}
				else
				{
					// 크로스 디지몬 교체 전 데이터 설정
					( (CDigimon*)pDigimon )->Scene_CrossChangePartner( pSync->nNextType, pSync->nNextArrIDX, pSync->szNextDigimonName, pSync->nNextScale*0.0001f, pSync->nNextLevel );			
					//SendServer( CROSSCHANGE_PARTNER_ING, 0 );
				}
				
				// 크로스 디지몬 교체시 테이머 몸 동작
				CTamer* pTamer = g_pCharMng->GetTamerUser();
				assert_cs( pTamer );				
				pTamer->Scene_Evol( pDigimon, 0 );
				// 내부 프로토콜 보내기. 크로스 디지몬 교체 중이다 
				SendServer( CROSSCHANGE_PARTNER_ING, 0 );

				// 인터페이스 닫아주자
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TALK );
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_REV );
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_COMP );								
			}			
		}
		break;	
#endif
	case CHANGE_PARTNER:
		{
			sCHANGE_PARTNER* pSync = (sCHANGE_PARTNER*)pValue1;

			// 교체 성공
			if( pSync != NULL )
			{
				CsC_AvObject* pDigimon = g_pMngCollector->GetObject( pSync->nUID );
				if( pDigimon == NULL )
				{
					assert_cs( false );
					return;
				}

				assert_cs( pDigimon->GetLeafRTTI() == RTTI_DIGIMON_USER );

				( (CDigimon*)pDigimon )->GetBuff()->ReleaseBuff( nsBuff::BK_CANDY_SIZE_CHANGE );

				( (CDigimon*)pDigimon )->Scene_ChangePartner( pSync->nNextType, pSync->nNextArrIDX - 1, pSync->szNextDigimonName, pSync->nNextScale*0.0001f, pSync->nNextLevel );				
				CTamer* pTamer = g_pCharMng->GetTamerUser();
				assert_cs( pTamer );

				if( nsCsFileTable::g_pDigimonMng->GetDigimon( pSync->nNextType )->GetInfo()->s_eEvolutionType == nsCsDigimonTable::ET_SPIRIT
#ifdef SDM_TAMER_XGUAGE_20180628
					|| nsCsFileTable::g_pDigimonMng->GetDigimon( pSync->nNextType )->GetInfo()->s_eEvolutionType == nsCsDigimonTable::ET_MIX 
#endif
					)
					pTamer->Scene_Evol( pDigimon, pSync->nNextType );
				else
					pTamer->Scene_Evol( pDigimon, 0 );

				SendServer( CHANGE_PARTNER_ING, 0 );

				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TALK );
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_REV );
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_COMP );								
			}			
		}
		break;
#ifdef CROSSWARS_SYSTEM
	case CROSSRECOVER_PARTNER:
		{
			sCHANGE_PARTNER* pSync = (sCHANGE_PARTNER*)pValue1;

			// 교체 성공
			if( pSync != NULL )
			{
				CsC_AvObject* pDigimon = g_pMngCollector->GetObject( pSync->nUID );
				if( pDigimon == NULL )
				{
					assert_cs( false );
					return;
				}

				assert_cs( pDigimon->GetLeafRTTI() == RTTI_DIGIMON_USER );

				( (CDigimon*)pDigimon )->GetBuff()->ReleaseBuff( nsBuff::BK_CANDY_SIZE_CHANGE );

				( (CDigimon*)pDigimon )->Scene_CrossRecoverPartner( pSync->nNextType, pSync->nNextArrIDX, pSync->szNextDigimonName, pSync->nNextScale*0.0001f, pSync->nNextLevel );				
				CTamer* pTamer = g_pCharMng->GetTamerUser();
				assert_cs( pTamer );
				pTamer->Scene_Evol( pDigimon, 0 );
				SendServer( CROSSRECOVER_PARTNER_ING, 0 );

				// 인터페이스 닫아주자
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TALK );
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_REV );
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_COMP );								
			}
		}
		break;
#endif
	case CHANGE_PARTNER_WITHOUTSCENE:
		{
			sCHANGE_PARTNER* pSync = (sCHANGE_PARTNER*)pValue1;

			// 파트너 채인지 시작
			if( pSync != NULL )
			{
				CsC_AvObject* pDigimon = g_pMngCollector->GetObject( pSync->nUID );
				if( pDigimon == NULL )
				{
					assert_cs( false );
					return;
				}				

				assert_cs( pDigimon->GetLeafRTTI() == RTTI_DIGIMON_USER );	


				( (CDigimon*)pDigimon )->GetBuff()->ReleaseBuff( nsBuff::BK_CANDY_SIZE_CHANGE );

				( (CDigimon*)pDigimon )->WithoutScene_ChangePartner( pSync->nNextType, pSync->nNextArrIDX - 1, pSync->szNextDigimonName, pSync->nNextScale*0.0001f, pSync->nNextLevel );				
				CTamer* pTamer = g_pCharMng->GetTamerUser();
				assert_cs( pTamer );
				//pTamer->Scene_Evol( pDigimon, 0 );
				//SendServer( CHANGE_PARTNER_ING, 0 );

				// 인터페이스 닫아주자
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TALK );
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_REV );
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_COMP );
			}	

		}
		break;
	case CASH_CHANGE_TNAME:
		{
			bool bSuccess = *(bool*)pValue1;
			g_pDataMng->ItemUnlock( m_Sync[ sync ].s_nEtcValue1 ); // 저장해논 사용한 아이템 위치
			if( bSuccess == true )
			{
				g_pDataMng->GetInven()->GetData( m_Sync[ sync ].s_nEtcValue1 )->DecreaseCount( 1 );
			}
		}
		break;

	case GUILD_CREATE:
		{
			std::pair<bool, int> createInfo;
			createInfo.first = *(bool*)pValue1;
			createInfo.second = m_Sync[ sync ].s_nEtcValue1;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_CREATE, &createInfo );
		}
		break;

	case GUILD_LEAVE:	
		break;

	case CASH_CAPSULE_COIN_SERVER:
		{
			cItemData* pItemData = (cItemData*)pValue1;
			g_pDataMng->GetInven()->SetCapsuleItem( *pItemData );
		}
		break;	
#ifdef GUILD_RENEWAL
	case GUILD_DESTROY:
		{
		}
#else
	case GUILD_DESTROY:
#endif
	case CASH_EXTRA_CARDINVEN:
		// 별다른 처리 없다
	case CASH_EXTRA_INVEN:
	case DELETE_TACTICS:
	case USE_DIGIMON_SKILL:	
	case CASH_EXTRA_WAREHOUSE:
	case CASH_EXTRA_TACTICSHOUSE:
	case CHANGE_KFM_ING:
	case CHANGE_PARTNER_ING:
#ifdef CROSSWARS_SYSTEM
	case CASH_EXTRA_CROSSINVEN:
	case CROSSCHANGE_PARTNER_ING:
	case CROSSRECOVER_PARTNER_ING:
#endif
	case TACTICSHOUSE_LIST:			
	case GUILD_INVATE:

	case GUILD_BAN:	
	case GUILD_AP_MASTER:
	case GUILD_AP_2MASTER:
	case GUILD_AP_DAT:
	case GUILD_AP_NORMAL:
	case GUILD_AP_NEWBI:
	case CASH_CAPSULE_COIN_COMPLEATE:
	case CASH_CAPSULE_COIN_WAIT:
	case CASH_CAPSULE_COIN_CLIENT:
	case FRIEND_LIST:
	case SHUTOUT_LIST:
	case TEMP_FRIEND_LOCAL_TO_SERVER:
	case CASH_ITEM_BUY:	
	case CASH_ITEM_PRESENT:
	case RIDE_ON:
	case RIDE_OFF:
	case GET_REWARD:
	case MEMORY_SKILL_DELETE:
		break;

	default:
		assert_cs( false );
	}
	m_Sync[ sync ].Decrease( nRefConstant );
}
	
cData_ServerSync::sSYNC* cData_ServerSync::GetSyncData( eSYNC sync )
{
	assert_cs( IsEmptyRefCount( sync ) == false );
	return &m_Sync[ sync ];
}
void cData_ServerSync::sSYNC::Increase( int nRefConstant )
{
	++s_nRefCount;
	s_Vector.PushBack( nRefConstant );
}
void cData_ServerSync::sSYNC::Decrease( int nRefConstant )
{
	assert_cs( s_nRefCount > 0 );
	--s_nRefCount;
	s_Vector.DeleteElement_FromData( nRefConstant );
}
