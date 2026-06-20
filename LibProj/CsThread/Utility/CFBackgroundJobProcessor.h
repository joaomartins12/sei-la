/********************************************************************
	created:	2006/03/10
	created:	10:3:2006   11:05
	filename: 	d:\work\Lsp\CFNetworkLIB\CFNetworkLIB\CFBackgroundJobProcessor.h
	file path:	d:\work\Lsp\CFNetworkLIB\CFNetworkLIB
	file base:	CFBossWorkerThread
	file ext:	h
	author:		celes
	
	- Job Instance를 등록하면 background로 처리하고 결과를 알려주는 Thread.
	- Job은 CFJobBase를 상속받아서 구현해야 합니다.
	- Job을 분류해서 받을 수 있도록 Port를 제공합니다.
	- Job을 등록할 때 Port 번호를 적어주면 처리가 끝나고 해당 Port로 결과를 보내줍니다.
	- Job의 결과는 Polling으로 받도록 되어 있습니다.
	- StartJobProcessor호출후에는 Port를 만들 수 없습니다.
	- StartJobProcessor호출시에 dwProcessorNumber 인자는 Win32에서는 무시합니다.
	- Port를 만들때 Port Type을 지정하는데 Type에 따라서 사용해야 하는 MakeJob 함수가 다릅니다.
	- PORT_TYPE_NORMAL의 경우 MakeJob( CFJobBase *pJob )을 사용하시면 됩니다.
	- PORT_TYPE_FRAME의 경우 MakeJob( JobContainer &jobs )를 사용하시면 됩니다.
	- MakeJob( JobContainer &jobs )는 프레임(GetCompletionJobs을 호출)당 한번만 호출 가능합니다.

	- Thread Priority Define
	#define THREAD_PRIORITY_LOWEST          THREAD_BASE_PRIORITY_MIN
	#define THREAD_PRIORITY_BELOW_NORMAL    (THREAD_PRIORITY_LOWEST+1)
	#define THREAD_PRIORITY_NORMAL          0
	#define THREAD_PRIORITY_HIGHEST         THREAD_BASE_PRIORITY_MAX
	#define THREAD_PRIORITY_ABOVE_NORMAL    (THREAD_PRIORITY_HIGHEST-1)
	#define THREAD_PRIORITY_ERROR_RETURN    (MAXLONG)

	#define THREAD_PRIORITY_TIME_CRITICAL   THREAD_BASE_PRIORITY_LOWRT
	#define THREAD_PRIORITY_IDLE            THREAD_BASE_PRIORITY_IDL

*********************************************************************/

#pragma once

#include"../CsThreadBase.h"
#include"../CThreadSafeQueue.h"
#include"../CsWaitableCollection.h"


enum{
	INVALID_PORT = 0xffffffff,

	MAX_PORT = 20,
	MAX_THREAD = 20,
	MAX_QUEUE = 5,
	MAX_PROCESSOR_NUMBER = 5,

	STATUS_STOP = 0,	// Job Processor는 중지되었음.
	STATUS_START = 1,	// Job Processor는 작동하고 있음.
	STATUS_WAIT = 2,	// Job Processor에 중지 명령을 내리고 WorkerThread들이 중지되기를 기다리고 있음.

	PORT_TYPE_NORMAL = 0,
	PORT_TYPE_WAIT = 1,

	PRIORITY_ABOVE_NORMAL = THREAD_PRIORITY_ABOVE_NORMAL,
	PRIORITY_NORMAL = THREAD_PRIORITY_NORMAL,
	PRIORITY_BELOW_NORMAL = THREAD_PRIORITY_BELOW_NORMAL,
};

enum{
	WT_STYLE_DEFAULT		= 0x00000000L,
	WT_STYLE_WAITABLE		= 0x00000001L,
	WT_STYLE_SYNC_QUEUE		= 0x00000002L,
	WT_STYLE_ASYNC_QUEUE	= 0x00000004L,

	PROCESS_NUMBER_NOT_ASSIGN = 0xffffffffL,
};

inline BOOL CheckStyle( DWORD dwWorkerThreadStyle, DWORD dwCheckStyle )
{
	return ( dwWorkerThreadStyle & dwCheckStyle );
}

struct JOB_QUEUE_OPTION
{
	DWORD	dwThreadNumber;
	BOOL	bSuspend;

	struct WORKER_THREAD_INFO
	{
		DWORD	dwProcessNumber;
		DWORD	dwThreadAttribute;
	} info[MAX_THREAD];
};

class CFJobBase
{
public:
	CFJobBase( DWORD dwJobClsID ){ m_dwJobClsID = dwJobClsID; }
	virtual ~CFJobBase(){};
	virtual void DoJob( CsEvent *pevStop ) = 0;
	virtual void DoResult() = 0;

	DWORD GetJobClsID(){ return m_dwJobClsID; }
	

	// 요건 건드리지 맙시다.
	DWORD m_dwPort;

private:
	DWORD m_dwJobClsID;
};

//////////////////////////////////////////////////////////////////////////////////
typedef IFThreadSafeQueue<CFJobBase*> ITSJobQueue;
typedef std::vector<CFJobBase*> CFJobContainer;
typedef std::vector<CFJobBase*>::iterator CFJobContainerIterator;
/////////////////////////////////////////////////////////////////////////////////

struct CFPortInfo
{
	CFPortInfo() : evCompletion( TRUE )
	{
		Init();
	}

	void Init()
	{
		pCompletionJobQueue = NULL;
		dwPortType = 0;
		dwPortStatus = 0;
		lRemainJobCount = 0;
		evCompletion.Set();
	}

	ITSJobQueue *pCompletionJobQueue;
	DWORD dwPortType;
	DWORD dwPortStatus;
	volatile LONG lRemainJobCount;
	CsEvent evCompletion;
};

class CFBackgroundJobProcessorBase
{
public:
	enum{
		PORT_STATUS_UNLOCKED = 0,
		PORT_STATUS_LOCKED = 1,
	};

	CFBackgroundJobProcessorBase();
	virtual ~CFBackgroundJobProcessorBase();

	BOOL StartJobProcessor( DWORD dwWorkerThreadCount, BOOL bSuspend = FALSE );
	BOOL StartJobProcessorEx( JOB_QUEUE_OPTION &option );

	BOOL TryStopJobProcessor();
	BOOL IsCompletionStopJobProcessor();
	DWORD GetStatus(){ return m_dwStatus; }

	void SupendThreads();
	void ResumeThreads();

	void SetThreadPriority( DWORD dwThreadNum, int dwThreadPriority );
	int GetThreadPriority( DWORD dwThreadNum );

	// 모든 스레드를 Sleep 시키고 GetCompletionJob을 통해서 대기 하지 말자. 설마 그렇게 쓰는 사람 없겠지.
	void RequestSleep( DWORD dwWorkerThreadIndex ){}
	void RequestWakeup( DWORD dwWorkerThreadIndex ){}

	void SetThreadProcessor( DWORD dwThreadNum, DWORD dwProcessorNum );
	int GetThreadProcessor( DWORD dwThreadNum );

	DWORD CreatePort( DWORD dwPortType = PORT_TYPE_NORMAL );
	BOOL MakeJob( CFJobBase* pJob, DWORD dwPort );
	BOOL MakeWaitJob( CFJobContainer &jobs, DWORD dwPort );

	// 요청해둔 Job이 완료될때까지 기다린다. dwMilisecond를 지정해주면 시간내에 완료가 안되면 Timeout 된다.
	// Timeout이 되면 FALSE가 리턴되고 시간내에 완료가 되면 TRUE가 리턴된다.
	// NORMAL PORT의 경우는 기다리지 않고 dwMillisecond가 무시되며 무조건 TRUE가 리턴된다. 
	BOOL GetCompletionJob( CFJobContainer &vCompletionJobs, DWORD dwPort, DWORD dwMillisecond = INFINITE );

protected:
	virtual CsThreadBase * CreateWorkerThread( 
		const char *szThreadName, ITSJobQueue ** paWaitJobQueue, CFPortInfo* paPortInfo ) = 0;
	virtual void DestroyWorkerThread( CsThreadBase *pWorkerThread ) = 0;
	virtual ITSJobQueue * AllocCompletionQueue() = 0;
	virtual void AllocWaitQueue( ITSJobQueue ** paWaitQueue, DWORD &dwCount ) = 0;

	void Terminate();
	void ClearJobQueue();

protected:
	CsThreadBase* m_paBossWorkerThread[MAX_THREAD];
	CsWaitableCollection m_ThreadCollection;

	DWORD m_dwStatus;
	DWORD m_dwPortCount;
	DWORD m_dwThreadCount;
	DWORD m_dwQueueCount;

	DWORD m_dwNormalQueueIndex;
	DWORD m_dwWaitQueueIndex;

	ITSJobQueue *m_paWaitJobQueue[MAX_QUEUE];
	CFPortInfo m_aPortInfo[MAX_PORT];
};

class CFBackgroundJobProcessor : public CFBackgroundJobProcessorBase
{
public:
	CFBackgroundJobProcessor(){}
	virtual ~CFBackgroundJobProcessor();

	// 다른 라이브러리에서 공통적으로 사용하기 위한 용도의 BackgroundJobProcessor
	// Instance는 CFMTLib 내부에 존재하게 됨.
	static BOOL InitializeGlobalInstance();
	static CFBackgroundJobProcessor* GetGlobalInstance();
	static BOOL DeInitializeGlobalInstance();

protected:
	CsThreadBase * CreateWorkerThread( const char *szThreadName, ITSJobQueue ** paWaitJobQueue, CFPortInfo* paPortInfo );
	void DestroyWorkerThread( CsThreadBase *pWorkerThread );
	ITSJobQueue * AllocCompletionQueue();
	void AllocWaitQueue( ITSJobQueue ** paWaitQueue, DWORD &dwCount );
};