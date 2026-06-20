#include "StdAfx.h"
#include "AdaptPartySystem.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

AdaptPartySystem::AdaptPartySystem(void):m_pContents(NULL)
{
}

AdaptPartySystem::~AdaptPartySystem(void)
{
}

bool AdaptPartySystem::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, NULL );
	if( E_CT_PARTYSYSTEM != pContents->GetContentsIdentity() )
		return false;

	m_pContents = static_cast< CPartyContents* >( pContents );

	return true;
}

bool AdaptPartySystem::IsJoinParty() const
{
	SAFE_POINTER_RETVAL( m_pContents, false );
	return m_pContents->IsJoinParty();
}

bool AdaptPartySystem::IsSendInvite() const
{
	SAFE_POINTER_RETVAL( m_pContents, false );
	return m_pContents->IsSendInvite();
}

bool AdaptPartySystem::IsMePartyMaster() const
{
	SAFE_POINTER_RETVAL( m_pContents, false );
	return m_pContents->IsMePartyMaster();
}

bool AdaptPartySystem::IsPartyMaster(int const& nSlot) const
{
	SAFE_POINTER_RETVAL( m_pContents, false );
	return m_pContents->IsPartyMaster( nSlot );
}

bool AdaptPartySystem::IsMemberDigimon( uint const& digimonUIDX ) const
{
	SAFE_POINTER_RETVAL( m_pContents, false );
	return m_pContents->IsPartyMemberFromDigimonUIDX( digimonUIDX );
}

bool AdaptPartySystem::IsMemberTamer( uint const& tamerUIDX ) const
{
	SAFE_POINTER_RETVAL( m_pContents, false );
	return m_pContents->IsPartyMemberFromTamerUIDX( tamerUIDX );
}

SMemberInfo const*	AdaptPartySystem::GetMemberInfo( int const& nSlot) const
{
	SAFE_POINTER_RETVAL( m_pContents, NULL );
	return m_pContents->GetMemberInfoFromSlotNo( nSlot );
}

SPartyInfo const* AdaptPartySystem::GetPartyInfo() const
{
	SAFE_POINTER_RETVAL( m_pContents, NULL );
	return m_pContents->GetPartyInfo();
}

