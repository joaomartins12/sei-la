

#include "stdafx.h"
#include "Data_QuestOwner.h"





//=======================================================================================================
//
//	Digivice
//
//=======================================================================================================

void cData_QuestOwner::sDigiviceOwner::Delete()
{
	std::map< DWORD, sQuestInfo* >::iterator it = s_mapQuest.begin();
	std::map< DWORD, sQuestInfo* >::iterator itEnd = s_mapQuest.end();
	for( ; it!=itEnd; ++it )
	{
		NiDelete it->second;
	}
	s_mapQuest.clear();	
}

void cData_QuestOwner::sDigiviceOwner::DeleteQuest( CsQuest* pFT )
{
	std::map< DWORD, sQuestInfo* >::iterator it = s_mapQuest.begin();
	std::map< DWORD, sQuestInfo* >::iterator itEnd = s_mapQuest.end();
	for( ; it!=itEnd; ++it )
	{
		if( it->second->s_pFTQuest == pFT )
		{
			NiDelete it->second;
			s_mapQuest.erase( it );
			return;
		}
	}
}

void cData_QuestOwner::sDigiviceOwner::Init( bool bUseDigiviceIF )
{
	s_bUseDigiviceIF = bUseDigiviceIF;
}

void cData_QuestOwner::sDigiviceOwner::Add_New( nQuest::cInfo* pOrgInfo, CsQuest* pFT, cData_Quest::sPROCESS* pProcess )
{
	sQuestInfo* pInfo = NiNew sQuestInfo;
	pInfo->s_pFTQuest = pFT;
	pInfo->s_pProcess = pProcess;	

	assert_cs( s_mapQuest.find( pFT->GetUniqID() ) == s_mapQuest.end() );
	s_mapQuest[ pFT->GetUniqID() ] = pInfo;

	if( s_bUseDigiviceIF == true )
	{
		g_pDataMng->GetQuestOwner()->IF_DigiviceQuest_Open();

		// 튜토리얼이라면
		if( pFT->IsQuestType(CsQuest::QT_TUTORIAL) )
		{
			DWORD dwQuestID = pFT->GetUniqID();
			GAME_EVENT_ST.OnEvent( EVENT_CODE::TUTORIALQUEST_DIGIVICE_CHECK, &dwQuestID);
		}
	}
}

void cData_QuestOwner::sDigiviceOwner::Add_Pointer( sDigiviceOwner::sQuestInfo* pQuestInfo )
{
	assert_cs( s_mapQuest.find( pQuestInfo->s_pFTQuest->GetUniqID() ) == s_mapQuest.end() );
	s_mapQuest[ pQuestInfo->s_pFTQuest->GetUniqID() ] = pQuestInfo;

	if( s_bUseDigiviceIF == true )
	{
		g_pDataMng->GetQuestOwner()->IF_DigiviceQuest_Open();
	}
}
