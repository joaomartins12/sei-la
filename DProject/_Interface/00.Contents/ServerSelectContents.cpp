#include "StdAfx.h"
#include "ServerSelectContents.h"

#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

int const ServerSelectContents::IsContentsIdentity(void)
{
	return E_CT_SERVER_SELECT;
}

ServerSelectContents::ServerSelectContents(void) : m_bVisible(false)
{
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_SERVER_LIST, this, &ServerSelectContents::RecvServerList );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_SERVERSELECT_CLUSTER, this, &ServerSelectContents::RecvServerSelectCluster );
}

ServerSelectContents::~ServerSelectContents(void)
{
}

int const ServerSelectContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool ServerSelectContents::Initialize(void)
{
	m_RefreshTime.SetDeltaTime(4000);
	return true;
}

void ServerSelectContents::UnInitialize(void)
{
}

bool ServerSelectContents::IntraConnection(ContentsSystem& System)
{ 
	return true; 
}

void ServerSelectContents::Update(float const& fElapsedTime)
{
}

void ServerSelectContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void ServerSelectContents::MakeMainActorData(void)
{
}

void ServerSelectContents::ClearMainActorData(void)
{
	ClearAllRegistered();
}

void ServerSelectContents::ServerListClear()
{
	m_ServerInfoList.clear();
}

sServerInfo const* ServerSelectContents::GetServerInfoFromIdx( int const& nIdx ) const
{
	std::list<sServerInfo>::const_iterator it = m_ServerInfoList.begin();
	for( ; it != m_ServerInfoList.end(); ++it )
	{
		if( (*it).m_nIDX == nIdx )
			return &(*it);
	}

	return NULL;
}

// 신규 서버 정보
sServerInfo const* ServerSelectContents::GetNewServerInfo() const
{
	std::list<sServerInfo>::const_iterator it = m_ServerInfoList.begin();
	for( ; it != m_ServerInfoList.end(); ++it )
	{
		if( (*it).m_bNewServer )
			return &(*it);
	}

	return NULL;
}

// 첫번째 서버
sServerInfo const* ServerSelectContents::GetFirstServerInfo() const
{
	if( m_ServerInfoList.empty() )
		return NULL;

	std::list<sServerInfo>::const_iterator it = m_ServerInfoList.begin();
	return &(*it);
}

// 가장 많은 캐릭터를 가지고 있는 서버
sServerInfo const* ServerSelectContents::GetHaveManyCharacterServerInfo() const
{
	int nCharCount = 0;
	UINT nServerIdx = 0;
	std::list<sServerInfo>::const_iterator it = m_ServerInfoList.begin();
	for( ; it != m_ServerInfoList.end(); ++it )
	{
		if( (*it).m_nTamerNumber > nCharCount )
		{
			nCharCount = (*it).m_nTamerNumber;
			nServerIdx = (*it).m_nIDX;
		}
	}

	return GetServerInfoFromIdx( nServerIdx );
}

UINT ServerSelectContents::GetSelectedLastServerIdx()
{
	if( m_ServerInfoList.empty() ) // 서버 목록이 없을 경우
		return 0;

	// 마지막으로 접속한 서버 정보
	sServerInfo const* pServerInfo = GetServerInfoFromIdx( g_pResist->m_AccountGlobal.s_nLastConnetServer );
	if( pServerInfo )
		return pServerInfo->m_nIDX;

	// 신서버가 있는 경우
	pServerInfo = GetNewServerInfo();
	if( pServerInfo )
		return pServerInfo->m_nIDX;

	// 가장 많은 캐릭터를 가지고 있는 서버
	pServerInfo = GetHaveManyCharacterServerInfo();
	if( pServerInfo )
		return pServerInfo->m_nIDX;

	return 1;// 위 해당사항이 없을 경우 목록의 첫번째 선택.
}

void ServerSelectContents::ConnectingServer( const int& nSelServerIdx )
{
	sServerInfo const* pInfo = GetServerInfoFromIdx( nSelServerIdx );
	SAFE_POINTER_RET( pInfo );
	
	switch( pInfo->m_nDisable )
	{
	case sServerInfo::RUNNING:			break;// 구동중
	case sServerInfo::READY:			cPrintMsg::PrintMsg( eLOGINRESULT::SERVER_IS_NOT_READY );		return;// 서버 준비중
	case sServerInfo::REPAIR:			cPrintMsg::PrintMsg( eLOGINRESULT::SERVER_IS_MAINTENANCE );		return;// 서버 점검
	}

	if( sServerInfo::eFull == pInfo->m_nClusterState )
	{
		cPrintMsg::PrintMsg( eLOGINRESULT::SERVER_CONNECT_USER_FULL );	return;	// 꽉참
		return;
	}

	SAFE_POINTER_RET( g_pResist );
	std::wstring clusterName = pInfo->m_szClusterName;
	g_pResist->m_AccountGlobal.s_nLastConnetServer = pInfo->m_nIDX;	
	_tcscpy_s( g_pResist->m_AccountGlobal.s_szServerName, sizeof( g_pResist->m_AccountGlobal.s_szServerName ), clusterName.c_str() );

	// 계정의 서버 정보를 불러 들인다.
	g_pResist->LoadAccountServer( clusterName.c_str() );	

#ifdef VERSION_KOR
	if( 0 == clusterName.compare(__T( "루체몬" )) )
		net::cluster = 100;
	else if(0 == clusterName.compare( __T( "리리스몬" ) ) )
		net::cluster = 300;
	else if(0 == clusterName.compare( __T( "리바이어몬" ) ) )
		net::cluster = 200;
	else if(0 == clusterName.compare( __T( "발바몬" ) ) )
		net::cluster = 400;
#endif

	if( !net::account->SendSelectCluster( pInfo->m_nIDX ) )
		cPrintMsg::PrintMsg(cPrintMsg::LOGIN_CHECKING_SERVER);
	else
		cPrintMsg::PrintMsg( 10019 );
}

void ServerSelectContents::gotoBack()
{
	SAFE_POINTER_RET(net::account);

	net::account->m_nIsChangeServer = true;
	net::account->DoDisconnect();

	if( GLOBALDATA_ST.IsLoginSkiped() )
		PostQuitMessage(0);
	else	
	{
		GLOBALDATA_ST.ResetAccountInfo();
		FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_LOGIN);
	}
}

void ServerSelectContents::RequestServerList()
{

	if( !m_RefreshTime.IsEnable() )
		return;

	SAFE_POINTER_RET(net::account);
	net::account->SendReqClusterList();
	BHPRT("Send Server List");
	m_RefreshTime.SetDisable();
}

void ServerSelectContents::RecvServerList( void* pData )
{
	SAFE_POINTER_RET( pData );

	m_ServerInfoList.clear();

	GS2C_RECV_SERVERLIST* pRecv = static_cast<GS2C_RECV_SERVERLIST*>(pData);
	m_ServerInfoList = pRecv->m_listServer;

	GLOBALDATA_ST.ClearServerListInfo();
	std::list<sServerInfo>::const_iterator it = m_ServerInfoList.begin();
	for (; it != m_ServerInfoList.end(); ++it) {
		GLOBALDATA_ST.AddServerListInfo( (*it).m_nIDX, (*it).m_szClusterName, (*it).m_nTamerNumber, (*it).m_nMaxTamerSlot, (*it).m_nOpenTamerSlot);
	}

	if (FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_LOGIN)) {

	
		FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);
	}
	else if (FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_SECURITY)) {

	
		FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);
	}
	else if (FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_SERVERSEL)) {

	
		Notify(eRecieve_ServerList);
	}

	m_RefreshTime.SetDisable();

#ifdef DEBUG_NETWORK
	DBG("Receiving server list\n");
#endif
	
}

void ServerSelectContents::RecvServerSelectCluster(void * pData)
{
#ifdef DEBUG_NETWORK
	DBG("RecvSelectCluster\n");
#endif
	SAFE_POINTER_RET( pData );
	AC2C_RECV_SERVERSELECT_CLUSTER * pRecv = static_cast<AC2C_RECV_SERVERSELECT_CLUSTER*>(pData);
	cMessageBox::DelMsg( 10019, false);
	if (eLOGINRESULT::SUCCESS == pRecv->nResult) {

		return;
	}

	cPrintMsg::PrintMsg(pRecv->nResult);

#ifdef DEBUG_NETWORK
	DBG("RecvSelectCluster265\n");
#endif
	RequestServerList();
}

void ServerSelectContents::CheckConnectAccountSever()
{
	if( net::account && net::account->IsActive() ) // 이미 접속 되어 있다면 접속 시도를 하지 않는다.
	{
		m_bVisible = true;
		net::account->SendReqClusterList();
		return;
	}

	GLOBALDATA_ST.LoadInfo_AccountIP_Port();
	if( net::start(net::type::account, GLOBALDATA_ST.GetAccountIP().c_str(), GLOBALDATA_ST.GetAccountPort()) == false )// Account Server에 접속 실패시
	{
		cMessageBox::DelMsg( 10019, false );
		gotoBack();
	}
}

bool ServerSelectContents::IsVisible() const
{
	return m_bVisible;
}

std::list<sServerInfo> const& ServerSelectContents::GetServerList() const
{
	return m_ServerInfoList;
}

std::string ServerSelectContents::GetServerImg(int const& nServerIdx) const
{
	switch( GLOBALDATA_ST.GetConnectionCountryType() )
	{
	case GData::eCountryType::eCountry_Kor:
		{
			switch( nServerIdx )
			{
			case 1:		return "Lobby\\SelectServer\\lobby_server_LUCEMON.tga";	
			case 2:		return "Lobby\\SelectServer\\lobby_server_LEVIAMON.tga";	
			case 3:		return "Lobby\\SelectServer\\lobby_server_LILITHMON.tga";	
			case 4:		return "Lobby\\SelectServer\\lobby_server_BARBAMON.tga";	
			default:	return "Lobby\\SelectServer\\lobby_server_OMEGAMON.tga";	
			}
		}
		break;
	case GData::eCountryType::eCountry_GSP:
	case GData::eCountryType::eCountry_Steam:
		{
			switch( nServerIdx )
			{
			case 1:		return "Lobby\\SelectServer\\lobby_server_LEVIAMON.tga";	
			case 2:		return "Lobby\\SelectServer\\lobby_server_LUCEMON.tga";	
			case 3:		return "Lobby\\SelectServer\\lobby_server_LILITHMON.tga";	
			case 4:		return "Lobby\\SelectServer\\lobby_server_BARBAMON.tga";	
			case 5:		return "Lobby\\SelectServer\\lobby_server_BEELZEBUMON.tga";	
			case 6:		return "Lobby\\SelectServer\\lobby_server_OMEGAMON.tga";	
			default:	return "Lobby\\SelectServer\\lobby_server_OMEGAMON.tga";	
			}
		}
		break;

	case GData::eCountryType::eCountry_Aeria:		
		{
			switch( nServerIdx )
			{
			case 1:		return "Lobby\\SelectServer\\lobby_server_SERAPHIMON.tga";		
			default:	return "Lobby\\SelectServer\\lobby_server_OMEGAMON.tga";		
			}
		}
		break;
	
	case GData::eCountryType::eCountry_Thailand:
		{
			switch( nServerIdx )
			{
			case 1:		return "Lobby\\SelectServer\\lobby_server_LEVIAMON.tga";	
			case 2:		return "Lobby\\SelectServer\\lobby_server_BELPHEMON.tga";	
			case 3:		return "Lobby\\SelectServer\\lobby_server_LILITHMON.tga";	
			default:	return "Lobby\\SelectServer\\lobby_server_OMEGAMON.tga";	
			}
		}
		break;
	case GData::eCountryType::eCountry_Taiwan:
		return "Lobby\\SelectServer\\lobby_server_OMEGAMON.tga";
	case GData::eCountryType::eCountry_Hongkong:		
		return "Lobby\\SelectServer\\lobby_server_OMEGAMON.tga";	
	}
	return "";
}


//////////////////////////////////////////////////////////////////////////
// 3D Object 맵 로드
//////////////////////////////////////////////////////////////////////////
bool ServerSelectContents::Load3DEffect(std::string const& loadFileName)
{
	NiStream kStream;
	if( !kStream.Load( loadFileName.c_str() ) )
		return false;

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_Effect.SetNiObject( pNode, CGeometry::Normal );	

	NiTimeController::StartAnimations( pNode, 0.0f );
	pNode->UpdateEffects();
	pNode->Update(0.0f);
	return true;
}

void ServerSelectContents::RenderBackgroundMap()
{
	m_Effect.Render();
}

void ServerSelectContents::UpdateBackgroundMap(float const& fAccumTime)
{
	if( m_Effect.m_pNiNode )
	{
		m_Effect.m_pNiNode->Update(fAccumTime);
		CsGBVisible::OnVisible( &m_Effect, m_Effect.m_pNiNode, CsGBVisible::VP_BILLBOARD, fAccumTime );
	}
}

void ServerSelectContents::Load3DBackgroundData()
{
	sCAMERAINFO ci;
	ci.s_fDist = 2800.0f;
	ci.s_fFarPlane = 100000.0f;
	ci.s_fInitPitch = 0.0f;
	CAMERA_ST.Reset( &ci );
	CAMERA_ST.ReleaseDistRange();
	CAMERA_ST.ReleaseRotationLimit();
	CAMERA_ST.SetDeltaHeight( 0.0f );
	CAMERA_ST._UpdateCamera();

	Load3DEffect("Data\\Interface\\Lobby\\BackgroundEff.nif");
}


void ServerSelectContents::Remove3DBackgroundData()
{
	m_Effect.Delete();
}