#include "stdafx.h"
#include <assert.h>
#include "CsThreadBase.h"
#include "CsThreadManager.h"
#include "CsWaitableCollection.h"
#include "CsAutolock.h"
#include <algorithm>

std::vector< CsThreadManager::THREAD_LIST_DATA > CsThreadManager::m_vThreadList;
CsCriticalSection *CsThreadManager::m_pCS = NULL;

#ifdef _USE_THREAD_MANAGER_

void CsThreadManager::Initialize()
{
	m_vThreadList.clear();
	m_pCS = new CsCriticalSection;
}
void CsThreadManager::DeInitialize()
{
	m_vThreadList.clear();
	delete m_pCS;
	m_pCS = NULL;
}

BOOL CsThreadManager::RegisterThread( CsThreadBase *pThread, const char* szFileName, int nLine )
{
	if( !m_pCS ){
		return FALSE;
	}

	_SYNCHRONIZE_THREAD_( m_pCS );

	THREAD_DATA data;
	data.dwThreadID		= pThread->GetThreadId();
	data.pThread		= pThread;
	data.strThreadName	= pThread->GetThreadName();
	data.strFileName	= szFileName;
	data.nLine			= nLine;
	DWORD dwParentID	= pThread->GetParentThreadId();


	ThreadFindFromThreadListData threadListFindFunction( dwParentID );
	std::vector< THREAD_LIST_DATA >::iterator it =
		find_if( m_vThreadList.begin(), m_vThreadList.end(), threadListFindFunction );
	
	// Parent 노드가 존재하면 Thread가 등록이 되어 있는지 검사.
	if( it != m_vThreadList.end() )
	{
		ThreadFindFromThreadData threadFindFunction( data.dwThreadID );
		std::vector< THREAD_DATA >::iterator it2 =
			find_if( it->vThreadListData.begin(), it->vThreadListData.end(), threadFindFunction );
		
		// Thread가 등록되어 있으면 FALSE 리턴.
		if( it2 != it->vThreadListData.end() )
			return FALSE;

		// Thread가 등록되어 있지 않으면 TRUE 리턴.
		it->vThreadListData.push_back( data );
		return TRUE;
	}

	// Parent 노드가 존재하지 않으면 노드를 만든다.
	THREAD_LIST_DATA threadListData;
	threadListData.dwParentID = dwParentID;
	threadListData.vThreadListData.clear();
	
	// 새로 생성된 노드에 Thread 등록.
	threadListData.vThreadListData.push_back( data );
	m_vThreadList.push_back( threadListData );

	return TRUE;
}

BOOL CsThreadManager::UnregisterThread( CsThreadBase *pThread )
{
	if( !m_pCS ){
		return FALSE;
	}

	_SYNCHRONIZE_THREAD_( m_pCS );

	DWORD dwParentID = pThread->GetParentThreadId();
	DWORD dwThreadID = pThread->GetThreadId();

	ThreadFindFromThreadListData threadListFindFunction( dwParentID );
	std::vector< THREAD_LIST_DATA >::iterator it =
		find_if( m_vThreadList.begin(), m_vThreadList.end(), threadListFindFunction );

	// Parent 노드가 존재하면 Thread가 등록이 되어 있는지 검사.
	if( it != m_vThreadList.end() )
	{
		ThreadFindFromThreadData threadFindFunction( dwThreadID );
		std::vector< THREAD_DATA >::iterator it2 =
			find_if( it->vThreadListData.begin(), it->vThreadListData.end(), threadFindFunction );

		// Thread가 등록되어 있지 않으면 FALSE 리턴.
		if( it2 == it->vThreadListData.end() )
			return FALSE;

		// Thread가 등록되어 있으면 erase하고 TRUE 리턴.
		it->vThreadListData.erase( it2 );
		return TRUE;
	}

	// Parent 노드가 존재하지 않으면 FALSE 리턴.
	return FALSE;
}

VOID CsThreadManager::Enumerate( std::vector< THREAD_INFO > &vThreadInfoList )
{
	if( !m_pCS ){
		return;
	}

	_SYNCHRONIZE_THREAD_( m_pCS );
	
	DWORD dwParentID = ::GetCurrentThreadId();
	ThreadFindFromThreadListData threadListFindFunction( dwParentID );
	std::vector< THREAD_LIST_DATA >::iterator it =
		find_if( m_vThreadList.begin(), m_vThreadList.end(), threadListFindFunction );

	if( it != m_vThreadList.end() )
	{
		THREAD_LIST_DATA *pThreadListData = &*it;
		std::vector< THREAD_DATA >::iterator it2 = pThreadListData->vThreadListData.begin();
		while( it2 != pThreadListData->vThreadListData.end() )
		{
			THREAD_DATA *pThreadData = &*it2;
			THREAD_INFO info;

			info.dwParentID		= pThreadListData->dwParentID;
			info.dwThreadID		= pThreadData->dwThreadID;
			info.strThreadName	= pThreadData->pThread->GetThreadName();
			info.strFileName	= pThreadData->strFileName;
			info.nLine			= pThreadData->nLine;

			vThreadInfoList.push_back( info );

			it2++;
		}
	}
}

VOID CsThreadManager::EnumerateByCurrentThread( std::vector< CsThreadBase* > &vThreadList )
{
	if( !m_pCS ){
		return;
	}

	_SYNCHRONIZE_THREAD_( m_pCS );

	DWORD dwParentID = ::GetCurrentThreadId();
	ThreadFindFromThreadListData threadListFindFunction( dwParentID );
	std::vector< THREAD_LIST_DATA >::iterator it =
		find_if( m_vThreadList.begin(), m_vThreadList.end(), threadListFindFunction );

	if( it != m_vThreadList.end() )
	{
		if( (*it).dwParentID == dwParentID )
		{
			THREAD_LIST_DATA *pThreadListData = &*it;
			std::vector< THREAD_DATA >::iterator it2 = pThreadListData->vThreadListData.begin();
			while( it2 != pThreadListData->vThreadListData.end() )
			{
				vThreadList.push_back( (*it2).pThread );
				it2++;
			}
			return;
		}
	}	
}

VOID CsThreadManager::WaitGroupByCurrentThread()
{
	if( !m_pCS ){
		return;
	}

	DWORD dwParentID = ::GetCurrentThreadId();

	CsWaitableCollection waitCollection;
	// CRITICAL_SECTION
	{
		_SYNCHRONIZE_THREAD_( m_pCS );

		ThreadFindFromThreadListData threadListFindFunction( dwParentID );
		std::vector< THREAD_LIST_DATA >::iterator it =
			find_if( m_vThreadList.begin(), m_vThreadList.end(), threadListFindFunction );

		if( it == m_vThreadList.end() )
			return;

		if( (*it).dwParentID == dwParentID )
		{
			THREAD_LIST_DATA *pThreadListData = &*it;
			if( pThreadListData->vThreadListData.empty() ){
				return;
			}
			
			std::vector< THREAD_DATA >::iterator it2 = pThreadListData->vThreadListData.begin();
			while( it2 != pThreadListData->vThreadListData.end() )
			{
				waitCollection.AddObject( (*it2).pThread );
				it2++;
			}
		}
	}// CRITICAL_SECTION

	waitCollection.Wait( TRUE );
}

VOID CsThreadManager::StopGroupByCurrentThread()
{
	if( !m_pCS ){
		return;
	}
	
	// CRITICAL_SECTION
	{
		_SYNCHRONIZE_THREAD_( m_pCS );

		DWORD dwParentID = ::GetCurrentThreadId();
		ThreadFindFromThreadListData threadListFindFunction( dwParentID );
		std::vector< THREAD_LIST_DATA >::iterator it =
			find_if( m_vThreadList.begin(), m_vThreadList.end(), threadListFindFunction );

		if( it == m_vThreadList.end() )
			return;


		THREAD_LIST_DATA *pThreadListData = &*it;
		if( pThreadListData->vThreadListData.empty() ){
			return;
		}

		std::vector< THREAD_DATA >::iterator it2 = pThreadListData->vThreadListData.begin();
		while( it2 != pThreadListData->vThreadListData.end() )
		{
			(*it2).pThread->StopThread();
			it2++;
		}

	}// CRITICAL_SECTION
}

#else

void CsThreadManager::Initialize()
{
	__noop;
}

void CsThreadManager::DeInitialize()
{
	__noop;
}

BOOL CsThreadManager::RegisterThread( CsThreadBase *pThread, const char* szFileName, int nLine )
{
	return FALSE;
}

BOOL CsThreadManager::UnregisterThread( CsThreadBase *pThread )
{
	return FALSE;
}

VOID CsThreadManager::Enumerate( std::vector< THREAD_INFO > &vThreadInfoList )
{
}

VOID CsThreadManager::EnumerateByCurrentThread( std::vector< CsThreadBase* > &vThreadList )
{
}

#endif
