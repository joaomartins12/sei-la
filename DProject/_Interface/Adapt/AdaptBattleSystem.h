#pragma once

class IUIContentBase;
class CBattleSystemContents;

class AdaptBattleSystem
{
public:
	explicit AdaptBattleSystem(void);
	virtual ~AdaptBattleSystem(void);

	bool			SetContents(IUIContentBase* pContents);  
	bool			IsBattleMatchingRegisted() const;
	bool			IsBattlePlaying() const;
	bool			GetBattleLoadingInfo(BWait::sWait::WAIT_PLAYER_LIST_CON & info) const;
	void			GetBattleRegistInfo( char & cTeamCode, unsigned int & nBattleType, unsigned int & nBattleMode ) const;

private:
	CBattleSystemContents* m_pBattleSystemContents;
};