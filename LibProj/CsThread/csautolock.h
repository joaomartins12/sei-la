/********************************************************************
	created:	2006/03/06
	created:	6:3:2006   20:49
	filename: 	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB\CsAutolock.h
	file path:	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB
	file base:	CsAutolock
	file ext:	h
	author:		celes
	
	brace( '{}' ) scope를 벗어나면 자동으로 Unlock을 해주는 lock입니다.
	사용예)
	{
		CsAutolock( g_CriticalSection );
	 -- 여기에서 g_CriticalSection이 lock 됨.

		-- Critical Section --
	}-- 여기에서 g_CriticalSection이 자동으로 unlock 됨.

	특성상 CRITICAL_SECTION만 지원합니다.

*********************************************************************/

#pragma once

#include"CsCriticalSection.h"
#include"CsSpinLock.h"
#include"CsMutex.h"

class CsAutolock
{
public:
	CsAutolock( CsCriticalSection *pCS );
	virtual ~CsAutolock();

protected:
	CsCriticalSection* m_pCS;
};

inline CsAutolock::CsAutolock( CsCriticalSection *pCS )
{
	m_pCS = pCS;
	m_pCS->Lock();
}

inline CsAutolock::~CsAutolock()
{
	m_pCS->Unlock();
}

class CsAutoSpinlock
{
public:
	CsAutoSpinlock( CsSpinlock *pCS );
	virtual ~CsAutoSpinlock();

protected:
	CsSpinlock* m_pCS;
};

inline CsAutoSpinlock::CsAutoSpinlock( CsSpinlock *pCS )
{
	m_pCS = pCS;
	m_pCS->Lock();
}

inline CsAutoSpinlock::~CsAutoSpinlock()
{
	m_pCS->Unlock();
}

#define _SYNCHRONIZE_THREAD_( cs ) CsAutolock _autolock( cs );
#define _SYNCHRONIZE_THREAD_SPINLOCK_( cs ) CsAutoSpinlock _autolock( cs );