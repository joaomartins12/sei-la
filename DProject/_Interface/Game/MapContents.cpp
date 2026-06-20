#include "StdAfx.h"

#include "MapContents.h"


CMapContents::CMapContents(void) : m_bRegionShow(true),m_AdaptParty(0)
{
	EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_PartyMember_Digimon_Change, this );
	EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_PartyMember_Info_Change, this );
	EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_PartyMember_Map_Change, this );
}

CMapContents::~CMapContents(void)
{
	m_AdaptParty = 0;
	GAME_EVENT_ST.DeleteEventAll( this );
	EventRouter()->UnRegisterAll( this );
}
int const CMapContents::IsContentsIdentity(void)
{
	return E_CT_MAP_INTERFACE;
}
int const CMapContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CMapContents::Initialize(void)
{
	return true;
}

void CMapContents::UnInitialize(void)
{
}

bool CMapContents::IntraConnection(ContentsSystem& System)
{
	m_AdaptParty = new AdaptPartySystem;
	if( m_AdaptParty )
		m_AdaptParty->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_PARTYSYSTEM ) );
	return true;
}

void CMapContents::Update(float const& fElapsedTime)
{
}

void CMapContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void CMapContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

void CMapContents::MakeMainActorData(void)
{
}

void CMapContents::ClearMainActorData(void)
{
}

void CMapContents::ClearWorldData(void)
{
	InitContentsData();
}

void CMapContents::MakeWorldData(void)
{
	InitContentsData();
}

void CMapContents::InitContentsData(void)
{
}

void CMapContents::SetRegionShow(bool	show)
{
	m_bRegionShow = show;
}

bool CMapContents::IsRegionShow() const
{
	return m_bRegionShow;
}

bool CMapContents::IsJoinParty() const
{
	SAFE_POINTER_RETVAL( m_AdaptParty, false );
	return m_AdaptParty->IsJoinParty();
}

SPartyInfo const* CMapContents::GetPartyInfo() const
{
	SAFE_POINTER_RETVAL( m_AdaptParty, NULL );
	return m_AdaptParty->GetPartyInfo();
}