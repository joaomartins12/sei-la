#include "StdAfx.h"
#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "AchieveContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../Adapt/AdaptDigimonAchive.h"
int const AchieveContents::IsContentsIdentity(void)
{
	return E_CT_ACHIEVE_CONTENTS;
}

AchieveContents::AchieveContents(void)
{	
	m_AdaptDigimonAchive = 0;
	m_nDispTitle		= 0;
	m_nRankAchieve		= 0;
	m_nQuestCompCount	= 0;;	

	m_mapComp.clear();
	m_listProc.clear();
	m_seqCheckAll.SetEndTime( 30.0f );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ACHIEVE_SET_DISPTITLE,	 this, &AchieveContents::Set_DispTitle );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ACHIEVE_SET_RANKACHIEVE, this, &AchieveContents::Set_RankAchieve );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE,	 this, &AchieveContents::Set_CheckType );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ACHIEVE_GET_DISPTITLE,  this, &AchieveContents::Get_DispTitle );
}

AchieveContents::~AchieveContents(void)
{
	m_AdaptDigimonAchive = 0;

	m_mapComp.clear();
	m_listProc.clear();

	GAME_EVENT_ST.DeleteEventAll( this );
}

int const AchieveContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

void AchieveContents::MakeWorldData(void)
{
	FirstPostLoad();
}

void AchieveContents::ClearWorldData(void)
{

}
bool AchieveContents::IntraConnection(ContentsSystem& System)
{
	m_AdaptDigimonAchive = new AdaptDigimonArchiveSystem;
	if( m_AdaptDigimonAchive && !m_AdaptDigimonAchive->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_DIGIMON_ARCHIVE_CONTENTS ) ) )
		return false;
	return true; 
}
//////////////////////////////////////////////////////////////////////////
void AchieveContents::Set_DispTitle(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_DISPTITLE* pRecv = static_cast<GS2C_RECV_DISPTITLE*>(pData);

	m_nDispTitle = pRecv->dispTitle;
}

void AchieveContents::Set_RankAchieve(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_RANKACHIEVE* pRecv = static_cast<GS2C_RECV_RANKACHIEVE*>(pData);

	m_nRankAchieve = pRecv->rankAchieve;
}

void AchieveContents::Set_CheckType(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_CHECKTYPE* pRecv = static_cast<GS2C_RECV_CHECKTYPE*>(pData);

	CheckType(AchieveContents::eTYPE(pRecv->nType), pRecv->nValue1, pRecv->nValue2);
}

void AchieveContents::Get_DispTitle(void* pData)
{
	SAFE_POINTER_RET( pData );
	DWORD* nData = static_cast<DWORD*>(pData);

	*nData = m_nDispTitle;
}

void AchieveContents::DisplayInfo( CsAchieve::sINFO* pFTInfo )
{
	std::wstring wsAcieve = UISTRING_TEXT( "ACHIEVE_MSG_ACHIEVED" );
	DmCS::StringFn::Replace( wsAcieve, L"#Achieve#", pFTInfo->s_szName );
	g_pAchieveInfo->SetAchieveInfo( wsAcieve.c_str() );
	cPrintMsg::PrintMsg( 30345, pFTInfo->s_szName );

	// 업적창이 열려 있다면 닫아주자
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ACHIEVE ) )
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ACHIEVE );
	}
}

void AchieveContents::AchieveRank()
{	
	RemoveAchieveRank();

	std::list< CsAchieve* >::iterator it; 
	std::list< CsAchieve* >::iterator itEnd; 	

	// 테이머 랭크 업적 체크	
	if( ( m_nRankAchieve & Rank_Tamer ) != 0 )
	{
		if( g_pDataMng->GetQuest()->GetInfo()->CheckAchieve( ACHIEVE_TAMERRANK ) == nQuest::None )
		{
			if( net::game )
			{
				net::game->SendQuestAchievement( ACHIEVE_TAMERRANK );
			}	
			g_pDataMng->GetQuest()->GetInfo()->CompleteAchievement( ACHIEVE_TAMERRANK );
		}

		it = m_listProc.begin();
		itEnd = m_listProc.end();
		while( it!=itEnd )
		{
			if( (*it)->GetInfo()->s_nQuestID == ACHIEVE_TAMERRANK )
			{
				m_mapComp[ ACHIEVE_TAMERRANK ] = (*it);
				m_listProc.erase( it );
				break;
			}
			it++;
		}		
	}
	else
	{
		std::map< DWORD, CsAchieve* >::iterator itCom = m_mapComp.find( ACHIEVE_TAMERRANK );
		if( itCom != m_mapComp.end() )
		{
			m_mapComp.erase( itCom );
			m_listProc.push_back( itCom->second );				
		}
	}	

	// 디지몬 랭크 업적 체크	
	if( ( m_nRankAchieve & Rank_Digimon ) != 0 ||
		( m_nRankAchieve & Rank_Tactics ) != 0	)
	{
		if( g_pDataMng->GetQuest()->GetInfo()->CheckAchieve( ACHIEVE_DIGIMONRANK ) == nQuest::None )
		{
			if( net::game )
			{
				net::game->SendQuestAchievement( ACHIEVE_DIGIMONRANK );
			}	
			g_pDataMng->GetQuest()->GetInfo()->CompleteAchievement( ACHIEVE_DIGIMONRANK );			
		}

		it = m_listProc.begin();
		itEnd = m_listProc.end();
		while( it!=itEnd )
		{		
			if( (*it)->GetInfo()->s_nQuestID == ACHIEVE_DIGIMONRANK )
			{
				m_mapComp[ ACHIEVE_DIGIMONRANK ] = (*it);
				m_listProc.erase( it );
				break;
			}
			it++;
		}		
	}	
	else
	{
		std::map< DWORD, CsAchieve* >::iterator itCom = m_mapComp.find( ACHIEVE_DIGIMONRANK );
		if( itCom != m_mapComp.end() )
		{
			m_mapComp.erase( itCom );
			m_listProc.push_back( itCom->second );			
		}
	}

	// 디지몬 크기 업적 체크	
	if( ( m_nRankAchieve & Rank_Size ) != 0 )
	{
		if( g_pDataMng->GetQuest()->GetInfo()->CheckAchieve( ACHIEVE_DIGIMONSIZE ) == nQuest::None )
		{
			if( net::game )
			{
				net::game->SendQuestAchievement( ACHIEVE_DIGIMONSIZE );
			}	
			g_pDataMng->GetQuest()->GetInfo()->CompleteAchievement( ACHIEVE_DIGIMONSIZE );			
		}

		it = m_listProc.begin();
		itEnd = m_listProc.end();
		while( it!=itEnd )
		{		
			if( (*it)->GetInfo()->s_nQuestID == ACHIEVE_DIGIMONSIZE )
			{
				m_mapComp[ ACHIEVE_DIGIMONSIZE ] = (*it);
				m_listProc.erase( it );
				break;
			}
			it++;
		}		
	}	
	else
	{
		std::map< DWORD, CsAchieve* >::iterator itCom = m_mapComp.find( ACHIEVE_DIGIMONSIZE );
		if( itCom != m_mapComp.end() )
		{
			m_mapComp.erase( itCom );
			m_listProc.push_back( itCom->second );			
		}
	}


	// 둘다 랭크 업적 체크
	if( ( ( m_nRankAchieve & Rank_Digimon ) != 0 && ( m_nRankAchieve & Rank_Tamer ) != 0 &&  ( m_nRankAchieve & Rank_Size ) != 0 ) ||		 
		( ( m_nRankAchieve & Rank_Tactics ) != 0 && ( m_nRankAchieve & Rank_Tamer ) != 0 && ( m_nRankAchieve & Rank_Size ) != 0 ) )
	{		
		if( g_pDataMng->GetQuest()->GetInfo()->CheckAchieve( ACHIEVE_RANK ) == nQuest::None )
		{
			if( net::game )
			{
				net::game->SendQuestAchievement( ACHIEVE_RANK );
			}	
			g_pDataMng->GetQuest()->GetInfo()->CompleteAchievement( ACHIEVE_RANK );
		}	

		it = m_listProc.begin();
		itEnd = m_listProc.end();
		while( it!=itEnd )
		{
			if( (*it)->GetInfo()->s_nQuestID == ACHIEVE_RANK )
			{
				m_mapComp[ ACHIEVE_RANK ] = (*it);
				m_listProc.erase( it );
				break;
			}
			it++;
		}				
	}	
	else
	{
		std::map< DWORD, CsAchieve* >::iterator itCom = m_mapComp.find( ACHIEVE_RANK );
		if( itCom != m_mapComp.end() )
		{
			m_mapComp.erase( itCom );
			m_listProc.push_back( itCom->second );				
		}		
	}
}

bool AchieveContents::RemoveAchieveRank()
{	
	if( ( m_nRankAchieve & Rank_Tamer ) == 0 )
	{
		if( m_nDispTitle == ACHIEVE_TAMERRANK  )
		{
			if( net::game )
				net::game->SendTamerAchievement( 0 );

			m_nDispTitle = 0;
			return true;
		}	
	}

	if( ( m_nRankAchieve & Rank_Digimon ) == 0 &&
		( m_nRankAchieve & Rank_Tactics ) == 0 )
	{
		if( m_nDispTitle == ACHIEVE_DIGIMONRANK  )
		{
			if( net::game )
				net::game->SendTamerAchievement( 0 );

			m_nDispTitle = 0;
			return true;
		}	
	}

	if( ( m_nRankAchieve & Rank_Size ) == 0 )
	{
		if( m_nDispTitle == ACHIEVE_DIGIMONSIZE )
		{
			if( net::game )
				net::game->SendTamerAchievement( 0 );

			m_nDispTitle = 0;
			return true;
		}		
	}

	if( ( ( m_nRankAchieve & Rank_Digimon ) == 0 && ( m_nRankAchieve & Rank_Tamer ) == 0 &&  ( m_nRankAchieve & Rank_Size ) == 0 ) ||		 
		( ( m_nRankAchieve & Rank_Tactics ) == 0 && ( m_nRankAchieve & Rank_Tamer ) == 0 && ( m_nRankAchieve & Rank_Size ) == 0 ) )
	{
		if( m_nDispTitle == ACHIEVE_RANK )
		{
			if( net::game )
				net::game->SendTamerAchievement( 0 );	

			m_nDispTitle = 0;
			return true;
		}
	}

	return false;
}

void AchieveContents::CheckType( eTYPE nType, int nValue1 /*=0*/, int nValue2 /*=0*/ )
{
	m_nQuestCompCount = -1;

	CsAchieve::sINFO* pFTInfo = NULL;
	CsQuestCompRequire* pFTQuestCompReq = NULL;
	std::list< CsAchieve* >::iterator it    = m_listProc.begin();
	std::list< CsAchieve* >::iterator itEnd = m_listProc.end();
	while( it!=itEnd )
	{
		pFTInfo = (*it)->GetInfo();	
		// 1500~1503번은 스킵
		if( pFTInfo->s_nQuestID == ACHIEVE_RANK ||
			pFTInfo->s_nQuestID == ACHIEVE_TAMERRANK ||
			pFTInfo->s_nQuestID == ACHIEVE_DIGIMONRANK ||
			pFTInfo->s_nQuestID == ACHIEVE_DIGIMONSIZE )
		{
			++it;
			continue;
		}

		if( pFTInfo->s_nType != nType )
		{
			++it;
			continue;
		}

#ifdef PATCH_QUESTS_SPINZ
		CsQuest* quest = nsCsFileTable::g_pQuestMng->GetQuest(pFTInfo->s_nQuestID);
		if (!quest) {
			++it;
			continue;
		}
#endif
		pFTQuestCompReq = nsCsFileTable::g_pQuestMng->GetQuest( pFTInfo->s_nQuestID )->GetCompRequireGroup()->Get( 0 );
		switch( nType )
		{
		case AchieveContents::CA_TLevel:
		case AchieveContents::CA_DLevel:
		case AchieveContents::CA_FriendCount:
			if( nValue1 >= pFTQuestCompReq->GetTypeCount() )
			{
				CompAchieve( it ); 
				continue;
			}
			break;

		case AchieveContents::CA_GetTactics:
		case AchieveContents::CA_NpcTalk:
			if( nValue1 == pFTQuestCompReq->GetTypeCount() )
			{
				CompAchieve( it );
				continue;
			}
			break;

		case AchieveContents::CA_Tactics_Evo:
			if( ( nValue1 == pFTQuestCompReq->GetTypeCount() )&&( nValue2 == pFTQuestCompReq->GetTargetID() ) )
			{
				CompAchieve( it );
				continue;
			}
			break;
#ifdef CROSSWARS_SYSTEM
		case AchieveContents::CA_Partner_Evo_CrossWars:
#endif
		case AchieveContents::CA_Partner_Evo_JointProgress:
			if( nValue1 == pFTQuestCompReq->GetTypeCount() )
			{
				CompAchieve( it );
				continue;
			}
			break;
		case AchieveContents::CA_Partner_Evo_Mega:
		case AchieveContents::CA_Partner_Evo_Bust:		
		case AchieveContents::CA_GuildJoin:
		case AchieveContents::CA_RideOpen:		
			CompAchieve( it );
			continue;
		case AchieveContents::CA_InvenSlot_AllOpen:
			if( nValue1 >= nLimit::Inven )
			{
				CompAchieve( it );
				continue;
			}
			break;
		case AchieveContents::CA_WareHouse_AllOpen:
			if( nValue1 >= nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxWareHouse )
			{
				CompAchieve( it );
				continue;
			}
			break;
		case AchieveContents::CA_Money:
			if( g_pDataMng->GetInven()->GetMoney() >= pFTQuestCompReq->GetTypeCount() )
			{
				CompAchieve( it );
				continue;
			}
			break;
		case AchieveContents::CA_Equip_Costume_1:
			switch( nValue1 )
			{
			case 17005:		case 17011:		case 17016:		case 17021:
#ifdef ADD_ACHIEVE_COSTUME
			case 67005:		case 67011:		case 67016:		case 67021:
#endif
				CompAchieve( it );
				continue;
			}
			break;
		case AchieveContents::CA_Equip_Costume_2:
			switch( nValue1 )
			{
			case 17037:		case 17038:		case 17039:		case 17040:		case 17041:
#ifdef ADD_ACHIEVE_COSTUME
			case 67037:		case 67038:		case 67039:		case 67040:
#endif
				CompAchieve( it );
				continue;
			}
			break;
		case AchieveContents::CA_Equip_Costume_3:
			switch( nValue1 )
			{
			case 17006:		case 17012:		case 17017:		case 17022:
#ifdef ADD_ACHIEVE_COSTUME
			case 67006:		case 67012:		case 67017:		case 67022:
#endif
				CompAchieve( it );
				continue;
			}
			break;

		case AchieveContents::CA_Equip_Costume_4:
			switch( nValue1 )
			{
			case 17008:		case 17014:		case 17019:		case 17024:
#ifdef ADD_ACHIEVE_COSTUME
			case 67008:		case 67014:		case 67019:		case 67024:
#endif
				CompAchieve( it );
				continue;
			}
			break;

		case AchieveContents::CA_Equip_Costume_5:
			switch( nValue1 )
			{
			case 17007:		case 17013:		case 17018:		case 17023:
#ifdef ADD_ACHIEVE_COSTUME
			case 67007:		case 67013:		case 67018:		case 67023:
#endif
				CompAchieve( it );
				continue;
			}
			break;

		case AchieveContents::CA_Equip_Costume_6:
			switch( nValue1 )
			{
			case 17009:		case 17015:		case 17020:		case 17025:
#ifdef ADD_ACHIEVE_COSTUME
			case 67009:		case 67015:		case 67020:		case 67025:
#endif
				CompAchieve( it );
				continue;
			}
			break;

		case AchieveContents::CA_Equip_Costume_7:
			switch( nValue1 )
			{
			case 2003:		case 2103:		case 2203:		case 2303:
				CompAchieve( it );
				continue;
			}
			break;

		case AchieveContents::CA_QuestCount:
			if( _CalQuestCompCount() >= pFTQuestCompReq->GetTypeCount() )
			{
				CompAchieve( it );
				continue;
			}
			break;
		case AchieveContents::CA_QuestComp:
			if( nValue1 == pFTQuestCompReq->GetTypeCount() )
			{
				CompAchieve( it );
				continue;
			}
			break;
		case AchieveContents::CA_MAP_1:
			{
				int nArray[ 10 ] = { 1, 100, 101, 102, 103, 104, 110, 111, 112, 113 };
				if( _IsOpenMap( nArray, 10 ) == true )
				{
					CompAchieve( it );
					continue;
				}
			}
			break;
		case AchieveContents::CA_MAP_2:
			{
				int nArray[ 12 ] = { 1100, 1101, 1102, 1103, 1104, 1105, 1106, 1107, 1108, 1109, 1201, 1202 };
				if( _IsOpenMap( nArray, 12 ) == true )
				{
					CompAchieve( it );
					continue;
				}
			}
			break;
		case AchieveContents::CA_MAP_3:
			{
				int nArray[ 7 ] = { 1100, 1101, 1102, 1103, 1104, 1105, 1106 };
				if( _IsOpenMap( nArray, 7 ) == true )
				{
					CompAchieve( it );
					continue;
				}
			}
			break;
		case AchieveContents::CA_MAP_4:
			{
				int nArray[ 5 ] = { 1107, 1108, 1109, 1201, 1202 };
				if( _IsOpenMap( nArray, 5 ) == true )
				{
					CompAchieve( it );
					continue;
				}
			}
			break;
		case AchieveContents::CA_MAP_5:
			{
				int nArray[ 5 ] = { 2100, 2101, 2102, 2103, 2104 };
				if( _IsOpenMap( nArray, 5 ) == true )
				{
					CompAchieve( it );
					continue;
				}
			}
			break;
		case AchieveContents::CA_MAP_6:
			{
				int nArray[ 3 ] = { 11, 12, 13 };
				if( _IsOpenMap( nArray, 3 ) == true )
				{
					CompAchieve( it );
					continue;
				}
			}
			break;
		case AchieveContents::CA_MAP_ALL:
			{
				if( _IsOpenMap_All() == true )
				{
					CompAchieve( it );
					continue;
				}
			}
			break;	

		case AchieveContents::CA_ItemUse:
			if( pFTQuestCompReq->GetTypeCount() == nValue1 )
			{
				CompAchieve( it );
				continue;
			}
			break;
		case AchieveContents::CA_Masters_Match:
			if( nValue1 >= pFTQuestCompReq->GetTypeCount() )
			{
				CompAchieve( it ); 
				continue;
			}
			break;
		default:
			assert_cs( false );
			break;
		}

		++it;
	}
}

void AchieveContents::CompAchieve( std::list< CsAchieve* >::iterator& it )
{
	CsAchieve::sINFO* pFTInfo = (*it)->GetInfo();

	if( net::game )
	{
		net::game->SendQuestAchievement( pFTInfo->s_nQuestID );
	}	

	g_pDataMng->GetQuest()->GetInfo()->CompleteAchievement( pFTInfo->s_nQuestID );
	m_mapComp[ pFTInfo->s_nQuestID ] = (*it);
	it = m_listProc.erase( it );

	DisplayInfo( pFTInfo );
}

bool AchieveContents::_IsOpenSlot( int nBaseDigimonID, int nSlot, int nLevel, cEvoUnit* pEvoUnit )
{
	CDigimonEvolveObj* pEvoObj = nsCsFileTable::g_pEvolMng->GetEvolveInfo( nBaseDigimonID )->GetEvolveObjByEvoSlot( nSlot );
	if( pEvoObj == NULL )
		return false;

	if( pEvoObj->m_nEnableSlot == 0 )
		return false;

	if( pEvoObj->m_nOpenItemTypeS != 0 ) 
	{
		if( ( pEvoUnit[ nSlot ].m_nSlotState & 1 ) == 0 )
			return false;
	}

	// 레벨 플레그 설정
	if( nLevel < pEvoObj->m_nOpenLevel )
		return false;

	// 퀘스트 완료 조건
	if( pEvoObj->m_nOpenQuest != 0 ) //진화에 퀘스트 조건이 있으면
	{
		if( g_pDataMng->GetQuest()->IsCompleate( pEvoObj->m_nOpenQuest ) == false )
			return false;
	}

	return true;
}

bool AchieveContents::_IsOpenMap( int* pArray, int nArrayCnt )
{
	for( int i=0; i<nArrayCnt; ++i )
	{
		if( g_pDataMng->GetMapRegion()->IsOpenedMap( pArray[ i ] ) == false )
			return false;
	}
	return true;
}

bool AchieveContents::_IsOpenMap_All()
{
	CsMapList::MAP* pMap = nsCsMapTable::g_pMapListMng->GetListMap();
	CsMapList::MAP_IT it = pMap->begin();
	CsMapList::MAP_IT itEnd = pMap->end();

	for( ; it!=itEnd; ++it )
	{
		if( g_pDataMng->GetMapRegion()->IsOpenedMap( it->second->GetInfo()->s_dwMapID ) == false )
			return false;
	}
	return true;
}

int AchieveContents::_CalQuestCompCount()
{
	if( m_nQuestCompCount != -1 )
		return m_nQuestCompCount;

	m_nQuestCompCount = 0;
	nQuest::cInfo* pInfo = g_pDataMng->GetQuest()->GetInfo();
	for( int i=1; i<(nLimit::Quest*nLimit::MaxQuestSetCount); ++i )
	{
		if( nsCsFileTable::g_pQuestMng->IsQuest( i ) == false )
			continue;

		if( pInfo && pInfo->CheckAchieve( i ) == nQuest::Complete )
		{			
			if( nsCsFileTable::g_pQuestMng->GetQuest( i )->GetQuestType() != CsQuest::QT_ACHIEVE )
				++m_nQuestCompCount;
		}
	}

	// 한지호는 미리 퀘스트가 완료 되어 있는 상태임
	if( g_pCharMng->GetTamerUser()->GetFTID() == 80004 )
	{
		m_nQuestCompCount = CsMax( 0, m_nQuestCompCount - 39 );
	}

	return m_nQuestCompCount;
}

void AchieveContents::FirstPostLoad()
{
	m_mapComp.clear();
	m_listProc.clear();

	nQuest::cInfo* pQuestInfo = g_pDataMng->GetQuest()->GetInfo();
	 
	CsAchieve::MAP* pMap = nsCsFileTable::g_pAchieveMng->GetMap();

	CsAchieve::MAP_IT it = pMap->begin();
	CsAchieve::MAP_IT itEnd = pMap->end();
	uint res = 0;

	for( ; it!=itEnd; ++it )
	{
		res = pQuestInfo->CheckAchieve( it->first );

		switch( it->second->GetInfo()->s_nQuestID )
		{
		case ACHIEVE_RANK:			// 랭크 업적							
		case ACHIEVE_TAMERRANK:		// 테이머랭크 업적						
		case ACHIEVE_DIGIMONRANK:	// 디지몬랭크 업적	
		case ACHIEVE_DIGIMONSIZE:	// 디지몬크기 업적	
			m_listProc.push_back( it->second );
			break;
		default:
			{
				switch( res )
				{
				case nQuest::Complete:
					m_mapComp[ it->first ] = it->second;
					break;
				case nQuest::None:
					m_listProc.push_back( it->second );
					break;
				default:
					assert_cs( false );
				}
			}
			break;
		}		
	}	

	AchieveRank();
}

void AchieveContents::CheckAllAchieve()
{
	m_seqCheckAll.Start();

	// 초기 정보 받자
	g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::TACTICSHOUSE_LIST, 0 );

	MSG msg;
	while( 1 )
	{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			if(msg.message == WM_QUIT)
			{
				//SetNextGameStep( GAME_EXIT );
				return;
			}

			if (!TranslateAccelerator(GAMEAPP_ST.GetHWnd(), NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			net::execute();
			if( g_pDataMng->GetServerSync()->IsEmptyRefCount( cData_ServerSync::TACTICSHOUSE_LIST ) )
				break;
			if( g_bActive == false )
				Sleep(9);
		}
	}

	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
	cData_Tactics* pDataTactics = g_pDataMng->GetTactics();
	//cData_TacticsHouse* pDataTHouse = g_pDataMng->GetTacticsHouse();
	int nMaxCnt = 0;
	int nLevel = 0;
	cEvoUnit* pEvoUnit = NULL;
	DWORD nDigimonID;

	//=================================================================================================================
	//
	// 100
	//
	//=================================================================================================================
	CheckType( CA_TLevel, pTamer->GetBaseStat()->GetLevel() );
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::CHECK_ACHIEVE_TYPE, null );

	cCommunityContent::sGUILD_INFO guildInfo;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDINFO, &guildInfo );	
	
	if( guildInfo.s_szGuildName.size() > 0 )
		CheckType( CA_GuildJoin );
	CheckType( CA_InvenSlot_AllOpen, g_pDataMng->GetInven()->GetInvenSlotCount() );


	int iSlotCount = 0;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::WAREHOUSE_GET_SLOTCOUNT, &iSlotCount );
	CheckType( CA_WareHouse_AllOpen, iSlotCount/*g_pDataMng->GetWarehouse()->GetSlotCount()*/ );
	CheckType( CA_Money );

	//=================================================================================================================
	//
	// 200
	//
	//=================================================================================================================
	pEvoUnit = pDigimon->GetAttributeEvoUnit();
	nDigimonID = pDigimon->GetBaseDigimonFTID();
	for( int e=1; e<nLimit::EvoUnit; ++e )
	{
		if( nsCsFileTable::g_pEvolMng->GetEvolveInfo( nDigimonID )->GetEvolveObjByEvoSlot( e ) == NULL )
			continue;
		if( ( pEvoUnit[ e ].m_nSlotState & 1 )&&( pEvoUnit[ e ].m_nSlotState & 8 ) )
		{
			CheckType( CA_RideOpen );
			goto END_CHECK_CA_RideOpen;
		}
	}

	nMaxCnt = pDataTactics->GetTacticsCount();
	for( int i=0; i<nMaxCnt; ++i )
	{
		if( pDataTactics->GetTactics( i )->s_dwBaseDigimonID == 0 )
			continue;

		pEvoUnit = pDataTactics->GetTactics( i )->s_EvoUnit;
		nDigimonID = pDataTactics->GetTactics( i )->s_dwBaseDigimonID;
		for( int e=1; e<nLimit::EvoUnit; ++e )
		{
			if( nsCsFileTable::g_pEvolMng->GetEvolveInfo( nDigimonID )->GetEvolveObjByEvoSlot( e ) == NULL )
				continue;
			if( ( pEvoUnit[ e ].m_nSlotState & 1 )&&( pEvoUnit[ e ].m_nSlotState & 8 ) )
			{
				CheckType( CA_RideOpen );
				goto END_CHECK_CA_RideOpen;
			}
		}
	}

END_CHECK_CA_RideOpen:

	// 가지고 있는 용병 체크
	CheckType( CA_GetTactics, pDigimon->GetBaseDigimonFTID() );
	nMaxCnt = pDataTactics->GetTacticsCount();
	for( int i=0; i<nMaxCnt; ++i )
	{
		if( pDataTactics->GetTactics( i )->s_dwBaseDigimonID != 0 )
			CheckType( CA_GetTactics, pDataTactics->GetTactics( i )->s_dwBaseDigimonID );
	}

	
	nMaxCnt = m_AdaptDigimonAchive->GetOpenedArchiveCnt();
	for( int i=0; i<nMaxCnt; ++i )
	{
		DWORD BaseDigimonID = m_AdaptDigimonAchive->GetDigimonBaseIDInArchive(i);
		if( BaseDigimonID != 0 )
			CheckType( CA_GetTactics, BaseDigimonID );
	}

	nDigimonID = pDigimon->GetBaseDigimonFTID();
	pEvoUnit = pDigimon->GetAttributeEvoUnit();
	nLevel = pDigimon->GetBaseStat()->GetLevel();
	CheckType( CA_DLevel, nLevel );
	for( int e=1; e<nLimit::EvoUnit; ++e )
	{
		if( _IsOpenSlot( nDigimonID, e, nLevel, pEvoUnit ) == false )
			continue;

		if( nsCsFileTable::g_pDigimonMng->IsStartDigimon( nDigimonID ) )
		{
			CDigimonEvolveObj* pEvoObj = nsCsFileTable::g_pEvolMng->GetEvolveInfo( nDigimonID )->GetEvolveObjByEvoSlot( e );
			int nEvoType = nsCsFileTable::g_pDigimonMng->GetDigimon( pEvoObj->m_dwID )->GetInfo()->s_eEvolutionType;
			if( nEvoType == nsCsDigimonTable::ET_MEGA )
				CheckType( AchieveContents::CA_Partner_Evo_Mega );
			else if( nEvoType == nsCsDigimonTable::ET_BUSTERMODE )
				CheckType( AchieveContents::CA_Partner_Evo_Bust );
			//else if( nEvoType == nsCsDigimonTable::ET_JOINTPROGRESSMODE )
			//{
				// 조그레스 업적
			//	CheckType( AchieveContents::CA_Partner_Evo_JointProgress );
			//}
		}
		else
		{
			CheckType( CA_Tactics_Evo, nDigimonID, e );
		}
	}

	nMaxCnt = pDataTactics->GetTacticsCount();
	for( int i=0; i<nMaxCnt; ++i )
	{
		if( pDataTactics->GetTactics( i )->s_dwBaseDigimonID == 0 )
			continue;

		nDigimonID = pDataTactics->GetTactics( i )->s_dwBaseDigimonID;
		pEvoUnit = pDataTactics->GetTactics( i )->s_EvoUnit;
		nLevel = pDataTactics->GetTactics( i )->s_nLevel;
		CheckType( CA_DLevel, nLevel );
		for( int e=1; e<nLimit::EvoUnit; ++e )
		{
			if( _IsOpenSlot( nDigimonID, e, nLevel, pEvoUnit ) == false )
				continue;

			if( nsCsFileTable::g_pDigimonMng->IsStartDigimon( nDigimonID ) )
			{
				CDigimonEvolveObj* pEvoObj = nsCsFileTable::g_pEvolMng->GetEvolveInfo( nDigimonID )->GetEvolveObjByEvoSlot( e );
				int nEvoType = nsCsFileTable::g_pDigimonMng->GetDigimon( pEvoObj->m_dwID )->GetInfo()->s_eEvolutionType;
				if( nEvoType == nsCsDigimonTable::ET_MEGA )
					CheckType( AchieveContents::CA_Partner_Evo_Mega );
				else if( nEvoType == nsCsDigimonTable::ET_BUSTERMODE )
					CheckType( AchieveContents::CA_Partner_Evo_Bust );
				/*
				else if( nEvoType == nsCsDigimonTable::ET_JOINTPROGRESSMODE )
				{
					// 조그레스 업적
					CheckType( AchieveContents::CA_Partner_Evo_JointProgress );
				}
				*/
			}
			else
			{
				CheckType( CA_Tactics_Evo, nDigimonID, e );
			}
		}
	}

	//=================================================================================================================
	//
	// 300
	//
	//=================================================================================================================
	CheckType( AchieveContents::CA_Equip_Costume_1, g_pDataMng->GetTEquip()->GetData( nTamer::Costume )->m_nType );
	CheckType( AchieveContents::CA_Equip_Costume_2, g_pDataMng->GetTEquip()->GetData( nTamer::Costume )->m_nType );
	CheckType( AchieveContents::CA_Equip_Costume_3, g_pDataMng->GetTEquip()->GetData( nTamer::Costume )->m_nType );
	CheckType( AchieveContents::CA_Equip_Costume_4, g_pDataMng->GetTEquip()->GetData( nTamer::Costume )->m_nType );
	CheckType( AchieveContents::CA_Equip_Costume_5, g_pDataMng->GetTEquip()->GetData( nTamer::Costume )->m_nType );
	CheckType( AchieveContents::CA_Equip_Costume_6, g_pDataMng->GetTEquip()->GetData( nTamer::Costume )->m_nType );
	CheckType( AchieveContents::CA_Equip_Costume_7, g_pDataMng->GetTEquip()->GetData( nTamer::Costume )->m_nType );

	//=================================================================================================================
	//
	// 400
	//
	//=================================================================================================================
	CheckType( AchieveContents::CA_MAP_1 );
	CheckType( AchieveContents::CA_MAP_2 );
	CheckType( AchieveContents::CA_MAP_3 );
	CheckType( AchieveContents::CA_MAP_4 );
	CheckType( AchieveContents::CA_MAP_5 );
	CheckType( AchieveContents::CA_MAP_6 );
	CheckType( AchieveContents::CA_MAP_ALL );
	
	//마스터즈매칭 업적 확인
	CheckType( AchieveContents::CA_Masters_Match, g_pGameIF->m_sMyInfo.s_nMyCnt );
	
	//=================================================================================================================
	//
	// etc
	//
	//=================================================================================================================
	CheckType( AchieveContents::CA_QuestCount );
	nQuest::cInfo* pInfo = g_pDataMng->GetQuest()->GetInfo();
	for( int i=1; i<(nLimit::Quest*nLimit::MaxQuestSetCount); ++i )
	{
		if( nsCsFileTable::g_pQuestMng->IsQuest( i ) == false )
			continue;

		// 완료 중인지만 체크
		if( pInfo->CheckAchieve( i ) == nQuest::Complete )
		{
			CheckType( AchieveContents::CA_QuestComp, i );
		}			
	}
	
	// CA_NpcTalk => 체크 안됨
}

bool AchieveContents::IsEnableCheckAll()
{ 
	return m_seqCheckAll.IsCoolTime() == false; 
}

DWORD AchieveContents::GetDispTitle()
{ 
	return m_nDispTitle; 
}

std::map< DWORD, CsAchieve* >const*	AchieveContents::GetCompMap() const
{ 
	return &m_mapComp; 
}	

std::list< CsAchieve* >const* AchieveContents::GetProcList() const
{ 
	return &m_listProc; 
}
