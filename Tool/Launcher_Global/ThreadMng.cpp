
#include "stdafx.h"
#include "ThreadMng.h"

CThread*		g_pThread = NULL;

void CThread::GlobalInit()
{
	assert( g_pThread == NULL );	
	g_pThread = new CThread;
	g_pThread->Init();
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

void CThread::Init()
{
	// Unzip
	m_pUnzip = CsThread< CThread_Data >::MakeInstance( "Data" );	
	m_pUnzip->CreateThread();
	m_pUnzip->SetPriority( THREAD_PRIORITY_NORMAL );
	m_Collection.AddObject( m_pUnzip );
}

void CThread::Destroy()
{
	m_pUnzip->StopThread();
	m_Collection.Wait( TRUE );

	CsThread< CThread_Data >::ReleaseInstance( m_pUnzip );
}
