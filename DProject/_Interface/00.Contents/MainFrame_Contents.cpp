#include "StdAfx.h"
#include "MainFrame_Contents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

#include "../Adapt/AdaptTutorialQuest.h"

//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------
int const CMainFrameContents::IsContentsIdentity(void)
{
	return E_CT_MAINFRAME_CONTENTS;
}

CMainFrameContents::CMainFrameContents(void)
: m_nCurTab(0), m_nDeleteDigimonID(-1), m_pTarget(NULL), m_pAdaptTutorial(NULL)
#ifdef CROSSWARS_SYSTEM
, m_pRenderDigimon(NULL)
, m_bWaitRecvCross( false )
#endif
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::Recv_Ntf_XGuage_Update, this, &CMainFrameContents::_XGuageUpdate );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::Recv_Ntf_XGuage_Max_Update, this, &CMainFrameContents::_MaxXGuageUpdate );
	//GAME_EVENT_ST.AddEvent( EVENT_CODE::CHANGE_EQUIP, this, &CMainFrameContents::_ChangeEquip );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::Recv_XGToCrystal_Result, this, &CMainFrameContents::_RecvXGToCrystalResult );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::Recv_CrystalToXG_Result, this, &CMainFrameContents::_RecvCrystalToXGResult );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::CHANGE_QUICK_EXPAND, this, &CMainFrameContents::_ChangeQuickExpand);

	GAME_EVENT_ST.AddEvent( EVENT_CODE::SET_BALLOON_TITLE, this, &CMainFrameContents::_SetBalloonTitle );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SET_ACTIONKEY_NPC, this, &CMainFrameContents::_SetActionKeyNpc );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_CLOSE_TAMERSTATUS,			this, &CMainFrameContents::_OpenCloseTamerStatus );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::UPDATE_TAMERSTATUS,		this, &CMainFrameContents::_UpdateTamerStatus );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQUEST_DELETE_DIGIMON, this, &CMainFrameContents::_RequestDeleteDigimon );
#ifdef CROSSWARS_SYSTEM
	GAME_EVENT_ST.AddEvent( EVENT_CODE::CHANGE_CROSSINVEN_SIZE,		this, &CMainFrameContents::_ChangeCrossInvenSize );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::CHANGE_CROSSDIGIMON,		this, &CMainFrameContents::_ChangeCrossDigimon );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_CROSSLOADER,			this, &CMainFrameContents::_OpenCrossLoader );
#endif
#ifdef USE_BARCODE_REDER
	GAME_EVENT_ST.AddEvent( EVENT_CODE::USE_GAMESCANNER,			this, &CMainFrameContents::_UseGameScanner );
#endif
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_CLOSE_DIGIMONSTATUS,		this, &CMainFrameContents::_OpenCloseDigimonStatus );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::UPDATE_STAT_DIGIMONSTATUS,		this, &CMainFrameContents::_UpdateDigimonStat );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::UPDATE_SKILL_DIGIMONSTATUS,		this, &CMainFrameContents::_UpdateDigimonSkill );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::CHANGE_DIGIMON_DIGIMONSTATUS,	this, &CMainFrameContents::_ChangeDigimon );
	GAME_EVENT_ST.AddEvent(EVENT_CODE::RECV_SKILL_LEVELUP_DIGIMONSTATUS, this, &CMainFrameContents::_RecvDigimonSkillLevelUp);

	// Target UI
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SET_TARGET_MARK,		this, &CMainFrameContents::_SetTargetUI );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RELEASE_TARGET_MARK,	this, &CMainFrameContents::_ReleaseTargetUI );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::UPDATE_TARGET_HPRATE,	this, &CMainFrameContents::_UpdateTargetUI );

#ifdef UI_INVENTORY_RENEWAL
	if( EventRouter() )
		EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_Inventory_NewItem, this );
#endif
}

CMainFrameContents::~CMainFrameContents(void)
{
	m_pAdaptTutorial = NULL;

	GAME_EVENT_ST.DeleteEventAll(this);
#ifdef UI_INVENTORY_RENEWAL
	if( EventRouter() )
		EventRouter()->UnRegisterAll( this );
#endif
}

int const CMainFrameContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CMainFrameContents::Initialize(void)
{
	return true;
}

void CMainFrameContents::UnInitialize(void)
{
}

bool CMainFrameContents::IntraConnection(ContentsSystem& System)
{ 
	m_pAdaptTutorial = new AdaptTutorialQuest;
	if( m_pAdaptTutorial && !m_pAdaptTutorial->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_TUTORIAL_CONTENTS ) ) )
		return false;

	return true;
}

void CMainFrameContents::Update(float const& fElapsedTime)
{
}

void CMainFrameContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void CMainFrameContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
#ifdef UI_INVENTORY_RENEWAL
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EStreamEvt_Inventory_NewItem:
		{
			Notify( eNotifyNew, kStream );
		}
		break;
	}
#endif
}

void CMainFrameContents::MakeMainActorData(void)
{
}

void CMainFrameContents::ClearMainActorData(void)
{
	_ResetTamerStatus();
#ifdef CROSSWARS_SYSTEM
	m_bWaitRecvCross = false;
#endif
}

void CMainFrameContents::ClearWorldData(void)
{
}

void CMainFrameContents::MakeWorldData(void)
{
}

//////////////////////////////////////////////////////////////////////////
void CMainFrameContents::_ChangeEquip(void* pData)
{
// 	SAFE_POINTER_RET( g_pCharMng );
// 	SAFE_POINTER_RET( pData );
// 	GS2C_RECV_TAMER_ITEM_EQUIP* pRecv = static_cast<GS2C_RECV_TAMER_ITEM_EQUIP*>(pData);
// 	CTamer* pTamer = g_pCharMng->GetTamer( pRecv->m_nTamerIDX );
// 	SAFE_POINTER_RET( pTamer );
// 	
// 	CsC_PartObject::sCHANGE_PART_INFO cp;
// 	cp.s_nFileTableID = pRecv->m_nItemType;	
// 	cp.s_nPartIndex = pRecv->m_nEquipSlot;
// 	cp.s_nRemainTime = pRecv->m_nEndTime;
// 
// 	if( pRecv->m_nEquipSlot == nsPART::Costume )
// 	{
// 		pTamer->SetPcbangCostume( 0 );
// 		if( pRecv->m_nItemType == PCBANG_COSTUME_ITEM_ID ) // pc방 아이템을 입은경우
// 		{		
// 			cp.s_nFileTableID = pRecv->m_nChangeCostumeNo;					
// 			pTamer->SetPcbangCostume( pRecv->m_nChangeCostumeNo );		
// 		}		
// #ifdef GM_CLOCKING
// 		if( pRecv->m_nItemType == CLOCKING_ITEM_ID  )
// 			pTamer->SetClocking( true );
// 		else
// 			pTamer->SetClocking( false );
// #endif
// 	}
// 	else if( pRecv->m_nEquipSlot == nsPART::EquipAura )
// 	{
// 		if( g_pCharMng->IsTamerUser( pRecv->m_nTamerIDX )) // 내 캐릭터 일때만 동작
// 		{
// 			SAFE_POINTER_RET(g_pDataMng);
// 			cData_ServerSync* pServerSync = g_pDataMng->GetServerSync();
// 			SAFE_POINTER_RET(pServerSync);
// 
// 			if( pServerSync->IsEmptyRefCount(cData_ServerSync::RIDE_OFF) )	// 한번만 라이딩 해제기능 보내도록 검사.
// 			{
// 				if( pTamer->IsRide() == true ) //라이딩 중일 땐 내려야 한다
// 				{
// 					pTamer->SetRideEnd();
// 					g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::RIDE_OFF, 0 );
// 
// 				}
// 				g_pCharMng->GetTamerUser()->DeletePath();//가던길 멈주자
// 			}				
// 		}
// 
// 		pTamer->Scene_Aura( &cp );	
// 	}
// 
// 	pTamer->ChangePart( &cp );
// 
// #ifdef SDM_TAMER_XGUAGE_20180628
// 	if( g_pCharMng->IsTamerUser( pRecv->m_nTamerIDX )) // 내 캐릭터 일때만 동작
// 	{
// 		if( (pRecv->m_nEquipSlot + nItem::Head) == nItem::XAI )
// 		{
// 			if( 0 != pRecv->m_nItemType )
// 				_OpenCloseXaiGuageWindow( true );
// 			else
// 				_OpenCloseXaiGuageWindow( false );
// 		}
// 	}
// #endif
}

void CMainFrameContents::_OpenCloseXaiGuageWindow( bool bVisibe )
{
	if( !bVisibe ) // 장비 해제
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_XAIGUAGE_WINDOW );
	else
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_XAIGUAGE_WINDOW );

#ifdef SDM_TAMER_XGUAGE_20180628
	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RET(pDigimonUser);
	pDigimonUser->UpdateXEvoFlag();
	SAFE_POINTER_RET(g_pGameIF->GetQuickEvol());
	g_pGameIF->GetQuickEvol()->SetTargetObject( pDigimonUser );
#endif
}

void CMainFrameContents::_XGuageUpdate(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_XGuage_Update * recv = static_cast<GS2C_RECV_XGuage_Update*>(pData);

	SAFE_POINTER_RET(g_pCharMng);
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET(pUser);

	CTamerUser::sUSER_STAT* pUserState = dynamic_cast<CTamerUser::sUSER_STAT*>(pUser->GetBaseStat());
	SAFE_POINTER_RET(pUserState);

#ifdef SDM_TAMER_XGUAGE_20180628
	pUserState->SetCurrentCrystal( recv->nCrystal );
	pUserState->SetCurrentXGuage( recv->nXGuage );

	ContentsStream kTmp;
	kTmp << recv->nCrystal << recv->nXGuage;
	Notify( eUpdate_Current_XaiGuage, kTmp );
#endif
}

void CMainFrameContents::_MaxXGuageUpdate(void*pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_XGuage_Max_Update * recv = static_cast<GS2C_RECV_XGuage_Max_Update*>(pData);

	SAFE_POINTER_RET(g_pCharMng);
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET(pUser);

	CTamerUser::sUSER_STAT* pUserState = dynamic_cast<CTamerUser::sUSER_STAT*>(pUser->GetBaseStat());
	SAFE_POINTER_RET(pUserState);

#ifdef SDM_TAMER_XGUAGE_20180628
	pUserState->SetMaxCrystalXGuage( recv->nCrystalMax, recv->nXGuageMax );
	
	if( recv->nXGuageMax == 0 )// 장비의 Max 게이지가 0인경우 착용 해제
		_OpenCloseXaiGuageWindow(false);
	else
	{
		_OpenCloseXaiGuageWindow(true);
		ContentsStream kTmp;
		kTmp << recv->nCrystalMax << recv->nXGuageMax;
		Notify( eUpdate_Max_XaiGuage, kTmp );
	}
#endif
}

void CMainFrameContents::_RecvXGToCrystalResult(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_XGuage_to_Crystal * recv = static_cast<GS2C_RECV_XGuage_to_Crystal*>(pData);
	if( 0 == recv->nResult )
		return;

	cPrintMsg::PrintMsg( recv->nResult );
}

void CMainFrameContents::_RecvCrystalToXGResult(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_Crystal_to_XGuage * recv = static_cast<GS2C_RECV_Crystal_to_XGuage*>(pData);
	if( 0 == recv->nResult )
		return;

	cPrintMsg::PrintMsg( recv->nResult );
}

std::string	CMainFrameContents::GetTamerImgFile()
{
	SAFE_POINTER_RETVAL( g_pCharMng, "" );
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pTamerUser, "" );
	SAFE_POINTER_RETVAL( g_pModelDataMng, "" );
	
	return g_pModelDataMng->GetLargeModelIconFile( pTamerUser->GetModelID() );
}

std::string	CMainFrameContents::GetDigimonImgFile()
{
	SAFE_POINTER_RETVAL( g_pCharMng, "" );
	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RETVAL( pDigimonUser, "" );
	SAFE_POINTER_RETVAL( g_pModelDataMng, "" );

	return g_pModelDataMng->GetLargeModelIconFile( pDigimonUser->GetModelID() );
}

// X - Guague를 Crystal로 변환 요청
bool CMainFrameContents::SendXGToCrystal()
{
	SAFE_POINTER_RETVAL( net::game, false );
	SAFE_POINTER_RETVAL(g_pCharMng, false);
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL(pUser, false);

	CTamerUser::sUSER_STAT* pUserState = dynamic_cast<CTamerUser::sUSER_STAT*>(pUser->GetBaseStat());
	SAFE_POINTER_RETVAL(pUserState,false);

#ifdef SDM_TAMER_XGUAGE_20180628
	unsigned int uiMaxX = pUserState->GetMaxXGuage();
	unsigned short usMaxC = pUserState->GetMaxCrystal();
	if( uiMaxX == 0 || usMaxC == 0 )
		return false;

	unsigned int niConsumXG = GetConsumChargeValue();
	unsigned int uiCurX = pUserState->GetCurrentXGuage();
	if( uiCurX < niConsumXG )
		return false;

	unsigned short usCurC = pUserState->GetCurrentCrystal();
	if( uiCurX == usMaxC )
		return false;

	net::game->Send_XGToCrystal();
	return true;
#else
	return false;
#endif
}

unsigned int CMainFrameContents::GetConsumChargeValue()
{
	SAFE_POINTER_RETVAL(nsCsFileTable::g_pBaseMng,0);
	CsBaseMng::sLIMIT* pLimit = nsCsFileTable::g_pBaseMng->GetLimit();
	SAFE_POINTER_RETVAL(pLimit,0);
	return pLimit->s_nConsume_XG;
}

// Crystal을 X - Guague로 변환 요청
bool CMainFrameContents::SendCrystalToXG()
{
	SAFE_POINTER_RETVAL( net::game, false );
	SAFE_POINTER_RETVAL(g_pCharMng, false);
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL(pUser, false);

	CTamerUser::sUSER_STAT* pUserState = dynamic_cast<CTamerUser::sUSER_STAT*>(pUser->GetBaseStat());
	SAFE_POINTER_RETVAL(pUserState, false);

#ifdef SDM_TAMER_XGUAGE_20180628
	unsigned int uiMaxX = pUserState->GetMaxXGuage();
	unsigned short usMaxC = pUserState->GetMaxCrystal();
	if( uiMaxX == 0 || usMaxC == 0 )
		return false;

	unsigned short usCurC = pUserState->GetCurrentCrystal();
	if( usCurC == 0 )
		return false;

	unsigned int uiCurX = pUserState->GetCurrentXGuage();
	if( uiCurX == uiMaxX )
		return false;

	net::game->Send_CrystalToXG();
	return true;
#else
	return false;
#endif
}


unsigned short CMainFrameContents::GetMaxCrystal() const
{
	SAFE_POINTER_RETVAL(g_pCharMng, 0);
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL(pUser, 0);

	CTamerUser::sUSER_STAT* pUserState = dynamic_cast<CTamerUser::sUSER_STAT*>(pUser->GetBaseStat());
	SAFE_POINTER_RETVAL(pUserState, 0);

#ifdef SDM_TAMER_XGUAGE_20180628
	return pUserState->GetMaxCrystal();
#else
	return 0;
#endif
}

unsigned int CMainFrameContents::GetMaxXGuage() const
{
	SAFE_POINTER_RETVAL(g_pCharMng, 0);
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL(pUser, 0);

	CTamerUser::sUSER_STAT* pUserState = dynamic_cast<CTamerUser::sUSER_STAT*>(pUser->GetBaseStat());
	SAFE_POINTER_RETVAL(pUserState, 0);

#ifdef SDM_TAMER_XGUAGE_20180628
	return pUserState->GetMaxXGuage();
#else
	return 0;
#endif
}

unsigned short CMainFrameContents::GetCurrentCrystal() const
{
	SAFE_POINTER_RETVAL(g_pCharMng, 0);
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL(pUser, 0);

	CTamerUser::sUSER_STAT* pUserState = dynamic_cast<CTamerUser::sUSER_STAT*>(pUser->GetBaseStat());
	SAFE_POINTER_RETVAL(pUserState, 0);

#ifdef SDM_TAMER_XGUAGE_20180628
	return pUserState->GetCurrentCrystal();
#else
	return 0;
#endif
}

unsigned int CMainFrameContents::GetCurrentXGuage() const
{
	SAFE_POINTER_RETVAL(g_pCharMng, 0);
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL(pUser, 0);

	CTamerUser::sUSER_STAT* pUserState = dynamic_cast<CTamerUser::sUSER_STAT*>(pUser->GetBaseStat());
	SAFE_POINTER_RETVAL(pUserState, 0);

#ifdef SDM_TAMER_XGUAGE_20180628
	return pUserState->GetCurrentXGuage();
#else
	return 0;
#endif
}

void CMainFrameContents::_ChangeQuickExpand(void* pData)
{
	SAFE_POINTER_RET(pData);
	bool bOpend = *static_cast<bool*>(pData);

	ContentsStream kTmp;
	kTmp << bOpend;
	Notify( eChange_QuickSlot_Expand, kTmp);
}

void CMainFrameContents::_SetBalloonTitle(void* pData)
{
	SAFE_POINTER_RET( pData );
	ContentsStream kTmp = *static_cast<ContentsStream*>(pData);

	uint uUID = 0;
	std::wstring wsTitle;
	int type = 0;
	kTmp >> uUID >> wsTitle >> type;

	SAFE_POINTER_RET( g_pTalkBallone );

	if( type == cTalkBalloon::TYPE_5 )// 준비중
	{
		g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_5, uUID, wsTitle.c_str(), true, cTalkBalloon::sBALLOONE::ePERSONSTORE_OBJ, LIVE_UNLIMIT );
		return;
	}

#ifdef CONSIGNMENT_BALLOON_CASH
	CsC_AvObject* pTarget = g_pMngCollector->GetObject( uUID );

#ifdef COMPAT_487_2
	if (pTarget == NULL) {
		return;
	}
#endif

	TCHAR* szFileName = nsCsFileTable::g_pBaseMng->IsBallonNewType( pTarget->GetFTID() );
	if( szFileName != NULL )
	{
		g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_6, uUID, wsTitle.c_str(), true,
			cTalkBalloon::sBALLOONE::ePERSONSTORE_OBJ, LIVE_UNLIMIT );
	}
	else
	{
		g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_4, uUID, wsTitle.c_str(), true,
			cTalkBalloon::sBALLOONE::ePERSONSTORE_OBJ, LIVE_UNLIMIT );
	}
#else
	g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_4, uUID, wsTitle.c_str(), true, cTalkBalloon::sBALLOONE::ePERSONSTORE_OBJ, LIVE_UNLIMIT );
#endif
}

void CMainFrameContents::_SetActionKeyNpc(void* pData)
{
	SAFE_POINTER_RET( pData );
	ContentsStream kTmp = *static_cast<ContentsStream*>( pData );

	SAFE_POINTER_RET( g_pGameIF );
	SAFE_POINTER_RET( pData );

	UINT nFTID;
	UINT nObjType;
	kTmp >> nFTID >> nObjType;

	if( !_CheckTamerState() )
		return;

	bool bIsOpen = g_pGameIF->IsActiveWindow( cBaseWindow::WT_ACTIONKEY );

	if( nFTID == 0 )
	{
		// 열려 있으면 닫고
		if( bIsOpen )
		{
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ACTIONKEY );
			g_pMngCollector->ResetSearched();
		}

		return;
	}

	// 닫혀 있으면 열고
	if( !bIsOpen )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_ACTIONKEY );

	// NPC FTID 저장
	m_nFTID = nFTID;

	// NPC 이름 찾기
	SAFE_POINTER_RET( nsCsFileTable::g_pNpcMng );
	CsNpc* pNpc = nsCsFileTable::g_pNpcMng->GetNpc( m_nFTID );
	SAFE_POINTER_RET( pNpc );

	CsNpc::sINFO* pNpcInfo = pNpc->GetInfo();
	SAFE_POINTER_RET( pNpcInfo );

	std::wstring wsName = pNpcInfo->s_szName;
	
	// 혹시라도 엔피시가 설정되지 않았더라도 클라가 터지면 안 된다
	if( wsName.empty() )
		wsName = L"?";

	ContentsStream kSend;
	kSend << wsName;

	Notify( eChange_Near_Npc, kSend );
}

bool CMainFrameContents::_CheckTamerState()
{
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pTamer, false );
	cCondition* pCondition = pTamer->GetCondition();
	SAFE_POINTER_RETVAL( pCondition, false );

	if( pCondition->IsCondition( nSync::Die ) ||
		pCondition->IsCondition( nSync::Battle ) ||
		pCondition->IsCondition( nSync::Shop ) ||
		pCondition->IsCondition( nSync::ShopReady ) )
		return false;

	return true;
}

void CMainFrameContents::StartTalkNpc()
{
	SAFE_POINTER_RET( g_pMngCollector );
	SAFE_POINTER_RET( g_pGameIF );
	SAFE_POINTER_RET( g_pServerMoveOwner );

	CNpc* pNpc = g_pNpcMng->GetNpc( m_nFTID );
	SAFE_POINTER_RET( pNpc );

	// 디지몬 교체 중 대화 불가능
	SAFE_POINTER_RET( g_pDataMng );
	cData_ServerSync* pSync = g_pDataMng->GetServerSync();
	SAFE_POINTER_RET( pSync );
	if( pSync->IsChageDigimon() )
		return;

	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTamer );
	pTamer->SetArriveTarget( pNpc );
	pTamer->SetTargetPos( pNpc->GetPos(), 200.0f, false, false );
}

void CMainFrameContents::_ResetTamerStatus()
{
	m_nCurTab = 0;
	m_nDeleteDigimonID = -1;
#ifdef CROSSWARS_SYSTEM
	if( m_pRenderDigimon )
	{
		if( g_pCharMng )
		{
			g_pCharMng->DeleteTempChar( m_pRenderDigimon );
			m_pRenderDigimon = NULL;
		}
	}
	cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_D_NAME, 0 );
	cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_T_NAME, 0 );
	cSeparate::DeleteInstance( cBaseWindow::WT_NEW_TAMERSTATUS, 0 );
#endif
}

void CMainFrameContents::_OpenCloseTamerStatus(void* pData)
{
	SAFE_POINTER_RET( pData );
	ContentsStream kTmp = *static_cast<ContentsStream*>(pData);

	bool bSetTab = false;
	int nTabNo = 0;
	kTmp >> bSetTab >> nTabNo;

	SAFE_POINTER_RET( g_pGameIF );
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_TAMERSTATUS ) )
	{
#ifdef LJW_TUTORIAL_RENEWAL_20191128
		if( m_pAdaptTutorial && m_pAdaptTutorial->IsTutorialPlaying_ClientMode() )
			return;
#endif

#ifdef CROSSWARS_SYSTEM
		if( IsCrossLoader() )
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_NEW_TAMERSTATUS, 0, 4 );
		else
#endif
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_NEW_TAMERSTATUS, 0, 3 );
	}
	else
	{
		if( !bSetTab || m_nCurTab == nTabNo )
		{
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_NEW_TAMERSTATUS );
			_ResetTamerStatus();
			return;
		}
	}

	m_nCurTab = nTabNo;

	ContentsStream kSend;
	kSend << m_nCurTab;
	Notify( eSetTab, kSend );
}

void CMainFrameContents::_UpdateTamerStatus(void* pData)
{
	Notify( eUpdateStatus );
}

void CMainFrameContents::_RequestDeleteDigimon(void* pData)
{
	SAFE_POINTER_RET( pData );
	int nIndex = *static_cast< int* >( pData );

	m_nDeleteDigimonID = nIndex;
	if( g_pGameIF && !g_pGameIF->IsActiveWindow( cBaseWindow::WT_DELETEDIGIMON ) )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_DELETEDIGIMON );
}

void CMainFrameContents::CloseTamerStatus()
{
	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_NEW_TAMERSTATUS );

	_ResetTamerStatus();
}

void CMainFrameContents::DeleteDigimon(std::wstring wsPass)
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_ServerSync* pServerSync = g_pDataMng->GetServerSync();
	SAFE_POINTER_RET( pServerSync );
	if( pServerSync->IsEmptyRefCount( cData_ServerSync::DELETE_TACTICS, 0 ) == false )
	{
		cPrintMsg::PrintMsg( 10004 );
	}
	else
	{
		pServerSync->SendServer( cData_ServerSync::DELETE_TACTICS, 0 );
		if( net::game )
		{
			char szResistNumber[ 64 ] = {0, };
			W2M( szResistNumber, wsPass.c_str(), 64 );
			net::game->SendDeletePartner( m_nDeleteDigimonID + 1, szResistNumber );
			if( g_pGameIF )
			{
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DELETEDIGIMON );
				m_nDeleteDigimonID = -1;
			}
		}
	}
}

int CMainFrameContents::GetFriendShip() const
{
	SAFE_POINTER_RETVAL( g_pCharMng, 0 );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, 0 );
	CDigimonUser::sBASE_STAT* pStat = dynamic_cast<CDigimonUser::sBASE_STAT*>(pUser->GetBaseStat());
	SAFE_POINTER_RETVAL( pStat, 0 );

	return pStat->GetFriendShip();
}

int CMainFrameContents::GetTamerStat(eAbilType eType) const
{
	SAFE_POINTER_RETVAL( g_pCharMng, 0 );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, 0 );
	CTamerUser::sUSER_STAT* pStat= dynamic_cast<CTamerUser::sUSER_STAT*>(pUser->GetBaseStat());
	SAFE_POINTER_RETVAL( pStat, 0 );

	switch( eType )
	{
	case eAT:
		return pStat->GetAtt();
	case eDE:
		return pStat->GetDef();
	case eHP:
		return pStat->GetMaxHP();
	case eDS:
		return pStat->GetMaxDS();
	case eLV:
		return pStat->GetLevel();
#ifdef CROSSWARS_SYSTEM
	case eMaxCR:
		return pStat->GetMaxCR();
	case eCurCR:
		return pStat->GetCR();
#endif
	}

	return 0;
}

void CMainFrameContents::GetBaseStat(int& nType, int& nBaseStat, eAbilType eType)
{
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTamerUser );
	CTamerUser::sBASE_STAT* pBaseStat = pTamerUser->GetBaseStat();
	SAFE_POINTER_RET( pBaseStat );
	
	FMTamer::sTINFO TInfo;
	TInfo.s_nTamerID = pTamerUser->GetOrgModelID();
	TInfo.s_nLevel = pBaseStat->GetLevel();

	switch( eType )
	{
	case eAT:
		nType = APPLY_AP;
		nBaseStat = FMTamer::GetBaseAtt( &TInfo );
		break;
	case eDE:
		nType = APPLY_DP;
		nBaseStat = FMTamer::GetBaseDef( &TInfo );
		break;
	case eHP:
		nType = APPLY_MAXHP;
		nBaseStat = FMTamer::GetBaseMaxHP( &TInfo );
		break;
	case eDS:
		nType = APPLY_MAXDS;
		nBaseStat = FMTamer::GetBaseMaxDS( &TInfo );
		break;
	}
}

std::wstring CMainFrameContents::GetTamerName() const
{
	SAFE_POINTER_RETVAL( g_pCharMng, L"" );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, L"" );
	return pUser->GetName();
}

std::wstring CMainFrameContents::GetDigimonName() const
{
	SAFE_POINTER_RETVAL( g_pCharMng, L"" );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RETVAL( pUser, L"" );
	return pUser->GetName();
}

CsCoolTimeSeq* CMainFrameContents::GetFTItemCoolTime(uint nType) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, NULL );

	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nType );
	SAFE_POINTER_RETVAL( pFTItem, NULL );

	CsItem::sINFO* pFTItemInfo = pFTItem->GetInfo();
	SAFE_POINTER_RETVAL( pFTItemInfo, NULL );

	return pFTItemInfo->GetCoolTimeSeq();
}

cItemInfo* CMainFrameContents::GetEquipItem(int nEquipType) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, NULL );
	cData_TEquip* pEquip = g_pDataMng->GetTEquip();
	SAFE_POINTER_RETVAL( pEquip, NULL );

	cItemInfo* pEquipItem;
	if( nLimit::Equip <= nEquipType )			// digivice
		pEquipItem = pEquip->GetDigiviceItem();
	else
		pEquipItem = pEquip->GetData( nEquipType );
	SAFE_POINTER_RETVAL( pEquipItem, NULL );
	if( !pEquipItem->IsEnable() )
		return NULL;

	return pEquipItem;
}

uint CMainFrameContents::GetEquipItemType(int nEquipType) const
{
	cItemInfo* pEquipItem = GetEquipItem( nEquipType );
	if( pEquipItem && pEquipItem->IsEnable() )
		return pEquipItem->GetType();

	return 0;
}

int CMainFrameContents::GetOpenedChipsetSlot() const
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_Digivice* pDigivice = g_pDataMng->GetDigivice();
	SAFE_POINTER_RETVAL( pDigivice, 0 );
	return pDigivice->GetChipsetCount();
}

cItemInfo * CMainFrameContents::GetChipsetItem(int nChipsetIndex) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, NULL );
	cData_Digivice* pDigivice = g_pDataMng->GetDigivice();
	SAFE_POINTER_RETVAL( pDigivice, NULL );
	cItemInfo* pChipset = pDigivice->GetChipset( nChipsetIndex );
	SAFE_POINTER_RETVAL( pChipset, NULL );
	if( !pChipset->IsEnable() )
		return NULL;

	return pChipset;
}

int CMainFrameContents::GetSkillType(int nIndex, eSkillType eType) const
{
	SAFE_POINTER_RETVAL( g_pCharMng, -1 );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, -1 );
	cSkill* pSkillMng = pUser->GetSkillMng();
	SAFE_POINTER_RETVAL( pSkillMng, -1 );

	switch( eType )
	{
	case eBasic:
		{
			cSkill::sINFO* pSkillInfo = pSkillMng->GetSkill( nIndex );
			SAFE_POINTER_BEK( pSkillInfo );
			CsSkill* pFTSkill = pSkillInfo->s_pFTSkill;
			SAFE_POINTER_BEK( pFTSkill );
			CsSkill::sINFO* pFTSkillInfo = pFTSkill->GetInfo();
			SAFE_POINTER_BEK( pFTSkillInfo );

			if( !pFTSkill->IsActive() && !pFTSkill->IsPasive() && !pFTSkill->IsEmotionSkill() )
				return pFTSkillInfo->s_nIcon;
		}
		break;
	case eActive:
		{
			cSkill::sINFO* pSkillInfo = pSkillMng->GetTamerActiveSkill( nIndex );
			SAFE_POINTER_BEK( pSkillInfo );
			CsSkill* pFTSkill = pSkillInfo->s_pFTSkill;
			SAFE_POINTER_BEK( pFTSkill );
			CsSkill::sINFO* pFTSkillInfo = pFTSkill->GetInfo();
			SAFE_POINTER_BEK( pFTSkillInfo );

			if( pFTSkill->IsActive() )
				return pFTSkillInfo->s_nIcon;
		}
		break;
	case ePassive:
		{
			cSkill::sINFO* pSkillInfo = pSkillMng->GetTamerPasiveSkill( nIndex );
			SAFE_POINTER_BEK( pSkillInfo );
			CsSkill* pFTSkill = pSkillInfo->s_pFTSkill;
			SAFE_POINTER_BEK( pFTSkill );
			CsSkill::sINFO* pFTSkillInfo = pFTSkill->GetInfo();
			SAFE_POINTER_BEK( pFTSkillInfo );

			if( pFTSkill->IsPasive() )
				return pFTSkillInfo->s_nIcon;
		}
		break;
	}

	return -1;
}

int CMainFrameContents::GetSkillIndex(int nIndex, eSkillType eType) const
{
	SAFE_POINTER_RETVAL( g_pCharMng, -1 );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, -1 );
	cSkill* pSkillMng = pUser->GetSkillMng();
	SAFE_POINTER_RETVAL( pSkillMng, -1 );

	switch( eType )
	{
	case eBasic:
		{
			cSkill::sINFO* pSkillInfo = pSkillMng->GetSkill( nIndex );
			SAFE_POINTER_RETVAL( pSkillInfo, -1 );
			CsSkill* pFTSkill = pSkillInfo->s_pFTSkill;
			SAFE_POINTER_RETVAL( pFTSkill, -1 );

			if( !pFTSkill->IsActive() && !pFTSkill->IsPasive() && !pFTSkill->IsEmotionSkill() )
				return nIndex;
		}
		break;
	case eActive:
		return pSkillMng->GetTamerActiveSkillIndex( nIndex );
	case ePassive:
		return pSkillMng->GetTamerPasiveSkillIndex( nIndex );
	}

	return -1;
}

CsCoolTimeSeq* CMainFrameContents::GetSkillCoolTime(int nIndex) const
{
	if( -1 == nIndex )
		return NULL;

	SAFE_POINTER_RETVAL( g_pCharMng, NULL );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, NULL );
	cSkill* pSkillMng = pUser->GetSkillMng();
	SAFE_POINTER_RETVAL( pSkillMng, NULL );
	cSkill::sINFO* pSkillInfo = pSkillMng->GetSkill( nIndex );
	SAFE_POINTER_RETVAL( pSkillInfo, NULL );

	return &pSkillInfo->s_CoolTime;
}

int CMainFrameContents::GetTacticsCount() const
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	SAFE_POINTER_RETVAL( pTactics, 0 );
	return pTactics->GetTacticsCount();
}

int CMainFrameContents::GetTacticsOpenCount() const
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	SAFE_POINTER_RETVAL( pTactics, 0 );
	return pTactics->_GetOpenSlot();
}

int CMainFrameContents::GetTacticsLevel(int nTacticsIndex) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	SAFE_POINTER_RETVAL( pTactics, 0 );
	cData_PostLoad::sDATA* pTacticsData = pTactics->GetTactics( nTacticsIndex );
	SAFE_POINTER_RETVAL( pTacticsData, 0 );
	if( 0 == pTacticsData->s_dwBaseDigimonID )
		return 0;

	return pTacticsData->s_nLevel;
}

uint CMainFrameContents::GetTacticsType(int nTacticsIndex) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	SAFE_POINTER_RETVAL( pTactics, 0 );

	if( nTacticsIndex >= pTactics->_GetOpenSlot() )
		return 0;

	cData_PostLoad::sDATA* pTacticsData = pTactics->GetTactics( nTacticsIndex );
	SAFE_POINTER_RETVAL( pTacticsData, 0 );
	if( 0 == pTacticsData->s_dwBaseDigimonID )
		return 0;

	return pTacticsData->s_Type.m_nType;
}

std::string CMainFrameContents::GetTacticsFileName(int nTacticsIndex) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, "" );
	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	SAFE_POINTER_RETVAL( pTactics, "" );

	if( nTacticsIndex >= pTactics->_GetOpenSlot() )
		return "";

	cData_PostLoad::sDATA* pTacticsData = pTactics->GetTactics( nTacticsIndex );
	SAFE_POINTER_RETVAL( pTacticsData, "" );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, "" );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pTacticsData->s_dwBaseDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, "" );
	CsDigimon::sINFO* pDigimonInfo = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonInfo, "" );

	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, "" );
	SAFE_POINTER_RETVAL( g_pModelDataMng, "" );

	if( 0 != pDigimonInfo->s_dwModelID )
	{
		DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimonModelIdx( pDigimonInfo->s_dwModelID );
		return g_pModelDataMng->GetLargeModelIconFile( dwModelID );
	}

	return "";
}

int CMainFrameContents::GetTacticsHatchLv(int nTacticsIndex) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	SAFE_POINTER_RETVAL( pTactics, 0 );
	cData_PostLoad::sDATA* pTacticsData = pTactics->GetTactics( nTacticsIndex );
	SAFE_POINTER_RETVAL( pTacticsData, 0 );
	return pTacticsData->s_HatchLevel;
}

bool CMainFrameContents::IsCashSkill(int nSkillIndex)
{
	if( -1 == nSkillIndex )
		return false;

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, false );
	cSkill* pSkillMng = pUser->GetSkillMng();
	SAFE_POINTER_RETVAL( pSkillMng, false );
	cSkill::sINFO* pSkill = pSkillMng->GetTamerActiveSkill( nSkillIndex );
	SAFE_POINTER_RETVAL( pSkill, false );

	if( 0 == pSkill->s_nLifeTime )
		return false;

	return true;
}

bool CMainFrameContents::IsEquipDigivice() const
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_TEquip* pEquip = g_pDataMng->GetTEquip();
	SAFE_POINTER_RETVAL( pEquip, false );
	cItemInfo* pItemInfo = pEquip->GetDigiviceItem();
	SAFE_POINTER_RETVAL( pItemInfo, false );
	if( !pItemInfo->IsEnable() )
		return false;

	return true;
}

void CMainFrameContents::WearEquipItem(int nIconSlot)
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	CsItem::sINFO* pFTItemInfo = pInven->GetFTData( TO_ID( nIconSlot ) );
	switch( pFTItemInfo->s_nType_L )
	{
	case nItem::Head:
	case nItem::Coat:
	case nItem::Glove:
	case nItem::Pants:
	case nItem::Shoes:
	case nItem::Costume:
	case nItem::Glass:
	case nItem::Digivice:
#ifdef SDM_TAMER_XGUAGE_20180628
	case nItem::XAI:
#endif
	case nItem::EquipAura: //오라 추가 chu8820
		g_pDataMng->SendItemUse( nIconSlot );
		break;

	case nItem::Earring:
	case nItem::Necklace:
	case nItem::Ring:
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
	case nItem::Bracelet:
#endif
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ENCHANT_OPTION ) )
				cPrintMsg::PrintMsg( 11014 );
			else
				g_pDataMng->SendItemUse( nIconSlot );
		}
		break;
	}
}

void CMainFrameContents::MoveEquipItem(int nEquipType)
{
	int nSrvIndex = 0;
	if( nTamer::MaxParts == nEquipType )
		nSrvIndex = TO_DIGIVICE_SID( 0 );
	else
		nSrvIndex = TO_TEQUIP_SID( nEquipType );

	if( g_pDataMng && g_pDataMng->IsItemUse( nSrvIndex ) )
		g_pDataMng->SendItemMoveInven( nSrvIndex );
}

void CMainFrameContents::MoveChipsetItem(int nIconSlot, int nChipsetIndex, bool bToInven, bool bFromChip)
{
	SAFE_POINTER_RET( g_pDataMng );

	if( GetOpenedChipsetSlot() <= nChipsetIndex )
		return;

	if( bToInven )
	{
		g_pDataMng->SendItemMoveInven( TO_CHIPSET_SID( nChipsetIndex ) );
		return;
	}

	if( bFromChip )
	{
		if( g_pDataMng->IsItemUse( nIconSlot ) )
		{
			g_pDataMng->SendItemMove( nIconSlot, TO_CHIPSET_SID( nChipsetIndex ) );
			return;
		}
	}

	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	cItemInfo* pItemInfo = pInven->GetData( TO_ID( nIconSlot ) );
	SAFE_POINTER_RET( pItemInfo );
	if( !pItemInfo->IsEnable() )
		return;

	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() );
	SAFE_POINTER_RET( pFTItem );
	CsItem::sINFO* pFTItemInfo = pFTItem->GetInfo();
	SAFE_POINTER_RET( pFTItemInfo );

	if( pFTItemInfo->s_nType_L == nItem::Chipset )
	{
		if( g_pDataMng->IsItemUse( nIconSlot ) )
		{
			g_pDataMng->SendItemMove( nIconSlot, TO_CHIPSET_SID( nChipsetIndex ) );
		}
	}
}

void CMainFrameContents::OnClickTactitcs(int nTacticsIndex)
{
	SAFE_POINTER_RET( g_pDataMng );
	if( g_pDataMng->IsItemLock( TO_MEMORY_SID( nTacticsIndex ) ) )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	SAFE_POINTER_RET( g_pGameIF );
	cPopUpWindow* pPopWin = g_pGameIF->GetPopup();
	SAFE_POINTER_RET( pPopWin );
	pPopWin->SetPopup( CURSOR_ST.GetPos(), CsPoint( 10, 0 ), cPopUpWindow::DIGIVICE_TACTICS, nTacticsIndex );
}

void CMainFrameContents::UseSkill(int nSkillIndex)
{
	if( -1 == nSkillIndex )
		return;

	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );
	cSkill* pSkillMng = pUser->GetSkillMng();
	SAFE_POINTER_RET( pSkillMng );
	CsSkill* pFTSkill = pSkillMng->GetFTSkill( nSkillIndex );
	SAFE_POINTER_RET( pFTSkill );
	
	if( pFTSkill->IsPasive() )
		return;

	pUser->UseSkill( nSkillIndex );
}

void CMainFrameContents::DeleteSkill(int nSkillIndex)
{
	if( -1 == nSkillIndex )
		return;

	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );
	cSkill* pSkillMng = pUser->GetSkillMng();
	SAFE_POINTER_RET( pSkillMng );
	cSkill::sINFO* pSkill = pSkillMng->GetTamerActiveSkill( nSkillIndex );
	SAFE_POINTER_RET( pSkill );

	if( pSkill->s_nLifeTime == 0 )
		return;

	CsSkill* pFTSkill = pSkill->s_pFTSkill;
	SAFE_POINTER_RET( pFTSkill );
	CsSkill::sINFO* pFTSkillInfo = pFTSkill->GetInfo();
	SAFE_POINTER_RET( pFTSkillInfo );

	SAFE_POINTER_RET( nsCsFileTable::g_pSkillMng );
	CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, pFTSkillInfo->s_dwID );
	
	SAFE_POINTER_RET( pTamerSkill );
	CsTamerSkill::sTSkill_INFO* pTSkillInfo = pTamerSkill->Get_TSkill_Info();
	SAFE_POINTER_RET( pTSkillInfo );
	CsSkill* pTotalSkill = nsCsFileTable::g_pSkillMng->GetSkill( pTSkillInfo->s_dwSkillCode );
	SAFE_POINTER_RET( pTotalSkill );
	CsSkill::sINFO* pFTTSkillInfo = pTotalSkill->GetInfo();
	SAFE_POINTER_RET( pFTTSkillInfo );

	cPrintMsg::PrintMsg( 30583, pFTTSkillInfo->s_szName );	// 스킬을 삭제 하시겠습니까?
	cMessageBox::GetFirstMessageBox()->SetValue1( pTSkillInfo->s_nIndex );	// 스킬 인덱스 넘김
}

#ifdef CROSSWARS_SYSTEM
int CMainFrameContents::GetCrossInvenMaxCount() const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pBaseMng, 0 );
	CsBaseMng::sLIMIT* pLimit = nsCsFileTable::g_pBaseMng->GetLimit();
	SAFE_POINTER_RETVAL( pLimit, 0 );
	return pLimit->s_nMaxCrossInven;
}

int CMainFrameContents::GetCrossInvenCount() const
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_Inven_Cross* pInvenCross = g_pDataMng->GetInvenCross();
	SAFE_POINTER_RETVAL( pInvenCross, 0 );
	return pInvenCross->GetInvenSlotCount();
}

cItemInfo* CMainFrameContents::GetCrossInvenItem(int nCrossIndex) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, NULL );
	cData_Inven_Cross* pInvenCross = g_pDataMng->GetInvenCross();
	SAFE_POINTER_RETVAL( pInvenCross, NULL );
	cItemInfo* pCrossItem = pInvenCross->GetData( nCrossIndex );
	SAFE_POINTER_RETVAL( pCrossItem, NULL );
	if( !pCrossItem->IsEnable() )
		return NULL;
	return pCrossItem;
}

int CMainFrameContents::GetCrossTacticsMaxCount() const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pBaseMng, 0 );
	CsBaseMng::sLIMIT* pLimit = nsCsFileTable::g_pBaseMng->GetLimit();
	SAFE_POINTER_RETVAL( pLimit, 0 );
	return pLimit->s_nMaxCrossTactics;
}

int CMainFrameContents::GetCrossTacticsCount() const
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTactics();
	SAFE_POINTER_RETVAL( pCrossTactics, 0 );
	return pCrossTactics->GetCrossCnt();
}

DWORD CMainFrameContents::GetCrossTacticsID(int nCrossIndex) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTactics();
	SAFE_POINTER_RETVAL( pCrossTactics, 0 );
	cData_CrossTactics::sCross* pTactics = pCrossTactics->GetTactics( nCrossIndex );
	SAFE_POINTER_RETVAL( pTactics, 0 );
	return pTactics->s_dwBaseDigimonID;
}

std::string CMainFrameContents::GetCrossTacticsFileName(DWORD dwDigimonID) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, "" );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, "" );
	CsDigimon::sINFO* pDigimonInfo = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonInfo, "" );

	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, "" );
	SAFE_POINTER_RETVAL( g_pModelDataMng, "" );

	if( 0 != pDigimonInfo->s_dwModelID )
	{
		DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimonModelIdx( pDigimonInfo->s_dwModelID );
		return g_pModelDataMng->GetSmallModelIconFile( dwModelID );
	}

	return "";
}

cData_CrossTactics::sCross* CMainFrameContents::GetCrossTacticsInfo(int nCrossIndex) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, NULL );
	cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTactics();
	SAFE_POINTER_RETVAL( pCrossTactics, NULL );
	cData_CrossTactics::sCross* pTactics = pCrossTactics->GetTactics( nCrossIndex );
	SAFE_POINTER_RETVAL( pTactics, NULL );
	return pTactics;
}

bool CMainFrameContents::IsSelected(int nIndex) const
{
	std::list< sCrossInfo >::const_iterator it = m_lSelectedCross.begin();
	std::list< sCrossInfo >::const_iterator itEnd = m_lSelectedCross.end();
	for( ; it != itEnd; ++it )
	{
		if( nIndex == it->nCrossIndex )
			return true;
	}
	return false;
}

bool CMainFrameContents::IsCross() const
{
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, false );
	DWORD dwMyDigimon = pUser->GetBaseDigimonFTID();
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, false );
	return nsCsFileTable::g_pDigimonMng->IsCrossDigimon( dwMyDigimon );
}

bool CMainFrameContents::IsCrossLoader() const
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven_Cross* pCrossInven = g_pDataMng->GetInvenCross();
	SAFE_POINTER_RETVAL( pCrossInven, false );
	return pCrossInven->IsCrossLoader();
}

CDigimon* CMainFrameContents::GetRenderDigimon()
{
	if( !m_pRenderDigimon )
	{
		if( !m_lSelectedCross.empty() )
			_SetRenderCrossTactics();
	}
	return m_pRenderDigimon;
}

bool CMainFrameContents::SelectCrossTactics(int nCrossIndex)
{
	if( !_CheckSelectCrossTactics( nCrossIndex ) )
	{
		_SetCrossTactics();
		return false;
	}

	sCrossInfo sInfo;
	sInfo.nCrossIndex = nCrossIndex;
	sInfo.nCrossRankID = GetCrossTacticsID( nCrossIndex );
	m_lSelectedCross.push_back( sInfo );
	m_lSelectedCross.sort( sCrossInfo() );
	_SetCrossTactics();
	return true;
}

void CMainFrameContents::OnClickDigiCross()
{
	if( m_bWaitRecvCross )
		return;

	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pUser );
	DWORD dwMyDigimon = pUser->GetBaseDigimonFTID();
	SAFE_POINTER_RET( nsCsFileTable::g_pDigimonMng );
	// 현재 데리고 있는 디지몬이 크로스 중이면
	if( nsCsFileTable::g_pDigimonMng->IsCrossDigimon( dwMyDigimon ) )
		_ReleaseCrossTactics();
	else
		_CombineCrossTactics();
}

void CMainFrameContents::MoveCrossInvenItem( int nIconSlot, int nCrossIndex, int nIconCount )
{
	if( g_pDataMng )
		g_pDataMng->SendItemMove( nIconSlot, TO_INVENCROSS_SID( nCrossIndex ), nIconCount );
}

void CMainFrameContents::_SetCrossTactics()
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTactics();
	SAFE_POINTER_RET( pCrossTactics );
	pCrossTactics->ResetCrossIdx();

	// 선택된 크로스 디지몬이 한 마리일 때
	if( 1 == m_lSelectedCross.size() )
	{
		pCrossTactics->SetCrossCnt( 1 );
		std::list< sCrossInfo >::iterator it = m_lSelectedCross.begin();
		if( it != m_lSelectedCross.end() )
		{
			pCrossTactics->SetCrossEnable( true );
			pCrossTactics->GetCrossIdx()[0] = it->nCrossIndex;
			DWORD dwDigimonID = GetCrossTacticsID( it->nCrossIndex );
			pCrossTactics->SetCrossID( dwDigimonID );
		}
	}
	else
	{
		// 선택된 크로스 디지몬이 여러 마리일 때 - 조합찾기
		SAFE_POINTER_RET( nsCsFileTable::g_pDigimonMng );
		CsCrossDigimon::MAP* mapCombine = nsCsFileTable::g_pDigimonMng->GetCrossCombineMap();
		SAFE_POINTER_RET( mapCombine );
		CsCrossDigimon::MAP_IT it = mapCombine->begin();
		for( ; it != mapCombine->end(); ++it )
		{
			CsCrossDigimon* pFTCross = it->second;
			SAFE_POINTER_CON( pFTCross );
			CsCrossDigimon::sINFO* pFTCrossInfo = pFTCross->GetInfo();
			SAFE_POINTER_CON( pFTCrossInfo );
			if( pFTCrossInfo->s_nCrossRank != m_lSelectedCross.size() )
				continue;

			int nCombineIndex = 0;
			std::list< sCrossInfo >::iterator itSel = m_lSelectedCross.begin();
			std::list< sCrossInfo >::iterator itSelEnd = m_lSelectedCross.end();
			for( ; itSel != itSelEnd; ++itSel )
			{
				cData_CrossTactics::sCross* pCrossInfo = GetCrossTacticsInfo( itSel->nCrossIndex );
				SAFE_POINTER_CON( pCrossInfo );
				if( pCrossInfo->s_dwBaseDigimonID == pFTCrossInfo->s_nCrossDigimon[ nCombineIndex ] )
					pCrossTactics->GetCrossIdx()[ nCombineIndex++ ] = itSel->nCrossIndex;
			}

			if( pFTCrossInfo->s_nCrossRank == nCombineIndex )
			{
				pCrossTactics->SetCrossEnable( true );
				pCrossTactics->SetCrossCnt( nCombineIndex );
				pCrossTactics->SetCrossID( pFTCrossInfo->s_dwDigimonID );
			}
			else
				pCrossTactics->ResetCrossIdx();
		}
	}

	_SetRenderCrossTactics();
}

void CMainFrameContents::_SetRenderCrossTactics()
{
	if( m_pRenderDigimon )
	{
		if( g_pCharMng )
		{
			g_pCharMng->DeleteTempChar( m_pRenderDigimon );
			m_pRenderDigimon = NULL;
		}
	}

	if( m_lSelectedCross.empty() )
	{
		int nIndex = -1;
		ContentsStream kSend;
		kSend << nIndex;
		Notify( eChangeCrossTactics, kSend );
		return;
	}

	SAFE_POINTER_RET( g_pDataMng );
	cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTactics();
	SAFE_POINTER_RET( pCrossTactics );
	int nCrossID = pCrossTactics->GetCrossID();
	if( 0 != nCrossID )
	{
		// 렌더할 디지몬 만들기
		sCREATEINFO prop;
		prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
		prop.s_dwOptionPlag = OP_LIGHT;
		prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

		m_pRenderDigimon = g_pCharMng->AddTempDigimon( &prop, nCrossID, NULL, NiPoint3::ZERO, 0.0f, NULL );
		SAFE_POINTER_RET( m_pRenderDigimon );
		CsC_AlphaProp* pAlpha = m_pRenderDigimon->GetProp_Alpha();
		if( pAlpha )
			pAlpha->SetUseDistAlpha( false );
		m_pRenderDigimon->SetViewSight( true );
	}

	int* pCrossIndex = pCrossTactics->GetCrossIdx();
	ContentsStream kSend;
	kSend << pCrossIndex[0];
	Notify( eChangeCrossTactics, kSend );
}

void CMainFrameContents::_ReleaseCrossTactics()
{
	// 렌더 타겟 지우고
	if( m_pRenderDigimon )
	{
		if( g_pCharMng )
		{
			g_pCharMng->DeleteTempChar( m_pRenderDigimon );
			m_pRenderDigimon = NULL;
		}
	}
	// 해제 패킷 보낸다
	SAFE_POINTER_RET( g_pDataMng );
	cData_ServerSync* pServerSync = g_pDataMng->GetServerSync();
	SAFE_POINTER_RET( pServerSync );
	pServerSync->SendServer( cData_ServerSync::CROSSRECOVER_PARTNER, 0);
	cWindow::PlaySound( cWindow::SD_Bu4 );
	m_bWaitRecvCross = true;

	// 등록된 크로스 지워준다
	SAFE_POINTER_RET( g_pDataMng );
	cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();
	SAFE_POINTER_RET( pCrossTactics );
	pCrossTactics->ResetCrossIdx();
	m_lSelectedCross.clear();
}

void CMainFrameContents::_CombineCrossTactics()
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();
	SAFE_POINTER_RET( pCrossTactics );

	// 크로스 가능한지 확인
	if( !_CheckDigiCross() )
	{
		pCrossTactics->ResetCrossIdx();
		return;
	}

	// 크로스 가능
	if( pCrossTactics->GetCrossEnable() )
	{
		// 크라운 체크
		if( pCrossTactics->CheckCodeCrown() )
		{
			// 크로스 패킷 보낸다
			int nCnt = pCrossTactics->GetCrossCnt();
			SAFE_POINTER_RET( g_pDataMng );
			cData_ServerSync* pServerSync = g_pDataMng->GetServerSync();
			SAFE_POINTER_RET( pServerSync );
			pServerSync->SendServer( cData_ServerSync::CROSSCHANGE_PARTNER, 0, &nCnt, pCrossTactics->GetCrossIdx() ); 				
			cWindow::PlaySound( cWindow::SD_Bu4 );
			m_bWaitRecvCross = true;
		}
	}
}

bool CMainFrameContents::_CheckDigiCross()
{
	SAFE_POINTER_RETVAL( g_pMngCollector, false );
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS )
		return false;

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	if( pUser && pUser->IsBattle() )
	{
		cPrintMsg::PrintMsg( 16002 );
		return false;
	}

	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
	if( pDUser )
	{
		if( pDUser->IsRide() )
		{
			cPrintMsg::PrintMsg( 30014 );
			return false;
		}

		if( pDUser->IsCurJointProgress() )
		{
			cPrintMsg::PrintMsg( 10004 );
			return false;
		}
	}

	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
		return false;

	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_ServerSync* pServerSync = g_pDataMng->GetServerSync();
	if( pServerSync && pServerSync->IsChageDigimon() )
	{
		cWindow::PlaySound( cWindow::SD_MSS1 );
		return false;
	}

	//크로스워즈도 진화제한구역에선 진화 불가능하도록 수정 chu8820 14.04.01
	SAFE_POINTER_RETVAL( nsCsGBTerrain::g_pCurRoot, false );
	CsGBTerrainRoot::sINFO* pTerrainInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	SAFE_POINTER_RETVAL( pTerrainInfo, false );
	SAFE_POINTER_RETVAL( nsCsMapTable::g_pMapRegionMng, false );
	if( nsCsMapTable::g_pMapRegionMng->IsLimitEvoGroup( pTerrainInfo->s_dwMapID ) )
	{
		CsMap_LimitEvoGroup* pLimitEvo = nsCsMapTable::g_pMapRegionMng->GetLimitEvoGroup( pTerrainInfo->s_dwMapID );
		SAFE_POINTER_RETVAL( pLimitEvo, false );
		CsMap_LimitEvo::LIST* pList = pLimitEvo->GetList();
		SAFE_POINTER_RETVAL( pList, false );
		CsMap_LimitEvo::LIST_IT it = pList->begin();
		CsMap_LimitEvo::LIST_IT itEnd = pList->end();
		for( ; it != itEnd; ++it )
		{
			CsMap_LimitEvo::sINFO* pLimitEvoInfo = (*it)->GetInfo();
			SAFE_POINTER_CON( pLimitEvoInfo );
			NiPoint2 vLE( (float)pLimitEvoInfo->s_nCenterX, (float)pLimitEvoInfo->s_nCenterY );
			if( ( pDUser->GetPos2D() - vLE ).Length() < pLimitEvoInfo->s_nRadius )
			{
				cPrintMsg::PrintMsg( 30110 );
				return false;
			}
		}
	}

	if( 1 == m_lSelectedCross.size() )
		return true;

	std::list< sCrossInfo >::iterator itSel = m_lSelectedCross.begin();
	std::list< sCrossInfo >::iterator itSelEnd = m_lSelectedCross.end();
	for( ; itSel != itSelEnd; ++itSel )
	{
		cData_CrossTactics::sCross* pCrossInfo = GetCrossTacticsInfo( itSel->nCrossIndex );
		SAFE_POINTER_CON( pCrossInfo );
		if( 0 == pCrossInfo->s_nEnable )
		{
			cPrintMsg::PrintMsg( 30590 );			// 활성화가 안 된 디지몬이 있다
			cWindow::PlaySound( cWindow::SD_MSS1 );
			return false;
		}
	}

	return true;
}

bool CMainFrameContents::_CheckSelectCrossTactics(int nCrossIndex)
{
	if( IsCross() )
		return false;

	if( 0 == GetCrossTacticsID( nCrossIndex ) )
		return false;

	std::list< sCrossInfo >::iterator it = m_lSelectedCross.begin();
	std::list< sCrossInfo >::iterator itEnd = m_lSelectedCross.end();
	for( ; it != itEnd; )
	{
		if( nCrossIndex == it->nCrossIndex )
		{
			it = m_lSelectedCross.erase( it );
			return false;
		}
		else
			++it;
	}

	return true;
}

void CMainFrameContents::_OpenCrossLoader(void* pData)
{
	if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_TAMERSTATUS ) )
	{
		CloseTamerStatus();
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_NEW_TAMERSTATUS, 0, 4 );
	}
}

void CMainFrameContents::_ChangeCrossInvenSize(void* pData)
{
	SAFE_POINTER_RET( pData );
	int nCount = *static_cast< int* >( pData );
	ContentsStream kSend;
	kSend << nCount;
	Notify( eChangeCInvenSize, kSend );
}

void CMainFrameContents::_ChangeCrossDigimon(void* pData)
{
	m_bWaitRecvCross = false;
	Notify( eUpdateStatus );
}

#endif

#ifdef USE_BARCODE_REDER
void CMainFrameContents::_UseGameScanner(void* pData)
{
	SAFE_POINTER_RET( pData );
	bool bUseScanner = *static_cast< bool* >( pData );

	ContentsStream kSend;
	kSend << bUseScanner;
	Notify( eUseScanner, kSend );
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////
void CMainFrameContents::_OpenCloseDigimonStatus(void* pData)
{
	SAFE_POINTER_RET( g_pGameIF );
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_DIGIMONSTATUS ) )
		CloseDigimonStatus();
	else
	{
#ifdef LJW_TUTORIAL_RENEWAL_20191128
		if( m_pAdaptTutorial && m_pAdaptTutorial->IsTutorialPlaying_ClientMode() )
			return;
#endif

		g_pGameIF->GetDynamicIF( cBaseWindow::WT_NEW_DIGIMONSTATUS );
	}
}

void CMainFrameContents::_UpdateDigimonStat(void* pData)
{
	Notify( eUpdateStat );
}

void CMainFrameContents::_UpdateDigimonSkill(void* pData)
{
	Notify( eUpdateSkill );
}

void CMainFrameContents::_ChangeDigimon(void* pData)
{
	Notify( eChangeDigimon );
}

void CMainFrameContents::_RecvDigimonSkillLevelUp(void * pData)
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET(pData);
	GS2C_RECV_DIGIMON_SKILL_LEVEL_UP* pRecv = static_cast<GS2C_RECV_DIGIMON_SKILL_LEVEL_UP*>(pData);

	// 실패 떨어지면 창 닫고 서버랑 동기화
	if (pRecv->nResult != nsDigimonSkillLevelUpResult::Success)
	{
		cPrintMsg::PrintMsg(pRecv->nResult);
		CloseDigimonStatus();
	}

	SAFE_POINTER_RET(g_pCharMng);
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RET(pDUser);

	// 현재 디지몬과 다름
	if (pDUser->GetFTID() != pRecv->nDigimonSourceIndex)
	{
		CloseDigimonStatus();
		return;
	}

	cEvoUnit* pEvo = pDUser->GetCurAttributeEvoUnit();
	SAFE_POINTER_RET(pEvo);
	pEvo->m_nSkillLevel[pRecv->nSkillIndex] = pRecv->nSkillLevel;
	pEvo->m_nSkillPoint = pRecv->nSkillPoint;

	Notify(eUpdateSkill);

	if (pRecv->nResult == nsDigimonSkillLevelUpResult::Success)
	{
		CsDigimon::sINFO* pFTDigimon = GetDigimonFTInfo();
		SAFE_POINTER_RET(pFTDigimon);
		CsSkill::sINFO* pFTInfo = GetDigimonFTSkillInfo(pRecv->nSkillIndex);
		SAFE_POINTER_RET(pFTInfo);
		cPrintMsg::PrintMsg(30005, pFTDigimon->s_szName, pFTInfo->s_szName);
	}
#endif
}

void CMainFrameContents::OpenCloseHelpWindow()
{
	SAFE_POINTER_RET( g_pGameIF );
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGIMONSTATUS_HELP ) )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_DIGIMONSTATUS_HELP );
	else
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DIGIMONSTATUS_HELP );
}

void CMainFrameContents::CloseDigimonStatus()
{
	SAFE_POINTER_RET( g_pGameIF );
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_NEW_DIGIMONSTATUS );

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGIMONSTATUS_HELP ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DIGIMONSTATUS_HELP );
}

void CMainFrameContents::LevelUpDigimonSkill(int nIndex)
{
	cEvoUnit* pEvo = GetEvoUnit();
	SAFE_POINTER_RET( pEvo );

	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pUser );
	cSkill* pSkillMng = pUser->GetSkillMng();
	SAFE_POINTER_RET( pSkillMng );

	// 스킬 사용 불가
	if( !pSkillMng->IsUseSkill( pUser, nIndex ) )
		return;

	CsSkill::sINFO* pFTInfo = GetDigimonFTSkillInfo( nIndex );
	SAFE_POINTER_RET( pFTInfo );

	// 메모리 스킬은 레벨업 x
	if( pFTInfo->s_nMemorySkill )
		return;

	if( pEvo->m_nSkillPoint < pFTInfo->s_nLevelupPoint )
		return;

#ifdef SDM_DIGIMONSKILL_LV_EXPEND_20181206
	if( pEvo->m_nSkillLevel[ nIndex ] >= pEvo->m_nSkillMaxLevel[ nIndex ] )
		return;
#else
	if( pEvo->m_nSkillLevel[ nIndex ] >= pFTInfo->s_nMaxLevel )
		return;
#endif

	// 직접 레벨 올리고, 포인트 줄임
	pEvo->m_nSkillLevel[ nIndex ] += 1;
	pEvo->m_nSkillPoint -= pFTInfo->s_nLevelupPoint;

	// 서버에서 결과 안보냄
	CDigimonEvolveObj* pFTEvo = pUser->GetFTEvolCurObj();
	SAFE_POINTER_RET( pFTEvo );
	if( net::game )
		net::game->SendDigimonSkillLevelUp( pUser->GetUniqID(), pFTEvo->m_nEvoSlot, nIndex );

	Notify( eUpdateSkill );

	CsDigimon::sINFO* pFTDigimon = GetDigimonFTInfo();
	SAFE_POINTER_RET( pFTDigimon );
	cPrintMsg::PrintMsg( 30005, pFTDigimon->s_szName, pFTInfo->s_szName );
}

void CMainFrameContents::DeleteDigimonSkill(int nIndex)
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_ServerSync* pServerSync = g_pDataMng->GetServerSync();
	SAFE_POINTER_RET( pServerSync );

	if( pServerSync->IsEmptyRefCount( cData_ServerSync::MEMORY_SKILL_DELETE ) )	// 한번만 라이딩 해제기능 보내도록 검사.
	{
		SAFE_POINTER_RET(g_pCharMng);
		CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );

		CsSkill::sINFO* pFTInfo = GetDigimonFTSkillInfo( nIndex );
		SAFE_POINTER_RET( pFTInfo );
		if( nSkill::UNKNOWN_MEMORY_SKILL_TYPE == pFTInfo->s_nMemorySkill )
			return;

		// 노말 상태 인지 체크
		if( CDigimon::DIGIMON_NORMAL == pUser->GetDigimonState() )
		{
			pServerSync->SendServer( cData_ServerSync::MEMORY_SKILL_DELETE, 0 );
			cPrintMsg::PrintMsg( 30687, pFTInfo->s_szName );					// 스킬을 삭제 하시겠습니까?
			cMessageBox::GetFirstMessageBox()->SetValue1( pFTInfo->s_dwID );	// 스킬 코드 넘김
		}
	}
}

void CMainFrameContents::OpenEvolveDigimon(int nIndex)
{
	CDigimonEvolveInfo* pEvoInfo = GetEvoInfo();
	SAFE_POINTER_RET( pEvoInfo );
	CDigimonEvolveObj* pEvoObj = pEvoInfo->GetEvolveObjByEvoSlot( nIndex );
	SAFE_POINTER_RET( pEvoObj );

	// 디지몬상태창에서 진화 오픈 시도 - 조그레스 포함버전
	if( pEvoObj->m_nEvolutionTree == CDigimonUser::TREE_CAPSULE )
	{
		if( g_pGameIF )
		{
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_CAPSULE_EVOLUTION, 0, nIndex );
			return;
		}
	}

	SAFE_POINTER_RET( g_pDataMng );
	cData_Quest* pQuest = g_pDataMng->GetQuest();
	SAFE_POINTER_RET( pQuest );
	if( pEvoObj->m_nJoGressQuestCheck && pQuest->IsProcess( pEvoObj->m_nJoGressQuestCheck ) )
	{
		cPrintMsg::PrintMsg( 30506 );
		return;
	}

// 	SAFE_POINTER_RET( g_pCharMng );
// 	CDigimonUser* pUser = g_pCharMng->GetDigimonUser(0);
// 	SAFE_POINTER_RET( pUser );
// 
// 	// 오픈 레벨 - 아이템에서 레벨 체크함 + 슬롯은 레벨 상관 없이 뚫을 수 있음
// 	if( pEvoObj->m_nOpenLevel != 0 )
// 	{
// 		CDigimonUser::sBASE_STAT* pStat = pUser->GetBaseStat();
// 		SAFE_POINTER_RET( pStat );
// 		if( pStat->GetLevel() < pEvoObj->m_nOpenLevel )
// 		{
// 			cPrintMsg::PrintMsg( 13005 );			// 진화를 할 수 없습니다.
// 			return;
// 		}
// 	}

	SAFE_POINTER_RET( nsCsFileTable::g_pQuestMng );
	SAFE_POINTER_RET( g_pDataMng );
	// 오픈 퀘스트
	if( pEvoObj->m_nOpenQuest != 0 )
	{
		CsQuest* pQuestInfo = nsCsFileTable::g_pQuestMng->GetQuest( pEvoObj->m_nOpenQuest );
		SAFE_POINTER_RET( pQuestInfo );
		cData_Quest* pQuest = g_pDataMng->GetQuest();
		SAFE_POINTER_RET( pQuest );
		if( !pQuest->IsCompleate( pEvoObj->m_nOpenQuest ) )			
		{
			cPrintMsg::PrintMsg( 30507 );		//임시로 띄움
			return;
		}
	}

	// 오픈 아이템 조건이면.
	if( pEvoObj->m_nOpenItemTypeS != 0 )
	{
		//assert_cs( nsCsFileTable::g_pItemMng->IsItem( nsCsFileTable::g_pItemMng->TypeT_to_Disp( pEvolObj->m_nOpenItemTypeS ) ) );

		// 아이템 갯수 체크
		int nTypeL = 0, nTypeS = 0;
		CsItem::TypeS_to_TypeLS( pEvoObj->m_nOpenItemTypeS, nTypeL, nTypeS );
		// 가지고 있는 수량을 충족하면.
		cData_Inven* pInven = g_pDataMng->GetInven();
		SAFE_POINTER_RET( pInven );
		if( pInven->GetCount_Item_TypeLS( nTypeL, nTypeS ) >= pEvoObj->m_nOpenItemNum )
		{
			int nFirstSlot = pInven->GetFirstSlot_Item_TypeLS( nTypeL, nTypeS );
			if( g_pDataMng->IsItemUse( nFirstSlot ) == true )
			{
				CsDigimon::sINFO* pFTInfo = GetEvoDigimonFTInfo( nIndex );
				SAFE_POINTER_RET( pFTInfo );
				int nCount = pEvoObj->m_nOpenItemNum;
				SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
				DWORD dwDispID = nsCsFileTable::g_pItemMng->TypeT_to_Disp( pEvoObj->m_nOpenItemTypeS );
				CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( dwDispID );
				SAFE_POINTER_RET( pFTItem );
				CsItem::sINFO* pFTItemInfo = pFTItem->GetInfo();
				SAFE_POINTER_RET( pFTItemInfo );
#ifdef VERSION_USA
				//(요청사항)조그레스진화체에서 조그레스가 아닌 트리로 진화하려 할 때
				//메세지 출력하도록 수정 13.09.04 chu8820
				if( pEvoObj->m_nJoGressQuestCheck != 0 )
					cPrintMsg::PrintMsg( 30600, pFTItemInfo->s_szName, &nCount, pFTInfo->s_szName );
				else
					cPrintMsg::PrintMsg( 30034, pFTItemInfo->s_szName, &nCount, pFTInfo->s_szName );
#else
				cPrintMsg::PrintMsg( 30034, pFTItemInfo->s_szName, &nCount, pFTInfo->s_szName );
#endif						
				cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
				SAFE_POINTER_RET( pMBox );
				pMBox->SetValue1( pEvoObj->m_nEvoSlot );
				pMBox->SetValue2( pEvoObj->m_nOpenItemNum );
				pMBox->SetValue3( pEvoObj->m_nOpenItemTypeS );
			}
		}
		// 아니면
		else
		{
			SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
			DWORD dwDispID = nsCsFileTable::g_pItemMng->TypeT_to_Disp( pEvoObj->m_nOpenItemTypeS );
			CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( dwDispID );
			SAFE_POINTER_RET( pFTItem );
			CsItem::sINFO* pFTItemInfo = pFTItem->GetInfo();
			SAFE_POINTER_RET( pFTItemInfo );
			cPrintMsg::PrintMsg( 11040, pFTItemInfo->s_szName );	//	<tc:990>[str]@</tc> 아이템 보유개수가 부족합니다
		}
	}

	// 조건1 슬롯버튼 눌렀는데 합체 슬롯이면 퀘스트 메시지	ps) 위의 아이템조건 까지 있으면 두개를 수행
	if( pEvoObj->m_nChipsetType != 0 && pEvoObj->m_nOpenQuest != 0 ) 
		cPrintMsg::PrintMsg( 30507 );// 임시 (아직 사용할수없습니다.)
}

void CMainFrameContents::OpenRideDigimon(int nIndex)
{
	CDigimonEvolveInfo* pEvoInfo = GetEvoInfo();
	SAFE_POINTER_RET( pEvoInfo );
	CDigimonEvolveObj* pEvoObj = pEvoInfo->GetEvolveObjByEvoSlot( nIndex );
	SAFE_POINTER_RET( pEvoObj );

	SAFE_POINTER_RET( nsCsFileTable::g_pDigimonMng );
	CsRide* pFTRide = nsCsFileTable::g_pDigimonMng->GetRide( pEvoObj->m_dwID );
	CsRide::sINFO* pFTRideInfo = pFTRide->GetInfo();
	SAFE_POINTER_RET( pFTRideInfo );

	int nOpenInfoCnt = 0;

	for( int o = 0; o < FT_RIDE_OPENINFO; ++o )
	{
		CsRide::sINFO::sOPEN_INFO* pOpenInfo = &pFTRideInfo->s_OpenInfo[ o ];
		SAFE_POINTER_CON( pOpenInfo );
		SAFE_POINTER_CON( nsCsFileTable::g_pItemMng );
		int nItemId = nsCsFileTable::g_pItemMng->TypeT_to_Disp( pOpenInfo->s_nItemType_S );
		if( nItemId == 0 )				
			continue;

		int nTypeL = 0, nTypeS = 0;
		CsItem::TypeS_to_TypeLS( pOpenInfo->s_nItemType_S, nTypeL, nTypeS );

		// 아이템 갯수 체크
		SAFE_POINTER_CON( g_pDataMng );
		cData_Inven* pInven = g_pDataMng->GetInven();
		SAFE_POINTER_CON( pInven );
		if( pInven->GetCount_Item_TypeLS( nTypeL, nTypeS ) >= pOpenInfo->s_nNeedCount )						
		{
			int nFirstSlot = pInven->GetFirstSlot_Item_TypeLS( nTypeL, nTypeS );
			if( g_pDataMng->IsItemUse( TO_INVEN_SID( nFirstSlot ) ) == true )
			{
				SAFE_POINTER_CON( nsCsFileTable::g_pDigimonMng );
				CsDigimon* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pFTRideInfo->s_dwDigimonID );
				SAFE_POINTER_CON( pFTDigimon );
				CsDigimon::sINFO* pFTDigimonInfo = pFTDigimon->GetInfo();
				SAFE_POINTER_CON( pFTDigimonInfo );

				CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nItemId );
				SAFE_POINTER_CON( pFTItem );
				CsItem::sINFO* pFTItemInfo = pFTItem->GetInfo();
				SAFE_POINTER_CON( pFTItemInfo );
				cPrintMsg::PrintMsg( 30041, pFTItemInfo->s_szName, &pOpenInfo->s_nNeedCount, pFTDigimonInfo->s_szName );

				cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
				if( pMBox )
				{
					pMBox->SetValue1( nIndex );
					pMBox->SetValue2( pOpenInfo->s_nNeedCount );
					pMBox->SetValue3( pOpenInfo->s_nItemType_S );
				}
			}
			return;
		}

		++nOpenInfoCnt;
	}

	switch( nOpenInfoCnt )
	{
	case 1:
		{
			int nItemId = nsCsFileTable::g_pItemMng->TypeT_to_Disp( pFTRideInfo->s_OpenInfo[ 0 ].s_nItemType_S );
			CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nItemId );
			SAFE_POINTER_RET( pFTItem );
			CsItem::sINFO* pFTItemInfo = pFTItem->GetInfo();
			SAFE_POINTER_RET( pFTItemInfo );
			cPrintMsg::PrintMsg( 11040, pFTItemInfo->s_szName );
		}				
		break;
	case 2:
		{
			int nItemId1 = nsCsFileTable::g_pItemMng->TypeT_to_Disp( pFTRideInfo->s_OpenInfo[ 0 ].s_nItemType_S );
			int nItemId2 = nsCsFileTable::g_pItemMng->TypeT_to_Disp( pFTRideInfo->s_OpenInfo[ 1 ].s_nItemType_S );

			CsItem* pFTItem1 = nsCsFileTable::g_pItemMng->GetItem( nItemId1 );
			SAFE_POINTER_RET( pFTItem1 );
			CsItem::sINFO* pFTItemInfo1 = pFTItem1->GetInfo();
			SAFE_POINTER_RET( pFTItemInfo1 );

			CsItem* pFTItem2 = nsCsFileTable::g_pItemMng->GetItem( nItemId2 );
			SAFE_POINTER_RET( pFTItem2 );
			CsItem::sINFO* pFTItemInfo2 = pFTItem2->GetInfo();
			SAFE_POINTER_RET( pFTItemInfo2 );

			cPrintMsg::PrintMsg( 11041, pFTItemInfo1->s_szName, pFTItemInfo2->s_szName );
		}					
		break;
	default:
		assert_cs( false );
	}
}

void CMainFrameContents::RequestEvolveDigimon(int nIndex)
{
	CDigimonEvolveObj* pCurEvo = GetEvoCurObject();
	SAFE_POINTER_RET( pCurEvo );
	SAFE_POINTER_RET( g_pGameIF );
	for( int i = 0; i < MAX_EVOLUTION; ++i )
	{
		if( pCurEvo->m_nEvolutionList[ i ].nSlot != nIndex )
			continue;

		cQuickEvol* pEvol = g_pGameIF->GetQuickEvol();
		SAFE_POINTER_CON( pEvol );
		pEvol->Req_ActivateEvolve( i );
		break;
	}
}

void CMainFrameContents::RewardAttributeNormal(int nIndex)
{
	// 1, 0~2 사이 NewAttribute::Data
	int nState = nIndex - nsCsDigimonTable::AT_NONE;
	if( NewAttribute::Data > nState || NewAttribute::Virus < nState )
		return;

	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
	CsElementItem* pElemItem = nsCsFileTable::g_pItemMng->GetElementItem1( nState );
	SAFE_POINTER_RET( pElemItem );
	CsElementItem::sINFO* pElemItemInfo = pElemItem->GetInfo();
	SAFE_POINTER_RET( pElemItemInfo );

	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pElemItemInfo->s_dwItemID );
	SAFE_POINTER_RET( pFTItem );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RET( pFTInfo );

	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	if( 0 == pInven->GetEmptySlotCount() )
	{
		cPrintMsg::PrintMsg( 11015 );
		return;
	}

	cPrintMsg::PrintMsg( 30520, pFTInfo->s_szName);
	cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
	if( pMBox )
	{
		pMBox->SetValue1( NewAttribute::Digital );
		pMBox->SetValue2( nState );
	}
}

void CMainFrameContents::RewardAttributeNature(int nIndex)
{
	// 0, 0~10 사이 NewAttribute::NatualType
	if( NewAttribute::Steel < nIndex )
		return;

	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
	CsElementItem* pFTElemItem = nsCsFileTable::g_pItemMng->GetElementItem2( nIndex );
	SAFE_POINTER_RET( pFTElemItem );
	CsElementItem::sINFO* pFTElemInfo = pFTElemItem->GetInfo();
	SAFE_POINTER_RET( pFTElemInfo );

	CsItem* FTItem = nsCsFileTable::g_pItemMng->GetItem( pFTElemInfo->s_dwItemID );
	SAFE_POINTER_RET( FTItem );
	CsItem::sINFO* pFTInfo = FTItem->GetInfo();
	SAFE_POINTER_RET( pFTInfo );

	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	if( 0 == pInven->GetEmptySlotCount() )
	{
		cPrintMsg::PrintMsg( 11015 );
		return;
	}

	cPrintMsg::PrintMsg( 30520, pFTInfo->s_szName);
	cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
	if( pMBox )
	{
		pMBox->SetValue1( NewAttribute::Natual );
		pMBox->SetValue2( nIndex );
	}
}

bool CMainFrameContents::IsXAnti() const
{
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, false );
	CsDigimon* pFTDigimon = pUser->GetFTDigimon();
	SAFE_POINTER_RETVAL( pFTDigimon, false );
	return pFTDigimon->isX_Anti();
}

bool CMainFrameContents::IsXAnti(DWORD dwID) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, false );
	CsDigimon* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwID );
	SAFE_POINTER_RETVAL( pFTDigimon, false );
	return pFTDigimon->isX_Anti();
}

bool CMainFrameContents::IsUseSkill(int nIndex) const
{
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, false );
	cSkill* pSkillMng = pUser->GetSkillMng();
	SAFE_POINTER_RETVAL( pSkillMng, false );
	return pSkillMng->IsUseSkill( pUser, nIndex );
}

bool CMainFrameContents::IsSkillLevelUp(int nIndex)
{
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, false );
	cSkill* pSkillMng = pUser->GetSkillMng();
	SAFE_POINTER_RETVAL( pSkillMng, false );

	// 스킬 사용 불가
	if( !pSkillMng->IsUseSkill( pUser, nIndex ) )
		return false;

	cEvoUnit* pEvo = pUser->GetCurAttributeEvoUnit();
	SAFE_POINTER_RETVAL( pEvo, false );
	CsSkill::sINFO* pFTSkillInfo = GetDigimonFTSkillInfo( nIndex );
	SAFE_POINTER_RETVAL( pFTSkillInfo, false );

	// 메모리 스킬
	if( 0 < pFTSkillInfo->s_nMemorySkill )
		return false;

	// 스킬 포인트 부족
	if( pEvo->m_nSkillPoint < pFTSkillInfo->s_nLevelupPoint )
		return false;
	// 스킬 레벨 최대
#ifdef SDM_DIGIMONSKILL_LV_EXPEND_20181206
	if( pEvo->m_nSkillLevel[ nIndex ] == pEvo->m_nSkillMaxLevel[ nIndex ] )
		return false;
#else
	if( pEvo->m_nSkillLevel[ nIndex ] == pFTSkillInfo->s_nMaxLevel )
		return false;
#endif

	return true;
}

bool CMainFrameContents::IsUpperDigimonBaseStat(eAbilType eType) const
{
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, false );
	CDigimonUser::sUSER_STAT* pStat = dynamic_cast<CDigimonUser::sUSER_STAT*>(pUser->GetBaseStat());
	SAFE_POINTER_RETVAL( pStat, false );

	FMDigimon::sDINFO info;
	info.s_nDigimonID		= pUser->GetFTID();
	info.s_nLevel			= pStat->GetLevel();
	info.s_fGameOrgScale	= pUser->GetOrgGameScale();

	switch( eType )
	{
	case eHP:
		{
			int nBaseValue = FMDigimon::GetBaseMaxHP( &info );
			int nCurrentValue = pStat->GetMaxHP();
			return ( nCurrentValue > nBaseValue ) ? true : false;
		}
		break;
	case eDS:
		{
			int nBaseValue = FMDigimon::GetBaseMaxDS( &info );
			int nCurrentValue = pStat->GetMaxDS();
			return ( nCurrentValue > nBaseValue ) ? true : false;
		}
		break;
	case eAT:
		{
			int nBaseValue = FMDigimon::GetBaseAtt( &info );
			int nCurrentValue = pStat->GetAtt();
			return ( nCurrentValue > nBaseValue ) ? true : false;
		}
		break;
	case eAS:
		{
			float nBaseValue = FMDigimon::GetAttackSpeed( info.s_nDigimonID );
			float nCurrentValue = pStat->GetAttackSpeed();
			return ( nCurrentValue < nBaseValue ) ? true : false;
		}
		break;
	case eCT:
		{
			int nBaseValue = FMDigimon::GetBaseCr( &info );
			int nCurrentValue = pStat->GetCritical();
			return ( nCurrentValue > nBaseValue ) ? true : false;
		}
		break;
	case eHT:
		{
			int nBaseValue = FMDigimon::GetBaseHt( &info );
			int nCurrentValue = pStat->GetHitRate();
			return ( nCurrentValue > nBaseValue ) ? true : false;
		}
		break;
	case eDE:
		{
			int nBaseValue = FMDigimon::GetBaseDef( &info );
			int nCurrentValue = pStat->GetDef();
			return ( nCurrentValue > nBaseValue ) ? true : false;
		}
		break;
	case eBL:
		{
			int nCurrentValue = pStat->GetBL();
			return ( nCurrentValue > 0 ) ? true : false;
		}
		break;
	case eEV:
		{
			int nBaseValue = FMDigimon::GetBaseEv( &info );
			int nCurrentValue = pStat->GetEvade();
			return ( nCurrentValue > nBaseValue ) ? true : false;
		}
		break;
	}

	return false;
}

bool CMainFrameContents::IsActiveEvoSlot(CDigimonEvolveObj* pEvo, int nIndex) const
{
	SAFE_POINTER_RETVAL( pEvo, false );

	for( int i = 0; i < MAX_EVOLUTION; ++i )
	{
		if( pEvo->m_nEvolutionList[ i ].nSlot == nIndex )
			return true;
	}
	return false;
}

bool CMainFrameContents::IsActiveOpenRide(int nIndex) const
{
	CDigimonEvolveInfo* pEvoInfo = GetEvoInfo();
	SAFE_POINTER_RETVAL( pEvoInfo, false );

	CDigimonEvolveObj* pEvoObj = pEvoInfo->GetEvolveObjByEvoSlot( nIndex );
	SAFE_POINTER_RETVAL( pEvoObj, false );
	if( 0 == pEvoObj->m_nEnableSlot )
		return false;

	// 라이딩 아님
	if( !nsCsFileTable::g_pDigimonMng->IsRide( pEvoObj->m_dwID ) )
		return false;

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RETVAL( pUser, false );

	if( pUser->IsEvoPlag( nIndex, CDigimonUser::eNone ) )
		return false;
	// 오픈 안 된 상태
	if( pUser->IsEvoPlag( nIndex, CDigimonUser::eClosed ) )
		return false;
	// 이미 탄 상태
	if( pUser->IsEvoPlag( nIndex, CDigimonUser::eRide ) )
		return false;

	return true;
}

bool CMainFrameContents::IsOpenedRide(int nIndex) const
{
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RETVAL( pUser, false );

	// 오픈!
	if( pUser->IsEvoPlag( nIndex, CDigimonUser::eRide ) )
		return true;

	return false;
}

bool CMainFrameContents::IsActiveRewardNormal(int nState) const
{
	// 경험치 얻기
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, false );
	DWORD dwExpRate = pUser->GetFTAttributeExp( nState + nsCsDigimonTable::AT_NONE );

	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, false );
	CsDigimon* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pUser->GetFTID() );
	SAFE_POINTER_RETVAL( pFTDigimon, false );
	CsDigimon::sINFO* pFTDInfo = pFTDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pFTDInfo, false );

	// 인덱스가 내속성에 해당되면
	if( nState == pFTDInfo->s_eAttributeType - nsCsDigimonTable::AT_NONE )
	{
		if( ( 1 == ( dwExpRate / 10000 ) )										// 경험치 완료인가?
			&& ( nsCsDigimonTable::AT_NONE != pFTDInfo->s_eAttributeType )		// 1~5 값중 2~4 인가?
			&& ( nsCsDigimonTable::AT_UNIDENTIFIED != pFTDInfo->s_eAttributeType ) )
			return true;
	}

	return false;
}

bool CMainFrameContents::IsActiveRewardNature(int nState) const
{
	// 경험치 얻기
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, false );
	DWORD dwNatureExpRate = pUser->GetFTNatureExp( nState );

	// 인덱스가 내속성에 해당되면
	if( pUser->IsCurrentElement( nState ) )
	{
		if( nsCsDigimonTable::NT_NONE != nState + nsCsDigimonTable::NT_ICE)	// 무속성이 아니면
		{
			// 경험치 완료인가?
			if( 1 == ( dwNatureExpRate / 10000 ) )
				return true;
		}
	}
	return false;
}

bool CMainFrameContents::IsActiveDigimonParts(int const& nParts) const
{
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, false );
	return pUser->IsPartsApplied( nParts );
}

void CMainFrameContents::GetDigimonSizeInfo(int& nTall, float& fPercent, bool& bIsBuff)
{
	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pUser );
	
	float fToolHeight = pUser->GetToolHeight_Int();
	nTall = DmCS::StringFn::FloatRounding( fToolHeight );
	fPercent = DmCS::StringFn::FloatRounding(pUser->GetGameScale()*100.0f, 3);
	
	cBuffData* pBuff = pUser->GetBuff();
	if( !pBuff )
		bIsBuff = false;
	else
		bIsBuff = pBuff->IsBuffData( nsBuff::BK_CANDY_SIZE_CHANGE );
}

void CMainFrameContents::GetDigimonRideInfo(bool& bIsRide, int& nImageIndex)
{
	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pUser );

	CsDigimon::sINFO* pFTInfo = GetDigimonFTInfo();
	SAFE_POINTER_RET( pFTInfo );

	SAFE_POINTER_RET( nsCsFileTable::g_pDigimonMng );
	// 현재 개체는 탈수 있는 놈인가
	if( nsCsFileTable::g_pDigimonMng->IsRide( pFTInfo->s_dwDigimonID ) )
	{
		bIsRide = true;

		// 뚫려 있는가
		if( pUser->IsEvoPlag( CDigimonUser::eRide ) )
			nImageIndex = 0;
		else
			nImageIndex = 1;
	}
	else
	{
		// 다른 개체 중에 내가 탈수 있는 놈이 존재 하는가?
		SAFE_POINTER_RET( nsCsFileTable::g_pEvolMng );
		CDigimonEvolveInfo* pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( pUser->GetBaseDigimonFTID() );
		SAFE_POINTER_RET( pFTEvolInfo );
		for( int i = 0; i < nLimit::EvoUnit; ++i )
		{
			CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( i );
			if( pFTEvolObj == NULL )
				continue;

			if( pFTEvolObj->m_nEnableSlot == 0 )
				continue;

			if( nsCsFileTable::g_pDigimonMng->IsRide( pFTEvolObj->m_dwID ) == false )
				continue;

			bIsRide = true;
			nImageIndex = 2;
			break;
		}
	}
}

void CMainFrameContents::GetDigimonStatInfo(eAbilType eType, int& nType, int& nBase, int& nChipset, int& nFriendShip)
{
	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pDigimonUser );
	CDigimonUser::sUSER_STAT* pDStat = GetDigimonBaseStat();
	SAFE_POINTER_RET( pDStat );
	FMDigimon::sDINFO info;
	info.s_nDigimonID		= pDigimonUser->GetFTID();
	info.s_nLevel			= pDStat->GetLevel();
	info.s_fGameOrgScale	= pDigimonUser->GetOrgGameScale();

	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTamerUser );
	CTamerUser::sUSER_STAT* pTStat = dynamic_cast< CTamerUser::sUSER_STAT* >( pTamerUser->GetBaseStat() );
	SAFE_POINTER_RET( pTStat );
	FMTamer::sTINFO TInfo;
	TInfo.s_nTamerID = pTamerUser->GetModelID();
	TInfo.s_nLevel = pTStat->GetLevel();
	TInfo.s_nDigimonFriendShip = pDStat->GetFriendShip();

	int nEvol = 0;

	switch( eType )
	{
	case eHP:
		nType = APPLY_MAXHP;
		nBase = FMDigimon::GetBaseMaxHP( &info );
		nEvol = FMDigimon::GetEvolMaxHP( &info );
		nChipset = FMDigimon::GetChipsetMaxHP( &info );
		nFriendShip = FMTamer::GetTotalMaxHP_FriendShip( &TInfo );
		break;
	case eDS:
		nType = APPLY_MAXDS;
		nBase = FMDigimon::GetBaseMaxDS( &info );
		nEvol = FMDigimon::GetEvolMaxDS( &info );
		nChipset = FMDigimon::GetChipsetMaxDS( &info );
		nFriendShip = FMTamer::GetTotalMaxDS_FriendShip( &TInfo );
		break;
	case eAT:
		nType = APPLY_AP;
		nBase = FMDigimon::GetBaseAtt( &info );
		nEvol = FMDigimon::GetEvolAtt( &info );
		nChipset = FMDigimon::GetChipsetAtt( &info );
		nFriendShip = FMTamer::GetTotalAtt_FriendShip( &TInfo );
		break;	
	case eAS:
		nType = APPLY_AS;
		nBase = FMDigimon::GetAttackSpeed( info.s_nDigimonID ) * 1000;
		nEvol = 0;
		nChipset = FMDigimon::GetChipsetAs( &info );
		nFriendShip = 0;
		break;	
	case eCT:
		nType = APPLY_CA;
		nBase = FMDigimon::GetBaseCr( &info );
		nEvol = 0;
		nChipset = FMDigimon::GetChipsetCr( &info );
		nFriendShip = 0;
		break;	
	case eHT:
		nType = APPLY_HT;
		nBase = FMDigimon::GetBaseHt( &info );
		nEvol = 0;
		nChipset = FMDigimon::GetChipsetHt( &info );
		nFriendShip = 0;
		break;	
	case eDE:
		nType = APPLY_DP;
		nBase = FMDigimon::GetBaseDef( &info );
		nEvol = FMDigimon::GetEvolDef( &info );
		nChipset = FMDigimon::GetChipsetDef( &info );
		nFriendShip = FMTamer::GetTotalDef_FriendShip( &TInfo );
		break;	
	case eBL:
		nType = APPLY_BL;	
		nBase =  0;
		nEvol = 0;
		nChipset = 0;
		nFriendShip = 0;
		break;
	case eEV:
		nType = APPLY_EV;
		nBase = FMDigimon::GetBaseEv( &info );
		nEvol = 0;
		nChipset = FMDigimon::GetChipsetEv( &info );
		nFriendShip = 0;
		break;
	default:
		assert_cs( false );
	}

	//덱 적용 효과 버프
	int nUsingDeck = 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_USE_DECK, &nUsingDeck );

	if( nUsingDeck != INT_MIN )
	{
		cBuffData* pBuff = pTamerUser->GetBuff();
		SAFE_POINTER_RET( pBuff );
		cBuffData::LIST_ENCY_BUFF* pEncyBuff = pBuff->_GetEncyBuff();
		SAFE_POINTER_RET( pEncyBuff );
		for( int i = 0 ; i < ENCY_MAX_OPTION ; i++ )
		{
			std::pair<int,USHORT> sdfOpt;
			sdfOpt.first = i;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_OPTION, &sdfOpt );
			USHORT nOption = sdfOpt.second;

			std::pair<int,USHORT> sdfCon;
			sdfCon.first = i;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_CONDITION, &sdfCon );
			USHORT nCondition = sdfCon.second;

			std::pair<int,USHORT> sdfVal;
			sdfVal.first = i;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_ENCYCLOPEDIA_VAL, &sdfVal );
			USHORT nVal = sdfVal.second;

			if( nOption == 0 )
				continue;
			if( nCondition == EncyclopediaContents::COND_ACTIVE_TIME )
			{
				cBuffData::LIST_ENCY_BUFF_IT it = pEncyBuff->begin();
				cBuffData::LIST_ENCY_BUFF_IT itEnd = pEncyBuff->end();

				bool bBuff = false;
				for( ; it != itEnd ; ++it )
				{
					if( it->s_nOptIdx == i )
						bBuff = true;
				}
				if( !bBuff )
					continue;
			}

			switch(nOption)//옵션 별로 적용하는 방식 다름 - 서버 계산에 따라감
			{
			case EncyclopediaContents::DECK_HP_UP:
				if( nType == APPLY_MAXHP )
				{
					nBase += (int)( nBase * nVal ) / 100;
					nEvol += (int)( nEvol * nVal ) / 100;
					nFriendShip += (int)( nFriendShip * nVal ) / 100;
				}
				break;
			case EncyclopediaContents::DECK_ATK_SPEED:
				if( nType == APPLY_AS )
				{
					nBase -= (int)( nBase * nVal ) / 100;
				}
				break;
			case EncyclopediaContents::DECK_ATTACK_UP:
			case EncyclopediaContents::DECK_SKILL_UP:
			case EncyclopediaContents::DECK_CRIT_UP:
			case EncyclopediaContents::DECK_SKILL_COOL:
				break;
			default:
				break;
			}
		}
	}

	nBase += nEvol;
}

void CMainFrameContents::GetDigimonNatureInfo(int nMyType, int& nUpType, int& nDownType)
{
	SAFE_POINTER_RET( nsCsFileTable::g_pNatureMng );
	const int nNatureCount = 11;
	for( int i = 0; i < nNatureCount; ++i )
	{
		int nGab = nsCsFileTable::g_pNatureMng->GetValue( nMyType + nsCsDigimonTable::NT_ICE, i + nsCsDigimonTable::NT_ICE );
		if( 0 < nGab )
			nUpType = i;
		else if( 0 > nGab )
			nDownType = i;
	}
}

int CMainFrameContents::GetDigimonHatchLv() const
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_PostLoad::sDATA* pDigimonData = g_pDataMng->GetPostLoad()->GetDigimonData();
	SAFE_POINTER_RETVAL( pDigimonData, 0 );
	return pDigimonData->s_HatchLevel;
}

int CMainFrameContents::GetNormalAttributeType() const
{
	CsDigimon::sINFO* pFTInfo = GetDigimonFTInfo();
	SAFE_POINTER_RETVAL( pFTInfo, 0 );
	return pFTInfo->s_eAttributeType - nsCsDigimonTable::AT_NONE;
}

int CMainFrameContents::GetNatureAttributeType() const
{
	CsDigimon::sINFO* pFTInfo = GetDigimonFTInfo();
	SAFE_POINTER_RETVAL( pFTInfo, 0 );
	return pFTInfo->s_eBaseNatureType - nsCsDigimonTable::NT_ICE;
}

DWORD CMainFrameContents::GetNormalAttributeExp(int nState) const
{
	SAFE_POINTER_RETVAL( g_pCharMng, 0 );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, 0 );
	return pUser->GetFTAttributeExp( nState + nsCsDigimonTable::AT_NONE );
}

DWORD CMainFrameContents::GetNatureAttributeExp(int nState) const
{
	SAFE_POINTER_RETVAL( g_pCharMng, 0 );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, 0 );
	return pUser->GetFTNatureExp( nState );
}

std::string CMainFrameContents::GetEvoFileName(int nIndex) const
{
	CsDigimon::sINFO* pFTInfo = GetEvoDigimonFTInfo( nIndex );
	SAFE_POINTER_RETVAL( pFTInfo, "" );
	SAFE_POINTER_RETVAL( g_pModelDataMng, "" );
	return g_pModelDataMng->GetEvoModelIconFile( pFTInfo->s_dwModelID );
}

CsDigimon::sINFO* CMainFrameContents::GetDigimonFTInfo() const
{
	SAFE_POINTER_RETVAL( g_pCharMng, NULL );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, NULL );
	CsDigimon* pFTDigimon = pUser->GetFTDigimon();
	SAFE_POINTER_RETVAL( pFTDigimon, NULL );
	return pFTDigimon->GetInfo();
}

CsDigimon::sINFO* CMainFrameContents::GetEvoDigimonFTInfo(int nIndex) const
{
	CDigimonEvolveInfo* pEvoInfo = GetEvoInfo();
	SAFE_POINTER_RETVAL( pEvoInfo, NULL );
	CDigimonEvolveObj* pEvoObj = pEvoInfo->GetEvolveObjByEvoSlot( nIndex );
	SAFE_POINTER_RETVAL( pEvoObj, NULL );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, NULL );
	CsDigimon* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pEvoObj->m_dwID );
	SAFE_POINTER_RETVAL( pFTDigimon, NULL );
	return pFTDigimon->GetInfo();
}

CDigimonUser::sUSER_STAT* CMainFrameContents::GetDigimonBaseStat() const
{
	SAFE_POINTER_RETVAL( g_pCharMng, NULL );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, NULL );
	return dynamic_cast<CDigimonUser::sUSER_STAT*>(pUser->GetBaseStat());
}

CDigimonUser::sENCHENT_STAT* CMainFrameContents::GetDigimonEnchantStat() const
{
	SAFE_POINTER_RETVAL( g_pCharMng, NULL );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, NULL );
	return dynamic_cast<CDigimonUser::sENCHENT_STAT*>(pUser->GetEnchantStat());
}

cEvoUnit* CMainFrameContents::GetEvoUnit() const
{
	SAFE_POINTER_RETVAL( g_pCharMng, NULL );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, NULL );
	return pUser->GetCurAttributeEvoUnit();
}

CDigimonEvolveObj* CMainFrameContents::GetEvoCurObject() const
{
	SAFE_POINTER_RETVAL( g_pCharMng, NULL );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, NULL );
	return pUser->GetFTEvolCurObj();
}

CDigimonEvolveInfo* CMainFrameContents::GetEvoInfo() const
{
	SAFE_POINTER_RETVAL( g_pCharMng, NULL );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, NULL );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pEvolMng, NULL );
	DWORD dwBaseID = pUser->GetBaseDigimonFTID();
	return nsCsFileTable::g_pEvolMng->GetEvolveInfo( dwBaseID );
}

CsSkill::sINFO* CMainFrameContents::GetDigimonFTSkillInfo(int nIndex)
{
	SAFE_POINTER_RETVAL( g_pCharMng, NULL );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pUser, NULL );
	cSkill* pSkillMng = pUser->GetSkillMng();
	SAFE_POINTER_RETVAL( pSkillMng, NULL );
	cSkill::sINFO* pSkillInfo = pSkillMng->GetSkill( nIndex );
	SAFE_POINTER_RETVAL( pSkillInfo, NULL );
	CsSkill* pFTSkill = pSkillInfo->s_pFTSkill;
	SAFE_POINTER_RETVAL( pFTSkill, NULL );
	return pFTSkill->GetInfo();
}


//////////////////////////////////////////////////////////////////////////
// Target UI
void CMainFrameContents::_SetTargetUI(void* pData)
{
	SAFE_POINTER_RET( pData );
	UINT nTargetUIDX = *static_cast< UINT* >( pData );

	CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUIDX );
	SAFE_POINTER_RET( pTarget );

	switch( pTarget->GetLeafRTTI() )
	{
	case RTTI_MONSTER:
		{
			m_pTarget = pTarget;
			UINT nFTID = m_pTarget->GetFTID();
			bool bIsOwner = _CheckTargetOwner( m_pTarget );
			ContentsStream kSend;
			kSend << nFTID << bIsOwner;
			Notify( eSetTargetMonster, kSend );
		}
		break;
	case RTTI_EMPLOYMENT:
		{
			_ConnectBuffInfo( NULL );
			Notify( eReleaseTarget );
		}
		return;
	case RTTI_TAMER_USER:
	case RTTI_DIGIMON_USER:
	default:
		{
			m_pTarget = pTarget;
			Notify( eSetTarget );
		}
		break;
	}

	ContentsStream kSend;
	kSend << nTargetUIDX;
	NotifyContentsAndStream( CONTENTS_EVENT::EstreamEvt_MainFrame_SetTarget, kSend );

	_ConnectBuffInfo( m_pTarget );		// 버프창 연결

	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );
	if( net::game )
		net::game->SendSetTarget( pUser->GetUniqID(), m_pTarget->GetUniqID() );
}

void CMainFrameContents::_ReleaseTargetUI(void* pData)
{
	CloseTargetUI();

	Notify( eReleaseTarget );
}

void CMainFrameContents::_UpdateTargetUI(void* pData)
{
// 	SAFE_POINTER_RET( pData );
// 	std::pair< int, float > pairRecv = *static_cast< std::pair< int, float >* >( pData );
// 
// 	CsC_AvObject* pTarget = g_pMngCollector->GetObject( pairRecv.first );
// 	SAFE_POINTER_RET( pTarget );
// 
// 	SAFE_POINTER_RET( m_pTarget );
// 	if( m_pTarget->GetUniqID() != pairRecv.first )
// 		return;
// 
// 	ContentsStream send;
// 	send << pairRecv.second;
// 	Notify( eUpdateTarget, send );
}

void CMainFrameContents::CloseTargetUI()
{
	SAFE_POINTER_RET( m_pTarget );

	UINT nTargetUID = m_pTarget->GetUniqID();
	ContentsStream kSend;
	kSend << nTargetUID;
	NotifyContentsAndStream( CONTENTS_EVENT::EstreamEvt_MainFrame_ReleaseTarget, kSend );

	// 버프 연결 해제
	_ConnectBuffInfo( NULL );

	// close cPopUpWindow
	if( g_pGameIF )
	{
		cPopUpWindow* pPopUpWin = g_pGameIF->GetPopup();
		if( pPopUpWin )
		{
			pPopUpWin->ClosePopup( cPopUpWindow::OTHER_TAMER );
			pPopUpWin->ClosePopup( cPopUpWindow::OTHER_DIGIMON );
		}
	}

	m_pTarget = NULL;

	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );
	if( net::game )
		net::game->SendSetTarget( pUser->GetUniqID(), 0 );
}

void CMainFrameContents::OpenPopUpWindow()
{
	SAFE_POINTER_RET( m_pTarget );

	SAFE_POINTER_RET( g_pGameIF );
	cPopUpWindow* pPopUpWin = g_pGameIF->GetPopup();
	SAFE_POINTER_RET( pPopUpWin );

	switch( m_pTarget->GetLeafRTTI() )
	{
	case RTTI_TAMER:
		pPopUpWin->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::OTHER_TAMER, m_pTarget->GetUniqID() );
		break;
	case RTTI_DIGIMON:
		pPopUpWin->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::OTHER_DIGIMON, m_pTarget->GetUniqID() );
		break;
	}
}

bool CMainFrameContents::ReleaseTarget()
{
	SAFE_POINTER_RETVAL( g_pCharResMng, false );
	SAFE_POINTER_RETVAL( g_pCharResMng->GetTargetMark(), false );
	g_pCharResMng->ReleaseTargetMark( true );
	return true;
}

void CMainFrameContents::_ConnectBuffInfo(CsC_AvObject* pTarget)
{
	SAFE_POINTER_RET( g_pGameIF );
	cBuffInfo* pBuffInfo = g_pGameIF->GetBuffInfo( IF_BUFF_TARGET );
	SAFE_POINTER_RET( pBuffInfo );
	cBuffData* pBuffData = NULL;
	if( !pTarget )
	{
		// 버프 해제
		pBuffInfo->SetBuffData( pBuffData );
		return;
	}

	// 버프 연결
	switch( pTarget->GetLeafRTTI() )
	{
	case RTTI_TAMER:
	case RTTI_TAMER_USER:
		pBuffData = dynamic_cast< CTamer* >( pTarget )->GetBuff();
		break;
	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
		pBuffData = dynamic_cast< CDigimon* >( pTarget )->GetBuff();
		break;
	case RTTI_MONSTER:
		pBuffData = dynamic_cast< CMonster* >( pTarget )->GetBuff();
		break;
	}

	pBuffInfo->SetBuffData( pBuffData );
}

bool CMainFrameContents::_CheckTargetOwner(CsC_AvObject* pTarget)
{
	SAFE_POINTER_RETVAL( pTarget, false );

	// 몬스터 소유권자 존재 확인
	uint nOwnerID = dynamic_cast< CMonster* >( pTarget )->GetOwnerID();

	// 소유권자 없음
	if( 0 == nOwnerID )
		return true;

	if( g_pCharMng )
	{
		CTamerUser* pUser = g_pCharMng->GetTamerUser();
		// 자신이 소유권자
		if( pUser && pUser->GetUniqID() == nOwnerID )
			return true;
	}

	return false;
}

CsC_AvObject* CMainFrameContents::GetTargetObject() const
{
	return m_pTarget;
}

void CMainFrameContents::GetTargetName(std::wstring& wsName, std::wstring& wsPenName)
{
	// 타겟 이름 초기화
	wsName = L"";
	wsPenName = L"";			// Pen Name 없을 수도 있다 - 안 띄워야 함

	SAFE_POINTER_RET( m_pTarget );

	switch( m_pTarget->GetLeafRTTI() )
	{
	case RTTI_NPC:
		{
			CsNpc* pFTNpc = dynamic_cast< CNpc* >( m_pTarget )->GetFTNpc();
			if( pFTNpc )
			{
				CsNpc::sINFO* pFTNpcInfo = pFTNpc->GetInfo();
				if( pFTNpcInfo )
				{
					wsName = pFTNpcInfo->s_szName;
					wsPenName = pFTNpcInfo->s_szPenName;
				}
			}
		}
		break;
	case RTTI_MONSTER:
		{
			CsMonster* pFTMonster = dynamic_cast< CMonster* >( m_pTarget )->GetMonsterFT();
			if( pFTMonster )
			{
				CsMonster::sINFO* pFTMonInfo = pFTMonster->GetInfo();
				if( pFTMonInfo )
				{
					wsName = pFTMonInfo->s_szName;
					wsPenName = pFTMonInfo->s_szPenName;
				}
			}
		}
		break;
	default:
		wsName = m_pTarget->GetName();
		break;
	}
}

void CMainFrameContents::GetTargetStat(int& nLevel, float& fHpRate)
{
	SAFE_POINTER_RET( m_pTarget );

	CsC_AvObject::sBASE_STAT* pBaseStat = m_pTarget->GetBaseStat();
	SAFE_POINTER_RET( pBaseStat );

	// 레벨
	nLevel = pBaseStat->GetLevel();
	if( CsC_AvObject::INVALIDE_STAT == nLevel )
		nLevel = 0;

	// Hp
	if( pBaseStat->GetHP() != CsC_AvObject::INVALIDE_STAT )
	{
		fHpRate = pBaseStat->GetHPRate();
		return;
	}
	fHpRate = pBaseStat->GetHPRate();
}

std::string CMainFrameContents::GetTargetImgFile()
{
	SAFE_POINTER_RETVAL( m_pTarget, "" );
	UINT nModelID = m_pTarget->GetModelID();
	SAFE_POINTER_RETVAL( g_pModelDataMng, "" );
	std::string strImgPath = g_pModelDataMng->GetLargeModelIconFile( nModelID );

	if( CsFPS::CsFPSystem::IsExist( 0, strImgPath.c_str() ) )
		return strImgPath;
	else
		return "Data\\Dummy\\dummyL.tga";
}

UINT CMainFrameContents::GetExpireTime() const
{
	SAFE_POINTER_RETVAL( m_pTarget, 0 );

	if( RTTI_MONSTER != m_pTarget->GetLeafRTTI() )
		return 0;

	return dynamic_cast< CMonster* >( m_pTarget )->GetExpireTimeTS();
}

UINT CMainFrameContents::GetTotalExpireTime() const
{
	SAFE_POINTER_RETVAL( m_pTarget, 0 );

	if( RTTI_MONSTER != m_pTarget->GetLeafRTTI() )
		return 0;

	return dynamic_cast< CMonster* >( m_pTarget )->GetTotalExpireTime();
}

CsMonster::sINFO* CMainFrameContents::GetMonsterFTInfo(DWORD const& dwDigimonID) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pMonsterMng, NULL );
	CsMonster* pFTMonster = nsCsFileTable::g_pMonsterMng->GetMonster( dwDigimonID );
	SAFE_POINTER_RETVAL( pFTMonster, NULL );
	return pFTMonster->GetInfo();
}

int CMainFrameContents::GetNatureAttributeType(DWORD const& dwDigimonID) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, -1 );
	CsDigimon* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDigimonID );
	SAFE_POINTER_RETVAL( pFTDigimon, -1 );
	CsDigimon::sINFO* pFTInfo = pFTDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pFTInfo, -1 );

	return pFTInfo->s_eBaseNatureType;
}

int CMainFrameContents::GetNormalAttributeType(DWORD const& dwDigimonID) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, -1 );
	CsDigimon* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDigimonID );
	SAFE_POINTER_RETVAL( pFTDigimon, -1 );
	CsDigimon::sINFO* pFTInfo = pFTDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pFTInfo, -1 );

	return pFTInfo->s_eAttributeType;
}

int CMainFrameContents::GetNatureAttributeIndex(DWORD const& dwDigimonID) const
{
	int nType = GetNatureAttributeType( dwDigimonID );
	if( -1 == nType )
		return -1;

	return nType - nsCsDigimonTable::NT_ICE;
}

int CMainFrameContents::GetNormalAttributeIndex(DWORD const& dwDigimonID) const
{
	int nType = GetNormalAttributeType( dwDigimonID );
	if( -1 == nType )
		return -1;

	return nType - nsCsDigimonTable::AT_NONE;
}

CMainFrameContents::eCompareResult CMainFrameContents::GetNatureCompare(DWORD const& dwDigimonID) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, eCompError );
	CsDigimon::sINFO* pDUserInfo = GetDigimonFTInfo();
	SAFE_POINTER_RETVAL( pDUserInfo, eCompError );
	CsDigimonMng::eCOMPARE eFTCompare = nsCsFileTable::g_pDigimonMng->NatureCompare( pDUserInfo->s_dwDigimonID, dwDigimonID );

	switch( eFTCompare )
	{
	case CsDigimonMng::UP:
	case CsDigimonMng::BEST:	return eUpper;
	case CsDigimonMng::EQUAL:	return eEqual;
	case CsDigimonMng::DOWN:
	case CsDigimonMng::WORST:	return eLower;
	}
	return eCompError;
}

CMainFrameContents::eCompareResult CMainFrameContents::GetNormalCompare(DWORD const& dwDigimonID) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng, eCompError );
	CsDigimon::sINFO* pDUserInfo = GetDigimonFTInfo();
	SAFE_POINTER_RETVAL( pDUserInfo, eCompError );
	CsDigimonMng::eCOMPARE eFTCompare = 
		nsCsFileTable::g_pDigimonMng->AttibuteCompare( pDUserInfo->s_dwDigimonID, dwDigimonID );
	switch( eFTCompare )
	{
	case CsDigimonMng::UP:
	case CsDigimonMng::BEST:	return eUpper;
	case CsDigimonMng::EQUAL:	return eEqual;
	case CsDigimonMng::DOWN:
	case CsDigimonMng::WORST:	return eLower;
	}
	return eCompError;
}