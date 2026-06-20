#include"stdafx.h"
#include<assert.h>
#include"CFBackgroundJobProcessor.h"
#include"../CsWaitableCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// CFBackgroundJobProcessorBase Imp
////////////////////////////////////////////////////////////////////////////////////////////

CFBackgroundJobProcessorBase::CFBackgroundJobProcessorBase()
{
	m_dwStatus = STATUS_STOP;
	m_dwPortCount = 0;
	m_dwThreadCount = 0;
	m_dwQueueCount = 0;

	for( int i = 0; i < MAX_THREAD; i++ ){
		m_paBossWorkerThread[i] = NULL;
	}

	for( int i = 0; i < MAX_QUEUE; i++ ){
		m_paWaitJobQueue[i] = NULL;
	}

	m_dwNormalQueueIndex = 0;
	m_dwWaitQueueIndex = 0;
}

CFBackgroundJobProcessorBase::~CFBackgroundJobProcessorBase()
{
}

BOOL CFBackgroundJobProcessorBase::StartJobProcessor( DWORD dwWorkerThreadCount, BOOL bSuspend )
{
	if( m_dwStatus != STATUS_STOP ){
		return FALSE;
	}

	// CreatePort가 한번도 호출 되지 않았다면 시작할 수 없다.
	if( m_dwPortCount == 0 ){
		return FALSE;
	}

	int count = 0;

	m_ThreadCollection.Clear();
	for( int i = 0; i < (int)dwWorkerThreadCount; i++ )
	{
		char szThreadName[64];
		sprintf_s( szThreadName, 64, "Job Thread %d", count );
		CsThreadBase *pNewThread = CreateWorkerThread( szThreadName, m_paWaitJobQueue, m_aPortInfo );
		pNewThread->CreateThread( (void*)WT_STYLE_DEFAULT );
		
		if( pNewThread )
		{
			m_paBossWorkerThread[count] = pNewThread;
			m_ThreadCollection.AddObject( pNewThread );
			count++;
		}
	}

	if( count == 0 ){
		return FALSE;
	}

	m_dwThreadCount = count;
	m_dwStatus = STATUS_START;
	return TRUE;
}

BOOL CFBackgroundJobProcessorBase::StartJobProcessorEx( JOB_QUEUE_OPTION &option )
{
	if( m_dwStatus != STATUS_STOP ){
		return FALSE;
	}

	// CreatePort가 한번도 호출 되지 않았다면 시작할 수 없다.
	if( m_dwPortCount == 0 ){
		return FALSE;
	}

	int count = 0;

	m_ThreadCollection.Clear();
	for( int i = 0; i < (int)option.dwThreadNumber; i++ )
	{
		JOB_QUEUE_OPTION::WORKER_THREAD_INFO *pInfo = &option.info[i];
		char szThreadName[64];
		DWORD dwProcessorNumber = pInfo->dwProcessNumber;
		sprintf_s( szThreadName, 64, "Job Thread %d", count );
		CsThreadBase *pNewThread = CreateWorkerThread( szThreadName, m_paWaitJobQueue, m_aPortInfo );

		if( pNewThread )
		{
			pNewThread->CreateThread( (void*&)pInfo->dwThreadAttribute, TRUE );
			if( dwProcessorNumber != PROCESS_NUMBER_NOT_ASSIGN ){
				pNewThread->SetThreadProcessor( dwProcessorNumber );
			}

			if( !option.bSuspend ){
				pNewThread->Resume();
			}

			m_paBossWorkerThread[count] = pNewThread;
			m_ThreadCollection.AddObject( pNewThread );
			count++;
		}
	}

	if( count == 0 ){
		return FALSE;
	}

	m_dwThreadCount = count;
	m_dwStatus = STATUS_START;
	return TRUE;
}

BOOL CFBackgroundJobProcessorBase::TryStopJobProcessor()
{
	if( m_dwStatus != STATUS_START ){
		return FALSE;
	}

	// 모든 Thread에게 종료하라고 알린다.
	for( int i = 0; i < (int)m_dwThreadCount; i++ ){
		m_paBossWorkerThread[i]->StopThread();
	}
	m_dwStatus = STATUS_WAIT;

	return TRUE;
}

BOOL CFBackgroundJobProcessorBase::IsCompletionStopJobProcessor()
{
	if( m_dwStatus != STATUS_WAIT ){
		return FALSE;
	}

	if( TRUE == CsKWaitTimeout( m_ThreadCollection.Wait( TRUE, IGNORE ) ) ){
		return FALSE;
	}

	ClearJobQueue();

	// 스레드를 모두 지워준다.
	for( int i = 0; i < (int)m_dwThreadCount; i++ )
	{
		DestroyWorkerThread( m_paBossWorkerThread[i] );
		m_paBossWorkerThread[i] = NULL;
	}

	m_dwThreadCount = 0;
	m_dwStatus = STATUS_STOP;

	return TRUE;
}

void CFBackgroundJobProcessorBase::SupendThreads()
{
	for( int i = 0; i < (int)m_dwThreadCount; i++ ){
		m_paBossWorkerThread[i]->Suspend();
	}
}

void CFBackgroundJobProcessorBase::ResumeThreads()
{
	for( int i = 0; i < (int)m_dwThreadCount; i++ ){
		m_paBossWorkerThread[i]->Resume();
	}
}

void CFBackgroundJobProcessorBase::SetThreadPriority( DWORD dwThreadNum, int dwThreadPriority )
{
	if( dwThreadNum >= m_dwThreadCount ){
		return;
	}

	m_paBossWorkerThread[dwThreadNum]->SetPriority( dwThreadPriority );
}

int CFBackgroundJobProcessorBase::GetThreadPriority( DWORD dwThreadNum )
{
	if( dwThreadNum >= m_dwThreadCount ){
		return 0L;
	}

	return m_paBossWorkerThread[dwThreadNum]->GetPriority();
}

void CFBackgroundJobProcessorBase::SetThreadProcessor( DWORD dwThreadNum, DWORD dwProcessorNum )
{
	if( dwThreadNum >= m_dwThreadCount ){
		return;
	}

	m_paBossWorkerThread[dwThreadNum]->SetThreadProcessor( dwProcessorNum );
}

int CFBackgroundJobProcessorBase::GetThreadProcessor( DWORD dwThreadNum )
{
	if( dwThreadNum >= m_dwThreadCount ){
		return 0xffffffff;
	}

	return m_paBossWorkerThread[dwThreadNum]->GetThreadProcessor();
}

DWORD CFBackgroundJobProcessorBase::CreatePort( DWORD dwPortType )
{
	if( m_dwStatus != STATUS_STOP ){
		return INVALID_PORT;
	}

	if( m_dwPortCount >= MAX_PORT ){
		return INVALID_PORT;
	}

	if( m_dwPortCount == 0 ){
		assert( m_paWaitJobQueue[0] == NULL );
		AllocWaitQueue( m_paWaitJobQueue, m_dwQueueCount );
		assert( m_dwQueueCount != 0 );
	}

	if( dwPortType != PORT_TYPE_WAIT )
	{
		dwPortType = PORT_TYPE_NORMAL;
	}

	DWORD dwNewPort = m_dwPortCount;
	m_dwPortCount++;

	// Port Info를 초기화 하세.
	m_aPortInfo[ dwNewPort ].Init();
	m_aPortInfo[ dwNewPort ].dwPortType = dwPortType;
	m_aPortInfo[ dwNewPort ].pCompletionJobQueue = AllocCompletionQueue();

	return dwNewPort;
}

BOOL CFBackgroundJobProcessorBase::MakeJob( CFJobBase* pJob, DWORD dwPort )
{
	assert( pJob );

	if( dwPort >= m_dwPortCount ){
		return FALSE;
	}

	// Port 타입이 Normal이 아니면 MakeJob을 할 수 없다.
	if( m_aPortInfo[ dwPort ].dwPortType != PORT_TYPE_NORMAL ){
		return FALSE;
	}

	pJob->m_dwPort = dwPort;
	m_paWaitJobQueue[m_dwNormalQueueIndex]->Put( pJob );

	return TRUE;
}

BOOL CFBackgroundJobProcessorBase::MakeWaitJob( CFJobContainer &jobs, DWORD dwPort )
{
	if( dwPort >= m_dwPortCount ){
		return FALSE;
	}

	// Port 타입이 Wait이 아니면 MakeWaitJob을 할 수 없다.
	if( m_aPortInfo[ dwPort ].dwPortType != PORT_TYPE_WAIT ){
		return FALSE;
	}

	// MakeWaitJob은 한프레임에 한번만 호출할 수 있다.
	if( m_aPortInfo[ dwPort ].dwPortStatus == PORT_STATUS_LOCKED ){
		return FALSE;
	}

	if( jobs.empty() ){
		return FALSE;
	}

	m_aPortInfo[ dwPort ].dwPortStatus = PORT_STATUS_LOCKED;
	m_aPortInfo[ dwPort ].lRemainJobCount = static_cast<DWORD>( jobs.size() );

	CFJobContainer::iterator it = jobs.begin();
	while( it != jobs.end() )
	{
		CFJobBase *pJob = (*it);
		assert( pJob );

		pJob->m_dwPort = dwPort;
		m_paWaitJobQueue[m_dwWaitQueueIndex]->Put( pJob );
		it++;
	}

	// Job을 요청했으니 끝날때까지 대기하기 위해서 Event를 Reset 시킨다.
	m_aPortInfo[ dwPort ].evCompletion.Reset();

	return TRUE;
}

BOOL CFBackgroundJobProcessorBase::GetCompletionJob( CFJobContainer &vCompletionJobs, DWORD dwPort, DWORD dwMillisecond )
{
	if( m_dwStatus != STATUS_START )
		return FALSE;

	vCompletionJobs.clear();

	if( dwPort >= m_dwPortCount )
		return FALSE;

	// Port 타입이 Wait인 경우에는 Job이 종료될때까지 기다린다.
	if( m_aPortInfo[ dwPort ].dwPortStatus == PORT_TYPE_WAIT ){
		// Timeout이면 FALSE 리턴.
		BOOL bRet = CsKWaitTimeout( m_aPortInfo[ dwPort ].evCompletion.Wait( dwMillisecond ) );
		if( bRet ){
			return FALSE;
		}
	}

	m_aPortInfo[ dwPort ].pCompletionJobQueue->Get( vCompletionJobs );
	return TRUE;
}

void CFBackgroundJobProcessorBase::Terminate()
{
	if( m_dwStatus == STATUS_START ){
		// BackgroundJobProcessor가 시작된 상태라면 Free가 완료되기 전까지 Block.
		TryStopJobProcessor();
		while( IsCompletionStopJobProcessor() == FALSE ){
			Sleep(1);
		}
	}
	else{
		// BackgroundJobProcessor가 시작되기 전에 CreatePort가 호출 될 수 있으므로 ClearJobQueue가 호출 되어야 함.
		ClearJobQueue();
	}
}

void CFBackgroundJobProcessorBase::ClearJobQueue()
{
	// 큐에 남아 있는 Job들을 delete 해준다.
	for( int i = 0; i < (int)m_dwPortCount; i++ )
	{
		std::vector<CFJobBase*> vJobs;
		m_aPortInfo[ i ].pCompletionJobQueue->Get( vJobs );

		std::vector<CFJobBase*>::iterator it = vJobs.begin();
		while( it != vJobs.end() )
		{
			delete (*it);
			it++;
		}

		delete m_aPortInfo[ i ].pCompletionJobQueue;
		m_aPortInfo[ i ].pCompletionJobQueue = NULL;
	}
	m_dwPortCount = 0;

	// WaitJobQueue에 있는 Job들도 Delete 해준다.
	for( int i = 0; i < (int)m_dwQueueCount; i++ )
	{
		std::vector<CFJobBase*> vJobs;
		m_paWaitJobQueue[ i ]->Get( vJobs );

		std::vector<CFJobBase*>::iterator it = vJobs.begin();
		while( it != vJobs.end() )
		{
			delete (*it);
			it++;
		}
			
		delete m_paWaitJobQueue[ i ];
		m_paWaitJobQueue[ i ] = NULL;
	}

	m_dwQueueCount = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////
// CFBossWorkerThreadHandler Definition
////////////////////////////////////////////////////////////////////////////////////////////

class CFBossWorkerThreadHandler
{
public:
	CFBossWorkerThreadHandler( ITSJobQueue **paWaitJobQueue, CFPortInfo *paPortInfo );
	virtual ~CFBossWorkerThreadHandler();

public:
	DWORD ThreadProc( THREAD_PROC_ARG* pArg );

protected:
	enum{
		QUEUE_INDEX = 0,		
	};

protected:
	ITSJobQueue** m_paWaitJobQueue;
	CFPortInfo *m_paPortInfo;
};

///////////////////////////////////////////////////////////////////////////////////////////
typedef CsThread<CFBossWorkerThreadHandler, ITSJobQueue**, CFPortInfo*> CFBossWorkerThread;
///////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////
// GlobalJobProcessor Instance
////////////////////////////////////////////////////////////////////////////////////////////////////////

static CFBackgroundJobProcessor* g_pGlobalInstance = NULL;

CFBackgroundJobProcessor::~CFBackgroundJobProcessor()
{
	Terminate();
}

BOOL CFBackgroundJobProcessor::InitializeGlobalInstance()
{
	if( g_pGlobalInstance )
		return FALSE;

	g_pGlobalInstance = new CFBackgroundJobProcessor;

	return TRUE;
}

CFBackgroundJobProcessor* CFBackgroundJobProcessor::GetGlobalInstance()
{
	return g_pGlobalInstance;
}

BOOL CFBackgroundJobProcessor::DeInitializeGlobalInstance()
{
	if( !g_pGlobalInstance )
		return FALSE;

	delete g_pGlobalInstance;
	g_pGlobalInstance = NULL;

	return TRUE;
}

CsThreadBase* CFBackgroundJobProcessor::CreateWorkerThread(
	const char *szThreadName, ITSJobQueue ** paWaitJobQueue, CFPortInfo* paPortInfo )
{
	CFBossWorkerThread *pThread =  CFBossWorkerThread::MakeInstance( szThreadName, paWaitJobQueue, paPortInfo );
	return pThread;
}

void CFBackgroundJobProcessor::DestroyWorkerThread( CsThreadBase *pWorkerThread )
{
	CFBossWorkerThread::ReleaseInstance( pWorkerThread );
}

ITSJobQueue* CFBackgroundJobProcessor::AllocCompletionQueue()
{
	return new CThreadSafeQueue<CFJobBase*>;
}

void CFBackgroundJobProcessor::AllocWaitQueue( ITSJobQueue ** paWaitQueue, DWORD &dwCount )
{
	dwCount = 1;
	m_dwNormalQueueIndex = 0;

	paWaitQueue[m_dwNormalQueueIndex] = new CWaitQueue<CFJobBase*>;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CFBossWorkerThread Imp
////////////////////////////////////////////////////////////////////////////////////////////////////////

CFBossWorkerThreadHandler::CFBossWorkerThreadHandler( ITSJobQueue ** paWaitJobQueue, CFPortInfo* paPortInfo )
: m_paWaitJobQueue( paWaitJobQueue ), m_paPortInfo( paPortInfo )
{
}

CFBossWorkerThreadHandler::~CFBossWorkerThreadHandler()
{
	m_paWaitJobQueue = NULL;
	m_paPortInfo = NULL;
}

DWORD CFBossWorkerThreadHandler::ThreadProc( THREAD_PROC_ARG* pArg )
{
	CsEvent* pevStop = pArg->pevStop;
	//char msg[1024];
	//sprintf( msg, "create thread %s, %d\n", pArg->strThreadName.c_str(), GetCurrentProcessorNumber() );
	//OutputDebugString( msg );

	while( TRUE )
	{
		CFJobBase *pJob = NULL;

		if( !m_paWaitJobQueue[QUEUE_INDEX]->Get( pJob, pevStop ) ){
			break;
		}

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