/********************************************************************
	created:	2006/03/30
	created:	30:3:2006   16:11
	filename: 	d:\work\Kuf-cod\Network\CFMTLib\CFSemaphore.h
	file path:	d:\work\Kuf-cod\Network\CFMTLib
	file base:	CFSemaphore
	file ext:	h
	author:		celes
	
	Mutex 처럼 Critical Section을 상호배타적으로 획득할 수 있도록 해주는 동기화 객체.
	Mutex와 다른 점은 획득가능한 횟수를 정할 수 있다.

	예를들어 초기값을 2로 지정해 놓으면 
*********************************************************************/

#pragma once

#include"CsWaitableObject.h"

class CFSemaphore : public CsWaitableObject
{
public:
	CFSemaphore( int nInitialCount = 0, int nMaximumCount = 0x7fffffff, LPCTSTR lpName = NULL, LPSECURITY_ATTRIBUTES lpsa = NULL );
	CFSemaphore( LPCTSTR lpName, BOOL bInheritHandle = FALSE, DWORD dwDesiredAccess = SEMAPHORE_ALL_ACCESS );

	BOOL Release( LONG lReleaseCount, LONG *plPreviousCount = NULL );
};
