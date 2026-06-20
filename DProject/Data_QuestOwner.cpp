
#include "stdafx.h"
#include "Data_QuestOwner.h"

//=======================================================================================================
//
//	cData_QuestOwner
//
//=======================================================================================================
void cData_QuestOwner::Delete()
{
	QuestOwnerCont::iterator it = m_mapNpcOwner.begin();
	for( ; it != m_mapNpcOwner.end(); ++it )
	{
		SAFE_POINTER_CON(it->second);
		it->second->Delete();
		NISAFE_DELETE( it->second );
	}
	m_mapNpcOwner.clear();

	m_ProcDigiviceOwner.Delete();
	m_ReadyRevDigiviceOwner.Delete();
	m_CheckDigiviceOwner.Delete();
	m_HighLevelDigiviceOwner.Delete();
}

void cData_QuestOwner::Init()
{
	m_mapNpcOwner.clear();
	m_ProcDigiviceOwner.Init( false );
	m_ReadyRevDigiviceOwner.Init( true );
	m_CheckDigiviceOwner.Init( false );
	m_HighLevelDigiviceOwner.Init( false );
}

void cData_QuestOwner::LoadMap()
{
	// 벡터 링크 리셋
	QuestOwnerCont::iterator it = m_mapNpcOwner.begin();
	for( ; it != m_mapNpcOwner.end(); ++it )
	{
		SAFE_POINTER_CON(it->second);
		it->second->Delete();
		NISAFE_DELETE( it->second );
	}
	m_mapNpcOwner.clear();
	m_ProcDigiviceOwner.Delete();
	m_ReadyRevDigiviceOwner.Delete();
	m_CheckDigiviceOwner.Delete();
	m_HighLevelDigiviceOwner.Delete();
}

void cData_QuestOwner::Update()
{
	// 받아야 할 퀘가 있다면
	if( ( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )||( IF_DigiviceQuest_Exist() == false ) )
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DIGIVICEQUEST );
	}
}

void cData_QuestOwner::AddQuest_New( nQuest::cInfo* pOrgInfo, CsQuest* pFT )
{
	DWORD dwSTID = pFT->GetStartTarget_ID();

	// 시작대상
	switch( pFT->GetStartTarget_Type() )
	{
	case CsQuest::ST_NPC:
		{
			// 엔피씨가 존재 한다면
			if( g_pNpcMng->IsNpc( dwSTID ) )
			{
				_GetNpcOwner( dwSTID )->Add_New( pOrgInfo, pFT );
			}
		}
		break;
	case CsQuest::ST_DIGIVICE:
		{
			// 레벨 + 3 까지는 가능-*
			int nTamerLevel = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel() + 4;
			if( pFT->GetLevel() < nTamerLevel )
			{
				if( cData_Quest::CheckPreRequire( pFT ) == true )
				{
					m_ReadyRevDigiviceOwner.Add_New( pOrgInfo, pFT, NULL );					
				}
				else
				{
					m_CheckDigiviceOwner.Add_New( pOrgInfo, pFT, NULL );
				}
			}
			else
			{
				m_HighLevelDigiviceOwner.Add_New( pOrgInfo, pFT, NULL );
			}
		}
		break;
	default:
		assert_cs( false );
	}
}

void cData_QuestOwner::AddQuest_Process( nQuest::cInfo* pOrgInfo, CsQuest* pFT, cData_Quest::sPROCESS* pProcess )
{
	SAFE_POINTER_RET( pOrgInfo );
	SAFE_POINTER_RET( pFT );

	// 시작 대상
	DWORD dwSTID = pFT->GetStartTarget_ID();

	switch( pFT->GetStartTarget_Type() )
	{
	case CsQuest::ST_NPC:
		{
			// 완료 대상
			switch( pFT->GetQuestTarget() )
			{
			case CsQuest::QTG_SELF:
				if( g_pNpcMng->IsNpc( pFT->GetStartTarget_ID() ) )
				{
					_GetNpcOwner( pFT->GetStartTarget_ID() )->Add_Process( pOrgInfo, pFT, pProcess );
				}
				break;
			case CsQuest::QTG_NPC:
				if( g_pNpcMng->IsNpc( pFT->GetQuestTargetValue1() ) )
				{
					_GetNpcOwner( pFT->GetQuestTargetValue1() )->Add_Process( pOrgInfo, pFT, pProcess );
				}
				break;
			case CsQuest::QTG_COMPLETE:
				m_ProcDigiviceOwner.Add_New( pOrgInfo, pFT, pProcess );
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	case CsQuest::ST_DIGIVICE:
		{
			// 완료 대상
			switch( pFT->GetQuestTarget() )
			{
			case CsQuest::QTG_COMPLETE:
			case CsQuest::QTG_SELF:
				m_ProcDigiviceOwner.Add_New( pOrgInfo, pFT, pProcess );
				break;
			case CsQuest::QTG_NPC:
				if( g_pNpcMng->IsNpc( pFT->GetQuestTargetValue1() ) )
				{
					_GetNpcOwner( pFT->GetQuestTargetValue1() )->Add_Process( pOrgInfo, pFT, pProcess );
				}
				break;
			default:
				assert_cs( false );
			}			
		}
		break;
	default:
		assert_cs( false );
	}

	// 완료 조건	
	CsQuestCompRequire::LIST* pCRList = pProcess->s_CurCompRequireGroup.GetList();
	CsQuestCompRequire::LIST_IT itCR = pCRList->begin();
	CsQuestCompRequire::LIST_IT itEndCR = pCRList->end();
	CsQuestCompRequire* pData;
	for( ; itCR != itEndCR; ++itCR )
	{
		pData = *itCR;

		if( pData->IsCompleat() == true )
			continue;

		if( pData->GetType() == CsQuestCompRequire::NPCTALK )
		{
			if( g_pNpcMng->IsNpc( pData->GetTypeID() ) )
			{
				g_pNpcMng->GetNpc( pData->GetTypeID() )->SetQuestViewType( CNpc::QV_NPCTALK );
			}
		}
	}	


}

// 테이머 레벨업 하는 경우 퀘스트 완료 하는경우에 호출됨
void cData_QuestOwner::RevQuestCheck( nQuest::cInfo* pOrgInfo )
{
	// 엔피씨
	{
		QuestOwnerCont::iterator it = m_mapNpcOwner.begin();
		for( ; it != m_mapNpcOwner.end(); ++it )
		{
			SAFE_POINTER_CON(it->second);
			if( it->second->CheckRevType( pOrgInfo ) )
				it->second->CalDisplayRev();
		}
	}	

	// 디지바이스 - 하이레벨만 체크
	{
		// 레벨 + 3 까지는 가능-*
		int nTamerLevel = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel() + 4;

		std::map< DWORD, sDigiviceOwner::sQuestInfo* >* pMap = m_HighLevelDigiviceOwner.GetMap();
		std::map< DWORD, sDigiviceOwner::sQuestInfo* >::iterator it = pMap->begin();
		std::map< DWORD, sDigiviceOwner::sQuestInfo* >::iterator itEnd = pMap->end();
		std::map< DWORD, sDigiviceOwner::sQuestInfo* >::iterator itNext = it;
		CsQuest* pFTQuest;
		while( itNext!=itEnd )
		{
			it = itNext;
			++itNext;

			pFTQuest = it->second->s_pFTQuest;
			if( pFTQuest->GetLevel() < nTamerLevel )
			{
				m_CheckDigiviceOwner.Add_Pointer( it->second );
				pMap->erase( it );
			}
		}
	}
}

bool cData_QuestOwner::IF_DigiviceQuest_Exist()
{
	// 받아야 할 퀘가 있다면
	if( m_ReadyRevDigiviceOwner.GetMap()->empty() == false )
	{
		return true;
	}

	std::map< DWORD, sDigiviceOwner::sQuestInfo* >* pMap = m_ProcDigiviceOwner.GetMap();
	std::map< DWORD, sDigiviceOwner::sQuestInfo* >::iterator it = pMap->begin();
	std::map< DWORD, sDigiviceOwner::sQuestInfo* >::iterator itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		if( it->second->s_pProcess->s_bCompleate == true )
		{
			return true;
		}
	}

	return false;
}

void cData_QuestOwner::IF_DigiviceQuest_Open()
{
	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
		return;

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGIVICEQUEST ) == false )
	{
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_DIGIVICEQUEST,0,0,false );
	}
	else
	{
		g_pGameIF->GetDigiviceQuest()->RestartSpriteAni();
	}
}

cData_QuestOwner::sNpcOwner*
cData_QuestOwner::_GetNpcOwner( DWORD const& dwNpcFTID )
{
	int nIDX = dwNpcFTID%NPC_FT_MASK;
	QuestOwnerCont::iterator it = m_mapNpcOwner.find( nIDX );
	if( it == m_mapNpcOwner.end() )
	{
		sNpcOwner* pOwner = NiNew sNpcOwner;
		pOwner->Init();
		g_pNpcMng->GetNpc( nIDX )->SetQuestOwner( pOwner );

		auto rib = m_mapNpcOwner.insert( std::make_pair( nIDX, pOwner ) );
		it = rib.first;
	}

	return it->second;
}

void cData_QuestOwner::Check_DigimonTalk( bool const& bPostLoad )
{
	DWORD dwPlag = sNpcOwner::ENABLE_REV_MAIN | sNpcOwner::ENABLE_REV_SUB | sNpcOwner::ENABLE_REV_REPEAT | sNpcOwner::ENABLE_REV_EVENTREPEAT;

	QuestOwnerCont::iterator it = m_mapNpcOwner.begin();
	for( ; it != m_mapNpcOwner.end(); ++it )
	{
		sNpcOwner*  pNpcOwner = it->second;
		SAFE_POINTER_CON(pNpcOwner);
		if( !pNpcOwner->IsHaveType( dwPlag ) )
			continue;

		cDigimonTalk::sTalkEle sEle;
		sEle.s_dwNpc = it->first+NPC_FT_MASK;

		if( bPostLoad == true )
			cDigimonTalk::Print( 50006, &sEle );
		else
			cDigimonTalk::Print( 50007, &sEle );
		break;
	}
}

