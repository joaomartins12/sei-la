
#include "stdafx.h"
#include "NpcExtra.h"


//====================================================================================================
//
//		sNPC_EXTRA_QUEST
//
//====================================================================================================

void sNPC_EXTRA_QUEST::sACT::Delete()
{
	s_list.clear();
}

void sNPC_EXTRA_QUEST::Delete()
{
	int nCnt = s_vpAct.Size();
	for( int i=0; i<nCnt; ++i )
	{
		s_vpAct[ i ]->Delete();
		delete s_vpAct[ i ];
	}
	s_vpAct.Destroy();
}

void sNPC_EXTRA_QUEST::Init()
{
	// 초기 상태 보이게
	s_eInitState = NEQ_SHOW;
}

