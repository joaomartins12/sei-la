

#include "stdafx.h"
#include "Data_QuestOwner.h"


//=======================================================================================================
//
//	sNpcOwner
//
//=======================================================================================================
void cData_QuestOwner::sNpcOwner::Delete()
{
	std::map< DWORD, sQuestInfo* >::iterator it = s_mapQuest.begin();
	std::map< DWORD, sQuestInfo* >::iterator itEnd = s_mapQuest.end();
	for( ; it!=itEnd; ++it )
	{
		NiDelete it->second;
	}
	s_mapQuest.clear();
}

void cData_QuestOwner::sNpcOwner::Init()
{
	s_eDisplayRevType = _DISPLAY_NONE;
}

void cData_QuestOwner::sNpcOwner::Add_New( nQuest::cInfo* pOrgInfo, CsQuest* pFT )
{
	sQuestInfo* pInfo = NiNew sQuestInfo;
	pInfo->s_eType = _CheckPreRequire( pOrgInfo, pFT );
	pInfo->s_pFTQuest = pFT;
	pInfo->s_pProcess = NULL;

	// 현재 디스플레이 보다 우선시 된다면
	if( pInfo->s_eType < s_eDisplayRevType )
	{
		s_eDisplayRevType = pInfo->s_eType;
	}

	assert_cs( s_mapQuest.find( pFT->GetUniqID() ) == s_mapQuest.end() );
	s_mapQuest[ pFT->GetUniqID() ] = pInfo;
}

void cData_QuestOwner::sNpcOwner::Add_Process( nQuest::cInfo* pOrgInfo, CsQuest* pFT, cData_Quest::sPROCESS* pProcess )
{
	sQuestInfo* pInfo = NiNew sQuestInfo;
	pInfo->s_eType = pProcess->s_bCompleate == true ? REQUITE : PROCESS;
	pInfo->s_pFTQuest = pFT;
	pInfo->s_pProcess = pProcess;

	// 현재 디스플레이 보다 우선시 된다면
	if( pInfo->s_eType < s_eDisplayRevType )
	{
		s_eDisplayRevType = pInfo->s_eType;
	}

	assert_cs( s_mapQuest.find( pFT->GetUniqID() ) == s_mapQuest.end() );
	s_mapQuest[ pFT->GetUniqID() ] = pInfo;	
}

void cData_QuestOwner::sNpcOwner::DeleteQuest( uint nQuestFTID )
{
	// 찾는다.
	bool bFind = false;
	std::map< DWORD, sQuestInfo* >::iterator it = s_mapQuest.begin();
	std::map< DWORD, sQuestInfo* >::iterator itEnd = s_mapQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( it->second->s_pFTQuest->GetUniqID() == nQuestFTID )
		{
			bFind = true;
			// 데이터 제거
			NiDelete it->second;
			// 리스트에서 제거
			s_mapQuest.erase( it );
			break;
		}
	}
	assert_cs( bFind == true );
	CalDisplayRev();
}

void cData_QuestOwner::sNpcOwner::CalDisplayRev()
{
	// 디스플레이를 초기위치로 놓고
	// 우선순위 디스플레이를 찾는다
	s_eDisplayRevType = _DISPLAY_NONE;
	std::map< DWORD, sQuestInfo* >::iterator it = s_mapQuest.begin();
	std::map< DWORD, sQuestInfo* >::iterator itEnd = s_mapQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( it->second->s_pProcess != NULL )
		{
			it->second->s_eType = it->second->s_pProcess->s_bCompleate == true ? REQUITE : PROCESS;
		}

		// 현재 디스플레이 보다 우선시 된다면
		if( it->second->s_eType < s_eDisplayRevType )
		{
			s_eDisplayRevType = it->second->s_eType;
		}
	}
}

bool cData_QuestOwner::sNpcOwner::CheckRevType( nQuest::cInfo* pOrgInfo )
{
	bool bReturn = false;
	std::map< DWORD, sQuestInfo* >::iterator it = s_mapQuest.begin();
	std::map< DWORD, sQuestInfo* >::iterator itEnd = s_mapQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( pOrgInfo->Check( it->second->s_pFTQuest->GetUniqID() ) == nQuest::Execute )
			continue;

		eREV_TYPE eNewType = _CheckPreRequire( pOrgInfo, it->second->s_pFTQuest );		
		if( it->second->s_eType != eNewType )
		{
			it->second->s_eType = eNewType;
			bReturn = true;
		}
	}
	return bReturn;
}

bool cData_QuestOwner::sNpcOwner::IsHaveType( DWORD nRevPlag )
{
	std::map< DWORD, sQuestInfo* >::iterator it = s_mapQuest.begin();
	std::map< DWORD, sQuestInfo* >::iterator itEnd = s_mapQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( ( it->second->s_eType & nRevPlag ) != 0 )
			return true;
	}
	return false;
}

// 선행조건 체크
cData_QuestOwner::sNpcOwner::eREV_TYPE
cData_QuestOwner::sNpcOwner::_CheckPreRequire( nQuest::cInfo* pOrgInfo, CsQuest* pFT )
{
	// 테이머 레벨
	int nTamerLevel = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();

	// 디지몬 레벨
	int nDigimonLevel = g_pCharMng->GetDigimonUser( 0 )->GetBaseStat()->GetLevel();

	//-------------------------------------------//
	//#6350 디지몬 조건이 걸려있는 퀘스트가 특정 조건에서 발생이 되지 않는 오류 와 연관 있음 20170621
	//DWORD dwDigimonType = g_pCharMng->GetDigimonUser( 0 )->GetModelID();
	DWORD dwDigimonType = g_pCharMng->GetDigimonUser( 0 )->GetCurrentDigimonType();
	//-------------------------------------------//

	// 곧 받을수 있는 퀘인가 ( 레벨 )
	bool bSoonRev_Level = false;

	CsQuestPreRequireGroup* pPreRequireGroup = pFT->GetPreRequireGroup();
	int nPreREquireCount = pPreRequireGroup->GetListCount();
	CsQuestPreRequire* pPreRequire = NULL;
	for( int i=0; i<nPreREquireCount; ++i )
	{
		pPreRequire = pPreRequireGroup->Get( i );
		switch( pPreRequire->GetType() )
		{
			// 퀘스트 완료 했는가 - 완료 못한 퀘스트가 있다면 바로 리턴
		case CsQuestPreRequire::QUEST:
			{
				if( pOrgInfo->Check( pPreRequire->GetTypeID() ) != nQuest::Complete )
				{
					return DISABLE_REV;
				}
			}
			break;

			// 퀘스트 레벨 제한이 걸리는가
		case CsQuestPreRequire::T_LEVEL:
			{
				if( nTamerLevel < (int)pPreRequire->GetTypeID() )
				{
					if( nTamerLevel + 5 < (int)pPreRequire->GetTypeID() )
					{
						return DISABLE_REV;
					}
					bSoonRev_Level = true;
				}
			}
			break;
			// 선행 아이템이 없는가
		case CsQuestPreRequire::ITEM:
			{
				int nReqItemCount = g_pDataMng->GetInven()->GetCount_Item_ID( pPreRequire->GetTypeID() );
				nReqItemCount += g_pDataMng->GetTEquip()->GetCount_Item_ID( pPreRequire->GetTypeID() );
				if( nReqItemCount < pPreRequire->GetTypeCount() )
				{
					return DISABLE_REV;
				}
			}
			break;
		case CsQuestPreRequire::D_LEVEL:
			{
				if( nDigimonLevel < (int)pPreRequire->GetTypeID() )
				{
					if( nDigimonLevel + 5 < (int)pPreRequire->GetTypeID() )
					{
						return DISABLE_REV;
					}
					bSoonRev_Level = true;
				}
			}
			break;
		case CsQuestPreRequire::D_TYPE:
			{
				if( dwDigimonType != pPreRequire->GetTypeID() )
				{
					return DISABLE_REV;
				}
			}
			break;
		case CsQuestPreRequire::REGION:			
			assert_csm1( false, _T( "NPC가 주는 퀘는 선행 조건에 Region이 들어가면 안된다. QuestID = %d" ), pFT->GetUniqID() );
			break;
		case CsQuestPreRequire::D_EVO:			
			{
				CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
				SAFE_POINTER_RETVAL( pDigimon, DISABLE_REV );

				if( !pDigimon->IsOpenedEvoSlot( pPreRequire->GetTypeID() ) )
					return DISABLE_REV;
			}
			break;
		case CsQuestPreRequire::D_EVO_DISABLE: // 특정 슬롯이 활성화 되어 있으면 받을 수 없는 퀘스트 체크
			{
				CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
				SAFE_POINTER_RETVAL( pDigimon, DISABLE_REV );

				if( pDigimon->IsOpenedEvoSlot( pPreRequire->GetTypeID() ) )
					return DISABLE_REV;
			}
			break;
		case CsQuestPreRequire::D_BASE_TABLE_IDX: // 디지몬의 기본체 ID 조건
			{
				CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
				SAFE_POINTER_RETVAL( pDigimon, DISABLE_REV );

				if( pPreRequire->GetTypeID() != pDigimon->GetBaseDigimonFTID() )
					return DISABLE_REV;
			}
			break;
		case CsQuestPreRequire::TAMER_TABLE_IDX: // 테이머 테이블 ID 조건
			{
				CTamerUser* pTamer = g_pCharMng->GetTamerUser();
				SAFE_POINTER_RETVAL( pTamer, DISABLE_REV );

				if( pPreRequire->GetTypeID() != pTamer->GetFTID() )
					return DISABLE_REV;
			}
			break;
		case CsQuestPreRequire::QUEST_ID_CANNOT_COMPLETED: // 완료된 퀘스트가 있을시 진행 불가
			{
				if( pOrgInfo->Check( pPreRequire->GetTypeID() ) == nQuest::Complete )
					return DISABLE_REV;
			}
			break;
		case CsQuestPreRequire::QUEST_ID_CANNOT_PROGRESS: // 진행 중인 퀘스트가 있을시 진행 불가
			{
				if( pOrgInfo->Check( pPreRequire->GetTypeID() ) == nQuest::Execute )
					return DISABLE_REV;
			}
			break;
		default:
			assert_cs( false );
		}
	}

	if( bSoonRev_Level == true )
		return SOON_REV_LEVEL;

	switch( pFT->GetQuestType() )
	{
	case CsQuest::QT_MAIN:			return ENABLE_REV_MAIN;
	case CsQuest::QT_SUB:			return ENABLE_REV_SUB;
//	case CsQuest::QT_TIME:
	case CsQuest::QT_EVENTREPEAT:	
		return ENABLE_REV_EVENTREPEAT;
	case CsQuest::QT_DAILY:
	case CsQuest::QT_REPEAT:
	case CsQuest::QT_JOINTPROGRESS:
#ifdef SDM_TAMER_XGUAGE_20180628
	case CsQuest::QT_XANTI_JOINTPROGRESS:
#endif
		return ENABLE_REV_REPEAT;
	case CsQuest::QT_TUTORIAL:			return ENABLE_REV_MAIN;
	}	
	return _DISPLAY_NONE;	
}

void cData_QuestOwner::sNpcOwner::GetQuestInfoList( DWORD nRevPlag, std::list< sQuestInfo* >* pList )
{
	assert_cs( pList->size() == 0 );

	std::map< DWORD, sQuestInfo* >::iterator it = s_mapQuest.begin();
	std::map< DWORD, sQuestInfo* >::iterator itEnd = s_mapQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( ( it->second->s_eType & nRevPlag ) != 0 )
		{
			pList->push_back( it->second );
		}
	}
}