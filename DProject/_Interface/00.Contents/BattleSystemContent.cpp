#include "StdAfx.h"
#include "BattleSystemContent.h"

#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

//////////////////////////////////////////////////////////////////////////
// 배틀 디지몬 3D 데이터
//////////////////////////////////////////////////////////////////////////
BRegist::sDigimonData::sDigimonData():m_pObject(NULL)
{

}

BRegist::sDigimonData::~sDigimonData()
{

}

void BRegist::sDigimonData::MakeDigimon(unsigned int const& nFileTableIdx, NiPoint3 const& nPos, float const& fPosHeight )
{
	unsigned int modelID = nFileTableIdx;
	//////////////////////////////////////////////////////////////////////////
	// 3D 오브젝트 셋팅
	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
	prop.s_dwOptionPlag = OP_STAT | OP_LIGHT;
	prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();
	m_pObject = g_pCharMng->AddTempDigimon( &prop, nFileTableIdx, NULL, nPos , 0.2f, NULL );
	if( m_pObject )
	{
		float fToolHeight = m_pObject->CsC_AvObject::GetToolHeight();
		if( fToolHeight > fPosHeight )
		{
			float digimonScale = fPosHeight / fToolHeight;
			m_pObject->SetScale( digimonScale );
		}
		m_pObject->SetAnimation( ANI::IDLE_NORMAL ); 		
		modelID = m_pObject->GetModelID();
	}	
	//////////////////////////////////////////////////////////////////////////

	// 디지몬 아이콘 이미지 셋팅
	DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimonModelIdx( nFileTableIdx );
	m_DigimonImgFile = g_pModelDataMng->GetBigModelIconFile( dwModelID );
	//////////////////////////////////////////////////////////////////////////
	// 디지몬 계열 이름 셋팅
	m_DigimonTableName = CBattleSystemContents::GetDigimonExplainName( nFileTableIdx );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 배틀 등록 맵 데이터 
//////////////////////////////////////////////////////////////////////////
BRegist::sMapInfos::sMapInfos()
:m_nLimitDLv(0),m_nLimitPlayCount(0),m_nNeedTicket(0),m_WinRankPoint(0),m_LoseRankPoint(0),m_nPlayCount(0)
{
}

BRegist::sMapInfos::~sMapInfos()
{
}

void BRegist::sMapInfos::SetBattleMap( DWORD const& dwMapID, std::string const& imgFileName )
{
	m_dwMapID = dwMapID;
	m_cMapFileName = imgFileName;

	if( nsCsMapTable::g_pMapListMng )
	{
		CsMapList* pMapList = nsCsMapTable::g_pMapListMng->GetList( dwMapID );
		if( pMapList )
		{
			CsMapList::sINFO* pInfo = pMapList->GetInfo();
			if( pInfo )
				m_wcMapName = pInfo->s_szMapDiscript;
		}
	}
}

void BRegist::sMapInfos::SetLimitInfo( unsigned int const& nDlv, unsigned int const& nTicket, unsigned int const& nJoinCount )
{
	m_nLimitDLv = nDlv;
	m_nNeedTicket = nTicket;
	m_nLimitPlayCount = nJoinCount;
}

void BRegist::sMapInfos::SetRewardPoint(int const& winPoint, int const& losePoint)
{
	m_WinRankPoint = winPoint;		// 이겼을 때 주어지는 포인트
	m_LoseRankPoint = losePoint;	// 졌을 때 주어지는 포인트
}

unsigned int BRegist::sMapInfos::GetNeedTicket() const	{	return m_nNeedTicket;	}
std::wstring BRegist::sMapInfos::GetMapName() const		{	return m_wcMapName;		}
std::string	BRegist::sMapInfos::GetMapFileName() const	{	return m_cMapFileName;	}
unsigned int BRegist::sMapInfos::GetLimitDLv() const	{	return m_nLimitDLv;		}
unsigned int BRegist::sMapInfos::GetLimitTicket() const {	return m_nNeedTicket;	}
unsigned int BRegist::sMapInfos::GetLimitPlayCount() const	{	return m_nLimitPlayCount;}
DWORD BRegist::sMapInfos::GetMapID() const				{	return m_dwMapID;		}
BRegist::REWARD_ITEM_CON const& BRegist::sMapInfos::GetViewWinRewardItems() const	{	return m_ViewWinRewardItems;	}
BRegist::REWARD_ITEM_CON const& BRegist::sMapInfos::GetViewLoseRewardItems() const	{	return m_ViewLoseRewardItems;	}

void BRegist::sMapInfos::SetPlayCount( unsigned int const& nTotalCount )	{	m_nPlayCount = nTotalCount;	}
unsigned int BRegist::sMapInfos::GetPlayCount() const						{	return m_nPlayCount;	}
bool BRegist::sMapInfos::IsEnablePlayCount() const							{ return m_nPlayCount < m_nLimitPlayCount; }


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 배틀 등록 관련 데이터 모음.
//////////////////////////////////////////////////////////////////////////
BRegist::sRegist::sRegist():m_nCurrentState( BRegist::eState_Type_Select ),m_pRenderDigimon(NULL),
m_vCharPos(NiPoint3::ZERO),m_fBoxHeight(0.0f), m_dwRegistNpcIDX(0),
m_nSelectType(eDuelCategory::TEAM),m_nSelectMode(eDuelSubCategory::SINGLE_DUEL),m_dwSelectMap(0),
m_pSelectedBattleInfo(NULL)
{
}

BRegist::sRegist::~sRegist()
{
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 데이터 로드
//////////////////////////////////////////////////////////////////////////
bool BRegist::sRegist::LoadRegistData()	
{
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RETVAL( pDUser, false ); // 내 디지몬 정보가 없다.

	// 맵 파일을 읽지 못했다고 패이지 전환이 되지 않으면 안된다.
	LoadRegistMap("Data/etcobject/BattleWaitRoom.nif");

	// 배틀에 참여 가능한 디지몬 목록 및 3D Object 모델 로드
	LoadDigimon( m_vCharPos, m_fBoxHeight, 1 );

	// 내 디지몬 3D Object 로드
	DWORD dwDigimonFTID = pDUser->GetBaseDigimonFTID();
	Add3D_Digimon(m_BattlenMyDigimonInfo, m_vCharPos, m_fBoxHeight, dwDigimonFTID);

	// 배틀 신청에 필요한 데이터 테이블 로드
	LoadBattleInfoFromTable();
	return true;
}

// 서버에서 받은 모드별 플레이 횟수 저장
void BRegist::sRegist::SetModePlayCount(std::map<char,int> const& playCountInfo)
{
	std::map<char,int>::const_iterator it = playCountInfo.begin();
	for( ; it != playCountInfo.end(); ++it )
	{
		MAP_TYPE_INFO_IT secIT = m_BattleInfos.find( it->first );
		if( secIT == m_BattleInfos.end() )
			continue;

		MAP_MODE_INFO_IT forIT = secIT->second.begin();
		for( ; forIT != secIT->second.end(); ++forIT )
		{
			MAP_INFO_LIST_IT mapIT = forIT->second.begin();
			for( ; mapIT != forIT->second.end(); ++mapIT )
				(*mapIT).SetPlayCount( it->second );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 데이터 삭제
//////////////////////////////////////////////////////////////////////////
void BRegist::sRegist::ClearData()
{
	m_vCharPos = NiPoint3::ZERO;
	m_fBoxHeight = 0.0f;
	m_dwRegistNpcIDX = 0;
	m_BattleRegiestMap.Delete();
	m_BattleInfos.clear();
	ResetSelectedInfo();
	DeleteDigimonInfo( m_RegistedDigimons );
	DeleteDigimonInfo( m_BattlenMyDigimonInfo );
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청에 필요한 데이터 초기화
//////////////////////////////////////////////////////////////////////////
void BRegist::sRegist::ResetSelectedInfo()
{
	m_pSelectedBattleInfo = NULL;
	m_dwSelectMap = 0;
	m_nCurrentState = BRegist::eState_Type_Select;
	m_nSelectType = eDuelCategory::TEAM;
	m_nSelectMode = eDuelSubCategory::SINGLE_DUEL;
	m_pRenderDigimon = NULL;

}
//////////////////////////////////////////////////////////////////////////
// 배틀 신청 NPC IDX 저장, 배틀 등록시 서버에 보냄.
//////////////////////////////////////////////////////////////////////////
void BRegist::sRegist::SetTargetNpdID(DWORD const& dwNpcIDX)
{
	m_dwRegistNpcIDX = dwNpcIDX;
}

//////////////////////////////////////////////////////////////////////////
// 컨테이너에 있는 디지몬 3D Object를 제거
//////////////////////////////////////////////////////////////////////////
void BRegist::sRegist::DeleteDigimonInfo( MAP_DIGIMONINFO & continer)
{
	SAFE_POINTER_RET( g_pCharMng );

	MAP_DIGIMONINFO_IT it = continer.begin();
	for( ; it != continer.end(); ++it )
	{
		g_pCharMng->DeleteTempChar( it->second.m_pObject );
		it->second.m_pObject = NULL;
	}

	continer.clear();
}

//////////////////////////////////////////////////////////////////////////
// 3D Object 맵 로드
//////////////////////////////////////////////////////////////////////////
bool BRegist::sRegist::LoadRegistMap(std::string const& loadFileName)
{
	NiStream kStream;
	if( !kStream.Load( loadFileName.c_str() ) )
		return false;

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_BattleRegiestMap.SetNiObject( pNode, CGeometry::Normal );	

	NiTimeController::StartAnimations( pNode, 0.0f );
	pNode->UpdateEffects();
	pNode->Update(0.0f);

	float fBoxHeight = 0.0f;
	NiAVObject* pBox = pNode->GetObjectByName("digimonbox");
	if( pBox )
		fBoxHeight = pBox->GetWorldBound().GetRadius() * 2.0f;

	m_vCharPos	= pBox->GetWorldTranslate();
	m_fBoxHeight= fBoxHeight;
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 배틀에 참여 가능한 디지몬 목록을 테이블에서 읽어 저장 시킴
//////////////////////////////////////////////////////////////////////////
void BRegist::sRegist::LoadDigimon( NiPoint3 const& position, float const& fHeight, int const& nSeasonNumber )
{
	TB_BATTLE_ENTERABLE_INFO const& _Info = nsCsFileTable::g_pBattleTableMng->GetBattleEnterableDigimonInfo();
	TB_BATTLE_ENTERABLE_INFO_CIT it = _Info.find(nSeasonNumber);
	if( it == _Info.end() )
		return;

	std::list<DWORD>::const_iterator subIT = it->second.begin();
	for( ; subIT != it->second.end(); ++subIT )
		Add3D_Digimon( m_RegistedDigimons, position, fHeight, (*subIT) );
}

//////////////////////////////////////////////////////////////////////////
// 컨테이너에 디지몬 3D 데이터를 저장 시킴.
//////////////////////////////////////////////////////////////////////////
void BRegist::sRegist::Add3D_Digimon( MAP_DIGIMONINFO & continer, NiPoint3 const& position, float const& fHeight, DWORD const& dwDigimonIDX )
{
	sDigimonData addInfo;
	addInfo.MakeDigimon( dwDigimonIDX, position, fHeight );
	continer.insert( std::make_pair( dwDigimonIDX, addInfo ) );
}

//////////////////////////////////////////////////////////////////////////
// 테이블에서 각 배틀 타입/모드별 데이터 저장
//////////////////////////////////////////////////////////////////////////
void BRegist::sRegist::LoadBattleInfoFromTable()
{
	TB_BATTLE_TALBE_INFO const&	_Info = nsCsFileTable::g_pBattleTableMng->GetBattleInfo();
	TB_BATTLE_TALBE_INFO_CIT it = _Info.begin();
	for( ; it != _Info.end(); ++it )
	{
		MAP_TYPE_INFO_IT _findInfo = m_BattleInfos.find( it->first );
		if( _findInfo == m_BattleInfos.end() )
		{
			auto rib = m_BattleInfos.insert( std::make_pair( it->first, MAP_MODE_INFO() ) );
			_findInfo = rib.first;
		}

		TB_BATTLE_MODE_INFO_CIT subIt = it->second.begin();
		for( ; subIt != it->second.end(); ++subIt )
		{
			if( !(*subIt).m_bActive )
				continue;

			MAP_MODE_INFO_IT secIt = _findInfo->second.find( (*subIt).m_VsType );
			if( secIt == _findInfo->second.end() )
			{
				auto rib = _findInfo->second.insert( std::make_pair((*subIt).m_VsType, MAP_INFO_LIST()) );
				secIt = rib.first;
			}

			sMapInfos addInfo;
			addInfo.SetBattleMap( (*subIt).m_dwMapID, (*subIt).m_strMapImgFile );
			addInfo.SetLimitInfo( (*subIt).m_nLimitDLv, (*subIt).m_nNeedTicket, (*subIt).m_nLimitJoinCount );			
			addInfo.SetRewardPoint( (*subIt).m_WinRankPoint, (*subIt).m_LoseRankPoint );

			// 승리 보상 테이블 로드
			LoadRewardItems( (*subIt).m_WinRewardTalbeIdx, addInfo.m_ViewWinRewardItems );
			// 패배 보상 테이블 로드
			LoadRewardItems( (*subIt).m_LoseRewardTalbeIdx, addInfo.m_ViewLoseRewardItems );

			secIt->second.push_back( addInfo );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 테이블에서 각 배틀 타입/모드별 보상 아이템을 셋팅 한다.
//////////////////////////////////////////////////////////////////////////
void BRegist::sRegist::LoadRewardItems( int const& nRewardTableIdx, REWARD_ITEM_CON& container )
{
	TB_BATTLE_REWARD_INFO const& _Info = nsCsFileTable::g_pBattleTableMng->GetBattleRewardInfo();
	TB_BATTLE_REWARD_INFO_CIT it = _Info.find( nRewardTableIdx );
	if( it == _Info.end() )
		return;

	TB_BATTLE_REWARD_ITEM_CIT subIT = it->second.begin();
	for( ; subIT != it->second.end(); ++subIT )
	{
		sViewRewardItem addInfo;
		addInfo.m_dwItemCode = (*subIT).m_dwItemCode;
		addInfo.m_dwItemCount = (*subIT).m_Count;

		container.push_back( addInfo );
	}
}

//////////////////////////////////////////////////////////////////////////
// 서버에 배틀 신청 패킷을 보낸다
//////////////////////////////////////////////////////////////////////////
bool BRegist::sRegist::SendBattleRegist()
{
	if( net::game )
	{
		net::game->SendBattleMatchRequest( m_nSelectType, m_nSelectMode, m_dwRegistNpcIDX, m_dwSelectMap );
		return true;
	}

	return false;
}

void BRegist::sRegist::GetTypeList( std::list<unsigned int> & listCon ) const
{
	MAP_TYPE_INFO_CIT it = m_BattleInfos.begin();
	for( ; it != m_BattleInfos.end(); ++it )
		listCon.push_back( it->first );
}

void BRegist::sRegist::SetSelectType(unsigned int const& nType)
{
	if( eDuelCategory::MAX <= nType )
		return;

	m_nSelectType = nType;
}

void BRegist::sRegist::RenderBackgroundMap()
{
	m_BattleRegiestMap.Render();
	if( m_pRenderDigimon )
		m_pRenderDigimon->Render();
}

void BRegist::sRegist::UpdateBackgroundMap(float const& fAccumTime)
{
	if( m_BattleRegiestMap.m_pNiNode )
		m_BattleRegiestMap.m_pNiNode->Update(fAccumTime);
}

void BRegist::sRegist::UpdateOjbect( float const& fElapsedTime )
{
 	if( m_pRenderDigimon )
 		m_pRenderDigimon->CsC_AvObject::Update(fElapsedTime);
}

void BRegist::sRegist::_SelectedMyDigimon()
{
	MAP_DIGIMONINFO_CIT it = m_BattlenMyDigimonInfo.begin();
	if( it == m_BattlenMyDigimonInfo.end() )
		m_pRenderDigimon = NULL;
	else
		m_pRenderDigimon = it->second.m_pObject;

	_ResetDigimonRote( 0.2f );
}

void BRegist::sRegist::_ResetDigimonRote( float const& fRot )
{
	SAFE_POINTER_RET( m_pRenderDigimon );

	m_pRenderDigimon->SetRotation(fRot, true);
}

bool BRegist::sRegist::ChangeNextStep()
{
	switch( m_nCurrentState )
	{
	case eState_Type_Select: 
		{
			m_nCurrentState = eState_Mode_Select;	
			InitModeSelectInfo(m_nSelectType);
		}
		return true;// 배틀 타입 선택 페이지
	case eState_Mode_Select: return false;// 배틀 모드 선택 페이지
	default:
		return false;
	}	
}

bool BRegist::sRegist::ChangeBackStep()
{
	switch( m_nCurrentState )
	{
	case eState_Mode_Select: 
		{
			m_nCurrentState = eState_Type_Select;	
			ResetSelectedInfo();
		}
		return true;	// 배틀 타입 선택 페이지
	case eState_Type_Select: return false;// 배틀 모드 선택 페이지
	default:
		return false;
	}
}

bool BRegist::sRegist::InitModeSelectInfo(unsigned int const& nBattleType)
{
	MAP_TYPE_INFO_CIT it = m_BattleInfos.find( nBattleType );
	if( it == m_BattleInfos.end() )
		return false;

	MAP_MODE_INFO_CIT subIT = it->second.begin();
	if( subIT == it->second.end() )
		return false;

	MAP_INFO_LIST_CIT secIT = subIT->second.begin();
	if( secIT == subIT->second.end() )
		return false;

	m_nSelectType = nBattleType;
	m_nSelectMode = subIT->first;
	m_dwSelectMap = (*secIT).GetMapID();

	m_pSelectedBattleInfo = &(*secIT);
	
	_SelectedMyDigimon();
	return true;
}

BRegist::eRegistState BRegist::sRegist::GetCurrentStep() const
{
	return m_nCurrentState;
}

bool BRegist::sRegist::IsEnoughTicket( unsigned int const& unHaveTicketCount) const
{
	SAFE_POINTER_RETVAL( m_pSelectedBattleInfo, false );

	if( m_pSelectedBattleInfo->GetLimitTicket() <= unHaveTicketCount )
		return true;

	return false;
}

bool BRegist::sRegist::IsEnoughPlayCount() const
{
	SAFE_POINTER_RETVAL( m_pSelectedBattleInfo, false );
	return m_pSelectedBattleInfo->IsEnablePlayCount();
}

// 참여 가능한 디지몬인지 체크
bool BRegist::sRegist::IsEnoughDigimonType( DWORD const& dwDigimonIDX ) const
{
	MAP_DIGIMONINFO_CIT it = m_RegistedDigimons.find( dwDigimonIDX );
	if( it == m_RegistedDigimons.end() )
		return false;
	return true;
}

bool BRegist::sRegist::IsEnoughDigimonLevel( int const& nDigimonLv ) const
{
	SAFE_POINTER_RETVAL( m_pSelectedBattleInfo, false );
	if( m_pSelectedBattleInfo->GetLimitDLv() <= nDigimonLv )
		return true;

	return false;
}

BRegist::MAP_DIGIMONINFO const& BRegist::sRegist::GetDigimonList() const
{
	return m_RegistedDigimons;
}

BRegist::sMapInfos const* BRegist::sRegist::GetCurrentBattleInfo() const
{
	return m_pSelectedBattleInfo;
}

unsigned int BRegist::sRegist::GetSelectType() const
{
	return m_nSelectType;
}

unsigned int BRegist::sRegist::GetSelectMode() const
{
	return m_nSelectMode;
}

DWORD BRegist::sRegist::GetSelectMapIDX() const
{
	return m_dwSelectMap;
}

bool BRegist::sRegist::CurrentModeDown()
{
	MAP_TYPE_INFO_CIT it = m_BattleInfos.find( m_nSelectType );
	if( it == m_BattleInfos.end() )
		return false;

	int nCurrentMode = m_nSelectMode - 1;
	if( nCurrentMode == 0 )
		return false;

	MAP_MODE_INFO_CIT itMode = it->second.find( nCurrentMode );
	if( itMode == it->second.end() )
		return false;

	MAP_INFO_LIST_CIT itMaps = itMode->second.begin();
	if( itMaps == itMode->second.end() )
		return false;

	m_nSelectMode = nCurrentMode;
	m_dwSelectMap = (*itMaps).GetMapID();
	m_pSelectedBattleInfo = &(*itMaps);
	return true;
}

bool BRegist::sRegist::CurrentModeUp()
{
	MAP_TYPE_INFO_CIT it = m_BattleInfos.find( m_nSelectType );
	if( it == m_BattleInfos.end() )
		return false;

	int nCurrentMode = m_nSelectMode + 1;
	if( nCurrentMode > eDuelSubCategory::SEXDECUPLE_DUEL )
		return false;

	MAP_MODE_INFO_CIT itMode = it->second.find( nCurrentMode );
	if( itMode == it->second.end() )
		return false;

	MAP_INFO_LIST_CIT itMaps = itMode->second.begin();
	if( itMaps == itMode->second.end() )
		return false;

	m_nSelectMode = nCurrentMode;
	m_dwSelectMap = (*itMaps).GetMapID();
	m_pSelectedBattleInfo = &(*itMaps);
	return true;
}

//***********************************************************************************************************/
//////////////////////////////////////////////////////////////////////////
//	배틀 등록 중인 데이터
//////////////////////////////////////////////////////////////////////////
BWait::sWait::sWait()
:m_bRegisted(false),m_nBattleType(eDuelCategory::TEAM),m_nBattleMode(eDuelSubCategory::SINGLE_DUEL)
,m_ndwBattleMapId(0),m_nBattleUIDX(0),m_nMyTeamCode(0)
{

}

BWait::sWait::~sWait()
{

}

void BWait::sWait::ClearData()
{
	m_bRegisted = false;
	m_nBattleType = eDuelCategory::TEAM;
	m_nBattleMode = eDuelSubCategory::SINGLE_DUEL;
	m_nBattleUIDX = 0;
	m_ndwBattleMapId = 0;
	m_nMyTeamCode = 0;
	m_WaitPlayers.clear();
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청한 상태인지 체크
//////////////////////////////////////////////////////////////////////////
bool BWait::sWait::IsBattleRegisted() const
{
	return m_bRegisted;
}

unsigned int BWait::sWait::GetBattleUIDX() const
{
	return m_nBattleUIDX;
}

unsigned int BWait::sWait::GetBattleType() const
{
	return m_nBattleType;
}

unsigned int BWait::sWait::GetBattleMode() const
{
	return m_nBattleMode;
}

char BWait::sWait::GetMyTeamCode() const
{
	return m_nMyTeamCode;
}

bool BWait::sWait::SendRegistReady()
{
	if( !m_bRegisted )
		return false;

	if( 0 == m_nBattleUIDX )
		return false;

	if( net::game )
		net::game->SendBattleReady(1, m_nBattleUIDX);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 서버에 배틀 신청 취소 패킷을 보낸다
//////////////////////////////////////////////////////////////////////////
bool BWait::sWait::SendRegistCancel()
{
	if( !m_bRegisted )
		return false;

	if( net::game )
	{
		if( 0 == m_nBattleUIDX )
			net::game->SendBattleMatchCancel();
		else
			net::game->SendBattleReady(0, m_nBattleUIDX);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 성공시 서버에서 받은 데이터를 Wait 구조체에 넣어 관리 한다.(등록만 되어 있는 데이터)
//////////////////////////////////////////////////////////////////////////
void BWait::sWait::SetBattleWaitInfo( u1 const& battleType, u1 const& battleMode, u4 const& mapID )
{
	m_bRegisted		= true;
	m_nBattleType	= battleType;	
	m_nBattleMode	= battleMode;
	m_ndwBattleMapId = mapID;
}

void BWait::sWait::RecvSetWaitInfo( u4 const& battleUIDX, u4 const& mapID, u1 const& battleType, u1 const& battleMode )
{ 
	// 클라이언트에서 요청한 데이터와 서버에서 받은 데이터가 같은지 비교
	m_bRegisted			= true;
	m_nBattleUIDX		= battleUIDX;
	m_nBattleType		= battleType;
	m_nBattleMode		= battleMode;
	m_ndwBattleMapId	= mapID;

// 	// 클라이언트에서 요청한 데이터와 서버에서 받은 데이터가 같은지 비교
// 	if( !m_bRegisted )
// 	{
// 		assert_cs( m_bRegisted );
// 		m_bRegisted = true;
// 	}
// 
// 	m_nBattleUIDX	= battleUIDX;
// 
// 	if( m_nBattleType != battleType )
// 	{
// 		assert_cs( m_nBattleType == battleType );
// 		m_nBattleType	= battleType;
// 	}
// 
// 	if( m_nBattleMode != battleMode )
// 	{
// 		assert_cs( m_nBattleMode == battleMode );
// 		m_nBattleMode	= battleMode;
// 	}
// 
// 	if( m_ndwBattleMapId != mapID )
// 	{
// 		assert_cs( m_ndwBattleMapId == mapID );
// 		m_ndwBattleMapId	= mapID;
// 	}
}

//////////////////////////////////////////////////////////////////////////
// 팀별 플레이어 저장
//////////////////////////////////////////////////////////////////////////
void BWait::sWait::SetWaitPlayerInfo( std::map<n1, std::list<sBattlePlayerInfo>> const& playerlist )
{
	m_WaitPlayers.clear();

	std::map<n1, std::list<sBattlePlayerInfo>>::const_iterator it = playerlist.begin();
	for( ; it != playerlist.end(); ++it )
	{
		WAIT_PLAYER_LIST_CON_IT subIT = m_WaitPlayers.find( it->first );
		if( subIT == m_WaitPlayers.end() )
		{
			auto rib = m_WaitPlayers.insert( std::make_pair( it->first, PLAYER_INFO() ) );
			subIT = rib.first;
		}

		std::list<sBattlePlayerInfo>::const_iterator listIt = it->second.begin();
		for( ; listIt != it->second.end(); ++listIt )
		{
			BWait::sWait::sPlayerInfo addPlayer;
			addPlayer.m_PlayerName = (*listIt).m_nTamerName;
			addPlayer.m_nDigimonTableIdx = (*listIt).m_uDigimonTableIdx;
			addPlayer.m_uTamerUIDX = (*listIt).m_uTamerUIDX;

			// 디지몬 아이콘 이미지 셋팅
			DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimonModelIdx( addPlayer.m_nDigimonTableIdx );
			addPlayer.m_DigimonIconFileName = g_pModelDataMng->GetBigModelIconFile( dwModelID );

			addPlayer.m_bIsMe = g_pCharMng->IsTamerUserFromUIDX(addPlayer.m_uTamerUIDX);
			if( addPlayer.m_bIsMe )
				m_nMyTeamCode = it->first;
			subIT->second.push_back( addPlayer );
		}	
	}
}

bool BWait::sWait::ChangeReadyState( DWORD const& dwPlayerIDX, bool bReady )
{
	BWait::sWait::sPlayerInfo* pPlayerInfo = _FindPlayerInfo( dwPlayerIDX );
	SAFE_POINTER_RETVAL( pPlayerInfo, false );
	pPlayerInfo->m_nbReady = bReady;
	return true;
}

BWait::sWait::sPlayerInfo*	BWait::sWait::_FindPlayerInfo( DWORD const& dwPlayerIDX )
{
	WAIT_PLAYER_LIST_CON_IT it = m_WaitPlayers.begin();
	for( ; it != m_WaitPlayers.end(); ++it )
	{
		PLAYER_INFO_IT subIt = it->second.begin();
		for( ; subIt != it->second.end(); ++subIt )
		{
			if( (*subIt).m_uTamerUIDX == dwPlayerIDX )
				return &(*subIt);
		}
	}
	return NULL;
}

BWait::sWait::WAIT_PLAYER_LIST_CON const& BWait::sWait::GetWaitPlayerList() const
{
	return m_WaitPlayers;
}
//////////////////////////////////////////////////////////////////////////

//***********************************************************************************************************/
//////////////////////////////////////////////////////////////////////////
//	배틀 중에 필요한 데이터
//////////////////////////////////////////////////////////////////////////

void BPlaying::sBattlePlaying::ResetData()
{
	m_nBattleUIDX = 0;
	m_cTeamCode = 0;
	m_TimerInfo.Reset();
}

bool BPlaying::sBattlePlaying::IsMyTeam( unsigned int const& nTeamCode ) const
{
	return (nTeamCode == m_cTeamCode) ? true : false;
}	

void BPlaying::sBattlePlaying::CopyWaitInfo( BWait::sWait const& data )
{
	m_nBattleUIDX = data.GetBattleUIDX();
	m_cTeamCode = data.GetMyTeamCode();
}

void BPlaying::sBattlePlaying::SetBattleTimerInfo( unsigned int const& start, unsigned int const& end, unsigned int const& round )
{
	m_TimerInfo.m_nStart = start;
	m_TimerInfo.m_nEnd = end;
	m_TimerInfo.m_nRound = round;
}

unsigned int BPlaying::sBattlePlaying::GetStartTime() const
{
	return m_TimerInfo.GetStartTime();
}

unsigned int BPlaying::sBattlePlaying::GetBattleRoundTime() const
{
	return m_TimerInfo.m_nRound;
}

char BPlaying::sBattlePlaying::GetMyTeamCode() const
{
	return m_cTeamCode;
}

//***********************************************************************************************************/
//////////////////////////////////////////////////////////////////////////
//	배틀 결과 데이터
//////////////////////////////////////////////////////////////////////////
BResult::sResultInfo::sResultInfo():m_cMyTeamCode(0),m_cWinTeamCode(0),m_pRenderTamer(NULL),m_pRenderDigimon(NULL)
{

}

BResult::sResultInfo::~sResultInfo()
{

}

void BResult::sResultInfo::SetBattleResult(char const& cMyTeamCode, char const& cWinTeamCode)
{
	m_cMyTeamCode = cMyTeamCode;
	m_cWinTeamCode = cWinTeamCode;
}

void BResult::sResultInfo::SetRewardItems( std::list<sRewardItemInfo> const& itemList, int const& nGetPoint )
{
	m_bRecvedRewardData = true;
	m_bGetPoint = nGetPoint;	// 현재 획득한 포인트
	m_RewardItems.clear();

	std::list<sRewardItemInfo>::const_iterator it = itemList.begin();
	for( ; it != itemList.end(); ++it )
	{
		sReward add;
		add.m_dwItemCode = (*it).m_nItemCode;
		add.m_dwItemCount = (*it).m_nItemCount;

		CsItem* pGetItem = nsCsFileTable::g_pItemMng->GetItem( add.m_dwItemCode );
		SAFE_POINTER_CON(pGetItem);
		CsItem::sINFO* pInfo = pGetItem->GetInfo();
		SAFE_POINTER_CON(pInfo);
		add.m_wsItemName = pInfo->s_szName;

		m_RewardItems.push_back( add );
	}
}

BResult::sResultInfo::RESULT_REWARD_ITEM_CON const&	BResult::sResultInfo::GetRewardItems() const
{
	return m_RewardItems;
}

BResult::eResultType BResult::sResultInfo::GetMyResultValue() const
{
	if( 0 == m_cWinTeamCode )//Draw
		return DRAW;
	else if( m_cMyTeamCode == m_cWinTeamCode )
		return WIN;
	return LOSE;
}

bool BResult::sResultInfo::IsRecvedRewardData() const
{
	return m_bRecvedRewardData;
}

bool BResult::sResultInfo::LoadResultData()
{
	// 맵 파일을 읽지 못했다고 패이지 전환이 되지 않으면 안된다.
	LoadResultMap( "Data/etcobject/BattleWaitRoom.nif" );

	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
	prop.s_dwOptionPlag = OP_STAT | OP_LIGHT;
	prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	if( pUser )
	{
		CsC_PartObject::sSETTING_PART* pPart = pUser->GetSettingPartArray();
		CsC_PartObject::sCHANGE_PART_INFO cp[ nsPART::MAX_TOTAL_COUNT ];
		for( int tp=0; tp<nsPART::MAX_TOTAL_COUNT/*nLimit::Equip*/; ++tp )
		{
			cp[ tp ].s_nFileTableID = pPart[ tp ].s_nSettingID;
			cp[ tp ].s_nRemainTime	= pPart[ tp ].s_nRemainTime;			
			cp[ tp ].s_nPartIndex = tp;	
		}

		m_pRenderTamer = g_pCharMng->AddTempTamer( &prop, pUser->GetFTID(), m_vCharPos + NiPoint3( 0, 30, 0 ), 0.5f, NULL, cp, 0 );
		m_pRenderTamer->SetViewSight(true);
		switch( GetMyResultValue() )
		{
		case LOSE:
		case DRAW:	m_pRenderTamer->SetAnimation( 901905 );		break;	// Emotion - OTL
		case WIN:	m_pRenderTamer->SetAnimation( 901906 );		break;	// Emotion - Joy
		}
	}

	CDigimonUser* pMyDigimon = g_pCharMng->GetDigimonUser(0);
	if( pMyDigimon )
	{
		m_pRenderDigimon = g_pCharMng->AddTempDigimon( &prop, pMyDigimon->GetBaseDigimonFTID(), NULL, m_vCharPos + NiPoint3( 80, 0, 0 ), 0.5f, NULL );
		float fToolHeight = m_pRenderDigimon->CsC_AvObject::GetToolHeight();
		if( fToolHeight > m_fBoxHeight )
		{
			float digimonScale = m_fBoxHeight / fToolHeight;
			m_pRenderDigimon->SetScale( digimonScale );
		}

// 		switch( GetMyResultValue() )
// 		{
// 		case LOSE:
// 		case DRAW:	m_pRenderDigimon->SetAnimation( ANI::DAMAGE_DEAD );		break;
// 		case WIN:	m_pRenderDigimon->SetAnimation( ANI::IDLE_CHAR );		break;
// 		}
	}
	
	return true;
}

void BResult::sResultInfo::ClearData()
{
	m_bRecvedRewardData = false;
	m_BattleResultMap.Delete();
	m_RewardItems.clear();
	m_cWinTeamCode = 0;
	m_cMyTeamCode = 0;
	m_fBoxHeight = 0.0f;
	m_bGetPoint = 0;
	if( m_pRenderTamer )
	{
		g_pCharMng->DeleteTempChar( m_pRenderTamer );
		m_pRenderTamer = NULL;
	}

	if( m_pRenderDigimon )
	{
		g_pCharMng->DeleteTempChar( m_pRenderDigimon );
		m_pRenderDigimon = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// 배틀 결과 페이지 3D Map 로드
//////////////////////////////////////////////////////////////////////////
bool BResult::sResultInfo::LoadResultMap(std::string const& loadFileName)
{
	NiStream kStream;
	if( !kStream.Load( loadFileName.c_str() ) )
		return false;

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_BattleResultMap.SetNiObject( pNode, CGeometry::Normal );	

	NiTimeController::StartAnimations( pNode, 0.0f );
	pNode->UpdateEffects();
	pNode->Update(0.0f);

	float fBoxHeight = 0.0f;
	NiAVObject* pBox = pNode->GetObjectByName("digimonbox");
	if( pBox )
		fBoxHeight = pBox->GetWorldBound().GetRadius() * 2.0f;

	m_vCharPos	= pBox->GetWorldTranslate();
	m_fBoxHeight= fBoxHeight;

	return true;
}

void BResult::sResultInfo::RenderBackgroundMap()
{
	m_BattleResultMap.Render();
	if( m_pRenderTamer )
		m_pRenderTamer->CsC_AvObject::Render();
	if( m_pRenderDigimon )
		m_pRenderDigimon->CsC_AvObject::Render();
}

void BResult::sResultInfo::UpdateBackgroundMap(float const& fAccumTime)
{
	if( m_BattleResultMap.m_pNiNode )
		m_BattleResultMap.m_pNiNode->Update( fAccumTime );
}

void BResult::sResultInfo::UpdateResultObject(float const& fElapsedTime)
{
	if( m_pRenderTamer )
		static_cast<CTamer*>(m_pRenderTamer)->CsC_PartObject::Update( fElapsedTime );
		
	if( m_pRenderDigimon )
		m_pRenderDigimon->CsC_AvObject::Update(fElapsedTime);
}
//////////////////////////////////////////////////////////////////////////


//***********************************************************************************************************/
//////////////////////////////////////////////////////////////////////////
int const CBattleSystemContents::IsContentsIdentity(void)
{
	return E_CT_BATTLE_SYSTEM;
}

CBattleSystemContents::sMyBattleInfo::sMyBattleInfo():m_nHaveBattleTicketPoint(0),m_bRecvData(false)
{

}

CBattleSystemContents::sMyBattleInfo::~sMyBattleInfo()
{

}

void CBattleSystemContents::sMyBattleInfo::ResetData()
{
	m_nHaveBattleTicketPoint = 0;
	m_bRecvData = false;
	m_modeUserData.clear();
}

void CBattleSystemContents::sMyBattleInfo::SetRecved()
{
	m_bRecvData = true;
}

void CBattleSystemContents::sMyBattleInfo::AddBattleTicketPoint( DWORD const& addPoint )
{
	m_nHaveBattleTicketPoint += addPoint;
}

void CBattleSystemContents::sMyBattleInfo::SetBattleTicketPoint(__int64 const& nCount)
{
	m_nHaveBattleTicketPoint = nCount;
}

void CBattleSystemContents::sMyBattleInfo::SetBattleTypeUserData( std::map<char, sMainTypeData> const& mapUserData )
{
	std::map<char, sMainTypeData>::const_iterator it = mapUserData.begin();
	for( ; it != mapUserData.end(); ++it )
	{
		sModeUserData * pUserData = GetUserData( it->first );
		SAFE_POINTER_CON( pUserData );

		pUserData->m_nTotalPlayCount = it->second.m_nTotalPlayCount;
		pUserData->m_nTotalRankPoint = it->second.m_nTotalRankPoint; 
	}	
}

__int64	CBattleSystemContents::sMyBattleInfo::GetBattleTicketPoint() const
{
	return m_nHaveBattleTicketPoint;
}

bool CBattleSystemContents::sMyBattleInfo::IsRecvData() const
{
	return m_bRecvData;
}

std::map<char, int> CBattleSystemContents::sMyBattleInfo::GetModePlayCount() const
{
	std::map<char, int> value;
	std::map<char, sModeUserData>::const_iterator it = m_modeUserData.begin();
	for( ; it != m_modeUserData.end(); ++it )
		value.insert( std::make_pair( it->first, it->second.m_nTotalPlayCount ) );
	return value;
}

CBattleSystemContents::sMyBattleInfo::sModeUserData * CBattleSystemContents::sMyBattleInfo::GetUserData( char const& cMode )
{
	std::map<char, sModeUserData>::iterator it = m_modeUserData.find( cMode );
	if( it == m_modeUserData.end() )
	{
		auto rib = m_modeUserData.insert( std::make_pair( cMode, sModeUserData() ) );
		it = rib.first;
	}
	return &(it->second);
}


void CBattleSystemContents::sMyBattleInfo::UpdatePlayCount( char const& cMode, int const& nPlayCount )
{
	sModeUserData * pUserData = GetUserData( cMode );
	SAFE_POINTER_RET( pUserData );
	pUserData->m_nTotalPlayCount = nPlayCount;
}

void CBattleSystemContents::sMyBattleInfo::SumRankPoint( char const& cMode, __int64 const& nSumRankPoint )
{
	sModeUserData * pUserData = GetUserData( cMode );
	SAFE_POINTER_RET( pUserData );
	pUserData->m_nTotalRankPoint += nSumRankPoint;
}

//////////////////////////////////////////////////////////////////////////
//***********************************************************************************************************/
CBattleSystemContents::CBattleSystemContents(void):
m_AdaptPartySystem(NULL)
,m_fBackupDistMin(0)
,m_fBackupDistMax(0)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_BATTLEMATCH_REGIST_WINDOW,		this, &CBattleSystemContents::_OpenRegisterPage );	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_BATTLEMATCH_MY_INFO,			this, &CBattleSystemContents::_RecvMyBattleInfoRequest );		// 내 배틀 정보 서버에서 받음.
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ADD_BATTLE_POINT,				this, &CBattleSystemContents::_Recv_AddBattlePoint );		
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_BATTLEMATCH_REGISTER_RESULT,	this, &CBattleSystemContents::Recv_BattleMatch_Regist );	// 배틀 신청 결과
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_BATTLEMATCH_REGISTER_CANCEL,	this, &CBattleSystemContents::Recv_Regist_Cancel );	// 배틀 신청 취소	

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_BATTLEMATCH_READY_STATE,		this, &CBattleSystemContents::Recv_RegistReadyState );	// 내가 보낸 배틀 준비or취소 상태 패킷
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_BATTLEMATCH_CANCEL,				this, &CBattleSystemContents::Recv_Match_Cancel );	// 배틀 매칭이 취소 됬을 때 날라옴.
	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_BATTLE_MATCH_READY_REQUEST,		this, &CBattleSystemContents::NTF_BattleReadyRequest );	// 배틀 매칭 완료시 플레이어들의 매칭 수락 요청 
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_BATTLEMATCH_READY_STATE,		this, &CBattleSystemContents::NTF_RegistReadyState );	// 내가 아닌 다른 플레이어들의 준비 상태 패킷
		
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_BATTLEMATCH_COMPLETE,			this, &CBattleSystemContents::NTF_BattleMatchComplete );
	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_BATTLE_START,					this, &CBattleSystemContents::NTF_Battle_Start );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_BATTLE_END,						this, &CBattleSystemContents::NTF_Battle_End );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_BATTLE_REWARDS,					this, &CBattleSystemContents::NTF_Battle_Rewards );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::IS_BATTLEMATCH_REGISTED,			this, &CBattleSystemContents::Get_IsRegisted );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::IS_MY_BATTLE_TEAM,					this, &CBattleSystemContents::Get_IsMyBattleTeam );	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::IS_BATTLE_PLAYING,					this, &CBattleSystemContents::Get_IsBattlePlaying );	
}

CBattleSystemContents::~CBattleSystemContents(void)
{
	m_AdaptPartySystem = 0;
	GAME_EVENT_ST.DeleteEventAll( this );
}

int const CBattleSystemContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CBattleSystemContents::Initialize(void)
{
	return true;
}

void CBattleSystemContents::UnInitialize(void)
{
}

bool CBattleSystemContents::IntraConnection(ContentsSystem& System)
{ 
	m_AdaptPartySystem = new AdaptPartySystem;
	if( m_AdaptPartySystem && !m_AdaptPartySystem->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_PARTYSYSTEM ) ) )
		return false;

	return true; 
}

void CBattleSystemContents::Update(float const& fElapsedTime)
{
}

void CBattleSystemContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void CBattleSystemContents::MakeMainActorData(void)
{
}

void CBattleSystemContents::ClearMainActorData(void)
{
	ClearAllRegistered();
}

void CBattleSystemContents::MakeWorldData(void)
{
	m_sRegistInfo.ClearData();		// 배틀 등록 관련 데이터
	m_sWaitInfo.ClearData();		// 배틀 등록 후 대기 상태 정보
	m_sResultInfo.ClearData();		// 배틀 종료 관련 데이터
	m_sMyBattleInfo.ResetData();
	m_sPlayBattleInfo.ResetData();
	m_fBackupDistMin = 0;
	m_fBackupDistMax =0;
	// 배틀 시스템 드랍으로 주석 처리함.
	// 배틀 정보 요청 함수 
	//_SendMyBattleInfoRequest();
}

void CBattleSystemContents::ClearWorldData(void)
{
	m_sRegistInfo.ClearData();		// 배틀 등록 관련 데이터
	m_sWaitInfo.ClearData();		// 배틀 등록 후 대기 상태 정보
	m_sResultInfo.ClearData();		// 배틀 종료 관련 데이터
	m_sMyBattleInfo.ResetData();
	m_sPlayBattleInfo.ResetData();
	m_fBackupDistMin = 0;
	m_fBackupDistMax =0;
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 페이지 열기
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::_OpenRegisterPage(void* pData)
{
	if( g_pDataMng )
	{
		cData_ServerSync* pSyncInfo = g_pDataMng->GetServerSync();
		if( pSyncInfo && pSyncInfo->IsChageDigimon() )			
		{	// 디지몬 교체 중 배틀 신청 페이지 진입 불가 메시지 출력
			//cPrintMsg::PrintMsg( 30015 );
			return;
		}
	}

	// 이동 중지	
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	if( pUser )
	{
		pUser->ReleaseArriveTarget();
		pUser->DeletePath();		
	}

	CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
	if( pDigimon )
		pDigimon->DeletePath();

	/************************************************************************/
	/* 대상 NPC가 배틀 등록 NPC인지 체크                                    */
	/************************************************************************/
	CsC_AvObject* pTargetObject = static_cast<CsC_AvObject*>(pData);
	if( pTargetObject->GetLeafRTTI() != RTTI_NPC )
		return;

	CNpc* pNpcObject = dynamic_cast<CNpc*>(pTargetObject);
	SAFE_POINTER_RET( pNpcObject );

	CsNpc* pNpc = pNpcObject->GetFTNpc();
	SAFE_POINTER_RET( pNpc );

	CsNpc::sINFO* pFTNpc = pNpc->GetInfo();
	SAFE_POINTER_RET( pFTNpc );

	if( pFTNpc->s_eType != nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC )
		return;	// 배틀 등록 신청 NPC가 아니다

	/************************************************************************/

	SAFE_POINTER_RET( m_AdaptPartySystem );// 파티 컨텐츠가 없으면 리턴
	if( m_AdaptPartySystem->IsJoinParty() )// 파티중이면 참가 안됨
	{
		cPrintMsg::PrintMsg( eDuelErr::DURING_PARTY );
		return;
	}

	if( m_AdaptPartySystem->IsSendInvite() )// 파티 신청중이면 참가 안됨
	{
		cPrintMsg::PrintMsg( eDuelErr::DURING_PARTY_INVATE );
		return;
	}

	if( m_sWaitInfo.IsBattleRegisted() )// 이미 배틀 신청 중이면 패스
	{
		cPrintMsg::PrintMsg( eDuelErr::ALREADY_WAITING_ENTER );
		return;
	}

	// 배틀 신청 관련 데이터 로드
	DWORD dwTargetObjIdx = pTargetObject->GetFTID();
	m_sRegistInfo.SetTargetNpdID( dwTargetObjIdx );

	m_sRegistInfo.LoadRegistData();
	m_sRegistInfo.SetModePlayCount( m_sMyBattleInfo.GetModePlayCount() );
	FLOWMGR_ST.PushFlow( Flow::CFlow::FLW_BATTLE_REGISTER );	
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 페이지 닫기 , 관련 데이터 UI 삭제시 해줌.
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::CloseRegisterPage()
{
	FLOWMGR_ST.PopFlow( Flow::CFlow::FLW_BATTLE_REGISTER );

	// 이동 중지	
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	if( pUser )
	{
		pUser->ReleaseArriveTarget();
		pUser->DeletePath();		
	}

	CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
	if( pDigimon )
		pDigimon->DeletePath();
}
//////////////////////////////////////////////////////////////////////////
// 배틀 신청 및 결과 페이지 전환시 카메라 셋팅값 변경
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::_SetCameraInfo()
{
	m_fBackupDistMin = CAMERA_ST.GetDistRange_Min();
	m_fBackupDistMax = CAMERA_ST.GetDistRange_Max();

	sCAMERAINFO ci;
	ci.s_fDist = 300.0f;
	ci.s_fFarPlane = 10000.0f;
	ci.s_fInitPitch = 0.0f;
	CAMERA_ST.Reset( &ci );
	CAMERA_ST.SetUsingTerrainCollition(false);
	CAMERA_ST.ReleaseRotationLimit();
	CAMERA_ST.SetDeltaHeight( 0.0f );
	CAMERA_ST._UpdateCamera();
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 및 결과 페이지 전환시 카메라 셋팅값 복구
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::_ResetCameraInfo()
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


void CBattleSystemContents::StartRegistPage()
{
	_SetCameraInfo();
}

void CBattleSystemContents::EndRegistPage()
{
	_ResetCameraInfo();
	m_sRegistInfo.ClearData();
}

void CBattleSystemContents::RenderRegistBackMap()
{
	m_sRegistInfo.RenderBackgroundMap();
}

void CBattleSystemContents::UpdateRegistBackMap(float const& fAccumTime)
{
	m_sRegistInfo.UpdateBackgroundMap(fAccumTime);
}

void CBattleSystemContents::UpdateRegistObject(float const& fElapsedTime)
{
	m_sRegistInfo.UpdateOjbect(fElapsedTime);
}

void CBattleSystemContents::GetBattleTypeList( std::list<unsigned int>& listType ) const
{
	listType.clear();
	m_sRegistInfo.GetTypeList( listType );
}

//////////////////////////////////////////////////////////////////////////
// 배틀 타입 저장
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::SetBattleTypeSelect(unsigned int const& nSelectType)
{
	m_sRegistInfo.SetSelectType( nSelectType );
}

void CBattleSystemContents::RegisterNextStep()
{
	if( !m_sRegistInfo.ChangeNextStep() )
		return;

	BRegist::eRegistState step = m_sRegistInfo.GetCurrentStep();
	ContentsStream kTmp;
	kTmp << step;
	Notify( eChange_State, kTmp );
}

void CBattleSystemContents::RegisterBackStep()
{
	if( !m_sRegistInfo.ChangeBackStep() )
		return;

	BRegist::eRegistState step = m_sRegistInfo.GetCurrentStep();
	ContentsStream kTmp;
	kTmp << step;
	Notify( eChange_State, kTmp );
}

void CBattleSystemContents::GotoBack()
{
	switch( m_sRegistInfo.GetCurrentStep() )
	{
	case BRegist::eState_Type_Select:	CloseRegisterPage();	break;
	case BRegist::eState_Mode_Select:	RegisterBackStep();		break;
	}
}

BRegist::MAP_DIGIMONINFO const & CBattleSystemContents::GetRegistableDigimonList() const
{
	return m_sRegistInfo.GetDigimonList();
}

BRegist::sMapInfos const* CBattleSystemContents::GetCurrentReigistBattleInfo() const
{
	return m_sRegistInfo.GetCurrentBattleInfo();
}


unsigned int CBattleSystemContents::GetSelectedType() const
{
	return m_sRegistInfo.GetSelectType();
}

unsigned int CBattleSystemContents::GetSelectedMode() const
{
	return m_sRegistInfo.GetSelectMode();
}

bool CBattleSystemContents::IsEnoughDigimonLv() const
{
	int nDigimonLv = GetMyDigimonLv();
	return m_sRegistInfo.IsEnoughDigimonLevel(nDigimonLv);
}

bool CBattleSystemContents::IsEnoughDigimon() const
{
	DWORD dwDigimonIDX = GetMyDigimonBaseFTID();
	return m_sRegistInfo.IsEnoughDigimonType( dwDigimonIDX );
}

bool CBattleSystemContents::IsJoinBattleHaveDigimon() const
{
	DWORD dwDigimonIDX = GetMyDigimonBaseFTID();
	if( !m_sRegistInfo.IsEnoughDigimonType( dwDigimonIDX ) )
		return false;

	int nDigimonLv = GetMyDigimonLv();
	if( !m_sRegistInfo.IsEnoughDigimonLevel(nDigimonLv) )
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 패킷 보내기전 신청 가능 상태인지 체크 함수
//////////////////////////////////////////////////////////////////////////
bool CBattleSystemContents::_CheckRegistEnable() const
{
	// 이미 참가 신청중인경우
	if( m_sWaitInfo.IsBattleRegisted() )// 배틀 매칭 신청 중인가?
	{
		cPrintMsg::PrintMsg( eDuelErr::ALREADY_WAITING_ENTER );
		return false;
	}

	SAFE_POINTER_RETVAL( m_AdaptPartySystem, false );// 파티 컨텐츠가 없으면 리턴

	if( m_AdaptPartySystem->IsJoinParty() )// 파티중이면 참가 안됨
	{
		cPrintMsg::PrintMsg( eDuelErr::DURING_PARTY );
		return false;
	}

	if( m_AdaptPartySystem->IsSendInvite() )// 파티 신청중이면 참가 안됨
	{
		cPrintMsg::PrintMsg( eDuelErr::DURING_PARTY_INVATE );
		return false;
	}
 
	if( !m_sRegistInfo.IsEnoughTicket(m_sMyBattleInfo.GetBattleTicketPoint()) )
	{	// 필요 티켓 부족
		cPrintMsg::PrintMsg( eDuelErr::TICKET_LOW );
		return false;
	}

	if( !m_sRegistInfo.IsEnoughPlayCount() )
	{	// 입장 가능 횟수 초과
		cPrintMsg::PrintMsg( eDuelErr::PLAY_COUNT_OVER );
		return false;
	}

	int nDLv = GetMyDigimonLv();
	if( !m_sRegistInfo.IsEnoughDigimonLevel( nDLv ) )
	{	// 참여 할 수 있는 디지몬 레벨 부족
		cPrintMsg::PrintMsg( eDuelErr::DIGIMON_LEVEL_LOW );
		return false;
	}

	DWORD dwDigimonIDX = GetMyDigimonBaseFTID();
	if( !m_sRegistInfo.IsEnoughDigimonType( dwDigimonIDX ) )
	{	// 참여 할 수 없는 디지몬인 경우
		cPrintMsg::PrintMsg( eDuelErr::NOT_AVAILABLE_PARTNER_DIGIMON );
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
// 서버에 배틀 매칭 등록 패킷 보냄
//////////////////////////////////////////////////////////////////////////
bool CBattleSystemContents::Send_BattleMatch_Regist()
{ 
	// 메세지 박스가 이 외에 다른게 띄워져 있으면 참가 안됨
	if( cMessageBox::GetCount() > 1 )
		return false;

 	//배틀 신청 가능한 상태인지 체크
	if( !_CheckRegistEnable() )
		return false;

	return m_sRegistInfo.SendBattleRegist();
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 결과값을 받는다.
// 배틀 신청 결과가 성공이면 게임 화면으로 이동
// 신청 실패시 에러 메시지 출력
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::Recv_BattleMatch_Regist(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_BATTLEMATCHING_REGISTER* pRecv = static_cast<GS2C_RECV_BATTLEMATCHING_REGISTER*>(pData);

	if( pRecv->m_nResult != eDuelErr::DUEL_MATCH_REGISTER_SUCCESS )// 배틀 신청 성공이 아닌경우 
	{
		m_sWaitInfo.ClearData();
		cPrintMsg::PrintMsg( pRecv->m_nResult );
		Notify( eRegistFales );
		return;
	}

	cPrintMsg::PrintMsg( 40016 );// 배틀 등록 성공
	
	m_sWaitInfo.SetBattleWaitInfo( pRecv->m_nBattleMainMode, pRecv->m_nBattleSubMode, pRecv->m_nMapIdx );
	g_pGameIF->GetDynamicIF(cBaseWindow::WT_BATTLE_MATCHING_WAIT_BUTTON);
	CloseRegisterPage();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//	배틀 결과 페이지 셋팅
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::StartResultPage()
{
	m_fBackupDistMin = CAMERA_ST.GetDistRange_Min();
	m_fBackupDistMax = CAMERA_ST.GetDistRange_Max();

	sCAMERAINFO ci;
	ci.s_fDist = 0.0f;
	ci.s_fFarPlane = 10000.0f;
	ci.s_fInitPitch = 0.0f;
	CAMERA_ST.Reset( &ci );
	CAMERA_ST.SetUsingTerrainCollition(false);
	CAMERA_ST.ReleaseRotationLimit();
	CAMERA_ST.SetDeltaHeight( 0.0f );
	CAMERA_ST._UpdateCamera();
}

//////////////////////////////////////////////////////////////////////////
//	배틀 결과 페이지 종료 
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::EndResultPage()
{
	_ResetCameraInfo();
	m_sResultInfo.ClearData();
}

BResult::eResultType CBattleSystemContents::GetMyResult() const
{
	return m_sResultInfo.GetMyResultValue();
}

BResult::sResultInfo::RESULT_REWARD_ITEM_CON const&	CBattleSystemContents::GetRewardItems() const
{
	return m_sResultInfo.GetRewardItems();
}
//////////////////////////////////////////////////////////////////////////
// 배틀 결과 페이지 열기
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::OpenResultPage()
{
	if( !m_sResultInfo.IsRecvedRewardData() )
		return;

	// 결과 페이지 관련 데이터 로드
	m_sResultInfo.LoadResultData();

	if( FLOWMGR_ST.GetCurTopFlowID() == Flow::CFlow::FLW_MAINGAME )
		FLOWMGR_ST.PushFlow( Flow::CFlow::FLW_BATTLE_RESULT );
	else
		FLOWMGR_ST.ChangeFlow( Flow::CFlow::FLW_BATTLE_RESULT );


	// 이동 중지	
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	if( pUser )
	{
		pUser->ReleaseArriveTarget();
		pUser->DeletePath();		
	}

	CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
	if( pDigimon )
		pDigimon->DeletePath();	
}

//////////////////////////////////////////////////////////////////////////
// 배틀 결과 페이지 닫기 , 관련 데이터 UI 삭제시 해줌.
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::CloseResultPage()
{
	FLOWMGR_ST.PopFlow( Flow::CFlow::FLW_BATTLE_RESULT );

	m_sPlayBattleInfo.ResetData();
	//m_sResultInfo.ClearData();	
}

//////////////////////////////////////////////////////////////////////////
// 배틀 결과 페이지의 3D Map 렌더링
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::RenderResultBackMap()
{
	m_sResultInfo.RenderBackgroundMap();
}

void CBattleSystemContents::UpdateResultBackMap(float const& fAccumTime)
{
	m_sResultInfo.UpdateBackgroundMap(fAccumTime);
}

void CBattleSystemContents::UpdateResultObject(float const& fElapsedTime)
{
	m_sResultInfo.UpdateResultObject(fElapsedTime);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int CBattleSystemContents::GetMyDigimonLv() const
{
	CDigimonUser * pDUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RETVAL( pDUser, 0 );
	CsC_AvObject::sBASE_STAT * pDigimonInfo = pDUser->GetBaseStat();
	SAFE_POINTER_RETVAL( pDigimonInfo, 0 );
	return pDigimonInfo->GetLevel();
}

DWORD CBattleSystemContents::GetMyDigimonBaseFTID() const
{
	CDigimonUser const* pDUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RETVAL( pDUser, 0 );
	return pDUser->GetBaseDigimonFTID();
}

std::wstring CBattleSystemContents::GetMyDigimonName() const
{
	CDigimonUser * pDUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RETVAL( pDUser, L"" );
	std::wstring name = pDUser->GetName();
	return name;
}

std::wstring CBattleSystemContents::GetMyDigimonTableName() const
{
	DWORD dwDigimonID = GetMyDigimonBaseFTID();
	return GetDigimonExplainName(dwDigimonID);
}

std::wstring CBattleSystemContents::GetDigimonExplainName( DWORD const& dwDigimonTbID )
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTacticsMng, L"" );
	CsTacticsExplain* pExplain = nsCsFileTable::g_pTacticsMng->GetTacticsExplain( dwDigimonTbID );
	SAFE_POINTER_RETVAL( pExplain, L"" );
	CsTacticsExplain::sINFO* pInfo = pExplain->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, L"" );
	return pInfo->s_szTacticsName;
}

void CBattleSystemContents::SetCurrentModeDown()
{
	if( !m_sRegistInfo.CurrentModeDown() )
		return;

	Notify( eChange_Mode );
}

void CBattleSystemContents::SetCurrentModeUp()
{
	if( !m_sRegistInfo.CurrentModeUp() )
		return;

	Notify( eChange_Mode );
}

unsigned	int CBattleSystemContents::GetHaveBattleTicketPoint() const
{
	return m_sMyBattleInfo.GetBattleTicketPoint();
}

bool CBattleSystemContents::IsMyBattleTeam( int const& nCheckTeamCode )const
{
	return m_sPlayBattleInfo.IsMyTeam( nCheckTeamCode );
}

//////////////////////////////////////////////////////////////////////////
// 서버에 내 배틀 정보를 요청 함.
//////////////////////////////////////////////////////////////////////////
bool CBattleSystemContents::_SendMyBattleInfoRequest()
{
	// 이미 서버에서 데이터를 받았으면 서버에 요청 패킷을 보내지 않음.
	if( m_sMyBattleInfo.IsRecvData() )
		return false;

	net::game->SendMyBattleMatchInfo();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 서버에 내 배틀 정보를 요청 함.
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::_RecvMyBattleInfoRequest(void*pData)
{
	SAFE_POINTER_RET( pData )
	GS2C_RECV_BATTLEMATCHING_MY_INFO* pRecv = static_cast<GS2C_RECV_BATTLEMATCHING_MY_INFO*>(pData);

	m_sMyBattleInfo.SetRecved();
	m_sMyBattleInfo.SetBattleTicketPoint( pRecv->m_nBattleTicketPoint );
	m_sMyBattleInfo.SetBattleTypeUserData( pRecv->m_nBattleTypeUserData );
}

void CBattleSystemContents::_Recv_AddBattlePoint(void* pData)
{
	SAFE_POINTER_RET( pData )
	DWORD dwAddPoint = *static_cast<DWORD*>(pData);
	m_sMyBattleInfo.AddBattleTicketPoint( dwAddPoint );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 대기 윈도우 토글
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::ToggleWaitingWindow()
{
	if( !m_sWaitInfo.IsBattleRegisted() )
	{
		g_pGameIF->CloseDynamicIF(cBaseWindow::WT_BATTLE_MATCHING_WAIT_BUTTON);
		return;
	}

	cBaseWindow* pWin = g_pGameIF->_GetPointer( cBaseWindow::WT_BATTLE_MATCHING_WAIT_WINDOW, 0 );
	if( pWin )
	{
		if( pWin->IsShowWindow() )
			pWin->SetShowWindow(false);
		else
			pWin->SetShowWindow(true);
	}
	else
		g_pGameIF->GetDynamicIF(cBaseWindow::WT_BATTLE_MATCHING_WAIT_WINDOW);
}

//////////////////////////////////////////////////////////////////////////
// 배틀 매칭 신청 중인가?
//////////////////////////////////////////////////////////////////////////
bool CBattleSystemContents::IsBattleMatchRegisted() const	// 배틀 매칭 신청 중인가?
{
	return m_sWaitInfo.IsBattleRegisted();
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 대기 중인 배틀 타입
//////////////////////////////////////////////////////////////////////////
unsigned int CBattleSystemContents::GetRegistedBattleType() const
{
	return m_sWaitInfo.GetBattleType();
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 대기 중인 배틀 모드
//////////////////////////////////////////////////////////////////////////
unsigned int CBattleSystemContents::GetRegistedBattleMode() const
{
	return m_sWaitInfo.GetBattleMode();
}

BWait::sWait::WAIT_PLAYER_LIST_CON const& CBattleSystemContents::GetWaitPlayerInfo() const
{
	return m_sWaitInfo.GetWaitPlayerList();
}

bool CBattleSystemContents::GetBattleLoadingInfo( BWait::sWait::WAIT_PLAYER_LIST_CON& info ) const
{
	if( !m_sWaitInfo.IsBattleRegisted() )
		return false;

	info = m_sWaitInfo.GetWaitPlayerList();
	return true;
}

void CBattleSystemContents::GetBattleRegistInfo( char & nTeamCode, unsigned int & nBattleType, unsigned int & nBattleMode ) const
{
	nTeamCode = 0;	nBattleType = 0;	nBattleMode = 0;
	if( !m_sWaitInfo.IsBattleRegisted() )
		return;

	nTeamCode = m_sWaitInfo.GetMyTeamCode();
	nBattleType = m_sWaitInfo.GetBattleType();
	nBattleMode = m_sWaitInfo.GetBattleMode();
}

//////////////////////////////////////////////////////////////////////////
// 배틀 준비 상태 수락
//////////////////////////////////////////////////////////////////////////
bool CBattleSystemContents::Send_Regist_Ready()
{
	if( !m_sWaitInfo.SendRegistReady() )// 배틀 등록이 아닌상태일 경우 데이터 삭제 및 UI 닫기
	{
		m_sWaitInfo.ClearData();
		g_pGameIF->CloseDynamicIF(cBaseWindow::WT_BATTLE_MATCHING_WAIT_BUTTON);
		g_pGameIF->CloseDynamicIF(cBaseWindow::WT_BATTLE_MATCHING_WAIT_WINDOW);
		return false;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
// 서버에 배틀 등록 취소 패킷을 보낸다.
//////////////////////////////////////////////////////////////////////////
bool CBattleSystemContents::Send_Regist_Cancel()
{
	if( !m_sWaitInfo.SendRegistCancel() )// 배틀 등록이 아닌상태일 경우 데이터 삭제 및 UI 닫기
	{
		m_sWaitInfo.ClearData();
		g_pGameIF->CloseDynamicIF(cBaseWindow::WT_BATTLE_MATCHING_WAIT_BUTTON);
		g_pGameIF->CloseDynamicIF(cBaseWindow::WT_BATTLE_MATCHING_WAIT_WINDOW);
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 배틀 신청 취소 패킷을 받아서 취소 
// 성공 : 등록 정보 초기화 및 배틀 신청 중인 버튼 제거
// 실패 : 에러 메시지 출력
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::Recv_Regist_Cancel(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_BATTLE_REGISTER_REQUEST_CANCEL* pRecv = static_cast<GS2C_RECV_BATTLE_REGISTER_REQUEST_CANCEL*>(pData);

	cPrintMsg::PrintMsg( 40017 );
	// 배틀 신청 중인 버튼 제거
	m_sWaitInfo.ClearData();
	g_pGameIF->CloseDynamicIF(cBaseWindow::WT_BATTLE_MATCHING_WAIT_BUTTON);
	g_pGameIF->CloseDynamicIF(cBaseWindow::WT_BATTLE_MATCHING_WAIT_WINDOW);
}

//////////////////////////////////////////////////////////////////////////
// 내가 보낸 배틀 준비or취소 상태 패킷
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::Recv_RegistReadyState(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_BATTLE_REGISTER_READY_STATE* pRecv = static_cast<GS2C_RECV_BATTLE_REGISTER_READY_STATE*>(pData);

	if( pRecv->m_nResult != 0 )
	{	// 준비/취소 변경 실패
		cPrintMsg::PrintMsg( pRecv->m_nResult );
		return;
	}

	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTamer );

	DWORD dwTamerUIDX = pTamer->GetUniqID();
	if( !m_sWaitInfo.ChangeReadyState( dwTamerUIDX, pRecv->m_nReadyState ) )
		return;

	ContentsStream kTmp;
	kTmp << dwTamerUIDX << pRecv->m_nReadyState;
	Notify( eMy_Ready_Change, kTmp );
}

//////////////////////////////////////////////////////////////////////////
// 배틀 매칭이 취소 됬을 때 날라옴.
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::Recv_Match_Cancel(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_BATTLE_MATCH_CANCEL* pRecv = static_cast<GS2C_NTF_BATTLE_MATCH_CANCEL*>(pData);

	m_sWaitInfo.ClearData();
	Notify( eBattleMatchCancel );
	g_pGameIF->CloseDynamicIF(cBaseWindow::WT_BATTLE_MATCHING_WAIT_BUTTON);
}

//////////////////////////////////////////////////////////////////////////
// 내가 아닌 다른 플레이어들의 준비 상태 패킷
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::NTF_RegistReadyState(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_NTF_BATTLE_REGISTER_READY_STATE* pRecv = static_cast<GS2C_NTF_BATTLE_REGISTER_READY_STATE*>(pData);

	if( !m_sWaitInfo.ChangeReadyState( pRecv->m_nTamerIDX, pRecv->m_nReadyState ) )
		return;

	ContentsStream kTmp;
	kTmp << pRecv->m_nTamerIDX << pRecv->m_nReadyState;
	Notify( eOther_Ready_Change, kTmp );
}

//////////////////////////////////////////////////////////////////////////
// 배틀 매칭 완료시 플레이어들의 매칭 수락 요청 
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::NTF_BattleReadyRequest(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_BATTLE_READY_STATE_ASK* pRecv = static_cast<GS2C_NTF_BATTLE_READY_STATE_ASK*>(pData);

	m_sWaitInfo.RecvSetWaitInfo( pRecv->m_nBattleIdx, pRecv->m_nMapIdx, pRecv->m_cBattleType, pRecv->m_cBattleMod );
	m_sWaitInfo.SetWaitPlayerInfo( pRecv->m_mapTeamInfo );

	cBaseWindow* pWin = g_pGameIF->_GetPointer( cBaseWindow::WT_BATTLE_MATCHING_WAIT_WINDOW, 0 );
	if( pWin )
	{
		if( !pWin->IsShowWindow() )
			pWin->SetShowWindow(true);
	}
	else
		g_pGameIF->GetDynamicIF(cBaseWindow::WT_BATTLE_MATCHING_WAIT_WINDOW);

	Notify( eAsk_BattleRequest );
}

//////////////////////////////////////////////////////////////////////////
// 배틀 매칭 신청 중인가?	
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::Get_IsRegisted(void* pData)
{
	SAFE_POINTER_RET(pData);
	bool * bValue = static_cast<bool*>(pData);
	*bValue = IsBattleMatchRegisted();
}

//////////////////////////////////////////////////////////////////////////
// 배틀 전장에 있는가?	
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::Get_IsBattlePlaying(void* pData)
{
	SAFE_POINTER_RET(pData);
	bool * bValue = static_cast<bool*>(pData);
	*bValue = IsBattlePlaying();
}

//////////////////////////////////////////////////////////////////////////
// 배틀 팀이 나와 같은가?
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::Get_IsMyBattleTeam(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int,bool> * bValue = static_cast<std::pair<int,bool>*>(pData);

	(*bValue).second = IsMyBattleTeam( bValue->first );
}

//////////////////////////////////////////////////////////////////////////
// 배틀 중인가
//////////////////////////////////////////////////////////////////////////
bool CBattleSystemContents::IsBattlePlaying() const				// 배틀 중인가
{
	return (nLIB::SVR_BATTLE == nsCsGBTerrain::g_nSvrLibType ) ? true : false;
	//return m_sPlayBattleInfo.GetMyTeamCode() != 0 ? true : false;
}


//////////////////////////////////////////////////////////////////////////
// 배틀 매칭이 완료 된 후 날라오는 패킷, 맵 로딩 준비 시작함
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::NTF_BattleMatchComplete(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_BATTLE_MATCH_COMPLETE * recv = static_cast<GS2C_NTF_BATTLE_MATCH_COMPLETE*>(pData);

	m_sPlayBattleInfo.CopyWaitInfo( m_sWaitInfo );
	//배틀 로딩시 필요한 데이터를 로딩 컨텐츠에 넘긴다.
	NotifyContents( CONTENTS_EVENT::EEvt_BattleLoading_Start );

	m_sWaitInfo.ClearData();

	if( g_pGameIF )
	{
		g_pGameIF->CloseDynamicIF(cBaseWindow::WT_BATTLE_MATCHING_WAIT_BUTTON);
		g_pGameIF->CloseDynamicIF(cBaseWindow::WT_BATTLE_MATCHING_WAIT_WINDOW);
	}
}

//////////////////////////////////////////////////////////////////////////
//	배틀 시작/종료 관련 함수
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 배틀 맵 진입 후 모든 유저가 로딩이 끝났을 때 
// 시작 타이머 데이터 받음.
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::NTF_Battle_Start(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_BATTLE_START * recv = static_cast<GS2C_NTF_BATTLE_START*>(pData);

	m_sPlayBattleInfo.SetBattleTimerInfo( recv->m_nStart, recv->m_nEnd, recv->m_nRound );

	if( g_pGameIF )
	{
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_BATTLESTART_TIMER );
		g_pGameIF->GetDynamicIF(cBaseWindow::WT_BATTLE_EVOL, 0, 2);
	}

	m_sMyBattleInfo.SetBattleTicketPoint( recv->m_nBattleTicketPoint );
	m_sMyBattleInfo.UpdatePlayCount( recv->m_nBattleMainMode, recv->m_nPlayCount );

	float fStartTime = (float)m_sPlayBattleInfo.GetStartTime();
	ContentsStream kTmp;
	kTmp << fStartTime;
	Notify(eBattleReadyTimerStart, kTmp);
}

//////////////////////////////////////////////////////////////////////////
// 배틀 종료
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::NTF_Battle_End(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_BATTLE_END * recv = static_cast<GS2C_NTF_BATTLE_END*>(pData);

	m_sResultInfo.SetBattleResult( m_sPlayBattleInfo.GetMyTeamCode(), recv->m_nWinTeamCode );

	if( g_pGameIF )
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_BATTLESTART_TIMER );	
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_BATTLEROUND_TIMER );
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_BATTLE_EVOL );
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_BATTLESTART_END );
	}

	Notify( eBattleRoundEnd );
}

//////////////////////////////////////////////////////////////////////////
// 배틀 종료 후 보상 데이터 받음.
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::NTF_Battle_Rewards(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_BATTLE_REWARDS * recv = static_cast<GS2C_NTF_BATTLE_REWARDS*>(pData);

	// 획득한 랭킹 포인트 모드별 합산
	m_sMyBattleInfo.SumRankPoint( recv->m_nBattleMainType, recv->m_nDuelPoint );
	m_sResultInfo.SetRewardItems( recv->m_Rewardlist, recv->m_nDuelPoint );
}

//////////////////////////////////////////////////////////////////////////
// 배틀 시작 대기 시간이 지난 후 3D Map 문이 열리고 라운드 타이머 셋팅
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::OnBattleStart()
{
	_BattleMapDoorOpen();

	if( g_pGameIF )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_BATTLEROUND_TIMER );

	float fRoundTime = (float)m_sPlayBattleInfo.GetBattleRoundTime();
	ContentsStream kTmp;
	kTmp << fRoundTime;
	Notify( eBattleRoundStart, kTmp );
}

//////////////////////////////////////////////////////////////////////////
// 배틀 시작시 배틀 맵의 문 여는 함수
//////////////////////////////////////////////////////////////////////////
void CBattleSystemContents::_BattleMapDoorOpen()
{
	SAFE_POINTER_RET( nsCsGBTerrain::g_pCurRoot );

	nsCsGBTerrain::g_pCurRoot->SetActor_Animation( 3, "door" );
	nsCsGBTerrain::g_pCurRoot->DoAct_Object( CsGBObject::ACT_DELETE, "door" );
	nsCsGBTerrain::g_pCurRoot->DoPath_Object( CsGBObject::PATH_DELETE, "door" );
}
