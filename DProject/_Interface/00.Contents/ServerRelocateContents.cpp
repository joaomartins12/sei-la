#include "StdAfx.h"
#include "ServerRelocateContents.h"

#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../Adapt/AdaptCommunity.h"
#include "../Adapt/AdaptUnionStore.h"
#include "../Adapt/AdaptTrade.h"
#include "../Adapt/AdaptMacroProtectSystem.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int const CServerRelocateContents::IsContentsIdentity(void)
{
	return E_CT_SERVERCHANGE_CONTENTS;
}

CServerRelocateContents::CServerRelocateContents(void):m_fBackupDistMin(0.0f),m_nCurrentPageType(eNonePage),m_fBackupDistMax(0.0f)
,m_AdaptUnionStore(0),m_AdaptCommunity(0),m_nChangeServerIdx(-1),m_nUseItemInvenPos(-1),m_AdaptTrade(0),m_bCheckedShareHouseLimit(false),m_AdaptMacroProtect(0)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_SERVER_RELOCATE_PAGE_UI, this, &CServerRelocateContents::_OpenUIPage );
	// 이전 하려는 서버에 생성 가능한 이름인지에 대한 결과값
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_SERVER_RELOCATE_CHECK_TAMERNAME_RESULT, this, &CServerRelocateContents::_RecvCheckTamerName_Result );
	// 이전하기 위해 서버에 요청 패킷에대한 결과값
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_SERVER_RELOCATE_ITEMUSE_RESULT, this, &CServerRelocateContents::_RecvItemUse_Result );	
	// 서버 이전 페이지 상태인지 체크 
	GAME_EVENT_ST.AddEvent( EVENT_CODE::IS_SERVERR_RELOCATE_PAGE_SHOW, this, &CServerRelocateContents::_CheckShowPage );	
	EventRouter()->Register( CONTENTS_EVENT::EEvt_MacroProtect_Start, this );
}

CServerRelocateContents::~CServerRelocateContents(void)
{
	m_AdaptUnionStore = 0;
	m_AdaptCommunity = 0;
	m_AdaptTrade = 0;
	m_AdaptMacroProtect = 0;
	EventRouter()->UnRegisterAll( this );
	GAME_EVENT_ST.DeleteEventAll( this );
}

int const CServerRelocateContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CServerRelocateContents::Initialize(void)
{

	return true;
}

void CServerRelocateContents::UnInitialize(void)
{
}

bool CServerRelocateContents::IntraConnection(ContentsSystem& System)
{
	m_AdaptUnionStore = new AdaptUnionStore;
	if( m_AdaptUnionStore && !m_AdaptUnionStore->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_UNIONSTORE_CONTENTS ) ) )
		return false;

	m_AdaptCommunity = new AdaptCommunityContesnt;
	if( m_AdaptCommunity && !m_AdaptCommunity->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_COMMUNITY ) ) )
		return false;

	m_AdaptTrade = new AdaptTradeContents;
	if( m_AdaptTrade && !m_AdaptTrade->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_TRADE ) ) )
		return false;

	m_AdaptMacroProtect = new AdaptMacroProtect;
	if( m_AdaptMacroProtect && !m_AdaptMacroProtect->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_MACROPROTECT ) ) )
		return false;
	return true; 
}

void CServerRelocateContents::Update(float const& fElapsedTime)
{
}

void CServerRelocateContents::NotifyEvent(int const& iNotifiedEvt)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EEvt_MacroProtect_Start:
		{
			if( IsServerRelocatePageShow() )
				CloseServerChangePage();
		}
		break;
	}
}

void CServerRelocateContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

void CServerRelocateContents::MakeMainActorData(void)
{
}

void CServerRelocateContents::ClearMainActorData(void)
{
	ClearAllRegistered();
}

int CServerRelocateContents::GetCurrentPageType() const
{
	return m_nCurrentPageType;
}

// 서버 변경 페이지 상태인지 체크
bool CServerRelocateContents::IsServerRelocatePageShow() const
{
	return m_nCurrentPageType == eNonePage ? false : true;
}

void CServerRelocateContents::_OpenUIPage(void* pData)
{
	SAFE_POINTER_RET(pData);

	SAFE_POINTER_RET( m_AdaptTrade );

	// 거래 중에는 안됨^
	if( m_AdaptTrade->IsTrading() )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}
	// 매크로 프로텍터 답변중에는 서버 이전 페이지 이용 불가
	if( m_AdaptMacroProtect->IsMacroProtectUsing() )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	m_nUseItemInvenPos = *static_cast<int*>(pData);

	FLOWMGR_ST.PushFlow( Flow::CFlow::FLW_SERVER_RELOCATE );
	_InitData();
}

void CServerRelocateContents::CloseServerChangePage()
{
	FLOWMGR_ST.PopFlow( Flow::CFlow::FLW_SERVER_RELOCATE );
	m_nCurrentPageType = eTermNCondigitionPage;
}

void CServerRelocateContents::NextStep()
{
	switch( m_nCurrentPageType )
	{
	case eTermNCondigitionPage: m_nCurrentPageType = eCautiousPage;	break;
	case eCautiousPage:			m_nCurrentPageType = eRegistPage;	break;
	case eRegistPage: 
		return;
	}

	ContentsStream kTmp;
	kTmp << m_nCurrentPageType; 
	Notify( eChangePageStep, kTmp );
}

void CServerRelocateContents::BackStep()
{
	switch( m_nCurrentPageType )
	{
	case eTermNCondigitionPage: 
		return;
	case eCautiousPage:			m_nCurrentPageType = eTermNCondigitionPage;	break;
	case eRegistPage:			m_nCurrentPageType = eCautiousPage;			break;
	}

	ContentsStream kTmp;
	kTmp << m_nCurrentPageType; 
	Notify( eChangePageStep, kTmp );
}

void CServerRelocateContents::Load3DMapData()
{
	_Load3DMap( "Data/etcobject/ServerRelocateBG.nif" );
	_SetCameraInfo();
}

void CServerRelocateContents::Clear3DMapData()
{
	_ClearData();
	_ResetCameraInfo();
	m_BackgroundMap.Delete();
	m_nUseItemInvenPos = -1;
}

void CServerRelocateContents::Update3DMapObject( float const& fAccumTime )
{
	if( m_BackgroundMap.m_pNiNode )
		m_BackgroundMap.m_pNiNode->Update( fAccumTime );
}

void CServerRelocateContents::Render3DMapObject()
{
	m_BackgroundMap.Render();
}

void CServerRelocateContents::_InitData()
{
	m_nChangeServerIdx = -1;
	m_nCurrentPageType = eTermNCondigitionPage;
	m_nChangeTamerName.clear();
	m_bUsedTamerName = false;
	m_bCheckedShareHouseLimit = false;
}

void CServerRelocateContents::_ClearData()
{
	m_nChangeServerIdx = -1;
	m_nCurrentPageType = eNonePage;
	m_nChangeTamerName.clear();
	m_bUsedTamerName = false;
	m_bCheckedShareHouseLimit = false;
}

void CServerRelocateContents::SetShareHouseLimitCheck( bool const& bValue )
{
	m_bCheckedShareHouseLimit = bValue;
}

bool CServerRelocateContents::GetShareHouseLimitCheck() const
{
	return m_bCheckedShareHouseLimit;
}

void CServerRelocateContents::SetChangedServerIDX(int const& nServerIdx)
{
	m_nChangeServerIdx = nServerIdx;
	m_bUsedTamerName = false;
	ContentsStream kTmp;
	kTmp << m_nChangeServerIdx;
	Notify(eChangeServerSelected, kTmp);
}

bool CServerRelocateContents::SendCheckTamerName(std::wstring const& checkName)
{
	SAFE_POINTER_RETVAL(net::game, false);

	if( m_nChangeServerIdx == -1 )
	{
		// 서버를 선택 안했다
		cPrintMsg::PrintMsg( eRelocateTamerResult::NO_SELECTED_SERVER );
		return false;
	}	

	switch( Language::CheckRelocateTamerName( std::wstring(checkName) ) )
	{
	case Language::STRING_EMPTY:
	case Language::STRING_SIZE_MIN:
	case Language::STRING_SIZE_MAX:		cPrintMsg::PrintMsg( cPrintMsg::CHARCREATE_NAME_LEN_ERROR ); return false;
	case Language::STRING_UNKNOWN_TYPE:	cPrintMsg::PrintMsg( 10029 ); return false;
	case Language::SUCCEEDED:
		break;
	default:
		return false;
	}

	// 사용해도 되는이름인지 검사	
	if( !nsCsFileTable::g_pCuidMng->CheckID( checkName.c_str() ) )
	{
		cPrintMsg::PrintMsg( eRelocateTamerResult::DENY_TAMER_NAME );
		return false;
	}

	m_nChangeTamerName = checkName;

	if( net::game )
	{
		net::game->Send_ServerRelocate_CheckTamerName( m_nChangeServerIdx, m_nChangeTamerName );
		cPrintMsg::PrintMsg( 14010 );
	}
	return true;
}

bool CServerRelocateContents::SendServerChangeRequest()
{
	if( m_nUseItemInvenPos == -1 )
	{	// 서버 이전 아이템이 없다
		cPrintMsg::PrintMsg( eRelocateTamerResult::NOT_MATCH_ITEM );
		return false;
	}

	if( !m_bCheckedShareHouseLimit )
	{
		cPrintMsg::PrintMsg( 10119 );
		return false;
	}

	if( m_nChangeServerIdx == -1 )
	{
		// 서버를 선택 안했다
		cPrintMsg::PrintMsg( eRelocateTamerResult::NO_SELECTED_SERVER );
		return false;
	}

	// 길드에 가입 되어 있는 상태
	if( IHaveGuild() )
	{
		cPrintMsg::PrintMsg( eRelocateTamerResult::JOINED_GUILD );
		return false;
	}

	// 위탁 상점이 있는 상태
	if( IHaveUnionStore() )
	{
		cPrintMsg::PrintMsg( eRelocateTamerResult::OPENED_COMMISSION_SHOP );
		return false;
	}

	// 이전 제한 시간이 남아 있는 상태
	if( !IsEnableServerRelocateTime() )
	{
		CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
		if( pTamerUser )
		{
			std::wstring msg = cTooltip::Time2Str( pTamerUser->GetServerRelocateCompletedTime(),_TIME_TS, cTooltip::TT_LASTTIME );
			cPrintMsg::PrintMsg( eRelocateTamerResult::COOL_DOWN, const_cast<TCHAR*>(msg.c_str()) );
		}
		return false;
	}

	// 이전 하려는 서버에 테이머 슬롯이 없는 경우
	int nEmptySlot = GLOBALDATA_ST.GetEmptySlotCountFromServer( m_nChangeServerIdx );
	if( nEmptySlot <= 0 )
	{
		cPrintMsg::PrintMsg( eRelocateTamerResult::NOT_ENOUGH_TAMER_SLOT );
		return false;
	}

#ifdef SDM_TAMER_SERVERRELOCATE_NOCHECK_TAMERNAME_20170914
	m_nChangeTamerName = L"Empty";
#else
	if( !m_bUsedTamerName || m_nChangeTamerName.empty() )
	{	// 이전할 서버에 사용할 이름 검사를 하지 않은 경우, 이름을 입력하지 않았다.
		cPrintMsg::PrintMsg( eRelocateTamerResult::BLANK_TAMER_NAME );
		return false;
	}	
#endif

	if( net::game )
	{
		net::game->Send_ServerRelocate_ItemUse( m_nUseItemInvenPos, m_nChangeServerIdx, m_nChangeTamerName );
		cPrintMsg::PrintMsg( 14010 );
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 3D Map 로드
//////////////////////////////////////////////////////////////////////////
bool CServerRelocateContents::_Load3DMap(std::string const& loadFileName)
{
	NiStream kStream;
	if( !kStream.Load( loadFileName.c_str() ) )
		return false;

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_BackgroundMap.SetNiObject( pNode, CGeometry::Normal );	

	NiTimeController::StartAnimations( pNode, 0.0f );
	pNode->UpdateEffects();
	pNode->Update(0.0f);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 및 결과 페이지 전환시 카메라 셋팅값 변경
//////////////////////////////////////////////////////////////////////////
void CServerRelocateContents::_SetCameraInfo()
{
	m_fBackupDistMin = CAMERA_ST.GetDistRange_Min();
	m_fBackupDistMax = CAMERA_ST.GetDistRange_Max();

	sCAMERAINFO ci;
	ci.s_fDist = 0.0f;
	ci.s_fFarPlane = 10000.0f;
	ci.s_fInitPitch = 0.0f;
	CAMERA_ST.Reset( &ci );
	CAMERA_ST.SetUsingTerrainCollition(false);
	CAMERA_ST.SetDistRange( 0.0f, 1.0f );
	CAMERA_ST.ReleaseRotationLimit();
	CAMERA_ST.SetDeltaHeight( 0.0f );
	CAMERA_ST._UpdateCamera();
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 및 결과 페이지 전환시 카메라 셋팅값 복구
//////////////////////////////////////////////////////////////////////////
void CServerRelocateContents::_ResetCameraInfo()
{
	if( m_fBackupDistMin == 0 || m_fBackupDistMax == 0 )
		return;

	sCAMERAINFO ci;	
	ci.s_fDist = 1000.0f;
	ci.s_fInitRoll = NI_PI;
	ci.s_fInitPitch += NI_PI*0.4f;
	ci.s_fFarPlane = 200000.0f;
	CAMERA_ST.Reset( &ci );
	CAMERA_ST.SetUsingTerrainCollition(true);
	CAMERA_ST.SetDistRange( m_fBackupDistMin, m_fBackupDistMax );
	CAMERA_ST.SetRotationLimit( -CsD2R( 35 ), CsD2R( 70 ) );
	//CAMERA_ST.SetDeltaHeight( 120.0f );
	CAMERA_ST.SetDistAccel( 1.5f );

	m_fBackupDistMin = 0;
	m_fBackupDistMax = 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool CServerRelocateContents::IHaveGuild() const
{
	SAFE_POINTER_RETVAL(m_AdaptCommunity, false);
	return m_AdaptCommunity->IhaveGuild();
}

bool CServerRelocateContents::IHaveUnionStore() const
{
	SAFE_POINTER_RETVAL(m_AdaptUnionStore, false);
	return m_AdaptUnionStore->IhaveUnionStore();
}

// 서버 이전 제한 시간이 남아 있는가
bool CServerRelocateContents::IsEnableServerRelocateTime() const
{
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pTamerUser, false );
	return pTamerUser->IsEnableServerRelocate();
}

UINT64 CServerRelocateContents::GetLimitMaxMoney(int const& nServerIDX) const
{
	SAFE_POINTER_RETVAL(nsCsFileTable::g_pServerTranferTBMng, 0);
	return nsCsFileTable::g_pServerTranferTBMng->GetLimitValueFromType( sTransferLimit::eLimit_Money, nServerIDX );
}

bool CServerRelocateContents::IsServerRelocateEnable(int const& nServerIDX) const
{
	SAFE_POINTER_RETVAL(nsCsFileTable::g_pServerTranferTBMng, false);
	return nsCsFileTable::g_pServerTranferTBMng->IsServerRelocateEnable( nServerIDX );
}
//////////////////////////////////////////////////////////////////////////
// 이전 하려는 서버에 생성 가능한 이름인지에 대한 결과값
//////////////////////////////////////////////////////////////////////////
void CServerRelocateContents::_RecvCheckTamerName_Result(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_CHECK_RELOCATE_NEW_TAMER_NAME* pRecv = static_cast<GS2C_RECV_CHECK_RELOCATE_NEW_TAMER_NAME*>(pData);

	cMessageBox::DelMsg( 14010, false );

	if( eRelocateTamerResult::SUCCESS == pRecv->m_nResult )
		m_bUsedTamerName = true;
	else
	{
		m_bUsedTamerName = false;
		m_nChangeTamerName.clear();
		cPrintMsg::PrintMsg( pRecv->m_nResult );
	}

	ContentsStream kTmp;
	kTmp << m_bUsedTamerName;
	Notify( eRecvNameCheckPacket, kTmp );
}

//////////////////////////////////////////////////////////////////////////
// 이전하기 위해 서버에 요청 패킷에대한 결과값
//////////////////////////////////////////////////////////////////////////
void CServerRelocateContents::_RecvItemUse_Result(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_REQUEST_RELOCATE_ITEM_USE* pRecv = static_cast<GS2C_RECV_REQUEST_RELOCATE_ITEM_USE*>(pData);
	
	cMessageBox::DelMsg( 14010, false );

	bool bResult = true;	// 아이템 사용에 대한 결과값
	if( eRelocateTamerResult::SUCCESS != pRecv->m_nResult )
	{
		bResult = false;
		// 에러 메시지 출력
		cPrintMsg::PrintMsg( pRecv->m_nResult );
	}
	else
		cPrintMsg::PrintMsg( 10100 );

	ContentsStream kTmp;
	kTmp << bResult;
	Notify( eRecvItemUseResult, kTmp );
}

//////////////////////////////////////////////////////////////////////////
// 서버 이전 페이지에 있는지 체크 함수
//////////////////////////////////////////////////////////////////////////
void CServerRelocateContents::_CheckShowPage(void* pData)
{
	SAFE_POINTER_RET(pData);
	bool * bValue = static_cast<bool*>(pData);
	*bValue = IsServerRelocatePageShow();
}

void CServerRelocateContents::ReCheckTamerName()
{
	if( !m_bUsedTamerName )
		return;

	m_bUsedTamerName = false;
	Notify( eReCheckTamerName );
}
