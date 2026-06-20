

#include "stdafx.h"
#include "AutoUpgradeDlg.h"

bool CAutoUpgradeDlg::_Check_Pack()
{
	// 현재는 01번 팩만 체크 하겠다
	if( _taccess_s( L"Pack01", 0 ) == 0 )
	{
		// 폴더가 존재 한다면 총 파일 갯수를 구한다.
		SetState( eSTATE_DO_PACK );
		Proc_MainState();

		g_pThread->Get()->ResetInfo();
		g_pThread->Get()->SetVersion( U_WAIT );
		Proc_SubState();

		// 쓰레드에 데이터 셋팅		
		sTHREAD_INPUT ti;
		ti.s_eType = sTHREAD_INPUT::DO_PACK;
		ti.s_szDownPath = L"Pack01";
		g_pThread->Add( &ti );

		// 디스플레이를 위한 타이머 셋팅
		SetTimer( ID_TIMER_DO_PACK, ID_TIMER_DO_PACK_TICK, NULL );
		return true;
	}
	return false;
}
