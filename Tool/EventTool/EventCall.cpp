#include "StdAfx.h"
#include "EventCall.h"

CEventCall::CEventCall(void)
{
}

CEventCall::~CEventCall(void)
{
}

void CEventCall::PlaySound( char* szSound )
{
	g_pSoundMgr->PlaySound( szSound );
}

void CEventCall::PlayMusic( char* szMusic )
{
	g_pSoundMgr->PlayMusic( szMusic );
}

void CEventCall::EventTalk( UINT nTargetUID, TCHAR* szMsg, bool bExistDelete, int ObjType )
{
	g_pTalkBallone->EventTalk( nTargetUID, szMsg, bExistDelete, ObjType );
}
