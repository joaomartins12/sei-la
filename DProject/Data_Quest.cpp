
#include "stdafx.h"
#include "Data_Quest.h"

//=======================================================================================================
//
//	전역
//
//=======================================================================================================
bool cData_Quest::CheckPreRequire( CsQuest* pFT )
{
	CsQuestPreRequireGroup* pGroup = pFT->GetPreRequireGroup(); //선행 조건을 얻어서
	int nPRCount = pGroup->GetListCount(); // 조건들의 수

	CsQuestPreRequire* pPR = NULL;	
	for( int pr=0; pr<nPRCount; ++pr ) // 조건의 수만큼 돌리자
	{
		pPR = pGroup->Get( pr );
		switch( pPR->GetType() )
		{
		case CsQuestPreRequire::QUEST:	// 조건이 퀘스트면
			if( g_pDataMng->GetQuest()->IsCompleate( pPR->GetTypeID() ) == false )	
				return false;
			break;
		case CsQuestPreRequire::ITEM:	// 조건이 아이템
			{
				int nReqItemCount = g_pDataMng->GetInven()->GetCount_Item_ID( pPR->GetTypeID() );
				nReqItemCount += g_pDataMng->GetTEquip()->GetCount_Item_ID( pPR->GetTypeID() );
				if( nReqItemCount < pPR->GetTypeCount() )
					return false;
			}
			break;
		case CsQuestPreRequire::T_LEVEL:	// 조건이 테이머 레벨
			if( g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel() < (int)pPR->GetTypeID() )
				return false;
			break;
		case CsQuestPreRequire::D_LEVEL:	// 조건이 디지몬 레벨
			if( g_pCharMng->GetDigimonUser( 0 )->GetBaseStat()->GetLevel() < (int)pPR->GetTypeID() )
				return false;
			break;
		case CsQuestPreRequire::D_TYPE:	// 조건이 디지몬 타입
			{
				DWORD dwDigimonType = g_pCharMng->GetDigimonUser( 0 )->GetCurrentDigimonType();
				if( dwDigimonType != pPR->GetTypeID() )
					return false;
			}
			break;
		case CsQuestPreRequire::REGION:	// 조건이 영역돌입
			{
				int nMapRegion = nsCsMapTable::g_pMapListMng->MapIDToMapRegionID( pPR->GetTypeID() / 1000 );
				int nRegion = pPR->GetTypeID() % 1000;				
				if( g_pDataMng->GetMapRegion()->GetMapRegion()->IsOpened( nMapRegion, nRegion ) == false )
					return false;
			}
			break;
		case CsQuestPreRequire::D_EVO:	// 조건이 테이머 레벨
			{				
				CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
				SAFE_POINTER_RETVAL( pDigimon, false );

				if( !pDigimon->IsOpenedEvoSlot( pPR->GetTypeID() ) )
					return false;
			}
			break;

		case CsQuestPreRequire::D_EVO_DISABLE: // 특정 슬롯이 활성화 되어 있으면 받을 수 없는 퀘스트 체크
			{
				CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
				SAFE_POINTER_RETVAL( pDigimon, false );

				if( pDigimon->IsOpenedEvoSlot( pPR->GetTypeID() ) )
					return false;
			}
			break;
		case CsQuestPreRequire::D_BASE_TABLE_IDX: // 디지몬의 기본체 ID 조건
			{
				CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
				SAFE_POINTER_RETVAL( pDigimon, false );

				if( pPR->GetTypeID() != pDigimon->GetBaseDigimonFTID() )
					return false;
			}
			break;
		case CsQuestPreRequire::TAMER_TABLE_IDX: // 테이머 테이블 ID 조건
			{
				CTamerUser* pTamer = g_pCharMng->GetTamerUser();
				SAFE_POINTER_RETVAL( pTamer, false );

				if( pPR->GetTypeID() != pTamer->GetFTID() )
					return false;
			}
			break;
		case CsQuestPreRequire::QUEST_ID_CANNOT_COMPLETED: // 완료된 퀘스트가 있을시 진행 불가
			{
				if( g_pDataMng->GetQuest()->IsCompleate( pPR->GetTypeID() ) )
					return false;
			}
			break;
		case CsQuestPreRequire::QUEST_ID_CANNOT_PROGRESS: // 진행 중인 퀘스트가 있을시 진행 불가
			{
				if( g_pDataMng->GetQuest()->IsProcess( pPR->GetTypeID() ) )
					return false;
			}
			break;
		default:
			assert_cs( false );
		}
	}
	return true;
}




bool cData_Quest::CALLBACK_QuestCheck( int nQuest )
{
	// 수행중 체크 할필요 없이 완료 인지만 체크 하기 때문에
	return g_pDataMng->GetQuest()->IsCompleate( nQuest );
}


//===========================================================================
//
//		cData_Quest
//
//===========================================================================
CSGBMEMPOOL_CPP( cData_Quest::sPROCESS );

void cData_Quest::GlobalInit()
{
	CSGBMEMPOOL_INIT( cData_Quest::sPROCESS, 8 );

	sUDD_BASE::ResistCallBack_QuestCheck( CALLBACK_QuestCheck );
}

void cData_Quest::GlobalShotDown()
{
	CSGBMEMPOOL_DELETE( cData_Quest::sPROCESS );

	sUDD_BASE::ResistCallBack_QuestCheck( NULL );
}


void cData_Quest::Delete()
{
	_DeleteProcQuest();
}



void cData_Quest::InitNetOff()
{
	m_Info.Clear();

	cProfile x("./dmo.ini");

	char key[ 32 ];
	int i = 0;
	int nID;
	while( true )
	{
		sprintf_s( key, 32, "%d", i );
		nID = x.GetInt( "QUEST_COMP", key );
		if( nID == 0 )
			break;
		if( nsCsFileTable::g_pQuestMng->IsQuest( nID ) )
		{
			m_Info.Begin( nID );
			m_Info.Complete( nID );
		}		
		++i;
	}

	i = 0;	
	while( true )
	{
		sprintf_s( key, 32, "%d", i );
		nID = x.GetInt( "QUEST_PROC", key );
		if( nID == 0 )
			break;
		if( nsCsFileTable::g_pQuestMng->IsQuest( nID ) )
		{
			m_Info.Begin( nID );
		}		
		++i;
	}	
}


//===========================================================================
//
//		맵 새로 로드시
//
//===========================================================================

void cData_Quest::PostRevPostLoad( DWORD dwMapID )
{
	//====================================================================================
	// 퀘스트 강제 취소
	//====================================================================================
	QuestReLoad();
}

void cData_Quest::PostRevAddQuest( CsQuest* pQuest )
{
	// 이미 완료한 퀘스트 인지 체크

	switch( m_Info.Check( pQuest->GetUniqID() ) )
	{
		// 이미 완료 했다면 패스
	case nQuest::Complete:
		return;

		// 수행중
	case nQuest::Execute:
		{
			// 프로세스 생성
			sPROCESS* pProcess = InsertProcess( pQuest );
			// 맵에 기입
			m_mapProcQuest[ pQuest->GetUniqID() ] = pProcess;
			
			// Owner쪽에 입력
			g_pDataMng->GetQuestOwner()->AddQuest_Process( GetInfo(), pQuest, pProcess );
		}
		break;

		// 아직 안받음
	case nQuest::None:
		// Owner쪽에 입력
		g_pDataMng->GetQuestOwner()->AddQuest_New( GetInfo(), pQuest );
		break;

	default:
		assert_cs( false );
	}
}

int cData_Quest::GetInfoArrayIndex( DWORD dwFTID )
{
	for( int i=0; i<nQuest::MaxExecute; ++i )
	{
		if( m_Info.GetExeInfo( i ).m_nIDX == dwFTID )
			return i;
	}
	assert_cs( false );
	return 0;
}


//===========================================================================
//
//		퀘스트 시작
//
//===========================================================================

void cData_Quest::RevQuestCheck()
{
	g_pDataMng->GetQuestOwner()->RevQuestCheck( &m_Info );

	cData_QuestOwner::sDigiviceOwner::sQuestInfo* pQuestInfo = NULL;

	// [ 디지바이스퀘 ] 체크 단계 => 준비 단계
	{
		std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >* pMap = g_pDataMng->GetQuestOwner()->GetCheckDigiviceOwner()->GetMap();
		std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >::iterator it = pMap->begin();
		std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >::iterator itEnd = pMap->end();
		std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >::iterator itNext = it;
		while( itNext!=itEnd )
		{
			it = itNext;
			++itNext;

			CsQuest* pFTQuest = it->second->s_pFTQuest;
			if( cData_Quest::CheckPreRequire( pFTQuest ) == true )
			{
				NiDelete it->second;
				pMap->erase( it );
				// 체크 단계 => 준비 단계
				g_pDataMng->GetQuestOwner()->GetReadyRevDigiviceOwner()->Add_New( &m_Info, pFTQuest, NULL );
			}
		}
	}	

	// [ 디지바이스퀘 ] 준비 단계 => 체크 단계
	{
		std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >* pMap = g_pDataMng->GetQuestOwner()->GetReadyRevDigiviceOwner()->GetMap();
		std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >::iterator it = pMap->begin();
		std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >::iterator itEnd = pMap->end();
		std::map< DWORD, cData_QuestOwner::sDigiviceOwner::sQuestInfo* >::iterator itNext = it;
		while( itNext!=itEnd )
		{
			it = itNext;
			++itNext;

			CsQuest* pFTQuest = it->second->s_pFTQuest;
			if( cData_Quest::CheckPreRequire( pFTQuest ) == false )
			{
				NiDelete it->second;
				pMap->erase( it );
				// 준비 단계 => 체크 단계
				g_pDataMng->GetQuestOwner()->GetCheckDigiviceOwner()->Add_New( &m_Info, pFTQuest, NULL );
			}
		}
	}
}

bool cData_Quest::RevQuest_ByNpc( CsQuest* pFT, CsC_AvObject* pNpc )
{
	// 수행지급 아이템
	if( _RevQuest_ApplyRequite( pFT ) == false )
	{
		return false;
	}

	// Owner쪽에 제거
	CNpc* pNpcObject = (CNpc*)pNpc;
	pNpcObject->GetQuestOwner()->DeleteQuest( pFT->GetUniqID() );	// NPC에게서 퀘받았으면 말풍선에서 그 목록제거 

	// info 입력 - 프로세스 생성 전에 입력하자 ~!!
	bool bSuccess = m_Info.Begin( pFT->GetUniqID() );
	assert_cs( bSuccess == true );

	// 프로세스 생성
	sPROCESS* pProcess = InsertProcess( pFT );
	// 맵에 기입
	m_mapProcQuest[ pFT->GetUniqID() ] = pProcess;

	// Owner쪽에 입력
	g_pDataMng->GetQuestOwner()->AddQuest_Process( GetInfo(), pFT, pProcess );

	// IF에 연결
	int nArrayIndex = GetInfoArrayIndex( pFT->GetUniqID() );
	g_pResist->m_Character.s_bQuestHelperViewID[ nArrayIndex ] = true;

	if( g_pGameIF->GetQuestList() )
	{		
		g_pGameIF->GetQuestList()->AddQuest( nArrayIndex, pFT->GetUniqID() );
	}
	g_pGameIF->GetQuestHelper()->ResetHelperString();

	// 서버에 동기화
	if( net::game )
		net::game->SendQuestRequire( pFT->GetUniqID() );

	// 메세지	
	cPrintMsg::PrintMsg( 14003, pFT->m_szTitleText );

	g_pDataMng->GetQuestOwner()->Check_DigimonTalk( false );

	// 튜토리얼이라면
	if( pFT->IsQuestType(CsQuest::QT_TUTORIAL) )
	{
		DWORD dwQuestID = pFT->GetUniqID();
		GAME_EVENT_ST.OnEvent( EVENT_CODE::TUTORIALQUEST_ACCEPT, &dwQuestID);
	}
	return true;
}

bool cData_Quest::RevQuest_ByDigivice( CsQuest* pFT )
{
	// 수행지급 아이템
	if( _RevQuest_ApplyRequite( pFT ) == false )
		return false;

	// Owner쪽에 제거
	g_pDataMng->GetQuestOwner()->GetReadyRevDigiviceOwner()->DeleteQuest( pFT );

	// info 입력 - 프로세스 생성 전에 입력하자 ~!!
	bool bSuccess = m_Info.Begin( pFT->GetUniqID() );
	assert_cs( bSuccess == true );

	// 프로세스 생성
	sPROCESS* pProcess = InsertProcess( pFT );

	// 맵에 기입
	m_mapProcQuest[ pFT->GetUniqID() ] = pProcess;

	// Owner쪽에 입력
	g_pDataMng->GetQuestOwner()->AddQuest_Process( GetInfo(), pFT, pProcess );

	// IF에 연결
	int nArrayIndex = GetInfoArrayIndex( pFT->GetUniqID() );
	g_pResist->m_Character.s_bQuestHelperViewID[ nArrayIndex ] = true;

	if( g_pGameIF->GetQuestList() )
	{		
		g_pGameIF->GetQuestList()->AddQuest( nArrayIndex, pFT->GetUniqID() );
	}
	g_pGameIF->GetQuestHelper()->ResetHelperString();

	// 서버에 동기화
	if( net::game )
	{
		net::game->SendQuestRequire( pFT->GetUniqID() );
	}

	// 메세지	
	cPrintMsg::PrintMsg( 14003, pFT->m_szTitleText );

	g_pDataMng->GetQuestOwner()->Check_DigimonTalk( false );

	// 튜토리얼이라면
	if( pFT->IsQuestType(CsQuest::QT_TUTORIAL) )
	{
		DWORD dwQuestID = pFT->GetUniqID();
		GAME_EVENT_ST.OnEvent( EVENT_CODE::TUTORIALQUEST_ACCEPT, &dwQuestID);
	}
	return true;
}

bool cData_Quest::_RevQuest_ApplyRequite( CsQuest* pFT )
{	
	// 수행지급 아이템이 있다면 받자
	CsQuestApplyRequiteGroup* pApplyRequiteGroup = pFT->GetApplyRequiteGroup();
	CsQuestApplyRequite* pApplyRequite;
	int nElementCount = pApplyRequiteGroup->GetListCount();

	// 받아야 하는데 인벤에 공간이 없다..
	if( g_pDataMng->GetInven()->GetEmptySlotCount() < nElementCount )
	{
		cPrintMsg::PrintMsg( cPrintMsg::GAME_INVEN_LIMIT_COUNT );
		return false;
	}

	int nReturnInven;
	for( int i=0; i<nElementCount; ++i )
	{
		pApplyRequite = pApplyRequiteGroup->Get( i );
		switch( pApplyRequite->GetType() )
		{
		case CsQuestApplyRequite::ITEM:
			{
				nReturnInven = g_pDataMng->GetInven()->ItemCrop( pApplyRequite->GetTypeID(), pApplyRequite->GetTypeCount() );
				assert_cs( nReturnInven == 0 );

				// 받았다면 메세지				
				cPrintMsg::PrintMsg( 20019, nsCsFileTable::g_pItemMng->GetItem( pApplyRequite->GetTypeID() )->GetInfo()->s_szName );
			}			
			break;
		default:
			assert_cs( false );
		}
	}

	return true;
}


//===========================================================================
//
//		Process
//
//===========================================================================

cData_Quest::sPROCESS*
cData_Quest::InsertProcess( CsQuest* pFT )
{
	sPROCESS* pProcess = sPROCESS::NewInstance();
	// 파일테이블
	pProcess->s_pFT = pFT;
	// 완료 조건 복사 - 갯수는 0으로 셋팅 된다
	pProcess->s_CurCompRequireGroup.Clone( pProcess->s_pFT->GetCompRequireGroup() );

	_CheckProcessCondition( pProcess );
	
	// 완료 되었는가
	pProcess->s_bCompleate = pProcess->s_CurCompRequireGroup.IsCompleate();
#ifdef TRICK_ORTREAT
/////////////////////////////////////////////////////
	// 수행중 퀘스트 저장하고 그 퀘스트의 선행지급 아이템중 169 아이템이면 카운트 다운.
	CsQuestApplyRequiteGroup* pApplyRequiteGroup = pFT->GetApplyRequiteGroup();
	CsQuestApplyRequite* pApplyRequite;
	int nElementCount = pApplyRequiteGroup->GetListCount();	
	
	for( int i=0; i<nElementCount; ++i )
	{
		pApplyRequite = pApplyRequiteGroup->Get( i );
		switch( pApplyRequite->GetType() )
		{		
		case CsQuestApplyRequite::ITEM:
			{
				CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( pApplyRequite->GetTypeID() )->GetInfo();
				if( 3 == pFTInfo->s_btUseTimeType && (nItem::TimeSet == pFTInfo->s_nType_L) ){

					g_pGameIF->CloseDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_TIME_ITEM, true );
					g_pGameIF->GetDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_TIME_ITEM, cBGSprite::TIME_ITEM );
				}

				
			}
			break;		
		}
	}
#endif

	return pProcess;
}



bool cData_Quest::CalProcess(int nCheckType)
{
	// 진행 하는 퀘스트
	bool bChange = false;
	sPROCESS* pProcess = NULL;
	std::map< DWORD, sPROCESS* >::iterator itProcess = m_mapProcQuest.begin();
	std::map< DWORD, sPROCESS* >::iterator itProcessEnd = m_mapProcQuest.end();
	for( ; itProcess !=itProcessEnd; ++itProcess )
	{
		pProcess = itProcess->second;
		// 리턴값 0 - 변경 없음, 1 - 작게 변경, 2 - 크게 변경
		switch( _CheckProcessCondition( pProcess, nCheckType ) )
		{
		case 0:	break;// 0 - 변경 없음
		case 1:// 1 - 작게 변경
			{
				bChange = true;
				pProcess->s_bCompleate = pProcess->s_CurCompRequireGroup.IsCompleate();
				CNpc* pNpc = (CNpc*)_GetOwner( pProcess->s_pFT );
				if( pNpc != NULL )
					pNpc->GetQuestOwner()->CalDisplayRev();
			}
			break;
		case 2:	// 2 - 크게 변경
			{
				bChange = true;
				// 완료 되었는가
				bool bCompleate = pProcess->s_CurCompRequireGroup.IsCompleate();
				// 완료 형이 변햇는가 ??
				if( pProcess->s_bCompleate != bCompleate )
				{
					// 엔피시 디스플레이 정보 변경
					pProcess->s_bCompleate = bCompleate;
					CNpc* pNpc = (CNpc*)_GetOwner( pProcess->s_pFT );
					if( pNpc != NULL )
						pNpc->GetQuestOwner()->CalDisplayRev();
				}

				_CompReadyQuest( bCompleate, pProcess->s_pFT );						
			}
			break;
		default:	assert_cs( false );	break;
		}		
	}

	// 변경 되었으면
	if( bChange == true )
	{
		// IF에 연결
		g_pGameIF->GetQuestHelper()->ResetHelperString();
		if( g_pGameIF->GetQuestList() )
		{
			g_pGameIF->GetQuestList()->ResetQuestList( true );
		}
	}

	return bChange;
}

// 리턴값 0 - 변경 없음, 1 - 작게 변경, 2 - 크게 변경
int cData_Quest::_CheckProcessCondition( sPROCESS* pProcess, int nCheckType )
{
	if( NULL == pProcess )
		return 0;

	bool bCheckAll = (nCheckType == -1)?true:false;
	int nState = 0;
	nQuest::sExeInfo* pExeInfo = m_Info.GetExeInfo_ByID( (u2)pProcess->s_pFT->GetUniqID() );
	CsQuestCompRequire::LIST* completeList = pProcess->s_CurCompRequireGroup.GetList();
	CsQuestCompRequire::LIST_IT it = completeList->begin();
	for( int n = 0; it != completeList->end(); ++it, ++n )
	{
		if( !bCheckAll )
		{
			if( nCheckType != (*it)->GetType() )
				continue;
		}

		switch( (*it)->GetType() )
		{
		case CsQuestCompRequire::T_LEVEL:	// 테이머 레벨 체크
			{
				if( g_pCharMng->GetMyTamerLv() < (int)(*it)->GetTypeID() )
					(*it)->SetCurCount( 0 );
				else
				{
					if( (*it)->GetCurCount() == 0 )
						nState = 2;

					(*it)->SetCurCount( 1 );					
				}
			}break;
		case CsQuestCompRequire::D_TYPE:	// 디지몬 타입 체크
			{
				DWORD dwDigimonType = g_pCharMng->GetDigimonUser( 0 )->GetCurrentDigimonType();
				if( dwDigimonType != (*it)->GetTypeID() )
				{
					if( (*it)->GetCurCount() == 1 )
						nState = 2;
					(*it)->SetCurCount( 0 );
				}
				else
				{
					if( (*it)->GetCurCount() == 0 )
						nState = 2;
					(*it)->SetCurCount( 1 );					
				}				
			}break;
		case CsQuestCompRequire::MONSTER:
		case CsQuestCompRequire::NPCTALK:
		case CsQuestCompRequire::CLIENT:
		case CsQuestCompRequire::USE_ITEM:
		case CsQuestCompRequire::USE_ITEM_NPC:
		case CsQuestCompRequire::USE_ITEM_MONSTER:
		case CsQuestCompRequire::USE_ITEM_REGION:
		case CsQuestCompRequire::REGION:
			{
				assert_cs( pExeInfo != NULL );
				if( pExeInfo )
				{
					if( (*it)->GetCurCount() != CsMin( (*it)->GetTypeCount(), pExeInfo->m_nCondition[ n ] ) )
					{
						if( (*it)->GetCurCount() < CsMin( (*it)->GetTypeCount(), pExeInfo->m_nCondition[ n ] ) )
							nState = 2;
						else if( nState == 0 )
							nState = 1;

						(*it)->SetCurCount( pExeInfo->m_nCondition[ n ] );
					}
				}				
			}
			break;		
			// 영역 별도 체크
			// 		case CsQuestCompRequire::REGION:
			// 			{
			// 				if( pExeInfo )
			// 				{
			// 					if( (*it)->GetCurCount() != CsMin( (*it)->GetTypeCount(), pExeInfo->m_nCondition[ n ] ) )
			// 					{
			// 						if( (*it)->GetCurCount() < CsMin( (*it)->GetTypeCount(), pExeInfo->m_nCondition[ n ] ) )
			// 							nState = 2;
			// 						else if( nState == 0 )
			// 							nState = 1;
			// 
			// 						(*it)->SetCurCount( pExeInfo->m_nCondition[ n ] );
			// 					}
			// 					else if( pExeInfo->m_nCondition[ n ] == 0 )
			// 					{
			// 
			// 					}
			// 				}		
			//				if( pExeInfo )
			//				{
			//					if( (*it)->GetCurCount() != CsMin( (*it)->GetTypeCount(), pExeInfo->m_nCondition[ n ] ) )
			//					{
			// 						DWORD dwMap = (*it)->GetTypeID()/1000;
			// 						int nRegion = (*it)->GetTypeID()%1000;
			// 						if( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID != dwMap )
			// 							break;
			// 						if( g_pCharMng->GetTamerUser()->GetCurPosRegionIndex() != nRegion )
			// 							break;
			// 						(*it)->SetCurCount( pExeInfo->m_nCondition[ n ] );
			// 						nState = 2;
			//					}
			// 					else if( pExeInfo->m_nCondition[ n ] != (*it)->GetCurCount() )
			// 					{
			// 						(*it)->SetCurCount( pExeInfo->m_nCondition[ n ] );
			// 						nState = 1;
			// 					}	
			//				}
			// 			}
			// 			break;
			// 아이템 - 별도 체크
		case CsQuestCompRequire::ITEM:
			{
				int nReqItemCount = g_pDataMng->GetInven()->GetCount_Item_ID( (*it)->GetTypeID() );
				nReqItemCount += g_pDataMng->GetTEquip()->GetCount_Item_ID( (*it)->GetTypeID() );

				int nOldItemCount = (*it)->GetCurCount();
				if( nOldItemCount != nReqItemCount )
				{
					(*it)->SetCurCount( nReqItemCount );
					if( nOldItemCount < (*it)->GetCurCount() )
						nState = 2;
					else if( nState == 0 )
						nState = 1;
				}

				// 통신용 업데이트
				if( pExeInfo )
					pExeInfo->m_nCondition[ n ] = (*it)->GetCurCount();
			}
			break;
		default:		assert_cs( false );		break;
		}
	}
	return nState;
}

// 리턴값 0 - 변경 없음, 1 - 작게 변경, 2 - 크게 변경
// int cData_Quest::_CheckProcessCondition( sPROCESS* pProcess )
// {
// 	int nState = 0;
// 
// 	cMapRegion* pMapRegion = g_pDataMng->GetMapRegion()->GetMapRegion();
// 	// 갯수 집어 넣자
// 	int nCR = pProcess->s_CurCompRequireGroup.GetListCount();
// 	nQuest::sExeInfo* pExeInfo = m_Info.GetExeInfo_ByID( (u2)pProcess->s_pFT->GetUniqID() );
// 	CsQuestCompRequire* pCompRequire;
// 	for( int i=0; i<nCR; ++i )
// 	{
// 		pCompRequire = pProcess->s_CurCompRequireGroup.Get( i );
// 
// 		switch( pCompRequire->GetType() )
// 		{
// 		case CsQuestCompRequire::T_LEVEL:	// 테이머 레벨 체크
// 			{
// 				if( g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel() < (int)pCompRequire->GetTypeID() )
// 				{
// 					pCompRequire->SetCurCount( 0 );
// 				}
// 				else
// 				{
// 					pCompRequire->SetCurCount( 1 );
// 					nState = 2;
// 				}
// 			}break;
// 		case CsQuestCompRequire::D_TYPE:	// 디지몬 타입 체크
// 			{
// 				if( g_pCharMng->GetDigimonUser( 0 )->GetBaseDigimonFTID() != pCompRequire->GetTypeID() )
// 				{
// 					if( pCompRequire->GetCurCount() == 1 )
// 						nState = 2;
// 					pCompRequire->SetCurCount( 0 );
// 				}
// 				else
// 				{
// 					if( pCompRequire->GetCurCount() == 0 )
// 						nState = 2;
// 					pCompRequire->SetCurCount( 1 );					
// 				}				
// 			}break;
// 		case CsQuestCompRequire::MONSTER:
// 		case CsQuestCompRequire::NPCTALK:
// 		case CsQuestCompRequire::CLIENT:
// 		case CsQuestCompRequire::USE_ITEM:
// 		case CsQuestCompRequire::USE_ITEM_NPC:
// 		case CsQuestCompRequire::USE_ITEM_MONSTER:
// 		case CsQuestCompRequire::USE_ITEM_REGION:
// 			{
// 				assert_cs( pExeInfo != NULL );
// 				if( pExeInfo )
// 				{
// 					if( pCompRequire->GetCurCount() != CsMin( pCompRequire->GetTypeCount(), pExeInfo->m_nCondition[ i ] ) )
// 					{
// 						if( pCompRequire->GetCurCount() < CsMin( pCompRequire->GetTypeCount(), pExeInfo->m_nCondition[ i ] ) )
// 							nState = 2;
// 						else if( nState == 0 )
// 							nState = 1;
// 
// 						pCompRequire->SetCurCount( pExeInfo->m_nCondition[ i ] );
// 					}
// 				}				
// 			}
// 			break;		
// 			// 영역 별도 체크
// 		case CsQuestCompRequire::REGION:
// 			{
// 				DWORD dwMap = pCompRequire->GetTypeID()/1000;
// 				int nRegion = pCompRequire->GetTypeID()%1000;
// 				assert_cs( nRegion < nLimit::Region );				
// 				if( pMapRegion->IsOpened( nsCsMapTable::g_pMapListMng->MapIDToMapRegionID( dwMap ), nRegion ) == true )
// 				{
// 					if( pCompRequire->GetCurCount() == 0 )
// 					{
// 						pCompRequire->SetCurCount( 1 );
// 						nState = 2;
// 					}
// 				}
// 				else
// 				{
// 					assert_cs( pCompRequire->GetCurCount() == 0 );
// 				}
// 			}
// 			break;
// 			// 아이템 - 별도 체크
// 		case CsQuestCompRequire::ITEM:
// 			{
// 				int nReqItemCount = g_pDataMng->GetInven()->GetCount_Item_ID( pCompRequire->GetTypeID() );
// 				nReqItemCount += g_pDataMng->GetTEquip()->GetCount_Item_ID( pCompRequire->GetTypeID() );
// 				
// 				int nOldItemCount = pCompRequire->GetCurCount();
// 				if( nOldItemCount != nReqItemCount )
// 				{
// 					pCompRequire->SetCurCount( nReqItemCount );
// 					if( nOldItemCount < pCompRequire->GetCurCount() )
// 						nState = 2;
// 					else if( nState == 0 )
// 						nState = 1;
// 				}
// 
// 				// 통신용 업데이트
// 				if( pExeInfo )
// 				{
// 					pExeInfo->m_nCondition[ i ] = pCompRequire->GetCurCount();
// 				}				
// 			}
// 			break;
// 		default:
// 			assert_cs( false );
// 		}
// 	}
// 	return nState;
// }

void cData_Quest::_DeleteProcQuest()
{
	// 진행 하는 퀘스트
	std::map< DWORD, sPROCESS* >::iterator itProcess = m_mapProcQuest.begin();
	std::map< DWORD, sPROCESS* >::iterator itProcessEnd = m_mapProcQuest.end();
	for( ; itProcess !=itProcessEnd; ++itProcess )
	{
		itProcess->second->Delete();
		sPROCESS::DeleteInstance( itProcess->second );
	}
	m_mapProcQuest.clear();
}

CsC_AvObject* cData_Quest::_GetOwner( CsQuest* pFT )
{
	switch( pFT->GetQuestTarget() )
	{
	case CsQuest::QTG_SELF:
		if( g_pNpcMng->IsNpc( pFT->GetStartTarget_ID() ) )
		{
			return g_pNpcMng->GetNpc( pFT->GetStartTarget_ID() );
		}
		break;
	case CsQuest::QTG_NPC:
		if( g_pNpcMng->IsNpc( pFT->GetQuestTargetValue1() ) )
		{
			return g_pNpcMng->GetNpc( pFT->GetQuestTargetValue1() );
		}
		break;
	case CsQuest::QTG_COMPLETE:
		break;
	default:
		assert_cs( false );
	}
	return NULL;
}

TCHAR* cData_Quest::NpcTalk( DWORD dwNpcFTID )
{
	// 진행 하는 퀘스트
	CsQuestCompRequireGroup* pGroup = NULL;
	CsQuestCompRequire* pCompRequite = NULL;	
	CsQuest* pQuestFT;

	CsQuest* pNpcTalkRetunQuest = NULL;
	// 완료 조건 이 말걸어야 하는거라면	//	퀘스트 수행중 리스트
	std::map< DWORD, sPROCESS* >::iterator itProcess = m_mapProcQuest.begin();
	std::map< DWORD, sPROCESS* >::iterator itProcessEnd = m_mapProcQuest.end();
	for( ; itProcess !=itProcessEnd; ++itProcess )
	{
		pQuestFT = itProcess->second->s_pFT;
		pGroup = &itProcess->second->s_CurCompRequireGroup;

		int nCount = pGroup->GetListCount();
		for( int i=0; i<nCount; ++i )
		{
			pCompRequite = pGroup->Get( i );
			switch( pCompRequite->GetType() )
			{
			case CsQuestCompRequire::NPCTALK:
				{
					if( pCompRequite->GetTypeID() != dwNpcFTID )
						continue;

					if( pCompRequite->IsCompleat() == true )
						continue;

					// 서버에 통신
					if( net::game )
					{
						net::game->SendQuestChangeInfo( pQuestFT->GetUniqID(), i, 1 );
					}

					// 통신용 업데이트
					m_Info.GetExeInfo_ByID( (u2)pQuestFT->GetUniqID() )->m_nCondition[ i ] = 1;

					// 마크 제거
					assert_cs( g_pNpcMng->IsNpc( dwNpcFTID ) == true );
					g_pNpcMng->GetNpc( dwNpcFTID )->SetQuestViewType( CNpc::QV_NONE );

					// 대화 복사
					pCompRequite->IncreaseCurCount();
					bool bIsGroupCompleate = pGroup->IsCompleate();
					_CompReadyQuest( bIsGroupCompleate, pQuestFT );

					if( bIsGroupCompleate == true )
					{
						// 엔피시 디스플레이 정보 변경
						itProcess->second->s_bCompleate = true;
						CNpc* pNpc = (CNpc*)_GetOwner( pQuestFT );
						if( pNpc != NULL )
						{
							pNpc->GetQuestOwner()->CalDisplayRev();
						}						
					}
					// IF에 연결
					g_pGameIF->GetQuestHelper()->ResetHelperString();
					if( g_pGameIF->GetQuestList() )
					{
						g_pGameIF->GetQuestList()->ResetQuestList( true );
					}
					pNpcTalkRetunQuest = pQuestFT;
				}
				break;
			case CsQuestCompRequire::MONSTER:
			case CsQuestCompRequire::ITEM:
			case CsQuestCompRequire::CLIENT:
			case CsQuestCompRequire::REGION:
			case CsQuestCompRequire::USE_ITEM:
			case CsQuestCompRequire::USE_ITEM_NPC:
			case CsQuestCompRequire::USE_ITEM_REGION:
			case CsQuestCompRequire::USE_ITEM_MONSTER:
			case CsQuestCompRequire::T_LEVEL:
			case CsQuestCompRequire::D_TYPE:
				break;
			default:
				assert_cs( false );
			}
		}
	}

	if( pNpcTalkRetunQuest != NULL )
	{
		return pNpcTalkRetunQuest->m_szExpert;
	}

	// 퀘스트 중에 말걸었다면
	itProcess = m_mapProcQuest.begin();
	itProcessEnd = m_mapProcQuest.end();
	for( ; itProcess !=itProcessEnd; ++itProcess )
	{
		pQuestFT = itProcess->second->s_pFT;
		switch( pQuestFT->GetStartTarget_Type() )
		{
		case CsQuest::ST_NPC:
			{
				if( pQuestFT->GetStartTarget_ID() == dwNpcFTID )
				{
					return pQuestFT->m_szProcess;
				}
			}
			break;
		case CsQuest::ST_DIGIVICE:
			break;
		default:
			assert_cs( false );
		}
	}

	return NULL;
}

void cData_Quest::CheckRegion( DWORD const & dwMapId, int const& dwRegionID )
{
	// 완료 조건 이 말걸어야 하는거라면	
	std::map< DWORD, sPROCESS* >::iterator itProcess = m_mapProcQuest.begin();
	std::map< DWORD, sPROCESS* >::iterator itProcessEnd = m_mapProcQuest.end();
	for( ; itProcess !=itProcessEnd; ++itProcess )
	{
		CsQuest* pQuestFT = itProcess->second->s_pFT;
		CsQuestCompRequireGroup* pGroup = &itProcess->second->s_CurCompRequireGroup;

		int nCount = pGroup->GetListCount();
		for( int i=0; i<nCount; ++i )
		{
			CsQuestCompRequire* pCompRequite = pGroup->Get( i );
			if( pCompRequite->IsCompleat() == true )
				continue;

			switch( pCompRequite->GetType() )
			{
			case CsQuestCompRequire::REGION:
				{
					DWORD dwMap = pCompRequite->GetTypeID()/1000;
					int nRegion = pCompRequite->GetTypeID()%1000;

					if( dwMapId != dwMap || dwRegionID != nRegion )
						continue;				

					nQuest::sExeInfo* pExeInfo = m_Info.GetExeInfo_ByID( (u2)pQuestFT->GetUniqID() );

					if( pCompRequite->GetTypeCount() != pExeInfo->m_nCondition[ i ] )
					{
						pExeInfo->m_nCondition[ i ] += 1;
						bool bCompRequite = pCompRequite->IncreaseCurCount();

						// 서버에 통신
						if( net::game )
							net::game->SendQuestChangeInfo( pQuestFT->GetUniqID(), i, pCompRequite->GetCurCount() );

						bool bIsGroupCompleate = pGroup->IsCompleate();
						_CompReadyQuest( bIsGroupCompleate, pQuestFT );
						if( bIsGroupCompleate == true )
						{
							// 엔피시 디스플레이 정보 변경
							itProcess->second->s_bCompleate = true;
							CNpc* pNpc = (CNpc*)_GetOwner( pQuestFT );
							if( pNpc != NULL )
								pNpc->GetQuestOwner()->CalDisplayRev();
						}

						// IF에 연결
						g_pGameIF->GetQuestHelper()->ResetHelperString();
						if( g_pGameIF->GetQuestList() )
							g_pGameIF->GetQuestList()->ResetQuestList( true );
					}					
				}break;
			}
		}
	}

}

DWORD cData_Quest::UseItem( CsItem* pFTUseItem )
{
	// 진행 하는 퀘스트
	CsQuestCompRequireGroup* pGroup = NULL;
	CsQuestCompRequire* pCompRequite = NULL;	
	CsQuest* pQuestFT;

	// 완료 조건 이 말걸어야 하는거라면	
	std::map< DWORD, sPROCESS* >::iterator itProcess = m_mapProcQuest.begin();
	std::map< DWORD, sPROCESS* >::iterator itProcessEnd = m_mapProcQuest.end();
	for( ; itProcess !=itProcessEnd; ++itProcess )
	{
		pQuestFT = itProcess->second->s_pFT;
		pGroup = &itProcess->second->s_CurCompRequireGroup;

		int nCount = pGroup->GetListCount();
		for( int i=0; i<nCount; ++i )
		{
			pCompRequite = pGroup->Get( i );
			if( pCompRequite->IsCompleat() == true )
				continue;

			switch( pCompRequite->GetType() )
			{
			case CsQuestCompRequire::USE_ITEM_NPC:
				{
					if( pCompRequite->GetTypeID() != pFTUseItem->GetInfo()->s_dwItemID )
						continue;

					CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
					if( pTarget == NULL )
						continue;

					if( pTarget->GetLeafRTTI() != RTTI_NPC )
						continue;

					if( pCompRequite->GetTargetID() != pTarget->GetFTID() )
						continue;

					// 거리체크
					if( FMCommon::CalDist_2D( g_pCharMng->GetTamerUser(), pTarget ) > IF_TARGET_TAMER_MAX_DIST )
					{
						// 그쪽으로 움직이자
						g_pCharMng->GetTamerUser()->SetTamerUserState( CTamerUser::TU_USE_TARGET_ITEM, pTarget );
						g_pCharMng->GetTamerUser()->SetStateValue( CTamerUser::TU_USE_TARGET_ITEM, pFTUseItem->GetInfo()->s_dwItemID );
						return 0;
					}

					// 통신용 업데이트
					m_Info.GetExeInfo_ByID( (u2)pQuestFT->GetUniqID() )->m_nCondition[ i ] += 1;

					// 대화 복사
					bool bCompRequite = pCompRequite->IncreaseCurCount();

					// 서버에 통신
					if( net::game )
					{
						net::game->SendQuestChangeInfo( pQuestFT->GetUniqID(), i, pCompRequite->GetCurCount() );
					}

					bool bIsGroupCompleate = pGroup->IsCompleate();
					_CompReadyQuest( bIsGroupCompleate, pQuestFT );
					if( bIsGroupCompleate == true )
					{
						// 엔피시 디스플레이 정보 변경
						itProcess->second->s_bCompleate = true;
						CNpc* pNpc = (CNpc*)_GetOwner( pQuestFT );
						if( pNpc != NULL )
						{
							pNpc->GetQuestOwner()->CalDisplayRev();
						}
					}
					
					// IF에 연결
					g_pGameIF->GetQuestHelper()->ResetHelperString();
					if( g_pGameIF->GetQuestList() )
					{
						g_pGameIF->GetQuestList()->ResetQuestList( true );
					}

					// 대화창 오픈
					if( ( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_REV ) == false )&&
						( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_COMP ) == false )	)
					{
						cTalk* pTalk = (cTalk*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_TALK );
						if( pTalk )
						{
							// 토크 중복 생성 되지 않게
							pTalk->SetTarget( pTarget, pTalk->GetTarget() != pTarget, pQuestFT->m_szExpert );
						}
					}

					return pQuestFT->GetUniqID();
				}
				break;

			case CsQuestCompRequire::USE_ITEM_MONSTER:
				{
					if( pCompRequite->GetTypeID() != pFTUseItem->GetInfo()->s_dwItemID )
						continue;

					CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
					if( pTarget == NULL )
						continue;

					if( pTarget->GetLeafRTTI() != RTTI_MONSTER )
						continue;

					if( pCompRequite->GetTargetID() != pTarget->GetFTID() )
						continue;

					// 통신용 업데이트
					m_Info.GetExeInfo_ByID( (u2)pQuestFT->GetUniqID() )->m_nCondition[ i ] += 1;

					// 대화 복사
					bool bCompRequite = pCompRequite->IncreaseCurCount();

					// 서버에 통신
					if( net::game )
					{
						net::game->SendQuestChangeInfo( pQuestFT->GetUniqID(), i, pCompRequite->GetCurCount() );
					}

					bool bIsGroupCompleate = pGroup->IsCompleate();
					_CompReadyQuest( bIsGroupCompleate, pQuestFT );
					if( bIsGroupCompleate == true )
					{
						// 엔피시 디스플레이 정보 변경
						itProcess->second->s_bCompleate = true;
						CNpc* pNpc = (CNpc*)_GetOwner( pQuestFT );
						if( pNpc != NULL )
						{
							pNpc->GetQuestOwner()->CalDisplayRev();
						}
					}					

					// IF에 연결
					g_pGameIF->GetQuestHelper()->ResetHelperString();
					if( g_pGameIF->GetQuestList() )
					{
						g_pGameIF->GetQuestList()->ResetQuestList( true );
					}
					return pQuestFT->GetUniqID();
				}
				break;

			case CsQuestCompRequire::USE_ITEM_REGION:
				{
					if( pCompRequite->GetTypeID() != pFTUseItem->GetInfo()->s_dwItemID )
						continue;

					DWORD dwMap = pCompRequite->GetTargetID()/1000;
					int nRegion = pCompRequite->GetTargetID()%1000;

					if( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID != dwMap )
						continue;
					if( g_pCharMng->GetTamerUser()->GetCurPosRegionIndex() != nRegion )
						continue;					

					// 통신용 업데이트
					m_Info.GetExeInfo_ByID( (u2)pQuestFT->GetUniqID() )->m_nCondition[ i ] += 1;

					// 대화 복사
					bool bCompRequite = pCompRequite->IncreaseCurCount();

					// 서버에 통신
					if( net::game )
					{
						net::game->SendQuestChangeInfo( pQuestFT->GetUniqID(), i, pCompRequite->GetCurCount() );
					}

					bool bIsGroupCompleate = pGroup->IsCompleate();
					_CompReadyQuest( bIsGroupCompleate, pQuestFT );
					if( bIsGroupCompleate == true )
					{
						// 엔피시 디스플레이 정보 변경
						itProcess->second->s_bCompleate = true;
						CNpc* pNpc = (CNpc*)_GetOwner( pQuestFT );
						if( pNpc != NULL )
						{
							pNpc->GetQuestOwner()->CalDisplayRev();
						}
					}

					// IF에 연결
					g_pGameIF->GetQuestHelper()->ResetHelperString();
					if( g_pGameIF->GetQuestList() )
					{
						g_pGameIF->GetQuestList()->ResetQuestList( true );
					}
					return pQuestFT->GetUniqID();
				}
				break;

			case CsQuestCompRequire::USE_ITEM:
				{
					if( pCompRequite->GetTypeID() != pFTUseItem->GetInfo()->s_dwItemID )
						continue;

					// 통신용 업데이트
					m_Info.GetExeInfo_ByID( (u2)pQuestFT->GetUniqID() )->m_nCondition[ i ] += 1;

					// 대화 복사
					bool bCompRequite = pCompRequite->IncreaseCurCount();

					// 서버에 통신
					if( net::game )
					{
						net::game->SendQuestChangeInfo( pQuestFT->GetUniqID(), i, pCompRequite->GetCurCount() );
					}
					
					bool bIsGroupCompleate = pGroup->IsCompleate();
					_CompReadyQuest( bIsGroupCompleate, pQuestFT );
					if( bIsGroupCompleate == true )
					{
						// 엔피시 디스플레이 정보 변경
						itProcess->second->s_bCompleate = true;
						CNpc* pNpc = (CNpc*)_GetOwner( pQuestFT );
						if( pNpc != NULL )
						{
							pNpc->GetQuestOwner()->CalDisplayRev();
						}
					}

					// IF에 연결
					g_pGameIF->GetQuestHelper()->ResetHelperString();
					if( g_pGameIF->GetQuestList() )
					{
						g_pGameIF->GetQuestList()->ResetQuestList( true );
					}
					return pQuestFT->GetUniqID();
				}
				break;
			case CsQuestCompRequire::MONSTER:
			case CsQuestCompRequire::ITEM:
			case CsQuestCompRequire::CLIENT:
			case CsQuestCompRequire::REGION:
			case CsQuestCompRequire::NPCTALK:		
			case CsQuestCompRequire::T_LEVEL:
			case CsQuestCompRequire::D_TYPE:	
				break;
			default:
				assert_cs( false );
			}
		}
	}

	return 0;
}

//===========================================================================
//
//		퀘스트 포기
//
//===========================================================================
void cData_Quest::DropProcAllQuest()
{
	std::list< DWORD > listDel;

	// 진행 하는 퀘스트 
	std::map< DWORD, sPROCESS* >::iterator itProcess = m_mapProcQuest.begin();
	std::map< DWORD, sPROCESS* >::iterator itProcessEnd = m_mapProcQuest.end();
	for( ; itProcess !=itProcessEnd; ++itProcess )
	{
		listDel.push_back( itProcess->second->s_pFT->GetUniqID() );
	}

	std::list< DWORD >::iterator itDel = listDel.begin();
	std::list< DWORD >::iterator itDelEnd = listDel.end();
	for( ; itDel != itDelEnd; ++itDel )
	{
		DropProcQuest( *itDel );		
	}

	assert_cs( m_mapProcQuest.empty() );
}

void cData_Quest::DropProcQuest()
{
	// IF에 연결
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_DISCRIPT ) == false )
		return;

	DropProcQuest( g_pGameIF->GetQuestDiscript()->GetQuest()->GetUniqID() );
}

void cData_Quest::DropProcQuest( DWORD nQuestID )
{
	CsQuest* pQuestFT = nsCsFileTable::g_pQuestMng->GetQuest( nQuestID );
	SAFE_POINTER_RET( pQuestFT );

	// 튜토리얼 퀘스트는 포기 못함.
// 	if( pQuestFT->IsQuestType( CsQuest::QT_TUTORIAL ) )
// 	{	QUEST_DROP_BTN
// 		cPrintMsg::PrintMsg( 20062 );
// 		return;
// 	}

	DWORD dwQuestID = pQuestFT->GetUniqID();
	std::map< DWORD, sPROCESS* >::iterator it = m_mapProcQuest.find( dwQuestID );
	if( it == m_mapProcQuest.end() )
		return;
	SAFE_POINTER_RET( it->second );	
	it->second->Delete();
	// 맵에서 제거
	sPROCESS::DeleteInstance( it->second );
	m_mapProcQuest.erase( it );

	// 퀘스트 헬퍼 뷰어 에서 끔
	int nArrayIndex = GetInfoArrayIndex( dwQuestID );
	g_pResist->m_Character.s_bQuestHelperViewID[ nArrayIndex ] = false;

	// Info 에서 제거
	bool bSuccess = m_Info.Cancel( dwQuestID );
	assert_cs( bSuccess == true );

	// 수행지급 아이템, 수행 시간 제거
	CsQuestApplyRequiteGroup* pApplyRequiteGroup = pQuestFT->GetApplyRequiteGroup();
	CsQuestApplyRequite* pApplyRequite;
	int nElementCount = pApplyRequiteGroup->GetListCount();
	for( int i=0; i<nElementCount; ++i )
	{
		pApplyRequite = pApplyRequiteGroup->Get( i );
		switch( pApplyRequite->GetType() )
		{
		case CsQuestApplyRequite::ITEM:
			{
#ifdef TRICK_ORTREAT
				/////////////////////////////////////////////////////////////////////////////////////////////////
				CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( pApplyRequite->GetTypeID() )->GetInfo();				

				// 삭제 하려는 아이템이 nItem::TimeSet면 추가로 아래를 수행한다.
				// 문제점.다른퀘에도 같은 타입아이템중복되면 있으면 망한다. <- 미확인 문제
				if( 3 == pFTInfo->s_btUseTimeType && (nItem::TimeSet == pFTInfo->s_nType_L) ){

					// 여기로 오면... "시간제한 아이템과 함께하는 퀘스트"를 포기한것. 사냥해서 모든 완료조건 아이템을 모두 지워준다.

					//2.이펙트 폭탄이 터진다. 좌절 이모션
					int nFindIndex;
					CsEmotion::sINFO* pInfo = nsCsFileTable::g_pTamerMng->FindEmotion( L"좌절", nFindIndex );
					if( pInfo )
					{
						if( ( net::game ) )
						{						
							net::game->SendEmoticon( pInfo->s_nID , 1);
						}
						else
							g_pCharMng->GetTamerUser()->SetAnimation( pInfo->s_nAniID );
					}


					//3.아이템(완료조건의아이템) 지워준다. ex)호박퀘스트면 인밴에 모아둔 사탕. 
					int nCR = pQuestFT->GetCompRequireGroup()->GetListCount();
					CsQuestCompRequire *pCompRequire;
					for( int i=0; i<nCR; ++i )
					{
						pCompRequire = pQuestFT->GetCompRequireGroup()->Get(i);
						//완료조건을 참조하여 아이템 싹 다지워준다.
						if(pCompRequire)
						{
							if(CsQuestCompRequire::ITEM == pCompRequire->GetType())
							{
								CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( pCompRequire->GetTypeID() )->GetInfo();				
								g_pDataMng->GetInven()->ItemRemove_NoServer( pCompRequire->GetTypeID(), true );	// 인밴 보관한거 다지운다.				
								int nItemIndex = pCompRequire->GetTypeID();
								GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_ITEMREMOVE_NOSERVER, &nItemIndex );
								//							g_pDataMng->GetWarehouse()->ItemRemove_NoServer( pCompRequire->GetTypeID(), true );	// 창고 보관한거 다지운다.

							}
						}
					}

					// 4. 카운트 지워준다.
					g_pGameIF->CloseDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_TIME_ITEM, true );
				}
				/////////////////////////////////////////////////////////////////////////////////////////////////
#endif
				//지급 아이템 삭제
				g_pDataMng->GetInven()->ItemRemove_NoServer( pApplyRequite->GetTypeID(), pApplyRequite->GetTypeCount(), false );				
			}
			break;		
		default:
			assert_cs( false );
		}
	}	


	// Owner쪽에 Process제거
	CNpc* pNpc = (CNpc*)_GetOwner( pQuestFT );
	if( pNpc != NULL )
	{
		pNpc->GetQuestOwner()->DeleteQuest( pQuestFT->GetUniqID() );
		pNpc->SetQuestViewType( CNpc::QV_NONE );
	}
	// 디지바이스
	else
	{
		g_pDataMng->GetQuestOwner()->GetProcDigiviceOwner()->DeleteQuest( pQuestFT );
	}

	// Owner쪽에 입력
	g_pDataMng->GetQuestOwner()->AddQuest_New( &m_Info, pQuestFT );

	// IF에 연결	
	if( g_pGameIF->GetQuestList() )
	{
		g_pGameIF->GetQuestList()->ResetQuestList();
	}
	g_pGameIF->GetQuestHelper()->ResetHelperString();

	// 서버에 동기화
	if( net::game )
	{
		net::game->SendQuestCancel( pQuestFT->GetUniqID() );
	}

	// 퀘스트에 연결된 이벤트 번호 삭제
	int nEventSceneNum = nsCsFileTable::g_pQuestMng->GetQuest( pQuestFT->GetUniqID() )->m_nEvent[ CsQuest::EVENT_QUEST_REQUITE ];
	if( g_pEventDataMng->IsData( nEventSceneNum ) )
	{
		g_pEventSceneMng->DeleteScene( nEventSceneNum );
	}

	// 메세지 표현	
	cPrintMsg::PrintMsg( 30002, pQuestFT->m_szTitleText );

	if( pQuestFT->IsQuestType( CsQuest::QT_TUTORIAL ) )
		GAME_EVENT_ST.OnEvent( EVENT_CODE::TUTORIALQUEST_CANCEL, &dwQuestID );
	
	// 문제 생길 인터페이스 닫아주자
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_COMP );
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_REV );
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TALK );
}

//===========================================================================
//
//		퀘스트 완료
//
//===========================================================================
void cData_Quest::_CompReadyQuest( bool bCompleate, CsQuest* pQuestFT )
{
	// 해당 퀘스트 전부 완료라면
	if( bCompleate == true )
	{
		// 시작타겟이 디지바이스 퀘면서 완료 대상이 시작타겟이라면
		bool bUseDigiviceIF = false;
		if( ( pQuestFT->GetStartTarget_Type() == CsQuest::ST_DIGIVICE )&&( pQuestFT->GetQuestTarget() == CsQuest::QTG_SELF ) )
		{
			bUseDigiviceIF = true;
		}
		// 즉시 완료 퀘스트인경우
		if( pQuestFT->GetQuestTarget() == CsQuest::QTG_COMPLETE )
		{
			bUseDigiviceIF = true;
		}

		if( bUseDigiviceIF == true )
		{
			g_pDataMng->GetQuestOwner()->IF_DigiviceQuest_Open();
		}


		// 퀘스트 헬퍼 뷰어 에서 끔
		g_pCharMng->GetTamerUser()->GetProp_Effect()->AddEffect( "system\\QuestEnd.nif", 1.0f, 0 );

		// 메세지 표현		
		cWindow::PlaySound( cWindow::SD_Qu1 );
		cPrintMsg::PrintMsg( 14005, pQuestFT->m_szTitleText );

		cDigimonTalk::sTalkEle sEle;
		sEle.s_dwQuest = pQuestFT->GetUniqID();
		cDigimonTalk::Print( 14005, &sEle );

		//_NewBiQuestCheck( pQuestFT );

		int nEventSceneNum = nsCsFileTable::g_pQuestMng->GetQuest( pQuestFT->GetUniqID() )->m_nEvent[ CsQuest::EVENT_QUEST_COMPLEAT ];
		if( nsCsFileTable::g_pSceneDataMng->IsData( (DWORD)nEventSceneNum ) == true )
		{
			g_pEventScene->EventStart( nEventSceneNum );	
		}

		g_pNpcMng->CheckQuestExtra();
	}
	else
	{
		// 메세지 표현
		cPrintMsg::PrintMsg( 14006, pQuestFT->m_szTitleText );
	}
}

void cData_Quest::_NewBiQuestCheck( CsQuest* pQuestFT )
{
	//초보자 퀘스트 헬퍼 연결
	switch( pQuestFT->GetUniqID() )
	{
	case 622:		// 닷트에서의 첫 임무!
		CsNpc* pFTNpc = nsCsFileTable::g_pNpcMng->GetNpc( 90009 ); //요코하마 매리
		g_pHelpArrow->SetArrowPoint( 90009, pFTNpc->GetInfo()->s_dwMapID, cQuestFindWay::FindWay_Npc, pFTNpc->GetInfo()->s_szName );	
		break;
	}
}

bool cData_Quest::CompQuest( CsQuest* pQuestFT, CsC_AvObject* pNpc )
{
	assert_cs( m_mapProcQuest[ pQuestFT->GetUniqID() ]->s_bCompleate == true );
	SAFE_POINTER_RETVAL( pQuestFT, false );

	std::map< DWORD, sPROCESS* >::iterator it = m_mapProcQuest.find( pQuestFT->GetUniqID() );
	if( it == m_mapProcQuest.end() )
		return false;

	sPROCESS* pProcess = it->second;	// 완료하기이전에 수행중 정보를 가져온다.
	SAFE_POINTER_RETVAL( pProcess, false );

	//===================================================================================================
	// 조그레스 퀘스트 검사
	//===================================================================================================
	// 먼저 조그레스 퀘스트인지?
	// 완료할 퀘스트가 선행조건이 맞지않으면..
	switch( pQuestFT->GetQuestType() )
	{
	case CsQuest::QT_JOINTPROGRESS:
	case CsQuest::QT_XANTI_JOINTPROGRESS:
		{
			if(!CheckPreRequire(pQuestFT))
			{
				cPrintMsg::PrintMsg( 30515 );
				return false;
			}
		}
		break;
	}	

	//===================================================================================================
	// 보상 증가
	//===================================================================================================
	uint nExp = 0;
	u8 nMoney = 0;

	int nReqItemCount = 0;

	// 넣어 줄수 있는 상황인제 체크
	CsQuestRequiteGroup* pRequiteGroup = pQuestFT->GetRequiteGroup();
	for( int i=0; i<pRequiteGroup->GetListCount(); ++i )
	{
		CsQuestRequite* pRequite = pRequiteGroup->Get( i );
		assert_cs( pRequite->GetMethodID() == CsQuestRequite::RM_GIVE );
		if( pRequite->GetType() == CsQuestRequite::ITEM )
		{
			++nReqItemCount;
		}
	}
	
	// 인벤의 공간이 부족하다면
	if( g_pDataMng->GetInven()->GetEmptySlotCount() < nReqItemCount )
	{
		cPrintMsg::PrintMsg( cPrintMsg::GAME_INVEN_LIMIT_COUNT );
		return false;
	}


	// 진화 슬롯 오픈 보상일 경우 예외 처리
	pRequiteGroup = pQuestFT->GetRequiteGroup();
	for( int i=0; i<pRequiteGroup->GetListCount(); ++i )
	{
		CsQuestRequite* pRequite = pRequiteGroup->Get( i );
		if( pRequite->GetType() != CsQuestRequite::EVOSLOT )
			continue;

		DWORD dwDigimonTbID = pRequite->GetTypeID();
		int nOpenSlotIndex = pRequite->GetTypeCount();

		CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
		SAFE_POINTER_RETVAL( pDUser, false );

		// 오픈 하려는 슬롯의 디지몬의 인덱스가 다르다.
		if( dwDigimonTbID != pDUser->GetBaseDigimonFTID() )
		{
			cPrintMsg::PrintMsg( 31020 );
			return false;
		}

		// 이전 슬롯이 오픈 안되어 있을 경우 
		if( nOpenSlotIndex > 0 && !pDUser->IsOpenedEvoSlot( nOpenSlotIndex - 1 ) )
		{
			cPrintMsg::PrintMsg( 31022 );
			return false;
		}

		// 이미 열려 있는 슬롯인 경우 에러
		if( pDUser->IsOpenedEvoSlot( nOpenSlotIndex ) )
		{
			cPrintMsg::PrintMsg( 31021 );
			return false;
		}
	}

	for( int i=0; i<pRequiteGroup->GetListCount(); ++i )
	{
		CsQuestRequite* pRequite = pRequiteGroup->Get( i );
		assert_cs( pRequite->GetMethodID() == CsQuestRequite::RM_GIVE );
		switch( pRequite->GetType() )
		{
		case CsQuestRequite::MONEY:
			nMoney = pRequite->GetTypeID();
			break;
		case CsQuestRequite::EXP:
			//nExp = g_pDataMng->GetQuest()->GetQuestExp( pQuestFT, pRequite->GetTypeID() );
			nExp = pRequite->GetTypeID();
			break;
		case CsQuestRequite::ITEM:
			{				
				// [4/15/2016 hyun] 수정. 다수의 아이템 먹었을때 처리
				CsItem::sINFO* pItem = nsCsFileTable::g_pItemMng->GetItem( pRequite->GetTypeID() )->GetInfo();
				// 보상 받았다면 메세지				
				if(pItem)
				{
					int GetItemCount = pRequite->GetTypeCount();
					if(GetItemCount == 1)
						cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_CROP, pItem->s_szName, &g_pDataMng->GetItemColor(pItem));
					else if(GetItemCount > 1)
						cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_MANYCROP, pItem->s_szName, &GetItemCount, &g_pDataMng->GetItemColor(pItem));
				}
				// @@ 여기까지
				g_pDataMng->GetInven()->ItemCrop( pRequite->GetTypeID(), pRequite->GetTypeCount() );
			}			
			break;
		case CsQuestRequite::EVOSLOT:

			break;
		default:
			assert_cs( false );
		}
	}

	// 아이템 다 넣어 줬다면 돈과 경험치 증가 시키자
	// 돈증가
	g_pDataMng->GetInven()->IncreaseMoney( nMoney, true );

	//===================================================================================================
	// 요구사항 감소
	//===================================================================================================
	int nCR = pProcess->s_CurCompRequireGroup.GetListCount();
	CsQuestCompRequire* pCompRequire;
	for( int i=0; i<nCR; ++i )
	{
		pCompRequire = pProcess->s_CurCompRequireGroup.Get( i );
		if(pCompRequire)
		{
			switch( pCompRequire->GetType() )
			{
			case CsQuestCompRequire::MONSTER:
			case CsQuestCompRequire::NPCTALK:
			case CsQuestCompRequire::CLIENT:
			case CsQuestCompRequire::REGION:
			case CsQuestCompRequire::USE_ITEM:
			case CsQuestCompRequire::USE_ITEM_NPC:
			case CsQuestCompRequire::USE_ITEM_MONSTER:
			case CsQuestCompRequire::USE_ITEM_REGION:
			case CsQuestCompRequire::T_LEVEL:
			case CsQuestCompRequire::D_TYPE:
				break;
				// 아이템 - 별도 체크
			case CsQuestCompRequire::ITEM:
				{

#ifdef TRICK_ORTREAT
					//===================================================================================================
					// 카운트 삭제
					//===================================================================================================

					int nAG = pProcess->s_pFT->GetApplyRequiteGroup()->GetListCount();
					CsQuestApplyRequite * pCsApplyRequite;
					for( int i=0; i<nAG; ++i )
					{
						pCsApplyRequite = pProcess->s_pFT->GetApplyRequiteGroup()->Get(i);
						if(CsQuestApplyRequite::ITEM == pCsApplyRequite->GetType())
						{
							CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( pCsApplyRequite->GetTypeID() )->GetInfo();

							if( 3 == pFTInfo->s_btUseTimeType && (nItem::TimeSet == pFTInfo->s_nType_L) ){
								g_pGameIF->CloseDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_TIME_ITEM, true );	// 카운트 삭제	
								g_pDataMng->GetInven()->ItemRemove_NoServer( pCompRequire->GetTypeID(), true );	// 인밴 보관한거 다지운다.

								int nItemIndex = pCompRequire->GetTypeID();
								GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_ITEMREMOVE_NOSERVER, &nItemIndex );

								//							g_pDataMng->GetWarehouse()->ItemRemove_NoServer( pCompRequire->GetTypeID(), true );	// 창고 보관한거 다지운다.
							}
						}
					}
#endif		

					g_pDataMng->GetInven()->ItemRemove_NoServer( pCompRequire->GetTypeID(), pCompRequire->GetCurCount(), false );
				}
				break;
			default:
				assert_cs( false );
			}
		}
	}
#ifdef TRICK_ORTREAT
	//===================================================================================================
	// 카운트 삭제
	//===================================================================================================

	nCR = pProcess->s_pFT->GetApplyRequiteGroup()->GetListCount();
	CsQuestApplyRequite * pCsApplyRequite;
	for( int i=0; i<nCR; ++i )
	{
		pCsApplyRequite = pProcess->s_pFT->GetApplyRequiteGroup()->Get(i);
		if(CsQuestApplyRequite::ITEM == pCsApplyRequite->GetType())
		{
			CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( pCsApplyRequite->GetTypeID() )->GetInfo();

			if( 3 == pFTInfo->s_btUseTimeType && (nItem::TimeSet == pFTInfo->s_nType_L) ){
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_TIME_ITEM, true );	// 카운트 삭제				
			}
		}
	}	
#endif


	// 맵에서 제거
	m_mapProcQuest[ pQuestFT->GetUniqID() ]->Delete();
	sPROCESS::DeleteInstance( m_mapProcQuest[ pQuestFT->GetUniqID() ] );
	m_mapProcQuest.erase( pQuestFT->GetUniqID() );

	// 퀘스트 헬퍼 뷰어 에서 끔
	int nArrayIndex = GetInfoArrayIndex( pQuestFT->GetUniqID() );
	g_pResist->m_Character.s_bQuestHelperViewID[ nArrayIndex ] = false;

	// Info 에 적용
	bool bSuccess = m_Info.Complete( pQuestFT->GetUniqID() );
	assert_cs( bSuccess == true );

	// Owner쪽에 Process제거
	if( ( (CNpc*)pNpc )->GetQuestOwner() )
	{
		( (CNpc*)pNpc )->GetQuestOwner()->DeleteQuest( pQuestFT->GetUniqID() );		
	}
	// 디지바이스
	else
	{
		g_pDataMng->GetQuestOwner()->GetProcDigiviceOwner()->DeleteQuest( pQuestFT );
	}

	// IF에 연결	
	if( g_pGameIF->GetQuestList() )
	{
		g_pGameIF->GetQuestList()->ResetQuestList();
	}
	g_pGameIF->GetQuestHelper()->ResetHelperString();

	// 서버에 동기화
	if( net::game )
	{
		net::game->SendQuestComplete( pQuestFT->GetUniqID() );
	}

	// 반복퀘 처리 // 조그레스 퀘 처리
	if( ( pQuestFT->GetQuestType() == CsQuest::QT_REPEAT )||
		( pQuestFT->GetQuestType() == CsQuest::QT_EVENTREPEAT )
		//|| ( pQuestFT->GetQuestType() == CsQuest::QT_TIME )
		|| ( pQuestFT->GetQuestType() == CsQuest::QT_JOINTPROGRESS )
#ifdef SDM_TAMER_XGUAGE_20180628
		|| ( pQuestFT->GetQuestType() == CsQuest::QT_XANTI_JOINTPROGRESS )
#endif
		)		
	{
		m_Info.Clear( pQuestFT->GetUniqID() );
		g_pDataMng->GetQuestOwner()->AddQuest_New( GetInfo(), pQuestFT );
	}

	// 슬롯을 뚫어준다.
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
	if( pDUser )
	{
		pDUser->CompQuestJointProgress( pQuestFT->GetUniqID() );
		// 퀘스트 완료 플래그 재설정
		pDUser->SetEvoPlag( false, 1, pDUser->GetMaxEvoUnit() +1 );
		// 퀵에볼
		if( g_pGameIF )
		{
			g_pGameIF->GetQuickEvol()->SetTargetObject( pDUser );
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_DIGIMON_DIGIMONSTATUS, NULL );
		}
	}

	// Owner쪽에 수행 조건 안되는 퀘스트 수행조건 되는지 다시 체크
	RevQuestCheck();
	// 퀵슬롯 체크
	cData_QuickSlot::CheckItemCount_AllUser();
	// 퀘스트 체크
	CalProcess();

	//==========================================================================================================
	// 지형 퀘스트 오브젝트 체크
	//==========================================================================================================
	nsCsGBTerrain::g_pCurRoot->CheckQuestObject( pQuestFT->GetUniqID() );

	//==========================================================================================================
	// 엔피씨 퀘스트~ 온오프
	//==========================================================================================================
	g_pNpcMng->CheckQuestExtra();

	//==========================================================================================================
	// 업적 체크
	//==========================================================================================================
	GS2C_RECV_CHECKTYPE recv;
	recv.nType = AchieveContents::CA_QuestCount;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );

	recv.nType = AchieveContents::CA_QuestComp;
	recv.nValue1 = pQuestFT->GetUniqID();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );

	//==========================================================================================================
	// 완료 예외처리
	//==========================================================================================================
	_CompQuest_PostCheck( pQuestFT );

	//==========================================================================================================
	// 메세지 표현
	//==========================================================================================================
	cPrintMsg::PrintMsg( 14001, pQuestFT->m_szTitleText );
	switch( pQuestFT->GetUniqID() )
	{
	case 525:		cDigimonTalk::Print( 40000 );									break;
	default:		g_pDataMng->GetQuestOwner()->Check_DigimonTalk( false );		break;
	}

	if( pQuestFT->IsQuestType(CsQuest::QT_TUTORIAL) )
	{
		DWORD dwQuestID = pQuestFT->GetUniqID();
		GAME_EVENT_ST.OnEvent( EVENT_CODE::TUTORIALQUEST_COMPLETE, &dwQuestID );
	}

	return true;
}
void cData_Quest::QuestReLoad()
{
	SAFE_POINTER_RET(nsCsFileTable::g_pQuestMng);

	CTamerUser* pUser =	g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET(pUser);

	if( net::game )
	{		
		for( int i=0; i<nQuest::MaxExecute; ++i )
		{
			nQuest::sExeInfo ei = m_Info.GetExeInfo( i );
			if( ei.m_nIDX == 0 )
				continue;	

			CsQuest* pQuestInfo = nsCsFileTable::g_pQuestMng->GetQuest( ei.m_nIDX );
			if( NULL == pQuestInfo )// 퀘스트 목록에 없는 퀘스트를 가지고 있다면
			{
				net::game->SendQuestCancel( ei.m_nIDX );
				m_Info.Cancel( ei.m_nIDX );
			}
			else if( pQuestInfo->IsImmediate() )// 즉시 완료 퀘인데 가지고 있다면
			{
				// 캔슬
				net::game->SendQuestCancel( ei.m_nIDX );
				m_Info.Cancel( ei.m_nIDX );
			}
// 			else if( pQuestInfo->IsQuestType(CsQuest::QT_TUTORIAL) )
// 			{
// 				if( !pUser->IsServerOption( CTamerUser::SO_DO_TUTORIAL ) )
// 				{
//					// 캔슬
// 					net::game->SendQuestCancel( ei.m_nIDX );
// 					m_Info.Cancel( ei.m_nIDX );
//				}
//			}
		}
	}

	// 기존 진행 퀘스트 날려 주자
	_DeleteProcQuest();

	SAFE_POINTER_RET(g_pDataMng);

	g_pDataMng->GetQuestOwner()->LoadMap();

	CsQuest::MAP* pMap = nsCsFileTable::g_pQuestMng->GetQuestMap();
	if( pMap )
	{
		CsQuest::MAP_IT it = pMap->begin();
		for( ; it != pMap->end(); ++it )
		{
			SAFE_POINTER_CON( it->second );

			// 업적 퀘스트라면 패스
			if( it->second->IsQuestType( CsQuest::QT_ACHIEVE ) )
				continue;

			PostRevAddQuest( it->second );
		}
	}

	g_pDataMng->GetQuestOwner()->Check_DigimonTalk( true );

	g_pNpcMng->CheckQuestExtra();  //NPC초기화

	if( g_pDataMng->GetQuestOwner()->IF_DigiviceQuest_Exist() )
		g_pDataMng->GetQuestOwner()->IF_DigiviceQuest_Open();
}

bool cData_Quest::IsJointProgressQuest( DWORD dwQuestID )
{
	switch(dwQuestID)
	{
	case 3000:
	case 3001:
	case 3002:
		return true;
	}
	return false;
}

void cData_Quest::_CompQuest_PostCheck( CsQuest* pQuestFT )
{
#ifndef VERSION_USA
	switch( pQuestFT->GetUniqID() )
	{
	case 2746:			// 편지 보내기 이벤트
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_LETTER );
		break;
	}
#endif
}

//===========================================================================
//
//		부가
//
//===========================================================================
NiColor cData_Quest::GetTitleColor( CsQuest* pQuestFT )
{
	int nTLevel = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();
	int nDelta = pQuestFT->GetLevel() - nTLevel;

	switch( nDelta )
	{
	case 4:
	case 3:
		return NiColor( 255/255.0f, 169/255.0f, 0/255.0f );

	case 2:
	case 1:
	case 0:
	case -1:
	case -2:
	case -3:
	case -4:
	case -5:
		return NiColor( 242/255.0f, 224/255.0f, 24/255.0f );

	case -6:
	case -7:
	case -8:
	case -9:
	case -10:
		return NiColor( 90/255.0f, 191/255.0f, 90/255.0f );

	case -11:
	case -12:
	case -13:
		return NiColor( 96/255.0f, 148/255.0f, 170/255.0f );
	
	case -14:
	case -15:
		return NiColor( 115/255.0f, 138/255.0f, 148/255.0f );
	}

	if( nDelta > 4 )
	{
		return NiColor( 255/255.0f, 0/255.0f, 0/255.0f );
	}

	// 가장 차이 많이 날때
	return NiColor( 123/255.0f, 123/255.0f, 123/255.0f );
}

// int cData_Quest::GetQuestExp( CsQuest* pQuestFT, int nExp )
// {
// 	int nTLevel = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();
// 	int nDelta = pQuestFT->GetLevel() - nTLevel;
// 
// #ifdef EXP_UNLIMITED
// 	if( nDelta >= -98 )		// -15 -> -98로 변경 13.01.10 lks007
// #else
// 	if( nDelta >= -15 )
// #endif
// 	{
// 		return nExp;
// 	}
// 
// 	return 0;

	//switch( nDelta )
	//{
	//case -6:
	//case -7:
	//case -8:
	//case -9:
	//case -10:
	//	return CsFloat2Int( nExp*0.8f );

	//case -11:
	//	return CsFloat2Int( nExp*0.7f );

	//case -12:
	//	return CsFloat2Int( nExp*0.6f );

	//case -13:
	//	return CsFloat2Int( nExp*0.5f );

	//case -14:
	//	return CsFloat2Int( nExp*0.4f );

	//case -15:
	//	return CsFloat2Int( nExp*0.3f );
	//}

	//// 가장 차이 많이 날때
	//if( nDelta < -15 )
	//	return 0;

	//return nExp;
//}

void cData_Quest::ResetDailyQuest()
{
	CsQuest::MAP* pMap = nsCsFileTable::g_pQuestMng->GetQuestMap();
	CsQuest::MAP_IT it = pMap->begin();
	CsQuest::MAP_IT itEnd = pMap->end();
	cData_QuestOwner* pOwner = g_pDataMng->GetQuestOwner();

	bool bCheck = false;
	for( ; it!=itEnd; ++it )
	{
		// 이미 완료한 퀘스트 인지 체크
		switch( m_Info.Check( it->second->GetUniqID() ) )
		{
			// 이미 완료 했다면 패스
		case nQuest::Complete:
			{				
				if( it->second->GetQuestType() == CsQuest::QT_DAILY )
				{
					bCheck = true;

					m_Info.Clear( it->second->GetUniqID() );
					pOwner->AddQuest_New( GetInfo(), it->second );
				}
			}
			break;
			
		case nQuest::Execute:		// 수행중
		case nQuest::None:			// 아직 안받음		
			break;

		default:
			assert_cs( false );
		}				
	}

	if( bCheck == true )
	{
		pOwner->Check_DigimonTalk( false );
	}
}


cData_Quest::sPROCESS* cData_Quest::GetProcess( DWORD dwFTID )
{ 
	assert_cs( m_mapProcQuest.find( dwFTID ) != m_mapProcQuest.end() ); 

	if( m_mapProcQuest.find(dwFTID) == m_mapProcQuest.end() )
		return NULL;

	return m_mapProcQuest[ dwFTID ]; 
}

bool cData_Quest::IsProcess( DWORD dwFTID ) const
{ 
	return ( m_mapProcQuest.find( dwFTID ) != m_mapProcQuest.end() ); 
}

bool cData_Quest::IsCompleate( DWORD dwFTID )// 수행중 체크 안들어 가기 때문에 CheckAchieve 사용
{ 
	return ( m_Info.CheckAchieve( dwFTID ) == nQuest::Complete ); 
}

bool cData_Quest::IsNoAccept( DWORD dwFTID ) // 아직 받은 지은 퀘스트 체크
{ 
	return ( m_Info.Check( dwFTID ) == nQuest::None ); 
}

void cData_Quest::UpdateQuestCondition( DWORD const& dwQuestID, int const& nDepth, int const& nValue )
{
	if( nQuest::MaxCondition <= nDepth )
		return;

	nQuest::sExeInfo* pExeInfo = m_Info.GetExeInfo_ByID((u2)dwQuestID);
	SAFE_POINTER_RET( pExeInfo );

	pExeInfo->m_nCondition[ nDepth ] = (u1)nValue;
	CalProcess();

	if( nsCsFileTable::g_pQuestMng )
	{
		CsQuest* pFT = nsCsFileTable::g_pQuestMng->GetQuest( dwQuestID );
		if( pFT && pFT->IsQuestType(CsQuest::QT_TUTORIAL) )
		{
			if( IsProcessQuestAllComplete( dwQuestID ) )// 현재 진행 중인 퀘스트가 모두 완료 인경우
			{
				DWORD dwAllCompleteQuestID = dwQuestID;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::TUTORIALQUEST_PROCESS_ALL_COMPLETE, &dwAllCompleteQuestID );
			}
			else
			{
				ContentsStream kTmp;
				kTmp << dwQuestID << nDepth << nValue;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::TUTORIALQUEST_PROCESS_UPDATE, &kTmp );
			}
		}
	}	
}

bool cData_Quest::IsProcessQuestAllComplete( DWORD const& dwQuestID )
{
	std::map< DWORD, sPROCESS* >::iterator it = m_mapProcQuest.find( dwQuestID );
	if( it == m_mapProcQuest.end() )
		return false;
	SAFE_POINTER_RETVAL( it->second, false );
	return it->second->s_CurCompRequireGroup.IsCompleate();
}