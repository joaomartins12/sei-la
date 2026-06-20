/********************************************************************
	created:	2006/03/29
	created:	29:3:2006   18:09
	filename: 	d:\work\Kuf-cod\Network\CFMTLib\Utility\CFMonitor.h
	file path:	d:\work\Kuf-cod\Network\CFMTLib\Utility
	file base:	CFMonitor
	file ext:	h
	author:		celes
	
	- 같은 자원을 공유해서 사용하는 스레드를 조율하기 사용되는 동기화 툴
	- Condition class를 구현해서 융통성 있게 동기화 방법을 적용할 수 있다.
	- Monitor의 기본 동작은 다음과 같다.

	~ 모니터에 동시에 진입가능한 스레드의 갯수는 한개로 제한된다.
	~ 모니터에 진입한 스레드의 Condition이 FALSE이면 스레드는 바로 Wait Queue에 들어가고
	  Wait이 된다. 
	~ 진입한 스레드가 모니터에서 Leave 되면 모니터는 Wait Queue에서 대기중인 스레드중 Condition이 TRUE
	  인 스레드를 깨운다. 없으면 진입을 원하는 스레드를 Enter 시킨다.
	~ 진입한 스레드가 Wait이 되면 모니터는 Wait Queue에서 대기중인 스레드중 Condition이 TRUE인
	  스레드를 깨운다.
    ~ 만약 Wait Queue가 비었거나 Condition이 TRUE인 스레드가 없다면 모니터에 진입하기를 원하는 새로운
	  스레드를 Enter 시킨다.
*********************************************************************/

#pragma once

#include "../CsWaitableObject.h"
#include "../CsEvent.h"
#include "../CsSemaphore.h"

class CFCondition
{
public:
	CFCondition();
	virtual ~CFCondition();

	void WaitForEvent();
	void SignalEvent();

	// 자원을 획득할 조건을 구현해준다.
	virtual BOOL Condition() = 0;

private:
	CsEvent m_evCondition;
	
	friend class CFMonitor;
};


class CFMonitor
{
public:
	CFMonitor();
	virtual ~CFMonitor();

	class CFConditionNode {
	public:
		CFConditionNode( CFCondition *pcCondition );

	public: 
		CFCondition *m_pcCondition;
		CFConditionNode *m_pcnNext;
		CFConditionNode *m_pcnPrev;

	};

	void Enter(void);
	void Leave(void);
	BOOL WaitForCondition( CFCondition *pcCondition);

	DWORD Status(void) const;

	// Enter를 하기 전에 Leave가 먼저 호출되는 경우를 대비해서 Semaphore를 사용함.
	CFSemaphore m_csMutex;

	// 노드의 끝을 가르킴. 노드는 링 구조로 되어 있기 때문에 next는 노드의 시작이 됨.
	CFConditionNode m_cnMaster;

	// 메모리 재할당을 막기 위해서 사용이 끝난 노드들을 저장한다.
	CFConditionNode m_cnFree;

	// 생성자 상태.
	DWORD m_dwStatus;

private:
	void LeaveAndScanConditions();
	void AppendConditionNode( CFConditionNode *pcnNewNode );
	void RemoveConditionNode( CFConditionNode *pcnOldNode );
	void AddToFreeList( CFConditionNode *pcnFreeNode );
	CFConditionNode *AllocateNodeForCondition( CFCondition *pcCondition );
	void Cleanup();
};
