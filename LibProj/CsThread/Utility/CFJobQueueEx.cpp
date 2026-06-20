#include "stdafx.h"
#include "CFJobQueueEx.h"

////////////////////////////////////////////////////////////////////////////////////////////
// CFBossWorkerThreadHandlerEx Definition
////////////////////////////////////////////////////////////////////////////////////////////

class CFBossWorkerThreadHandlerEx
{
public:
	CFBossWorkerThreadHandlerEx( ITSJobQueue **paWaitJobQueue, CFPortInfo *paPortInfo );
	virtual ~CFBossWorkerThreadHandlerEx();

public:
	DWORD ThreadProc( THREAD_PROC_ARG* pArg );

protected:
	enum{
		NORMAL_QUEUE_INDEX = 0,
		WAIT_QUEUE_INDEX = 1,
	};

protected:
	ITSJobQueue** m_paWaitJobQueue;
	CFPortInfo *m_paPortInfo;
};

///////////////////////////////////////////////////////////////////////////////////////////
typedef CsThread<CFBossWorkerThreadHandlerEx, ITSJobQueue**, CFPortInfo*> CFBossWorkerThreadEx;
///////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////
// GlobalJobProcessor Instance
////////////////////////////////////////////////////////////////////////////////////////////////////////

static CFJobQueueEx* g_pGlobalInstance = NULL;

CFJobQueueEx::~CFJobQueueEx()
{
	Terminate();
}

BOOL CFJobQueueEx::InitializeGlobalInstance()
{
	if( g_pGlobalInstance )
		return FALSE;

	g_pGlobalInstance = new CFJobQueueEx;

	return TRUE;
}

CFJobQueueEx* CFJobQueueEx::GetGlobalInstance()
{
	return g_pGlobalInstance;
}

BOOL CFJobQueueEx::DeInitializeGlobalInstance()
{
	if( !g_pGlobalInstance )
		return FALSE;

	delete g_pGlobalInstance;
	g_pGlobalInstance = NULL;

	return TRUE;
}

CsThreadBase* CFJobQueueEx::CreateWorkerThread(
	const char *szThreadName, ITSJobQueue ** paWaitJobQueue, CFPortInfo* paPortInfo )
{
	CFBossWorkerThreadEx *pThread =  CFBossWorkerThreadEx::MakeInstance( szThreadName, paWaitJobQueue, paPortInfo );
	return pThread;
}

void CFJobQueueEx::DestroyWorkerThread( CsThreadBase *pWorkerThread )
{
	CFBossWorkerThreadEx::ReleaseInstance( pWorkerThread );
}

ITSJobQueue* CFJobQueueEx::AllocCompletionQueue()
{
	return new CThreadSafeQueue<CFJobBase*>;
}

void CFJobQueueEx::AllocWaitQueue( ITSJobQueue ** paWaitQueue, DWORD &dwCount )
{
	dwCount = 2;
	m_dwNormalQueueIndex = 0;
	m_dwWaitQueueIndex = 1;

	paWaitQueue[m_dwNormalQueueIndex] = new CLocklessQueue<CFJobBase*>;
	paWaitQueue[m_dwWaitQueueIndex] = new CLocklessQueue<CFJobBase*>;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CFBossWorkerThread Imp
////////////////////////////////////////////////////////////////////////////////////////////////////////

CFBossWorkerThreadHandlerEx::CFBossWorkerThreadHandlerEx( ITSJobQueue ** paWaitJobQueue, CFPortInfo* paPortInfo )
: m_paWaitJobQueue( paWaitJobQueue ), m_paPortInfo( paPortInfo )
{
}

CFBossWorkerThreadHandlerEx::~CFBossWorkerThreadHandlerEx()
{
	m_paWaitJobQueue = NULL;
	m_paPortInfo = NULL;
}

DWORD CFBossWorkerThreadHandlerEx::ThreadProc( THREAD_PROC_ARG* pArg )
{
	CsEvent* pevStop = pArg->pevStop;
	DWORD dwThreadAttribute = reinterpret_cast<DWORD&>( pArg->pArg );

	// 만약 스레드 속성이 default라면 default 속성 setting.
	if( dwThreadAttribute == WT_STYLE_DEFAULT ){
		dwThreadAttribute = WT_STYLE_ASYNC_QUEUE | WT_STYLE_SYNC_QUEUE;
	}

	//char msg[1024];
	//sprintf( msg, "create thread %s, %d attr(%d)\n",
	//	pArg->strThreadName.c_str(), GetCurrentProcessorNumber(), dwThreadAttribute );
	//OutputDebugString( msg );

	BOOL bUseAsyncQueue = CheckStyle( dwThreadAttribute, WT_STYLE_ASYNC_QUEUE );
	BOOL bUseSyncQueue = CheckStyle( dwThreadAttribute, WT_STYLE_SYNC_QUEUE );

	while( TRUE )
	{
		CFJobBase *pJob = NULL;

		if( *pArg->m_pbStop )
			break;

		BOOL bRet = FALSE;

		// AsyncQueue를 사용하는 스레드라면 WaitQueue에서 job을 얻어온다.

		if( bUseSyncQueue ){
			bRet = m_paWaitJobQueue[WAIT_QUEUE_INDEX]->Get( pJob );
		}

		// job을 얻지 못한 상태에서 SyncQueue를 사용하는 스레드라면 NormalQueue에서 job을 얻어온다.
		if( !bRet )
		{
			if( bUseAsyncQueue ){
				bRet = m_paWaitJobQueue[NORMAL_QUEUE_INDEX]->Get( pJob );
			}
		}

		// 사용하는 모든 큐에 job이 없다면 잠시 cpu를 놓고 다시 반복.
		if( !bRet )
		{
			YieldProcessor();
			continue;
		}

		// job이 있다면 실행.
		if( pJob ){
			pJob->DoJob( pevStop );
		}

		DWORD dwPort = pJob->m_dwPort;
		CFPortInfo *pPortInfo = &m_paPortInfo[ dwPort ];
		pPortInfo->pCompletionJobQueue->Put( pJob );

		// Port Type이 Wait라면 남은 Job이 없을때 Event를 Signal 시킨다.
		if( pPortInfo->dwPortType == PORT_TYPE_WAIT )
		{
			LONG lResult = ::InterlockedDecrement( &pPortInfo->lRemainJobCount );
			if( lResult == 0){
				pPortInfo->evCompletion.Set();
			}
		}
	}
	return 0L;
}