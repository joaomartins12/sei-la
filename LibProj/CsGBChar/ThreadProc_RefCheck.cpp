

#include "stdafx.h"
#include "ThreadProc_RefCheck.h"

#define REF_CHECK_TIME		1000*60	*15		// 1초 * 초 * 분

DWORD CsC_ThreadProc_RefCheck::ThreadProc( THREAD_PROC_ARG* pArg )
{
	CsEvent *pevStop = pArg->pevStop;

	while( TRUE )
	{
		DWORD dwRet = pevStop->Wait( REF_CHECK_TIME );

		// Stop Event가 Signal 되었는가?
		if( CsKWaitSucceeded( dwRet, 1 ) )
		{
			// 지울게 없다면
			break;
		}
		else if( CsKWaitTimeout( dwRet ) )
		{
			_SYNCHRONIZE_THREAD_( m_pCS_ProcLoad );
			g_pThread->GetResMng()->RefCheck();
		}
		else
		{
			// wait가 실패되었으므로 에러메시지를 알린다.
			assert_cs( false );
			CsKThrowError( GetLastError() );
			break;
		}
	}
	return 0L;
}
