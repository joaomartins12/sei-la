/********************************************************************
	created:	2006/03/06
	created:	6:3:2006   19:28
	filename: 	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB\CsWaitableObject.h
	file path:	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB
	file base:	CsWaitableObject
	file ext:	h
	author:		celes
	
	Kernel Object중 WaitFor~~ 시리즈의 함수로 Signal 되기 전까지 wait 할 수 있는
	특징을 가진 Object들의 warpping class 입니다.

*********************************************************************/

#pragma once

#include"CsKernelObject.h"

class CsWaitableObject : public CsKernelObject
{
public:
	virtual ~CsWaitableObject();

	DWORD Wait( DWORD dwMilliseconds = INFINITE ) const;
	DWORD WaitTwo( CsWaitableObject &rhWaitableObject, BOOL bWaitAll, DWORD dwMilliseconds = INFINITE ) const;
	DWORD WaitTwo( HANDLE &rhEvent, BOOL bWaitAll, DWORD dwMilliseconds = INFINITE ) const;
};

inline CsWaitableObject::~CsWaitableObject()
{
}

inline DWORD CsWaitableObject::Wait( DWORD dwMilliseconds /*= INFINITE*/) const
{
	return ::WaitForSingleObject( m_hHandle, dwMilliseconds );
}

inline DWORD CsWaitableObject::WaitTwo( CsWaitableObject &rhWaitableObject, BOOL bWaitAll, DWORD dwMilliseconds /*= INFINITE*/ ) const
{
	HANDLE handles[2] = { m_hHandle, rhWaitableObject.GetHandle() };
	
	return ::WaitForMultipleObjects( 2, handles, bWaitAll, dwMilliseconds );
}

inline DWORD CsWaitableObject::WaitTwo( HANDLE &rhEvent, BOOL bWaitAll, DWORD dwMilliseconds /*= INFINITE */) const
{
	HANDLE handles[2] = { m_hHandle, rhEvent };

	return ::WaitForMultipleObjects( 2, handles, bWaitAll, dwMilliseconds );
}
