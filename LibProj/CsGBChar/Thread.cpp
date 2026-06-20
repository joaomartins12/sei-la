
#include "stdafx.h"
#include "Thread.h"

CsC_Thread*		g_pThread = NULL;

void CsC_Thread::GlobalInit()
{
	assert_cs( g_pThread == NULL );	
	g_pThread = NiNew CsC_Thread;
	g_pThread->Init();
}

void CsC_Thread::GlobalShotDown()
{
	if( g_pThread != NULL )
	{
		g_pThread->Destroy();
		SAFE_NIDELETE( g_pThread );
	}

	THREAD_MEMPOOL_DELETE( sTCUnit )
	THREAD_MEMPOOL_DELETE( sTCUnitPart )
	THREAD_MEMPOOL_DELETE( sDCUnitPart )
	//THREAD_MEMPOOL_DELETE( sTCardPart )
	THREAD_MEMPOOL_DELETE( CsC_LoadSequence::sTEXTKEY_SHADER )
	THREAD_MEMPOOL_DELETE( CsC_LoadSequence::sTEXTKEY_EVENT )
	THREAD_MEMPOOL_DELETE( CsC_LoadSequence::sSEQUENCE_INFO )	
	THREAD_MEMPOOL_DELETE( CsC_EventObject )
	THREAD_MEMPOOL_DELETE( CsC_EventTamerObject )
	THREAD_MEMPOOL_DELETE( CsC_EffectObject )
	THREAD_MEMPOOL_DELETE( CsC_PartObject )
	THREAD_MEMPOOL_DELETE( CsC_DPartObject )
	//THREAD_MEMPOOL_DELETE( CsC_CardObject )
	THREAD_MEMPOOL_DELETE( CsC_AvObject )	
	sRES_LOAD::GlobalShutDown();

	CsC_Property::ShotDown();
}

void CsC_Thread::SetRefThreadSuspend( bool bValue )
{
	if( g_pThread == NULL )
		return;

	g_pThread->_ThreadSuspend( bValue );
}

void CsC_Thread::_ThreadSuspend( bool bValue )
{
	//_ThreadSuspend( m_pLoadProc, bValue );
	//_ThreadSuspend( m_pDeleteProc, bValue );
	_ThreadSuspend( m_pRefCheckProc, bValue );
}


void CsC_Thread::_ThreadSuspend( CsThreadBase* pThread, bool bValue )
{
	if( NULL == pThread )
		return;

	DWORD dwCode = -1;
	pThread->GetExitCode( &dwCode );
	if( STILL_ACTIVE != dwCode )
		return;

	if( bValue )
	{
		DWORD dwCount = pThread->Suspend();
		while( 0 > dwCount )
			dwCount = pThread->Suspend();
	}
	else
	{
		DWORD dwCount = pThread->Resume();
		if( dwCount == -1 )
			return;// 스레드 활성화 실패

		while( 1 < dwCount )
			dwCount = pThread->Resume();
	}
}

void CsC_Thread::Init()
{
	g_pModelDataMng = NiNew CsModelDataMng;
	g_pModelDataMng->Init();	

	g_pEventSceneMng = NiNew CEventSceneMng;
	g_pEventSceneMng->Init();	

	g_pSEffectMgr = NiNew SEffectMgr;

	m_ResMng.Init();

	// Load
	m_pLoadProc = CsThread< CsC_ThreadProc_Load >::MakeInstance( "LoadChar" );	
	m_pLoadProc->CreateThread();
	m_pLoadProc->SetPriority( THREAD_PRIORITY_NORMAL );
	m_Collection.AddObject( m_pLoadProc );

	// Delete
	m_pDeleteProc = CsThread< CsC_ThreadProc_Delete >::MakeInstance( "DeleteChar" );	
	m_pDeleteProc->CreateThread();
	m_pDeleteProc->SetPriority( THREAD_PRIORITY_BELOW_NORMAL );
	m_Collection.AddObject( m_pDeleteProc );

	// RefCheck
	m_pRefCheckProc = CsThread< CsC_ThreadProc_RefCheck >::MakeInstance( "RefCheck" );	
	m_pRefCheckProc->CreateThread();
	m_pRefCheckProc->SetPriority( THREAD_PRIORITY_BELOW_NORMAL );
	m_pRefCheckProc->GetProcInstance()->SetCriticalSection_LoadProp( m_pLoadProc->GetProcInstance()->GetCS_ProcLoad() );
	m_Collection.AddObject( m_pRefCheckProc );

	fPushUnitLoad = &CsC_ThreadProc_Load::PushUnit_Thread;
	fPushUnitDelete = &CsC_ThreadProc_Delete::PushUnit_Thread;
}

void CsC_Thread::Reset()
{
	m_pRefCheckProc->StopThread();
	m_pLoadProc->StopThread();
	m_pDeleteProc->StopThread();
	m_Collection.Wait( TRUE );
	assert_cs( m_pLoadProc->GetProcInstance()->IsEmpty() == true );	
	assert_cs( m_pDeleteProc->GetProcInstance()->IsEmpty() == true );
	CsThread< CsC_ThreadProc_Load >::ReleaseInstance( m_pLoadProc );
	CsThread< CsC_ThreadProc_Load >::ReleaseInstance( m_pDeleteProc );
	CsThread< CsC_ThreadProc_Load >::ReleaseInstance( m_pRefCheckProc );

	// Load
	m_pLoadProc = CsThread< CsC_ThreadProc_Load >::MakeInstance( "LoadChar" );	
	m_pLoadProc->CreateThread();
	m_pLoadProc->SetPriority( THREAD_PRIORITY_NORMAL );
	m_Collection.AddObject( m_pLoadProc );

	// Delete
	m_pDeleteProc = CsThread< CsC_ThreadProc_Delete >::MakeInstance( "DeleteChar" );	
	m_pDeleteProc->CreateThread();
	m_pDeleteProc->SetPriority( THREAD_PRIORITY_BELOW_NORMAL );
	m_Collection.AddObject( m_pDeleteProc );

	// RefCheck
	m_pRefCheckProc = CsThread< CsC_ThreadProc_RefCheck >::MakeInstance( "RefCheck" );	
	m_pRefCheckProc->CreateThread();
	m_pRefCheckProc->SetPriority( THREAD_PRIORITY_IDLE );
	m_pRefCheckProc->GetProcInstance()->SetCriticalSection_LoadProp( m_pLoadProc->GetProcInstance()->GetCS_ProcLoad() );
	m_Collection.AddObject( m_pRefCheckProc );
}

void CsC_Thread::UseThread( bool bUseThread )
{
	if( bUseThread == true )
	{
		assert_cs( false );
		fPushUnitLoad = &CsC_ThreadProc_Load::PushUnit_Thread;
		fPushUnitDelete = &CsC_ThreadProc_Delete::PushUnit_Thread;
	}
	else
	{
		fPushUnitLoad = &CsC_ThreadProc_Load::PushUnit_NoThread;
		fPushUnitDelete = &CsC_ThreadProc_Delete::PushUnit_NoThread;

		m_pRefCheckProc->StopThread();
		m_pLoadProc->StopThread();
		m_pDeleteProc->StopThread();
		m_Collection.Wait( TRUE );
	}
}

void CsC_Thread::Destroy()
{
	// 나중에 -0-;
	// 로드 하던 것만 마저 하고 ( 하던거래야 하나? 나머진 다 버리자 )
	// 지우는것도 하던것만 마저하고 나머진 다 강제 지워 버리자

	g_pEventSceneMng->Delete();
	SAFE_NIDELETE( g_pEventSceneMng );

	m_pRefCheckProc->StopThread();
	m_pLoadProc->StopThread();
	m_pDeleteProc->StopThread();
	m_Collection.Wait( TRUE );

	g_pModelDataMng->Delete();
	SAFE_NIDELETE( g_pModelDataMng );

	assert_cs( m_pLoadProc->GetProcInstance()->IsEmpty() == true );	
	assert_cs( m_pDeleteProc->GetProcInstance()->IsEmpty() == true );

	m_ResMng.Destroy();
	CsThread< CsC_ThreadProc_Load >::ReleaseInstance( m_pLoadProc );
	CsThread< CsC_ThreadProc_Load >::ReleaseInstance( m_pDeleteProc );
	CsThread< CsC_ThreadProc_Load >::ReleaseInstance( m_pRefCheckProc );

	SAFE_NIDELETE( g_pSEffectMgr );

}