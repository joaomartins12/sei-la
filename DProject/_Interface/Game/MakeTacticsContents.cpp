#include "StdAfx.h"
#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "MakeTacticsContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../Adapt/AdaptTutorialQuest.h"

int const MakeTacticsContents::IsContentsIdentity(void)
{
	return E_CT_MAKETACTICS_CONTENTS;
}

MakeTacticsContents::MakeTacticsContents(void):m_AdaptTutorialQuest(0),iSuccessLevel(0),iDigimonEggIdx(0),iBackupDiskIdx(0)
,bWaitRecvServer(false),iRequireDataCnt(0)
#ifdef MINIGAME
,iMinigame_StartBarTime(0)
,iMiniGame_ClickResult_NextBarIdx(0)
,iMiniGame_TimeOut_NextBarTime(0)
,iMiniGame_SuccessCnt(0)
,bMiniGame_ClickResult(false)
,m_nIsGame(false)
,m_nSuccessCnt(0)
#endif
{	
	m_pDigimonEgg.Reset();
	m_pBackupDisk.Reset();

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::MAKETACTICS_DIGITAMA_DATA,			this, &MakeTacticsContents::Recv_DigimonEgg );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::MAKETACTICS_DIGITAMA_LV,			this, &MakeTacticsContents::Recv_DigimonEggLevel );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::MAKETACTICS_DIGITAMA_SC_LV,			this, &MakeTacticsContents::Recv_DigimonSuccessLevel );
	
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::GET_MAKETACTICS_DIGITAMA_COUNT,		this, &MakeTacticsContents::Get_DigimonEggCount );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::GET_MAKETACTICS_DIGITAMA_ISENABLE,	this, &MakeTacticsContents::Get_DigimonEggIsEnable );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::GET_MAKETACTICS_DIGITAMA_TYPE,		this, &MakeTacticsContents::Get_DigimonEggType );

#ifdef MINIGAME
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_MAKETACTICS_DIGITAMA_START_MINIGAME, this, &MakeTacticsContents::Recv_DigimonEgg_MiniGame_Start );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_MAKETACTICS_DIGITAMA_CLICK_RESULT, this, &MakeTacticsContents::Recv_DigimonEgg_MiniGame_ClickResult );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_MAKETACTICS_DIGITAMA_TIME_OUT, this, &MakeTacticsContents::Recv_DigimonEgg_MiniGame_TimeOut );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_MAKETACTICS_DIGITAMA_ERROR_MESSAGE, this, &MakeTacticsContents::Recv_DigimonEgg_MiniGame_ErrorMessage );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_MAKETACTICS_DIGITAMA_END_MINIGAME, this, &MakeTacticsContents::Recv_DigimonEgg_MiniGame_End );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_MAKETACTICS_MINIGAME_INIT, this, &MakeTacticsContents::Recv_DigimonEgg_Minigame_Info );
#endif
}

MakeTacticsContents::~MakeTacticsContents(void)
{
	m_AdaptTutorialQuest = 0;
	GAME_EVENT_STPTR->DeleteEventAll(this);
}

int const MakeTacticsContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool MakeTacticsContents::IntraConnection(ContentsSystem& System)
{ 
	m_AdaptTutorialQuest = new AdaptTutorialQuest;
	if( m_AdaptTutorialQuest && !m_AdaptTutorialQuest->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_TUTORIAL_CONTENTS ) ) )
		return false;
	return true;
}

///////////////////////////////////////////////////////////////////////

void MakeTacticsContents::Recv_DigimonEgg( void* pData )
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_MAKE_DIGITAMA* pRecv = static_cast<GS2C_RECV_MAKE_DIGITAMA*>(pData);

	m_pDigimonEgg.m_nType					= pRecv->m_nEggType;
	m_pDigimonEgg.m_nRemainTradeLimitTime	= pRecv->m_nEggTradeLimitTime;
	m_pDigimonEgg.m_nCount					= pRecv->m_nEggLevel;
	assert( pRecv->m_nEggLevel <= 5 );
	m_pBackupDisk.m_nType					= pRecv->m_nBackupDisk; 
	m_pBackupDisk.m_nRemainTradeLimitTime	= pRecv->m_nBackupdiskTradeLimitTime;
}

void MakeTacticsContents::Recv_DigimonEggLevel( void* pData )
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_MAKE_DIGITAMA_LV* pRecv = static_cast<GS2C_RECV_MAKE_DIGITAMA_LV*>(pData);

	m_pDigimonEgg.m_nLevel = pRecv->m_nEggLevel;
}

void MakeTacticsContents::Recv_DigimonSuccessLevel( void* pData )
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_MAKE_DIGITAMA_SCLV* pRecv = static_cast<GS2C_RECV_MAKE_DIGITAMA_SCLV*>(pData);

	if(pRecv->m_nSuccessLevel != -1)
	{
		iSuccessLevel = pRecv->m_nSuccessLevel;
	}
	else
	{
		SetSuccessLevel();
	}
}

void MakeTacticsContents::Get_DigimonEggCount(void* pData)
{
	SAFE_POINTER_RET( pData );
	int * nData = static_cast<int*>(pData);

	*nData = m_pDigimonEgg.m_nCount;
}

void MakeTacticsContents::Get_DigimonEggIsEnable(void* pData)
{
	SAFE_POINTER_RET( pData );
	bool * nData = static_cast<bool*>(pData);

	*nData = m_pDigimonEgg.IsEnable();
}

void MakeTacticsContents::Get_DigimonEggType(void* pData)
{
	SAFE_POINTER_RET( pData );
	int * nData = static_cast<int*>(pData);

	*nData = m_pDigimonEgg.m_nType;	
}

void MakeTacticsContents::DataInputFailed()
{
	m_pDigimonEgg.m_nType				  = 0;
	m_pDigimonEgg.m_nRemainTradeLimitTime = 0;
	m_pDigimonEgg.m_nCount				  = 0;
}

void MakeTacticsContents::SetSuccessLevel()
{
	m_pDigimonEgg.m_nCount = iSuccessLevel; 
}

void MakeTacticsContents::ResetDigimonEgg()
{

	m_pDigimonEgg.Reset();
}

bool MakeTacticsContents::AutoResistDigiMonEgg()
{
	assert_cs( !bWaitRecvServer );
	int nOrgIndex = iDigimonEggIdx;

	cData_Inven* pInven = g_pDataMng->GetInven();
	int nInvenSlotCount = pInven->GetInvenSlotCount();

	CsItem::sINFO* pFTItemInfo;
	for( int i=iDigimonEggIdx; i<nInvenSlotCount; ++i )	
	{
		if( pInven->GetData( i )->IsEnable() == false )	
			continue;

		pFTItemInfo = pInven->GetFTData( i );	
		if( pFTItemInfo->s_nType_L == IF_DIGITAMA_MAKE_TACTICS_ITEM_TYPE )	
		{			
			if( IsDigimonEggResist( i ) == IF_DIGITAMA_MAKE_TACTICS_ITEM_TYPE )
			{
				iDigimonEggIdx = i+1;
#ifdef MINIGAME
				SetMinigameComp(false);
#endif
				return true;
			}
		}
	}

	// 못찾았다면
	// 처음부터 원래 있던 곳까지 찾는다.
	for( int i=0; i<nOrgIndex; ++i )
	{
		if( pInven->GetData( i )->IsEnable() == false )
			continue;

		pFTItemInfo = pInven->GetFTData( i );
		if( pFTItemInfo->s_nType_L == IF_DIGITAMA_MAKE_TACTICS_ITEM_TYPE )
		{

			if( IsDigimonEggResist( i ) == IF_DIGITAMA_MAKE_TACTICS_ITEM_TYPE )
			{
				iDigimonEggIdx = i+1;
#ifdef MINIGAME
				SetMinigameComp(false);
#endif
				return true;
			}			
		}
	}

	// 그래도 못찾았다면 없는거다
	iDigimonEggIdx = 0;

	cPrintMsg::PrintMsg( 20002 );

	return false;
}

bool MakeTacticsContents::InputDataDigimonEgg(int invenIDX, n4 NpcID, bool bVipMemberShip)
{	
	if( GetDigimonEggType() != 0 )								// 이미 등록 된게 있다면
	{		
		if( GetDigimonEggCount() > 0 )							// 레벨업 시도했던거라면
		{
			cPrintMsg::PrintMsg( 20003 );
			return true;
		}

		if( g_pDataMng->GetInven()->GetEmptySlotCount() == 0 )	// 인벤에 빈공간이 없다면
		{
			cPrintMsg::PrintMsg( 11015 );
			return true;
		}

		// 동기화
		if( net::game )
		{
			net::game->SendHatchCancel(NpcID, bVipMemberShip);
		}

		cItemInfo cancelItem = m_pDigimonEgg;
		cancelItem.m_nCount = 1;

		g_pDataMng->GetInven()->ItemCrop( &cancelItem );		// 아니면 인벤에 넣어 주자
	}

	cItemInfo* pInfo = g_pDataMng->GetInven()->GetData( invenIDX );

	if(!pInfo)
		return true;

	m_pDigimonEgg = *pInfo;	
	m_pDigimonEgg.m_nCount = 0;		// 레벨로 사용	

	pInfo->DecreaseCount( 1 );						// 인벤에서 빼준다.

	if( pInfo->m_nCount == 0 )
	{
		assert_cs( pInfo->IsEnable() == false );
	}

	else if( pInfo->m_nCount != 0)
	{
		assert_cs( pInfo->IsEnable() == true );
	}

	return false;
}

void MakeTacticsContents::RemoveDigimonEgg()
{
	cItemInfo cancelItem = m_pDigimonEgg;
	cancelItem.m_nCount = 1;
	g_pDataMng->GetInven()->ItemCrop( &cancelItem );
	m_pDigimonEgg.Reset();
}

cItemInfo* MakeTacticsContents::GetDigimonEgg()
{
	return &m_pDigimonEgg;
}

uint MakeTacticsContents::GetDigimonEggType()
{
	return m_pDigimonEgg.m_nType;
}

uint MakeTacticsContents::GetDigimonEggCount()
{
	return m_pDigimonEgg.GetCount();
}

void MakeTacticsContents::ResetBackupDisk()
{
	m_pBackupDisk.Reset();
}

bool MakeTacticsContents::AutoResisitBackupDisk()
{
	assert_cs( !bWaitRecvServer );
	int nOrgIndex = iBackupDiskIdx;
	
	cData_Inven* pInven = g_pDataMng->GetInven();		
	int nInvenSlotCount = pInven->GetInvenSlotCount();
	
	CsItem::sINFO* pFTItemInfo;
	for( int i=iBackupDiskIdx+1; i<nInvenSlotCount; ++i )	
	{
		if( pInven->GetData( i )->IsEnable() == false )
			continue;

		pFTItemInfo = pInven->GetFTData( i );	

		if( pFTItemInfo->s_nType_L == nItem::BackupDisk )	
		{
			iBackupDiskIdx = i + 1;

			if( IsBackupDiskResist( i ) == nItem::BackupDisk )
				return true;
		}
	}

	// 못찾았다면
	// 처음부터 원래 있던 곳까지 찾는다.
	for( int i=0; i<=nOrgIndex; ++i )
	{
		if( pInven->GetData( i )->IsEnable() == false )
			continue;

		pFTItemInfo = pInven->GetFTData( i );
		if( pFTItemInfo->s_nType_L == nItem::BackupDisk )
		{
			iBackupDiskIdx = i + 1;
			if( IsBackupDiskResist( i ) == nItem::BackupDisk )
				return true;
		}
	}

	// 그래도 못찾았다면 없는거다

	iBackupDiskIdx = 0;

	cPrintMsg::PrintMsg( 30504 );

	return false;
}

bool MakeTacticsContents::InputBackupDisk(int invenIDX, n4 NpcID, bool bVipMemberShip)
{
	cItemInfo* pInfo = g_pDataMng->GetInven()->GetData( invenIDX );
	CsItem::sINFO* pIteminfo = g_pDataMng->GetInven()->GetFTData( invenIDX );

	if( GetBackupDiskType() != 0 )
	{		
		if( g_pDataMng->GetInven()->GetEmptySlotCount() == 0 )	// 인벤에 빈공간이 없다면
		{
			cPrintMsg::PrintMsg( 11015 );
			return true;
		}

		//2017-04-05-nova #5959
		if( GetDigimonEggCount() >= 4 )
		{
			if( pIteminfo->s_nType_S == 2 ) //1 = 이벤트타입 , 2 = 캐쉬타입, 캐쉬는 4단까지만
			{
				cPrintMsg::PrintMsg( 30659, pIteminfo->s_szName );
				return true;
			}		
		}

		// 동기화
		if( net::game )
		{
			net::game->SendHatchBackupDiskCancel(NpcID, bVipMemberShip);
		}

		cItemInfo BackupDiskItem = m_pBackupDisk;
		BackupDiskItem.m_nCount = 1;
		
		g_pDataMng->GetInven()->ItemCrop( &BackupDiskItem );	// 아니면 인벤에 넣어 주자
	}	

	if( GetDigimonEggCount() >= 4 )
	{
		if( pIteminfo->s_nType_S == 2 ) //1 = 이벤트타입 , 2 = 캐쉬타입, 캐쉬는 4단까지만
		{
			cPrintMsg::PrintMsg( 30659, pIteminfo->s_szName );
			return true;
		}		
	}

	m_pBackupDisk = *pInfo;
	m_pBackupDisk.m_nCount = 0;

	pInfo->DecreaseCount( 1 );

	if( pInfo->m_nCount == 0 )
	{
		assert_cs( pInfo->IsEnable() == false );
	}

	else if( pInfo->m_nCount != 0)
	{
		assert_cs( pInfo->IsEnable() == true );
	}

	return false;
}

void MakeTacticsContents::RemoveBackupDisk()
{
	cItemInfo removeBackupDisk = m_pBackupDisk;
	removeBackupDisk.m_nCount = 1;

	g_pDataMng->GetInven()->ItemCrop( &removeBackupDisk );	// 아니면 인벤에 넣어 주자
	m_pBackupDisk.Reset();
}

cItemInfo* MakeTacticsContents::GetBackupDisk()
{
	return &m_pBackupDisk;
}

uint MakeTacticsContents::GetBackupDiskType()
{
	return m_pBackupDisk.m_nType;
}

uint MakeTacticsContents::GetBackupDiskCount()
{
	return m_pBackupDisk.GetCount();
}

///////////////////////////////////////////////////////////////////////

void MakeTacticsContents::SetWaitRecvServer(bool wait)
{
	bWaitRecvServer = wait;
}

bool MakeTacticsContents::GetWaitRecvServer()
{
	return bWaitRecvServer;
}

void MakeTacticsContents::AddDataFlag()
{
	++iDataFlag;
}

void MakeTacticsContents::SetDataFlag(int flag)
{
	iDataFlag = flag;
}

int MakeTacticsContents::GetDataFlag()
{
	return iDataFlag;
}

void MakeTacticsContents::SetDataFlagMax(int max)
{
	iDataFlagMax = max;
}

int MakeTacticsContents::GetDataFlagMax()
{
	return iDataFlagMax;
}

void MakeTacticsContents::SetEnableGradeMax(int grade)
{
	iEnableGradeMax = grade;
}

int MakeTacticsContents::GetDigiMonEggIndex()
{
	return iDigimonEggIdx;
}

int MakeTacticsContents::GetBackupDiskIndex()
{
	return iBackupDiskIdx;
}

int MakeTacticsContents::IsDigimonEggResist(int nInvenIndex)
{
	if( bWaitRecvServer )		
	{
		cPrintMsg::PrintMsg( 10004 );
		return -1;
	}

	cItemInfo* pInfo = g_pDataMng->GetInven()->GetData( nInvenIndex );
	CsItem::sINFO* pItemInfo = nsCsFileTable::g_pItemMng->GetItem( pInfo->GetType() )->GetInfo();	

	if( pItemInfo->s_nType_L == IF_DIGITAMA_MAKE_TACTICS_ITEM_TYPE ) // 알이라면
	{		
		return pItemInfo->s_nType_L;
	}

	if( pItemInfo->s_nType_L == nItem::BackupDisk ) // 백업 디스크라면
	{		
		return pItemInfo->s_nType_L;
	}

	cPrintMsg::PrintMsg( 11014 );
	return -1;
}

int MakeTacticsContents::IsBackupDiskResist(int nInvenIndex)
{
	if( bWaitRecvServer )
	{
		cPrintMsg::PrintMsg( 10004 );
		return -1;
	}

	cItemInfo* pInfo = g_pDataMng->GetInven()->GetData( nInvenIndex );
	CsItem::sINFO* pItemInfo = nsCsFileTable::g_pItemMng->GetItem( pInfo->GetType() )->GetInfo();	

	if( pItemInfo->s_nType_L == nItem::BackupDisk ) 
	{		
		return pItemInfo->s_nType_L;
	}

	return -1;
}

bool MakeTacticsContents::TryLevelUp1()
{
	// 서버에 보내서 체크 중이라면 리턴
	if( bWaitRecvServer == true )
	{
		cPrintMsg::PrintMsg( 10004 );
		return true;
	}	

	assert_cs( GetDigimonEggType() != 0 );

	if( nsCsFileTable::g_pTacticsMng->IsTactics( GetDigimonEggType() ) == false )
	{
		CsMessageBox( MB_OK, UISTRING_TEXT( "MAKETACTICS_EGG_NOT_HATCH" ).c_str() );
		return true;
	}

	CsTactics::sINFO* pFTTactics = nsCsFileTable::g_pTacticsMng->GetTactics( GetDigimonEggType() )->GetInfo();

	// 현재 사용하려는 DATA 등급으로 최고 레벨인지 체크
	if( GetDigimonEggCount() >= pFTTactics->s_nLimitLevel[iDataFlag] )
	{
#ifdef MAKE_TACTICS_UI_UPGRADE
		// 근데 사용하려는 DATA가 최고 등급이었다면, 더 이상 강화 불가
		// 만약, 다음 단계를 제한해야 하는 경우, 제한 단계를 0으로 검사합니다

		// 조건 1 : 사용하려는 DATA 등급에서 제한이 걸릴 때 사용하려는 DATA가 최고 단계 일 때
		// 조건 2 : 현재 DATA 등급에서 제한되었는데도 다음 DATA 등급을 사용할 수 없을 때
		// 조건 3 : 용병알의 부화단수가 MAX에 도달했을 때
		bool bCon1 = iDataFlag + 1 == CsTactics::DATA_MAX;
		bool bCon2 = pFTTactics->s_nLimitLevel[iDataFlag + 1] == 0;
		bool bCon3 = GetDigimonEggCount() >= (uint)iEnableGradeMax;	// 디지타마가 MAX 부화단수 일 때

		if( bCon1 || bCon2 || bCon3 )
		{
			cPrintMsg::PrintMsg( 20005 );
			return true;
		}

		#ifdef TACTICS_USE_DATA_LV2	/*중급을 쓰게*/ 
		else if( iDataFlag < CsTactics::DATA_LV2 )
		{
			cPrintMsg::PrintMsg( 30674 ); 
			return true;
		}
		#endif

		#ifdef TACTICS_USE_DATA_LV3 /*고급을 쓰게*/ 
		else if( iDataFlag < CsTactics::DATA_LV3 )
		{
			cPrintMsg::PrintMsg( ----- ); 
			return true;
		}
		#endif

		#ifdef TACTICS_USE_DATA_LV4 /*최고급을 쓰게*/ 
		else if( iDataFlag < CsTactics::DATA_LV4 ) 
		{
			cPrintMsg::PrintMsg( ----- ); 
			return true;
		}
		#endif
#endif

		cPrintMsg::PrintMsg( 20005 );
		return true;
	}

	int nTypeL, nTypeS = 0;

	iRequireDataCnt = pFTTactics->s_nReqItemCount[iDataFlag];

	CsItem::TypeS_to_TypeLS( pFTTactics->s_nReqItemS_Type[iDataFlag], nTypeL, nTypeS );

	// Data 갯수 부족		
	if( g_pDataMng->GetInven()->GetCount_Item_TypeLS( nTypeL, nTypeS ) < iRequireDataCnt )
	{
		cPrintMsg::PrintMsg( 20006 );
		return true;
	}

	return false;
}

bool MakeTacticsContents::TryLevelUp2()
{
	CsTactics::sINFO* pFTTactics = nsCsFileTable::g_pTacticsMng->GetTactics( GetDigimonEggType() )->GetInfo();	

	if( ( pFTTactics->s_nViewWarning[iDataFlag] )&&( GetDigimonEggCount() >= pFTTactics->s_nViewWarning[iDataFlag] ) )
	{
		cPrintMsg::PrintMsg( 30054 );
	}
	else
	{
		return true;
	}

	return false;
}

bool MakeTacticsContents::Send_Server(int NpcID, bool bVipMemberShip)
{
	CsTactics::sINFO* pFTTactics = nsCsFileTable::g_pTacticsMng->GetTactics( GetDigimonEggType() )->GetInfo();
	int nTypeL, nTypeS = 0;

	CsItem::TypeS_to_TypeLS( pFTTactics->s_nReqItemS_Type[iDataFlag], nTypeL, nTypeS );
	g_pDataMng->GetInven()->ItemRemove_NoServer_Type( nTypeL, nTypeS, iRequireDataCnt, true ); //Data 정보 보내준다.

	if( net::game )
	{
		net::game->SendHatchUp(NpcID, iDataFlag, bVipMemberShip);
		bWaitRecvServer = true;
	}
	else
	{		
		iSuccessLevel = GetDigimonEggCount() + 1;
		return true;
	}

	return false;
}

int MakeTacticsContents::Resist_Digitama(int invenIDX, int NpcID, bool bVipMemberShip)
{
	assert_cs( !bWaitRecvServer );

	if(InputDataDigimonEgg(invenIDX, NpcID, bVipMemberShip))
	{
		return 1;
	}

	if( net::game )	
	{
		net::game->SendHatchIn( invenIDX, NpcID, bVipMemberShip );

		return 2;
	}

	return 0;
}

int MakeTacticsContents::Resist_BackupDisk(int invenIDX, int NpcID, bool bVipMemberShip)
{
	assert_cs( !bWaitRecvServer );

	if(InputBackupDisk(invenIDX, NpcID, bVipMemberShip))
	{
		return 1;
	}

	if( net::game )
	{
		net::game->SendHatchBackupDisk( invenIDX, NpcID, bVipMemberShip);

		return 2;
	}

	return 0;
}

void MakeTacticsContents::Requite_Tactics(TCHAR* szName, int NpcID, int nPotableIndex)
{
	SAFE_POINTER_RET( net::game );

	if( -1 == nPotableIndex )
		net::game->SendHatchOut( szName, NpcID, 0, true);
	else
		net::game->SendHatchOut( szName, NpcID, nPotableIndex, false);

	ResetDigimonEgg();
}

void MakeTacticsContents::DestroyResist_Digitama(int NpcID, bool bVipMemberShip)
{
	SAFE_POINTER_RET( net::game );
	net::game->SendHatchCancel(NpcID, bVipMemberShip);
}

void MakeTacticsContents::Send_MiniGameCheck()
{
#ifdef MINIGAME
	SAFE_POINTER_RET( net::game );
	net::game->SendMiniGameIsStartReq();
#endif
}

void MakeTacticsContents::Cancel_BackupDisk(int NpcID, bool bVipMemberShip)
{
	SAFE_POINTER_RET( net::game );
	net::game->SendHatchBackupDiskCancel(NpcID, bVipMemberShip);

	RemoveBackupDisk();
}

void MakeTacticsContents::Cancel_Digitama(int NpcID, bool bVipMemberShip)
{
	SAFE_POINTER_RET( net::game );
	net::game->SendHatchCancel(NpcID, bVipMemberShip);

	RemoveDigimonEgg();
}

bool MakeTacticsContents::IsTutorialPlaying() const
{
	SAFE_POINTER_RETVAL( m_AdaptTutorialQuest, false );
	return m_AdaptTutorialQuest->IsTutorialPlaying();
}

#ifdef MINIGAME
void MakeTacticsContents::Recv_DigimonEgg_MiniGame_Start(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_START* pRecv = static_cast<GS2C_RECV_MAKE_DIGITAMA_MINIGAME_START*>(pData);
	SetStartBarChargingTime(pRecv->nBarTime);
	ShowMinigame();
	SetInvenMoney();
	Notify(EventMiniGame::_Start);
}

void MakeTacticsContents::Recv_DigimonEgg_MiniGame_ClickResult(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_CLICKRESULT* pRecv = static_cast<GS2C_RECV_MAKE_DIGITAMA_MINIGAME_CLICKRESULT*>(pData);
	SetClickResult(pRecv->nResult);
	SetClickResultNextBarIdx(pRecv->nBarIndex);
	SetNextBarChargingTime(pRecv->nBarTime);
	Notify(EventMiniGame::_ClickResult);
}

void MakeTacticsContents::Recv_DigimonEgg_MiniGame_TimeOut(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_TIMEOUT* pRecv = static_cast<GS2C_RECV_MAKE_DIGITAMA_MINIGAME_TIMEOUT*>(pData);
	SetTimeOutNextBarTime(pRecv->nBarTime);
	Notify(EventMiniGame::_TimeOut);
}

void MakeTacticsContents::Recv_DigimonEgg_MiniGame_ErrorMessage(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_ERRORMESSAGE* pRecv = 
		static_cast<GS2C_RECV_MAKE_DIGITAMA_MINIGAME_ERRORMESSAGE*>(pData);
	switch(pRecv->nErrorCode)
	{
	case 1://이전에 시도하고 초기화가 안된 상태에서 게임 시작하려 할 때
		{
#ifdef VERSION_TH
			cPrintMsg::PrintMsg( 30543 );
#else
			cPrintMsg::PrintMsg( 30543,  &pRecv->nSuccessCnt);
#endif
		}
		break;
	case 2://다음 단계 부화율이 0/최대단계에서 게임 시작하려 할 때
		cPrintMsg::PrintMsg( 20005 );
		break;
	case 3:
		cPrintMsg::PrintMsg( 30542 );
		break;//캐시알 및 확률 100% 일 때 게임 시작하려 할 때
	default:
		CsMessageBox( MB_OK, UISTRING_TEXT( "MAKETACTICS_MINIGAME_ERROR_MESSAGE" ).c_str() );
	}
}

void	MakeTacticsContents::Recv_DigimonEgg_MiniGame_End(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_END* pRecv = 
		static_cast<GS2C_RECV_MAKE_DIGITAMA_MINIGAME_END*>(pData);

	Notify(EventMiniGame::_OnEndMiniGame);
	SetMinigameComp(true);
	SetMinigameSuccessCnt(pRecv->nSuccess);
	Notify(EventMakeTactics::_CloseMiniGame);
	ShowMinigameResultMsg(pRecv->nSuccess);
}
void MakeTacticsContents::Recv_DigimonEgg_Minigame_Info(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_INIT* pRecv = 
		static_cast<GS2C_RECV_MAKE_DIGITAMA_MINIGAME_INIT*>(pData);

	m_nIsGame = pRecv->nResult;
	m_nSuccessCnt = pRecv->nSuccessCnt;
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_MINIGAME_COUNT, true );
}

void MakeTacticsContents::Send_MiniGame_End(int iValue)
{
	if(net::game != NULL)
		net::game->SendMinigameEnd( iValue );
}
void MakeTacticsContents::Send_MiniGame_TimeOut(void)
{
	if(net::game != NULL)
		net::game->SendMinigameTimeOut();
}
void MakeTacticsContents::Send_MiniGame_ClickBt(uchar nGameKind, ushort nBarIndex)
{
	if(net::game != NULL)
		net::game->SendMinigameClickBtn( nGameKind, nBarIndex);
}



void MakeTacticsContents::ShowMinigame(void)
{
	cMinigame* pMinigame = (cMinigame*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_MINIGAME,0,0,false );
}
void MakeTacticsContents::HideMinigame(void)
{
	if(g_pGameIF->IsActiveWindow(cBaseWindow::WT_MINIGAME))
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_MINIGAME ); 
}
void MakeTacticsContents::SetInvenMoney(void)
{
	int nCount = 0;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_MAKETACTICS_DIGITAMA_COUNT, &nCount);

	int nMoney = (5000 + 5000*nCount);
	u8 nCurMoney = g_pDataMng->GetInven()->GetMoney().GetInt();
	nCurMoney -= nMoney; // 차감
	g_pDataMng->GetInven()->SetMoney( nCurMoney, false );	// 돈 설정

}
void MakeTacticsContents::ShowMinigameResultMsg(const int& ResultCnt)
{
	int nResult = -4;
	int nCount = ResultCnt;
	nResult += ResultCnt;

	if(ResultCnt > 3)
		nResult++;

#ifdef VERSION_TH
	cPrintMsg::PrintMsg( 30541, &nCount );
#else
	cPrintMsg::PrintMsg( 30541, &nCount, &nResult );
#endif

}


#endif

