
#include "stdafx.h"
#include "ThreadProc.h"
#include "ThreadMng.h"

CThread*		g_pThread = NULL;

bool CThread::GlobalInit()
{
	assert( g_pThread == NULL );	
	g_pThread = new CThread;
	return g_pThread->Init();
}

void CThread::GlobalShotDown()
{
	if( g_pThread != NULL )
	{
		g_pThread->Destroy();
		delete g_pThread;
		g_pThread = NULL;
	}
}

bool CThread::Init()
{
	m_pThreadProc = CsThread< CThreadProc >::MakeInstance( "Thread" );
	
	if( m_pThreadProc->CreateThread() != TRUE )
		return false;

	if( m_collection.AddObject( m_pThreadProc ) != TRUE )
		return false;

	return true;
}

void CThread::Destroy()
{	
	m_pThreadProc->StopThread();

	// 모든 스레드가 종료될때까지 대기한다.
	m_collection.Wait( TRUE );

	// 모든 스레드를 해제한다.
	CsThread< CThreadProc >::ReleaseInstance( m_pThreadProc );
}
