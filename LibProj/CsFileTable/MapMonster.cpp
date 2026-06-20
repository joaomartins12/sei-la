
#include "stdafx.h"
#include "MapMonster.h"


//=========================================================================================================
//
//		Information
//
//=========================================================================================================
void CsMapMonster::sINFO::Init()
{
	s_dwMapID = s_dwMonsterTableID = 0;
	s_nCenterX = s_nCenterY = 0;
	s_nRadius = 1000;
	s_nCount = 10;
	s_nResponTime = 60;
	
	s_nKillgen_MonFTID = 0;
	s_nKillgen_Count = 0;
	s_nKillgen_ViewCnt = 0;
	s_nMoveType = 1;
	s_bInsRespon = false;
}

DWORD CsMapMonster::sINFO::GetModelID()
{
	assert_cs( s_dwMonsterTableID != 0 );
	assert_cs( nsCsFileTable::g_pMonsterMng != NULL );
	assert_csm1( nsCsFileTable::g_pMonsterMng->IsMonster( s_dwMonsterTableID ), _T( "Mon ID = %d" ), s_dwMonsterTableID );
	return nsCsFileTable::g_pMonsterMng->GetMonster( s_dwMonsterTableID )->GetInfo()->s_dwModelID;
}

//=========================================================================================================
//
//		CsMapMonster
//
//=========================================================================================================
void CsMapMonster::Delete()
{

}

void CsMapMonster::Init( sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}

