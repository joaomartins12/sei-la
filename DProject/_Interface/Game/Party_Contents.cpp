#include "StdAfx.h"
#include "Party_Contents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../Adapt/AdaptBattleSystem.h"
#include "../Adapt/AdaptTutorialQuest.h"
#include "../Adapt/AdaptGotchaMachine.h"
#include "../Adapt/AdaptMacroProtectSystem.h"

#include "common_vs2019/pParty.h"

//-------------------------------------------------------------------------------------------------------------------------------------
sLootingRule::sLootingRule():m_nCropType(nCrop::Base), m_nRareRate(0), m_nDispRareGrade(0)
{
}

void sLootingRule::Init()
{
	m_nCropType = nCrop::Base;
	m_nRareRate = 0;
	m_nDispRareGrade = 0;
}

void sLootingRule::SetLootingRule(TItemLootingInfo const& pData)
{
	m_nCropType = pData.m_nCropType;	
	m_nRareRate = pData.m_nRareRate;
	m_nDispRareGrade = (pData.m_nDispRareGrade == 0)? nCrop::RareRate::Lv1 : pData.m_nDispRareGrade;
}

void sLootingRule::SetLootingRule(int const& nCroptype, int const& nRareRate, int const& nGrade)
{
	m_nCropType = nCroptype;	
	m_nRareRate = nRareRate;
	m_nDispRareGrade = (nGrade == 0)? nCrop::RareRate::Lv1 : nGrade;
}
//-------------------------------------------------------------------------------------------------------------------------------------
sTamerInfo::~sTamerInfo()
{
	g_pCharResMng->CharImageResDelete( &m_pImage );
}

void sTamerInfo::SetUID( int const& nUID )
{	m_nUID = nUID;	}

void sTamerInfo::SetLevel( int const& nLevel )
{	m_nLevel = nLevel;	}

void sTamerInfo::SetName( std::wstring const& Name )
{	m_Name = Name;	}

void sTamerInfo::SetBaseInfo( TBaseStateInfo const& tInfo )
{	m_BaseInfo = tInfo;	}

std::string const& sTamerInfo::GetImage() const
{	return m_ImgFile;	}

void sTamerInfo::SetType( int const& nType )
{	
	if( m_nType == nType )
		return;

	m_nType = nType;
	m_ImgFile = g_pModelDataMng->GetSmallModelIconFile( m_nType );

	g_pCharResMng->CharImageResDelete( &m_pImage );
	m_pImage = g_pCharResMng->CharImageResLoad( nType );
}

void sTamerInfo::RenderImg( CsPoint const& csPos, CsPoint const& csSize ) const
{
	if( m_pImage )
		m_pImage->Render_S( csPos, csSize );
}

//-------------------------------------------------------------------------------------------------------------------------------------
sDigimonInfo::~sDigimonInfo()
{
	g_pCharResMng->CharImageResDelete( &m_pImage );
}

void sDigimonInfo::SetUID( int const& nUID )
{	m_nUID = nUID;	}

void sDigimonInfo::SetLevel( int const& nLevel )
{	m_nLevel = nLevel;	}

void sDigimonInfo::SetName( std::wstring const& Name )
{	m_Name = Name;	}

void sDigimonInfo::SetBaseInfo( TBaseStateInfo const& tInfo )
{	m_BaseInfo = tInfo;	}

std::string const& sDigimonInfo::GetImage() const
{	return m_ImgFile;	}

void sDigimonInfo::SetType( int const& nType )
{	
	if( m_nType == nType )
		return;
	
	m_nType = nType;

	CsDigimon* pDigimonData = nsCsFileTable::g_pDigimonMng->GetDigimon( m_nType );
	if( pDigimonData )
	{
		CsDigimon::sINFO* pInfo = pDigimonData->GetInfo();
		if( pInfo )
			m_dwModelID = pInfo->s_dwModelID;
	}

	m_ImgFile = g_pModelDataMng->GetSmallModelIconFile( m_dwModelID );

	g_pCharResMng->CharImageResDelete( &m_pImage );
	m_pImage = g_pCharResMng->CharImageResLoad( m_dwModelID );		
}

void sDigimonInfo::RenderImg( CsPoint const& csPos, CsPoint const& csSize ) const
{
	if( m_pImage )
		m_pImage->Render_S( csPos, csSize );
}
//-------------------------------------------------------------------------------------------------------------------------------------
void sMemberInfo::SetSlotNumber( int const& nSlotNo)
{	m_nSlotNo = nSlotNo;	}

void sMemberInfo::SetMapNumber( int const& nMapID )
{	m_nMapNo = nMapID;	}

void sMemberInfo::SetChannelNumber( int const& nChannelNumber )
{	m_nChannelNo = nChannelNumber;	}

void sMemberInfo::SetTamerUID( int const& nTamerUID )
{	m_TamerInfo.SetUID( nTamerUID );	}

void sMemberInfo::SetDigimonUID( int const& nDigimonUID )
{	m_DigimonInfo.SetUID(nDigimonUID);	}

void sMemberInfo::SetTamerPos( nSync::Pos& tPos )
{	m_tPos = tPos;	}

void sMemberInfo::SetDigimonPos( nSync::Pos& dPos )
{	m_dPos = dPos;	}

void sMemberInfo::SetOnOffLine( bool bValue )
{	m_bOnline = bValue;	}

void sMemberInfo::SetTamerInfo( sTamerInfo const& sInfo )
{	m_TamerInfo = sInfo;	}

void sMemberInfo::SetDigimonInfo( sDigimonInfo const& sInfo )
{	m_DigimonInfo = sInfo;	}

void sMemberInfo::SetTamerInfo( TPartyTamerInfo const& sInfo )
{	
	m_TamerInfo.SetBaseInfo(sInfo.m_BaseInfo);
	m_TamerInfo.SetName(sInfo.m_Name);
	m_TamerInfo.SetLevel(sInfo.m_nLevel);
	m_TamerInfo.SetType(sInfo.m_nType);
	m_TamerInfo.SetUID(sInfo.m_nUID);
	CsC_AvObject* pObject = g_pMngCollector->GetObject( sInfo.m_nUID );
	if( pObject )
	{
		NiPoint2 pos = pObject->GetPos2D();
		m_tPos.m_nX = (int)pos.x;
		m_tPos.m_nY = (int)pos.y;
	}
}

void sMemberInfo::SetDigimonInfo( TPartyDigimonInfo const& sInfo )
{	
	m_DigimonInfo.SetBaseInfo(sInfo.m_BaseInfo);
	m_DigimonInfo.SetName(sInfo.m_Name);
	m_DigimonInfo.SetLevel(sInfo.m_nLevel);
	m_DigimonInfo.SetType(sInfo.m_nType);
	m_DigimonInfo.SetUID(sInfo.m_nUID);
	CsC_AvObject* pObject = g_pMngCollector->GetObject( sInfo.m_nUID );
	if( pObject )
	{
		NiPoint2 pos = pObject->GetPos2D();
		m_dPos.m_nX = (int)pos.x;
		m_dPos.m_nY = (int)pos.y;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
sPartyInfo::sPartyInfo():m_unPartyID(0), m_nMasterSlotNum(-1),m_nMySlotNumber(0)
{
}

sPartyInfo::~sPartyInfo()
{

}

void sPartyInfo::InitData()
{
	m_unPartyID = 0;
	m_nMySlotNumber = 0;
	m_nMasterSlotNum = -1;
	m_sLootingRule.Init();
	DeleteAllMemberInfo();
}

// 파티에 속해 있나?
bool sPartyInfo::IsParty() const
{
	return m_unPartyID == 0 ? false : true;
}

UINT sPartyInfo::GetPartyIDX() const
{
	return m_unPartyID;
}

// 현재 파티원 수, 나를 뺀 맴버수
size_t sPartyInfo::GetMemberCount() const
{
	return m_listMemberInfo.size();
}

// 내가 파티 장인가?
bool sPartyInfo::IsPartyMaster() const
{
	return m_nMySlotNumber == m_nMasterSlotNum ? true : false;
}

int sPartyInfo::GetPartyMasterSlot() const
{
	return m_nMasterSlotNum;
}

void sPartyInfo::SetDefaultLootingRule()
{
	m_sLootingRule.SetLootingRule( nCrop::Base, 0, nCrop::RareRate::Lv1 );
}

// 내 슬롯 번호 인가?
bool sPartyInfo::IsMyPartySlot( int const& nSlot )
{
	return m_nMySlotNumber == nSlot ? true : false;
}

int sPartyInfo::GetMyPartySlotNumber() const
{
	return m_nMySlotNumber;
}

bool sPartyInfo::IsPartyMemberFromUIDX(bool bTamer, int const& nUIDX) const
{
	if( m_unPartyID == 0 )
		return false;

	LIST_MEMBER_INFO_CIT it = m_listMemberInfo.begin();
	for( ; it != m_listMemberInfo.end(); ++it )
	{
		if( bTamer )
		{
			if( (*it)->m_TamerInfo.m_nUID == nUIDX )
				return true;
		}
		else
		{
			if( (*it)->m_DigimonInfo.m_nUID == nUIDX )
				return true;
		}
	}

	return false;

}

// 파티 마스터 변경
void sPartyInfo::SetPartyMaster( int const& nSlot )
{
	m_nMasterSlotNum = nSlot;
}


int sPartyInfo::GetMemberSlotNumFromName(std::wstring const& nName)
{
	LIST_MEMBER_INFO_CIT it = m_listMemberInfo.begin();
	for( ; it != m_listMemberInfo.end(); ++it )
	{
		if( 0 == (*it)->m_TamerInfo.m_Name.compare( nName ) )
			return (*it)->m_nSlotNo;
	}

	return -1;
}


// 파티 슬롯 번호로 파티원인지 체크
bool sPartyInfo::IsMember( int const& nSlot ) const
{
	SMemberInfo const* pMember = GetMemberInfoFromSlotNo( nSlot );
	if ( pMember )
		return true;
	return false;
}

bool sPartyInfo::IsMaster( int const& nSlot ) const
{
	return m_nMasterSlotNum == nSlot;
}

// 파티원 이름 얻어오기
std::wstring sPartyInfo::GetMemberName( int const& nSlot ) const
{
	SMemberInfo const* pMember = GetMemberInfoFromSlotNo( nSlot );
	if ( pMember )
		return pMember->m_TamerInfo.m_Name;
	
	return L"";
}

SMemberInfo const*  sPartyInfo::GetMemberInfoFromUIDX(int const& nUIDX) const
{
	LIST_MEMBER_INFO_CIT it = m_listMemberInfo.begin();
	for( ; it != m_listMemberInfo.end(); ++it )
	{
		if( (*it)->m_TamerInfo.m_nUID == nUIDX ||
			(*it)->m_DigimonInfo.m_nUID == nUIDX )
			return *it;
	}

	return NULL;
}

// 파티원 정보 얻기
SMemberInfo const* sPartyInfo::GetMemberInfoFromSlotNo( int const& nSlot ) const
{
	LIST_MEMBER_INFO_CIT it = m_listMemberInfo.begin();
	for( ; it != m_listMemberInfo.end(); ++it )
	{
		if( (*it)->m_nSlotNo == nSlot )
			return *it;
	}

	return NULL;
}

LIST_MEMBER_INFO const& sPartyInfo::GetList() const
{
	return m_listMemberInfo;
}

SMemberInfo * sPartyInfo::GetMemberInfo( int const& nSlot )
{
	LIST_MEMBER_INFO_IT it = m_listMemberInfo.begin();
	for( ; it != m_listMemberInfo.end(); ++it )
	{
		if( (*it)->m_nSlotNo == nSlot )
			return *it;
	}

	return NULL;
}

// 파티원 삭제
bool sPartyInfo::RemovePartyMemberFromSlotNo( int const& nSlot )
{
	LIST_MEMBER_INFO_IT it = m_listMemberInfo.begin();
	for( ; it != m_listMemberInfo.end(); ++it )
	{
		if( (*it)->m_nSlotNo == nSlot )
		{
			NISAFE_DELETE( (*it) );
			m_listMemberInfo.erase( it );
			return true;
		}
	}
	return false;
}

// 새로운 파티원 추가
bool sPartyInfo::AddNewMemberInfo( TPartyMember const& pData )
{
	if( IsMyPartySlot( pData.m_nSlotNo ) )
		return false;

	SMemberInfo * pMember = GetMemberInfo( pData.m_nSlotNo );
	if( pMember )
	{
		pMember->SetChannelNumber( pData.m_nChannelNo );
		pMember->SetMapNumber( pData.m_nMapNo );
		pMember->SetSlotNumber( pData.m_nSlotNo );
		pMember->SetDigimonInfo( pData.m_DigimonInfo );
		pMember->SetTamerInfo( pData.m_TamerInfo );
		pMember->m_bFaraway = false;
	}
	else
	{
		SMemberInfo * addInfo = NiNew SMemberInfo;	
		addInfo->SetChannelNumber( pData.m_nChannelNo );
		addInfo->SetMapNumber( pData.m_nMapNo );
		addInfo->SetSlotNumber( pData.m_nSlotNo );
		addInfo->SetDigimonInfo( pData.m_DigimonInfo );
		addInfo->SetTamerInfo( pData.m_TamerInfo );
		m_listMemberInfo.push_back( addInfo );
	}
	return true;
}

void sPartyInfo::DeleteAllMemberInfo()
{
	LIST_MEMBER_INFO_IT it = m_listMemberInfo.begin();
	for( ; it != m_listMemberInfo.end(); ++it )
		NISAFE_DELETE( (*it) );
	m_listMemberInfo.clear();
}

void sPartyInfo::CheckLeaveMemberInfo( std::list<TPartyMember> const& checkList, std::map<int,std::wstring> & slotList )
{
	struct CmpID
	{
		int slotNo;
		bool operator()(TPartyMember const& plhs)
		{
			return plhs.m_nSlotNo == slotNo;
		}
	};

	LIST_MEMBER_INFO_IT it = m_listMemberInfo.begin();
	for( ; it != m_listMemberInfo.end(); ++it )
	{
		if( IsMyPartySlot( (*it)->m_nSlotNo ) )
			continue;

		CmpID no;
		no.slotNo = (*it)->m_nSlotNo; 

		std::list<TPartyMember>::const_iterator findIT = std::find_if( checkList.begin(), checkList.end(), no );
		if( findIT != checkList.end() )
		{
			if( 0 != (*it)->m_TamerInfo.m_Name.compare( (*findIT).m_TamerInfo.m_Name ) )
				slotList.insert( std::make_pair( (*it)->m_nSlotNo, (*it)->m_TamerInfo.m_Name ) );
		}
		else
			slotList.insert( std::make_pair( (*it)->m_nSlotNo, (*it)->m_TamerInfo.m_Name ) );
	}
}

void sPartyInfo::CheckNewMemberInfo( std::list<TPartyMember> const& checkList, std::map<int,std::wstring> & slotList )
{
	struct CmpID
	{
		int slotNo;
		bool operator()(SMemberInfo const * plhs)
		{
			return plhs->m_nSlotNo == slotNo;
		}
	};

	std::list<TPartyMember>::const_iterator it = checkList.begin();
	for( ; it != checkList.end(); ++it )
	{
		if( IsMyPartySlot( (*it).m_nSlotNo ) )
			continue;

		CmpID no;
		no.slotNo = (*it).m_nSlotNo; 

		LIST_MEMBER_INFO_CIT findIT = std::find_if( m_listMemberInfo.begin(), m_listMemberInfo.end(), no );
		if( findIT != m_listMemberInfo.end() )
		{
			if( 0 != (*it).m_TamerInfo.m_Name.compare( (*findIT)->m_TamerInfo.m_Name ) )
				slotList.insert( std::make_pair( (*it).m_nSlotNo, (*it).m_TamerInfo.m_Name ) );
		}
		else
			slotList.insert( std::make_pair( (*it).m_nSlotNo, (*it).m_TamerInfo.m_Name ) );
 	}
}

//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------
int const CPartyContents::IsContentsIdentity(void)
{
	return E_CT_PARTYSYSTEM;
}

CPartyContents::CPartyContents(void):m_pSelectedMemberSlot(-1),m_AdaptBattleSystem(0),m_AdaptTutorialQuest(0), m_AdaptGotchaMachine(0),
m_AdaptMacroProtect(0)
{
	// 파티 정보 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_PARTY_INFO_INIT, this, &CPartyContents::Recv_PartyInfoInit );
	// 파티 정보 받음
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_PARTY_INFO, this, &CPartyContents::Recv_PartyInfo );
	// 테이머 이름으로 파티 초대 보냄
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_PARTY_INVITE_FROM_TAMERNAME, this, &CPartyContents::Send_PartyInvite_TamerName );
	// 오브젝트 ID로 파티 초대 보냄
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_PARTY_INVITE_FROM_UNIQUE_ID, this, &CPartyContents::Send_PartyInvite_UniqueID );

	// 파티 탈퇴 패킷 보냄
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_PARTY_LEAVE, this, &CPartyContents::Send_PartyLeave );

	// 파티 탈퇴 결과
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_PARTY_LEAVE, this, &CPartyContents::Recv_PartyLeave );
	
	// 내가 파티 생성
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_PARTY_MAKE_ME, this, &CPartyContents::Recv_Party_MakeMe );

	// 파티초대
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_PARTY_INVITE_ASK, this, &CPartyContents::Recv_PartyInvite );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_PARTY_INVITE_ANSWER_ACCEPT, this, &CPartyContents::Send_PartyInviteAnswerAccept );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_PARTY_INVITE_ANSWER_REJECT, this, &CPartyContents::Send_PartyInviteAnswerReject );
	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_PARTY_ADD_NEW_MEMBER, this, &CPartyContents::Recv_AddNewPartyMember );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_PARTY_INVITE_RESULT, this, &CPartyContents::Recv_PartyInvite_Result );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_PARTY_MEMBER_MAP_CHANGE, this, &CPartyContents::NTF_PartyMebmerMapChange );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_PARTY_MEMBER_INFO_CHANGE, this, &CPartyContents::NTF_PartyMemberInfoChange );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_PARTY_MEMBER_DIGIMON_CHANGE, this, &CPartyContents::NTF_PartyMemberDigimonChange );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_PARTY_MEMBER_ONOFFLINE, this, &CPartyContents::NTF_PartyMemberOnOffline );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_PARTY_MEMBER_BUFF_CHANGE, this, &CPartyContents::NTF_PartyMemberBuffChange );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_PARTY_MEMBER_POSITION_UPDATE, this, &CPartyContents::NTF_PartyMemberPositionUpdate );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_PARTY_EXP_UPDATE, this, &CPartyContents::NTF_PartyExpUpdate );



	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_PARTY_ITEM_LOOTING_INFO_CHANGE, this, &CPartyContents::Recv_PartyItemLootingInfoChange );	
	
	
	// 파티 멤버 강퇴 결과
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_PARTY_MEMBER_BANISH, this, &CPartyContents::Recv_PartyMemberBanish );
	// 파티 멤버 강퇴 보냄
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_PARTY_MEMBER_BANISH, this, &CPartyContents::Send_PartyMemberBanish );
	// 파티장 변경.
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_PARTY_CHANGE_MASTER, this, &CPartyContents::Recv_ChangeMaster );
	// 파티장 변경 보냄
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_PARTY_CHANGE_MASTER, this, &CPartyContents::Send_ChangeMaster );

	// 파티 멤버가 아이템을 획득했다는 패킷
	GAME_EVENT_ST.AddEvent( EVENT_CODE::NTF_PARTY_MEMBER_LOOT_ITEM, this, &CPartyContents::NTF_PartyMemberLootItem );
	// 파티원 이름을 얻는다.
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_PARTY_MEMBER_NAME, this, &CPartyContents::GetPartyMemberNameFromIdx );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_PARTY_LOOTING_WINDOW, this, &CPartyContents::OpenLootingWindow );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_PARTY_ITEMRATE_WINDOW, this, &CPartyContents::OpenLootingItemRateWindow );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_PARTY_LOOTING_INFO, this, &CPartyContents::GetLootingInfo );	
	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::IS_PARTYMEMBER_FROM_TAMERUIDX, this, &CPartyContents::IsPartyMemberFromTamerUIDX );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::IS_PARTYMEMBER_FROM_DIGIMONUIDX, this, &CPartyContents::IsPartyMemberFromDigimonUIDX );

// 	GAME_EVENT_ST.AddEvent( EVENT_CODE::SET_TARGET_MARK, this, &CPartyContents::CheckPartyMemberTargetMark );
// 	GAME_EVENT_ST.AddEvent( EVENT_CODE::RELEASE_TARGET_MARK, this, &CPartyContents::ReleasePartyMemberTargetMark );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_PARTY_UIDX, this, &CPartyContents::GetPartyUIDX );

	if( EventRouter() )
	{
		EventRouter()->Register( CONTENTS_EVENT::EstreamEvt_MainFrame_SetTarget, this );
		EventRouter()->Register( CONTENTS_EVENT::EstreamEvt_MainFrame_ReleaseTarget, this );
	}
}

CPartyContents::~CPartyContents(void)
{
	m_AdaptTutorialQuest = 0;
	m_AdaptBattleSystem = 0;
	m_AdaptGotchaMachine = 0;
	m_AdaptMacroProtect = 0;
	GAME_EVENT_ST.DeleteEventAll(this);
	if( EventRouter() )
		EventRouter()->UnRegisterAll( this );
}

int const CPartyContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CPartyContents::Initialize(void)
{
	return true;
}

void CPartyContents::UnInitialize(void)
{
}

bool CPartyContents::IntraConnection(ContentsSystem& System)
{ 
	m_AdaptTutorialQuest = new AdaptTutorialQuest;
	if( m_AdaptTutorialQuest && !m_AdaptTutorialQuest->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_TUTORIAL_CONTENTS ) ) )
		return false;

	m_AdaptBattleSystem = new AdaptBattleSystem;
	if( m_AdaptBattleSystem && !m_AdaptBattleSystem->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_BATTLE_SYSTEM ) ) )
		return false;

	m_AdaptGotchaMachine = new AdaptGotchaMachine;
	if( m_AdaptGotchaMachine && !m_AdaptGotchaMachine->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_GOTCHAMACHINE_CONTENTS ) ) )
		return false;

	m_AdaptMacroProtect = new AdaptMacroProtect;
	if (m_AdaptMacroProtect && !m_AdaptMacroProtect->SetContents(CONTENTSSYSTEM_PTR->GetContents(E_CONTENTS_TYPE::E_CT_MACROPROTECT)))
		return false;

	return true;
}

void CPartyContents::Update(float const& fElapsedTime)
{
	std::map<std::wstring,float>::iterator it = m_wsRequestTamersName.begin();
	for( ; it != m_wsRequestTamersName.end();  )
	{
		it->second -= fElapsedTime;
		if( it->second <= 0.0f )
			m_wsRequestTamersName.erase( it++ );
		else
			++it;
	}

	_CheckMemberFaraway();
}

void CPartyContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void CPartyContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EstreamEvt_MainFrame_SetTarget:
		{
			UINT nTargetUID = 0;
			kStream >> nTargetUID;
			CheckPartyMemberTargetMark( &nTargetUID );
		}
		break;
	case CONTENTS_EVENT::EstreamEvt_MainFrame_ReleaseTarget:
		{
			UINT nTargetUID = 0;
			kStream >> nTargetUID;
			ReleasePartyMemberTargetMark( &nTargetUID );
		}
		break;
	}
}

void CPartyContents::MakeMainActorData(void)
{
}

void CPartyContents::ClearMainActorData(void)
{
}

void CPartyContents::ClearWorldData(void)
{
	InitData();
}

void CPartyContents::MakeWorldData(void)
{
	InitData();
}

void CPartyContents::InitData(void)
{
	m_pSelectedMemberSlot = -1;
	m_wsInviteTamerName.c_str();	// 나를 파치 초대한 테이머 이름
	m_PartyData.InitData();

	_ClosePartyUI();

	if( g_pGameIF )
	{
		cTamerWindow* pTamerWin = g_pGameIF->GetTamerWindow();
		if( pTamerWin )
			pTamerWin->SetPartyMaster(false);

		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DIVISION );
	}

	if( g_pCharMng )
	{
		g_pCharMng->ResetNameTamer();
		g_pCharMng->ResetNameDigimon();
	}
}

// 파티 관련 UI 모두 지운다
void CPartyContents::_ClosePartyUI()
{
	SAFE_POINTER_RET( g_pGameIF );
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PARTY_WINDOW ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PARTY_WINDOW );
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_LOOTING ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_LOOTING );
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MYLOOTING ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_MYLOOTING );
}

// 파티 관련 UI 모두 생성한다.
void CPartyContents::_OpenPartyUI()
{
	SAFE_POINTER_RET( g_pGameIF );
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_PARTY_WINDOW ) )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_PARTY_WINDOW );
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_LOOTING ) )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_LOOTING );
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_MYLOOTING ) )
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_MYLOOTING );
}

void CPartyContents::_CheckMemberFaraway()
{
	if( !m_PartyData.IsParty() )
		return;

	LIST_MEMBER_INFO_IT it = m_PartyData.m_listMemberInfo.begin();
	for( ; it != m_PartyData.m_listMemberInfo.end(); ++it )
	{
		CsC_AvObject* pDest = g_pMngCollector->GetObject( (*it)->m_TamerInfo.m_nUID );
		bool bFaraway = true;
		if( pDest && pDest->GetProp_Alpha()->IsHideDistOut() == false )
			bFaraway = false;

		if( (*it)->m_bFaraway != bFaraway )
		{
			(*it)->m_bFaraway = bFaraway;

			ContentsStream kTmp;
			kTmp << (*it)->m_nSlotNo << bFaraway;
			Notify( eMEMBER_FARAWAY, kTmp );

			ContentsStream kNotify;
			kNotify << (*it)->m_TamerInfo.m_Name << bFaraway;
			NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_PartyMember_Faraway, kNotify );
			
		}
	}
}

// 내가 파티에 속해 있는가
bool CPartyContents::IsJoinParty() const
{
	return m_PartyData.IsParty();
}

// 내가 파티 장인가
bool CPartyContents::IsMePartyMaster() const
{
	if( m_PartyData.IsParty() )
		return m_PartyData.IsPartyMaster();
	return false;
}

int CPartyContents::GetPartyMasterSlot() const
{
	if( m_PartyData.IsParty() )
		return m_PartyData.GetPartyMasterSlot();
	return -1;
}

bool CPartyContents::IsPartyMaster( int const& nSlot ) const
{
	return m_PartyData.IsMaster(nSlot);
}

// 파티 초대 패킷을 보낸 상태인가?
bool CPartyContents::IsSendInvite() const
{
	return !m_wsRequestTamersName.empty();
}

// 파티 루팅 정보 셋팅
void CPartyContents::_SetPartyLooting(TItemLootingInfo const& pData)
{
	sLootingRule changeRule;
	changeRule.SetLootingRule( pData );

	if( m_PartyData.m_sLootingRule == changeRule )
		return;
		
	m_PartyData.m_sLootingRule.SetLootingRule(pData);

	std::wstring lootingRule;
	switch( m_PartyData.m_sLootingRule.m_nCropType )
	{
	case nCrop::Base:	lootingRule = UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_BASE" );		break;
	case nCrop::Order:	lootingRule = UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ORDER" );				break;
	case nCrop::Free:
		{
			if( 0 == m_PartyData.m_sLootingRule.m_nRareRate )
				lootingRule = UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_FREE" );
			else
				lootingRule = UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_RANDOM" );
		}			
		break;
	}

	std::wstring itemRare;
	switch( m_PartyData.m_sLootingRule.m_nDispRareGrade )
	{
	case nCrop::RareRate::Lv1:	itemRare = UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_ALL" );		break;
	case nCrop::RareRate::Lv2:	itemRare = UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_MAGIC" );	break;
	case nCrop::RareRate::Lv3:	itemRare = UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_REARE" );	break;
	case nCrop::RareRate::Lv4:	itemRare = UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_EPIC" );	break;
	}

	std::wstring msg;
	DmCS::StringFn::Format( msg, L"%s : %s", lootingRule.c_str(), itemRare.c_str() );
	cPrintMsg::PrintMsg( 30083, const_cast<TCHAR*>(msg.c_str()) );
}

// 파티장 슬롯 번호
void CPartyContents::_SetMasterSlotNumber( int const& nSlot )
{
	if( !m_PartyData.IsParty() ) // 파티에 가입 되어 있지 않다
		return;

	if( m_PartyData.IsMaster( nSlot ) )
		return;

	m_PartyData.SetPartyMaster( nSlot );

	cTamerWindow* pTamerWin = g_pGameIF->GetTamerWindow();
	// 내가 방장이 되는 경우
	if( m_PartyData.IsPartyMaster() )
	{
		cPrintMsg::PrintMsg( 30088 );
		if( pTamerWin )
			pTamerWin->SetPartyMaster(true);
	}
	else
	{
		std::wstring tamerName = m_PartyData.GetMemberName( nSlot );
		if( tamerName.empty() )// 파티 멤버가 아니다.
			return;

		if( pTamerWin )
			pTamerWin->SetPartyMaster(false);
		cPrintMsg::PrintMsg( 30074, const_cast<TCHAR*>(tamerName.c_str()) );		
	}

	Notify(eCHANGE_PARTY_MASTER);
}

// 파티원인가?
bool CPartyContents::IsMemberFromSlotNo( int const& nSlot ) const
{
	return m_PartyData.IsMember( nSlot );
}

// 파티원의 이름을 얻어온다.
std::wstring CPartyContents::GetMemberNameFromSlotNo(int const& nSlot) const
{
	return m_PartyData.GetMemberName(nSlot);
}

// 파티원의 이름으로 파티원 슬롯 번호를 얻어온다.
int CPartyContents::GetMemberSlotNumFromTamerName(std::wstring const& wsTamerName)
{
	if( !m_PartyData.IsParty() )
		return -1;

	if( IsMeFromName( wsTamerName ) )
		return m_PartyData.GetMyPartySlotNumber();
	
	return m_PartyData.GetMemberSlotNumFromName( wsTamerName );
}

SPartyInfo const* CPartyContents::GetPartyInfo() const
{
	return &m_PartyData;
}

// 파티원 정보를 얻는다
SMemberInfo const * CPartyContents::GetMemberInfoFromSlotNo(int const& nSlot) const
{
	if( !m_PartyData.IsParty() )
		return NULL;
	return m_PartyData.GetMemberInfoFromSlotNo( nSlot );
}

LIST_MEMBER_INFO const& CPartyContents::GetMemberList() const
{
	return m_PartyData.m_listMemberInfo;
}

// 아이템 루팅 윈도우를 생성하기 위해서 간단한 내 정보를 넘길 때 사용하기 때문에
// 다른 곳에서는 사용을 하지 말것.
bool CPartyContents::GetMySimpleInfo( SMemberInfo & myInfo )
{
	if( !m_PartyData.IsParty() )
		return false;

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pMyTamer = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pMyTamer, false );

	myInfo.m_TamerInfo.m_Name = pMyTamer->GetName();
	myInfo.m_nSlotNo = m_PartyData.GetMyPartySlotNumber();
	return true;
}

void CPartyContents::GetPartyMemberNameFromIdx(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int,std::wstring> * getData = static_cast<std::pair<int,std::wstring>*>(pData);
	if( !IsJoinParty() )
		return;
	
	getData->second = GetMemberNameFromSlotNo( getData->first );
}

void CPartyContents::GetLootingInfo(void*pData)
{
	SAFE_POINTER_RET(pData);

	sLootingRule* pget = static_cast<sLootingRule*>(pData);
	*pget = m_PartyData.m_sLootingRule;
}

bool CPartyContents::IsPartyMemberFromTamerUIDX(int const& nUIDX) const
{
	return m_PartyData.IsPartyMemberFromUIDX(true, nUIDX);
}

bool CPartyContents::IsPartyMemberFromDigimonUIDX(int const& nUIDX) const
{
	return m_PartyData.IsPartyMemberFromUIDX(false, nUIDX);
}

void CPartyContents::IsPartyMemberFromTamerUIDX(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int,bool>* pContaner = static_cast<std::pair<int,bool>*>(pData);

	pContaner->second = IsPartyMemberFromTamerUIDX( pContaner->first );
}

void CPartyContents::IsPartyMemberFromDigimonUIDX(void* pData)
{
	SAFE_POINTER_RET(pData);
	std::pair<int,bool>* pContaner = static_cast<std::pair<int,bool>*>(pData);

	pContaner->second = IsPartyMemberFromDigimonUIDX( pContaner->first );
}

bool CPartyContents::IsBattlePlaying() const		// 배틀 중인가?
{
	SAFE_POINTER_RETVAL( m_AdaptBattleSystem, false );

	return m_AdaptBattleSystem->IsBattlePlaying();	// 배틀 중일 경우
}

void CPartyContents::GetPartyMemberInfo( void* pData )
{
	SAFE_POINTER_RET(pData);
	std::pair<int,SMemberInfo>* pContaner = static_cast<std::pair<int,SMemberInfo>*>(pData);

	if( IsMemberFromSlotNo( pContaner->first ) )
		pContaner->second = *GetMemberInfoFromSlotNo(pContaner->first);
}

// 아이템 획득 방식 변경 창 열기
void CPartyContents::OpenLootingWindow(void * pData)
{
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DIVISION );
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_DIVISION );
}

void CPartyContents::OpenLootingItemRateWindow(void* pData)
{
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DIVISION );
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_DIVISION, 0, 1 );
}

void CPartyContents::Send_LootingRuleChange( int nCropType, int nItemClass, int nItemDispGrade )
{
	sLootingRule changeRule;
	changeRule.m_nCropType = nCropType;
	changeRule.m_nRareRate = nItemClass;
	changeRule.m_nDispRareGrade = nItemDispGrade;
	_SendLootingRule( changeRule );
}

void CPartyContents::Send_LootingItemRateChange( int nItemClass )
{
	sLootingRule changeRule;
	changeRule.m_nCropType = m_PartyData.m_sLootingRule.m_nCropType;
	changeRule.m_nRareRate = m_PartyData.m_sLootingRule.m_nRareRate;
	changeRule.m_nDispRareGrade = nItemClass;
	_SendLootingRule( changeRule );
}

void CPartyContents::_SendLootingRule( sLootingRule const& nRule )
{
	if( !m_PartyData.IsParty() )
	{
		cPrintMsg::PrintMsg( 30078 );
		return;
	}

	if( !m_PartyData.IsPartyMaster() )
	{
		cPrintMsg::PrintMsg( 30060 );
		return;
	}


	// 거래 중이면 실패
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
	{
		cPrintMsg::PrintMsg( 30024 );
		return;
	}

	if( m_PartyData.m_sLootingRule == nRule )
		return;

	if( net::game )
		net::game->SendPartyChangeCropType( nRule.m_nCropType, (n1)nRule.m_nRareRate, (n1)nRule.m_nDispRareGrade );
}

//////////////////////////////////////////////////////////////////////////
// 파티 요청 보냄( 테이머 이름 )
//////////////////////////////////////////////////////////////////////////
void CPartyContents::Send_PartyInvite_TamerName(void* pData)
{
	SAFE_POINTER_RET( pData );

	std::wstring wName = *static_cast<std::wstring*>(pData);

	if( wName.empty() )
	{
		cPrintMsg::PrintMsg( 30063 );
		return;
	}

	_SendPartyInvate( wName );
}

//////////////////////////////////////////////////////////////////////////
// 파티 요청 보냄( 오브젝트 ID )
//////////////////////////////////////////////////////////////////////////
void CPartyContents::Send_PartyInvite_UniqueID(void* pData)
{
	SAFE_POINTER_RET( pData );	 

	UINT nUniqueID = *static_cast<UINT*>(pData);

	// 객체가 존재 하는가
	CsC_AvObject* pTamer = g_pMngCollector->GetObject( nUniqueID );
	if( pTamer == NULL )
	{
		cPrintMsg::PrintMsg( 30063 );
		return;
	}

	// 객체가 테이머인가
	if( pTamer->GetLeafRTTI() != RTTI_TAMER )
	{
		cPrintMsg::PrintMsg( 30064 );
		return;
	}

	std::wstring tamerName = pTamer->GetName();
	_SendPartyInvate( tamerName );
}

//////////////////////////////////////////////////////////////////////////
// 서버에 파티 요청 패킷 보냄
//////////////////////////////////////////////////////////////////////////
void CPartyContents::_SendPartyInvate( std::wstring const& wsName )
{
	SAFE_POINTER_RET( net::game );

	// 메세지 박스 떠있는게 있다면 실패
	if( cMessageBox::IsMessageBox() )
	{
		cPrintMsg::PrintMsg( 30024 );
		return;
	}

	if( m_AdaptBattleSystem )
	{
		if( m_AdaptBattleSystem->IsBattleMatchingRegisted() )// 배틀 신청 중이면 파티 초대 못함
		{
			cPrintMsg::PrintMsg( 30464 );
			return;
		}

		if( m_AdaptBattleSystem->IsBattlePlaying() )	// 배틀 중일 경우
		{
			return;
		}
	}

	if( m_AdaptTutorialQuest && m_AdaptTutorialQuest->IsTutorialPlaying() )
	{
		cPrintMsg::PrintMsg( 30412 );
		return;
	}

	if (m_AdaptMacroProtect && m_AdaptMacroProtect->IsMacroProtectUsing())
	{
		cPrintMsg::PrintMsg(30024);
		return;
	}

	// 전장이냐
	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_NONE:
	case nLIB::SVR_BATTLE:	cPrintMsg::PrintMsg( 30455 );	return;
	}

	// 거래 중이면 실패
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
	{
		cPrintMsg::PrintMsg( 30024 );
		return;
	}	

	// 내가 파장이 아니면 실패
	if( m_PartyData.IsParty() )
	{
		if( !m_PartyData.IsPartyMaster() )
		{
			cPrintMsg::PrintMsg( 30060 );
			return;
		}

		// 비어 있는 파티 슬롯이 존재해야만 성공
		if( m_PartyData.GetMemberCount() == MAX_PARTY_COUNT )// 빈 슬롯 없음
		{		
			cPrintMsg::PrintMsg( 30061 );
			return;
		}
	}

	std::wstring tamerName = wsName;
	if( Language::SUCCEEDED != Language::CheckTamerName( tamerName ) )
	{
		cPrintMsg::PrintMsg( 11101 );
		return;
	}

	if( _tcscmp( g_pCharMng->GetTamerUser()->GetName(), tamerName.c_str() ) == 0 )	// 2017-06-20-nova 자기자신을 초대하면 실패
	{
		cPrintMsg::PrintMsg( 30064 );
		return;
	}

	// 파티 요청을 보냈는데 응답이 없는 테이머
	if( _IsRequestTamer( tamerName ) )
		return;

	cPrintMsg::PrintMsg( 30092, const_cast<TCHAR*>(tamerName.c_str()) );

	// 서버에 전송	
	net::game->SendPartyInvite( wsName );

	_AddRequestTamerList( tamerName );
}

bool CPartyContents::_IsRequestTamer( std::wstring const& requestTamer )
{
	std::map<std::wstring,float>::const_iterator it = m_wsRequestTamersName.find( requestTamer );
	if( it != m_wsRequestTamersName.end())
		return true;

	return false;
}

void CPartyContents::_RemoveRequestTamer( std::wstring const& requestTamer )
{
	std::map<std::wstring,float>::iterator it = m_wsRequestTamersName.begin();
	if( it == m_wsRequestTamersName.end() )
		return;
	m_wsRequestTamersName.erase( it );
}

void CPartyContents::_AddRequestTamerList( std::wstring const& addTamer )
{
	m_wsRequestTamersName.insert( std::make_pair( addTamer, 10.0f ) );// 응답이 10초 이상 없을 경우 자동 삭제 하도록 함.
}

void CPartyContents::Send_PartyInviteAnswerAccept(void* pData)
{
	net::game->SendPartyInviteResult(enPartyInviteResult::Accept, m_wsInviteTamerName );
	m_wsInviteTamerName.clear();
}

void CPartyContents::Send_PartyInviteAnswerReject(void* pData)
{
	net::game->SendPartyInviteResult(enPartyInviteResult::Reject, m_wsInviteTamerName );
	m_wsInviteTamerName.clear();
}

//////////////////////////////////////////////////////////////////////////
// 파티 탈퇴 패킷 보냄
//////////////////////////////////////////////////////////////////////////
void CPartyContents::Send_PartyLeave(void * pData)
{
	if( !m_PartyData.IsParty() )
	{
		cPrintMsg::PrintMsg( 30078 );
		return;
	}

	// 거래 중이면 실패
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
	{
		cPrintMsg::PrintMsg( 30024 );
		return;
	}

	// 배틀 중인때
	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
	{
		cPrintMsg::PrintMsg( 30455 );
		return;
	}

	if( net::game )
		net::game->SendPartyLeave();
}

//////////////////////////////////////////////////////////////////////////
// 파티 멤버 강퇴 보냄
//////////////////////////////////////////////////////////////////////////
void CPartyContents::Send_PartyMemberBanish(void* pData)
{
	SAFE_POINTER_RET(pData);

	int nBanishMemberSlotNum = *static_cast<int*>(pData);

	if( !m_PartyData.IsParty() )
	{
		cPrintMsg::PrintMsg( 30078 );
		return; // 파티에 속해 있지 않다.
	}

	// 파티장이 아닐때
	if( !m_PartyData.IsPartyMaster() )
	{
		cPrintMsg::PrintMsg( 30060 );
		return;
	}

	// 파티원이 아니다
	if( !m_PartyData.IsMember( nBanishMemberSlotNum ) )
	{
		cPrintMsg::PrintMsg( 30078 );
		return;
	}

	// 거래 중이면 실패
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
	{
		cPrintMsg::PrintMsg( 30024 );
		return;
	}

	std::wstring banishTamerName = m_PartyData.GetMemberName( nBanishMemberSlotNum );
	if( net::game )
		net::game->SendPartyKick(banishTamerName);
}

//////////////////////////////////////////////////////////////////////////
// 파티장 변경 보냄
//////////////////////////////////////////////////////////////////////////
void CPartyContents::Send_ChangeMaster(void* pData)
{
	SAFE_POINTER_RET( pData );
	int nNewMasterSlotNumber = *static_cast<int*>(pData);

	// 파티 중이 아닐 때
	if( !m_PartyData.IsParty() )
	{
		cPrintMsg::PrintMsg( 30078 );
		return;
	}

	// 내가 파티 장인가?
	if( !m_PartyData.IsPartyMaster() )
	{
		cPrintMsg::PrintMsg( 30060 );
		return;
	}

// 	// 새로운 파티장 번호가 나인경우
// 	if( !m_PartyData.IsMyPartySlot( nNewMasterSlotNumber ) )
// 	{
// 		return;
// 	}

	// 파티 멤버가 아닌 경우
	if( !m_PartyData.IsMember( nNewMasterSlotNumber ) )
	{
		return;
	}

	// 거래 중이면 실패
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
	{
		cPrintMsg::PrintMsg( 30024 );
		return;
	}

	if( net::game )
		net::game->SendPartyChangeMaster( nNewMasterSlotNumber );
}

//////////////////////////////////////////////////////////////////////////
// 파티 초대를 받을 수 있는 상태 인지 체크하는 함수
//////////////////////////////////////////////////////////////////////////
bool CPartyContents::_CheckInviteTamerState()
{
	if( g_pCharMng->GetTamerUser()->IsServerDie() )// 현재 죽었을 경우에는 처리 불가능 메세지
		return false;

	// 튜토리얼중
	if( m_AdaptTutorialQuest && m_AdaptTutorialQuest->IsTutorialPlaying() )
 		return false;

	// 매크로 프로텍트 중
	if (m_AdaptMacroProtect && m_AdaptMacroProtect->IsMacroProtectUsing())
		return false;

	if( cMessageBox::IsMessageBox() )				// 메세지 박스 떻을때는 처리 불가능 메세지	
		return false;
	
	switch( g_pMngCollector->GetSceneState() )
	{
	case CMngCollector::eSCENE_DATS:	// 닷츠센타 이동중에 초대거절 )
	case CMngCollector::eSCENE_EVENT:	// 이벤트씬 중일때 거절
		return false;
	}

	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )	// 배틀 중일때 거절
		return false;

	// 배틀 신청 중일 때도 거절해야함
	if( m_AdaptBattleSystem && m_AdaptBattleSystem->IsBattleMatchingRegisted() )
		return false;

	if( m_AdaptBattleSystem->IsBattlePlaying() )	// 배틀 중일 경우
		return false;

// 	if( g_pGameIF->GetGotchaMachine() != NULL )								// 갓차 머신 돌리는 중일 때
// 		return false;
	// 가챠머신 이용 중 초대 거절 - adapt로? ui로?
// 	if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_GOTCHA_MACHINE ) )
// 		return false;
	if( m_AdaptGotchaMachine && m_AdaptGotchaMachine->IsGotchaPlaying() )
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 파티 요청 받음.
//////////////////////////////////////////////////////////////////////////
void CPartyContents::Recv_PartyInvite(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_PARTY_MEMBER_INVITE_REQUEST * pRecv = static_cast<GS2C_RECV_PARTY_MEMBER_INVITE_REQUEST*>(pData);

	// 자동 거절 상태일때는 바로 거절
	if( g_pResist->m_Global.s_bPartyActive == false )
	{
		net::game->SendPartyInviteResult(enPartyInviteResult::Reject, pRecv->m_wsInviterTamerName );
		return;
	}

	// 파티 초대를 받을 수 있는 상태 인지 체크
	if( !_CheckInviteTamerState() )
	{
		net::game->SendPartyInviteResult(enPartyInviteResult::Impossible, pRecv->m_wsInviterTamerName );
		return;
	}

	m_wsInviteTamerName = pRecv->m_wsInviterTamerName;

	cPrintMsg::PrintMsg( 30065, const_cast<wchar_t*>(m_wsInviteTamerName.c_str()) );

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
	DmCS::StringFn::Format( CProtocol.m_wStr,L"%s:%s", UISTRING_TEXT( "PARTY_INVITE_TO_PARTY" ).c_str(), m_wsInviteTamerName.c_str() );
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

void CPartyContents::Recv_PartyInvite_Result(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_PARTY_MEMBER_INVITE_RESULT* pRecv = static_cast<GS2C_RECV_PARTY_MEMBER_INVITE_RESULT*>(pData);

	_RemoveRequestTamer( pRecv->m_wsTargetTamerName );

	switch( pRecv->m_nResultType )
	{
	case enPartyInviteResult::Impossible:	cPrintMsg::PrintMsg( 30066, const_cast<TCHAR*>(pRecv->m_wsTargetTamerName.c_str()) );	break;// 파티 요청을 응답할 수 없는 상태임
	case enPartyInviteResult::OffLine:		cPrintMsg::PrintMsg( 30067, const_cast<TCHAR*>(pRecv->m_wsTargetTamerName.c_str()) );	break;// 접속중이지 아니하거나, 존재하지 아니함
	case enPartyInviteResult::Reject:		cPrintMsg::PrintMsg( 30068, const_cast<TCHAR*>(pRecv->m_wsTargetTamerName.c_str()) );	break;// 파티 거부
	case enPartyInviteResult::AlreadyParty: cPrintMsg::PrintMsg( 30069, const_cast<TCHAR*>(pRecv->m_wsTargetTamerName.c_str()) );	break;	// 다른 파티에 가입중
	case enPartyInviteResult::Accept:				// 수락
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// 내가 파티 생성
//////////////////////////////////////////////////////////////////////////
void CPartyContents::Recv_Party_MakeMe(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_PARTY_MAKE* pRecv = static_cast<GS2C_RECV_PARTY_MAKE*>(pData);

	m_PartyData.m_unPartyID = pRecv->m_nPartyIDX;

	if( !m_PartyData.IsParty() )
	{
		InitData();
		return;
	}

	// 	// 본인의 슬롯 번호는 0번
	// 	// 파티장의 슬롯 번호 : 0번 ==> 본인이 파티장임
	m_PartyData.m_nMySlotNumber = 0;
	m_PartyData.SetDefaultLootingRule();
	_SetMasterSlotNumber( 0 );
	_OpenPartyUI();
	cPrintMsg::PrintMsg( 30085 );//수집 퀘스트인 경우 선행 공격자를 기준으로 합니다. 
}

void CPartyContents::Recv_PartyInfoInit(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_PARTY_INFO* pRecv = static_cast<GS2C_RECV_PARTY_INFO*>(pData);

	// 내가 파티가 아니고 변경된 파티 인덱스가 없을 경우
	if( !m_PartyData.IsParty() && 0 == pRecv->m_nPartyIDX )
		return;
	
	// 내가 속한 파티가 맵 로딩중 해체 됬을 경우
	if( m_PartyData.IsParty() && 0 == pRecv->m_nPartyIDX )
	{
		InitData();
		cPrintMsg::PrintMsg( 30076 );	
		NotifyContents( CONTENTS_EVENT::EEvt_Party_Leave );// 내가 파티 떠남.
	}
	// 내가 파티가 없는 상태에서 파티 정보가 있는 경우
	else if( !m_PartyData.IsParty() && 0 != pRecv->m_nPartyIDX)
	{
		m_PartyData.m_unPartyID = pRecv->m_nPartyIDX;

		std::list<TPartyMember>::const_iterator it = pRecv->m_listMemberInfo.begin();
		for( ; it != pRecv->m_listMemberInfo.end(); ++it )
			m_PartyData.AddNewMemberInfo( (*it) );

		_SetMasterSlotNumber( pRecv->m_nMasterSlotNo );

		// 파티 루팅 정보 셋팅
		_SetPartyLooting( pRecv->m_lootingInfo );

		_OpenPartyUI();

		cPrintMsg::PrintMsg( 30085 );
		cWindow::PlaySound( "System\\System_quest_01.wav" );

		if( g_pCharMng )
		{
			g_pCharMng->ResetNameTamer();
			g_pCharMng->ResetNameDigimon();
		}

		if( g_pCharResMng )
		{
			CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
			if( pTarget )
			{
				UINT nTargetUIDX = pTarget->GetUniqID();
				CheckPartyMemberTargetMark( &nTargetUIDX );
			}
		}

		NotifyContents( CONTENTS_EVENT::EEvt_Party_New );// 내가 새로운 파티에 들어왔다
	}
	else // 파티 정보 갱신
	{
		m_PartyData.m_unPartyID = pRecv->m_nPartyIDX;
		
		// 루팅 정보가 변경 됬는지 체크
		_SetPartyLooting(pRecv->m_lootingInfo);

		std::map<int,std::wstring> listLeaveMemberSlot;
		m_PartyData.CheckLeaveMemberInfo( pRecv->m_listMemberInfo, listLeaveMemberSlot);

		std::map<int,std::wstring> listNewMemberSlot;
		m_PartyData.CheckNewMemberInfo( pRecv->m_listMemberInfo, listNewMemberSlot);

		if( !listLeaveMemberSlot.empty() || !listNewMemberSlot.empty() )
			m_PartyData.DeleteAllMemberInfo();

		std::list<TPartyMember>::const_iterator it = pRecv->m_listMemberInfo.begin();
		for( ; it != pRecv->m_listMemberInfo.end(); ++it )
			m_PartyData.AddNewMemberInfo( (*it) );

		// 파티 마스터가 변경되 됬는지 체크
		_SetMasterSlotNumber( pRecv->m_nMasterSlotNo );

		Notify( eMEMBER_REFRESH );

		std::map<int,std::wstring>::iterator leaveit = listLeaveMemberSlot.begin();
		for( ; leaveit != listLeaveMemberSlot.end(); ++leaveit )
			cPrintMsg::PrintMsg( 30072, const_cast<TCHAR*>(leaveit->second.c_str()) );

		std::map<int,std::wstring>::iterator newit = listNewMemberSlot.begin();
		for( ; newit != listNewMemberSlot.end(); ++newit )
			cPrintMsg::PrintMsg( 30070, const_cast<TCHAR*>(newit->second.c_str()) );

	}	
}
//////////////////////////////////////////////////////////////////////////
// 파티 정보 받음.
//////////////////////////////////////////////////////////////////////////
void CPartyContents::Recv_PartyInfo(void* pData)
{
	SAFE_POINTER_RET(pData);

	GS2C_RECV_PARTY_INFO* pRecv = static_cast<GS2C_RECV_PARTY_INFO*>(pData);

	m_PartyData.m_unPartyID = pRecv->m_nPartyIDX;

	if( !m_PartyData.IsParty())
	{
		InitData();
		return;
	}

	m_PartyData.m_nMySlotNumber = pRecv->m_nMySlotNo;

	std::list<TPartyMember>::const_iterator it = pRecv->m_listMemberInfo.begin();
	for( ; it != pRecv->m_listMemberInfo.end(); ++it )
		m_PartyData.AddNewMemberInfo( (*it) );

	_SetMasterSlotNumber( pRecv->m_nMasterSlotNo );

	// 파티 루팅 정보 셋팅
	_SetPartyLooting( pRecv->m_lootingInfo );

	_OpenPartyUI();
	cPrintMsg::PrintMsg( 30085 );
	cWindow::PlaySound( "System\\System_quest_01.wav" );

	if( g_pCharMng )
	{
		g_pCharMng->ResetNameTamer();
		g_pCharMng->ResetNameDigimon();
	}

	if( g_pCharResMng )
	{
		CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
		if( pTarget )
		{
			UINT nTargetUIDX = pTarget->GetUniqID();
			CheckPartyMemberTargetMark( &nTargetUIDX );
		}
	}

	NotifyContents( CONTENTS_EVENT::EEvt_Party_New );// 내가 새로운 파티에 들어왔다
}
//////////////////////////////////////////////////////////////////////////
// 새로운 파티원 정보 받음.
//////////////////////////////////////////////////////////////////////////
void CPartyContents::Recv_AddNewPartyMember(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ADD_NEW_PARTY_MEMBER * pRecv = static_cast<GS2C_RECV_ADD_NEW_PARTY_MEMBER*>(pData);

	if( !m_PartyData.AddNewMemberInfo( pRecv->m_memberInfo ) )
		return;

	std::wstring newTamerName = pRecv->m_memberInfo.m_TamerInfo.m_Name;
	
	cWindow::PlaySound( "System\\System_quest_01.wav" );
	cPrintMsg::PrintMsg( 30070, const_cast<TCHAR*>(newTamerName.c_str()) );
	
	ContentsStream kTmp;
	kTmp << pRecv->m_memberInfo.m_nSlotNo;
	Notify(eADD_NEW_PARTY_MEMBER, kTmp);

	ContentsStream kNewMember;
	kNewMember << newTamerName;
	NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_PartyMember_New, kNewMember );// 새로운 파티 맴버가 들어왔다

	if( g_pCharMng )
	{
		g_pCharMng->ResetNameTamer();
		g_pCharMng->ResetNameDigimon();
	}
	
	if( g_pCharResMng )
	{
		CsC_AvObject* pTarget = g_pCharResMng->GetTargetMark();
		if( pTarget )
		{
			UINT nTargetUIDX = pTarget->GetUniqID();
			CheckPartyMemberTargetMark( &nTargetUIDX );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 파티 탈퇴 결과
//////////////////////////////////////////////////////////////////////////
void CPartyContents::Recv_PartyLeave(void * pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_PARTY_MEMBER_LEAVE * recv = static_cast<GS2C_RECV_PARTY_MEMBER_LEAVE*>(pData);

	if( !m_PartyData.IsParty() )
		return; // 파티에 속해 있지 않다.

	// 탈퇴 맴버가 나인가?
	if( m_PartyData.IsMyPartySlot( recv->m_cLeaveMemberSlotNum ) )
	{// 내가 탈퇴 대상일 경우 파티 데이터 초기화
		InitData();		
		cPrintMsg::PrintMsg( 30076 );	

		NotifyContents( CONTENTS_EVENT::EEvt_Party_Leave );// 내가 파티 떠남.
	}
	else
	{
		std::wstring tamerName = m_PartyData.GetMemberName( recv->m_cLeaveMemberSlotNum );
		if( tamerName.empty() )// 파티 멤버가 아니다.
			return;

		if( !m_PartyData.RemovePartyMemberFromSlotNo( recv->m_cLeaveMemberSlotNum ) )
			return;

		ContentsStream kTmp;
		kTmp << recv->m_cLeaveMemberSlotNum;
		Notify(eMEMBER_LEAVE, kTmp);

		if( m_pSelectedMemberSlot == recv->m_cLeaveMemberSlotNum )
			m_pSelectedMemberSlot = -1;

		cPrintMsg::PrintMsg( 30072, const_cast<TCHAR*>(tamerName.c_str()) );	

		ContentsStream kLeaveMember;
		kLeaveMember << tamerName;
		NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_PartyMember_Leave, kLeaveMember );// 내가 파티 떠남.		
	}	

	if( m_PartyData.GetMemberCount() == 0 )
		InitData();

	if( g_pCharMng )
	{
		g_pCharMng->ResetNameTamer();
		g_pCharMng->ResetNameDigimon();
	}
}

//////////////////////////////////////////////////////////////////////////
// 파티 멤버 강퇴 결과
//////////////////////////////////////////////////////////////////////////
void CPartyContents::Recv_PartyMemberBanish(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_PARTY_MEMBER_BANISH * recv = static_cast<GS2C_RECV_PARTY_MEMBER_BANISH*>(pData);

	if( !m_PartyData.IsParty() )
		return; // 파티에 속해 있지 않다.

	// 내가 강퇴 대상인가?
	if( m_PartyData.IsMyPartySlot( recv->m_cBanishMemberSlotNum ) )
	{// 내가 강퇴 대상일 경우 파티 데이터 초기화

		NotifyContents( CONTENTS_EVENT::EEvt_Party_Banish );// 내가 파티에서 강퇴 됬다.
		InitData();

		cPrintMsg::PrintMsg( 30080 );
	}
	else
	{
		std::wstring tamerName = m_PartyData.GetMemberName( recv->m_cBanishMemberSlotNum );
		if( tamerName.empty() )// 파티 멤버가 아니다.
			return;

		if( !m_PartyData.RemovePartyMemberFromSlotNo( recv->m_cBanishMemberSlotNum ) )
			return;

		ContentsStream kTmp;
		kTmp << recv->m_cBanishMemberSlotNum;
		Notify(eMEMBER_BANISH, kTmp);
		cPrintMsg::PrintMsg( 30079, const_cast<TCHAR*>(tamerName.c_str()) );

		ContentsStream kLeaveMember;
		kLeaveMember << tamerName;
		NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_PartyMember_Banish, kLeaveMember );// 내가 파티 떠남.		

		if( m_pSelectedMemberSlot == recv->m_cBanishMemberSlotNum )
			m_pSelectedMemberSlot = -1;

		if( m_PartyData.GetMemberCount() == 0 )
			InitData();
	}

	if( g_pCharMng )
	{
		g_pCharMng->ResetNameTamer();
		g_pCharMng->ResetNameDigimon();
	}
}

//////////////////////////////////////////////////////////////////////////
// 파티장 변경.
//////////////////////////////////////////////////////////////////////////
void CPartyContents::Recv_ChangeMaster(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_PARTY_MASTER_CHANGE* recv = static_cast<GS2C_RECV_PARTY_MASTER_CHANGE*>(pData);

	_SetMasterSlotNumber( recv->m_nSlotNo );
}

bool CPartyContents::IsMeFromName( std::wstring const& wsName )
{
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pMyTamer = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pMyTamer, false );

	std::wstring myName = pMyTamer->GetName();
	return (0 == wsName.compare( myName )) ? true : false;
}

//////////////////////////////////////////////////////////////////////////
// 파티 멤버가 아이템을 획득했다는 패킷
//////////////////////////////////////////////////////////////////////////
void CPartyContents::NTF_PartyMemberLootItem(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_PARTY_GET_ITEM* recv = static_cast<GS2C_NTF_PARTY_GET_ITEM*>(pData);

	if( !IsJoinParty() )
		return;

	CsItem* pGetItem = nsCsFileTable::g_pItemMng->GetItem( recv->m_GetItem.m_nType );
	SAFE_POINTER_RET(pGetItem);

	CsItem::sINFO* pItemInfo = pGetItem->GetInfo();
	SAFE_POINTER_RET(pItemInfo);

	int nMemberSlot = -1;
	bool IsMe = IsMeFromName( recv->m_szRealCropTamerName );
	if( !IsMe )
		nMemberSlot = GetMemberSlotNumFromTamerName( recv->m_szRealCropTamerName );
	else
		nMemberSlot = m_PartyData.GetMyPartySlotNumber();

	if( -1 == nMemberSlot )
		return;

	sLootingItems lootingInfo;
	std::wstring itemName = pItemInfo->s_szName;

	std::list<TDiceResult>::iterator it = recv->m_DiceResult.begin();
	for( ; it != recv->m_DiceResult.end(); ++it )
	{
		std::wstring msg = UISTRING_TEXT( "PARTY_DICE_RANDOM_ITEM" );
		DmCS::StringFn::Replace( msg, L"#Tamer#", (*it).m_szDiceTamerName.c_str() );
		DmCS::StringFn::Replace( msg, L"#Item#", itemName.c_str() );
		DmCS::StringFn::Replace( msg, L"#Num#", (*it).m_nDiceNumber );
		ContentsStream kStream;
		bool bParcing = false;	bool bCheckSameString = false;
		kStream << msg << bParcing << bCheckSameString;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_SYSTEM_PROCESS, &kStream );

		if(0 == (*it).m_szDiceTamerName.compare( recv->m_szRealCropTamerName ) )
			lootingInfo.m_nDiceNumber = (*it).m_nDiceNumber;
	}	

	lootingInfo.m_nItemCount = recv->m_GetItem.GetCount();
	lootingInfo.m_nSlotNum = nMemberSlot;
	lootingInfo.m_dwItemType = recv->m_GetItem.m_nType;
	cIconMng::_IconIDToTexIndex( lootingInfo.m_dwItemIconType, lootingInfo.m_dwItemIconIdx, NULL, pItemInfo->s_nIcon );
	lootingInfo.m_sLootingrule = m_PartyData.m_sLootingRule;	
	lootingInfo.m_ItemColor = g_pDataMng->GetItemColor( pItemInfo );
	ContentsStream kTmp;
	kTmp << lootingInfo << itemName;
	if( IsMe )
		Notify( eADD_ME_LOOTING_ITEM_VIEW, kTmp );
	else
		Notify( eADD_MEMBER_LOOTING_ITEM_VIEW, kTmp );
}

//////////////////////////////////////////////////////////////////////////
// 파티 멤버 맵 이동
//////////////////////////////////////////////////////////////////////////
void CPartyContents::NTF_PartyMebmerMapChange(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_PARTY_MEMBER_MAP_CHANGE* recv = static_cast<GS2C_NTF_PARTY_MEMBER_MAP_CHANGE*>(pData);

	if( !m_PartyData.IsParty() )
		return;

	if( !m_PartyData.IsMember( recv->m_nSlotNo ) )
		return;

	SMemberInfo * pInfo = m_PartyData.GetMemberInfo( recv->m_nSlotNo );
	SAFE_POINTER_RET( pInfo );

	pInfo->SetMapNumber( recv->m_nMapNo );
	pInfo->SetChannelNumber( recv->m_nChannelNo );
	pInfo->SetTamerUID( recv->m_nTamerUID );
	pInfo->SetDigimonUID( recv->m_nDigimonUID );

	ContentsStream kTmp;
	kTmp << recv->m_nSlotNo;
	Notify(eMEMBER_MAP_CHANGE, kTmp);

	NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_PartyMember_Map_Change, kTmp);

	if( g_pCharMng )
	{
		g_pCharMng->ResetNameTamer();
		g_pCharMng->ResetNameDigimon();
	}
}

//////////////////////////////////////////////////////////////////////////
// 파티 멤버 정보 변경
//////////////////////////////////////////////////////////////////////////
void CPartyContents::NTF_PartyMemberInfoChange(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_PARTY_MEMBER_INFO_CHANGE* recv = static_cast<GS2C_NTF_PARTY_MEMBER_INFO_CHANGE*>(pData);

	if( !m_PartyData.IsParty() )
		return;

	if( !m_PartyData.IsMember( recv->m_nSlotNo ) )
		return;

	SMemberInfo * pInfo = m_PartyData.GetMemberInfo( recv->m_nSlotNo );
	SAFE_POINTER_RET( pInfo );

	pInfo->m_DigimonInfo.SetType(recv->m_nDigimonType);
	pInfo->m_TamerInfo.SetLevel(recv->m_nTamerLevel);
	pInfo->m_DigimonInfo.SetLevel(recv->m_nDigimonLevel);
	pInfo->m_TamerInfo.SetBaseInfo(recv->m_TamerBaseInfo);
	pInfo->m_DigimonInfo.SetBaseInfo(recv->m_DigimonBaseInfo);
	
	ContentsStream kTmp;
	kTmp << recv->m_nSlotNo;
	Notify( eUPDATE_MEMBER_INFO, kTmp );

	NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_PartyMember_Info_Change, kTmp);

	if( g_pCharMng )
	{
		g_pCharMng->ResetNameTamer();
		g_pCharMng->ResetNameDigimon();
	}
}

//////////////////////////////////////////////////////////////////////////
// 파티 멤버 디지몬 변경
//////////////////////////////////////////////////////////////////////////
void CPartyContents::NTF_PartyMemberDigimonChange(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_PARTY_MEMBER_DIGIMON_CHANGE* recv = static_cast<GS2C_NTF_PARTY_MEMBER_DIGIMON_CHANGE*>(pData);

	if( !m_PartyData.IsParty() )
		return;

	if( !m_PartyData.IsMember( recv->m_nSlotNo ) )
		return;

	SMemberInfo * pInfo = m_PartyData.GetMemberInfo( recv->m_nSlotNo );
	SAFE_POINTER_RET( pInfo );

	pInfo->m_DigimonInfo.SetType(recv->m_nDigimonType);
	pInfo->m_DigimonInfo.SetName(recv->m_szDigimonName);	
	pInfo->m_DigimonInfo.SetBaseInfo(recv->m_DigimonBaseInfo);

	ContentsStream kTmp;
	kTmp << recv->m_nSlotNo;
	Notify( eMEMBER_DIGIMON_CHANGE, kTmp );	

	NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_PartyMember_Digimon_Change, kTmp);
}

//////////////////////////////////////////////////////////////////////////
// 파티 멤버 온/오프 라인 정보
//////////////////////////////////////////////////////////////////////////
void CPartyContents::NTF_PartyMemberOnOffline(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_PARTY_MEMBER_ONOFFLINE* recv = static_cast<GS2C_NTF_PARTY_MEMBER_ONOFFLINE*>(pData);

	if( !m_PartyData.IsParty() )
		return;

// 	SMemberInfo * pInfo = m_PartyData.GetMemberInfo( recv->m_nSlotNo );
// 	SAFE_POINTER_RET( pInfo );
// 
// 	pInfo->SetOnOffLine( recv->m_bOnline );
// 	ContentsStream kTmp;
// 	kTmp << recv->m_nSlotNo;
// 	Notify( eMEMBER_ONOFFLINE_CHANGE, kTmp );

	// 현재는 파티원이 접속 종료하게 되면 파티에서 탈퇴 시킨다.
	std::wstring tamerName = m_PartyData.GetMemberName( recv->m_nSlotNo );
	if( tamerName.empty() )// 파티 멤버가 아니다.
		return;

	if( !m_PartyData.RemovePartyMemberFromSlotNo( recv->m_nSlotNo ) )
		return;

	n1 removeSlot = recv->m_nSlotNo;
	ContentsStream kTmp;
	kTmp << removeSlot;
	Notify(eMEMBER_LEAVE, kTmp);

	cPrintMsg::PrintMsg( 30082, const_cast<TCHAR*>(tamerName.c_str()) );			

	if( m_PartyData.GetMemberCount() == 0 )
		InitData();
}


//////////////////////////////////////////////////////////////////////////
// 파티 멤버 버프 정보 변경
//////////////////////////////////////////////////////////////////////////
void CPartyContents::NTF_PartyMemberBuffChange(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_PARTY_MEMBER_BUFF_CHANGE* recv = static_cast<GS2C_NTF_PARTY_MEMBER_BUFF_CHANGE*>(pData);

// 	if(0 == nStatus)
// 	{
// 		g_pDataMng->GetParty()->DeleteBuff( nSlotNo, nBuffCode );
// 	}
// 	else
// 	{
// 		g_pDataMng->GetParty()->DeleteBuff( nSlotNo, nBuffCode );
// 		g_pDataMng->GetParty()->AddBuff( nSlotNo, nBuffCode );
// 
// 	}
}

//////////////////////////////////////////////////////////////////////////
// 파티원 위치 정보 갱신
//////////////////////////////////////////////////////////////////////////
void CPartyContents::NTF_PartyMemberPositionUpdate(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_PARTY_MEMBER_POSITION_UPDATE* recv = static_cast<GS2C_NTF_PARTY_MEMBER_POSITION_UPDATE*>(pData);

	if( !m_PartyData.IsParty() )
		return;

	if( !m_PartyData.IsMember( recv->m_nSlotNo ) )
		return;

	SMemberInfo * pInfo = m_PartyData.GetMemberInfo( recv->m_nSlotNo );
	SAFE_POINTER_RET( pInfo );

	pInfo->SetTamerPos(recv->m_tPos);
	pInfo->SetDigimonPos(recv->m_dPos);

	//Notify( eMEMBER_POSITION_CHANGE );
}

//////////////////////////////////////////////////////////////////////////
// 파티상태 일 때 추가 경험치 패킷 처리
//////////////////////////////////////////////////////////////////////////
void CPartyContents::NTF_PartyExpUpdate(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_NTF_PARTY_EXP_UPDATE* recv = static_cast<GS2C_NTF_PARTY_EXP_UPDATE*>(pData);
	
	u8 nTamerGainExp = CsFloat2Int64( recv->m_nTamerGainExp * 0.01f );
	u8 nTamerBonusExp = CsFloat2Int64( recv->m_nTamerBonusExp * 0.01f );
	u8 nTamerExp = CsFloat2Int64( recv->m_nTamerExp * 0.01f );

	u8 nDigimonGainExp = CsFloat2Int64( recv->m_nDigimonGainExp * 0.01f );
	u8 nDigimonBonusExp = CsFloat2Int64( recv->m_nDigimonBonusExp * 0.01f );
	u8 nDigimonExp = CsFloat2Int64( recv->m_nDigimonExp * 0.01f );

	if( recv->m_nDigimonBonusExp == 0 )
	{
		u8 TGain = nTamerGainExp + nTamerBonusExp;
		u8 DGain = nDigimonGainExp + nDigimonBonusExp;
		cPrintMsg::PrintMsg( 30090,const_cast<TCHAR*>(recv->m_szOfferTamerName.c_str()), &TGain, &DGain );
	}
	else
	{
		u8 TGain = nTamerGainExp + nTamerBonusExp;
		u8 DGain = nDigimonGainExp + nDigimonBonusExp;
		u8 TBonusGain = nTamerBonusExp;
		u8 DBonusGain = nDigimonBonusExp;
		cPrintMsg::PrintMsg( 30091, const_cast<TCHAR*>(recv->m_szOfferTamerName.c_str()), &TGain, &TBonusGain, &DGain, &DBonusGain );

#ifdef _DEBUG
		TCHAR sz[ 128 ];		
		_stprintf_s( sz, 128, _ONLY_KOR( "추가 경험치 %.2f%%를 획득하였음" ), ((float)nDigimonBonusExp / (float)( DGain - nDigimonBonusExp ) ) * 100.0f );		
		ContentsStream kStream;
		wstring str = sz;
		bool bParcing = false;	bool bCheckSameString = false;
		kStream << str << bParcing << bCheckSameString;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_SYSTEM_PROCESS, &kStream );
#endif
	}

	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );
	pUser->SetExp(nTamerExp);

	uint uiDigimonIDX = GetIDX( recv->m_nDigimonUID );
	CDigimon* pDigimon = g_pCharMng->GetDigimon( uiDigimonIDX ); 
	SAFE_POINTER_RET( pDigimon );
	if( pDigimon->GetLeafRTTI() != RTTI_DIGIMON_USER )
		return;

	CDigimonUser* pDigimonUser = static_cast<CDigimonUser*>( pDigimon );

	CDigimon::sBASE_STAT* pBaseStat = pDigimonUser->GetBaseStat();
	SAFE_POINTER_RET( pBaseStat );
	pBaseStat->SetExp( nDigimonExp );

	// begin 스킬 경험치+
	if(recv->m_nSkillExp > 0)
	{
		CDigimonEvolveObj* pEvolveObj = pDigimonUser->GetFTEvolCurObj();
		if( pEvolveObj )
		{
			cEvoUnit* pEvoUnit = pDigimonUser->GetAttributeEvoUnit( pEvolveObj->m_nEvoSlot );
			if( pEvoUnit )
				pEvoUnit->m_nSkillExp = CsFloat2Int( recv->m_nSkillExp );
		}
		
	}
	// end 스킬 경험치+
}

//////////////////////////////////////////////////////////////////////////
// 파티 아이템 루팅 정보 변경
//////////////////////////////////////////////////////////////////////////
void CPartyContents::Recv_PartyItemLootingInfoChange(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_PARTY_LOOTING_INFO_CHANGE* recv = static_cast<GS2C_RECV_PARTY_LOOTING_INFO_CHANGE*>(pData);

	_SetPartyLooting( recv->m_lootingInfo );
	Notify( eCHANGE_LOOTING_RULE );
}


void CPartyContents::CheckPartyMemberTargetMark( void* pData )
{
	SAFE_POINTER_RET( pData );
	UINT nTargetUIDX = *static_cast<UINT*>(pData);

	if( !m_PartyData.IsParty() )
		return;

	SMemberInfo const* pMemberInfo = m_PartyData.GetMemberInfoFromUIDX( nTargetUIDX );
	if( pMemberInfo )
	{
		m_pSelectedMemberSlot = pMemberInfo->m_nSlotNo;
		ContentsStream kTmp;
		kTmp << m_pSelectedMemberSlot;
		Notify( eMEMBER_SET_TARGET, kTmp );
	}
	else
	{
		if( -1 != m_pSelectedMemberSlot )
		{
			ContentsStream kTmp;
			kTmp << m_pSelectedMemberSlot;
			Notify( eMEMBER_RELEASE_TARGET, kTmp );
			m_pSelectedMemberSlot = -1;
		}
	}
}

void CPartyContents::ReleasePartyMemberTargetMark(void* pData)
{
	SAFE_POINTER_RET( pData );
	UINT nTargetUIDX = *static_cast<UINT*>(pData);

	if( !m_PartyData.IsParty() )
		return;

	SMemberInfo const* pMemberInfo = m_PartyData.GetMemberInfoFromUIDX( nTargetUIDX );

	if( pMemberInfo )
	{
		m_pSelectedMemberSlot = pMemberInfo->m_nSlotNo;
		ContentsStream kTmp;
		kTmp << m_pSelectedMemberSlot;
		Notify( eMEMBER_RELEASE_TARGET, kTmp );		
	}
}

void CPartyContents::GetPartyUIDX(void* pData)
{
	SAFE_POINTER_RET( pData );
	UINT * nPartyUIDX = static_cast<UINT*>(pData);

	*nPartyUIDX = m_PartyData.GetPartyIDX();
}