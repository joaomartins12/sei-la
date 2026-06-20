#pragma once

class IUIContentBase;

#include "../Game/Party_Contents.h"

class AdaptPartySystem
{
public:
	explicit AdaptPartySystem(void);
	virtual ~AdaptPartySystem(void);

	bool			SetContents(IUIContentBase* pContents);  
	bool			IsJoinParty() const;
	bool			IsSendInvite() const;
	bool			IsMePartyMaster() const;
	bool			IsPartyMaster(int const& nSlot) const;
	bool			IsMemberDigimon( uint const& digimonUIDX ) const;
	bool			IsMemberTamer( uint const& tamerUIDX ) const;

	SMemberInfo const*	GetMemberInfo( int const& nSlot) const;
	SPartyInfo const*	GetPartyInfo() const;

private:
	CPartyContents* m_pContents;
};