
#include "stdafx.h"
#include "DataMng.h"

#include "Flow/FlowMgr.h"
#include "Flow/Flow.h"

#include "_Interface/Adapt/AdaptWareHouse.h"
#include "_Interface/Adapt/AdaptDigimonAchive.h"
#include "_Interface/Adapt/AdaptEnchantOption.h"
cDataMng*	g_pDataMng = NULL;

#define		SORT_COOLTIME	20.0f

void cDataMng::GlobalInit()
{
	cData_Quest::GlobalInit();

	assert_cs( g_pDataMng == NULL );
	g_pDataMng = NiNew cDataMng;
	g_pDataMng->Init();	
}

void cDataMng::GlobalShotdown()
{
	if( g_pDataMng )
		g_pDataMng->Delete();
	SAFE_NIDELETE( g_pDataMng );

	cData_Quest::GlobalShotDown();
}

void cDataMng::Delete()
{
	m_Inven.Delete();
	m_TEquip.Delete();
	m_Digivice.Delete();
	m_PostLoad.Delete();	
	m_Quest.Delete();
	m_QuestOwner.Delete();
	m_Tactics.Delete();
	m_MapRegion.Delete();
	m_ServerSync.Delete();
	m_Cash.Delete(); //제거예정
	m_Attendance.Delete();

	GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_DELETE_DATA, NULL );

#ifdef GUILD_MARK_DOWN
	m_GuildMark.Delete();	
#endif

#ifdef CROSSWARS_SYSTEM
	m_InvenCross.Delete();
	m_CrossTatics.Delete();
#endif
}

void cDataMng::Init()
{
	m_Inven.Init();
	m_TEquip.Init();
	m_Digivice.Init();
	m_PostLoad.Init();
	m_Quest.Init();
	m_QuestOwner.Init();
	m_Tactics.Init();
	m_MapRegion.Init();
	m_ServerSync.Init();
	m_Cash.Init();
	m_Attendance.Init();

#ifdef PLAY_PANELTY
	m_nPlay_NormalTime = m_nPlay_CashTime = m_nPlay_BasePaneltyTime = m_nPlay_BaseFullTime = 0;
	m_nPlay_PaneltyType = nPlayTime::NO_PANELTY;
#endif

#ifdef GUILD_MARK_DOWN
	m_GuildMark.Init();
#endif

	m_SortSeq.SetEndTime( SORT_COOLTIME );

#ifdef CROSSWARS_SYSTEM
	m_InvenCross.Init();
	m_CrossTatics.Init();
#endif
	ReleaseItemLock();	

	m_MapObject.Init();
	
	m_pWarehouseAdapt = new AdaptWareHouse;

	if( m_pWarehouseAdapt )
	{
		m_pWarehouseAdapt->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_WAREHOUSE ) );
	}

	m_pDigimonArchiveAdapt = new AdaptDigimonArchiveSystem;
	if(m_pDigimonArchiveAdapt)
		m_pDigimonArchiveAdapt->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_DIGIMON_ARCHIVE_CONTENTS ) );

	m_pEnchantOptionAdapt = new AdaptEnchantOption;
	if( m_pEnchantOptionAdapt )
		m_pEnchantOptionAdapt->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_ENCHANT_OPTION_CONTENTS ) );

	m_nItemTryCount = 0;
}

void cDataMng::_InitNetOff()
{
	m_Inven.InitNetOff();
#ifdef CROSSWARS_SYSTEM
	m_InvenCross.InitNetOff();
	m_CrossTatics.InitNetOff();
#endif
	m_TEquip.InitNetOff();
	m_Digivice.InitNetOff();
	m_PostLoad.InitNetOff();
	m_Quest.InitNetOff();
	m_Tactics.InitNetOff();
	m_MapRegion.InitNetOff();
	m_Cash.InitNetOff();
	m_Attendance.InitNetOff();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_INITNET_OFF, NULL );

	m_SortSeq.SetEndTime( SORT_COOLTIME );
}

// 로딩 시작부에 지형에서 라이트랑 이런거 제거 하고 난후 호출 된다
void cDataMng::Reset()
{
#ifndef UI_INVENTORY_RENEWAL
	m_Inven.Reset();
#endif
	m_TEquip.Reset();
	m_Digivice.Reset();
	m_PostLoad.Reset();
	m_ServerSync.Reset();
	m_Cash.Reset();
	
	m_ListItem.clear();
#ifdef CROSSWARS_SYSTEM
	m_CrossListItem.clear();
#endif

	m_nSort = 0;
	m_SortSeq.Start();

	ReleaseItemLock();
	m_nItemTryCount = 0;

	bool bIsEnable = true;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable );

	if( IsNetOff() == true )
		_InitNetOff();
}

void cDataMng::LogOut()
{
	m_MapRegion.LogOut();

	GAME_EVENT_ST.OnEvent( EVENT_CODE::UNION_DELETE_DATA, NULL );

#ifdef PLAY_PANELTY
	m_nPlay_NormalTime = m_nPlay_CashTime = m_nPlay_BasePaneltyTime = m_nPlay_BaseFullTime = 0;
	m_nPlay_PaneltyType = nPlayTime::NO_PANELTY;	
#endif

	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::ENCYCLOPEDIA_LOGOUT, NULL );
}

void cDataMng::Update()
{
	m_QuestOwner.Update();

#ifdef GUILD_MARK_DOWN
	m_GuildMark.Update();
#endif
	m_ServerSync.Update();

	//////////////////////////////////////////////////////////////////////////
	/// 임시 코드
	//////////////////////////////////////////////////////////////////////////
	// 소트 횟수 지정하여 초당 몇번 소트 할지 결정한다.
	if( IsSort() == true)
	{
		if( m_NewSort.Update() )	// 0.1초마다 1회 정렬 실행.
			_SendSort();
	}
}

void cDataMng::DiedAlready()
{
	ReleaseItemLock();
	CDigimonUser* pUser;
	for( int i=0; i<MAX_DIGIMONUSER_COUNT; ++i )
	{
		pUser = g_pCharMng->GetDigimonUser( i );
		if( pUser )
			pUser->ReleaseSrvSendSkill();
	}	
}

NiColor cDataMng::GetItemColor( CsItem::sINFO const* pFTItem )
{
	SAFE_POINTER_RETVAL( pFTItem, FONT_WHITE );
	return GetItemColor( pFTItem->s_nClass );
}

NiColor cDataMng::GetItemColor( int nItemClass )
{
	switch( nItemClass )
	{
	case 1:		return NiColor( 167/255.0f, 167/255.0f, 167/255.0f );
	case 2:		return FONT_WHITE;
	case 3:		return NiColor( 34/255.0f, 255/255.0f, 73/255.0f );
	case 4:		return NiColor( 28/255.0f, 229/255.0f, 255/255.0f );
	case 5:		return NiColor( 255/255.0f, 41/255.0f, 178/255.0f );
	case 6:		return NiColor( 225/255.0f, 118/255.0f, 72/255.0f );
	case 7:		return NiColor( 255/255.0f, 212/255.0f, 72/255.0f );
	//case 8:	return NiColor( 255/255.0f, 102/255.0f, 255/255.0f );
	case 9:		return NiColor( 255/255.0f, 0, 87.0f/255.0f );
	case 10:	return NiColor( 255/255.0f, 240/255.0f, 0 );
	case 11:	return FONT_WHITE;
	case 12:	return FONT_WHITE;
	case 13:	return NiColor( 255/255.0f, 240/255.0f, 0 );
	}
	assert_csm1( false, _T( "Item Class Color Error = %d" ), nItemClass );
	return FONT_WHITE;
}

void cDataMng::ItemAllLock()
{
	int nCount = GetInven()->GetInvenSlotCount();
	for( int i=0; i<nCount; ++i )
	{
		ItemLock( TO_INVEN_SID( i ) );
	}
}

void cDataMng::ItemLock( int nLockIndex )
{	
	int nCount = m_vpItemLockIndex.Size();
	for( int i=0; i<nCount; ++i )
	{
		// 같은게 있다면 리턴
		if( m_vpItemLockIndex[ i ] == nLockIndex )
		{
			//assert_cs( false );
			return;
		}
	}
	m_vpItemLockIndex.PushBack( nLockIndex );
}

void cDataMng::ItemUnlock( int nLockIndex )
{
	m_vpItemLockIndex.DeleteElement_FromData( nLockIndex );
}

bool cDataMng::IsItemLock( int nLockIndex )
{
	if( m_vpItemLockIndex.Size() == 0 )
		return false;
	return m_vpItemLockIndex.IsExistData( nLockIndex );
}

bool cDataMng::_IsEnableItemMove( int nSrcSrvID, int nDestSrvID )
{
	assert_cs( IsItemLock( nSrcSrvID ) == false );
	assert_cs( IsItemLock( nDestSrvID ) == false );

	switch( TO_CONSTANT( nSrcSrvID ) )
	{
#ifdef CROSSWARS_SYSTEM
	case SERVER_DATA_INVENCROSS_CONSTANT:
		if( TO_ID( nSrcSrvID ) >= (int)GetInvenCross()->GetInvenSlotCount() )
		{
			// 데스트는 비여 있어야 한다.
			switch( TO_CONSTANT( nDestSrvID ) )
			{
			case SERVER_DATA_INVENCROSS_CONSTANT:
				if( g_pDataMng->GetInvenCross()->GetData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;			
			default:
				assert_cs( false );
			}
		}
		break;
#endif
	case SERVER_DATA_INVEN_CONSTANT:		
		if( TO_ID( nSrcSrvID ) >= (int)GetInven()->GetInvenSlotCount() )
		{
			// 데스트는 비여 있어야 한다.
			switch( TO_CONSTANT( nDestSrvID ) )
			{
#ifdef CROSSWARS_SYSTEM
			case SERVER_DATA_INVENCROSS_CONSTANT:
				if( g_pDataMng->GetInvenCross()->GetData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;
				break;
#endif
			case SERVER_DATA_INVEN_CONSTANT:
				if( g_pDataMng->GetInven()->GetData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;
				break;
			case SERVER_DATA_WAREHOUSE_CONSTANT:
//				if( g_pDataMng->m_pWarehouseAdapt->GetData( TO_ID( nDestSrvID ) )->IsEnable() == true )
				if( m_pWarehouseAdapt->GetData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;				
				break;
#ifdef NEW_SHARESTASH
			case SERVER_DATA_SHARESTASH_CONSTANT:
				if( m_pWarehouseAdapt->GetShareData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;				
				break;
#endif
			case SERVER_DATA_TEQUIP_CONSTANT:
				if( g_pDataMng->GetTEquip()->GetData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;
			case SERVER_DATA_MEMORY_CONSTANT:
				if( g_pDataMng->GetTactics()->GetTactics( TO_ID( nDestSrvID ) )->s_Type != 0 )
					return false;
			case SERVER_DATA_CHIPSET_CONSTANT:
				if( g_pDataMng->GetDigivice()->GetChipset( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	case SERVER_DATA_WAREHOUSE_CONSTANT:		
		if( TO_ID( nSrcSrvID ) >= (int)m_pWarehouseAdapt->GetSlotCount() )
		{
			// 데스트는 비여 있어야 한다.
			switch( TO_CONSTANT( nDestSrvID ) )
			{
			case SERVER_DATA_INVEN_CONSTANT:
				if( g_pDataMng->GetInven()->GetData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;
				break;
			case SERVER_DATA_WAREHOUSE_CONSTANT:
				if( m_pWarehouseAdapt->GetData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;
				break;
#ifdef NEW_SHARESTASH
			case SERVER_DATA_SHARESTASH_CONSTANT:
				if( m_pWarehouseAdapt->GetShareData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;				
				break;
#endif
			case SERVER_DATA_TEQUIP_CONSTANT:
				if( g_pDataMng->GetTEquip()->GetData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;
			case SERVER_DATA_MEMORY_CONSTANT:
				if( g_pDataMng->GetTactics()->GetTactics( TO_ID( nDestSrvID ) )->s_Type != 0 )
					return false;
			case SERVER_DATA_CHIPSET_CONSTANT:
				if( g_pDataMng->GetDigivice()->GetChipset( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;
				break;
			default:
				assert_cs( false );
			}
		}
		break;



#ifdef NEW_SHARESTASH
	case SERVER_DATA_SHARESTASH_CONSTANT:		
		if( TO_ID( nSrcSrvID ) >= (int)m_pWarehouseAdapt->GetShareSlotCount() )
		{
			// 데스트는 비여 있어야 한다.
			switch( TO_CONSTANT( nDestSrvID ) )
			{
			case SERVER_DATA_INVEN_CONSTANT:
				if( g_pDataMng->GetInven()->GetData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;
				break;
			case SERVER_DATA_WAREHOUSE_CONSTANT:
				if( m_pWarehouseAdapt->GetData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;
				break;
			case SERVER_DATA_SHARESTASH_CONSTANT:
				if( m_pWarehouseAdapt->GetShareData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;				
				break;
			case SERVER_DATA_TEQUIP_CONSTANT:
				if( g_pDataMng->GetTEquip()->GetData( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;
			case SERVER_DATA_MEMORY_CONSTANT:
				if( g_pDataMng->GetTactics()->GetTactics( TO_ID( nDestSrvID ) )->s_Type != 0 )
					return false;
			case SERVER_DATA_CHIPSET_CONSTANT:
				if( g_pDataMng->GetDigivice()->GetChipset( TO_ID( nDestSrvID ) )->IsEnable() == true )
					return false;
				break;
			default:
				assert_cs( false );
			}
		}
		break;
#endif

	case SERVER_DATA_TEQUIP_CONSTANT:
	case SERVER_DATA_MEMORY_CONSTANT:
	case SERVER_DATA_CHIPSET_CONSTANT:
	case SERVER_DATA_DIGIVICE_CONSTANT:
		break;
	default:
		assert_cs( false );
	}

	switch( TO_CONSTANT( nDestSrvID ) )
	{
	case SERVER_DATA_INVEN_CONSTANT:
		if( TO_ID( nDestSrvID ) >= (int)GetInven()->GetInvenSlotCount() )
			return false;
		break;
	case SERVER_DATA_WAREHOUSE_CONSTANT:
		if( TO_ID( nDestSrvID ) >= (int)m_pWarehouseAdapt->GetSlotCount() )
			return false;				
		break;
#ifdef NEW_SHARESTASH
	case SERVER_DATA_SHARESTASH_CONSTANT:
		if( TO_ID( nDestSrvID ) >= (int)m_pWarehouseAdapt->GetShareSlotCount() )
			return false;				
		break;
#endif
#ifdef CROSSWARS_SYSTEM
	case SERVER_DATA_INVENCROSS_CONSTANT:
		if( TO_ID( nDestSrvID ) >= (int)GetInvenCross()->GetInvenSlotCount() )
			return false;				
		break;
#endif
	case SERVER_DATA_TEQUIP_CONSTANT:
	case SERVER_DATA_MEMORY_CONSTANT:
	case SERVER_DATA_CHIPSET_CONSTANT:
	case SERVER_DATA_DIGIVICE_CONSTANT:
		break;
	default:
		assert_cs( false );
	}

	return true;
}

bool cDataMng::IsItemUse( int nSrvIndex )
{
	if(FLOWMGR_ST.GetCurTopFlowID() == Flow::CFlow::FLW_LOADING )
		return false;

	if(FLOWMGR_ST.GetCurTopFlowID() == Flow::CFlow::FLW_DATSCENTER || 
	   FLOWMGR_ST.GetCurTopFlowID() == Flow::CFlow::FLW_DATATRADECENTER )
	{
		cPrintMsg::PrintMsg( 10004 );
		return false;
	}

	//=============================================================================================================
	//	튜토리얼 여부
	//=============================================================================================================

	bool bTutorialPlaying = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_IS_TUTORIAL_PLAYING, &bTutorialPlaying );
	if( bTutorialPlaying )
	{
		cPrintMsg::PrintMsg( 30412 );
		return false;
	}

	//=============================================================================================================
	//	사망 여부
	//=============================================================================================================
	if( g_pCharMng->GetDigimonUser( 0 )->IsServerDie() == true )
	{
		return false;
	}

	//=============================================================================================================
	//	존재 여부
	//=============================================================================================================
	cItemInfo* pItem = SrvID2ItemInfo( nSrvIndex );
	if( pItem == NULL )
		return false;

	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();

	//=============================================================================================================
	//	배틀시 사용 유무 체크
	//=============================================================================================================
	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
	{
		if( pFTItem->GetInfo()->s_nUseBattle == 0 )
		{
			cPrintMsg::PrintMsg( 30455 );
			return false;
		}
	}

	//=============================================================================================================
	//	상태 이상 체크
	//=============================================================================================================
	if( g_pCharMng->GetDigimonUser( 0 )->GetBuff()->IsUnusualCond( cBuffData::UNUSUAL_ITEM ) == true )
	{
		cPrintMsg::PrintMsg( 30455 );
		return false;
	}	

	//=============================================================================================================
	//	사용 시기 체크
	//=============================================================================================================
	switch( pFTInfo->s_nUseMode )
	{
	case 0:				// 아무때나
		break;
	case 1:				// 전투중
		if( g_pCharMng->GetTamerUser()->IsBattle() == true )
		{
			cPrintMsg::PrintMsg( 16002 );
			cDigimonTalk::Print( 30011 );
			return false;
		}
		break;
	case 2:
		assert_cs( false );
		if( g_pCharMng->GetTamerUser()->IsBattle() == false )
		{
			// 메세지 몇번??????
			cPrintMsg::PrintMsg( 16002 );
			return false;
		}
		break;
	default:
		assert_cs( false );
	}

	//=============================================================================================================
	//	타겟
	//=============================================================================================================
	CsC_AvObject* pTarget = NULL;
	switch( pFTInfo->s_nUseCharacter )
	{
	case 0:		// 해당 사항 없음
	case 1:		// 둘다 사용
		pTarget = g_pCharMng->GetTamerUser();		
		break;
	case 2:		// 디지몬 사용 가능
		pTarget = g_pCharMng->GetDigimonUser( 0 );
		break;
	case 3:		// 테이머 사용 가능		
		pTarget = g_pCharMng->GetTamerUser();		
		break;		
	default:
		assert_cs( false );
	}

	//=============================================================================================================
	//	레벨
	//=============================================================================================================	
	//장비창에서 뺄때는 레벨 체크 안할꺼야 그냥 뺄 수 있어
	if( nItem::TabEquip != ( nSrvIndex / 1000 ) ) 
	{
		switch( pFTInfo->s_nType_L )
		{			
		case nItem::Chipset:		break;// 칩셋은 레벨 검사 안함
		case nItem::Head:// 장비	
		case nItem::Coat:
		case nItem::Glove:
		case nItem::Pants:
		case nItem::Shoes:
		case nItem::Costume:
		case nItem::Glass:
		case nItem::Necklace:
		case nItem::Earring:
		case nItem::Ring:
		case nItem::EquipAura: //오라 추가 chu8820
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
		case nItem::Bracelet:
#endif
			{
				/*감정했는지 체크*/
				if( pItem->m_nRate == 0 )
					break;
			}
		default:
			{
				if( !_CheckItemUseLevel( pItem ) )
					return false;
			}break;
		}
	}	

	//=============================================================================================================
	//	퀘스트
	//=============================================================================================================
	if( pFTInfo->s_nQuestRequire )
	{
		if( g_pDataMng->GetQuest()->IsCompleate( pFTInfo->s_nQuestRequire ) == false )
		{
			if( nsCsFileTable::g_pQuestMng->IsQuest( pFTInfo->s_nQuestRequire ) )
			{				
				cPrintMsg::PrintMsg( 14008, nsCsFileTable::g_pQuestMng->GetQuest( pFTInfo->s_nQuestRequire )->m_szTitleText );
			}
			else
			{
				assert_csm1( false, _T( "아이템 요구 퀘스트 %d가 존재하지 않습니다" ), pFTInfo->s_nQuestRequire );
			}			
			return false;
		}
	}


	//=============================================================================================================
	//	쿨타임
	//=============================================================================================================
	if( pFTInfo->s_nUseTimeGroup != 0 )
	{
		if( pFTInfo->GetCoolTimeSeq()->IsCoolTime() )
		{
			// 쿨타임 중입니다.
			cPrintMsg::PrintMsg( 13002 );
			return false;
		}
	}

	//=============================================================================================================
	//	사용시간
	//=============================================================================================================
	// 단 장비 아이템이 아닐경우에
	switch( pFTInfo->s_nType_L )
	{
		// 장비
	case nItem::Head:
	case nItem::Coat:
	case nItem::Glove:
	case nItem::Pants:
	case nItem::Shoes:
	case nItem::Costume:
	case nItem::Glass:
	case nItem::Necklace:
	case nItem::Earring:
	case nItem::Ring:
	case nItem::EquipAura: //오라 추가 chu8820
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
	case nItem::Bracelet:
#endif
		break;
	default:
		{
			if( cItemInfo::IsUseItemTime( pFTInfo ) )
			{		
				if( pItem->m_nEndTime == ITEMINFO_INVALIDE_TIME )
				{
					cPrintMsg::PrintMsg( 30362 );
					return false;
				}
			}
		}
	}

	return true;
}

cItemInfo* cDataMng::SrvID2ItemInfo( int nSrvID )
{
	switch( TO_CONSTANT( nSrvID ) )
	{
	case SERVER_DATA_INVEN_CONSTANT:
		if( GetInven()->IsExistItem( TO_ID( nSrvID ) ) == false )
			return NULL;
		return GetInven()->GetData( TO_ID( nSrvID ) );

	case SERVER_DATA_TEQUIP_CONSTANT:
		if( GetTEquip()->IsExistItem( TO_ID( nSrvID ) ) == false )
			return NULL;
		return GetTEquip()->GetData( TO_ID( nSrvID ) );

	case SERVER_DATA_CHIPSET_CONSTANT:
		if( GetDigivice()->GetChipset( TO_ID( nSrvID ) )->IsEnable() == false )
			return NULL;
		return GetDigivice()->GetChipset( TO_ID( nSrvID ) );

	case SERVER_DATA_DIGIVICE_CONSTANT:
		if( GetTEquip()->GetDigiviceItem()->IsEnable() == false )
			return NULL;
		return GetTEquip()->GetDigiviceItem();

	case SERVER_DATA_WAREHOUSE_CONSTANT:
		if( m_pWarehouseAdapt->GetData( TO_ID( nSrvID ) )->IsEnable() == false )
			return NULL;
		return m_pWarehouseAdapt->GetData( TO_ID( nSrvID ) );
#ifdef NEW_SHARESTASH
	case SERVER_DATA_SHARESTASH_CONSTANT:		
		if( m_pWarehouseAdapt->GetShareData( TO_ID( nSrvID ) )->IsEnable() == false )
			return NULL;
		return m_pWarehouseAdapt->GetShareData( TO_ID( nSrvID ) );
#endif
#ifdef CROSSWARS_SYSTEM
	case SERVER_DATA_INVENCROSS_CONSTANT:		
		if( GetInvenCross()->GetData( TO_ID( nSrvID ) )->IsEnable() == false )
			return NULL;
		return GetInvenCross()->GetData( TO_ID( nSrvID ) );
#endif
	}

	assert_csm1( false, _T( "SrvID2ItemInfo : %d 상수 정의 안됨" ), TO_CONSTANT( nSrvID ) );
	return NULL;
}

void cDataMng::SendItemUse( int nSrvID )
{
	if( IsItemLock( nSrvID ) == true )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}	

	switch( TO_CONSTANT( nSrvID ) )
	{
	case SERVER_DATA_INVEN_CONSTANT:
		_SendItemUse_Inven( TO_ID( nSrvID ) );
		break;
	case SERVER_DATA_TEQUIP_CONSTANT:
	case SERVER_DATA_MEMORY_CONSTANT:
	case SERVER_DATA_CHIPSET_CONSTANT:
		break;
#ifdef CROSSWARS_SYSTEM
	case SERVER_DATA_INVENCROSS_CONSTANT:
		_SendItemUse_InvenCross( TO_ID( nSrvID ) );
		break;
#endif
	}	
}

#ifdef CROSSWARS_SYSTEM
void cDataMng::_SendItemUse_InvenCross( int nInvenIndex )
{
	if( IsItemUse( TO_INVENCROSS_SID( nInvenIndex ) ) == false )
		return; 

	cItemInfo* pItem = GetInvenCross()->GetData( nInvenIndex );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();

	CsC_AvObject* pTarget = NULL;
	switch( pFTInfo->s_nUseCharacter )
	{
	case 0:		// 해당 사항 없음
	case 1:		// 둘다 사용
		pTarget = g_pCharMng->GetTamerUser();		
		break;
	case 2:		// 디지몬 사용 가능
		pTarget = g_pCharMng->GetDigimonUser( 0 );
		break;
	case 3:		// 테이머 사용 가능		
		pTarget = g_pCharMng->GetTamerUser();		
		break;		
	default:
		assert_cs( false );
	}

	//=============================================================================================================
	//	타입별 분리
	//=============================================================================================================
	switch( pFTInfo->s_nType_L )
	{

	case nItem::CrossWars:
		{
			// 아이템 롹킹
			ItemLock( TO_INVENCROSS_SID( nInvenIndex ) );

			if( net::game )
				net::game->SendItemUseCross( pTarget->GetUniqID(), TO_INVENCROSS_SID(nInvenIndex) );
			else
				ServerCrossItemUseSuccess( pTarget->GetUniqID(), nInvenIndex );


		}
		break;

	default:
		assert_csm1( false, _T( "item use Error : type = %d" ), pFTInfo->s_nType_L );
	}
}

#endif


bool cDataMng::SkillMemoryUse(CsItem::sINFO* pFTInfo, int InvenIndex)
{
	// 스킬을 배울 수 있는 최대 개수 확인
	cSkill* pDigimonSkill = g_pCharMng->GetDigimonUser(0)->GetSkillMng();
	int		nSkillCount = 0;
	int		nLearnCount = 0;
	bool	bSameSkill = false;
	for( int i=0; i<pDigimonSkill->GetSkillCount(); ++i )
	{
		CsSkill::sINFO*	pSkill = pDigimonSkill->GetSkill( i )->s_pFTSkill->GetInfo();

		// 동일한 스킬을 몇개 가지고 있나??
		if( pSkill->s_dwID == pFTInfo->s_dwSkill )
			nLearnCount++;

		USHORT	MemorySkill = 0;
		MemorySkill = pSkill->s_nMemorySkill;	// 스킬 메모리 타입

		USHORT	ItemType	= pFTInfo->s_nType_L;		// 아이템 타입.

		if( MemorySkill != nSkill::UNKNOWN_MEMORY_SKILL_TYPE)	// 메모리 스킬(캐시 스킬)인 경우
		{
			nSkillCount++;	// 메모리 스킬 갯수

			// 공격형 스킬 중복 검사.
			if( MemorySkill == nSkill::MEMORY_SKILL_ATK && ItemType == nItem::DigimonSkillChipATK)
			{
				bSameSkill = true;
				continue;				
			}
			// 방어형 스킬 중복 검사.
			if( MemorySkill == nSkill::MEMORY_SKILL_DEF && ItemType == nItem::DigimonSkillChipDEF)
			{
				bSameSkill = true;
				continue;				
			}
			// 보조형 스킬 중복 검사.
			if( MemorySkill == nSkill::MEMORY_SKILL_AST && ItemType == nItem::DigimonSkillChipAST)
			{
				bSameSkill = true;
				continue;				
			}
		}		
	}

	if( g_pCharMng->GetDigimonUser(0)->IsEvolDegener() )	// 진화 또는 퇴화중
	{
		cPrintMsg::PrintMsg( 13006 );	// 아직 사용할 수 없습니다.
		return false;
	}
	else if( nSkillCount >= nLimit::MAX_ItemSkillDigimon )	// 최대 스킬 갯수
	{
		cPrintMsg::PrintMsg( 30681 );	// 배울 수 있는 최대 개수를 넘었습니다.
		return false;
	}
	else if( nLearnCount )									// 동일한 스킬
	{
		cPrintMsg::PrintMsg( 30682 );	// 이미 습득한 스킬입니다.
		return false;
	}
	else if( bSameSkill )									// 타입이 같은 스킬
	{
		cPrintMsg::PrintMsg( 30683, pFTInfo->s_szName );	// 메모리 스킬 타입은 더이상 습득할수 없습니다.
		return false;
	}

	USHORT	DigimonType = 0;
	DigimonType = pFTInfo->s_nDoNotUseType;
	// 디지몬 사용 가능 타입 검사
	switch(DigimonType)
	{
	case 0:	// 제한 없음
		break;
	case 1:	// X항체 조그레스, 조그레스, 크로스워즈 타입은 사용 불가
		{
			SAFE_POINTER_RETVAL( g_pCharMng, false );
			CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
			SAFE_POINTER_RETVAL( pDigimonUser, false );

			if( pDigimonUser->IsCurJointProgress() )
			{
				SAFE_POINTER_RETVAL( nsCsFileTable::g_pBaseMng, false );
				std::wstring name = nsCsFileTable::g_pBaseMng->GetEvoTypeName(nsCsDigimonTable::ET_JOINTPROGRESSMODE);
				cPrintMsg::PrintMsg( 30684, const_cast<TCHAR*>(name.c_str()));
				return false;
			}
#ifdef CROSSWARS_SYSTEM
			if( nsCsFileTable::g_pDigimonMng->IsCrossDigimon( pDigimonUser->GetBaseDigimonFTID() ) )
			{
				cPrintMsg::PrintMsg( 30684, const_cast<TCHAR*>( UISTRING_TEXT( "COMMON_TXT_XROS_WARS" ).c_str() ) );
				return false;
			}
#endif
#ifdef SDM_TAMER_XGUAGE_20180628
			if( pDigimonUser->IsSameEvoultionType( nsCsDigimonTable::ET_X_JOGRESS ) )
			{
				SAFE_POINTER_RETVAL( nsCsFileTable::g_pBaseMng, false );
				std::wstring name = nsCsFileTable::g_pBaseMng->GetEvoTypeName(nsCsDigimonTable::ET_X_JOGRESS);
				cPrintMsg::PrintMsg( 30684, const_cast<TCHAR*>(name.c_str()));
				return false;
			}

			if( pDigimonUser->IsSameEvoultionType( nsCsDigimonTable::ET_MIX ) )
			{
				SAFE_POINTER_RETVAL( nsCsFileTable::g_pBaseMng, false );
				std::wstring name = nsCsFileTable::g_pBaseMng->GetEvoTypeName(nsCsDigimonTable::ET_MIX);
				cPrintMsg::PrintMsg( 30684, const_cast<TCHAR*>(name.c_str()));
				return false;
			}
#endif
		}
		break;
	case 2:	// X항체 조그레스, 조그레스만 사용 가능
		{
			SAFE_POINTER_RETVAL( g_pCharMng, false );
			CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
			SAFE_POINTER_RETVAL( pDigimonUser, false );

#ifdef CROSSWARS_SYSTEM
			if( nsCsFileTable::g_pDigimonMng->IsCrossDigimon( pDigimonUser->GetBaseDigimonFTID() ) )
			{
				cPrintMsg::PrintMsg( 30684, const_cast<TCHAR*>( UISTRING_TEXT( "COMMON_TXT_XROS_WARS" ).c_str() ) );
				return false;
			}
#endif

			if( !pDigimonUser->IsSameEvoultionType( nsCsDigimonTable::ET_JOINTPROGRESSMODE )
#ifdef SDM_TAMER_XGUAGE_20180628
				&& !pDigimonUser->IsSameEvoultionType( nsCsDigimonTable::ET_X_JOGRESS )
				&& !pDigimonUser->IsSameEvoultionType( nsCsDigimonTable::ET_MIX )
#endif
				)
			{
				SAFE_POINTER_RETVAL( nsCsFileTable::g_pBaseMng, false );
				std::wstring name = nsCsFileTable::g_pBaseMng->GetEvoTypeName( pDigimonUser->GetEvoType() );
				cPrintMsg::PrintMsg( 30684, const_cast<TCHAR*>(name.c_str()));
				return false;
			}
		}
		break;
	default:
		assert_cs( false );
		break;
	}


	// 디지몬이 현재 스킬을 등록할 레벨이 되었는지 검사.
	int nDigimonLv = g_pCharMng->GetDigimonUser(0)->GetBaseStat()->GetLevel();
	if( !pFTInfo->IsEnableDigimonLv( nDigimonLv ) )
	{
		cPrintMsg::PrintMsg( 30683, pFTInfo->s_szName );
		return false;
	}

	cPrintMsg::PrintMsg( 30680, pFTInfo->s_szName );				// 디지몬 스킬을 등록 하시겠습니까?
	cMessageBox::GetFirstMessageBox()->SetValue1( InvenIndex );	// 아이템 인벤 slot 넘김

	return true;
}

bool cDataMng::_CheckItemUseLevel( cItemInfo const* pItem )
{
	SAFE_POINTER_RETVAL( pItem, false );
	int nTLv = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();
	if( !pItem->IsEnableTamerLv( nTLv ) )
	{
		int nMinLv = 0; int nMMaxLv = 0;
		pItem->GetTamerReqLevel( nMinLv, nMMaxLv );
		if( nMinLv > nTLv )
			cPrintMsg::PrintMsg( 11042, &nMinLv );
		else
			cPrintMsg::PrintMsg( 20118, &nMMaxLv );

		return false;
	}

	int nDLv = g_pCharMng->GetDigimonUser( 0 )->GetBaseStat()->GetLevel();
	if( !pItem->IsEnableDigimonLv( nDLv ) )
	{
		int nMinLv = 0; int nMMaxLv = 0;
		pItem->GetDigimonReqLevel( nMinLv, nMMaxLv );
		if( nMinLv > nDLv )
			cPrintMsg::PrintMsg( 11013, &nMinLv );
		else
			cPrintMsg::PrintMsg( 20119, &nMMaxLv );

		return false;
	}

	return true;
}

void cDataMng::_SendItemUse_Inven( int nInvenIndex )
{
	if( IsItemUse( TO_INVEN_SID( nInvenIndex ) ) == false )
		return;

	cItemInfo* pItem = GetInven()->GetData( nInvenIndex );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();

	CsC_AvObject* pTarget = NULL;
	switch( pFTInfo->s_nUseCharacter )
	{
	case 0:		// 해당 사항 없음
	case 1:		// 둘다 사용
		pTarget = g_pCharMng->GetTamerUser();		
		break;
	case 2:		// 디지몬 사용 가능
		pTarget = g_pCharMng->GetDigimonUser( 0 );
		break;
	case 3:		// 테이머 사용 가능		
		pTarget = g_pCharMng->GetTamerUser();		
		break;		
	default:
		assert_cs( false );
	}

	//=============================================================================================================
	//	타입별 분리
	//=============================================================================================================
	switch( pFTInfo->s_nType_L )
	{
		// 장비
	case nItem::Head:
	case nItem::Coat:
	case nItem::Glove:
	case nItem::Pants:
	case nItem::Shoes:
	case nItem::Costume:
	case nItem::Glass:
#ifdef SDM_TAMER_XGUAGE_20180628
	case nItem::XAI:
#endif
		SendItemMove( TO_INVEN_SID( nInvenIndex ), TO_TEQUIP_SID( pFTInfo->s_nType_L - nItem::Head ) );
		break;
	case nItem::EquipAura: //오라 추가 chu8820
		SendItemMove( TO_INVEN_SID( nInvenIndex ), TO_TEQUIP_SID( (pFTInfo->s_nType_L - nItem::Head) ) ); //장비 타입 30번 없이 31번으로 넘어와서 1 더 빼줘야되
		break;
	case nItem::Portal:
		{
			bool bIsBattleRegisted = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_BATTLEMATCH_REGISTED, &bIsBattleRegisted );
			if( bIsBattleRegisted )// 배틀 신청중 인경우 맵이동 불가
			{
				cPrintMsg::PrintMsg( 40013 );	// 인벤토리에 여유 공간이 부족합니다.
				return;
			}

			// 아이템 롹킹
			if( g_pResist->IsEnablePortal() )
			{
				ItemLock( TO_INVEN_SID( nInvenIndex ) );

				if( net::game )
					net::game->SendItemUse( pTarget->GetUniqID(), nInvenIndex );
				else
					ServerItemUseSuccess( pTarget->GetUniqID(), nInvenIndex );
			}			
		}
		break;	
	case nItem::LevelRequire:
		{
			DWORD dwTotalType = pFTInfo->s_nType_L * 100 + pFTInfo->s_nType_S;
			switch( dwTotalType )
			{
#if COMMON_LIB_FIXED
			case nItem::SpiritJewelry:// 스피릿 진화체에만 사용되는 경험치 아이템
				{
					CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
					if( !pDigimonUser->IsSameEvoultionType( nsCsDigimonTable::ET_SPIRIT ) )
					{
						cPrintMsg::PrintMsg( 31002 );
						return;
					}
				}
				break;
#endif
			case nItem::ParcelJewelry:// 분양 디지몬에게만 사용 할 수 있는 경험치 물약
				{
					DWORD dwBaseDigimonFTID = g_pCharMng->GetDigimonUser( 0 )->GetBaseDigimonFTID();
					if( !nsCsFileTable::g_pDigimonParcelTBMng->IsParcelDigimon( dwBaseDigimonFTID ) )
					{
						cPrintMsg::PrintMsg( 31002 );
						return;
					}
				}
				break;
			default:
				break;
			}

			ItemLock( TO_INVEN_SID( nInvenIndex ) );

			if( net::game )
				net::game->SendItemUse( pTarget->GetUniqID(), nInvenIndex );
			else
				ServerItemUseSuccess( pTarget->GetUniqID(), nInvenIndex );
		}
		break;
		// break 안걸고 아래 루틴 타겠다

		// 포션, 카드, 소모품 - 아이템 사용
	case nItem::Potion:
	case nItem::Card:	
	case nItem::Consume:	
	case nItem::FriendShip_Increase:
#ifdef LEVELUP_EFFECT_LOOP
	case nItem::NoticeItem:
#endif
	case nItem::Fire_Work:
	case nItem::DogimonBattle:
	case nItem::AntiX:
		{
			// 아이템 롹킹
			ItemLock( TO_INVEN_SID( nInvenIndex ) );

			if( net::game )
				net::game->SendItemUse( pTarget->GetUniqID(), nInvenIndex );
			else
				ServerItemUseSuccess( pTarget->GetUniqID(), nInvenIndex );
		}
		break;
	case nItem::SkillLevelOpen:// 디지몬 스킬 레벨 확장 아이템 사용
		{
			SAFE_POINTER_RET( g_pCharMng );
			SAFE_POINTER_RET( nsCsFileTable::g_pBaseMng );

			CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
			SAFE_POINTER_RET( pDigimonUser );

			DWORD dwTotalType = pFTInfo->s_nType_L * 100 + pFTInfo->s_nType_S;
			int nEvoType = pDigimonUser->GetEvoType();
			if( !nsCsFileTable::g_pBaseMng->IsEnableSkillMaxLvUpFromEvoType( nEvoType, dwTotalType ) )
			{
				cPrintMsg::PrintMsg( nsSkillLimitOpenResult::ItemTypeError );// 현재 디지몬에 사용할 수없는 아이템
				return;
			}

			// 이전 단계의 스킬을 오픈 햇는지 여부 체크
			int nSkillMaxLv = pDigimonUser->GetCurrentEvoMaxSkillLv();
			int nCurrentMaxSkillRanke = nsCsFileTable::g_pBaseMng->GetSkillRankFromSkillMaxLv( nEvoType, nSkillMaxLv );
			int nNeedSkillRank = nsCsFileTable::g_pBaseMng->GetBeforOpenSkillRankeFromItemSubType( dwTotalType );
			if( nCurrentMaxSkillRanke < nNeedSkillRank )// 이전 스킬 랭크를 오픈하지 않았다.
			{
				cPrintMsg::PrintMsg( nsSkillLimitOpenResult::SkipBeforeLevel );// 현재 디지몬에 사용할 수없는 아이템
				return;
			}

			if( nCurrentMaxSkillRanke > nNeedSkillRank )// 이미 상위 스킬 랭크롤 오픈 하였다.
			{
				cPrintMsg::PrintMsg( nsSkillLimitOpenResult::AlreadyOpen );// 현재 디지몬에 사용할 수없는 아이템
				return;
			}

			int nMaxSkillLv = nsCsFileTable::g_pBaseMng->GetSkillMaxLvFromItemSubType( nEvoType, dwTotalType );
			cPrintMsg::PrintMsg( 11048, &nMaxSkillLv );// 현재 디지몬에 사용할 수없는 아이템
			cMessageBox* pMsg = cMessageBox::GetMessageBox( 11048 );
			if( pMsg )
			{
				pMsg->SetValue1( nInvenIndex );
				ItemUnlock( nInvenIndex );
			}
			return;
		}
		break;
// #ifdef FIREWORK
// 	case nItem::Fire_Work:
// 		{
// 			if( net::game )
// 				net::game->SendFireWork( pTarget->GetUniqID(), pFTInfo->s_dwItemID, nInvenIndex );
// 			else
// 				ServerItemUseSuccess( pTarget->GetUniqID(), nInvenIndex );
// 		}
// 		break;
// #endif
		
	case nItem::ScanUse_Item:
		{
			cItemInfo* pTempItem = g_pDataMng->GetInven()->GetData(nInvenIndex);

			int count = nsCsFileTable::g_pItemMng->GetDropCount(pTempItem->m_nType);

			if( nsCsFileTable::g_pItemMng->GetDropCount(pTempItem->m_nType) > g_pDataMng->GetInven()->GetEmptySlotCount() )
			{
				cPrintMsg::PrintMsg( 11015 );
				break;
			}

			// 아이템 롹킹
			ItemLock( TO_INVEN_SID( nInvenIndex ) );

			if( net::game )
				net::game->SendItemUse( pTarget->GetUniqID(), nInvenIndex );
			else
				ServerItemUseSuccess( pTarget->GetUniqID(), nInvenIndex );
		}
		break;
	case nItem::RClick_Hatch:
		{
			cPrintMsg::PrintMsg( 30534,  pFTInfo->s_szName );
			cMessageBox::GetFirstMessageBox()->SetValue1( nInvenIndex );	// 아이템 인벤slot data를 넘겨 메세지 박스에 아이템의 이름이 나오도록 함
			break;
		}
	case nItem::Buff:
	case nItem::SKILL_Card:
		{
			assert_cs( nsCsFileTable::g_pSkillMng->IsSkill( pFTItem->GetInfo()->s_dwSkill ) == true );

			for( int i = 0; i < SKILL_APPLY_MAX_COUNT; i++ )
			{
				USHORT nBuffID = nsCsFileTable::g_pSkillMng->GetSkill( pFTItem->GetInfo()->s_dwSkill )->GetInfo()->s_Apply[ i ].s_nBuffCode;

				if( nBuffID == 0 )
					break;

				if( SystemBuffCheck(nInvenIndex, pFTItem, pTarget, nBuffID) == true )
					return;

#ifdef LSJ_ALREADY_BUFF_CHECK	//	BUFF_CHECK_MSG 거의 같은 기능 확인이 필요함...
				if( AlreadyBuffCheck(nInvenIndex, pFTItem, pTarget, nBuffID) == true )
					return;
#endif

#ifdef BUFF_CHECK_MSG
				CsBuff::sINFO* pFTBuffInfo = nsCsFileTable::g_pBuffMng->GetBuff( nBuffID )->GetInfo();
				cBuffData* pBuff = g_pCharMng->GetTamerUser()->GetBuff();

				std::list< cBuffData::sBUFF_DATA >::iterator it = pBuff->GetBuff()->begin();
				std::list< cBuffData::sBUFF_DATA >::iterator itEnd = pBuff->GetBuff()->end();
				for( ; it != itEnd; ++it )
				{
					cItemInfo* pUseItem = g_pDataMng->GetInven()->GetData(nInvenIndex);
					
					// if( it->s_pBuffTable->GetInfo()->s_dwID == nBuffID )	// 원본 비교
					if( it->s_pBuffTable->GetInfo()->s_dwID == nBuffID ||	// 같은 버프 이거나
						it->s_pBuffTable->GetInfo()->s_nBuffClass == pFTBuffInfo->s_nBuffClass ) // 같은 Buff_Class 일 때
					{

						{	// 14.03.24 추가 코드 kimhoshok
							int CurSkillCode = it->s_pBuffTable->GetInfo()->s_dwSkillCode;// 사용중인 것						
							int UseReadySkillCode = pFTItem->GetInfo()->s_dwSkill;// 사용하려는 것
						
							CsSkill::sINFO* pCurInfo = nsCsFileTable::g_pSkillMng->GetSkill( CurSkillCode )->GetInfo();
							CsSkill::sINFO* pReadyInfo = nsCsFileTable::g_pSkillMng->GetSkill( UseReadySkillCode )->GetInfo();

							// 더 낮은 버프를 사용하려고 할 때(서버 Recv 처리에서 클라 처리가 필요해서 체크)
							if( pReadyInfo->s_Apply[i].s_nB < pCurInfo->s_Apply[i].s_nB )
							{
								cPrintMsg::PrintMsg( 30452 );
								return;
							}
						}

						cPrintMsg::PrintMsg( 20220 );
						cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
						pMBox->SetValue1( pTarget->GetUniqID() );
						pMBox->SetValue2( nInvenIndex );

						// 아이템 롹킹
						ItemLock( TO_INVEN_SID( nInvenIndex ) );
						return;
					}

				}
#endif
			}			
						
			// 아이템 롹킹
			ItemLock( TO_INVEN_SID( nInvenIndex ) );

			if( net::game )
				net::game->SendItemUse( pTarget->GetUniqID(), nInvenIndex );
			else
				ServerItemUseSuccess( pTarget->GetUniqID(), nInvenIndex );			
		}
		break;

	case nItem::Observer:
		{
// 			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_BATTLE_OBSERVER ) == false )
// 				g_pGameIF->GetDynamicIF( cBaseWindow::WT_BATTLE_OBSERVER, 0, nInvenIndex );
		}
		break;

		// 사용때 들어 오면 안된다

		// 사용시 무시
	case nItem::DigimonDataChange:
	case nItem::Quest:
	case nItem::Matrial2:
	case nItem::Matrial:
	case nItem::EventItem:
	case nItem::EventItem2:
	case nItem::Capsule_Coin:
	case nItem::Digitama1:
	case nItem::Digitama2:
	case nItem::Digitama3:
	case nItem::Digitama4:
	case nItem::Digitama5:
	case nItem::Digitama6:
	case nItem::Cash_DigiCore:
	case nItem::EItem_All:
	case nItem::EItem_Coat:
	case nItem::EItem_Glove:
	case nItem::EItem_Glass:
	case nItem::EItem_Pants:
	case nItem::EItem_Shoes:
	case nItem::BattleTag:
	case nItem::Excelator:
	case nItem::Cash_Resurection:	
	case nItem::Guild:		
	case nItem::Cash_ResetSkillPoint:
	case nItem::Cash_BackupDisk:
#ifdef TRICK_ORTREAT
	case nItem::TimeSet:
#endif
	case nItem::BackupDisk:
	case nItem::AttributeItem:
	case nItem::GotchaCoin:
	case nItem::Spirit:
		break;

	case nItem::PersonStore:// 개인 상점 아이템 사용
		{
			GAME_EVENT_ST.OnEvent(EVENT_CODE::REQ_PERSON_OPEN, &nInvenIndex);
		}
		break;

	case nItem::Digivice:
		SendItemMove( TO_INVEN_SID( nInvenIndex ), TO_DIGIVICE_SID( 0 ) );
		break;

		// 메모리
	case nItem::Memory:
		assert_cs( false );
		break;

		// 캡슐
	case nItem::Capsule:
		assert_cs( false );		
		break;

		// 칩셋
	case nItem::Chipset:
		{
			int nEmptyChipset = GetDigivice()->GetEmptyChipsetSlot();
			if( nEmptyChipset != -1 )
			{
				SendItemMove( TO_INVEN_SID( nInvenIndex ), TO_CHIPSET_SID( nEmptyChipset ) );
			}
			else
			{
				cPrintMsg::PrintMsg( cPrintMsg::CHIPSET_FULL );
			}
		}		
		break;	

	case nItem::Cash_Shouter_N:
	case nItem::Cash_Shouter_S:
	case nItem::Cash_Shouter_NT:
	case nItem::Cash_Shouter_ST:
	case nItem::GM_Shouter_T:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SHOUT_CASH ) == false )
				g_pGameIF->GetDynamicIF( cBaseWindow::WT_SHOUT_CASH, 0, nInvenIndex );
		}
		break;	

	case nItem::NoQuick_Consume:				
		{
			switch( pFTInfo->s_nType_S )	// 일반 소모품 선처리 할것있을때 사용
			{
			case 1: // 길드 인원증가 아이템사용
				{
					std::pair<int, bool> guild;
					guild.first = pFTInfo->s_nTypeValue;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::CHECK_INCREASE_GUILDMEMBERMAX, &guild );
					if (guild.second == true)
						return;
				}
				break;	
			case 2:	// 디지몬 스킬 초기화 아이템 사용
				{					
					cPrintMsg::PrintMsg( 30300, g_pCharMng->GetDigimonUser( 0 )->GetName() );
					cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
					pMBox->SetValue1( nInvenIndex );
					pMBox->SetValue2( pTarget->GetUniqID() );					
					return;
				}
				break;	
			case 20:		// 매크로 차단 아이템
				{
					GAME_EVENT_ST.OnEvent( EVENT_CODE::USING_MACROPROTECT );
					return;
				}
				break;
			}

			// 아이템 롹킹
			ItemLock( TO_INVEN_SID( nInvenIndex ) );

			if( net::game )
				net::game->SendItemUse( pTarget->GetUniqID(), nInvenIndex );
			else
				ServerItemUseSuccess( pTarget->GetUniqID(), nInvenIndex );
		}
		break;	
	case nItem::TacticsEnchant:
	case nItem::TacticsEnchantSub:	
	case nItem::TacticsEnchantReset:
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCHANT_TACTICS ) )
		{
			// 빈슬롯에 현재 아이템의 갯수 만큼 넣겠다
			g_pGameIF->GetEnchantTactics()->SetResist( nInvenIndex );
		}	
		break;	
		// 스케일 재설정 아이템
	case nItem::Rescale:
		{
			DWORD dwBaseDigimonFTID = g_pCharMng->GetDigimonUser( 0 )->GetBaseDigimonFTID();
			if( nsCsFileTable::g_pDigimonMng->IsStartDigimon( dwBaseDigimonFTID ) )
			{	// 파트너 디지몬은 크기 변경 불가
				cPrintMsg::PrintMsg( 30162 );
				return;
			}

			DWORD dwTotalType = pFTInfo->s_nType_L*100 + pFTInfo->s_nType_S;

			// 분양 디지몬의 경우 분양 전용 아이템만 사용 가능			
			bool bParcelDigimon = nsCsFileTable::g_pDigimonParcelTBMng->IsParcelDigimon( dwBaseDigimonFTID );
			if( dwTotalType == nItem::DigimonScaleParcelOut )// 분양 디지몬 전용인 아이템을 사용할 때
			{
				if( !bParcelDigimon )
				{
					cPrintMsg::PrintMsg( 31000 );
					return;
				}				
			}
			else
			{
				if( bParcelDigimon )
				{
					cPrintMsg::PrintMsg( 31001 );
					return;
				}	
			}

			cData_PostLoad* pPostLoad = g_pDataMng->GetPostLoad();
			SAFE_POINTER_RET( pPostLoad );
			cData_PostLoad::sDATA* pkDigimonData = pPostLoad->GetDigimonData();
			SAFE_POINTER_RET( pkDigimonData );

			switch( dwTotalType )
			{
#if COMMON_LIB_FIXED
			case nItem::DigimonScale6LevelExtra:			// 6단 전용 아이템과 조건 동일 - 크기 분포값만 다름
#endif
			case nItem::DigimonScale6LevelTotal:			// 6단 전용 아이템
				{											// 6단 전용 아이템인경우 디지몬이 6단 이상이여만 사용 가능
					if( pkDigimonData->s_HatchLevel < 6 )	// 6단 디지몬의 6단 전용 아이템으로만 크기 변경 가능
					{
						cPrintMsg::PrintMsg( 31000 );
						return;
					}
				}
				break;
#if COMMON_LIB_FIXED
			case nItem::DigimonScaleAllFixStep135:	// 단계 구분 없이 크기 135% 고정 아이템
				break;
#endif
			default:
				{
					// 일반 크기 변경 아이템인 경우 6단 이상인 디지몬은 사용 못함.
					if( pkDigimonData->s_HatchLevel >= 6 )
					{
						cPrintMsg::PrintMsg( 31000 );
						return;	
					}
				}
				break;
			}

			cPrintMsg::PrintMsg( 30160 );
			assert_cs( cMessageBox::IsMessageBox() );
			cMessageBox::GetFirstMessageBox()->SetValue1( nInvenIndex );
		}
		break;

		// 돈
	case nItem::Money:
		{
			if( net::game )
				net::game->SendItemUse( pTarget->GetUniqID(), nInvenIndex );
		}
		break;

		// 케쉬
	case nItem::Cash_ExtraInven:
		{
			if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::CASH_EXTRA_INVEN ) == false )
			{
				cPrintMsg::PrintMsg( 10004 );
				return;
			}
			if( g_pDataMng->GetInven()->GetInvenSlotCount() >= nLimit::Inven )
			{
				cPrintMsg::PrintMsg( 20024 );
				return;
			}

			cPrintMsg::PrintMsg( 20021, pFTInfo->s_szName );
			cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
			pMBox->SetValue1( nInvenIndex );
			pMBox->SetValue2( pTarget->GetUniqID() );
			pMBox->SetValue3( cData_ServerSync::CASH_EXTRA_INVEN );
		}
		break;
	case nItem::Cash_ExtraWarehouse:
		{
			if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::CASH_EXTRA_WAREHOUSE ) == false )
			{
				cPrintMsg::PrintMsg( 10004 );
				return;
			}
			if( m_pWarehouseAdapt->GetSlotCount() >= nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxWareHouse )
			{
				cPrintMsg::PrintMsg( 20024 );
				return;
			}
			cPrintMsg::PrintMsg( 20021, pFTInfo->s_szName );
			cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
			pMBox->SetValue1( nInvenIndex );
			pMBox->SetValue2( pTarget->GetUniqID() );
			pMBox->SetValue3( cData_ServerSync::CASH_EXTRA_WAREHOUSE );
		}
		break;

	case nItem::Cash_DigimonCareSlot:
		{
			if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::CASH_EXTRA_TACTICSHOUSE ) == false )
			{
				cPrintMsg::PrintMsg( 10004 );
				return;
			}
			int nTHouseCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxTacticsHouse;
			if(m_pDigimonArchiveAdapt->GetOpenedArchiveCnt() >= nTHouseCount)
			{
				cPrintMsg::PrintMsg( 20024 );
				return;
			}
			//if( g_pDataMng->GetTacticsHouse()->GetUseSlotCount() >= nTHouseCount )
			//{
			//	cPrintMsg::PrintMsg( 20024 );
			//	return;
			//}
			cPrintMsg::PrintMsg( 20021, pFTInfo->s_szName );
			cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
			pMBox->SetValue1( nInvenIndex );
			pMBox->SetValue2( pTarget->GetUniqID() );
			pMBox->SetValue3( cData_ServerSync::CASH_EXTRA_TACTICSHOUSE );
		}
		break;

	case nItem::Cash_Evoluter:
#if COMMON_LIB_FIXED
	case nItem::Cash_Evoluter2:
#endif
	case nItem::AntiXEvoluter:
		{
			if( GetServerSync()->IsEmptyRefCount( cData_ServerSync::DIGIMON_EVOLUTION_OPEN ) == false )
			{
				cPrintMsg::PrintMsg( 10004 );
			}
			else
			{
				if( g_pGameIF && !g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_DIGIMONSTATUS ) )
					GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_CLOSE_DIGIMONSTATUS, NULL );
			}	
		}		
		break;

	case nItem::Cash_ChangeTName:
		if( GetServerSync()->IsEmptyRefCount( cData_ServerSync::CASH_CHANGE_TNAME ) == false )
		{
			cPrintMsg::PrintMsg( 10004 );
		}
		else
		{
#ifdef UI_INVENTORY_RENEWAL
			int nPointer = 0;
#else
			int nPointer = nInvenIndex / IF_INVENTORY_COUNT;
#endif
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_INVENTORY, nPointer ) )
			{
				// 아이템 롹킹
				ItemLock( TO_INVEN_SID( nInvenIndex ) );
				g_pGameIF->GetInventory( nPointer )->NewInstTamerName( nInvenIndex );
			}
		}
		break;
	case nItem::ChangeTamer:
		{
			if(	nsCsGBTerrain::g_nSvrLibType != nLIB::SVR_GAME)
				return;

			if( GetServerSync()->IsEmptyRefCount( cData_ServerSync::CASH_CHANGE_TAMER ) == false ) 
			{
				cPrintMsg::PrintMsg( 10004 ); // 사용 할수 없습니다.
				return;
			}

			GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_CHANGETAMER_USINGITEM, &nInvenIndex );
		}
		break;
	case nItem::Cash_ChangeDName:
		{
#ifdef UI_INVENTORY_RENEWAL
			int nPointer = 0;
#else
			int nPointer = nInvenIndex / IF_INVENTORY_COUNT;
#endif
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_INVENTORY, nPointer ) )
			{
				// 아이템 롹킹
				ItemLock( TO_INVEN_SID( nInvenIndex ) );
				g_pGameIF->GetInventory( nPointer )->NewInstDigimonName( nInvenIndex );
			}
		}
		break;			
	case nItem::Cash_JumpBuster:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_JUMPBUSTER ) == true )
			{
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_JUMPBUSTER );
			}
			( (cJumpBuster*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_JUMPBUSTER, 0, pItem->GetType() ) )->SetJumpBusterItemInvenPos( nInvenIndex );
		}
		break;

		// 기간제 아이템
	case nItem::Portable_Item:
		{
			if( g_pGameIF->IsOpenNpcWindow( true ) )
				return;

#ifdef ITEM_USE_TIME_PASS
			if(pItem->m_nEndTime == 0xFFFFFFF0)
			{
				cPrintMsg::PrintMsg( 30176, pFTInfo->s_szName );
				cMessageBox::GetFirstMessageBox()->SetValue1( nInvenIndex );
				return;
			}
#endif

			if(g_pCharMng && g_pCharMng->GetTamerUser())
				g_pCharMng->GetTamerUser()->DeletePath();	// 이동중인 패스를 삭제.

			if(g_pServerMoveOwner)
				g_pServerMoveOwner->KeyReset();

			switch( pFTInfo->s_nType_S )
			{
			case eHATCH:	// 휴대용 부화기
				{
					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAKE_TACTICS ) )
						return;

					// 라이딩 중이거나 전투 중이면 사용 불가
					SAFE_POINTER_RET( g_pCharMng );
					CTamerUser* pTUser = g_pCharMng->GetTamerUser();
					SAFE_POINTER_RET( pTUser );
					if( pTUser->IsRide() )
					{
						cPrintMsg::PrintMsg( 30014 );
						return;
					}
					if( pTUser->IsBattle() )
					{
						cPrintMsg::PrintMsg( 16002 );
						return;
					}

#ifdef ITEM_USE_TIME_PASS
					cMakeTactics* pWin = dynamic_cast<cMakeTactics*>(g_pGameIF->GetDynamicIF( cBaseWindow::WT_MAKE_TACTICS, 0, nInvenIndex ));
#else
					cMakeTactics* pWin = dynamic_cast<cMakeTactics*>(g_pGameIF->GetDynamicIF( cBaseWindow::WT_MAKE_TACTICS ));
#endif
					SAFE_POINTER_RET( pWin );
					CsPoint csPos = pWin->GetRootSize();
					GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_POS_DEPENDON_INVEN, &csPos );
					pWin->SetPos( csPos );
					pWin->ResetDevice();
					pWin->SetTarget( NULL, true );
				}
				break;
			case eWARE_HOUSE:	// 휴대용 창고
				{
					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_WAREHOUSE ) )
						return;

					g_pGameIF->GetDynamicIF( cBaseWindow::WT_WAREHOUSE );
					g_pGameIF->GetWarehouse()->SetTargetPos( g_pCharMng->GetTamerUser()->GetPos2D() );
				}
				break;
			case eTATICS_HOUSE:	// 휴대용 디지몬(용병) 보관소
				{
					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TACTICSHOUSE ) )
						return;

					std::pair<int, NiPoint2> pairData;
					pairData.first = nInvenIndex;
					pairData.second = g_pCharMng->GetTamerUser()->GetPos2D(); 
					GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_SHOWVIEWER_BT_ITEM, &pairData);
				}
				break;
			
			case eSCANNER:	// 휴대용 스캐너
				{
					SAFE_POINTER_RET( g_pCharMng );
					CTamerUser* pTamer = g_pCharMng->GetTamerUser();
					SAFE_POINTER_RET( pTamer );
					UINT nWindowType = cBaseWindow::WT_DIGITAMA_SCAN;
					ContentsStream kTmp;
					kTmp << nWindowType << nInvenIndex << pTamer->GetPos2D();
					GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_OPEN_PORTABLE, &kTmp );
				}
				break;
			case ePAY_BACK:	// 휴대용 회수기
				{
					SAFE_POINTER_RET( g_pCharMng );
					CTamerUser* pTamer = g_pCharMng->GetTamerUser();
					SAFE_POINTER_RET( pTamer );
					UINT nWindowType = cBaseWindow::WT_DIGITAMA_SELL;
					ContentsStream kTmp;
					kTmp << nWindowType << nInvenIndex << pTamer->GetPos2D();
					GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_OPEN_PORTABLE, &kTmp );
				}
				break;
#ifdef LJW_EQUIPSYSTEM_190724
			case eANALYSOR:	// 휴대용 분석기
			case eFUSION:	// 휴대용 합성기
				{
#ifdef ITEM_USE_TIME_PASS
					int nPortable = nInvenIndex;
#else
					int nPortable = 0;
#endif
					GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_EQUIPSYSTEM_PORTABLE, &nPortable );
				}
				break;
#else // LJW_EQUIPSYSTEM_190724
			case eANALYSOR:	// 휴대용 분석기
				{
#ifdef ITEM_USE_TIME_PASS
					int nPortable = nInvenIndex;
#else
					int nPortable = 0;
#endif
					std::pair< int, int > pairSend( cBaseWindow::WT_ELEMENTITEM_CHECK, nPortable );
					GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_ELEMENT_ITEM_PORTABLE, &pairSend );
				}break;
			case eFUSION:		// 휴대용 합성기
				{
#ifdef ITEM_USE_TIME_PASS
					int nPortable = nInvenIndex;
#else
					int nPortable = 0;
#endif
					std::pair< int, int > pairSend( cBaseWindow::WT_ELEMENTITEM_ATTACH, nPortable );
					GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_ELEMENT_ITEM_PORTABLE, &pairSend );
				}break;
#endif // LJW_EQUIPSYSTEM_190724
			default:
				assert_cs( false );
			}
		}
		break;

		// 퀘스트 사용 아이템
	case nItem::UseQuest:
		{
			DWORD nQuestID = GetQuest()->UseItem( pFTItem );
			if( nQuestID != 0 )
			{
				// 아이템 롹킹
				ItemLock( TO_INVEN_SID( nInvenIndex ) );

				// 내부에서 처리 하므로 따로 사용 패킷을 써버로 보내지 아니하고 바로 처리한다.
				ServerItemUseSuccess( pTarget->GetUniqID(), nInvenIndex );


				CsC_AvObject* pTamer = g_pCharMng->GetTamerUser();
				CsC_AvObject* pSelectTarget = g_pCharResMng->GetTargetMark();

				// 별개 처리들
				switch( nQuestID )
				{				
				case 603:
					pTamer->GetProp_Effect()->AddEffect( "_Quest\\Fireworks.nif", 1, 0 );
					pTamer->PlaySound( "_Quest\\Fireworks.wav" );
					break;

				case 707:
					pTamer->SetAnimation( ANI::IDLE_ITEMCROP );
					pTamer->SetPauseAll( true );
					pTamer->PlaySound( "_Quest\\water_Get.wav" );
#ifndef VERSION_USA//임무 수행 완료 한글 출력 부분 GSP 제거 chu8820
					pTamer->GetProp_Effect()->AddEffect( "_Quest\\NormalEffect.nif", 1, 0 );
#endif
					break;

				case 661:
					pTamer->SetAnimation( ANI::IDLE_ITEMCROP );
					pTamer->SetPauseAll( true );
					pTamer->GetProp_Effect()->AddEffect( "_Quest\\Bomb.nif", 2, 0 );
					pTamer->PlaySound( "_Quest\\Bomb_Set.wav" );
					break;

				case 697:
					if( pSelectTarget )
					{
						pSelectTarget->GetProp_Effect()->AddEffect( "_Quest\\SmellBomb.nif", 8.0f, nsEFFECT::SPOS_HALF_THEIGHT );
						pSelectTarget->PlaySound( "_Quest\\Smell_Bomb.wav" );
					}
					break;

				case 291:
				case 212:		// 안테나
					{
						NiPoint3 vPos = pTamer->GetDir()*100.0f + pTamer->GetPos();
						pTamer->GetProp_Effect()->AddEffect( "_Quest\\Antenna.nif", 1.0f, 0 )->SetWorldPos( vPos );
						pTamer->SetAnimation( ANI::TAKE_OBJECT );
						pTamer->SetPauseTime( CsC_AvObject::PAUSE_PATH, 3.0f );
						pTamer->SetPauseTime( CsC_AvObject::PAUSE_ANIMATION, 3.0f, CsC_AvObject::PTE_ANIMATION, ANI::IDLE_NORMAL );
						pTamer->PlaySound( "_Quest\\Antenna.mp3" );
					}
					break;

				case 290:
				case 307:		// 물 아이템 사용
					{
						NiPoint3 vPos = pTamer->GetDir()*220.0f + pTamer->GetPos();
						pTamer->GetProp_Effect()->AddEffect( "_Quest\\WaterFall0002.nif", 7.0f, 0 )->SetWorldPos( vPos );
						pTamer->SetAnimation( ANI::TAKE_OBJECT );
						pTamer->SetPauseTime( CsC_AvObject::PAUSE_PATH, 1.0f );
						pTamer->SetPauseTime( CsC_AvObject::PAUSE_ANIMATION, 1.0f, CsC_AvObject::PTE_ANIMATION, ANI::IDLE_NORMAL );

						//pTamer->PlaySound( "_Quest\\Antenna.mp3" );
						pTamer->PlaySound( "_Quest\\Quest_Mission.wav" );
					}
					break;

				case 329:
					{
						CTamerUser* pTamer = g_pCharMng->GetTamerUser();
						NiPoint3 vPos = pTamer->GetPos();
						vPos.z -= 200;
						pTamer->GetProp_Effect()->AddEffect( "_Quest\\329.nif", 1.7f, 0 )->SetPos( vPos );
						pTamer->SetAnimation( ANI::TAKE_OBJECT );
						pTamer->SetPauseTime( CsC_AvObject::PAUSE_PATH, 2.0f );
						pTamer->SetPauseTime( CsC_AvObject::PAUSE_ANIMATION, 2.0f, CsC_AvObject::PTE_ANIMATION, ANI::IDLE_NORMAL );

						pTamer->PlaySound( "_Quest\\Quest_Mission.wav" );
					}
					break;

				default:
#ifndef VERSION_USA//임무 수행 완료 한글 출력 부분 GSP 제거 chu8820
					pTamer->GetProp_Effect()->AddEffect( "_Quest\\NormalEffect.nif", 1, 0 );
#endif
					pTamer->PlaySound( "_Quest\\Quest_Mission.wav" );
				}
			}
			break;
		}
	case nItem::GoldBar:	// Gold Bar	(상점에 처분 비용으로 계산 됨)
		{
			if( net::game )
				net::game->SendItemUse( pTarget->GetUniqID(), nInvenIndex );
			else
				ServerItemUseSuccess( pTarget->GetUniqID(), nInvenIndex );

			// 아이템 롹킹
			ItemLock( TO_INVEN_SID( nInvenIndex ) );
		}
		break;
	case nItem::Item_TamerSKillConsume:
		{
			// 캐쉬 스킬 개수가 2개 이상이라면 더이상 사용할 수 없다.
			if( g_pCharMng->GetTamerUser()->GetSkillMng()->CountCashSkill() >= 2 )
			{
				cPrintMsg::PrintMsg(30582);	// 기간제 테이머 스킬은 2개 까지 등록 가능합니다
				break;
			}

			cPrintMsg::PrintMsg( 30581, pFTInfo->s_szName );	// 테이머 스킬을 등록 하시겠습니까?
			cMessageBox::GetFirstMessageBox()->SetValue1( nInvenIndex );	// 아이템 인벤 slot 넘김
		}
		break;
#ifdef CROSSWARS_SYSTEM
	case nItem::Cash_CrossInvenSlot:
		{
			if(0 == g_pDataMng->GetPostLoad()->GetTamerData()->s_nCrossLoader)
			{
				cPrintMsg::PrintMsg( 30589 );
				return;
			}
			if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::CASH_EXTRA_CROSSINVEN ) == false )
			{
				cPrintMsg::PrintMsg( 10004 );
				return;
			}
			if( (SHORT)g_pDataMng->GetInvenCross()->GetInvenSlotCount() >= nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossInven )
			{
				cPrintMsg::PrintMsg( 20024 );
				return;
			}
			cPrintMsg::PrintMsg( 20021, pFTInfo->s_szName );
			cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
			pMBox->SetValue1( nInvenIndex );
			pMBox->SetValue2( pTarget->GetUniqID() );
			pMBox->SetValue3( cData_ServerSync::CASH_EXTRA_CROSSINVEN );
		}
		break;
	case nItem::CodeCrown:
		{
			// 로더가 활성화 될때만.

			
			if(0 == g_pDataMng->GetPostLoad()->GetTamerData()->s_nCrossLoader)
			{
				cPrintMsg::PrintMsg( 30589 );
				return;
			}
			if( net::game )
				net::game->SendItemUse( pTarget->GetUniqID(), nInvenIndex );
			else
				ServerItemUseSuccess( pTarget->GetUniqID(), nInvenIndex );


			// 아이템 롹킹
			ItemLock( TO_INVEN_SID( nInvenIndex ) );

		}
		break;

	case nItem::CrossLoader:
		{
			if( net::game )
				net::game->SendItemUse( pTarget->GetUniqID(), nInvenIndex );
			else
				ServerItemUseSuccess( pTarget->GetUniqID(), nInvenIndex );


			// 아이템 롹킹
			ItemLock( TO_INVEN_SID( nInvenIndex ) );
		}
		break;
	case nItem::CrossWars:
		{
			// 크로스 인밴에서 사용되야 한다.
		}
		break;
#endif
	
#ifdef DEBUFF_ITEM
	case nItem::Debuff:
		{
			if( g_pCharResMng->GetTargetMark() == NULL )
				return;

			if( g_pCharResMng->GetTargetMark()->GetClass() != nClass::Monster )	// 대상이 몬스터 인지 체크
			{
				cPrintMsg::PrintMsg( 11101 );	// 잘못된 대상입니다.
				break;
			}

			bool bEqual = true;

			switch( pFTInfo->s_nSection )
			{
			case 18600:	// 고유 몬스터 대상
				if( g_pCharResMng->GetTargetMark()->GetFTID() != pFTInfo->s_nTypeValue )
				{
					cPrintMsg::PrintMsg( 11101 );	// 잘못된 대상입니다.
					bEqual = false;
				}
				break;

			case 18601:	// 일반 몬스터 대상
				if( g_pCharMng->GetMonster( g_pCharResMng->GetTargetMark()->GetIDX() )->GetMonsterFT()->GetInfo()->s_nClass != 1 )
				{
					cPrintMsg::PrintMsg( 11101 );	// 잘못된 대상입니다.
					bEqual = false;
				}
				break;

			case 18602:	// 정예 몬스터 대상
				if( g_pCharMng->GetMonster( g_pCharResMng->GetTargetMark()->GetIDX() )->GetMonsterFT()->GetInfo()->s_nClass != 2 )
				{
					cPrintMsg::PrintMsg( 11101 );	// 잘못된 대상입니다.
					bEqual = false;
				}
				break;

			case 18603:	// 보스 몬스터 대상
				if( g_pCharMng->GetMonster( g_pCharResMng->GetTargetMark()->GetIDX() )->GetMonsterFT()->GetInfo()->s_nClass != 3 )
				{
					cPrintMsg::PrintMsg( 11101 );	// 잘못된 대상입니다.
					bEqual = false;
				}
				break;

			case 18604:	// 정예 보스 몬스터 대상
				if( g_pCharMng->GetMonster( g_pCharResMng->GetTargetMark()->GetIDX() )->GetMonsterFT()->GetInfo()->s_nClass != 4 )
				{
					cPrintMsg::PrintMsg( 11101 );	// 잘못된 대상입니다.
					bEqual = false;
				}
				break;

			case 18605:	// 특수(중보스) 몬스터 대상
				if( g_pCharMng->GetMonster( g_pCharResMng->GetTargetMark()->GetIDX() )->GetMonsterFT()->GetInfo()->s_nClass != 5 )
				{
					cPrintMsg::PrintMsg( 11101 );	// 잘못된 대상입니다.
					bEqual = false;
				}
				break;

			case 18606:	// 레이드 보스 대상
				if( g_pCharMng->GetMonster( g_pCharResMng->GetTargetMark()->GetIDX() )->GetMonsterFT()->GetInfo()->s_nClass != 6 )
				{
					cPrintMsg::PrintMsg( 11101 );	// 잘못된 대상입니다.
					bEqual = false;
				}
				break;
			}

			if( bEqual == true )
				g_pCharMng->GetTamerUser()->SetDebuffPos( g_pCharResMng->GetTargetMark(), nsCsFileTable::g_pSkillMng->GetSkill( pFTInfo->s_dwSkill )->GetInfo()->s_fAttRange, nInvenIndex );
		}
		break;
#endif
	case nItem::Earring:
	case nItem::Necklace:
	case nItem::Ring:
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
	case nItem::Bracelet:
#endif
		{
			// 강화 중이면
			if( m_pEnchantOptionAdapt && m_pEnchantOptionAdapt->IsProcessingEnchant() )
				break;

			//감정 했으면 장비 착용
			if( 0 != pItem->m_nRate )
				SendItemMove( TO_INVEN_SID( nInvenIndex ), TO_TEQUIP_SID( pFTInfo->s_nType_L - nItem::Head ) );
		}
		break;
	case nItem::AccStone:
		{
			// 강화 중이면
			if( m_pEnchantOptionAdapt && m_pEnchantOptionAdapt->IsProcessingEnchant() )
				break;
		}
		break;
	case nItem::Incubater:
		{
			//인큐베이터 아이템 RClick
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TACTICSHOUSE) )
			{
				GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_INCUBATOR_ITEM_IN_INVENTORY, &nInvenIndex);
			}
		}
		break;
	case nItem::CardMaster:// 씰 아이템을 사용
		{		
			
			if( GetServerSync()->IsEmptyRefCount( cData_ServerSync::CASH_IDENTIFY_CARD ) == false )
			{
				cPrintMsg::PrintMsg( 10004 );
				return;
			}

			// UnSeal UI를 열기 전, ReSeal UI가 열려 있으면 닫아줍니다.
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CARDRESEAL );
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_CARDIDENTIFY, 0, nInvenIndex );

		}
		break;
	case nItem::UnSealCardMaster:
 	case nItem::ReSealCardMaster:	// 씰 등록창이 열렸을 때에는 아이템을 등록해줍니다.
 		break;
	case nItem::DigimonSlotInc:
		{
			int nCurOpenSlot = g_pDataMng->GetTactics()->_GetOpenSlot();	//현재 오픈 상태, 몇개 열려있나
			if( nCurOpenSlot < nLimit::DigimonBaseSlot - 1 )	// 최대치가 아닌 상태에서 썼을 때만
			{
				//아이템 사용해서 오픈할지 물어봐
				cPrintMsg::PrintMsg( 20021, pFTInfo->s_szName );
				cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
				pMBox->SetValue1( nInvenIndex );
				pMBox->SetValue2( pTarget->GetUniqID() );
				pMBox->SetValue3( cData_ServerSync::TACTICS_OPEN );
			}
		}
		break;
	// 인벤 캡슐아이템 사용 시 동작
	case nItem::EvoCapsule:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CAPSULE_EVOLUTION ) )
			{
				cCapsuleEvolution* pCapsuleEvo = g_pGameIF->GetCapsuleEvol();

				if( pCapsuleEvo->IsResist( nInvenIndex ))
				{
					pCapsuleEvo->SetResist( nInvenIndex );
				}
			}
		}
		break;

#ifdef MEGAPHONE_BAN
	case nItem::Cash_Shouter_Ban:
		{
			if( g_pCharMng->GetTamerUser()->GetMegaPhoneBan() )
			{
				cPrintMsg::PrintMsg( 30675 );	// 이미 적용이 되어 있습니다.
				return;
			}

// 			if( g_pDataMng->GetBattle()->IsJoinMatch() )
// 			{
// 				cPrintMsg::PrintMsg( 30455 );	// 현재 사용할 수 없는 기능입니다.
// 				return;
// 			}

			if( g_pCharMng->GetTamerUser()->IsBattle() )
			{
				cPrintMsg::PrintMsg( 16002 );	// 전투중에는 사용할 수 없습니다.
				return;
			}

			// 아이템 롹킹
			ItemLock( TO_INVEN_SID( nInvenIndex ) );

			cPrintMsg::PrintMsg( 30676 );
			cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
			pMBox->SetValue1( pTarget->GetUniqID() );
			pMBox->SetValue2( nInvenIndex );
		}
		break;
#endif


	case nItem::DigimonSkillChipATK:	// 공격형 특성 메모리
	case nItem::DigimonSkillChipDEF:	// 방어형 특성 메모리
	case nItem::DigimonSkillChipAST:	// 보조형 특성 메모리
		SkillMemoryUse(pFTInfo, nInvenIndex);				//	#ifdef DIGIMON_CASH_SKILL
		break;
	case nItem::DigimonSkillMemoryChip:		// 메모리 칩	//	#ifdef DIGIMON_CASH_SKILL
		break;
	case nItem::RelocateTamer:		// 서버 이전 아이템 사용.
		GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_SERVER_RELOCATE_PAGE_UI, &nInvenIndex );
		break;
	case nItem::TamerSlotExpansion:	// 테이머 슬롯 확장 아이템
		{
			if( GLOBALDATA_ST.GetCurrentServerLockSlotCount() <= 0 )
			{
				// 더이상 테이머 슬롯을 확장 할 수 없다.
				cPrintMsg::PrintMsg( 10030 );
				return;
			}
			else
			{				
				ItemLock( TO_INVEN_SID( nInvenIndex ) );// 아이템 롹킹
				if( net::game )
					net::game->SendItemUse( pTarget->GetUniqID(), nInvenIndex );
			}
		}break;

#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
	case nItem::DigimonEffectChange:
		{
			CDigimonUser* pUser = g_pCharMng->GetDigimonUser(0);
			SAFE_POINTER_RET( pUser );

			int nTypeS = pFTInfo->s_nType_L * 100 + pFTInfo->s_nType_S;
			switch( nTypeS )
			{
			case nItem::EffectChange_Base:		// 디지몬 이펙트 해제
				{
					int ePartsIndex = nsDPART::None;
					nsCsFileTable::g_pItemMng->GetPartsIndex( pFTInfo->s_dwItemID, ePartsIndex );

					// 장착되어 있는게 없으면
					if( !pUser->IsPartsApplied( ePartsIndex ) )
					{
						cPrintMsg::PrintMsg( nsDIGIMON_EFFECT_CHANGE_RES::ALREADY_REMOVED );
						return;
					}
				}
				break;
			case nItem::EffectChange_Special:	// 디지몬 이펙트 장착
			case nItem::EffectChange_Special_Permanent:	// 영구 아이템
				{
					int ePartsIndex = nsDPART::None;
					nsCsFileTable::g_pItemMng->GetPartsIndex( pFTInfo->s_dwItemID, ePartsIndex );

					// 파트너 디지몬이 사용 가능 대상인지
					if( !nsCsFileTable::g_pItemMng->IsEnableDigimonPartsItem( pFTInfo->s_dwItemID, pUser->GetFTID(), ePartsIndex ) )
					{
						cPrintMsg::PrintMsg( nsDIGIMON_EFFECT_CHANGE_RES::INVALID_DIGIMON );
						return;
					}

					// 이미 장착 중이면
					if( pUser->IsPartsApplied( ePartsIndex ) )
					{
						cPrintMsg::PrintMsg( nsDIGIMON_EFFECT_CHANGE_RES::CHANGE_SAME_EFFECT );
						return;
					}
				}
				break;
			default:
				return;
			}

			if( net::game )
				net::game->SendChangeDigimonEffect( nInvenIndex, pFTInfo->s_dwItemID, pItem->GetCount() );
		}
		break;
#endif

	default:
		assert_csm1( false, _T( "item use Error : type = %d" ), pFTInfo->s_nType_L );
	}
}
#ifdef CROSSWARS_SYSTEM
void cDataMng::ServerCrossItemUseSuccess( uint nTargetUID, int nInvenIndex )
{
	cItemInfo* pItem = GetInvenCross()->GetData(  nInvenIndex );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();

	//=============================================================================================================
	//	타겟
	//=============================================================================================================
	CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );	

	//=============================================================================================================
	//	쿨타임
	//=============================================================================================================
	if( pFTInfo->s_nUseTimeGroup != 0 )
	{
		// 쿨타임 적용
		pFTInfo->GetCoolTimeSeq()->Start();		
	}

	//=============================================================================================================
	//	사운드
	//=============================================================================================================
	if( pTarget )
	{
		switch( pFTInfo->s_dwSoundID )
		{
		case 0:		break;
		case 1:		pTarget->PlaySound( SOUND_ITEM_DRINK );		break;
		case 2:		pTarget->PlaySound( SOUND_ITEM_SWALLOW );	break;
		case 3:		pTarget->PlaySound( SOUND_ITEM_FOOD );		break;
		case 4:		pTarget->PlaySound( SOUND_ITEM_DISK );		break;
		case 5:		pTarget->PlaySound( SOUND_ITEM_PORTAL );	break;
		default:	assert_cs( false );
		}
	}	

	//=============================================================================================================
	//	이펙트
	//=============================================================================================================
	if( pTarget )
	{
		if( pFTInfo->s_dwSkill != 0 && nsCsItemTable::SKILLCODE == pFTInfo->s_nSkillCodeType )
		{
			CsSkill* pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( pFTInfo->s_dwSkill );

			switch( pFTSkill->GetInfo()->s_Apply[ 0 ].s_nA )
			{
			case 1:	// HP
				pTarget->GetProp_Effect()->AddEffect( EFFECT_FOOD, 1.0f, nsEFFECT::POS_CHARPOS );
				break;
			case 2:	// DS
				pTarget->GetProp_Effect()->AddEffect( EFFECT_DG, 1.0f, nsEFFECT::POS_CHARPOS );
				break;
			case 15: // EXP
				{
					pTarget->PlaySound( SOUND_LEVEL_UP );
					DWORD dwPlag = nsEFFECT::POS_CHARPOS;
					pTarget->GetProp_Effect()->AddEffect( EFFECT_DIGIMON_LEVELUP, pTarget->GetToolWidth()*0.02f, dwPlag );					
				}				
				break;
			}
		}
	}	

	//=============================================================================================================
	//	후처리 - 클라이언트 처리
	//=============================================================================================================
	switch( pFTInfo->s_nType_L )
	{

	case nItem::CrossWars:
		{
			pTarget->PlaySound( SOUND_USECROSSITEM );
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();
		}
		break;


	default:
		assert_cs( false );
	}

	ItemUnlock( TO_INVENCROSS_SID( nInvenIndex ) );	
}
#endif

void cDataMng::ServerItemUseSuccess( uint nTargetUID, int nInvenIndex )
{
	cItemInfo* pItem = GetInven()->GetData( nInvenIndex );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();

	//=============================================================================================================
	//	타겟
	//=============================================================================================================
	CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );	

	//=============================================================================================================
	//	쿨타임
	//=============================================================================================================
	if( pFTInfo->s_nUseTimeGroup != 0 )
	{
		// 쿨타임 적용
		pFTInfo->GetCoolTimeSeq()->Start();		
	}

	//=============================================================================================================
	//	사운드
	//=============================================================================================================
	if( pTarget )
	{
		switch( pFTInfo->s_dwSoundID )
		{
		case 0:		break;
		case 1:		pTarget->PlaySound( SOUND_ITEM_DRINK );		break;
		case 2:		pTarget->PlaySound( SOUND_ITEM_SWALLOW );	break;
		case 3:		pTarget->PlaySound( SOUND_ITEM_FOOD );		break;
		case 4:		pTarget->PlaySound( SOUND_ITEM_DISK );		break;
		case 5:		pTarget->PlaySound( SOUND_ITEM_PORTAL );	break;
		default:	assert_cs( false );
		}
	}	

	//=============================================================================================================
	//	이펙트
	//=============================================================================================================
	if( pTarget )
	{
		if( pFTInfo->s_dwSkill != 0 && nsCsItemTable::SKILLCODE == pFTInfo->s_nSkillCodeType )
		{
			CsSkill* pFTSkill = NULL;
			if( pFTInfo->s_nType_L == 180 )
			{
				CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( pFTInfo->s_dwSkill, -1 );
				pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( pTamerSkill->Get_TSkill_Info()->s_dwSkillCode );
			}
			else
				pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( pFTInfo->s_dwSkill );

			switch( pFTSkill->GetInfo()->s_Apply[ 0 ].s_nA )
			{
			case 1:	// HP
				pTarget->GetProp_Effect()->AddEffect( EFFECT_FOOD, 1.0f, nsEFFECT::POS_CHARPOS );
				break;
			case 2:	// DS
				pTarget->GetProp_Effect()->AddEffect( EFFECT_DG, 1.0f, nsEFFECT::POS_CHARPOS );
				break;
			case 15: // EXP
				{
					pTarget->PlaySound( SOUND_LEVEL_UP );
					DWORD dwPlag = nsEFFECT::POS_CHARPOS;
					pTarget->GetProp_Effect()->AddEffect( EFFECT_DIGIMON_LEVELUP, pTarget->GetToolWidth()*0.02f, dwPlag );					
				}				
				break;
			}
		}
	}	

	//=============================================================================================================
	//	후처리 - 클라이언트 처리
	//=============================================================================================================
	switch( pFTInfo->s_nType_L )
	{
	case nItem::Head:
	case nItem::Coat:
	case nItem::Glove:
	case nItem::Pants:
	case nItem::Shoes:
	case nItem::Costume:
	case nItem::Glass:
	case nItem::Earring:
	case nItem::Necklace:
	case nItem::Ring:
	case nItem::EquipAura: //오라 추가 chu8820
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
	case nItem::Bracelet:
#endif
	case nItem::Memory:
	case nItem::Capsule:
	case nItem::Chipset:
	case nItem::Quest:
	case nItem::Matrial2:
	case nItem::Digitama1:
	case nItem::Digitama2:
	case nItem::Digitama3:
	case nItem::Digitama4:
	case nItem::Digitama5:
	case nItem::Digitama6:
	case nItem::EItem_All:
	case nItem::EItem_Coat:
	case nItem::EItem_Glove:
	case nItem::EItem_Glass:
	case nItem::EItem_Pants:
	case nItem::EItem_Shoes:
	case nItem::TacticsEnchant:
	case nItem::TacticsEnchantSub:
		assert_cs( false );
		break;

		// 포션, 카드, 소모품
	case nItem::LevelRequire:
	case nItem::Potion:
	case nItem::Card:
	case nItem::Portal:
	case nItem::Consume:	
	case nItem::UseQuest:
	case nItem::Rescale:
	case nItem::FriendShip_Increase:
#ifdef CROSSWARS_SYSTEM
	case nItem::CodeCrown:	
#endif
#ifdef LEVELUP_EFFECT_LOOP
	case nItem::NoticeItem:
#endif
	case nItem::Fire_Work:
	case nItem::RelocateTamer:		// 테이머 서버 이전 아이템
	case nItem::AntiX:
		{
			// 아이템 갯수 감소
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();			
		}
		break;
	case nItem::TamerSlotExpansion:	// 테이머 슬롯 확장 아이템
		{
			cPrintMsg::PrintMsg( 10032 );
			// 아이템 갯수 감소
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();			
		}
		break;
#ifdef CROSSWARS_SYSTEM
	case nItem::CrossLoader:
		{
			// 아이템 갯수 감소
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();

			GS2C_RECV_CHECKTYPE recv;
			recv.nType = AchieveContents::CA_ItemUse;
			recv.nValue1 = pFTInfo->s_dwItemID;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
		}
		break;
#endif
	case nItem::ScanUse_Item:
		{
			// 아이템 갯수 감소
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();
		}
		break;
#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
	case nItem::DigimonEffectChange:
		{
			int nTypeS = pFTInfo->s_nType_L * 100 + pFTInfo->s_nType_S;
			switch (nTypeS)
			{
			case nItem::EffectChange_Base:
			case nItem::EffectChange_Special:
				pItem->DecreaseCount(1);	// 아이템 개수 감소
				break;
			case nItem::EffectChange_Special_Permanent:	// 영구 아이템
				break;
			}

			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();
		}break;
#endif
	case nItem::Buff:
	case nItem::SKILL_Card:
		{
			// 아이템 갯수 감소
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();
			
			if( pTarget->GetLeafRTTI() == RTTI_TAMER ||
				pTarget->GetLeafRTTI() == RTTI_TAMER_USER )
			{
				g_pCharMng->GetDigimon( ( (CTamer*)pTarget )->GetDigimonLink() )->GetProp_Animation()->SetAnimation( ANI::ACT_BUFF );
			}
		}
		break;

	case nItem::NoQuick_Consume:
		{
			switch( pFTInfo->s_nType_S )	// 일반 소모품 후처리 할것있을때 사용
			{
			case 2: // 디지몬스킬 초기화 아이템 사용
				{
					float fScale = g_pCharMng->GetDigimonUser( 0 )->GetScale() * 0.3f; 
					DWORD dwMask = nsEFFECT::POS_CHARPOS | nsEFFECT::DIR_CAMERA_AXIS_Z| nsEFFECT::SPOS_BOUND_CENTER; 
					g_pCharMng->GetDigimonUser( 0 )->GetProp_Effect()->AddEffect( "System\\SkillResetEffect.nif", fScale, dwMask );
					cPrintMsg::PrintMsg( 30301, g_pCharMng->GetDigimonUser( 0 )->GetName() );							
				}
				break;
			case 30:	// 아이템 업적 적용
				{						
					GS2C_RECV_CHECKTYPE recv;
					recv.nType = AchieveContents::CA_ItemUse;
					recv.nValue1 = pFTInfo->s_dwItemID;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
				}
				break;
			}

			// 아이템 갯수 감소
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();
		}
		break;

		// 돈
	case nItem::Money:
		{
			g_pDataMng->GetInven()->IncreaseMoney( pItem->GetCount() * 1000, true );

			pItem->Reset();
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();
		}
		break;
		// 케쉬
	case nItem::Cash_ExtraInven:
		{
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CASH_EXTRA_INVEN, 0 );
			// 아이템 갯수 감소
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
		}
		break;
	case nItem::Cash_ExtraWarehouse:
		{
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CASH_EXTRA_WAREHOUSE, 0 );
			// 아이템 갯수 감소
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
		}
		break;
#ifdef CROSSWARS_SYSTEM
	case nItem::Cash_CrossInvenSlot:
		{
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CASH_EXTRA_CROSSINVEN, 0 );
			// 아이템 갯수 감소
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
			pTarget->PlaySound(SOUND_USECROSSINVENEXTRAITEM);			
		}
		break;
#endif
	case nItem::Cash_DigimonCareSlot:
		{	
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CASH_EXTRA_TACTICSHOUSE, 0 );
			// 아이템 갯수 감소
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
		}
		break;

		// 이건 cData_ServerSync 쪽에서 다 처리하겠다 - 들의 모임
	case nItem::Cash_ChangeTName:
		assert_cs( false );
		break;
	case nItem::DogimonBattle:
		{
			DWORD dwAddPoint = pItem->GetCount();
			GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_ADD_BATTLE_POINT, &dwAddPoint );
			pItem->Reset();
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();
		}
		break;
	case nItem::GoldBar:
		{
			g_pDataMng->GetInven()->IncreaseMoney( pItem->GetCount() * pFTInfo->s_dwSale, true );
			
			pItem->Reset();
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();
		}
		break;
	case nItem::Item_TamerSKillConsume:
		{
			// 아이템 갯수 감소
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();
		}
		break;
#ifdef DEBUFF_ITEM
	case nItem::Debuff:
		{
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();
		}
		break;
#endif
	case nItem::DigimonSlotInc:	// 용병 슬롯 오픈 아이템
		{
			//한번에 여러개 오픈하는 아이템 만들 경우 수정 해야됨;; 기획 안정해줘서 그냥 1씩 추가
			SAFE_POINTER_BEK( g_pDataMng );
			cData_Tactics* pTactics = g_pDataMng->GetTactics();
			SAFE_POINTER_BEK( pTactics );
			int nCurOpenSlot = pTactics->_GetOpenSlot();
			if( nCurOpenSlot < nLimit::DigimonBaseSlot - 1 )	//	파트너몬 제외한 오픈 슬롯 갯수가 최대치 미만일 때만
			{
				pItem->DecreaseCount( 1 );
				cData_QuickSlot::CheckItemCount_AllUser();
				pTactics->_SetOpenSlot( nCurOpenSlot + 1 );
// 				g_pDataMng->GetTactics()->SetTacticsCount( nCurOpenSlot + 1 );

				//3번디지몬 교체스킬 끼워넣기
				cSkill::sINFO* pSkillInfo = cSkill::sINFO::NewInstance();
				pSkillInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( 1000005/*1000008*/ );
				pSkillInfo->s_eAnimation = ANI::INVALIDE_ID;
				pSkillInfo->s_CoolTime.SetEndTime( 1.0f );
				pSkillInfo->s_nTacticsIndex = nCurOpenSlot;
				pSkillInfo->fUseSkill = &cSkill::__BattleTag_Digimon;

				SAFE_POINTER_BEK( g_pCharMng );
				CTamerUser* pTUser = g_pCharMng->GetTamerUser();
				SAFE_POINTER_BEK( pTUser );
				cSkill* pSkillMng = pTUser->GetSkillMng();
				SAFE_POINTER_BEK( pSkillMng );
				pSkillMng->m_vpInfo.PushInsert( nCurOpenSlot + 3, pSkillInfo );

				// 테이머 스킬창 갱신
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );

				GS2C_RECV_EXTEND_ACCOMPANY_SLOT kRecvdata;
				kRecvdata.OpenedCnt = nCurOpenSlot + 1;
				GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_ON_EXTENT_ACCOMPANY_SLOT, &kRecvdata);
			}
		}
		break;
#ifdef MEGAPHONE_BAN
	case nItem::Cash_Shouter_Ban:
		{
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();

			g_pCharMng->GetTamerUser()->SetMegaPhoneBan( true );

			cPrintMsg::PrintMsg( 30677 );	// 확성기 차단 기능이 적용되었습니다.
		}
		break;
#endif

	case nItem::DigimonSkillChipATK:
	case nItem::DigimonSkillChipDEF:
	case nItem::DigimonSkillChipAST:
		{

// 			char cEffectPath[ MAX_PATH ] = {0, };
// 			sprintf_s( cEffectPath, MAX_PATH, "system\\Buff\\%s.nif", pFTInfo->s_cNif );
// 			// 메모리 스킬 등록 이펙트 add
// 			g_pCharMng->GetDigimonUser(0)->GetProp_Effect()->AddEffect( cEffectPath, sqrt( g_pCharMng->GetDigimonUser(0)->GetToolWidth()*0.02f ), nsEFFECT::POS_CHARPOS );

			// 아이템 갯수 감소
			pItem->DecreaseCount( 1 );
			cData_QuickSlot::CheckItemCount_AllUser();
			g_pDataMng->GetQuest()->CalProcess();
		}
		break;

	default:
		assert_cs( false );
	}

	ItemUnlock( TO_INVEN_SID( nInvenIndex ) );	
}

void cDataMng::ServerItemUseFailed( int nInvenIndex )
{
	ItemUnlock( TO_INVEN_SID( nInvenIndex ) );
}
void cDataMng::SetChangeItemLimited(u2 Itempos, u1 limit)
{
	n4 nTab = TO_CONSTANT( Itempos );
	n4 nSlot = TO_ID( Itempos );

	cItemInfo* pItemInfo = NULL;

	switch( nTab )
	{
		//case SERVER_DATA_INVEN_CONSTANT:
		//	{
		//		pItemInfo = GetInven()->GetData( nSlot );			
		//	}
		//	break;
		//case SERVER_DATA_WAREHOUSE_CONSTANT:
		//	{
		//		//pDest = m_pWarehouseAdapt->GetData( nSlot ) ;
		//	}
		//	break;
		//case SERVER_DATA_SHARESTASH_CONSTANT:
		//	{
		//		//pDest = m_pWarehouseAdapt->GetShareData( nSlot ) ;
		//	}
		//	break;
	case SERVER_DATA_CHIPSET_CONSTANT:
		pItemInfo = m_Digivice.GetChipset( nSlot );
		break;
	case SERVER_DATA_DIGIVICE_CONSTANT:
		pItemInfo = GetTEquip()->GetDigiviceItem();
		break;
	default:
		assert_cs( false );
	}

	SAFE_POINTER_RET(pItemInfo);

	pItemInfo->m_nLimited = limit;
}


void cDataMng::SendItemMoveInven( int nSrcSrvID, int nCount /* = 0  */ )
{
	// 첫번재로 비여 있는 슬롯
	int nEmptySlot = GetInven()->FindEmptySlot( -1 );
	if( nEmptySlot == cData_Inven::INVALIDE_INVEN_INDEX )
	{
		cPrintMsg::PrintMsg( 11015 );
		return;
	}
	SendItemMove( nSrcSrvID, TO_INVEN_SID( nEmptySlot ) );
}

//==============================================================================================
//		공유창고 소트는 컨텐츠로 넘어갔다.
//==============================================================================================
//==============================================================================================
//		창고 소트는 컨텐츠로 넘어갔다.
//==============================================================================================
void cDataMng::InvenSort()
{	
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_INVENTORY, 0 ) == false ) // 인벤창이 show 상태인가?
		return;

	if( IsSort() == true ) // 소트 리스트에 데이터가 있는가?(소트중이냐?)
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}
#ifndef NEW_SHARESTASH
#ifndef VERSION_HK
	// 인밴창 그룹들  비활성화 시키기. 3개
	bool bIsEnable = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable );
#endif
#endif
	// 커서에 물린 아이콘 지우기
	CURSOR_ST.ReleaseIcon();
	// 총 인밴 슬롯 수만큼 63
	for( int i = 0; i < (int)GetInven()->GetInvenSlotCount(); i++ )
	{
		cItemInfo* pCItemInfo = GetInven()->GetData( i );
		SAFE_POINTER_CON( pCItemInfo );

		int nItemIdx = pCItemInfo->GetType();	// 아이템이 슬롯에 있으면 GO

		if( nItemIdx == 0 ) 
			continue;

		int nCnt = 0;

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
		if( cData_Inven::IsTradeLimitedItem( pCItemInfo ) )
		{
			nCnt = 1;
		}
		else
#endif
		{
			bool bFind = false;
			std::list< DWORD >::iterator it = m_ListItem.begin();	// 아이템 리스트를 돌려본다.
			std::list< DWORD >::iterator itEnd = m_ListItem.end();
			for( ; it != itEnd; it++ )
			{
				if( *it == nItemIdx )
				{
					bFind = true;
					break;
				}
			}

			CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( nItemIdx )->GetInfo();
#ifdef ITEM_USE_TIME_PASS
			if( (pFTInfo->s_bUseTimeType == 1 && pCItemInfo->m_nEndTime != 0xFFFFFFF0) ||
				(pFTInfo->s_bUseTimeType == 1 && pCItemInfo->m_nEndTime == 0xFFFFFFF0 )	)	
				bFind = false;
#endif
			if( bFind == true )
				continue;

			int nTotalItemCount = GetInven()->GetCount_Item_ID( nItemIdx, true );// 거래 제한이 있는 아이템의 갯수는 제외
			int nMaxCount = pFTInfo->s_nOverlap;
			nCnt = nTotalItemCount / nMaxCount; // 그아이템의 가지고 있는수량.
			if( nTotalItemCount % nMaxCount > 0 )
				nCnt++;
	
#ifdef ITEM_USE_TIME_PASS
			if( (pFTInfo->s_bUseTimeType == 1 && pCItemInfo->m_nEndTime != 0xFFFFFFF0) ||
				(pFTInfo->s_bUseTimeType == 1 && pCItemInfo->m_nEndTime == 0xFFFFFFF0 )	)	
				nCnt = 1;
#endif
		}

		for( int n = 0; n < nCnt; n++ )
			m_ListItem.push_back( nItemIdx );

	}
	m_ListItem.sort();	// 리스트 소트 인덱스 순서로 정렬
	m_nSort = 0;

#ifdef NEW_SHARESTASH
	if( m_ListItem.empty() != true )
	{
		// 인밴창 그룹들  비활성화 시키기. 3개
		bool bIsEnable = false;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable );
	}
#endif
	//_SendSort();
	m_NewSort.Start();
}

void cDataMng::_SendSort()
{
	std::list< DWORD >::iterator it = m_ListItem.begin();	
	// 인벤에 있는 아이템리스트들 돌려서.
	if( m_ListItem.empty() != true )
	{	
		int nSrc = _GetMoveInvenSlot( *it ); // 겹치기
		if( nSrc != cData_Inven::INVALIDE_INVEN_INDEX )
		{
			int nCode = SendItemMove( nSrc, m_nSort ); // 이동할 슬롯 인덱스 , 검사할 슬롯 인덱스
			if( nCode != 0 )
			{
				g_CriticalLog.Log( _T( "인벤정렬실패 : Src = %d, Dest = %d, Max = %d, ErrCode = %d" ), nSrc, m_nSort, GetInven()->GetInvenSlotCount(), nCode );
			}		
		}
		else
		{
			m_ListItem.erase( it );			
			_NextSort();
		}		
	}
}

int cDataMng::_GetMoveInvenSlot( DWORD dwItemID )
{
	// 아이템인덱스를 검사해서 슬롯인덱스와 일치하는 인덱스값.
	int nSrc = g_pDataMng->GetInven()->GetSlot_Item_ID_ReverseToSlot( m_nSort, dwItemID );	// 검사할 슬롯 번호 , 아이템 인덱스 검사해서 몇번 슬롯인덱스인가?		

	// 현재 위치에 겹치기 가능한가?
	cItemInfo* pData = g_pDataMng->GetInven()->GetData( m_nSort );// 검사할 슬롯 번호의 아이템 정보를 가져온다.

	if( pData->GetType() == dwItemID ) // 아이템 인덱스가 일치하면
	{
		int nMaxCount = nsCsFileTable::g_pItemMng->GetItem( pData->GetType() )->GetInfo()->s_nOverlap; // 현제 아이템의 슬롯에 주어지는 최대수량
		if( pData->GetCount() == nMaxCount )
		{
			return cData_Inven::INVALIDE_INVEN_INDEX;
		}	

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214		    
		if(cData_Inven::IsTradeLimitedItem( pData ))	// 한슬롯에 아이템이 만땅있으면..INVALIDE_INVEN_INDEX
		{
			return cData_Inven::INVALIDE_INVEN_INDEX;
		}
#endif

#ifdef ITEM_USE_TIME_PASS
		CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( pData->GetType() )->GetInfo();

		if(pFTInfo->s_bUseTimeType == 1 && pData->m_nEndTime != 0xFFFFFFF0)	//시간제 아이템이 시간이 흘러간경우
		{
			return cData_Inven::INVALIDE_INVEN_INDEX;
		}
#endif	

	}
	return nSrc;
}

void cDataMng::_NextSort()
{	
	if( m_ListItem.empty() == true )
	{
		_ReleaseSort();		
	}
	else
	{   		
		m_nSort++;
		_SendSort();
	}
}
#ifdef CROSSWARS_SYSTEM
void cDataMng::_ReleaseCrossSort()
{
	
	m_nSort = 0;	
}
#endif

void cDataMng::_ReleaseSort(bool	Check)
{
	m_ListItem.clear();
	m_nSort = 0;
	// 인밴창 그룹들 활성화 시키기
	bool bIsEnable = true;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable );	
	if( net::game && Check)
		net::game->SendQueryMoneyInven();
}

	
int cDataMng::SendItemMove( int nSrcSrvID, int nDestSrvID, int nCount /*=0*/ )
{
	if( 0 != m_nItemTryCount )
		return 100;

	if( ( IsItemLock( nSrcSrvID ) == true )||( IsItemLock( nDestSrvID ) == true ) )
	{
		cPrintMsg::PrintMsg( 10004 );
		return 100;
	}

	// 같은 장소로 이동이면 패스
	if( nSrcSrvID == nDestSrvID )
		return 1; 

	// 움직일 수 있는 자리 인지 체크
	if( _IsEnableItemMove( nSrcSrvID, nDestSrvID ) == false )
	{
		cPrintMsg::PrintMsg( 11016 );
		return 2;
	}

	// 창고로 옮길수 없는 아이템인지
	if( TO_CONSTANT( nDestSrvID ) == SERVER_DATA_WAREHOUSE_CONSTANT )
	{
		switch( TO_CONSTANT( nSrcSrvID ) )
		{
		case SERVER_DATA_INVEN_CONSTANT:
			{
				cItemInfo* pItemInfo = GetInven()->GetData( TO_ID( nSrcSrvID ) );
				CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() )->GetInfo();
				if( pFTItem->IsPCBangType() ) //PC방 아이템인경우 
				{			
					cPrintMsg::PrintMsg( 20204 );
					return 3;
				}
			}
			break;		
		}		
	}

	if( TO_CONSTANT( nSrcSrvID ) == SERVER_DATA_DIGIVICE_CONSTANT )
	{
		if( _SendItemMove_Digivice( nSrcSrvID, nDestSrvID ) == false )
			return 4;
	}
	else if( TO_CONSTANT( nDestSrvID ) == SERVER_DATA_DIGIVICE_CONSTANT )
	{
		if( _SendItemMove_Digivice( nDestSrvID, nSrcSrvID ) == false )
			return 5;
	}
	else if( TO_CONSTANT( nDestSrvID ) == SERVER_DATA_TEQUIP_CONSTANT )// 장비 착용 예외 처리 
	{
		if( !_SendItemMove_Equiped( nDestSrvID, nSrcSrvID ) )
			return 9;
	}

	// 샌드 블록이면
	if( true == net::game->IsSendBlock() )
		return 6;

	// 악세서리 강화 중이면
	if( m_pEnchantOptionAdapt && m_pEnchantOptionAdapt->IsProcessingEnchant() )
		return 7;

	// 아이템 롹킹
	ItemLock( nSrcSrvID );
	ItemLock( nDestSrvID );

	// 갯수 받아 놓는다
	switch( TO_CONSTANT( nSrcSrvID ) )
	{
#ifdef CROSSWARS_SYSTEM
	case SERVER_DATA_INVENCROSS_CONSTANT:
		{
			m_nItemTryCount = ( nCount == 0 ? GetInvenCross()->GetData( TO_ID( nSrcSrvID ) )->GetCount() : nCount );
			if( net::game )
			{
				if( m_nItemTryCount == GetInvenCross()->GetData( TO_ID( nSrcSrvID ) )->GetCount() )
				{
					net::game->SendItemMove( nSrcSrvID, nDestSrvID );
				}
				else
				{
					// 부분 합체
					m_nItemTryCount = GetInvenCross()->CalItemSplit(  nSrcSrvID ,  nDestSrvID , m_nItemTryCount );

					assert_cs( m_nItemTryCount >= 0 );
					if( m_nItemTryCount == 0 )
					{
						ItemUnlock( nSrcSrvID );
						ItemUnlock( nDestSrvID );
						return 6;						
					}
					net::game->SendItemSplit( nSrcSrvID, nDestSrvID, m_nItemTryCount );
					
				}
			}
			else
			{
				ServerItemMoveSuccess( nSrcSrvID, nDestSrvID );
			}
		}
		break;
#endif
	case SERVER_DATA_INVEN_CONSTANT:
		{
			m_nItemTryCount = ( nCount == 0 ? GetInven()->GetData( TO_ID( nSrcSrvID ) )->GetCount() : nCount );
			if( net::game )
			{
				if( m_nItemTryCount == GetInven()->GetData( TO_ID( nSrcSrvID ) )->GetCount() )
				{
					net::game->SendItemMove( nSrcSrvID, nDestSrvID );
				}
				else
				{
					// 부분 합체
#ifdef NEW_SHARESTASH
					m_nItemTryCount = GetInven()->CalItemSplit(  nSrcSrvID ,  nDestSrvID , m_nItemTryCount );
#else
					m_nItemTryCount = GetInven()->CalItemSplit( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
#endif
					assert_cs( m_nItemTryCount >= 0 );
					if( m_nItemTryCount == 0 )
					{
						ItemUnlock( nSrcSrvID );
						ItemUnlock( nDestSrvID );
						return 6;						
					}
					net::game->SendItemSplit( nSrcSrvID, nDestSrvID, m_nItemTryCount );
				}
			}
			else
			{
				ServerItemMoveSuccess( nSrcSrvID, nDestSrvID );
			}
		}
		break;
	case SERVER_DATA_WAREHOUSE_CONSTANT:
		{
			m_nItemTryCount = ( nCount == 0 ? m_pWarehouseAdapt->GetData( TO_ID( nSrcSrvID ) )->GetCount() : nCount );

			if( net::game )
			{
				if( m_nItemTryCount == m_pWarehouseAdapt->GetData( TO_ID( nSrcSrvID ) )->GetCount() )
				{
					net::game->SendItemMove( nSrcSrvID, nDestSrvID );
				}
				else
				{
					// 부분 합체
#ifdef NEW_SHARESTASH
					m_nItemTryCount = m_pWarehouseAdapt->CalItemSplit(  nSrcSrvID ,  nDestSrvID , m_nItemTryCount );
#else
					m_nItemTryCount = m_pWarehouseAdapt->CalItemSplit( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
#endif
					assert_cs( m_nItemTryCount >= 0 );
					if( m_nItemTryCount == 0 )
					{
						ItemUnlock( nSrcSrvID );
						ItemUnlock( nDestSrvID );
						return 7;						
					}
					net::game->SendItemSplit( nSrcSrvID, nDestSrvID, m_nItemTryCount );
				}
			}
			else
			{
				ServerItemMoveSuccess( nSrcSrvID, nDestSrvID );
			}
		}
		break;
#ifdef NEW_SHARESTASH
	case SERVER_DATA_SHARESTASH_CONSTANT:
		{
			m_nItemTryCount = ( nCount == 0 ? m_pWarehouseAdapt->GetShareData( TO_ID( nSrcSrvID ) )->GetCount() : nCount );

			if( net::game )
			{
				if( m_nItemTryCount == m_pWarehouseAdapt->GetShareData( TO_ID( nSrcSrvID ) )->GetCount() )
				{
					net::game->SendItemMove( nSrcSrvID, nDestSrvID );
				}
				else
				{
					// 부분 합체
					m_nItemTryCount = m_pWarehouseAdapt->CalItemSplit_ShareStash(  nSrcSrvID ,  nDestSrvID , m_nItemTryCount );
					assert_cs( m_nItemTryCount >= 0 );
					if( m_nItemTryCount == 0 )
					{
						ItemUnlock( nSrcSrvID );
						ItemUnlock( nDestSrvID );
						return 8;						
					}
					net::game->SendItemSplit( nSrcSrvID, nDestSrvID, m_nItemTryCount );
				}
			}
			else
			{
				ServerItemMoveSuccess( nSrcSrvID, nDestSrvID );
			}
		}
		break;
#endif
	case SERVER_DATA_TEQUIP_CONSTANT:
	case SERVER_DATA_MEMORY_CONSTANT:
	case SERVER_DATA_CHIPSET_CONSTANT:
	case SERVER_DATA_DIGIVICE_CONSTANT:
		{
			m_nItemTryCount = 1;
			if( net::game )
			{
				net::game->SendItemMove( nSrcSrvID, nDestSrvID );
			}
			else
			{
				ServerItemMoveSuccess( nSrcSrvID, nDestSrvID );
			}
		}	
		break;	
	default:
		assert_cs( false );
	}	

	return 0;
}

bool cDataMng::_SendItemMove_Equiped( int nTEquipSrvID, int nInvenSrvID )
{
#ifdef SDM_TAMER_XGUAGE_20180628
	if( TO_ID( nTEquipSrvID ) != nsPART::XAI )
		return true;// 현재 XAI 아이템인 경우에만 체크함

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, false );
	CTamerUser::sUSER_STAT* pUserState = dynamic_cast<CTamerUser::sUSER_STAT*>(pUser->GetBaseStat());

	assert_cs( TO_CONSTANT( nInvenSrvID ) == SERVER_DATA_INVEN_CONSTANT );
	cItemInfo* pDestInfo = m_Inven.GetData( TO_ID( nInvenSrvID ) );
	SAFE_POINTER_RETVAL( pDestInfo, false );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, false );	
	TB::sXAIStruct const* pXaiStruct = nsCsFileTable::g_pItemMng->GetXaiItem_Info(pDestInfo->GetType());// 착용 하려는 아이템이 XAI
	SAFE_POINTER_RETVAL( pXaiStruct, false );

	if( pXaiStruct->s_ucMaxCrystal < pUserState->GetCurrentCrystal() || pXaiStruct->s_unMaxXG < pUserState->GetCurrentXGuage() )
	{
		cPrintMsg::PrintMsg( 32000 );
		cMessageBox* pMsg = cMessageBox::GetFirstMessageBox();
		if( pMsg )
		{
			pMsg->SetValue1( nInvenSrvID );
			pMsg->SetValue2( nTEquipSrvID );

			ItemTryCount();

			ItemLock( nInvenSrvID );
			ItemLock( nTEquipSrvID );
		}
		return false;
	}
#endif
	return true;
}

bool cDataMng::_SendItemMove_Digivice( int nTEquipSrvID, int nInvenSrvID )
{
	assert_cs( TO_CONSTANT( nInvenSrvID ) == SERVER_DATA_INVEN_CONSTANT );

	cItemInfo* pDestInfo = GetInven()->GetData( TO_ID( nInvenSrvID ) );
	// 인벤의 디지바이스와 교체하려는것인가
	if( pDestInfo->IsEnable() == true )
	{
		CsItem::sINFO* pFTDest = nsCsFileTable::g_pItemMng->GetItem( pDestInfo->GetType() )->GetInfo();
		assert_cs( pFTDest->s_nType_L == 53 );

		// 칩셋
		int nNeedEmptySlot = 0;
		int nChipsetCount = GetDigivice()->GetChipsetCount();
		if( nChipsetCount > pFTDest->s_nDigiviceChipsetSlot )
		{
			for( int i=pFTDest->s_nDigiviceChipsetSlot; i<nChipsetCount; ++i )
			{
				if( GetDigivice()->GetChipset( i )->IsEnable() )
					++nNeedEmptySlot;
			}
		}
		int nSKillCount = GetDigivice()->GetTamerSkillCount();
		if( nSKillCount > pFTDest->s_nDigiviceSkillSlot )
		{
			for( int i=pFTDest->s_nDigiviceSkillSlot; i<nSKillCount; ++i )
			{
				if( GetDigivice()->GetTamerSkill( i )->IsEnable() )
					++nNeedEmptySlot;
			}
		}

		// 빈칸 만큼 인벤이 존재하는가
		if( nNeedEmptySlot > GetInven()->GetEmptySlotCount() )
		{
			cPrintMsg::PrintMsg( 11015 );
			return false;
		}

		if( m_nItemTryCount != 0 )
		{
			return false;
		}

		// 인벤의 여유 공간이 충분하다면 아이템을 인벤으로 옴겨주자
		int nFindSlot = GetInven()->FindEmptySlot( -1 );
		for( int i=pFTDest->s_nDigiviceChipsetSlot; i<nChipsetCount; ++i )
		{
			if( GetDigivice()->GetChipset( i )->IsEnable() )
			{
				m_nItemTryCount  = 0;
				assert_cs( nFindSlot != cData_Inven::INVALIDE_INVEN_INDEX );
				SendItemMove( TO_CHIPSET_SID( i ), TO_INVEN_SID( nFindSlot ), 1 );
				nFindSlot = GetInven()->FindEmptySlot( nFindSlot );						
			}
		}
		for( int i=pFTDest->s_nDigiviceSkillSlot; i<nSKillCount; ++i )
		{
			if( GetDigivice()->GetTamerSkill( i )->IsEnable() )
			{
				m_nItemTryCount  = 0;
				assert_cs( nFindSlot != cData_Inven::INVALIDE_INVEN_INDEX );
				SendItemMove( TO_TSKILL_SID( i ), TO_INVEN_SID( nFindSlot ), 1 );
				nFindSlot = GetInven()->FindEmptySlot( nFindSlot );
			}
		}
	}
	// 빈곳에 넣으려는 것이다.
	else
	{
		int nNeedEmptySlot = 1;		// 디지바이스 슬롯이라 기본 한개 필요
		int nChipsetCount = GetDigivice()->GetChipsetCount();
		for( int i=0; i<nChipsetCount; ++i )
		{
			if( GetDigivice()->GetChipset( i )->IsEnable() )
				++nNeedEmptySlot;
		}
		int nSKillCount = GetDigivice()->GetTamerSkillCount();
		for( int i=0; i<nSKillCount; ++i )
		{
			if( GetDigivice()->GetTamerSkill( i )->IsEnable() )
				++nNeedEmptySlot;
		}

		// 빈칸 만큼 인벤이 존재하는가
		if( nNeedEmptySlot > GetInven()->GetEmptySlotCount() )
		{
			cPrintMsg::PrintMsg( 11015 );
			return false;
		}

		// 인벤의 여유 공간이 충분하다면 아이템을 인벤으로 옴겨주자
		int nFindSlot = GetInven()->FindEmptySlot( -1, TO_ID( nInvenSrvID ) );
		for( int i=0; i<nChipsetCount; ++i )
		{
			if( GetDigivice()->GetChipset( i )->IsEnable() )
			{
				m_nItemTryCount  = 0;
				assert_cs( nFindSlot != cData_Inven::INVALIDE_INVEN_INDEX );
				SendItemMove( TO_CHIPSET_SID( i ), TO_INVEN_SID( nFindSlot ), 1 );
				nFindSlot = GetInven()->FindEmptySlot( nFindSlot, TO_ID( nInvenSrvID ) );						
			}
		}
		for( int i=0; i<nSKillCount; ++i )
		{
			if( GetDigivice()->GetTamerSkill( i )->IsEnable() )
			{
				m_nItemTryCount  = 0;
				assert_cs( nFindSlot != cData_Inven::INVALIDE_INVEN_INDEX );
				SendItemMove( TO_TSKILL_SID( i ), TO_INVEN_SID( nFindSlot ), 1 );
				nFindSlot = GetInven()->FindEmptySlot( nFindSlot, TO_ID( nInvenSrvID ) );
			}
		}
	}

	return true;	
}

void cDataMng::ServerItemMoveSuccess( int nSrcSrvID, int nDestSrvID )
{
	assert_cs( IsItemLock( nSrcSrvID ) == true );
	assert_cs( IsItemLock( nDestSrvID ) == true );
	ItemUnlock( nSrcSrvID );
	ItemUnlock( nDestSrvID );

	int nSrcID = TO_ID( nSrcSrvID );
	int nDestID = TO_ID( nDestSrvID );
	int nSrcConstant = TO_CONSTANT( nSrcSrvID );
	int nDestConstant = TO_CONSTANT( nDestSrvID );

	switch( nSrcConstant )
	{
#ifdef CROSSWARS_SYSTEM
	case SERVER_DATA_INVENCROSS_CONSTANT:
		{
			switch( nDestConstant )
			{
			case SERVER_DATA_INVENCROSS_CONSTANT:				
				
				GetInvenCross()->CrossToCross( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
				cWindow::PlaySound( cWindow::SD_At3 );
				break;
			case SERVER_DATA_INVEN_CONSTANT:

				GetInvenCross()->CrossToInven( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
				cWindow::PlaySound( cWindow::SD_At3 );
				break;

			default:
				assert_cs( false );
			}
		}
		break;
#endif
		// 인벤에서
	case SERVER_DATA_INVEN_CONSTANT:
		{
			switch( nDestConstant )
			{
#ifdef CROSSWARS_SYSTEM
			case SERVER_DATA_INVENCROSS_CONSTANT:				
				GetInvenCross()->InvenToCross( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
				cWindow::PlaySound( cWindow::SD_At3 );
				break;
#endif
				// 인벤으로
			case SERVER_DATA_INVEN_CONSTANT:
				GetInven()->ItemMove( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
				cWindow::PlaySound( cWindow::SD_At3 );
				break;
				// 창고로
			case SERVER_DATA_WAREHOUSE_CONSTANT:
				m_pWarehouseAdapt->InvenToWarehouse( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
				cWindow::PlaySound( cWindow::SD_At3 );

				// 퀘스트 체크
				g_pDataMng->GetQuest()->CalProcess();
				g_pDataMng->GetQuest()->RevQuestCheck();
				// 퀵슬롯 체크
				cData_QuickSlot::CheckItemCount_AllUser();

				break;
#ifdef NEW_SHARESTASH
				// 공유 창고로
			case SERVER_DATA_SHARESTASH_CONSTANT:
				m_pWarehouseAdapt->InvenToShareStash( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
				cWindow::PlaySound( cWindow::SD_At3 );

				// 퀘스트 체크
				g_pDataMng->GetQuest()->CalProcess();
				g_pDataMng->GetQuest()->RevQuestCheck();
				// 퀵슬롯 체크
				cData_QuickSlot::CheckItemCount_AllUser();

				break;
#endif
				// 테이머 장비 창으로
			case SERVER_DATA_TEQUIP_CONSTANT:
				assert_cs( m_nItemTryCount == 1 );
				GetTEquip()->ToInven( TO_ID( nDestSrvID ), TO_ID( nSrcSrvID ) );
				cWindow::PlaySound( cWindow::SD_At1 );
				break;
				// 디지바이스 아이템으로
			case SERVER_DATA_DIGIVICE_CONSTANT:
				GetTEquip()->ToInven_Digivice( TO_ID( nSrcSrvID ) );
				cWindow::PlaySound( cWindow::SD_At1 );
				break;
				// 칩셋으로
			case SERVER_DATA_CHIPSET_CONSTANT:
				//assert_cs( m_nItemTryCount == 1 );
				GetDigivice()->ChipsetToInven( TO_ID( nDestSrvID ), TO_ID( nSrcSrvID ) );
				cWindow::PlaySound( cWindow::SD_At1 );
				break;
			default:
				assert_cs( false );
			}
		}
		break;
		// 장비에서
	case SERVER_DATA_TEQUIP_CONSTANT:
		{
			switch( nDestConstant )
			{
				// 인벤으로
			case SERVER_DATA_INVEN_CONSTANT:
				assert_cs( m_nItemTryCount == 1 );
				GetTEquip()->ToInven( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ) );
				cWindow::PlaySound( cWindow::SD_Np1 );
				break;
			default:
				assert_cs( false );					// 다른데로의 교체는 있을 수 없다.
			}
		}
		break;
		// 장비창 디지바이스 아이템에서
	case SERVER_DATA_DIGIVICE_CONSTANT:
		{
			switch( nDestConstant )
			{
				// 인벤으로
			case SERVER_DATA_INVEN_CONSTANT:
				//assert_cs( m_nItemTryCount == 1 );
				GetTEquip()->ToInven_Digivice( TO_ID( nDestSrvID ) );
				cWindow::PlaySound( cWindow::SD_Np1 );
				break;
			default:
				assert_cs( false );					// 다른데로의 교체는 있을 수 없다.
			}
		}
		break;
		// 칩셋에서
	case SERVER_DATA_CHIPSET_CONSTANT:
		{
			switch( nDestConstant )
			{
				// 인벤으로
			case SERVER_DATA_INVEN_CONSTANT:
				//assert_cs( m_nItemTryCount == 1 );
				GetDigivice()->ChipsetToInven( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ) );
				cWindow::PlaySound( cWindow::SD_Np1 );
				break;
				// 칩셋으로
			case SERVER_DATA_CHIPSET_CONSTANT:
				//assert_cs( m_nItemTryCount == 1 );
				GetDigivice()->ChipsetToChipset( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ) );
				cWindow::PlaySound( cWindow::SD_At1 );
				break;
			default:
				assert_cs( false );
			}
		}
		break;
		// 창고에서
	case SERVER_DATA_WAREHOUSE_CONSTANT:
		{
			switch( nDestConstant )
			{
				// 인벤으로
			case SERVER_DATA_INVEN_CONSTANT:
				m_pWarehouseAdapt->WarehouseToInven( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
				cWindow::PlaySound( cWindow::SD_At3 );

				// 퀘스트 체크
				g_pDataMng->GetQuest()->CalProcess();
				g_pDataMng->GetQuest()->RevQuestCheck();
				// 퀵슬롯 체크
				cData_QuickSlot::CheckItemCount_AllUser();

				break;
				// 창고로
			case SERVER_DATA_WAREHOUSE_CONSTANT:
				m_pWarehouseAdapt->WarehouseToWarehouse( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
				cWindow::PlaySound( cWindow::SD_At3 );
				break;
#ifdef NEW_SHARESTASH
			case SERVER_DATA_SHARESTASH_CONSTANT:
				m_pWarehouseAdapt->WarehouseToShareStash( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
				cWindow::PlaySound( cWindow::SD_At3 );
				break;
#endif
			default:
				assert_cs( false );
			}
		}
		break;
#ifdef NEW_SHARESTASH
	case SERVER_DATA_SHARESTASH_CONSTANT:
		{
			switch( nDestConstant )
			{
				// 인벤으로
			case SERVER_DATA_INVEN_CONSTANT:
				m_pWarehouseAdapt->ShareStashToInven( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
				cWindow::PlaySound( cWindow::SD_At3 );

				// 퀘스트 체크
				g_pDataMng->GetQuest()->CalProcess();
				g_pDataMng->GetQuest()->RevQuestCheck();
				// 퀵슬롯 체크
				cData_QuickSlot::CheckItemCount_AllUser();

				break;
				// 창고로
			case SERVER_DATA_WAREHOUSE_CONSTANT:
				m_pWarehouseAdapt->ShareStashToWarehouse( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
				cWindow::PlaySound( cWindow::SD_At3 );
				break;
			case SERVER_DATA_SHARESTASH_CONSTANT:
				m_pWarehouseAdapt->ShareStashToShareStash( TO_ID( nSrcSrvID ), TO_ID( nDestSrvID ), m_nItemTryCount );
				cWindow::PlaySound( cWindow::SD_At3 );
				break;
			default:
				assert_cs( false );
			}
		}
		break;
#endif
	default:
		assert_cs( false );
	}	

	m_nItemTryCount = 0;
}

void cDataMng::ResetItemTryCount()
{
	m_nItemTryCount = 0;	
}

void cDataMng::ItemTryCount()
{
	m_nItemTryCount = 1;
}

void cDataMng::ServerItemMoveFailed( int nSrcSrvID, int nDestSrvID )
{
	assert_cs( IsItemLock( nSrcSrvID ) == true );
	assert_cs( IsItemLock( nDestSrvID ) == true );

	m_nItemTryCount = 0;

	ItemUnlock( nSrcSrvID );
	ItemUnlock( nDestSrvID );

	_ReleaseSort(false);	

#ifdef NEW_WAREHOUSESORT
	m_pWarehouseAdapt->_ReleaseWareHouseSort(false);
#endif

#ifdef NEW_SHARESTASH
	m_pWarehouseAdapt->_ReleaseShareStashSort(false);
#endif

	if( net::game )
		net::game->SendQueryMoneyInven();	// 인밴에 돈을 갱신하는것 같은데...
}

bool cDataMng::IsTradeItem( cItemInfo* pItemInfo )
{
	// 라이딩 상태인지 체크
	if( g_pCharMng->GetDigimonUser( 0 )->IsRide() )
	{
		cPrintMsg::PrintMsg( 30014 );
		return false;
	}

	// 퀘스트 아이템인지
	CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() )->GetInfo();
	SAFE_POINTER_RETVAL( pFTInfo, false );

	if( ( pFTInfo->s_nType_L == nItem::Quest )||( pFTInfo->s_nType_L == nItem::UseQuest ) )
	{
		cPrintMsg::PrintMsg( TRADE_RESULT_CANTNOT_TRADE_LIMITED_ITEM );
		return false;
	}

	// 귀속된 아이템인지	
	if( pItemInfo->m_nLimited != 0 )
	{
		cPrintMsg::PrintMsg( TRADE_RESULT_CANTNOT_TRADE_LIMITED_ITEM );
		return false;
	}

#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
	if( cData_Inven::IsTradeLimitedItem(pItemInfo) )
	{
		cPrintMsg::PrintMsg( TRADE_RESULT_CANTNOT_TRADE_LIMITED_CASHITEM );
		return false;
	}
#endif
	return true;
}

bool cDataMng::IsDropItem( cItemInfo* pItemInfo )
{
	// 물건 판매나 구매 중일때는 아이템 버리는거 안됨
//	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE ) )
//	{
//		cPersonStore* pUI =	g_pGameIF->GetPersonStore();
// 		switch( pUI->GetValueType() )
// 		{
// 		case PersonStoreContents::VT_RES_PERSON:
// 		case PersonStoreContents::VT_RES_PERSON_START:
// 		case PersonStoreContents::VT_RES_EMPLOYMENT:
// 		case PersonStoreContents::VT_RES_EMPLOYMENT_START:
// 			cPrintMsg::PrintMsg( 30357 );
// 			return false;
// 		}
//	}

	return true;
}

bool cDataMng::IsOpenStore()
{
	// 라이딩 상태인지 체크
	if( g_pCharMng->GetDigimonUser( 0 )->IsRide() )
	{
		cPrintMsg::PrintMsg( 30014 );
		return false;
	}

	// 현재 상황에서는 거래가 되지 않는다. ( Npc이용, 전투중, 거래중, 진화중, 파트너몬 교체중 )
	if( g_pGameIF->IsOpenNpcWindow() || g_pCharMng->GetDigimonUser( 0 )->IsBattle() || g_pDataMng->GetServerSync()->IsChageDigimon() )
	{
		cPrintMsg::PrintMsg( 30024 );
		return false;
	}

	// 메세지 박스 떠있는게 있다면 거래 실패
	if( cMessageBox::IsMessageBox() )
	{
		cPrintMsg::PrintMsg( 30024 );
		return false;
	}

	// 피로한 상태이면 안된다	
	if( g_pCharMng->GetTamerUser()->GetCondition()->IsCondition( nSync::Fatigue ) == true )
	{
		cPrintMsg::PrintMsg( 30358 );
		return false;
	}

	float fEnableDist = nsCsFileTable::g_pBaseMng->GetPersonStore()->s_BaseInfo.s_fStoreDist;
	NiPoint2 ptUser = g_pCharMng->GetTamerUser()->GetPos2D();

#ifdef CONSIGNMENT_CREATE_RESTRICT
	float fNpcDist = nsCsFileTable::g_pBaseMng->GetPersonStore()->s_BaseInfo.s_fNpcDist;
	{
		// 주변에 포탈이 있는지 검사
		if( nsCsGBTerrain::g_pCurRoot->GetPotalCheck( ptUser, fNpcDist ) )
		{
			cPrintMsg::PrintMsg( 30350 );
			return false;
		}
	}
#endif

	// 주변에 다른 테이머 있는지 검사
	{
		std::map< DWORD, CTamer* >* pVector = g_pCharMng->GetTamerMap();
		std::map< DWORD, CTamer* >::iterator it = pVector->begin();
		for( ; it != pVector->end(); ++it )
		{
			CTamer* pTamer = it->second;
			SAFE_POINTER_CON( pTamer );

			if( pTamer->GetCondition()->IsCondition( (nSync::eCondition)( nSync::Shop | nSync::ShopReady ) ) == 0 )
				continue;

			if( ( pTamer->GetPos2D() - ptUser ).Length() < fEnableDist )
			{
				cPrintMsg::PrintMsg( 30350 );
				return false;
			}
		}
	}
	
	// 주변에 Npc있는지 검사
	{
		CNpcMng::NpcCont* pVector = g_pNpcMng->GetNpcVector();
		CNpcMng::NpcCont::iterator it = pVector->begin();
		for( ; it != pVector->end(); ++it )
		{
			CNpc* pNpc = it->second;
			SAFE_POINTER_CON( pNpc );
#ifdef CONSIGNMENT_CREATE_RESTRICT
			if( ( pNpc->GetPos2D() - ptUser ).Length() < fNpcDist )
#else
			if( ( pNpc->GetPos2D() - ptUser ).Length() < fEnableDist )
#endif
			{
				cPrintMsg::PrintMsg( 30350 );
				return false;
			}
		}
	}


	// 주변에 다른 위탁상점 있는지 검사
	{
		std::map<DWORD, CEmployment*>* pVector = g_pCharMng->GetEmploymentMap();
		std::map<DWORD, CEmployment*>::iterator it = pVector->begin();
		for( ; it != pVector->end(); ++it )
		{
			CEmployment* pEmployment = it->second;
			SAFE_POINTER_CON( pEmployment );
			if( ( pEmployment->GetPos2D() - ptUser ).Length() < fEnableDist )
			{
				cPrintMsg::PrintMsg( 30350 );
				return false;
			}
		}
	}
	

	return true;
}

bool cDataMng::IsScanItem(cItemInfo* pItemInfo)
{
	SAFE_POINTER_RETVAL( pItemInfo, false );	
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, false );

	bool bIsRank = false;
	bool bIsType = false;

	// 아이템 랭크 검사
	const CsItemRank::MAP* pkRanks = nsCsFileTable::g_pItemMng->GetItemRank();
	CsItemRank::MAP::const_iterator it = pkRanks->find( pItemInfo->GetType() );
	if(it != pkRanks->end())
		bIsRank = true;

	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() );
	SAFE_POINTER_RETVAL( pFTItem, false );

	CsItem::sINFO* pFTItemInfo = pFTItem->GetInfo();
	SAFE_POINTER_RETVAL( pFTItemInfo, false );

	// 아이템 타입 검사
	switch( pFTItemInfo->s_nType_L )
	{
	case nItem::DigimonSkillChipATK:	//	67	// 공격형 특성 메모리
	case nItem::DigimonSkillChipDEF:	//	68	// 방어형 특성 메모리
	case nItem::DigimonSkillChipAST:	//	69	// 보조형 특성 메모리
	case nItem::Digitama2:		//	92
	case nItem::Digitama3:		//	93
	case nItem::Cash_Evoluter: // [3/14/2016 hyun] 추가	150
#if COMMON_LIB_FIXED
	case nItem::Cash_Evoluter2:
#endif
	case nItem::Cash_DigiCore:	//	161
	case nItem::EvoCapsule:		//	193
	case nItem::Spirit:			//	195
		bIsType = true;
	}	

	if( bIsRank && bIsType )
		return true;

	return false;
}

bool cDataMng::IsSellItem(cItemInfo* pItemInfo)
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, false );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() );
	SAFE_POINTER_RETVAL( pFTItem, false );
	CsItem::sINFO* pFTItemInfo = pFTItem->GetInfo();
	SAFE_POINTER_RETVAL( pFTItemInfo, false );

	// 파는 가격이 없을 경우
	if( pFTItemInfo->s_dwSale == 0 )
	{
		//cPrintMsg::PrintMsg( 11001 );
		return false;
	}

	// 타입 체크
	switch(pFTItemInfo->s_nType_L)
	{
	case nItem::Digitama1:		//	91
	case nItem::Digitama2:		//	92
	case nItem::Digitama3:		//	93
		//cPrintMsg::PrintMsg( 11001 );
		return true;
	}

	return false;
}
//==============================================================================================
//
//		플레이 타임 제한
//
//==============================================================================================
#ifdef PLAY_PANELTY

u4 cDataMng::PlayTime_Get_GameResetTime()
{
	assert_cs( net::game );

	tm time;
	nBase::GetTime2TM( net::game->GetTimeTS(), time );
	time.tm_year += 1900;
	time.tm_mon += 1;

	int nHour = 23;
	int nMin = 59;
	int nSec = 60;
	nSec -= time.tm_sec;
	nMin -= time.tm_min;
	nHour -= time.tm_hour;
	return nSec + nMin*60 + nHour*60*60;
}

void cDataMng::PlayTime_GameResetTime_PrintMsg()
{
	int nRTime = g_pDataMng->PlayTime_Get_GameResetTime();
	int nH = nRTime / 3600;
	nRTime = nRTime % 3600;
	int nM = nRTime / 60;			

	TCHAR sz[ 64 ];
	_stprintf_s( sz, 64, L"%.2d : %.2d", nH, nM );
	cPrintMsg::PrintMsg( 30416, sz );
}

#endif

bool cDataMng::SystemBuffCheck(int	InvenIdx, CsItem* pFTItem, CsC_AvObject* pTarget, USHORT	BuffID)
{
	CsBuff::sINFO* pFTBuffInfo = nsCsFileTable::g_pBuffMng->GetBuff( BuffID )->GetInfo();
	if( pFTBuffInfo->s_nBuffType == 3 ) // 시스템 버프인지 확인
	{
		CsBuff::sINFO* pFTFindExist = NULL;
		// 같은 클래스의 버프가 있는지 체크
		cBuffData* pBuffData = g_pCharMng->GetTamerUser()->GetBuff();
		std::list< cBuffData::sBUFF_DATA >* pList = pBuffData->GetSystemBuff();
		std::list< cBuffData::sBUFF_DATA >::iterator it = pList->begin();
		std::list< cBuffData::sBUFF_DATA >::iterator itEnd = pList->end();
		for( ; it!=itEnd; ++it )
		{
			if( ( *it ).s_pBuffTable->GetInfo()->s_nBuffClass == pFTBuffInfo->s_nBuffClass )
			{
				if( ( *it ).s_pBuffTable->GetInfo()->s_nMinLv <= pFTBuffInfo->s_nMinLv )
				{
					pFTFindExist = ( *it ).s_pBuffTable->GetInfo();
					break;
				}
			}
		}

		if( pFTFindExist )
		{
			// 더 높은 버프가 적용중일떄
			cPrintMsg::PrintMsg( 30453, pFTFindExist->s_szName );	
			return true;
		}
		else
		{
			cPrintMsg::PrintMsg( 20021, pFTItem->GetInfo()->s_szName );	
			cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
			pMBox->SetValue1( InvenIdx );
			pMBox->SetValue2( pTarget->GetUniqID() );
			pMBox->SetValue3( 0 );
			return true;
		}
	}
	return false;
}
bool cDataMng::AlreadyBuffCheck(int InvenIdx, CsItem* pFTItem, CsC_AvObject* pTarget, USHORT	BuffID)
{
	CsBuff::sINFO* pFTBuffInfo = nsCsFileTable::g_pBuffMng->GetBuff( BuffID )->GetInfo();

	if( pFTBuffInfo->s_nBuffType == 1 ) // 버프효과인지.
	{
		//	유저(테이머)의 중복된 버프가 있는지 검사
		cBuffData* pBuffData = g_pCharMng->GetTamerUser()->GetBuff();
		std::list< cBuffData::sBUFF_DATA >* pList = pBuffData->GetBuff();
		std::list< cBuffData::sBUFF_DATA >::iterator it = pList->begin();
		std::list< cBuffData::sBUFF_DATA >::iterator itEnd = pList->end();
		for( ; it!=itEnd; ++it )
		{
			cBuffData::sBUFF_DATA pBuff = ( *it );
			CsBuff* pCsBuff = pBuff.s_pBuffTable;
			SAFE_POINTER_CON( pCsBuff );
			CsBuff::sINFO* pBuffInfo = pCsBuff->GetInfo();
			SAFE_POINTER_CON( pBuffInfo );
			
			// 중복 버프
			if( pBuffInfo->s_dwSkillCode == pFTBuffInfo->s_dwSkillCode ||	// 스킬 코드가 같거나
				pBuffInfo->s_nBuffClass == pFTBuffInfo->s_nBuffClass )	// 버프 클래스가 같은 경우
			{
				// 레벨에 따른 처리
				OverlapBuffLevelCheck((int)pBuff.s_nBuffCurLV, pFTItem, InvenIdx, pTarget->GetUniqID());
				return true;
			}
		}

		//	디지몬의 중복된 버프가 있는지 검사
		pBuffData = g_pCharMng->GetDigimonUser(0)->GetBuff();
		pList = pBuffData->GetBuff();
		it = pList->begin();
		itEnd = pList->end();  
		for( ; it!=itEnd; ++it )
		{
			cBuffData::sBUFF_DATA pBuff = ( *it );
			CsBuff* pCsBuff = pBuff.s_pBuffTable;
			SAFE_POINTER_CON( pCsBuff );
			CsBuff::sINFO* pBuffInfo = pCsBuff->GetInfo();
			SAFE_POINTER_CON( pBuffInfo );

			// 중복 버프
			// 중복 버프
			if( pBuffInfo->s_dwSkillCode == pFTBuffInfo->s_dwSkillCode ||	// 스킬 코드가 같거나
				pBuffInfo->s_nBuffClass == pFTBuffInfo->s_nBuffClass )	// 버프 클래스가 같은 경우
			{
				// 레벨에 따른 처리
				OverlapBuffLevelCheck((int)pBuff.s_nBuffCurLV, pFTItem, InvenIdx, pTarget->GetUniqID());
				return true;
			}
		}
	}
	return false;
}

void cDataMng::OverlapBuffLevelCheck(int PrevBuffLevel, CsItem* pFTItem, int Value1, int Value2)
{
	SAFE_POINTER_RET(pFTItem);

	//	PrevBuffLevel;	// 이전 버프 단계
	int	BuffLevel = pFTItem->GetInfo()->s_nTypeValue;	// 현재 버프 단계

	// 새로운 버프가 같거나 더 높을 때
	if( PrevBuffLevel <= BuffLevel)
	{
		cPrintMsg::PrintMsg( 30456, pFTItem->GetInfo()->s_szName );	
		cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
		pMBox->SetValue1( Value1 );
		pMBox->SetValue2( Value2 );
		pMBox->SetValue3( 0 );
	}
	// 상위 레벨 버프가 적용되어 있습니다.
	else
	{
		cPrintMsg::PrintMsg( 30452 );
	}
}