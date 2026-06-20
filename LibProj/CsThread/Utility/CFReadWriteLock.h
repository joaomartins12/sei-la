/********************************************************************
	created:	2007/04/05
	created:	5:4:2007   15:59
	filename: 	d:\work\KUF-COD\Network\CFMTLib\Utility\CFReadWriteLock.h
	file path:	d:\work\KUF-COD\Network\CFMTLib\Utility
	file base:	CFReadWriteLock
	file ext:	h
	author:		celes
	
	purpose:	ExclusiveLockManager를 사용해서 만든 Read-Write Lock
	
	1. Read Lock이 걸린 상태에서 Read Lock을 걸면 Block 되지 않는다.
	2. Read Lock이 걸린 상태에서 Write Lock을 걸면 Read Lock이 모두 풀리기 전까지 Block 된다.
	3. Write Lock이 걸린 상태에서는 Read/Write Lock은 모두 Block 된다
	4. Starvation을 방지하기 위해서 Write Lock이 걸리면 이후의 모든 Read/Write Lock은 Block 된다.

	example

	void ReadProcess()
	{
		CFRWCondition cond;

		ReadWriteLock.TryReadLock( cond );
		// 리소스 읽기 접근 접근
		.......................
		ReadWriteLock.Unlock( cond );
	}

	void WriteProcess()
	{
		CFRWCondition cond;

		ReadWriteLock.TryWriteLock( cond );
		// 리소스 쓰기 접근 접근
		.......................
		ReadWriteLock.Unlock( cond );
	}
*********************************************************************/

#pragma once

#include "CFExclusiveLockManager.h"

class CFRWCondition;
typedef CFExclusiveLockManager< CFRWCondition > CFReadWriteLockManager;

class CFReadWriteLock
{
public:
	CFReadWriteLock();
	virtual ~CFReadWriteLock();

	void TryReadLock( CFRWCondition &condition );
	void TryWriteLock( CFRWCondition &condition );
	void Unlock( CFRWCondition &condition );

private:
	CFReadWriteLockManager m_RWLockManager;
};

class CFRWCondition : public CFExclusiveCondition< CFRWCondition >
{
public:
	CFRWCondition();

	BOOL IsExclusive( CFRWCondition* lhs );

	void SetWriteLock( BOOL bWriteLock = FALSE );
	BOOL IsWriteLock();

private:
	BOOL m_bWriteLock;
};