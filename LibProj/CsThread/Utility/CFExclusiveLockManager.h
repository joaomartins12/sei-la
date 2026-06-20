/********************************************************************
	created:	2006/03/31
	created:	31:3:2006   16:31
	filename: 	d:\work\Kuf-cod\Network\CFMTLib\Utility\CFExclusiveLockManager.h
	file path:	d:\work\Kuf-cod\Network\CFMTLib\Utility
	file base:	CFExclusiveLockManager
	file ext:	h
	author:		celes
	
	- Resource를 획득하려는 스레드들의 Condition을 관리해주고 서로 상호베타적인지
	검사해서 Lock을 관리해주는 Manager

	- IsExclusive()와 Lock Manager를 만들수 있다.

	- Condition 체크를 최소수행단위로 만들어주고 Starvation을 막아주기 위해 Monitor을 사용하였음.
*********************************************************************/

#pragma once

#include "CFMonitor.h"
#include<algorithm>
#include<list>
#include<assert.h>

// CFExclusiveLockManager의 T는 CFExclusiveCondition를 상속받아서 구현하면 된다.
template <typename T>
class CFExclusiveCondition
{
public:
	CFExclusiveCondition() : m_bActive( FALSE ){}
	void SetActive( BOOL bActive ){ m_bActive = bActive; }
	BOOL IsActive(){ return m_bActive; }

	virtual BOOL IsExclusive( T* rhs ) const = 0;

private:
	BOOL   m_bActive;
};

template <typename T>
class CFExclusiveLockManager
{
	class CFMyCondition : public CFCondition
	{
	public:
		CFMyCondition(){}
		void SetValue( std::list<T*>* pliConditionList,  T* pCondition )
		{
			m_pliConditionList = pliConditionList;
			m_pCondition = pCondition;
		}
		virtual ~CFMyCondition(){}
		BOOL Condition();

	protected:
		std::list<T*>* m_pliConditionList;
		T* m_pCondition;
	};

public:
	CFExclusiveLockManager();
	virtual ~CFExclusiveLockManager();

	void Lock( T* pCondition );
	void Unlock( T* pCondition );

	// pCondition과 똑같은 우선순위의 Lock을 건다. pCondition은 Unlock되고 다시 Lock 된다.
	void UnlockAndLock( T* pCondition );
protected:

	// 우선순위가 높은 Condition을 넣는다.
	void AddPrecededCondition( T* pPrecededCondition );
	// Argument Condition과 Exclusive하지 않은 Condition들을 Argument Condition 뒤로 밀어버린다.
	void MoveNotExclusiveConditionToBackOfCondition( T* pCondition );
	std::list<T*> m_liConditionList;
	CFMonitor m_Monitor;
};

template <typename T>
BOOL CFExclusiveLockManager<T>::CFMyCondition::Condition()
{
	// Starvation을 막기 위해서 상호배타적이지 않은 Condition중에 먼저 들어온 Condition이 존재한다면
	// FALSE를 리턴.

	// 자신의 Condition을 찾는다.
	std::list<T*>::iterator it = std::find(
		m_pliConditionList->begin(), m_pliConditionList->end(), m_pCondition );

	assert( it != m_pliConditionList->end() );

	while( it != m_pliConditionList->begin() )
	{
		it--;
		if( !m_pCondition->IsExclusive( *it ) )
			return FALSE;
	}

	return TRUE;
}

template <typename T>
CFExclusiveLockManager<T>::CFExclusiveLockManager()
{
}

template <typename T>
CFExclusiveLockManager<T>::~CFExclusiveLockManager()
{
}

template <typename T>
void CFExclusiveLockManager<T>::Lock( T* pCondition )
{
	CFMyCondition condition;
	m_Monitor.Enter();
	m_liConditionList.push_back( pCondition );
	
	std::list<T*>* pliConditionList = &m_liConditionList;
	condition.SetValue( pliConditionList, pCondition );
	
	pCondition->SetActive( FALSE );
	m_Monitor.WaitForCondition( &condition );
	pCondition->SetActive( TRUE );
	
	m_Monitor.Leave();
}

template <typename T>
void CFExclusiveLockManager<T>::Unlock( T* pCondition )
{
	m_Monitor.Enter();

	std::list<T*>::iterator it = std::find(
		m_liConditionList.begin(), m_liConditionList.end(), pCondition );

	assert( it != m_liConditionList.end() );

	if( it != m_liConditionList.end() ){
		m_liConditionList.erase( it );
	}

	m_Monitor.Leave();
}

template <typename T>
void CFExclusiveLockManager<T>::UnlockAndLock( T* pCondition )
{
	m_Monitor.Enter();
	
	std::list<T*>::iterator it = std::find(
		m_liConditionList.begin(), m_liConditionList.end(), pCondition );

	if( it != m_liConditionList.end() ){
		m_liConditionList.erase( it );
	}

	// 최대한 우선순위를 둬서 Condition을 등록하자!
	AddPrecededCondition( pCondition );

	CFMyCondition condition;
	std::list<T*>* pliConditionList = &m_liConditionList;
	condition.SetValue( pliConditionList, pCondition );
	
	pCondition->SetActive( FALSE );
	m_Monitor.WaitForCondition( &condition );
	pCondition->SetActive( TRUE );
	
	m_Monitor.Leave();
}

template <typename T>
void CFExclusiveLockManager<T>::AddPrecededCondition( T* pPrecededCondition )
{
	// 새로 Lock을 거는 Condition과 배타적이지 않으면서 Active 상태인 Condition 바로 뒤에 Condition Instance를 넣는다.
	std::list<T*>::iterator it = m_liConditionList.end();

	while( TRUE )
	{
		if( it == m_liConditionList.begin() )
		{
			m_liConditionList.push_front( pPrecededCondition );
			break;
		}

		it--;

		if( !pPrecededCondition->IsExclusive( *it ) && (*it)->IsActive() )
		{
			m_liConditionList.insert( ++it , pPrecededCondition );
			break;
		}
	}
}

// 새 Condition과 배타적이지 않으면셔 아직 Active 상태가 아닌 Condition 들을 모두 새 Condition 뒤로 옮긴다.
// (하는게 맞을까?--)
template <typename T>
void CFExclusiveLockManager<T>::MoveNotExclusiveConditionToBackOfCondition( T* pCondition )
{
	std::list<T*>::iterator myIt = std::find(
		m_liConditionList.begin(), m_liConditionList.end(), pCondition );
	std::list<T*>::iterator it = myIt;

	assert( it != m_liConditionList.end() );

	while( it != m_liConditionList.begin() )
	{
		it--;
		if( !(*myIt)->IsExclusive( *it ) && !(*it)->IsActive() )
		{
			std::list<T*>::iterator whereIt = myIt;
			whereIt++;
			m_liConditionList.insert( whereIt, *it );
			it = m_liConditionList.erase( it );
		}
	}
}