/********************************************************************
	created:	2006/02/23
	created:	23:2:2006   18:25
	filename: 	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB\CsThreadManager.h
	file path:	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB
	file base:	CsThreadManager
	file ext:	h
	author:		celes

	CsThread List를 관리하는 Class.
	CsThreadManager의 특징은 다음과 같습니다.

	1. Debug를 목적으로 하는 관리자 스레드를 생성합니다. 평소에는 아무런 일도 하지 않다가 통지를 받으면
	Minidump를 생성합니다. Debuger에 연결되어 있지 않을때 예기치 못한 문제가 발생하였을 경우를 대비하기 위함입니다.
	Minidump는 Visual Studio.NET 에서 바로 사용할 수 있으며 같은 소스가 존재할 때만 정상적으로 동작합니다.
	그 밖에 Debug를 위한 용도로 사용됩니다.

	2. 스레드들의 리스트를 부모스레드 기준으로 관리합니다.

	3. 부모스레드 기준으로 Enumeration 해줍니다. 모니터링의 목적으로 사용됩니다.

	4. 부모스레드를 그룹으로 하여 한번에 기능을 수행합니다.( wait, stop과 같은 기능들 )

	5. #define으로 코드의 변경 없이 enable/disable 가능합니다.

	6. CsThreadManager는 static class 입니다.

	그 밖의 기능들을 추가할 생각입니다.
*********************************************************************/

#pragma once

#include"CsCriticalSection.h"
#include<string>
#include<vector>

struct THREAD_INFO
{
	DWORD dwParentID;
	DWORD dwThreadID;
	std::string strThreadName;
	std::string strFileName;
	int nLine;
};

class CsThreadBase;
class CsThreadManager
{
public:
	static void Initialize();
	static void DeInitialize();
	
	static BOOL RegisterThread( CsThreadBase *pThread, const char* szFileName, int nLine );
	static BOOL UnregisterThread( CsThreadBase *pThread );

	static VOID Enumerate( std::vector< THREAD_INFO > &vThreadInfoList );
	static VOID EnumerateByCurrentThread( std::vector< CsThreadBase* > &vThreadList );
	static VOID WaitGroupByCurrentThread();
	static VOID StopGroupByCurrentThread();

protected:

	struct THREAD_DATA
	{
		DWORD dwThreadID;
		std::string strThreadName;
		std::string strFileName;
		int nLine;
		CsThreadBase* pThread;
	};

	struct THREAD_LIST_DATA
	{
		DWORD dwParentID;
		std::vector< THREAD_DATA > vThreadListData;
	};

	static std::vector< THREAD_LIST_DATA > m_vThreadList;
	static CsCriticalSection *m_pCS;

// 컨테이너에서 find할 때 사용되는 struct.
	struct ThreadFindFromThreadListData
	{
		explicit ThreadFindFromThreadListData( DWORD dwParentID ){ m_dwParentID = dwParentID; }
		BOOL operator()( const THREAD_LIST_DATA& rhs )
		{
			if( m_dwParentID == rhs.dwParentID )
				return TRUE;
			return FALSE;
		}

	private:
		DWORD m_dwParentID;
	};

	struct ThreadFindFromThreadData
	{
		explicit ThreadFindFromThreadData( DWORD dwThreadID ){ m_dwThreadID = dwThreadID; }
		BOOL operator()( const THREAD_DATA& rhs )
		{
			if( m_dwThreadID == rhs.dwThreadID )
				return TRUE;
			return FALSE;
		}

	private:
		DWORD m_dwThreadID;
	};
};