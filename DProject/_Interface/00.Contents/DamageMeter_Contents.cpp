#include "StdAfx.h"
#include "DamageMeter_Contents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

DamageMeterContents::sRankBase::sRankBase():m_nValue(0),m_bIsMe(false), m_bFaraway(false)
{
}

DamageMeterContents::sRankBase::~sRankBase()
{
}

void DamageMeterContents::sRankBase::SetName( std::wstring name )
{
	m_wsName = name;
}

bool DamageMeterContents::sRankBase::Update(float const& fDeltime) 
{
	return false;
};

std::wstring DamageMeterContents::sRankBase::GetName() const 
{ 
	return m_wsName; 
}

void DamageMeterContents::sRankBase::SetIsMe( bool bValue )
{
	m_bIsMe = bValue;
}

void DamageMeterContents::sRankBase::SetFaraway( bool bValue )
{
	m_bFaraway = bValue;
}

bool DamageMeterContents::sRankBase::IsMe() const
{
	return m_bIsMe;
}

bool DamageMeterContents::sRankBase::IsFaraway() const
{
	return m_bFaraway;
}
//////////////////////////////////////////////////////////////////////////

DamageMeterContents::sRankData::sRankData()
{
}

DamageMeterContents::sRankData::~sRankData()
{
}

__int64 DamageMeterContents::sRankData::GetValue() const 
{ 
	return m_nValue; 
}

void DamageMeterContents::sRankData::AddValue( uint const& nValue ) 
{ 
	m_nValue += nValue; 
}

//////////////////////////////////////////////////////////////////////////

DamageMeterContents::sDPSRankData::sDPSRankData()
:m_bBattleOn(false), m_fBattleTime(0.0f), m_nBattleOnDPS(0),m_nBattleOnDamage(0)
{
}

DamageMeterContents::sDPSRankData::~sDPSRankData()
{
}


bool DamageMeterContents::sDPSRankData::Update(float const& fDeltime)
{
	if( !m_bBattleOn )
		return false;

	m_fBattleTime += fDeltime;
	m_nBattleOnDPS = (__int64)(m_nBattleOnDamage / m_fBattleTime);	
	return true;
}

void DamageMeterContents::sDPSRankData::AddValue( uint const& nValue )
{ 
	if( !m_bBattleOn )
		SetDPSOn();

	m_nBattleOnDamage += nValue; 
}

__int64 DamageMeterContents::sDPSRankData::GetValue() const
{
	if( m_nValue == 0 )
		return m_nBattleOnDPS;

	if( m_nBattleOnDPS == 0 )
		return m_nValue;

	return (m_nValue + m_nBattleOnDPS) >> 1;
}

void DamageMeterContents::sDPSRankData::SetDPSOn()
{
	if( m_bBattleOn )
		return;

	m_bBattleOn = true;
	m_fBattleTime = 0.0f;
	m_nBattleOnDamage = 0;
}

void DamageMeterContents::sDPSRankData::SetDPSOff()
{
	if( !m_bBattleOn )
		return;

	m_bBattleOn = false;

	m_nBattleOnDPS = (__int64)(m_nBattleOnDamage / m_fBattleTime);	

	if( m_nBattleOnDPS > 0 )
	{
		if( m_nValue > 0 )
		{
			m_nValue += m_nBattleOnDPS;
			m_nValue = m_nValue >> 1;
		}
		else
			m_nValue += m_nBattleOnDPS;
	}

	m_fBattleTime = 0.0f;
	m_nBattleOnDPS = 0;
	m_nBattleOnDamage = 0;
}

//////////////////////////////////////////////////////////////////////////
int const DamageMeterContents::IsContentsIdentity(void)
{
	return E_CT_DAMAGE_METER;
}

DamageMeterContents::DamageMeterContents(void):m_pAdaptParty(0),m_nCurrentViewType(eDISP_TYPE::DAMAGE_DONE),m_fUpdateTime(0.0f),m_bOpenedMeter(false)
{	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::TOGGLE_DAMAGEMETER_WINDOW, this, &DamageMeterContents::toggleWindow );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::ADD_DAMAGEMETER_DATA, this, &DamageMeterContents::Add_DamageMeterData );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::DIGIMON_BATTLE_ON, this, &DamageMeterContents::DigimonBattleOn );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::DIGIMON_BATTLE_OFF, this, &DamageMeterContents::DigimonBattleOff );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::LOADING_START, this, &DamageMeterContents::DPSAllStop);
	
	EventRouter()->Register( CONTENTS_EVENT::EEvt_Party_Leave, this );
	EventRouter()->Register( CONTENTS_EVENT::EEvt_Party_Banish, this );
	EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_PartyMember_Leave, this );
	EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_PartyMember_Banish, this );
	EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_PartyMember_Faraway, this );
}

DamageMeterContents::~DamageMeterContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);
	EventRouter()->UnRegisterAll( this );
}

int const DamageMeterContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool DamageMeterContents::Initialize(void)
{
	return true;
}

void DamageMeterContents::UnInitialize(void)
{
}

bool DamageMeterContents::IntraConnection(ContentsSystem& System)
{
	m_pAdaptParty = new AdaptPartySystem;
	if( m_pAdaptParty )
		m_pAdaptParty->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_PARTYSYSTEM ) );

	return true;
}

void DamageMeterContents::Update(float const& fElapsedTime)
{
	m_fUpdateTime += fElapsedTime;

	bool bUpdate = false;
	LIST_RANKDATA_IT it = m_DamageDPSRank.begin();
	for( ; it != m_DamageDPSRank.end(); ++it )
	{
		if( (*it)->Update( fElapsedTime ) )
			bUpdate = true;
	}

	if( m_fUpdateTime < 1.0f )
		return;

	if( bUpdate )
		_NotifyUIUpdate( DAMAGE_DPS );
	
	while( m_fUpdateTime > 1.0f )
		m_fUpdateTime -= 1.0f;
}

void DamageMeterContents::NotifyEvent(int const& iNotifiedEvt)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EEvt_Party_Leave:
	case CONTENTS_EVENT::EEvt_Party_Banish:	// 내가 파티를 나갔을 때 파티원이였던 사람들의 DPS가 동작중이면 중지 시킨다.
		_StopDPSOther();
		break;
	}
}

void DamageMeterContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EStreamEvt_PartyMember_Leave:
	case CONTENTS_EVENT::EStreamEvt_PartyMember_Banish:	// 파티원 나갔을 때 나간 사람의 DPS가 동작중이면 중지 시킨다.
		{
			kStream.SetRp(0);
			std::wstring name;
			kStream >> name;
			_StopDPSFormName( name );
		}
		break;
	case CONTENTS_EVENT::EStreamEvt_PartyMember_Faraway:
		{
			kStream.SetRp(0);
			std::wstring name;
			bool bFaraway;
			kStream >> name >> bFaraway;
			if( bFaraway )
				_StopDPSFormName( name );
		}
		break;
	}
}

void DamageMeterContents::MakeMainActorData(void)
{
}

void DamageMeterContents::ClearMainActorData(void)
{
}

void DamageMeterContents::ClearWorldData(void)
{
	_ResetData();
}

void DamageMeterContents::MakeWorldData(void)
{
}

void DamageMeterContents::_ResetData()
{
	_ClearListData( m_DamageDoneRank );
	_ClearListData( m_DamageDPSRank );
	_ClearListData( m_DamageTakenRank );
	m_fUpdateTime = 0.0f;
	m_nCurrentViewType = DAMAGE_DONE;	
	m_bOpenedMeter = false;
}

//////////////////////////////////////////////////////////////////////////
void DamageMeterContents::toggleWindow(void* pData)
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DAMAGE_METER ) )
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DAMAGE_METER );
		m_bOpenedMeter = false;
		_AllStopDPS();
	}
	else
	{
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_DAMAGE_METER );
		m_bOpenedMeter = true;
	}
}

void DamageMeterContents::_ClearListData( LIST_RANKDATA& delList )
{
	LIST_RANKDATA_IT it = delList.begin();
	for( ; it != delList.end(); ++it )
		SAFE_DELETE((*it));
	delList.clear();
}

void DamageMeterContents::ClearDamageMeterData()
{
	_ClearListData( m_DamageDoneRank );
	_ClearListData( m_DamageDPSRank );
	_ClearListData( m_DamageTakenRank );
	Notify( eClear_Damage_Data );
}

int DamageMeterContents::GetCurrentViewType() const
{
	return m_nCurrentViewType;
}

void DamageMeterContents::_StopDPSOther()
{
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTamerUser );

	std::wstring wsMyName = pTamerUser->GetName();
	LIST_RANKDATA_IT it = m_DamageDPSRank.begin();
	for( ; it != m_DamageDPSRank.end(); ++it )
	{
		SAFE_POINTER_RET( (*it) );
		sDPSRankData* pData = static_cast<sDPSRankData*>((*it));
		if( pData->GetName().compare( wsMyName ) != 0 )
			pData->SetDPSOff();
	}

	_NotifyUIUpdate( DAMAGE_DPS );
}

void DamageMeterContents::_AllStopDPS()
{
	LIST_RANKDATA_IT it = m_DamageDPSRank.begin();
	for( ; it != m_DamageDPSRank.end(); ++it )
	{
		SAFE_POINTER_RET( (*it) );
		sDPSRankData* pData = static_cast<sDPSRankData*>((*it));
		pData->SetDPSOff();
	}

	_NotifyUIUpdate( DAMAGE_DPS );
}

void DamageMeterContents::_StopDPSFormName(std::wstring const& name)
{
	sRankBase* pDps = _FindRankData( m_DamageDPSRank, name );
	SAFE_POINTER_RET( pDps );
	static_cast<sDPSRankData*>(pDps)->SetDPSOff();

	_NotifyUIUpdate( DAMAGE_DPS );
}

DamageMeterContents::LIST_RANKDATA const&	DamageMeterContents::GetViewData(int const& nViewType) const
{
	switch(nViewType)
	{
	case DAMAGE_DONE:	return m_DamageDoneRank;
	case DAMAGE_DPS:	return m_DamageDPSRank;
	case DAMAGE_TAKEN:	return m_DamageTakenRank;
	default:
		return m_DamageDoneRank;
	}	
}

void DamageMeterContents::ChangePrevViewType()
{
	switch(m_nCurrentViewType)
	{
	case DAMAGE_DONE:	m_nCurrentViewType = DAMAGE_TAKEN;	break;
	case DAMAGE_DPS:	m_nCurrentViewType = DAMAGE_DONE;	break;
	case DAMAGE_TAKEN:	m_nCurrentViewType = DAMAGE_DPS;	break;
	}

	_RankDataSort(m_nCurrentViewType);

	ContentsStream ktmp;
	ktmp << m_nCurrentViewType;
	Notify( eChange_ViewType, ktmp );
}

void DamageMeterContents::ChangeNextViewType()
{
	switch(m_nCurrentViewType)
	{
	case DAMAGE_DONE:	m_nCurrentViewType = DAMAGE_DPS;	break;
	case DAMAGE_DPS:	m_nCurrentViewType = DAMAGE_TAKEN;	break;
	case DAMAGE_TAKEN:	m_nCurrentViewType = DAMAGE_DONE;	break;
	}

	_RankDataSort(m_nCurrentViewType);

	ContentsStream ktmp;
	ktmp << m_nCurrentViewType;
	Notify( eChange_ViewType, ktmp );
}

void DamageMeterContents::_RankDataSort(int const& nViewType)
{
	switch( nViewType )
	{
	case DAMAGE_DONE:	_SortRankData(m_DamageDoneRank);	break;
	case DAMAGE_DPS:	_SortRankData(m_DamageDPSRank);		break;
	case DAMAGE_TAKEN:	_SortRankData(m_DamageTakenRank);	break;
	}	
}

void DamageMeterContents::_NotifyUIUpdate(int const& nUpdateType)
{
	if( m_nCurrentViewType != nUpdateType )
		return;

	_RankDataSort(m_nCurrentViewType);

	ContentsStream ktmp;
	ktmp << m_nCurrentViewType;
	Notify( eUpdate_View_Type, ktmp );
}

DamageMeterContents::sRankBase* DamageMeterContents::_FindRankData( LIST_RANKDATA& findList, std::wstring const& name )
{
	LIST_RANKDATA_IT it = findList.begin();
	for( ; it != findList.end(); ++it )
	{
		if( (*it)->GetName().compare(name) == 0 )
			return (*it);
	}

	return NULL;
}

void DamageMeterContents::_SortRankData(LIST_RANKDATA & listData)
{
	if( listData.size() < 2 ) // 2개 이상
		return;

	struct ConditionPred
	{
		bool operator()(sRankBase const* plhs, sRankBase const* prhs)
		{
			if( plhs->GetValue() < prhs->GetValue() )
				return false;

			return true;
		}
	} Pred;

	listData.sort(Pred);
}

void DamageMeterContents::_AddDoneDamage( std::wstring const& name, uint const& damage, bool bIsMe )
{
	sRankBase* pRankData = _FindRankData( m_DamageDoneRank, name );
	if( NULL == pRankData )
	{
		pRankData = _AddRankData<sRankData>( m_DamageDoneRank, name );
		pRankData->SetIsMe( bIsMe );
	}
		
	pRankData->AddValue( damage );
	
	_NotifyUIUpdate( DAMAGE_DONE );

	sRankBase* pDPSRankData = _FindRankData( m_DamageDPSRank, name );
	if( NULL == pDPSRankData )
	{
		pDPSRankData = _AddRankData<sDPSRankData>( m_DamageDPSRank, name );
		pDPSRankData->SetIsMe( bIsMe );
	}

	pDPSRankData->AddValue( damage );	
}

void DamageMeterContents::_AddTakenDamage( std::wstring const& name, uint const& damage, bool bIsMe )
{
	sRankBase* pRankData = _FindRankData( m_DamageTakenRank, name );
	if( NULL == pRankData )
	{
		pRankData = _AddRankData<sRankData>( m_DamageTakenRank, name );
		pRankData->SetIsMe(bIsMe);
	}

	pRankData->AddValue( damage );

	_NotifyUIUpdate( DAMAGE_TAKEN );
}

void DamageMeterContents::_SetBattleOn( std::wstring const& name )
{
	sRankBase* pDPSRankData = _FindRankData( m_DamageDPSRank, name );
	if( pDPSRankData )
		static_cast<sDPSRankData*>(pDPSRankData)->SetDPSOn();
}

void DamageMeterContents::_SetBattleOff( std::wstring const& name )
{
	sRankBase* pDPSRankData = _FindRankData( m_DamageDPSRank, name );
	if( pDPSRankData )
	{
		static_cast<sDPSRankData*>(pDPSRankData)->SetDPSOff();
		_NotifyUIUpdate( DAMAGE_DPS );
	}
}

void DamageMeterContents::CloseMeterWindow()
{
	m_bOpenedMeter = false;
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DAMAGE_METER ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DAMAGE_METER );
}

// 내가 맵 이동시 모든 DPS 업데이트를 꺼줘야 한다.
void DamageMeterContents::DPSAllStop(void* pData)
{
	_AllStopDPS();
}

// 전투가 시작된 경우 DPS 업데이트를 켜줘야 한다.
void DamageMeterContents::DigimonBattleOn(void* pData)
{
	SAFE_POINTER_RET( pData );
	if( !m_bOpenedMeter )
		return;

	CDigimon* pObject = static_cast<CDigimon*>(pData);
	UINT digimonUIDX = pObject->GetUniqID();
	
	if( !g_pCharMng->IsDigimonUserFromUIDX( digimonUIDX ) && 
		!m_pAdaptParty->IsMemberDigimon( digimonUIDX ) )
		return;

	CTamer* pTamer = g_pCharMng->GetTamer( pObject->GetTamerLink() );
	SAFE_POINTER_RET( pTamer );

	_SetBattleOn( pTamer->GetName() );
}

// 전투가 끝난 경우 DPS 업데이트를 꺼줘야 한다.
void DamageMeterContents::DigimonBattleOff(void* pData)
{
	SAFE_POINTER_RET( pData );
	if( !m_bOpenedMeter )
		return;

	CDigimon* pObject = static_cast<CDigimon*>(pData);
	UINT digimonUIDX = pObject->GetUniqID();

	if( !g_pCharMng->IsDigimonUserFromUIDX( digimonUIDX ) && 
		!m_pAdaptParty->IsMemberDigimon( digimonUIDX ) )
		return;

	CTamer* pTamer = g_pCharMng->GetTamer( pObject->GetTamerLink() );
	SAFE_POINTER_RET( pTamer );

	_SetBattleOff( pTamer->GetName() );
}

void DamageMeterContents::Add_DamageMeterData(void* pData)
{
	SAFE_POINTER_RET( pData );
	sDamageMeterInfo pInfo = *static_cast<sDamageMeterInfo*>(pData);

	SAFE_POINTER_RET( m_pAdaptParty );
	if( !m_bOpenedMeter )
		return;

	CsC_AvObject* pHitter = g_pMngCollector->GetObject( pInfo.m_nHitterUidx );
	SAFE_POINTER_RET( pHitter );

	CsC_AvObject* pTarget = g_pMngCollector->GetObject( pInfo.m_nTargetUidx );
	SAFE_POINTER_RET( pTarget );

	bool bDoneDamage = false;
	switch( pHitter->GetLeafRTTI() )
	{
	case RTTI_TAMER:
	case RTTI_TAMER_USER:
		bDoneDamage = true;
		break;
	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
		{
			// 테이머스킬( 강타 ) 로 인한 데미지로 들어올 수 있음
			pHitter = g_pCharMng->GetTamer( ((CDigimon*)pHitter)->GetTamerLink() );
			SAFE_POINTER_RET( pHitter );
			pInfo.m_nHitterUidx = pHitter->GetUniqID();
			bDoneDamage = true;
		}
		break;
	default:
		{
			switch( pTarget->GetLeafRTTI() )
			{
			case RTTI_DIGIMON:
			case RTTI_DIGIMON_USER:
				{
					// 테이머스킬( 강타 ) 로 인한 데미지로 들어올 수 있음
					pTarget = g_pCharMng->GetTamer( ((CDigimon*)pTarget)->GetTamerLink() );
					SAFE_POINTER_RET( pTarget );
					pInfo.m_nTargetUidx = pTarget->GetUniqID();
				}break;
			case RTTI_TAMER:
			case RTTI_TAMER_USER:
				break;
			default:
				return;
			}
		}
		break;
	}

	if( bDoneDamage )// 데미지를 입혔을 경우
	{
		bool bIsMe = g_pCharMng->IsTamerUserFromUIDX( pInfo.m_nHitterUidx );
		bool bMember = m_pAdaptParty->IsMemberTamer( pInfo.m_nHitterUidx );
		if( !bIsMe && !bMember )
			return;

		_AddDoneDamage( pHitter->GetName(), abs(pInfo.m_nDamage), bIsMe );
	}
	else	// 데미지를 입었을 경우
	{

		bool bIsMe = g_pCharMng->IsTamerUserFromUIDX( pInfo.m_nTargetUidx );
		bool bMember = m_pAdaptParty->IsMemberTamer( pInfo.m_nTargetUidx );
		if( !bIsMe && !bMember )
			return;

		_AddTakenDamage( pTarget->GetName(), abs(pInfo.m_nDamage), bIsMe );
	}
}