
#include "stdafx.h"
#include "Tip.h"

cTip*	g_pTip = NULL;

void cTip::GlobalInit()
{
	assert_cs( g_pTip == NULL );
	g_pTip = NiNew cTip;
	g_pTip->Init();
}

void cTip::GlobalShotDown()
{
	if( g_pTip == NULL )
		return;

	g_pTip->Delete();
	SAFE_NIDELETE( g_pTip );
}

void cTip::Init()
{
	m_TimeSeq.SetDeltaTime( 1000*60 );
}

void cTip::PreResetMap()
{
	Delete();

	m_TimeSeq.OffReset();
}

void cTip::Delete()
{
}

void cTip::Update(float const& fDeltaTime)
{

	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
		return;

	if( g_pResist->m_Global.s_bTipBalloone == false )
		return;

	if( m_TimeSeq.IsEnable() == false )
		return;

	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
	if( pDigimonUser->GetProp_Attack()->GetFollowTarget() != NULL )
		return;
	if( pDigimonUser->IsBattle() == true )
		return;

	CsTalk_Tip* pFTTip = nsCsFileTable::g_pTalkMng->GetRandom_Tip();
	if( pFTTip == NULL )
		return;

	TCHAR sz[ FT_TALK_TIP_LEN ];
	_tcscpy_s( sz, FT_TALK_TIP_LEN, pFTTip->GetInfo()->s_szTip );	
	g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_2, pDigimonUser->GetUniqID(), sz, false );
	
	ST_CHAT_PROTOCOL	CProtocol;	
	CProtocol.m_Index= pDigimonUser->GetUniqID();
	CProtocol.m_Type = NS_CHAT::DIGIMON_TIP;
	CProtocol.m_wStr = sz;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

