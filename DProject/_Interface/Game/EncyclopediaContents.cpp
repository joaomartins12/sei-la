#include "StdAfx.h"
#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "EncyclopediaContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

EncyclopediaContents::sINFO::sINFO()
{ 
	s_ImgFileName.clear();
}

EncyclopediaContents::sEVOL_INFO::sEVOL_INFO()
{
	s_nGroup = 0;				
	s_nOrder = 0;				
	s_nSize = 0;				
	s_nEnchant = 0;				
	s_nEnchant_AT = 0;			
	s_nEnchant_BL = 0;			
	s_nEnchant_CT = 0;			
	s_nEnchant_EV = 0;			
	s_nEnchant_HP = 0;
	s_nLevel = 0;				
	s_nCount = 0;				
	s_bIsAllOpen = false;		
	s_bIsReward = false;		
}

EncyclopediaContents::sDATA::sDATA()
{
	s_ImgFileName.clear();
}

int const EncyclopediaContents::IsContentsIdentity(void)
{
	return E_CT_ENCYCLOPEDIA_CONTENTS;
}

EncyclopediaContents::EncyclopediaContents(void)
{	
	mEncyInfoMap.clear();
	mGroupStr.clear();
	mGroupInfoMap.clear();

	mUseDeck = 0;
	iDeckIdx = 0;
	mbIsRecv = false;

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ENCYCLOPEDIA_OPEN_DATA,		this, &EncyclopediaContents::Recv_OpenData );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ENCYCLOPEDIA_USE_DECK,		this, &EncyclopediaContents::Recv_UseDeck );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ENCYCLOPEDIA_USE_GETDECK,	this, &EncyclopediaContents::Recv_GetDeck );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ENCYCLOPEDIA_SET_EVOLINFO,	this, &EncyclopediaContents::Recv_SetEvolInfo );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ENCYCLOPEDIA_RECV_SERVER,	this, &EncyclopediaContents::Recv_Server_Encyclopedia );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ENCYCLOPEDIA_INCHANTRESULT, this, &EncyclopediaContents::Recv_InchantResult );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ENCYCLOPEDIA_GROUPINFOCON,  this, &EncyclopediaContents::Recv_GroupInfoCondition );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ENCYCLOPEDIA_LV_CHECK,		this, &EncyclopediaContents::Recv_LevelCheckUpdate );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ENCYCLOPEDIA_EVOL_UPDATE,	this, &EncyclopediaContents::Recv_EvolutionUpdate );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ENCYCLOPEDIA_BISREWARD,		this, &EncyclopediaContents::Recv_BisReward );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ENCYCLOPEDIA_SCALE,			this, &EncyclopediaContents::Recv_Scale );	
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::ENCYCLOPEDIA_LOGOUT,		this, &EncyclopediaContents::Recv_LogOut);

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::GET_ENCYCLOPEDIA_USE_DECK,	this, &EncyclopediaContents::Get_UseDeck );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::GET_ENCYCLOPEDIA_BISRECV,	this, &EncyclopediaContents::Get_BisRecv );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::GET_ENCYCLOPEDIA_NULL_CHECK,this, &EncyclopediaContents::Get_GroupInfoNullCheck );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::GET_ENCYCLOPEDIA_CONDITION, this, &EncyclopediaContents::Get_GroupInfoCondition );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::GET_ENCYCLOPEDIA_NATTYPE,   this, &EncyclopediaContents::Get_GroupInfoNatType );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::GET_ENCYCLOPEDIA_PROB,		this, &EncyclopediaContents::Get_GroupInfoProb );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::GET_ENCYCLOPEDIA_OPTION,	this, &EncyclopediaContents::Get_GroupInfoOption );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::GET_ENCYCLOPEDIA_VAL,		this, &EncyclopediaContents::Get_GroupInfoVal );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::GET_ENCYCLOPEDIA_TIME,		this, &EncyclopediaContents::Get_GroupInfoTime );
}

EncyclopediaContents::~EncyclopediaContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);
}

int const EncyclopediaContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

void EncyclopediaContents::Recv_OpenData(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ENCYCLOPEDIA_OPEN* pRecv = static_cast<GS2C_RECV_ENCYCLOPEDIA_OPEN*>(pData);	

	std::list<sEncyclopediaOpendedData>::const_iterator it = pRecv->m_listOpendedData.begin();
	for( ; it != pRecv->m_listOpendedData.end(); ++it )
	{		
		EncyclopediaContents::sEVOL_INFO* pEvolInfo = GetEncyclopediaEvolInfo( (*it).nDigimonID );
		SAFE_POINTER_CON( pEvolInfo );

		int nOpenCnt = 0;				// 계열체 내 진화 슬롯 오픈 갯수

		pEvolInfo->s_nLevel = (u1)(*it).nLevel;	
#ifdef SDM_DIGIMONBOOK_EXTEND_20181126
		for( int n = 1; n < nLimit::EvoUnit; ++n )
		{
			if( pEvolInfo->s_sInfo[ n ].s_nDigimonID == -1 	|| 
				pEvolInfo->s_sInfo[ n ].s_eImgState == EncyclopediaContents::sINFO::S_NONE )
			{
				continue;
			}

			if( (*it).nSlotOpened & (1 << (n - 1)) )
			{
				pEvolInfo->s_sInfo[ n ].s_eImgState = EncyclopediaContents::sINFO::S_OPEN;
				nOpenCnt++;
			}
			else
				pEvolInfo->s_sInfo[ n ].s_eImgState = EncyclopediaContents::sINFO::S_CLOSE;
		}
#else
		for( int i = 1 ; i < 9 ; i++ )	//8번 pop, 인덱스는 1부터 사용
		{
			if( pEvolInfo->s_sInfo[ i ].s_nDigimonID == -1 	|| 
				pEvolInfo->s_sInfo[ i ].s_eImgState == EncyclopediaContents::sINFO::S_NONE )
			{
				continue;
			}

			if( (*it).nIsOpen[i] != 0 )	
			{
				pEvolInfo->s_sInfo[ i ].s_eImgState = EncyclopediaContents::sINFO::S_OPEN;
				nOpenCnt++;		
			}
			else	
			{
				pEvolInfo->s_sInfo[ i ].s_eImgState = EncyclopediaContents::sINFO::S_CLOSE;
			}
		}
#endif
		if( nOpenCnt == pEvolInfo->s_nCount )	// 오픈된 갯수와 현재 계열체에서 사용중인 디지몬 수가 같다면( 모두 오픈한 상태)
			pEvolInfo->s_bIsAllOpen = true;

		pEvolInfo->s_nEnchant_AT = (u1)(*it).nEnchant_AT;
		pEvolInfo->s_nEnchant_CT = (u1)(*it).nEnchant_CT;
		pEvolInfo->s_nEnchant_BL = (u1)(*it).nEnchant_BL;
		pEvolInfo->s_nEnchant_EV = (u1)(*it).nEnchant_EV;
		pEvolInfo->s_nEnchant_HP = (u1)(*it).nEnchant_HP;

		pEvolInfo->s_nEnchant = GetEnchant_Val( (*it).nEnchant_AT, (*it).nEnchant_CT, (*it).nEnchant_BL, (*it).nEnchant_EV, (*it).nEnchant_HP );	// 강화값으로 별 갯수 얻어옴

		pEvolInfo->s_nSize = (*it).nSize / 100;

		// 아이템 획득 전/후 셋팅
		pEvolInfo->s_bIsReward = (0 == (*it).bIsReward)?false:true;		
	}
}

void EncyclopediaContents::Recv_UseDeck(void* pData)
{
	SetEncyInfoMap();		
	SetGroupInfoMap();

	SAFE_POINTER_RET(pData);
	GS2C_RECV_ENCYCLOPEDIA_USEDECK* pRecv = static_cast<GS2C_RECV_ENCYCLOPEDIA_USEDECK*>(pData);

	SetUseDeck(pRecv->iDeckIdx, pRecv->bUse);
}

void EncyclopediaContents::Recv_GetDeck(void* pData)
{
	SetEncyInfoMap();		
	SetGroupInfoMap();

	SAFE_POINTER_RET(pData);
	GS2C_RECV_ENCYCLOPEDIA_USEDECK* pRecv = static_cast<GS2C_RECV_ENCYCLOPEDIA_USEDECK*>(pData);

	iDeckIdx = pRecv->iDeckIdx;
}

void EncyclopediaContents::Recv_SetEvolInfo(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ENCYCLOPEDIA_EVOLINFO* pRecv = static_cast<GS2C_RECV_ENCYCLOPEDIA_EVOLINFO*>(pData);

	EncyclopediaContents::sEVOL_INFO* pEvolInfo = GetEncyclopediaEvolInfo( pRecv->iBaseDigimonID );
	if( pEvolInfo )
	{
		if( pEvolInfo->s_sInfo[ pRecv->iEvolSlot ].s_eImgState == EncyclopediaContents::sINFO::S_CLOSE )
			pEvolInfo->s_sInfo[ pRecv->iEvolSlot ].s_eImgState = EncyclopediaContents::sINFO::S_OPEN;

		//원래 닫혀있던 녀석이니  전체 오픈 여부 확인
		int nCnt = pEvolInfo->s_nCount;	//계열체 수 - 오픈 수로 전체 오픈되었는지 확인할거야

		for( int i = 0 ; i < nLimit::EvoUnit ; i++ ) // 디지몬 진화슬롯은 1번부터~
		{
			if( pEvolInfo->s_sInfo[ i ].s_nDigimonID == -1 || pEvolInfo->s_sInfo[ i ].s_eImgState == EncyclopediaContents::sINFO::S_NONE )	// 3
				continue;

			if( pEvolInfo->s_sInfo[ i ].s_eImgState == EncyclopediaContents::sINFO::S_OPEN )
				nCnt--;	//카운트 1감소, 카운트 0이면 모두 오픈된 상태임
		}

		if( nCnt <= 0 )
			pEvolInfo->s_bIsAllOpen = true;

		Notify(ENCYCLOPEDIA_REFRESHLIST);
	}
}

void EncyclopediaContents::Recv_Server_Encyclopedia(void* pData)
{
	mbIsRecv = true;
}

void EncyclopediaContents::Recv_InchantResult(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ENCYCLOPEDIA_INCHANTRESULT* pRecv = static_cast<GS2C_RECV_ENCYCLOPEDIA_INCHANTRESULT*>(pData);

	EncyclopediaContents::sEVOL_INFO* pEncyData = mEncyInfoMap[ pRecv->nDigimonID ];

	//비교해보고 갱신 여부 리턴
	bool bChange = CompEnchant_Val( pRecv->nDigimonID, pRecv->nAPLevel, pRecv->nCALevel, pRecv->nEVLevel, pRecv->nDPLevel, pRecv->nHPLevel );
	if( bChange )//true일때 갱신
	{
		// 별 정보 갱신
		pEncyData->s_nEnchant = GetEnchant_Val( pRecv->nAPLevel, pRecv->nCALevel, pRecv->nEVLevel, pRecv->nDPLevel, pRecv->nHPLevel );

		// 강화수치 갱신
		pEncyData->s_nEnchant_AT = (u1)pRecv->nAPLevel;
		pEncyData->s_nEnchant_CT = (u1)pRecv->nCALevel;
		pEncyData->s_nEnchant_EV = (u1)pRecv->nEVLevel;
		pEncyData->s_nEnchant_BL = (u1)pRecv->nDPLevel;
		pEncyData->s_nEnchant_HP = (u1)pRecv->nHPLevel;

		Notify(ENCYCLOPEDIA_REFRESHLIST);
	}

}

void EncyclopediaContents::Recv_GroupInfoCondition(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ENCYCLOPEDIA_OPTIDX* pRecv = static_cast<GS2C_RECV_ENCYCLOPEDIA_OPTIDX*>(pData);

	EncyclopediaContents::sGROUP_INFO* pGroupInfo = mGroupInfoMap[ mUseDeck ];

	switch( pGroupInfo->s_nCondition[pRecv->nOptIdx] )
	{
	case EncyclopediaContents::COND_ACTIVE:		//확률 발동(쿨타임 초기화)
		{

		}
		break;	
	case EncyclopediaContents::COND_ACTIVE_TIME:	//확률 발동	- 일정시간 효과
		{

		}
		break;
	default:
		{
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			//CProtocol.m_wStr = GetVAString(_T("Type Err - UsingDeck:%d, ConditionType:%d"), mUseDeck, pGroupInfo->s_nCondition[pRecv->nOptIdx]);
			DmCS::StringFn::Format(CProtocol.m_wStr, _T("Type Err - UsingDeck:%d, ConditionType:%d"), mUseDeck, pGroupInfo->s_nCondition[pRecv->nOptIdx]);
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		}
		break;
	}
}

void EncyclopediaContents::Recv_LevelCheckUpdate(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ENCYCLOPEDIA_TACTICSHOUSE* pRecv = static_cast<GS2C_RECV_ENCYCLOPEDIA_TACTICSHOUSE*>(pData);

	//레벨이 더 크면 도감 정보 갱신
	if( mEncyInfoMap[ pRecv->nDigimonID ]->s_nLevel < pRecv->nLevel )
	{
		mEncyInfoMap[ pRecv->nDigimonID ]->s_nLevel = pRecv->nLevel;

		Notify(ENCYCLOPEDIA_REFRESHLIST);
	}
}

void EncyclopediaContents::Recv_EvolutionUpdate(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ENCYCLOPEDIA_EVOLUTION* pRecv = static_cast<GS2C_RECV_ENCYCLOPEDIA_EVOLUTION*>(pData);

	EncyclopediaContents::sEVOL_INFO* pEncyData = mEncyInfoMap[ pRecv->nDigimonID ];

	int nOpenCnt = 0;		// 오픈된 디지몬 수 확인, 모두 오픈되었는지 확인

	// 오픈상태 갱신
	for( int i = 1 ; i < nLimit::EvoUnit ; i++ )
	{
		// 안쓰는 슬롯은 무시
		if( pEncyData->s_sInfo[ i ].s_nDigimonID == -1 || pEncyData->s_sInfo[ i ].s_eImgState == EncyclopediaContents::sINFO::S_NONE )
			continue;

		if(pRecv->nType == 0)
		{
			if( pRecv->nEvoUnit[ i ].m_nSlotState == 1 )	//오픈상태이면
			{
				pEncyData->s_sInfo[ i ].s_eImgState = EncyclopediaContents::sINFO::S_OPEN;
				nOpenCnt++;									//오픈디지몬 수 ++
			}
		}
		else
		{
			pEncyData->s_sInfo[ i ].s_eImgState = EncyclopediaContents::sINFO::S_OPEN;
			nOpenCnt++;										//오픈디지몬 수 ++	
		}		
	}
	if( nOpenCnt == pEncyData->s_nCount )					//모두 오픈된 상태면 
		pEncyData->s_bIsAllOpen = true;						//오픈상태 셋

	if(pRecv->nType == 0)
	{
		if( pEncyData->s_nSize < ( pRecv->nScale / 100 ) )	//스케일이 클때만 갱신
			pEncyData->s_nSize = pRecv->nScale / 100;
	}

	if( pEncyData->s_nLevel == 0 )						//레벨이 0일때는 1로 갱신
		pEncyData->s_nLevel = 1;

	Notify(ENCYCLOPEDIA_REFRESHLIST);
}

void EncyclopediaContents::Recv_BisReward(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ENCYCLOPEDIA_BISREWARD* pRecv = static_cast<GS2C_RECV_ENCYCLOPEDIA_BISREWARD*>(pData);

	mEncyInfoMap[ pRecv->nDigimonID ]->s_bIsReward = true;

	Notify(ENCYCLOPEDIA_REFRESHLIST);
}

void EncyclopediaContents::Recv_Scale(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ENCYCLOPEDIA_SCALE* pRecv = static_cast<GS2C_RECV_ENCYCLOPEDIA_SCALE*>(pData);

	//저장되있는 크기보다 커졋다면 ( 소수점은 버림 ) 갱신
	if( mEncyInfoMap[pRecv->nDigimonID]->s_nSize < ( pRecv->nScale / 100 ) )
	{
		mEncyInfoMap[pRecv->nDigimonID]->s_nSize = pRecv->nScale / 100;

		Notify(ENCYCLOPEDIA_REFRESHLIST);
	}
}

void EncyclopediaContents::Recv_LogOut(void* pData)
{
	mbIsRecv = false;

	//도감 정보 리셋
	MAP_IT it = mEncyInfoMap.begin();
	MAP_IT itEnd = mEncyInfoMap.end();
	for( ; it != itEnd ; it++)
	{
		for( int i = 1 ; i<nLimit::EvoUnit ; ++i )
		{
			if( it->second->s_sInfo[ i ].s_eImgState == sINFO::S_NONE )
				continue;
			//이미지는 닫힌 상태로 바꾸고
			it->second->s_sInfo[ i ].s_eImgState = sINFO::S_CLOSE;
			//디지몬 계열체 정보들 0으로
		}
		it->second->s_nSize = 0;
		it->second->s_nEnchant = 0;
		it->second->s_nEnchant_AT = 0;
		it->second->s_nEnchant_CT = 0;
		it->second->s_nEnchant_BL = 0;
		it->second->s_nEnchant_EV = 0;
		it->second->s_nEnchant_HP = 0;
		it->second->s_nLevel = 0;
		it->second->s_bIsAllOpen = false;
		it->second->s_bIsReward = false;
	}

	//덱 정보 리셋
	MAP_GROUP_IT it2 = mGroupInfoMap.begin();
	MAP_GROUP_IT itEnd2 = mGroupInfoMap.end();
	for( ; it2 != itEnd2 ; ++it2)
	{
		MAP_DATA_IT itList = it2->second->s_pListData.begin();
		MAP_DATA_IT itListEnd = it2->second->s_pListData.end();
		for( ; itList != itListEnd ; ++itList )
		{
			(*itList)->s_bIsOpen = false;
		}
		it2->second->s_bGroupOpen = false;
	}

	mUseDeck = INT_MIN;
}

void EncyclopediaContents::Get_UseDeck(void* pData)
{
	SAFE_POINTER_RET(pData);
	int* pGetData = static_cast<int*>(pData);

	*pGetData = mUseDeck;
}

void EncyclopediaContents::Get_BisRecv(void* pData)
{
	SAFE_POINTER_RET(pData);
	bool* pGetData = static_cast<bool*>(pData);

	*pGetData = mbIsRecv;
}

void EncyclopediaContents::Get_GroupInfoNullCheck(void* pData)
{
	SAFE_POINTER_RET(pData);
	bool* pGetData = static_cast<bool*>(pData);

	EncyclopediaContents::sGROUP_INFO* pGroupInfo = mGroupInfoMap[ mUseDeck ];

	if( pGroupInfo == NULL )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString(_T("GroupInfo is NULL - UsingDeck:%d"), mUseDeck);
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("GroupInfo is NULL - UsingDeck:%d"), mUseDeck);

		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );

		*pGetData = true;
	}
	*pGetData = false;
}

void EncyclopediaContents::Get_GroupInfoCondition(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int,USHORT>* pGetData = static_cast<std::pair<int,USHORT>*>(pData);

	if(pGetData->first >= 100)
	{
		MAP_GROUP_IT pGroupInfo = mGroupInfoMap.find(iDeckIdx);
		if(pGroupInfo == mGroupInfoMap.end())
			return;	

		pGetData->second = pGroupInfo->second->s_nCondition[pGetData->first%100];
	}
	else
	{
		MAP_GROUP_IT pGroupInfo = mGroupInfoMap.find(mUseDeck);
		if(pGroupInfo == mGroupInfoMap.end())
			return;	

		pGetData->second = pGroupInfo->second->s_nCondition[pGetData->first];
	}
}

void EncyclopediaContents::Get_GroupInfoNatType(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int,USHORT>* pGetData = static_cast<std::pair<int,USHORT>*>(pData);

	if(pGetData->first >= 100)
	{
		MAP_GROUP_IT pGroupInfo = mGroupInfoMap.find(iDeckIdx);
		if(pGroupInfo == mGroupInfoMap.end())
			return;	

		pGetData->second = pGroupInfo->second->s_nAT_Type[pGetData->first%100];
	}
	else
	{
		MAP_GROUP_IT pGroupInfo = mGroupInfoMap.find(mUseDeck);
		if(pGroupInfo == mGroupInfoMap.end())
			return;	

		pGetData->second = pGroupInfo->second->s_nAT_Type[pGetData->first];
	}
}

void EncyclopediaContents::Get_GroupInfoProb(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int,UINT>* pGetData = static_cast<std::pair<int,UINT>*>(pData);

	if(pGetData->first >= 100)
	{
		MAP_GROUP_IT pGroupInfo = mGroupInfoMap.find(iDeckIdx);
		if(pGroupInfo == mGroupInfoMap.end())
			return;	

		pGetData->second = pGroupInfo->second->s_nProb[pGetData->first%100];
	}
	else
	{
		MAP_GROUP_IT pGroupInfo = mGroupInfoMap.find(mUseDeck);
		if(pGroupInfo == mGroupInfoMap.end())
			return;	

		pGetData->second = pGroupInfo->second->s_nProb[pGetData->first];
	}
}

void EncyclopediaContents::Get_GroupInfoOption(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int,USHORT>* pGetData = static_cast<std::pair<int,USHORT>*>(pData);

	if(pGetData->first >= 100)
	{
		MAP_GROUP_IT pGroupInfo = mGroupInfoMap.find(iDeckIdx);
		if(pGroupInfo == mGroupInfoMap.end())
			return;	

		pGetData->second = pGroupInfo->second->s_nOption[pGetData->first%100];
	}
	else
	{
		MAP_GROUP_IT pGroupInfo = mGroupInfoMap.find(mUseDeck);
		if(pGroupInfo == mGroupInfoMap.end())
			return;	

		pGetData->second = pGroupInfo->second->s_nOption[pGetData->first];
	}
}

void EncyclopediaContents::Get_GroupInfoVal(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int,USHORT>* pGetData = static_cast<std::pair<int,USHORT>*>(pData);

	if(pGetData->first >= 100)
	{
		MAP_GROUP_IT pGroupInfo = mGroupInfoMap.find(iDeckIdx);
		if(pGroupInfo == mGroupInfoMap.end())
			return;	

		pGetData->second = pGroupInfo->second->s_nVal[pGetData->first%100];
	}
	else
	{
		MAP_GROUP_IT pGroupInfo = mGroupInfoMap.find(mUseDeck);
		if(pGroupInfo == mGroupInfoMap.end())
			return;	

		pGetData->second = pGroupInfo->second->s_nVal[pGetData->first];
	}	
}

void EncyclopediaContents::Get_GroupInfoTime(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int,u4>* pGetData = static_cast<std::pair<int,u4>*>(pData);

	if(pGetData->first >= 100)
	{
		MAP_GROUP_IT pGroupInfo = mGroupInfoMap.find(iDeckIdx);
		if(pGroupInfo == mGroupInfoMap.end())
			return;	

		pGetData->second = pGroupInfo->second->s_nTime[pGetData->first%100];
	}
	else
	{
		MAP_GROUP_IT pGroupInfo = mGroupInfoMap.find(mUseDeck);
		if(pGroupInfo == mGroupInfoMap.end())
			return;	

		pGetData->second = pGroupInfo->second->s_nTime[pGetData->first];
	}
}

void EncyclopediaContents::SetEncyInfoMap()
{
	if(mEncyInfoMap.size() != 0)
		return;	

	MakeGroupString( mGroupStr );

	//초기화는 모든 디지몬 잠겨있는 상태로 합시다. 일단 테이블 가져옴
	CDigimonEvolveInfo::MAP* pEvolMap = nsCsFileTable::g_pEvolMng->GetMap();
	CDigimonEvolveInfo::MAP_IT it = pEvolMap->begin();
	CDigimonEvolveInfo::MAP_IT itEnd = pEvolMap->end();

	for( ; it != itEnd ; ++it )
	{
		sEVOL_INFO* m_EncyInfo = csnew sEVOL_INFO;

		USHORT nCount = 0;	// 해당 계열체에서 사용하고 있는 디지몬 수

		int nDigimonID = it->first;
		//한 진화체 정보에서 각각의 디지몬 따로 저장할거야
		for( int i = 0 ; i < nLimit::EvoUnit ; i++ )	// 0번 인덱스는 원래 안씀ㅋ
		{
			CDigimonEvolveObj* pFTEvolObj = it->second->GetEvolveObjByEvoSlot( i );

			if( pFTEvolObj == NULL )
			{
				m_EncyInfo->s_sInfo[ i ].s_eImgState = sINFO::S_NONE;		// 없는건 건너뛰어
				m_EncyInfo->s_sInfo[ i ].s_nDigimonID = -1;
				continue;
			}

			if( pFTEvolObj->m_nEnableSlot == 0 )
			{
				m_EncyInfo->s_sInfo[ i ].s_eImgState = sINFO::S_NONE;
				m_EncyInfo->s_sInfo[ i ].s_nDigimonID = pFTEvolObj->m_dwID;
				continue;
			}

			m_EncyInfo->s_sInfo[ i ].s_nDigimonID = pFTEvolObj->m_dwID;		// 디지몬 ID
			m_EncyInfo->s_sInfo[ i ].s_nEvolSlot = pFTEvolObj->m_nEvoSlot;		// 진화 슬롯
			m_EncyInfo->s_sInfo[ i ].s_eImgState = sINFO::S_CLOSE;			// 이미지는 일단 다 잠긴상태로 저장해욤
			if( nCount < 5 )	// 1 ~ 5 (5)
				m_EncyInfo->s_sInfo[ i ].s_rcIcon = CsRect( CsPoint( 10 + (nCount * 52), 9 ), CsSIZE( 32, 32 ) );
			else if( nCount < 10 )			// 6번째 디지몬부터 다음줄
				m_EncyInfo->s_sInfo[ i ].s_rcIcon = CsRect( CsPoint( 10 + ( ( nCount - 5 ) * 52 ), 59 ), CsSIZE( 32, 32 ) );
			else
				m_EncyInfo->s_sInfo[ i ].s_rcIcon = CsRect( CsPoint( 10 + ( ( nCount - 10 ) * 52 ), 109 ), CsSIZE( 32, 32 ) );

			// 진화 이미지 이름 찾기
			m_EncyInfo->s_sInfo[ i ].s_ImgFileName.clear();
			CsDigimon::sINFO* pFTInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( m_EncyInfo->s_sInfo[ i ].s_nDigimonID )->GetInfo();
			if( pFTInfo == NULL ) // 디지몬 테이블에 디지몬 정보 없음 
			{
#ifndef _GIVE
				CsMessageBoxA( MB_OK, "DM_Digimon_List.xls - %d 디지몬 정보 없음",  m_EncyInfo->s_sInfo[ i ].s_nDigimonID );
#endif
				//임시로 벽돌 넣어
				m_EncyInfo->s_sInfo[ i ].s_ImgFileName = "Inventory\\Block.tga";
#ifdef CRASHDUMP_0326
				// 디지몬 테이블에 디지몬 정보 없음 
				DUMPLOG( " C012 - 1, %d ", m_EncyInfo->s_sInfo[ i ].s_nDigimonID );
#endif
			}
			else if( g_pModelDataMng->GetData( pFTInfo->s_dwModelID ) == NULL ) //모델데이터에 디지몬 정보 없음
			{
#ifndef _GIVE
				CsMessageBoxA( MB_OK, "Model.dat - %d 디지몬 정보 없음", pFTInfo->s_dwModelID );
#endif
				//임시로 벽돌 넣어
				m_EncyInfo->s_sInfo[ i ].s_ImgFileName = "Inventory\\Block.tga";
#ifdef CRASHDUMP_0326
				//모델데이터에 디지몬 정보 없음
				DUMPLOG( " C012 - 2, %d ", pFTInfo->s_dwModelID );
#endif
			}
			else
			{ 
				m_EncyInfo->s_sInfo[ i ].s_ImgFileName = g_pModelDataMng->GetEvoModelIconFile( pFTInfo->s_dwModelID );
			}

			nCount++;		// 디지몬 수 ++
		}

		int nIdxDigimon = it->second->GetInfo()->s_nDigimonID;				// 계열체 디지몬 번호
		m_EncyInfo->s_nGroup = GetGroup_DigimonName( mGroupStr, nIdxDigimon );	// 계열체 정렬 그룹 저장
		m_EncyInfo->s_nCount = (UCHAR)nCount;		// 해당 계열체에서 사용하고 있는 디지몬 수

		// 보상 받기 버튼
		std::wstring cDigimonName = L" ";
		CsTacticsExplain* pTE = nsCsFileTable::g_pTacticsMng->GetTacticsExplain( nIdxDigimon );
		if( pTE != NULL )
		{
			CsTacticsExplain::sINFO * pInfo = pTE->GetInfo();
			if( pInfo )
			{
				cDigimonName = pInfo->s_szTacticsName;
				m_EncyInfo->s_cDigimonName = pInfo->s_szTacticsName;
#ifdef VERSION_USA
				std::wstring::size_type pos = cDigimonName.find(L": ");
				if( pos != std::wstring::npos )
					cDigimonName = cDigimonName.substr( pos + 2 );
#endif
			}
		}

		//계열체 이름으로 텍스트 세팅
		mEncyInfoMap[ nIdxDigimon ] = m_EncyInfo;
	}

	CDigimonEvolveInfo::MAP_IT it2 = pEvolMap->begin();
	CDigimonEvolveInfo::MAP_IT itEnd2 = pEvolMap->end();

	for( ; it2 != itEnd2 ; ++it2 )
	{
		//진화체 이름 가나다 순 정렬 위해 정렬 번호 매겨놓자
		int nIdxDigimon = it2->second->GetInfo()->s_nDigimonID;	// 계열체 디지몬 번호
		mEncyInfoMap[ nIdxDigimon ]->s_nOrder = GetIdx_DigimonName( nIdxDigimon );	//계열체 정렬 번호 저장
	}

	mUseDeck = INT_MIN;	// 선택한 덱 번호, 미사용 중일 땐 -int
}

void EncyclopediaContents::SetGroupInfoMap()
{
	if(mGroupInfoMap.size() != 0)
		return;	

	//덱 정보
	CsEncy_Deck_Composition::MAP_IT itGroup = nsCsFileTable::g_pDigimonMng->GetGroupMap()->begin();
	CsEncy_Deck_Composition::MAP_IT itGroupEnd = nsCsFileTable::g_pDigimonMng->GetGroupMap()->end();

	CsEncy_Deck::MAP_IT itDeck = nsCsFileTable::g_pDigimonMng->GetDeckMap()->begin();

	for( ; itGroup != itGroupEnd ; itGroup++ )
	{
		int nidx = itGroup->second->GetInfo()->s_nGroupIdx;

		sGROUP_INFO* pInfo = csnew sGROUP_INFO;
		pInfo->s_nGroupIdx = nidx;
		pInfo->s_bGroupOpen = false;	//초기값은 다 못모은걸로
		pInfo->s_cRanderCnt = 0;

		int nListSize = itGroup->second->GetInfo()->s_pListData.size();
		std::list< CsEncy_Deck_Composition::sDATA* >::iterator itList = itGroup->second->GetInfo()->s_pListData.begin();
		std::list< CsEncy_Deck_Composition::sDATA* >::iterator itListEnd = itGroup->second->GetInfo()->s_pListData.end();

		for( ; itList != itListEnd ; itList++ )
		{
			sDATA* pData = csnew sDATA;

			//중복검사
			std::list< CsEncy_Deck_Composition::sDATA* >::iterator itTemp = itGroup->second->GetInfo()->s_pListData.begin();

			pData->s_bIsRender = true;	//중복되는 디지몬 랜더 여부 확인
			for( ;itTemp != itList ; itTemp++ )
			{
				if( (*itTemp)->s_dwDestDigimonID == (*itList)->s_dwDestDigimonID )
				{
					pData->s_bIsRender = false;//이미 존재하는 디지몬(중복디지몬) .ex)조그레스
					break;
				}
			}

			pData->s_bIsOpen = false;
			pData->s_dwBaseDigimonID = (*itList)->s_dwBaseDigimonID;
			pData->s_nEvolslot = (*itList)->s_nEvolslot;
			pData->s_dwDestDigimonID = (*itList)->s_dwDestDigimonID;

			// 진화 이미지 이름 찾기
			pData->s_ImgFileName.clear();
			CsDigimon::sINFO* pFTInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( pData->s_dwDestDigimonID )->GetInfo();
			if( pFTInfo == NULL ) // 디지몬 테이블에 디지몬 정보 없음
			{
#ifndef _GIVE
				CsMessageBoxA( MB_OK, "DM_Digimon_List.xls - %d 디지몬 정보 없음", pData->s_dwDestDigimonID );
#endif
				//임시로 벽돌 넣어
				pData->s_ImgFileName = "Inventory\\Block.tga";
#ifdef CRASHDUMP_0326
				// 디지몬 테이블에 디지몬 정보 없음
				DUMPLOG( " C012 - 1, %d ", pData->s_dwDestDigimonID );
#endif
			}
			else if( g_pModelDataMng->GetData( pFTInfo->s_dwModelID ) == NULL ) //모델데이터에 디지몬 정보 없음
			{
#ifndef _GIVE
				CsMessageBoxA( MB_OK, "Model.dat - %d 디지몬 정보 없음", pFTInfo->s_dwModelID );
#endif
				//임시로 벽돌 넣어
				pData->s_ImgFileName = "Inventory\\Block.tga";
#ifdef CRASHDUMP_0326
				//모델데이터에 디지몬 정보 없음
				DUMPLOG( " C012 - 2, %d ", pFTInfo->s_dwModelID );
#endif
			}
			else
			{
				pData->s_ImgFileName = g_pModelDataMng->GetEvoModelIconFile( pFTInfo->s_dwModelID );
			}

			if( pData->s_bIsRender )
				pInfo->s_cRanderCnt += 1;
			pInfo->s_pListData.push_back( pData );
		}
		//덱 옵션값 셋팅
		for( int i = 0; i < ENCY_MAX_OPTION ; i++ ) 
		{
			pInfo->s_nCondition[ i ] = itDeck->second->GetInfo()->s_nCondition[ i ];
			pInfo->s_nAT_Type[ i ] = itDeck->second->GetInfo()->s_nAT_Type[ i ];
			pInfo->s_nOption[ i ] = itDeck->second->GetInfo()->s_nOption[ i ];
			pInfo->s_nVal[ i ] = itDeck->second->GetInfo()->s_nVal[ i ];
			pInfo->s_nProb[ i ] = itDeck->second->GetInfo()->s_nProb[ i ];
			pInfo->s_nTime[ i ] = itDeck->second->GetInfo()->s_nTime[ i ];
		}

		//덱 선택 버튼
		pInfo->s_GroupName = itDeck->second->GetInfo()->s_szGroupName; 

		//덱 효과 사용 여부
		pInfo->s_bUse = false; 		

		mGroupInfoMap[ nidx ] = pInfo;

		itDeck++;
	}
}

void EncyclopediaContents::SetUseDeck( int nDeckIdx, bool bUse )
{
	if( nDeckIdx != 0 )
		mGroupInfoMap[nDeckIdx]->s_bUse = bUse;
	else	// nDeckIdx == 0 ( 선택 해제 )
	{
		MAP_GROUP_IT it = mGroupInfoMap.begin();
		MAP_GROUP_IT itEnd = mGroupInfoMap.end();

		for( ; it != itEnd ; ++it )
			it->second->s_bUse = false;
	}

	if( bUse )
		mUseDeck = nDeckIdx;
	else
		mUseDeck = INT_MIN;

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCYCLOPEDIA ) )
	{
		Notify(ENCYCLOPEDIA_USEDECK);
	}
}

void EncyclopediaContents::MakeGroupString( std::map<std::wstring,std::wstring>& wsGroupStr )
{
	// ex) 다, ㄱ~ㄴ
	wsGroupStr.insert(make_pair( UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_KEY_A" ), UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_A" ) ));
	wsGroupStr.insert(make_pair( UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_KEY_B" ), UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_B" ) ));
	wsGroupStr.insert(make_pair( UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_KEY_C" ), UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_C" ) ));
	wsGroupStr.insert(make_pair( UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_KEY_D" ), UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_D" ) ));
	wsGroupStr.insert(make_pair( UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_KEY_E" ), UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_E" ) ));
	wsGroupStr.insert(make_pair( UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_KEY_F" ), UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_F" ) ));
	wsGroupStr.insert(make_pair( UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_KEY_G" ), UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_G" ) ));

#ifndef VERSION_KOR
	wsGroupStr.insert(make_pair( UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_KEY_H" ), UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_H" ) ));
#endif

#if defined( VERSION_USA ) || defined( VERSION_TW )
	wsGroupStr.insert(make_pair( UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_KEY_I" ), UISTRING_TEXT( "ENCYCLOPEDIA_GROUP_I" ) ));
#endif
}

int EncyclopediaContents::GetGroup_DigimonName( std::map<std::wstring,std::wstring> const& wsPP, int nDigimonID )
{
	TCHAR cDigimonName[ 64 ];
	//계열체 이름
	if( nsCsFileTable::g_pTacticsMng->GetTacticsExplain( nDigimonID ) != NULL )
		_tcscpy( cDigimonName, nsCsFileTable::g_pTacticsMng->GetTacticsExplain( nDigimonID )->GetInfo()->s_szTacticsName );
	else 
		return 0;

	std::wstring w1s = cDigimonName;
	// GSP 에서 디지몬 계열체 이름 앞에 'Type: '이 붙어있어 정렬이 안됨.. type을 떼어낸 이름으로 정렬하자
#if( defined VERSION_USA || defined VERSION_TH )
	std::wstring::size_type n = w1s.find_first_of(L": ");
	if( n != std::wstring::npos && n != 0 )
	{
		w1s.replace(0, n,L"");
		w1s.replace(0, 2,L"");
	}
#endif

	std::wstring wsFirstText = w1s.substr(0, 1);

	int nGroupNumber = 0;
	if( wsFirstText.empty() )
		return nGroupNumber;

	size_t nLastNum= wsPP.size() - 1;
	std::map<std::wstring,std::wstring>::const_iterator it = wsPP.begin();
	for( ; it != wsPP.end(); ++it,++nGroupNumber )
	{
		if( nGroupNumber == nLastNum )
			break;

		if( wsFirstText < it->first )
			return nGroupNumber;
	}

	return nGroupNumber;
}

int EncyclopediaContents::GetIdx_DigimonName( int nDigimonID )
{
	TCHAR cCurName[ 64 ];
	TCHAR cListName[ 64 ];
	if( nsCsFileTable::g_pTacticsMng->GetTacticsExplain( nDigimonID ) != NULL )
		_tcscpy( cCurName, nsCsFileTable::g_pTacticsMng->GetTacticsExplain( nDigimonID )->GetInfo()->s_szTacticsName );

	EncyclopediaContents::MAP_IT it = mEncyInfoMap.begin();
	EncyclopediaContents::MAP_IT itEnd = mEncyInfoMap.end();

	int nOrder = 0;						// 리턴 값 ( 정렬 번호 0 ~ 디지몬 계열체 수 )

	for( ; it != itEnd ; ++it )
	{
		if( it->first == nDigimonID )		//비교할 디지몬과 입력받은 디지몬이 같은놈이면 패스
			continue; 
		if( nsCsFileTable::g_pTacticsMng->GetTacticsExplain( it->first ) != NULL )
			_tcscpy( cListName, nsCsFileTable::g_pTacticsMng->GetTacticsExplain(  it->first )->GetInfo()->s_szTacticsName );

		if( _tcscmp( cListName, cCurName ) == -1 )	// 가나다 순에서 뒤에놈이면
		{
			nOrder++;
		}
		else if( _tcscmp( cListName, cCurName ) == 0 )		//같은 이름이 있을 땐
		{
			if( it->first < nDigimonID ) // 계열체 번호로 비교
				nOrder++;
		}
	}
	return nOrder;
}

int EncyclopediaContents::GetEnchant_Val( int nEnchant_AT, int nEnchant_CT, int nEnchant_EV, int nEnchant_BL, int nEnchant_HP )
{
	//강화 수치 높은순 정렬
	int nVal[ IF_ENCY_MAX_ENCHANT_COUNT ] = { nEnchant_AT, nEnchant_CT, nEnchant_EV, nEnchant_BL, nEnchant_HP };

	for( int i = 0 ; i < IF_ENCY_MAX_ENCHANT_COUNT - 1 ; i++)
	{
		for ( int j = i+1 ; j < IF_ENCY_MAX_ENCHANT_COUNT ; j++ )
		{
			if( nVal[ i ] < nVal[ j ] )
			{
				int nTmp = nVal[ i ];
				nVal[ i ] = nVal[ j ];
				nVal[ j ] = nTmp;
			}
		}
	}

	if( nVal[ 1 ] >= 12 )
		return 4;
	else if( nVal[ 1 ] >= 10 )
		return 3;
	else if( nVal[ 1 ] >= 7 )
		return 2;
	else if( nVal[ 1 ] >= 4 )
		return 1;
	else 
		return 0;
}

bool EncyclopediaContents::CompEnchant_Val( int nDigimonID, int nEnchant_AT, int nEnchant_CT, int nEnchant_EV, int nEnchant_BL, int nEnchant_HP )
{
	// 현재 해당 계열체에 저장되있는 값과 비교
	int nCur_AT = mEncyInfoMap[ nDigimonID ]->s_nEnchant_AT;
	int nCur_CT = mEncyInfoMap[ nDigimonID ]->s_nEnchant_CT;
	int nCur_EV = mEncyInfoMap[ nDigimonID ]->s_nEnchant_EV;
	int nCur_BL = mEncyInfoMap[ nDigimonID ]->s_nEnchant_BL;
	int nCur_HP = mEncyInfoMap[ nDigimonID ]->s_nEnchant_HP;

	// 인덱스( 별 수, 0~4 )
	int nCurIdx = GetEnchant_Val(nCur_AT, nCur_CT, nCur_EV, nCur_BL, nCur_HP );
	int nNewIdx = GetEnchant_Val(nEnchant_AT, nEnchant_CT, nEnchant_EV, nEnchant_BL, nEnchant_HP );

	//별 수 비교해서
	if( nNewIdx > nCurIdx )	// 별이 더 높으면 갱신해줘야함
		return true;
	else if( nNewIdx == nCurIdx )	// 별 수가 같을 때
	{
		//강화 수치 높은순 정렬
		int nCurVal[ IF_ENCY_MAX_ENCHANT_COUNT ] = { nCur_AT, nCur_CT, nCur_BL, nCur_EV, nCur_HP };
		for( int i = 0 ; i < IF_ENCY_MAX_ENCHANT_COUNT - 1 ; i++)
		{
			for ( int j = i+1 ; j < IF_ENCY_MAX_ENCHANT_COUNT ; j++ )
			{
				if( nCurVal[ i ] < nCurVal[ j ] )
				{
					int nTmp = nCurVal[ i ];
					nCurVal[ i ] = nCurVal[ j ];
					nCurVal[ j ] = nTmp;
				}
			}
		}

		int nNewVal[ IF_ENCY_MAX_ENCHANT_COUNT ] = { nEnchant_AT, nEnchant_CT, nEnchant_EV, nEnchant_BL, nEnchant_HP };
		for( int i = 0 ; i < IF_ENCY_MAX_ENCHANT_COUNT - 1 ; i++)
		{
			for ( int j = i+1 ; j < IF_ENCY_MAX_ENCHANT_COUNT ; j++ )
			{
				if( nNewVal[ i ] < nNewVal[ j ] )
				{
					int nTmp = nNewVal[ i ];
					nNewVal[ i ] = nNewVal[ j ];
					nNewVal[ j ] = nTmp;
				}
			}
		}

		// 높은 두 값의 합으로 비교
		int nSumNew = nNewVal[ 0 ] + nNewVal[ 1 ];
		int nSumCur = nCurVal[ 0 ] + nCurVal[ 1 ];

		if( nSumNew > nSumCur )	// 기존것보다 합이 더 크다면 갱신
			return true;
		else if( nSumNew == nSumCur )	// 기존것과 합이 같을 때
		{
			if( nNewVal[ 0 ] > nCurVal[ 0 ] )	//높은값이 더 큰것
				return true;
		}
	}
	return false;
}

EncyclopediaContents::sEVOL_INFO* EncyclopediaContents::GetEncyclopediaEvolInfo( DWORD const& dwBaseDigimonID )
{	
	EncyclopediaContents::MAP_IT it = mEncyInfoMap.find( dwBaseDigimonID );
	if( it == mEncyInfoMap.end() )
		return NULL;
	return it->second;
}

int	EncyclopediaContents::GetUseDeck() 
{ 
	return mUseDeck; 
}

int EncyclopediaContents::GetScaleVal( int nScale )
{
	if( nScale >= 129 )
		return 4;		//빨간별
	else if( nScale >= 126 )
		return 3;		//파란별
	else if( nScale >= 121 )
		return 2;		//녹색별
	else if( nScale >= 116 )
		return 1;		//노란별
	else
		return 0;		//흰색별
}

int EncyclopediaContents::GetEnchant_Order( int nOrder, int& nEnchant_AT, int& nEnchant_CT, int& nEnchant_EV, int& nEnchant_BL, int& nEnchant_HP )
{
	int nVal[ IF_ENCY_MAX_ENCHANT_COUNT ][ 2 ] = { nEnchant_AT, 0, nEnchant_CT, 1, nEnchant_EV , 2, nEnchant_BL, 3, nEnchant_HP, 4};

	for( int i = 0 ; i < IF_ENCY_MAX_ENCHANT_COUNT - 1 ; i++)
	{
		for ( int j = i+1 ; j < IF_ENCY_MAX_ENCHANT_COUNT ; j++ )
		{
			if( nVal[ i ][ 0 ] < nVal[ j ][ 0 ] )	// 값이 크면 앞으로
			{
				int nTmp = nVal[ i ][ 0 ];
				nVal[ i ][ 0 ] = nVal[ j ][ 0 ];
				nVal[ j ][ 0 ] = nTmp;
				nTmp = nVal[ i ][ 1 ];
				nVal[ i ][ 1 ] = nVal[ j ][ 1 ];
				nVal[ j ][ 1 ] = nTmp;
			}
			else if( nVal[ i ][ 0 ] == nVal[ j ][ 0 ] )		//값이 같으면서
			{
				if( nVal[ i ][ 1 ] >= nVal[ j ][ 1 ] )	//강화 순서가 높으면 앞으로
				{
					int nTmp = nVal[ i ][ 0 ];
					nVal[ i ][ 0 ] = nVal[ j ][ 0 ];
					nVal[ j ][ 0 ] = nTmp;
					nTmp = nVal[ i ][ 1 ];
					nVal[ i ][ 1 ] = nVal[ j ][ 1 ];
					nVal[ j ][ 1 ] = nTmp;
				}
			}
		}
	}

	if( nOrder == 0 )
		return nVal[ 0 ][ 1 ];
	else
		return nVal[ 1 ][ 1 ];
}

EncyclopediaContents::sEVOL_INFO* EncyclopediaContents::GetEncyData(int num) const 
{ 
	EncyclopediaContents::MAP_CIT it = mEncyInfoMap.find( num );
	if( it == mEncyInfoMap.end() )
		return NULL;
	return it->second;
}

std::map< int, EncyclopediaContents::sEVOL_INFO* > const&	EncyclopediaContents::GetEncyMapData() const
{ 
	return mEncyInfoMap; 
}
std::map< int, EncyclopediaContents::sGROUP_INFO* > const&	EncyclopediaContents::GetGroupMapData() const
{ 
	return mGroupInfoMap; 
}

std::map<std::wstring,std::wstring> const& EncyclopediaContents::GetGroupString() const 
{ 
	return mGroupStr; 
}