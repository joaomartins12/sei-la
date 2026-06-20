#include "StdAfx.h"
#include "CharacterSelectContents.h"

#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "../../ContentsSystem/ContentsSystemDef.h"


CharacterSelectContents::sCharacterInfo::sCharacterInfo(int nSlotNum, bool bLock):m_nSlotNumber(nSlotNum),
m_nMapID(0),m_nCharImgIdx(-1),m_nTamerObjectTBIdx(0),m_nTamerLevel(1),m_nDigimonObjectTBIdx(0),m_nDigimonLevel(1),
m_fDigimonScale(1.0f),m_nDigimonEffectType(0),m_nPat_Leader(0),m_pTamer(NULL),m_pDigimon(NULL),m_pPat(NULL),m_nServerChangeWait(0),
m_bLocked(bLock),m_pTamerDummy(NULL),m_pDigimonDummy(NULL)
{
}

CharacterSelectContents::sCharacterInfo::sCharacterInfo():m_nSlotNumber(0),
m_nMapID(0),m_nCharImgIdx(-1),m_nTamerObjectTBIdx(0),m_nTamerLevel(1),m_nDigimonObjectTBIdx(0),m_nDigimonLevel(1),
m_fDigimonScale(1.0f),m_nDigimonEffectType(0),m_nPat_Leader(0),m_pTamer(NULL),m_pDigimon(NULL),m_pPat(NULL),m_nServerChangeWait(0),
m_bLocked(false)
{
}


CharacterSelectContents::sCharacterInfo::~sCharacterInfo()
{
	DeleteData();
}

// 슬롯 번호는 남기고 지운다.
void CharacterSelectContents::sCharacterInfo::ResetData()
{
	m_nMapID = 0;
	m_nCharImgIdx = -1;

	m_szTamerName.clear();
	m_szMapName.clear();

	m_nTamerObjectTBIdx = 0;
	m_nTamerLevel = 1;

	m_szDigimonName.clear();
	m_nDigimonObjectTBIdx = 0;
	m_nDigimonLevel = 1;
	m_fDigimonScale = 1.0f;
	m_nDigimonEffectType = 0;

	m_nPat_Leader = 0;

	if( m_pTamer )
	{
		g_pCharMng->DeleteTempChar( m_pTamer );
		m_pTamer = NULL;
	}

	if( m_pDigimon )
	{
		g_pCharMng->DeleteTempChar( m_pDigimon );
		m_pDigimon = NULL;
	}

	if( m_pPat )
	{
		g_pCharMng->DeleteTempChar( m_pPat );
		m_pPat = NULL;
	}

	SAFE_NIDELETE( m_pTamerDummy );
	SAFE_NIDELETE( m_pDigimonDummy );
}

void CharacterSelectContents::sCharacterInfo::DeleteData()
{
	m_nSlotNumber = -1;
	ResetData();
}

bool CharacterSelectContents::sCharacterInfo::IsServerChangeWaitCharactor() const
{
	return (m_nServerChangeWait == nTamer::eState::RelocateWaitTime) ? true : false;
}

bool CharacterSelectContents::sCharacterInfo::IsHaveCharInfo() const
{
	if( m_nTamerObjectTBIdx == 0 )
		return false;

// 	if( NULL == m_pTamer )
// 		return false;

	return true;
}

bool CharacterSelectContents::sCharacterInfo::IsLockSlot() const
{
	return m_bLocked;
}

CTamer* CharacterSelectContents::sCharacterInfo::CreateTamer(cItemData const* pEquipInfo, uint const& unTBIdx, bool const& bGray)
{
	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
	prop.s_dwOptionPlag = OP_STAT | OP_LIGHT;
	if( bGray )
		prop.s_eInsertGeomType = CGeometry::GetCharOutLineGrayType();
	else
		prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	//assert_cs( nLimit::Equip == nsPART::MAX_TOTAL_COUNT );

	CsC_PartObject::sCHANGE_PART_INFO cp[ nsPART::MAX_TOTAL_COUNT ];
	for( int tp=0; tp<nsPART::MAX_TOTAL_COUNT/*nLimit::Equip*/; ++tp )
	{
		// 디지바이스
		if( tp >= nTamer::MaxParts )
		{
			cp[ tp ].s_nPartIndex = tp;
			break;
		}

		cp[ tp ].s_nFileTableID = pEquipInfo[ tp ].m_nType;
		cp[ tp ].s_nRemainTime	= pEquipInfo[ tp ].m_nEndTime;			
		cp[ tp ].s_nPartIndex = tp;	
	}

	return g_pCharMng->AddTempTamer( &prop, unTBIdx, NiPoint3( -115, 0, -115 ), -0.5f, NULL, cp, 0 );
// 	CTamer* pTamer = g_pCharMng->AddTempTamer( &prop, unTBIdx, NiPoint3( -115, 0, -115 ), -0.5f, NULL, cp, 0 );
// 	if( pTamer )
// 	{
// 		CsC_EffectProp* pEffProp =	pTamer->GetProp_Effect();
// 		if( pEffProp )
// 			pEffProp->AddLoopEffect( CsC_EffectProp::LE_COSEFFECT, "System\\Character CRoom\\Shadow.nif" );
// 	}
// 
// 	return pTamer;
}

void CharacterSelectContents::sCharacterInfo::SetDigimonElement(uint const& unTBIdx)
{
	m_listDigimonEl.clear();

	CsDigimon* pDigimonInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( unTBIdx );
	SAFE_POINTER_RET( pDigimonInfo );

	CsDigimon::sINFO* pFTDigimon = pDigimonInfo->GetInfo();
	SAFE_POINTER_RET( pFTDigimon );

	for( int i=0; i<3; ++i )
	{
		if( pFTDigimon->s_eFamilyType[ i ] == 0 )
			continue;

		sDigimonElement add;
		add.nIconType = cIconMng::Family;
		add.nIconIdx = pFTDigimon->s_eFamilyType[ i ] - nsCsDigimonTable::FT_DR;
		add.nIconItemType = ICONITEM::DAttribute;
		m_listDigimonEl.push_back( add );
	}

	{
		sDigimonElement add;
		add.nIconType = cIconMng::DAttribute;
		add.nIconIdx = pFTDigimon->s_eAttributeType - nsCsDigimonTable::AT_NONE;
		add.nIconItemType = ICONITEM::DAttribute;
		m_listDigimonEl.push_back( add );
	}

	for( int i=0; i<3; ++i )
	{
		if(0 == pFTDigimon->s_eBaseNatureTypes[i] )
			continue;

		sDigimonElement add;
		add.nIconType = cIconMng::Nature;
		add.nIconIdx = pFTDigimon->s_eBaseNatureTypes[i] - nsCsDigimonTable::NT_ICE;
		add.nIconItemType = ICONITEM::DAttribute;
		m_listDigimonEl.push_back( add );
	}
}

CDigimon* CharacterSelectContents::sCharacterInfo::CreateDigimon(uint const& unTBIdx, uint const& nEffectType, bool const& bGray)
{
	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
	prop.s_dwOptionPlag = OP_STAT | OP_LIGHT;
	if( bGray )
		prop.s_eInsertGeomType = CGeometry::GetCharOutLineGrayType();
	else
		prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	CDigimon* pDigimon = g_pCharMng->AddTempDigimon( &prop, unTBIdx, NULL, NiPoint3( -20, 0, -110 ), -0.1f, NULL );

	if( pDigimon )
	{
		pDigimon->GetBaseStat()->SetLevel( m_nDigimonLevel );

// 		CsC_EffectProp* pEffProp =	pDigimon->GetProp_Effect();
// 		if( pEffProp )
// 			pEffProp->AddLoopEffect( CsC_EffectProp::LE_COSEFFECT, "System\\Character CRoom\\Shadow.nif" );

		UINT nModelID = pDigimon->GetModelID();
		float fScale = nsCsFileTable::g_pDigimonMng->GetDigimon( nModelID )->GetInfo()->s_fSelectScale;
		if( fScale != 0.0f )
			pDigimon->SetScale( fScale );

#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
		// 디지몬 이펙트 추가
		nsDPART::sDPART_INFO partInfo;
		partInfo.s_nFileTableID = nEffectType;
		if ( nsCsFileTable::g_pItemMng->GetDPartsInfo( nEffectType, pDigimon->GetFTID(), partInfo.s_nPartIndex, partInfo.s_File ) )
			pDigimon->ChangeParts(partInfo);
#endif
	}
	return pDigimon;
}

CPat* CharacterSelectContents::sCharacterInfo::CreatePat(uint const& unPatLeader, bool const& bGray)
{
	if(0 == unPatLeader)
		return NULL;

	CsMaster_CardLeader* pCardMaster = nsCsFileTable::g_pMaster_CardMng->GetMasterCardLeader(m_nPat_Leader);
	SAFE_POINTER_RETVAL( pCardMaster, NULL );

	CsMaster_CardLeader::sINFO* pInfo = pCardMaster->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, NULL );

	sCREATEINFO prop;
	prop.s_dwPropPlag = PR_ANI_ENABLE | PR_PATH_NONE | PR_ATTACK_NONE | PR_EFFECT_NONE | PR_MOVECON_NONE;
	prop.s_dwOptionPlag = OP_FOG | OP_ABVWIRE | OP_LIGHT;
	if( bGray )
		prop.s_eInsertGeomType = CGeometry::GetCharOutLineGrayType();
	else
		prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

	CPat* pPat = g_pCharMng->AddPat( m_pTamer, g_pCharMng->PopTempCharID(), pInfo->s_nDigimonID, &prop, NiPoint3( -115, 0, -115 ), -0.5f);	
	SAFE_POINTER_RETVAL( pPat, NULL );

	CsMaster_Card::MAP_IT	it = nsCsFileTable::g_pMaster_CardMng->GetMasterCardMap()->begin();
	CsMaster_Card::MAP_IT	it_end = nsCsFileTable::g_pMaster_CardMng->GetMasterCardMap()->end();
	float fscale = 1.0f;
	for( ;it != it_end; ++it)
	{
		if(it->second->GetInfo()->s_nDigimonID == pInfo->s_nDigimonID)
		{
			fscale = it->second->GetInfo()->s_nScale * 0.01f;
			break;
		}
	}
	
	pPat->SetScale(fscale);

	return pPat;
}

int CharacterSelectContents::sCharacterInfo::GetCharacterImageIdx( uint const& mTbIdx )
{
	switch( mTbIdx )
	{
	case CODE_MASARU:		return 0;	// Marcus Damon
	case CODE_YOSHINO:		return 1;	// Thomas Norstein
	case CODE_TOHMA:		return 2;	// Yoshino Fujieda
	case CODE_IKUTO:		return 3;	// Keenan Krier
	case CODE_TAICHI:		return 4;	// Tai Kamiya
	case CODE_MIMI:			return 5;	// Mimi Tachikawa
	case CODE_MATT:			return 6;	// Matt Ishida
	case CODE_TAKERU:		return 7;	// Takeru Kaishi
	case CODE_HIKARI:		return 8;	// Hikari Kamiya
	case CODE_SORA:			return 9;	// Sora Takenoushi
	case CODE_JANG:			return 10;	// Izzy Izumi
	case CODE_JUNG:			return 11;	// Joe Kido
	case CODE_TAKATO:		return 12;	// Takato Matsuki
	case CODE_RIKA:			return 13;	// Rika Nonaka
	case CODE_HENRY:		return 14;	// Henry Wong
	case CODE_JERI:			return 15;	// Kato Jeri
	case CODE_RYO:			return 16;	// Akiyama Ryo
	case CODE_HIRO:			return 17;	// Hiro Amanokawa
	case CODE_RULI:			return 18;	// Ruli Tsukiyono
	case CODE_KIYOSHIROU:	return 19;	// Kiyoshirou Higashimitarai
	default:				return 0;	// If not configured show Marcus Damon image
	}
}

void CharacterSelectContents::sCharacterInfo::CreateCharacter(sSimpleCharacterInfo const& info)
{
#ifdef DEBUG_NETWORK
	//DBG("Receiving character info for tamer -> %s", info.m_szTamerName);
#endif
	m_nSlotNumber			= info.m_nSlotNumber;
	m_nMapID				= info.m_nMapID;
	if( m_nMapID != 0 )
	{
		CsMapList* pMapInfo = nsCsMapTable::g_pMapListMng->GetList( m_nMapID );
		if( pMapInfo )
		{
			CsMapList::sINFO* pInfo = pMapInfo->GetInfo();
			if( pInfo )
				m_szMapName = pInfo->s_szMapDiscript;
		}
	}

	m_szTamerName			= info.m_szTamerName;
	m_nTamerObjectTBIdx		= info.m_nTamerObjectTBIdx;
	m_nTamerLevel			= info.m_nTamerLevel;

	m_szDigimonName			= info.m_szDigimonName;
	m_nDigimonObjectTBIdx	= info.m_nDigimonObjectTBIdx;
	m_nDigimonLevel			= info.m_nDigimonLevel;
	m_fDigimonScale			= info.m_nDigimonScale;

#if COMMON_LIB_FIXED
	m_nDigimonEffectType	= info.m_nDigimonEffectType;
#else
	m_nDigimonEffectType = 0;
#endif

	m_nPat_Leader			= info.m_nPat_Leader;
	m_nServerChangeWait		= info.m_nRelocateState;

	if( IsServerChangeWaitCharactor() )
	{
		m_pTamerDummy			= Create_Dummy( "Data\\EtcObject\\FigureTamer.nif", NiPoint3( -115, 0, -115 ), -0.5f );
		m_pDigimonDummy			= Create_Dummy( "Data\\EtcObject\\FigureDigimon.nif", NiPoint3( -20, 0, -110 ), -0.1f );
	}
	else
	{
		//bool bCharacterGray		= (m_nServerChangeWait != nTamer::eState::None) ? true : false;
		bool bCharacterGray = false;
		m_pTamer				= CreateTamer( info.m_TamerEquip, m_nTamerObjectTBIdx, bCharacterGray );
		m_pDigimon				= CreateDigimon(m_nDigimonObjectTBIdx, m_nDigimonEffectType, bCharacterGray);
		m_pPat					= CreatePat(m_nPat_Leader, bCharacterGray);
		SetDigimonElement( m_nDigimonObjectTBIdx );
	}
	m_nCharImgIdx			= GetCharacterImageIdx( m_nTamerObjectTBIdx );

}

CsNodeObj* CharacterSelectContents::sCharacterInfo::Create_Dummy( std::string const& dummyFile, NiPoint3 const& nPos, float const& fRotate )
{
	NiStream kStream;
	if( !kStream.Load( dummyFile.c_str() ) )
		return NULL;

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
	CsNodeObj* dummyObject = NiNew CsNodeObj;
	dummyObject->SetNiObject(pNode, CGeometry::Normal);
	pNode->SetTranslate( nPos );
	pNode->SetRotate( fRotate, 0, 0, 1 );
	pNode->Update( 0.0f );

	return dummyObject;
}

void CharacterSelectContents::sCharacterInfo::Update( float const& fAccumTime, float const& fDeltaTime )
{
	if( m_pTamer )
		m_pTamer->CsC_PartObject::Update( fDeltaTime );
	if( m_pDigimon )
		m_pDigimon->CsC_AvObject::Update( fDeltaTime );
	if( m_pPat )
		m_pPat->CsC_AvObject::Update( fDeltaTime );

	if( m_pTamerDummy && m_pTamerDummy->m_pNiNode )
		m_pTamerDummy->m_pNiNode->Update( fAccumTime );		
	if( m_pDigimonDummy && m_pDigimonDummy->m_pNiNode )
		m_pDigimonDummy->m_pNiNode->Update( fAccumTime );
}

void CharacterSelectContents::sCharacterInfo::Render()
{
	if( m_pTamer )		
		m_pTamer->Render();
	if( m_pDigimon )
		m_pDigimon->Render();
	if( m_pPat )
		m_pPat->Render();

	if( m_pTamerDummy )
		m_pTamerDummy->Render();
	if( m_pDigimonDummy )
		m_pDigimonDummy->Render();
}

//////////////////////////////////////////////////////////////////////////

int const CharacterSelectContents::IsContentsIdentity(void)
{
	return E_CT_CHARACTER_SELECT;
}

CharacterSelectContents::CharacterSelectContents(void):m_nSelectCharIdx(-1)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHAR_DELETE_RESULT, this, &CharacterSelectContents::RecvCharDel );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_CHARDEL_PASSWORD_WINDOW, this, &CharacterSelectContents::OpenCharDelWindow );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHAR_SELETECT_LIST, this, &CharacterSelectContents::RecvCharList );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHAR_SELECT_RESULT, this, &CharacterSelectContents::RecvCharSelected );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ADD_NEW_CHARACTER, this, &CharacterSelectContents::RecvNewCharacterInfo);
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_SELETECT_LIST_EMPTY_SLOT_NUMBER, this, &CharacterSelectContents::Get_EmptySlotNumber);	
}

CharacterSelectContents::~CharacterSelectContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);
}

int const CharacterSelectContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CharacterSelectContents::Initialize(void)
{

	return true;
}

void CharacterSelectContents::UnInitialize(void)
{
}

bool CharacterSelectContents::IntraConnection(ContentsSystem& System)
{ 
	return true; 
}

void CharacterSelectContents::Update(float const& fElapsedTime)
{
}

void CharacterSelectContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void CharacterSelectContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}


void CharacterSelectContents::MakeMainActorData(void)
{
}

void CharacterSelectContents::ClearMainActorData(void)
{
	ClearAllRegistered();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int CharacterSelectContents::GetSelectCharIdx() const
{
	return m_nSelectCharIdx;
}

void CharacterSelectContents::RestData()
{
	m_listCharInfo.clear();
	m_nSelectCharIdx = -1;
}

void CharacterSelectContents::gotoBack()
{
	FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);
	//RestData();
}

bool CharacterSelectContents::gotoCharCreate()
{
	if( IsHaveEmptySlot() )
		FLOWMGR_ST.PushFlow( Flow::CFlow::FLW_CHARCREATE );
	else
	{	// 빈 슬롯이 없다
		cPrintMsg::PrintMsg(cPrintMsg::CHARSELECT_MAX_CHAR_COUNT);
		return false;
	}
	
	return true;
}

void CharacterSelectContents::UpdateModel( float const& fAccumTime, float const& fDeltaTime )
{
	sCharacterInfo* pInfo = GetCharacterInfoFromSlot(m_nSelectCharIdx);
	SAFE_POINTER_RET(pInfo);

	pInfo->Update( fAccumTime, fDeltaTime );
}

void CharacterSelectContents::RenderModel()
{
	sCharacterInfo* pInfo = GetCharacterInfoFromSlot(m_nSelectCharIdx);
	SAFE_POINTER_RET(pInfo);
	pInfo->Render();
}

void CharacterSelectContents::OpenCharDelWindow(void* pData)
{
// #ifdef VERSION_STEAM
// 	std::wstring password;
// 	DmCS::StringFn::FromUTF8(password, net::MailAddress);
// 	SendDeleteCharacter(password.c_str());
// #else
	Notify( eOPEN_DELTE_WINDOW );
//#endif
}

bool CharacterSelectContents::IsHaveCharacter( int const& nSlotNumber ) const
{
	sCharacterInfo const* pInfo = GetCharacterInfo(nSlotNumber);
	SAFE_POINTER_RETVAL(pInfo,false);

	return pInfo->IsHaveCharInfo();
}

bool CharacterSelectContents::IsLockSlot( int const& nSlotNumber ) const
{
	sCharacterInfo const* pInfo = GetCharacterInfo(nSlotNumber);
	SAFE_POINTER_RETVAL(pInfo,false);

	return pInfo->IsLockSlot();
}

bool CharacterSelectContents::IsServerChangeWaitCharacter( int const& nSlotNumber ) const
{
	sCharacterInfo const* pInfo = GetCharacterInfo(nSlotNumber);
	SAFE_POINTER_RETVAL(pInfo,true);
	return pInfo->IsServerChangeWaitCharactor();
}

CharacterSelectContents::sCharacterInfo * CharacterSelectContents::GetCharacterInfoFromSlot(int const& nSlotNumber)
{
	LIST_CHARLIST_IT it = m_listCharInfo.begin();
	for( ; it != m_listCharInfo.end(); ++it )
	{
		if( (*it).m_nSlotNumber == nSlotNumber )
			return &(*it);
	}

	return NULL;
}

bool CharacterSelectContents::AddNewCharacterData( sSimpleCharacterInfo const& addCharInfo )
{
	LIST_CHARLIST_IT it = m_listCharInfo.begin();
	for( ; it != m_listCharInfo.end(); ++it )
	{
		if( (*it).m_nSlotNumber == addCharInfo.m_nSlotNumber )
		{
			(*it).CreateCharacter(addCharInfo);
			return true;
		}
	}

	return false;
}

bool CharacterSelectContents::IsHaveCharacter() const
{
	LIST_CHARLIST_CIT it = m_listCharInfo.begin();
	for( ; it != m_listCharInfo.end(); ++it )
	{
		if( (*it).IsHaveCharInfo() )
			return true;
	}
	return false;
}

bool CharacterSelectContents::IsHaveEmptySlot() const
{
	LIST_CHARLIST_CIT it = m_listCharInfo.begin();
	for( ; it != m_listCharInfo.end(); ++it )
	{
		if( !(*it).IsHaveCharInfo() && !(*it).IsLockSlot() )
			return true;
	}

	return false;
}

bool CharacterSelectContents::DeleteCharacter( int const& nDelSlotNumber )
{
	LIST_CHARLIST_IT it = m_listCharInfo.begin();
	for( ; it != m_listCharInfo.end(); ++it )
	{
		if( (*it).m_nSlotNumber != nDelSlotNumber )
			continue;

		// 폴더에 있는 캐릭터 정보 삭제
		if( g_pResist )
		{
			g_pResist->CharDelete( (*it).m_szTamerName.c_str() );

			if( g_pResist->m_AccountServer.s_nLastSelChar == nDelSlotNumber )
				g_pResist->m_AccountServer.s_nLastSelChar = -1;
		}

		(*it).ResetData();

		cPrintMsg::PrintMsg(cPrintMsg::CHARSELECT_DELETE_COMP);

		ContentsStream kStream;
		kStream << nDelSlotNumber;
		Notify( eCHAR_DELETE_SUCCESS, kStream );

		if( m_nSelectCharIdx == nDelSlotNumber )
			SetChangeCharacter(-1);
		return true;
	}
	return false;
}


CharacterSelectContents::sCharacterInfo const* CharacterSelectContents::GetCharacterInfo(int const& nSlotNumber) const
{
	LIST_CHARLIST_CIT it = m_listCharInfo.begin();
	for( ; it != m_listCharInfo.end(); ++it )
	{
		if( (*it).m_nSlotNumber == nSlotNumber )
			return &(*it);
	}

	return NULL;
}

CharacterSelectContents::LIST_CHARLIST const& CharacterSelectContents::GetCharacterList() const
{
	return m_listCharInfo;
}

void CharacterSelectContents::makeEmptyCharSlotInfo( int const& nOpenSlotCount, int const& nLockSlotCount )
{
	m_listCharInfo.clear();
	for( int n =0; n < nOpenSlotCount + nLockSlotCount; ++n )
	{
		if( n < nOpenSlotCount )
			m_listCharInfo.push_back( sCharacterInfo(n,false) );
		else
			m_listCharInfo.push_back( sCharacterInfo(n,true) );
	}
}

void CharacterSelectContents::SetChangeCharacter( int const& nSlotNumber )
{
	if( m_nSelectCharIdx != nSlotNumber )
		m_nSelectCharIdx = nSlotNumber;

	ContentsStream kStream;
	kStream << m_nSelectCharIdx;
	Notify( eCHANGE_SELECT_CHAR, kStream );
}

//////////////////////////////////////////////////////////////////////////
// Sender Function
//////////////////////////////////////////////////////////////////////////
bool CharacterSelectContents::SendGameStart()
{
	// 서버 변경 대기 상태인 캐릭터인지 체크
	if( IsServerChangeWaitCharacter( m_nSelectCharIdx ) )
	{
		cPrintMsg::PrintMsg( 10117 );
		return false;
	}

	if( IsLockSlot( m_nSelectCharIdx ) )
	{	// 잠긴 슬롯
		cPrintMsg::PrintMsg( 10033 );
		return false;
	}

	if( IsHaveCharacter( m_nSelectCharIdx ) )
	{
		cPrintMsg::PrintMsg( 10019 );
		if( net::gate )
			net::gate->SendSelect( m_nSelectCharIdx );
	}
	else
		gotoCharCreate();

	return true;
}

bool CharacterSelectContents::SendDeleteCharacter(TCHAR const* pPassword )
{
	SAFE_POINTER_RETVAL( pPassword, false );

	// 서버 변경 대기 상태인 캐릭터인지 체크
	if( IsServerChangeWaitCharacter( m_nSelectCharIdx ) )
	{
		cPrintMsg::PrintMsg( 10116 );
		return false;
	}

	if( !IsHaveCharacter( m_nSelectCharIdx ) )
		return false;

	// 잠긴 슬롯
	if( IsLockSlot( m_nSelectCharIdx ) )
		return false;

	if( net::gate )
		net::gate->SendRemove( m_nSelectCharIdx, pPassword );

	return true;
}

int CharacterSelectContents::GetEmptySlotNumber() const
{
	LIST_CHARLIST_CIT it = m_listCharInfo.begin();
	for( ; it != m_listCharInfo.end(); ++it )
	{
		if( !(*it).IsHaveCharInfo() )
			return (*it).m_nSlotNumber;
	}

	return -1;
}

bool CharacterSelectContents::IsPcbangConnect() const
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_PCBang* pPcBang = g_pDataMng->GetPCbang();
	SAFE_POINTER_RETVAL( pPcBang, false );
	return pPcBang->GetPcBang();
}

int CharacterSelectContents::GetMaxSlotCount() const
{
	return static_cast<int>(m_listCharInfo.size());
}
/////////////////////////////////////////////////////////////////////////
// Recver Function
//////////////////////////////////////////////////////////////////////////

void CharacterSelectContents::Get_EmptySlotNumber(void* pData)
{
	SAFE_POINTER_RET( pData);
	int* nEmptySlot = static_cast<int*>(pData);
	*nEmptySlot = GetEmptySlotNumber();
}

// 게임 시작 패킷
void CharacterSelectContents::RecvNewCharacterInfo(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_NEW_CHARACTER_INFO* pRecv = static_cast<GS2C_RECV_NEW_CHARACTER_INFO*>(pData);

	if( !AddNewCharacterData( pRecv->m_NewCharInfo ) )
		return;

	m_nSelectCharIdx = pRecv->m_NewCharInfo.m_nSlotNumber;
	ContentsStream kStream;
	kStream << m_nSelectCharIdx;
	Notify( eADD_NEW_CHARACTER, kStream );
}

void CharacterSelectContents::RecvCharSelected(void* pData)
{
	cMessageBox::DelMsg( 10019, false );

	// 레지트리에서 케릭터 선택
	sCharacterInfo * pStartCharInfo = GetCharacterInfoFromSlot( m_nSelectCharIdx );

	SAFE_POINTER_RET( pStartCharInfo );

	if( g_pResist )
		g_pResist->m_AccountServer.s_nLastSelChar = m_nSelectCharIdx;

	g_pResist->LoadChar( pStartCharInfo->m_szTamerName.c_str() );

	g_pGameEventMng->Delete();
	g_pGameEventMng->Init();
	
#ifndef TUTORIAL_DISABLE
	g_bFirstConnetChar = ( pStartCharInfo->m_nMapID == 0 );
#endif

	
	GLOBALDATA_ST.SetConnetServerInfo( g_pResist->m_AccountGlobal.s_nLastConnetServer, 
									   g_pResist->m_AccountGlobal.s_szServerName );

	FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_LOADING);

	if(OPTIONMNG_PTR)
		OPTIONMNG_PTR->LoadCharOption(OPTIONFILE_NAME);

	//RestData();

	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::LOADING_START );
}

void CharacterSelectContents::RecvCharList(void* pData)
{
	SAFE_POINTER_RET(pData);

	cMessageBox::DelMsg( 10019, false );

	RestData();

	m_nSelectCharIdx = g_pResist->m_AccountServer.s_nLastSelChar;
#ifdef DEBUG_NETWORK
	DBG("Last character index -> %d", m_nSelectCharIdx);
#endif

	int nOpenSlotCount;
	int nLockSlotCount;
	GLOBALDATA_ST.GetServerSlotCountInfo( g_pResist->m_AccountGlobal.s_nLastConnetServer, nOpenSlotCount, nLockSlotCount );
	makeEmptyCharSlotInfo( nOpenSlotCount, nLockSlotCount );	

	GS2C_RECV_CHARACTER_LIST* pRecv = static_cast<GS2C_RECV_CHARACTER_LIST*>(pData);
	std::list<sSimpleCharacterInfo>::const_iterator it = pRecv->m_listCharacterInfo.begin();
	for( ; it != pRecv->m_listCharacterInfo.end(); ++it )
		AddNewCharacterData( *it );

	if( !IsHaveCharacter() )
		FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_CHARCREATE);
	else
	{
		if( g_pResist->m_AccountServer.s_nLastSelChar == -1 )
			m_nSelectCharIdx = 0;
		FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_CHARSEL);
	}
}

void CharacterSelectContents::RecvCharDel( void* pData )
{
	SAFE_POINTER_RET(pData);

	std::pair<int, std::string> nRes = *static_cast<std::pair<int, std::string>*>(pData);

	switch(nRes.first)
	{
	case 0 :	// 주민번호 오류, 한국 버전
		{
			cPrintMsg::PrintMsg( 10043 );
		}
		break;
	case 1 :	// 제거 성공
		{
			DeleteCharacter( m_nSelectCharIdx );
		}
		break;
	case 2 :	// 이메일 오류, 미쿡 버전
		{
			if( GLOBALDATA_ST.Is2ndPassType( GData::e2ndNumberPass ) )
				cPrintMsg::PrintMsg( 10043 );
			else if( GLOBALDATA_ST.Is2ndPassType( GData::eAccountPass ) )
				cPrintMsg::PrintMsg( 10044 );
			else if( GLOBALDATA_ST.Is2ndPassType( GData::eEmail ) )
				cPrintMsg::PrintMsg( 10045 );
			else if( GLOBALDATA_ST.Is2ndPassType( GData::eStringPass ) )
				cPrintMsg::PrintMsg( 10046 );
		}		
		break;
	case 3:// 캐릭터 생성 후 일주일안에 삭제할 수 없습니다.
		{
// 			if( GLOBALDATA_ST.IsCountry( GData::eCountry_Taiwan ) )
// 			{
				if( 0 == nRes.second.size() )
					break;

				DatePacing( nRes.second.c_str() );
				break;
	//		}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CharacterSelectContents::DatePacing( char const * szDate )
{
	char szTemp[3][32]  = {0, };
	char szRealDate[32] = {0, };
	char szTime[32]		= {0, };
	char* pTime = NULL;

	// 시간 구하기
	pTime = strstr( const_cast<char*>(szDate), " " );
	strcpy( szTime, &pTime[1] );
	int nIndex = 0;

	// 날짜 구하기
	for( int i=0, j=0; i<32; ++i )
	{
		if( szDate[i] != '-' )
		{
			if( szDate[i] != ' ' )
			{
				++j;
				continue;
			}
		}

		if( *szTemp[0] == 0 )
		{
			strncpy( szTemp[ nIndex++ ], szDate, j );
			j = 0;
		}
		else
		{
			strncpy( szTemp[ nIndex++ ], &szDate[ i-j ], j );
			j = 0;
		}
	}

	// 문자 -> 숫자로
	int nYear = atoi( szTemp[0] );
	int nMonth = atoi( szTemp[1] );
	int nDay = atoi( szTemp[2] );

	// 일주일 후 삭제하기 때문에 날짜 계산 시 7일을 더한다.
	Get_Date( nYear, nMonth, nDay, szTime, szRealDate );

	TCHAR szRealDate2[32] = {0, };
	M2W( szRealDate2, szRealDate, 32 );
	cPrintMsg::PrintMsg( 10049, szRealDate2 );	// 캐릭터 생성 후 일주일안에 삭제할 수 없습니다.
}

void CharacterSelectContents::Get_Date( int nYear, int nMonth, int nDay, char* pTime, char* szDate )
{
	bool bFourYear = false;	// 4년 주기 체크

	if( nYear % 4 == 0 )
		bFourYear = true; // 2월이 29일까지 있음

	int nRealDay, nRealMonth, nRealYear;
	bool bMonthAdd = false;

	switch( nMonth )
	{
	case 2:	// 2월
		if( bFourYear )	// 29일까지 있다면(4년에 한번)
		{
			if( nDay + 7 > 29 )	// 2월을 넘었다면
			{
				nRealDay = (nDay + 7) - 29;
				bMonthAdd = true;	// 월 증가해야 함
			}
			else
				nRealDay = nDay + 7;
		}
		else
		{
			if( nDay + 7 > 28 )	// 2월을 넘었다면
			{
				nRealDay = (nDay + 7) - 28;
				bMonthAdd = true;	// 월 증가해야 함
			}
			else
				nRealDay = nDay + 7;
		}
		break;
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		if( nDay + 7 > 31 )	// 월을 넘었다면
		{
			nRealDay = (nDay + 7) - 31;
			bMonthAdd = true;	// 월 증가해야 함
		}
		else
			nRealDay = nDay + 7;
		break;

	case 4:
	case 6:
	case 9:
	case 11:
		if( nDay + 7 > 30 )	// 월을 넘었다면
		{
			nRealDay = (nDay + 7) - 30;
			bMonthAdd = true;	// 월 증가해야 함
		}
		else
			nRealDay = nDay + 7;
		break;

	default:
		assert_cs( false );
	}

	if( bMonthAdd )	// 월 증가
	{
		if( nMonth == 12 )	// 현재가 12월이라면
		{
			nRealMonth = 1;
			nRealYear = ++nYear;
		}
		else
		{
			nRealMonth = ++nMonth;
			nRealYear = nYear;
		}
	}
	else
	{
		nRealMonth = nMonth;
		nRealYear = nYear;
	}

	sprintf_s( szDate, 32, "%d/%d/%d %s", nRealYear, nRealMonth, nRealDay, pTime );
}

