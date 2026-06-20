//
//
//					CThreadSafeQueue.h
//
//		1. ThreadSafe 를 보장하는 큐.
//		2. ThreadSafe 를 보장하면서 데이터가 없으면 Block 되는 큐.
//
//		created  by celes 2004.03.19
//		modified by celes 2006.03.10
//
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAITQUEUE_H__7485DE7F_BC64_4648_9F6E_4F410F0D4BCD__INCLUDED_)
#define AFX_WAITQUEUE_H__7485DE7F_BC64_4648_9F6E_4F410F0D4BCD__INCLUDED_

#include<list>
#include<vector>
#include"CsAutolock.h"
#include"CsEvent.h"

enum
{
	RESULT_STOP_SIGNAL = 0,
	RESULT_SUCCESS,
	RESULT_FAILED,
	RESULT_EMPTY,
};

template <class Type>
class IFThreadSafeQueue
{
public:
	virtual ~IFThreadSafeQueue(){}

	virtual void Put(IN Type& item) = 0;
	virtual bool Get(OUT Type& item) = 0;
	virtual int Get(OUT Type& item, CsEvent* pevStop) = 0;

	// queue의 모든 item을 가져옴. 프로세스 종료시에 메모리 free를 위해 사용됨.
	virtual int Get(OUT std::vector<Type>& vItems) = 0;
};

template <class Type>
class CThreadSafeQueue : public IFThreadSafeQueue<Type>
{
public:
	CThreadSafeQueue();
	virtual ~CThreadSafeQueue();

	void Put( IN Type& item );
	bool Get( OUT Type& item );
	int Get( OUT Type& item, CsEvent* pevStop );	
	bool Empty();

	// queue의 모든 item을 가져옴. 프로세스 종료시에 메모리 free를 위해 사용됨.
	int Get( OUT std::vector<Type>& vItems ); 

private:
	std::list<Type> m_queue;
	CsCriticalSection m_CS;
};

class CThreadSafeRef
{
public:
	CThreadSafeRef(){ m_nRef = 0;}

	void		IncreaseRef(){ _SYNCHRONIZE_THREAD_(&m_CS); ++m_nRef; }
	void		AddRef( int nAdd ){ _SYNCHRONIZE_THREAD_(&m_CS); m_nRef += nAdd; }
	void		DecreaseRef(){ _SYNCHRONIZE_THREAD_(&m_CS); --m_nRef; }
	int			GetRef(){ _SYNCHRONIZE_THREAD_(&m_CS); return m_nRef; }

private:
	int					m_nRef;
	CsCriticalSection	m_CS;
};

class CThreadSafeBool
{
public:
	bool				Is(){ _SYNCHRONIZE_THREAD_(&m_CS); return m_bBool; }
	void				Set( bool bBool ){ _SYNCHRONIZE_THREAD_(&m_CS); m_bBool = bBool; }

private:
	bool				m_bBool;
	CsCriticalSection	m_CS;
};

class CThreadSafeDWORD
{
public:	
	void				Lock_Set( DWORD dwDWORD ){ _SYNCHRONIZE_THREAD_(&m_CS); m_nDWORD = dwDWORD; }
	void				Lock_AddOr( DWORD plag ){ _SYNCHRONIZE_THREAD_(&m_CS); m_nDWORD |= plag; }
	DWORD				Lock_Get(){ _SYNCHRONIZE_THREAD_(&m_CS); return m_nDWORD; }
	//bool				IsAnd( DWORD plag ){ _SYNCHRONIZE_THREAD_(&m_CS); return ( ( m_nDWORD & plag ) != 0 ); }

	DWORD				m_nDWORD;
	CsCriticalSection	m_CS;
};

template <class Type>
class CWaitQueue : public IFThreadSafeQueue<Type>  
{
public:
	CWaitQueue();
	virtual ~CWaitQueue();

	void Put(IN Type& item);
	bool Get(OUT Type& item);
	int Get(OUT Type& item, CsEvent* pevStop);

	// queue의 모든 item을 가져옴. 프로세스 종료시에 메모리 free를 위해 사용됨.
	// Semaphore가 적용되지 않기 때문에 다시 Get을 하면 실행을 보장 못함.
	int Get(OUT std::vector<Type>& vitems);

private:
	std::list<Type> m_queue;
	CsCriticalSection m_CS;		// Put, Get과 같은 CriticalSection을 보호하기 위해 사용.

	HANDLE m_hNotEmpty;			// Wait()를 호출했을때 큐에 값이 없으면 값이 Put될때까지 대기.(세마포어)
};

template <class Type>
class CLocklessQueue : public IFThreadSafeQueue<Type>
{
public:
	CLocklessQueue();
	virtual ~CLocklessQueue();

	void Put(IN Type& item);
	bool Get(OUT Type& item);
	int Get(OUT Type& item, CsEvent* pevStop);

	// queue의 모든 item을 가져옴. 프로세스 종료시에 메모리 free를 위해 사용됨.
	// Semaphore가 적용되지 않기 때문에 다시 Get을 하면 실행을 보장 못함.
	int Get(OUT std::vector<Type>& vitems);

private:
	struct CFEntry : public SLIST_ENTRY
	{
		Type item;
	};

	SLIST_HEADER m_head;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CThreadSafeQueue();
//
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class Type>
CThreadSafeQueue<Type>::CThreadSafeQueue()
{
	m_queue.clear();
}

template <class Type>
CThreadSafeQueue<Type>::~CThreadSafeQueue()
{
}

template <class Type>
void CThreadSafeQueue<Type>::Put(Type& item)
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	m_queue.push_back(item);
}

template <class Type>
bool CThreadSafeQueue<Type>::Get(Type& item)
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	if(!m_queue.size())
	{
		return false;
	}
	
	item = m_queue.front();
	m_queue.pop_front();
	return true;
}

template <class Type>
int CThreadSafeQueue<Type>::Get(Type& item, CsEvent* pevStop)
{
	DWORD dwResult = pevStop->Wait( IGNORE );
	if( CsKWaitSucceeded( dwResult, 1 ) )
		return RESULT_STOP_SIGNAL;

	_SYNCHRONIZE_THREAD_(&m_CS);
	if(!m_queue.size())
	{
		return RESULT_EMPTY;
	}

	item = m_queue.front();
	m_queue.pop_front();
	return RESULT_SUCCESS;
}

template <class Type>
int CThreadSafeQueue<Type>::Get(OUT std::vector<Type>& vItems)
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	while( m_queue.empty() == false )
	{
		Type item = m_queue.front();
		m_queue.pop_front();
		vItems.push_back(item);
	}
	return (int)vItems.size();
}

template <class Type>
bool CThreadSafeQueue<Type>::Empty()
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	return m_queue.empty();
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CWaitQueue();
//
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class Type>
CWaitQueue<Type>::CWaitQueue()
{
	m_queue.clear();
	m_hNotEmpty = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
}

template <class Type>
CWaitQueue<Type>::~CWaitQueue()
{
	CloseHandle(m_hNotEmpty);
}

template <class Type>
void CWaitQueue<Type>::Put(Type& item)
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	m_queue.push_back(item);
	ReleaseSemaphore(m_hNotEmpty, 1, NULL);
}

template <class Type>
bool CWaitQueue<Type>::Get( Type& item )
{
	WaitForSingleObject( m_hNotEmpty, INFINITE );

	_SYNCHRONIZE_THREAD_(&m_CS);
	item = m_queue.front();
	m_queue.pop_front();

	return true;
}

template <class Type>
int CWaitQueue<Type>::Get( Type& item, CsEvent* pevStop )
{
	DWORD dwResult = pevStop->WaitTwo( m_hNotEmpty, FALSE, INFINITE );
	if( CsKWaitSucceeded( dwResult, 2 ) )
	{
		switch( CsKWaitSucceededIndex( dwResult ) )
		{
		case 0: // 종료 시그널.
			return RESULT_STOP_SIGNAL;

		case 1: // 리소스를 가져가시게.
			break;
		}
	}
	else
	{
		CsKThrowError( GetLastError() );
		return RESULT_FAILED;
	}

	_SYNCHRONIZE_THREAD_(&m_CS);
	item = m_queue.front();
	m_queue.pop_front();

	return RESULT_SUCCESS;
}

template <class Type>
int CWaitQueue<Type>::Get(std::vector<Type>& vItems)
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	int size = static_cast<int>(m_queue.size());
	for(int i = 0; i < size; i++)
	{
		Type item = m_queue.front();
		m_queue.pop_front();
		vItems.push_back(item);
	}
	return size;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// CLocklessQueue
//
///////////////////////////////////////////////////////////////////////////////////////////////

template <class Type>
CLocklessQueue<Type>::CLocklessQueue()
{
	InitializeSListHead( &m_head );
}

template <class Type>
CLocklessQueue<Type>::~CLocklessQueue()
{
	InterlockedFlushSList( &m_head );
}

template <class Type>
void CLocklessQueue<Type>::Put(IN Type& item)
{
	CFEntry *pEntry = new CFEntry;
	pEntry->item = item;

#ifdef _XBOX
	InterlockedPushEntrySListRelease( &m_head, pEntry );
#else
	InterlockedPushEntrySList( &m_head, pEntry );
#endif
}

template <class Type>
bool CLocklessQueue<Type>::Get(OUT Type& item)
{
	CFEntry *pEntry = static_cast<CFEntry*>(
#ifdef _XBOX
		InterlockedPopEntrySListAcquire( &m_head )
#else
		InterlockedPopEntrySList( &m_head )
#endif
		);

	if( pEntry )
	{
		item = pEntry->item;
		delete pEntry;
		return true;
	}

	return false;
}

template <class Type>
int CLocklessQueue<Type>::Get(OUT Type& item, CsEvent* pevStop)
{
	DWORD dwResult = pevStop->Wait( IGNORE );
	if( CsKWaitSucceeded( dwResult, 1 ) )
		return RESULT_STOP_SIGNAL;

	if( !Get( item ) )
	{
		return RESULT_EMPTY;
	}

	return RESULT_SUCCESS;
}

template <class Type>
int CLocklessQueue<Type>::Get(OUT std::vector<Type>& vitems)
{
	CFEntry *pEntry = static_cast<CFEntry*>( InterlockedFlushSList( &m_head ) );
	while( true ){
		if( !pEntry )
			break;

#ifdef _XBOX
		__lwsync();
#endif

		vitems.push_back( pEntry->item );
		CFEntry *pWillDelete = pEntry;
		pEntry = static_cast<CFEntry*>( pEntry->Next );
		delete pWillDelete;
	}

	return (int)vitems.size();
}


#endif // !defined(AFX_WAITQUEUE_H__7485DE7F_BC64_4648_9F6E_4F410F0D4BCD__INCLUDED_)