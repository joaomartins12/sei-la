/********************************************************************
	created:	2007/11/12
	created:	12:11:2007   19:34
	filename: 	d:\work\Network\CsMTLib\CsSpinlock.h
	file path:	d:\work\Network\CsMTLib
	file base:	CsSpinlock
	file ext:	h
	author:		celes
	
	Thread를 wait 상태로 보내지 않고 loop를 돌면서 대기하는 lock.
*********************************************************************/

#pragma once

// 락의 상태
enum
{
	LOCKED   = 0, ///< 잠겼음
	UNLOCKED = -1, ///< 풀렸음
};

class CsSpinlock
{
public:
	CsSpinlock()
	{
		m_lock = UNLOCKED;
	}

	virtual ~CsSpinlock()
	{
		m_lock = UNLOCKED;
	}

	void Lock()
	{
		for ( ;; )
		{
			if (UNLOCKED == InterlockedCompareExchange( &m_lock, LOCKED, UNLOCKED) )
			{
				// At this point, you have safely acquired the
				// lock. However, if you do not have an import
				// barrier, then subsequent reads might get stale
				// copies of data that has been updated in L2.
#ifdef _XBOX
				__lwsync(); // Import barrier
#endif
				return;
			}
		}
	}

	void Unlock()
	{
		// You are about to release the lock, but if you do not
		// have an export barrier, then some of your previous
		// writes may not get to the cache until after the
		// write to *pLock.
		// Interlocked operations are not needed here because
		// no other thread is allowed to write to *pLock at this time.
#ifdef _XBOX
		__lwsync();
#endif
		// *pLock is -1 when available, 0 when locked.
		m_lock = UNLOCKED;
	}

private:
	volatile long m_lock;
};