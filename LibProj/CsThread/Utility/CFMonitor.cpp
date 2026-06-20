#include "stdafx.h"
#include "CFMonitor.h"
#include<stdio.h>

CFCondition::CFCondition() : m_evCondition( FALSE )
{
}

CFCondition::~CFCondition()
{
}

void CFCondition::WaitForEvent()
{
	m_evCondition.Wait();
}

void CFCondition::SignalEvent()
{
	m_evCondition.Set();
}

CFMonitor::CFConditionNode::CFConditionNode( CFCondition *pcCondition )
{
	m_pcCondition = pcCondition;
	m_pcnNext = NULL;
	m_pcnPrev = NULL;
}

CFMonitor::CFMonitor() : m_csMutex( 1, 1 ),
						 m_cnMaster( NULL ), m_cnFree( NULL ), m_dwStatus( NO_ERROR )
{
	// m_csMutex 초기화가 실패하면 m_dwStatus에 에러코드를 적고 리턴한다. 
	Cs_CHECK_CREATION_STATUS( m_csMutex.GetStatus(), m_dwStatus );

	m_cnMaster.m_pcnNext = m_cnMaster.m_pcnPrev = &m_cnMaster;
	m_cnFree.m_pcnNext = m_cnFree.m_pcnPrev = &m_cnFree;
}

CFMonitor::~CFMonitor()
{
	Cleanup();
}


void CFMonitor::Enter(void)
{
	// 모니터의 입구를 동기화 시킴. 결과로 모든 Condition은 Monitor에 한개만 들어 갈 수 있다.
	m_csMutex.Wait();
}

void CFMonitor::Leave(void)
{
	LeaveAndScanConditions();
}

BOOL CFMonitor::WaitForCondition( CFCondition *pcCondition )
{
	// Condition이 FALSE이면 모니터 리스트에서 대기시키고 
	// 모니터 리스트에서 대기중인 Condition을 검사한다. TRUE이면 Condition이 아무도 없다면
	// Monitor에 새로운 Condition이 Enter하는것을 허용한다.
	if( pcCondition->Condition() == FALSE )
	{
		CFConditionNode *pNewNode = AllocateNodeForCondition( pcCondition );
		if( pNewNode == NULL )
		{
			// 메모리 할당 실패.
			// new가 NULL을 리턴하는것은 왠만해선 불가능..
			CsKThrowError( ERROR_OUTOFMEMORY );
			return FALSE;
		}

		// 노드를 모니터 리스트에 추가한다.
		AppendConditionNode( pNewNode );

		// 모니터 리스트에서 대기중인 Condition을 검사한다.
		LeaveAndScanConditions();

		// Condition이 TRUE가 될때까지 대기한다.
		pcCondition->WaitForEvent();
	}

	return TRUE;
}

DWORD CFMonitor::Status(void) const
{
	return m_dwStatus;
}

void CFMonitor::LeaveAndScanConditions()
{
	CFConditionNode *pNode = m_cnMaster.m_pcnNext;

	while( pNode != &m_cnMaster )
	{
		if( pNode->m_pcCondition->Condition() ){
			break;
		}

		pNode = pNode->m_pcnNext;
	}

	if( pNode != &m_cnMaster ){
		// Condition이 TRUE인 노드가 존재하면 깨우고 모니터 리스트에서 지워준다.
		CFCondition *pcCondition = pNode->m_pcCondition;
		RemoveConditionNode( pNode );
		AddToFreeList( pNode );
		pcCondition->SignalEvent();
	}
	else{
		// Condition이 TRUE인 노드가 존재하지 않으면 모니터로 다른 Condition의 진입을 허가.
		m_csMutex.Release( 1 );
	}
}

void CFMonitor::AppendConditionNode( CFConditionNode *pcnNewNode )
{
	m_cnMaster.m_pcnPrev->m_pcnNext = pcnNewNode;
	pcnNewNode->m_pcnPrev = m_cnMaster.m_pcnPrev;
	m_cnMaster.m_pcnPrev = pcnNewNode;
	pcnNewNode->m_pcnNext = &m_cnMaster;
}

void CFMonitor::RemoveConditionNode( CFConditionNode *pcnOldNode )
{
	pcnOldNode->m_pcnPrev->m_pcnNext = pcnOldNode->m_pcnNext;
	pcnOldNode->m_pcnNext->m_pcnPrev = pcnOldNode->m_pcnPrev;
}

void CFMonitor::AddToFreeList( CFConditionNode *pcnFreeNode )
{
	pcnFreeNode->m_pcCondition = NULL;

	m_cnFree.m_pcnPrev->m_pcnNext = pcnFreeNode;
	pcnFreeNode->m_pcnPrev = m_cnFree.m_pcnPrev;
	m_cnFree.m_pcnPrev = pcnFreeNode;
	pcnFreeNode->m_pcnNext = &m_cnFree;
}

CFMonitor::CFConditionNode* CFMonitor::AllocateNodeForCondition( CFCondition *pcCondition )
{
	CFConditionNode *pNode = m_cnFree.m_pcnNext;
	if( pNode != &m_cnFree ){
		pNode->m_pcnPrev->m_pcnNext = pNode->m_pcnNext;
		pNode->m_pcnNext->m_pcnPrev = pNode->m_pcnPrev;
		pNode->m_pcCondition = pcCondition;
		pNode->m_pcnPrev = pNode;
		pNode->m_pcnNext = pNode;
	}
	else{
		pNode = new CFConditionNode( pcCondition );
	}

	return pNode;
}

void CFMonitor::Cleanup()
{
	// 모니터 리스트를 모두 삭제.
	CFConditionNode *pNode = m_cnMaster.m_pcnNext;
	while( pNode != &m_cnMaster )
	{
		CFConditionNode *pDeleteNode = pNode;
		pNode = pNode->m_pcnNext;
		delete pDeleteNode;
	}

	// Free 리스트를 모두 삭제.
	pNode = m_cnFree.m_pcnNext;
	while( pNode != &m_cnFree )
	{
		CFConditionNode *pDeleteNode = pNode;
		pNode = pNode->m_pcnNext;
		delete pDeleteNode;
	}
}


