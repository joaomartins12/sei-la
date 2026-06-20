/********************************************************************
	created:	2006/02/22
	created:	22:2:2006   16:25
	filename: 	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB\CsThreadBase.h
	file path:	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB
	file base:	CsThreadBase
	file ext:	h
	author:		celes
	
	Thread의 기본적인 기능을 가지고 있는 Class.
	ThreadProcedure 구현을 위해서 DWORD ThreadProc( THREAD_PROC_ARG* pArg ); Interface를 포함한
	CWorkerThread를 만들어서 구현해주어야 합니다.

	THREAD_PROC_ARG::m_hStopEvent는 메인스레드에서 하던일을 중지하고 스레드를 종료시키라고 보내는 이벤트입니다.
	그렇기 때문에 ThreadProc내에서 스레드가 대기중일때 m_hStopEvent가 Signal 되면 ThreadProc()를 빠져나오도록
	처리하는것을 권장합니다.
	
	ex)
	class CWorkerThread
	{
	public:
		DWORD ThreadProc( THREAD_PROC_ARG* pArg )
		{
			printf( "CWorkerThread Started\n" );
			WaitForSingleObject( pArg->m_hStopEvent, INFINITE );
			return 0L;
		}
	};

	void main()
	{
		CsThreadBase *pTestThread = new CsThread< CWorkerThread >;
		Sleep( 3000 );
		printf( "Stop CWorkerThread\n" );
		pTestThread->StopThread();
		pTestThread->Wait();
		delete pTestThread;
	}
*********************************************************************/

#pragma once

#include <string>
#include "CsWaitableObject.h"
#include "CsEvent.h"
#include "CsThreadManager.h"


struct THREAD_PROC_ARG
{
	CsEvent* pevStop;
	CsEvent* pevSignal;
	volatile BOOL *m_pbStop;
	LPVOID pArg;
	std::string strThreadName;
	LPVOID pProcInstance;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// CsThreadBase Declare
class CsThreadBase : public CsWaitableObject
{
public:
	virtual ~CsThreadBase();

	BOOL IsValid(){ return m_hHandle != INVALID_HANDLE_VALUE; }

	void StopThread();

	DWORD Suspend();
	DWORD Resume();

	BOOL CreateThread( 	LPVOID pParam = NULL, BOOL bSuspend = FALSE, LPSECURITY_ATTRIBUTES lpSA = NULL, DWORD dwStackSize = 0 );
	BOOL IsCreated(){ return m_bCreated; }

#ifndef	_XBOX
	BOOL Terminate(DWORD dwExitCode);
#endif
	BOOL GetExitCode(DWORD *pdwExitCode);
	const char* GetThreadName();
	const CsEvent* GetSignal();

	BOOL SetPriority(int nPriority);
	int GetPriority();

	// win32에서는 무시.
	void SetThreadProcessor( DWORD dwProcessorNumber )
	{
#ifdef _XBOX
		if( m_hHandle == INVALID_HANDLE_VALUE )
			return;

		::XSetThreadProcessor( m_hHandle, dwProcessorNumber );
		m_dwProcessorNumber = dwProcessorNumber;
#endif
	}

	DWORD GetThreadProcessor()
	{
		return m_dwProcessorNumber;
	}


	DWORD GetThreadId();
	DWORD GetParentThreadId();

protected:
	// 그냥 생성 못해유.
	CsThreadBase();
	virtual BOOL RunThread( const char* szThreadName, LPVOID pParam, BOOL bSuspend, LPSECURITY_ATTRIBUTES lpSA,
		DWORD dwStackSize ) = 0;

protected:
	BOOL m_bCreated;
	CsEvent m_evStop;			// 스레드 정지를 요청하는 이벤트.
	volatile BOOL m_bStopFlag;	// 스레드 정지 요청을 받으면 TRUE가 세팅된다.
	CsEvent m_evSignal;		// 공용으로 사용될 시그널.
	DWORD m_dwParentID;
	DWORD m_dwThreadID;			// 스레드 아이디.
	std::string m_strThreadName;// 스레드 이름.
	DWORD m_dwProcessorNumber;

protected:
	THREAD_PROC_ARG m_Arg;		// 스레드 프로시저에게 넘겨줄 인자.
};
// CsThreadBase Declare End
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CsSetThreadName( DWORD dwThreadID, LPCSTR szThreadName);
const char* GetFileName( const char* szFileName );
 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CsThread Declare
#define CsTHREAD_TEMPLATE_ARG T, TYPE_ARG1, TYPE_ARG2, TYPE_ARG3

template< class T, class TYPE_ARG1 = DWORD, class TYPE_ARG2 = DWORD, class TYPE_ARG3 = DWORD >
class CsThread : public CsThreadBase
{
public:
	static CsThread<CsTHREAD_TEMPLATE_ARG>* MakeInstance( const char* szThreadName );
	static CsThread<CsTHREAD_TEMPLATE_ARG>* MakeInstance( const char* szThreadName, TYPE_ARG1 arg1 );
	static CsThread<CsTHREAD_TEMPLATE_ARG>* MakeInstance( const char* szThreadName, TYPE_ARG1 arg1, TYPE_ARG2 arg2 );
	static CsThread<CsTHREAD_TEMPLATE_ARG>* MakeInstance( const char* szThreadName, TYPE_ARG1 arg1, TYPE_ARG2 arg2, TYPE_ARG3 arg3 );

	static void ReleaseInstance( CsThreadBase* pThread );


    T* GetProcInstance(){ return m_pProcInstance; }

	// SendMessage를 하려면 ThreadProc를 CsMessageThreadProc 또는 CsWaitingThreadProc를 상속 받아서 작성해야 한다. 
	void SendMessage( DWORD dwMsgID, DWORD dwArg1, DWORD dwArg2, FLOAT fArg1, FLOAT fArg2, void* pData = NULL, size_t unDataSize = 0 ){
		m_pProcInstance->SendMessage( dwMsgID, dwArg1, dwArg2, pData, unDataSize );
	}

protected:
	T *m_pProcInstance;

protected:
	// default constructor, copy constructor 호출 못해유.
	CsThread(){ m_pProcInstance = NULL; m_dwProcessorNumber = 0; }
	CsThread( const CsThread &rh ){}
	CsThread<CsTHREAD_TEMPLATE_ARG>& operator=( const CsThread<CsTHREAD_TEMPLATE_ARG>& rh ){ return *this; }

	virtual ~CsThread(){ delete m_pProcInstance; m_pProcInstance = NULL; }
	BOOL RunThread( const char* szThreadName, LPVOID pParam, BOOL bSuspend, LPSECURITY_ATTRIBUTES lpSA,
		DWORD dwStackSize );

	static DWORD WINAPI CallThreadHandlerProc( void* pArg );
};
// CsThread Declare End
///////////////////////////////////////////////////////////////////////////////////////////////////////////////









// CsThreadBase Implement ////////////////////////////////////////////////////////////////////////////////////
inline CsThreadBase::CsThreadBase() 
: m_bCreated( FALSE ), m_evStop( FALSE ), m_evSignal( FALSE ), m_bStopFlag( FALSE )
{
	m_dwThreadID = 0;
	m_dwParentID = ::GetCurrentThreadId();
}

inline CsThreadBase::~CsThreadBase()
{
	m_dwParentID = 0;
	m_dwThreadID = 0;
}

inline void CsThreadBase::StopThread()
{
	m_evStop.Set();
	m_bStopFlag = TRUE;
}

inline DWORD CsThreadBase::Suspend()
{
	return ::SuspendThread( m_hHandle );
}

inline DWORD CsThreadBase::Resume()
{
	return ::ResumeThread( m_hHandle );
}

inline BOOL CsThreadBase::CreateThread( LPVOID pParam, BOOL bSuspend, LPSECURITY_ATTRIBUTES lpSA, DWORD dwStackSize )
{
	if( m_bCreated )
		return FALSE;

	if( !RunThread( m_strThreadName.c_str(), pParam, bSuspend, lpSA, dwStackSize ) ){
		return FALSE;
	}
	m_bCreated = TRUE;

	return TRUE;
}


#ifndef	_XBOX
inline BOOL CsThreadBase::Terminate( DWORD dwExitCode )
{
	return ::TerminateThread( m_hHandle, dwExitCode );
}
#endif

inline BOOL CsThreadBase::GetExitCode( DWORD *pdwExitCode )
{
	return ::GetExitCodeThread( m_hHandle, pdwExitCode );
}

inline const char* CsThreadBase::GetThreadName()
{
	return m_strThreadName.c_str();
}

inline const CsEvent* CsThreadBase::GetSignal()
{
	return &m_evSignal;
}

inline BOOL CsThreadBase::SetPriority( int nPriority )
{
	return ::SetThreadPriority( m_hHandle, nPriority );
}

inline int	CsThreadBase::GetPriority()
{
	return ::GetThreadPriority( m_hHandle );
}

inline DWORD CsThreadBase::GetParentThreadId()
{
	return m_dwParentID;
}

inline DWORD CsThreadBase::GetThreadId()
{
	return m_dwThreadID;
}

// CsThread Implement ////////////////////////////////////////////////////////////////////////////////////////
template< class T, class TYPE_ARG1, class TYPE_ARG2, class TYPE_ARG3 >
inline void CsThread<CsTHREAD_TEMPLATE_ARG>::ReleaseInstance( CsThreadBase* pThread )
{
	if( !pThread )
		return;

	// Thread Manager에서 삭제.
	CsThreadManager::UnregisterThread( pThread );

	delete pThread;
}

template< class T, class TYPE_ARG1, class TYPE_ARG2, class TYPE_ARG3 >
inline BOOL CsThread<CsTHREAD_TEMPLATE_ARG>::RunThread( const char* szThreadName, LPVOID pParam, BOOL bSuspend, LPSECURITY_ATTRIBUTES lpSA,
								 DWORD dwStackSize )
{
	m_strThreadName = szThreadName;
	m_Arg.strThreadName = szThreadName;
	m_Arg.pevStop = &m_evStop;
	m_Arg.pevSignal = &m_evSignal;
	m_Arg.m_pbStop = &m_bStopFlag;
	m_Arg.pArg = pParam;
	m_Arg.pProcInstance = m_pProcInstance;

	m_hHandle = ::CreateThread( lpSA, dwStackSize, CallThreadHandlerProc, 
		&m_Arg, ( !bSuspend ? 0 : CREATE_SUSPENDED ), &m_dwThreadID );

	return CsKIsValidHandle( m_hHandle );
}

template< class T, class TYPE_ARG1, class TYPE_ARG2, class TYPE_ARG3 >
inline DWORD WINAPI CsThread<CsTHREAD_TEMPLATE_ARG>::CallThreadHandlerProc( void* pArg )
{
	THREAD_PROC_ARG* pThreadProcArg = static_cast<THREAD_PROC_ARG*>( pArg );
	T *pProcInstance = static_cast<T*>( pThreadProcArg->pProcInstance );
	DWORD dwThreadID = ::GetCurrentThreadId();
	CsSetThreadName( dwThreadID, pThreadProcArg->strThreadName.c_str() );
	pThreadProcArg->pProcInstance = NULL;

	return pProcInstance->ThreadProc( pThreadProcArg );
}

template< class T, class TYPE_ARG1, class TYPE_ARG2, class TYPE_ARG3 >
inline CsThread<CsTHREAD_TEMPLATE_ARG>* CsThread<CsTHREAD_TEMPLATE_ARG>::MakeInstance( const char* szThreadName )
{
	CsThread<T>* pNewThread = new CsThread<CsTHREAD_TEMPLATE_ARG>;
	pNewThread->m_pProcInstance = new T;
	pNewThread->m_strThreadName = szThreadName;

	// Thread Manager에 등록한다.
	CsThreadManager::RegisterThread( pNewThread, GetFileName( __FILE__ ), __LINE__ );

	return pNewThread;
}

template< class T, class TYPE_ARG1, class TYPE_ARG2, class TYPE_ARG3 >
inline CsThread<CsTHREAD_TEMPLATE_ARG>* CsThread<CsTHREAD_TEMPLATE_ARG>::MakeInstance( const char* szThreadName, TYPE_ARG1 arg1 )
{
	CsThread<CsTHREAD_TEMPLATE_ARG>* pNewThread = new CsThread<CsTHREAD_TEMPLATE_ARG>;
	pNewThread->m_pProcInstance = new T( arg1 );
	pNewThread->m_strThreadName = szThreadName;

	// Thread Manager에 등록한다.
	CsThreadManager::RegisterThread( pNewThread, GetFileName( __FILE__ ), __LINE__ );

	return pNewThread;
}

template< class T, class TYPE_ARG1, class TYPE_ARG2, class TYPE_ARG3 >
inline CsThread<CsTHREAD_TEMPLATE_ARG>* CsThread<CsTHREAD_TEMPLATE_ARG>::MakeInstance( const char* szThreadName, TYPE_ARG1 arg1, TYPE_ARG2 arg2 )
{
	CsThread<CsTHREAD_TEMPLATE_ARG>* pNewThread = new CsThread<CsTHREAD_TEMPLATE_ARG>;
	pNewThread->m_pProcInstance = new T( arg1, arg2 );
	pNewThread->m_strThreadName = szThreadName;

	// Thread Manager에 등록한다.
	CsThreadManager::RegisterThread( pNewThread, GetFileName( __FILE__ ), __LINE__ );

	return pNewThread;
}

template< class T, class TYPE_ARG1, class TYPE_ARG2, class TYPE_ARG3 >
inline CsThread<CsTHREAD_TEMPLATE_ARG>* CsThread<CsTHREAD_TEMPLATE_ARG>::MakeInstance( const char* szThreadName, TYPE_ARG1 arg1, TYPE_ARG2 arg2, TYPE_ARG3 arg3 )
{
	CsThread<CsTHREAD_TEMPLATE_ARG>* pNewThread = new CsThread<CsTHREAD_TEMPLATE_ARG>;
	pNewThread->m_pProcInstance = new T( arg1, arg2, arg3 );
	pNewThread->m_strThreadName = szThreadName;

	// Thread Manager에 등록한다.
	CsThreadManager::RegisterThread( pNewThread, GetFileName( __FILE__ ), __LINE__ );

	return pNewThread;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Thread 이름을 등록하기 위한 코드.
/////////////////////////////////////////////////////////////////////////////////////////////
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // must be 0x1000
	LPCSTR szName; // pointer to name (in user addr space)
	DWORD dwThreadID; // thread ID (-1=caller thread)
	DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;

inline void CsSetThreadName( DWORD dwThreadID, LPCSTR szThreadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
	}
	__except( EXCEPTION_CONTINUE_EXECUTION )
	{
	}
}

inline const char* GetFileName( const char* szFileName )
{
	const char* p = strrchr( szFileName, '\\' );
	if( p ){
		return p + 1;
	}

	return szFileName;
}
/////////////////////////////////////////////////////////////////////////////////////////////