#include "StdAfx.h"
#include "AdaptBattleSystem.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../00.Contents/BattleSystemContent.h"

AdaptBattleSystem::AdaptBattleSystem(void):m_pBattleSystemContents(NULL)
{
}

AdaptBattleSystem::~AdaptBattleSystem(void)
{
}

bool AdaptBattleSystem::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, NULL );
	if( E_CT_BATTLE_SYSTEM != pContents->GetContentsIdentity() )
		return false;

	m_pBattleSystemContents = static_cast< CBattleSystemContents* >( pContents );

	return true;
}

// 배틀 신청한 상태인지 체크
bool AdaptBattleSystem::IsBattleMatchingRegisted() const
{
	SAFE_POINTER_RETVAL( m_pBattleSystemContents, false );	
	return m_pBattleSystemContents->IsBattleMatchRegisted();
}

bool AdaptBattleSystem::IsBattlePlaying() const
{
	SAFE_POINTER_RETVAL( m_pBattleSystemContents, false );	
	return m_pBattleSystemContents->IsBattlePlaying();
}

// 배틀 로딩 UI 표시해줄 데이터를 가져 온다.
bool AdaptBattleSystem::GetBattleLoadingInfo(BWait::sWait::WAIT_PLAYER_LIST_CON & info) const
{
	SAFE_POINTER_RETVAL( m_pBattleSystemContents, false );	
	return m_pBattleSystemContents->GetBattleLoadingInfo(info);
}

void AdaptBattleSystem::GetBattleRegistInfo( char & cTeamCode, unsigned int & nBattleType, unsigned int & nBattleMode ) const
{
	SAFE_POINTER_RET( m_pBattleSystemContents );	
	m_pBattleSystemContents->GetBattleRegistInfo(cTeamCode, nBattleType, nBattleMode );
}