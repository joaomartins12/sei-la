/********************************************************************
	created:	2006/03/09
	created:	9:3:2006   20:22
	filename: 	d:\work\Lsp\CFNetworkLIB\CFNetworkLIB\CFReferenceCountManager.h
	file path:	d:\work\Lsp\CFNetworkLIB\CFNetworkLIB
	file base:	CFReferenceCountManager
	file ext:	h
	author:		celes
	
	- Object의 Reference Count를 계산하는 Manager.
	- delete를 요청받은 Object는 더 이상 Reference가 되지 않고 Reference Count가 0이 되었을때
	메모리에서 delete 시킵니다.

	- ID와 array(vector)의 Index가 동일 합니다. 그렇기 때문에 

	- ThreadSafe하게 동작하기 위해서 Reference()와 Release()를 DelObject, AddObject()를 Lock 시킵니다.
	최고 2^24 - 1개 만큼 저장 가능합니다.

	- Object의 ThreadSafe는 보장하지 않습니다.

	- Index의 대역폭은 0x00000000 ~ 0xffffffff 그리고 0xffffffff 입니다.

	- Type의 Pointer를 리턴함.

	<sample>
	// Object 넣기
	DWORD index = m_pRFManager->AddObject( instance );
	// 얻어오기
	instance = m_pRFManager->Reference( index );
	// Reference의 값이 NULL이면 Object가 이미 지워졌다는 뜻.
	if( instance )
	{
		DWORD dwRet = instance->Process();
		if( dwRet != 0 )
		{
			//instance를 지운다.
			m_pRFManager->DelObject( index );
		}
	}
	// Object를 다 썼으면 반환. 이때 DelObject가 호출되고 ReferenceCount가 0이 되면 TRUE를 리턴한다. 그때 instance를 delete 해주면 된다.
	BOOL bNeedDelete = m_pRFManager->Release( index );
	
	if( bNeedDelete )
	{
		delete instance;
	}
	

	
*********************************************************************/

#pragma once

#include"CsThreadBase.h"
#include"CThreadSafeQueue.h"
#include"CsWaitableCollection.h"
#include"./Utility/CFBackgroundJobProcessor.h"
#include"./Utility/CFJobQueueEx.h"
#include"./Utility/CFReferenceCountEvent.h"
#include"./Utility/CFExclusiveLockManager.h"
#include"./Utility/CFReferenceCountManager.h"
#include"./Utility/CFReadWriteLock.h"
#include"CsSpinlock.h"

#include<vector>
#include<assert.h>

template <typename T>
class CFReferenceCountManager
{
public:
	enum
	{
		INVALID_HANDLE = 0xffffffff
	};
public:
	enum{
		MIN_SIZE	  = 1024,
		MAX_SIZE	  = 65536,
	};

	CFReferenceCountManager( DWORD dwPoolSize = MIN_SIZE );
	virtual ~CFReferenceCountManager();

	DWORD AttachObject( T* pObj );
	BOOL DetachObject( DWORD index );
	T* Reference( DWORD index );
	BOOL Release( DWORD index );

	// Object들을 해제시키기 위해서만 사용.
	void Enumerate( std::vector<T*> &m_vObjectList );

protected:

	void clear();

	static DWORD ConvertSeqNumAndIndexToHandle( DWORD dwSeqNum, DWORD dwIndex )
	{
		return ( ( dwSeqNum << 16 ) + dwIndex );
	}

	static void ConvertHandleToSeqNumAndIndex( DWORD dwHandle, DWORD &dwSeqNum, DWORD &dwIndex )
	{
		dwSeqNum = ( dwHandle & 0xffff0000 ) >> 16;
		dwIndex = dwHandle & 0x0000ffff;
	}

	// 빈 노드일때의 struct의 상태
	// m_pObj = NULL, 
	// m_dwHandle의 상위 비트는 SeqNum, 하위 비트는 NextFreeIndex.

	struct _NODE
	{
		_NODE( DWORD dwSeqNum, DWORD dwIndex )
		{
			m_dwHandle = CFReferenceCountManager::ConvertSeqNumAndIndexToHandle( dwSeqNum, dwIndex );
			m_pObj = NULL;
			m_nRefCnt = 0;
			m_bDetach = FALSE;
		};
		
		_NODE( DWORD dwHandle )
		{
			m_dwHandle = dwHandle;
			m_pObj = NULL;
			m_nRefCnt = 0;
			m_bDetach = FALSE;
		};

		void SetSeqNumAndIndex( DWORD dwSeqNum, DWORD dwIndex )
		{
			m_dwHandle = CFReferenceCountManager::ConvertSeqNumAndIndexToHandle( dwSeqNum, dwIndex );
		};

		void GetSeqNumAndIndex( DWORD &dwSeqNum, DWORD &dwIndex )
		{
			CFReferenceCountManager::ConvertHandleToSeqNumAndIndex( m_dwHandle, dwSeqNum, dwIndex );
		};

		T *m_pObj;
		DWORD m_dwHandle;
		DWORD m_nRefCnt;
		BOOL  m_bDetach;
	};


	// Free Block Head Index 

	std::vector<_NODE> m_vObjectPool;			// Object Pool
	volatile DWORD m_dwFreeIndex;				// Free List Head Pointer
	volatile DWORD m_dwObjectCount;				// Object Count
	CsCriticalSection m_CS;
};

template <typename T>
CFReferenceCountManager<T>::CFReferenceCountManager( DWORD dwPoolSize/* = MIN_SIZE */)
{
	if( dwPoolSize < MIN_SIZE )
		dwPoolSize = MIN_SIZE;
	
	m_vObjectPool.resize( dwPoolSize, _NODE( 0 ) );
	clear();
}

template <typename T>
CFReferenceCountManager<T>::~CFReferenceCountManager()
{
	clear();
}

template <typename T>
void CFReferenceCountManager<T>::clear()
{
	DWORD dwSize = static_cast< DWORD >( m_vObjectPool.size() );
	
	for( DWORD i = 0; i < dwSize; i++ ){
		m_vObjectPool[i] = _NODE( i + 1 );
	}
	m_dwFreeIndex = 0;
	m_dwObjectCount = 0;
}


template <typename T>
DWORD CFReferenceCountManager<T>::AttachObject( T *pObj )
{
	_SYNCHRONIZE_THREAD_( &m_CS );

	// Object의 갯수가 MAX_SIZE를 넘어서면 INVALID_HANDLE을 리턴.
	if( m_dwObjectCount == MAX_SIZE || pObj == NULL )
		return INVALID_HANDLE;

	DWORD dwSeqNum;
	DWORD dwIndex;
	DWORD dwPoolSize = static_cast< DWORD >( m_vObjectPool.size() );

	// 더 이상 ObjectPool에 빈 노드가 존재하지 않으면 노드를 한개 증가시킨다.
	if( m_dwFreeIndex >= dwPoolSize ){
		m_vObjectPool.push_back( _NODE( m_dwFreeIndex + 1 ) );
	}

	_NODE *pNewNode = &m_vObjectPool[m_dwFreeIndex];
	
	pNewNode->GetSeqNumAndIndex( dwSeqNum, dwIndex );

	// 다음 FreeNode의 Index를 저장.
	DWORD dwNextFreeIndex = dwIndex;

	// SeqNum을 증가시킨다.
	++dwSeqNum %= 0x0fff; 

	// Object Count를 증가시킨다.
	++m_dwObjectCount;

	// 증가된 SeqNum과 Index를 조합해서 Handle을 만든다.
	pNewNode->SetSeqNumAndIndex( dwSeqNum, m_dwFreeIndex );
	pNewNode->m_pObj = pObj;

	// 다음 FreeNode의 Index를 Setting.
	m_dwFreeIndex = dwNextFreeIndex;

	return pNewNode->m_dwHandle;
}

template <typename T>
BOOL CFReferenceCountManager<T>::DetachObject( DWORD dwHandle )
{
	if( dwHandle == INVALID_HANDLE ){
		return FALSE;
	}

	_SYNCHRONIZE_THREAD_( &m_CS );
	
	DWORD dwSeqNum;
	DWORD dwIndex;
	DWORD dwPoolSize = static_cast< DWORD >( m_vObjectPool.size() );

	// Index와 SeqNum을 분리!
	ConvertHandleToSeqNumAndIndex( dwHandle, dwSeqNum, dwIndex );

	if( dwIndex >= dwPoolSize )
		return FALSE;

	_NODE *pDetachNode = &m_vObjectPool[dwIndex];
	
	// 존재하지 않거나 Handle이 틀린 경우 Detach 될 수 없다.
	if( pDetachNode->m_pObj == NULL || pDetachNode->m_dwHandle != dwHandle )
		return FALSE;

	// DetachNode를 하기 전에 Reference를 해야 한다.
	assert( pDetachNode->m_nRefCnt != 0 );

	// DetachNode가 된 Node는 다시 Detach 될 수 없다.
	assert( !pDetachNode->m_bDetach );

	pDetachNode->m_bDetach = TRUE;

	return TRUE;
}

template <typename T>
T* CFReferenceCountManager<T>::Reference( DWORD dwHandle )
{
	if( dwHandle == INVALID_HANDLE ){
		return NULL;
	}

	_SYNCHRONIZE_THREAD_( &m_CS );

	DWORD dwSeqNum;
	DWORD dwIndex;
	DWORD dwPoolSize = static_cast< DWORD >( m_vObjectPool.size() );

	// Index와 SeqNum을 분리!
	ConvertHandleToSeqNumAndIndex( dwHandle, dwSeqNum, dwIndex );

	if( dwIndex >= dwPoolSize )
		return NULL;

	_NODE *pNode = &m_vObjectPool[dwIndex];

	// 존재하지 않거나 Handle이 틀린 경우 Reference 될 수 없다. 또한 Detach가 요청된 Node는 Reference 될 수 없다.
	if( pNode->m_pObj == NULL || pNode->m_dwHandle != dwHandle || pNode->m_bDetach )
		return NULL;

	++pNode->m_nRefCnt;

	return pNode->m_pObj;
}

template <typename T>
BOOL CFReferenceCountManager<T>::Release( DWORD dwHandle )
{
	_SYNCHRONIZE_THREAD_( &m_CS );

	DWORD dwSeqNum;
	DWORD dwIndex;
	DWORD dwPoolSize = static_cast< DWORD >( m_vObjectPool.size() );

	// Index와 SeqNum을 분리!
	ConvertHandleToSeqNumAndIndex( dwHandle, dwSeqNum, dwIndex );

	if( dwIndex >= dwPoolSize )
		return NULL;

	_NODE *pNode = &m_vObjectPool[dwIndex];
	
	// 존재하지 않거나 Handle이 틀린 경우 Reference 될 수 없다.
	if( pNode->m_pObj == NULL || pNode->m_dwHandle != dwHandle )
		return FALSE;

	// Reference Count가 0보다 작아지면 안된다!
	assert( pNode->m_nRefCnt > 0 );

	// Detach가 요청되었고 Reference Count가 0이면 Node를 FreeList에 연결시킨다.
	if( --pNode->m_nRefCnt == 0 && pNode->m_bDetach ){
		m_vObjectPool[dwIndex] = _NODE( dwSeqNum, m_dwFreeIndex );
		m_dwFreeIndex = dwIndex;
		
		// Object Count를 감소시킨다.
		--m_dwObjectCount;
		return TRUE;
	}

	return FALSE;
}

template <typename T>
void CFReferenceCountManager<T>::Enumerate( std::vector<T*> &vObjectList )
{
	_SYNCHRONIZE_THREAD_( &m_CS );
	std::vector<_NODE>::iterator it = m_vObjectList.begin();
	while( it != m_vObjectList.end() )
	{
		if( it->m_pObj != NULL ){
			vObjectList.push_back( it->m_pObj );
		}
		it++
	}
}