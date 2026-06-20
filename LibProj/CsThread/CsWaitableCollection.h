/********************************************************************
	created:	2006/03/06
	created:	6:3:2006   20:46
	filename: 	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB\CsWaitableCollection.h
	file path:	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB
	file base:	CsWaitableCollection
	file ext:	h
	author:		celes
	
	복수의 CsWaitableObject Instance를 Wait할 때 편하게 쓰도록 만든 Warpping Class
	WaitForMultipleObjects에 AddObject 기능을 추가했습니다.
*********************************************************************/

#pragma once

#include"CsWaitableObject.h"
#include"CsCriticalSection.h"

class CsWaitableCollection
{
public:
	CsWaitableCollection();
	virtual ~CsWaitableCollection();

	CsWaitableCollection( const CsWaitableCollection & rhs );
	CsWaitableCollection& operator=( const CsWaitableCollection & rhs );

	LONG GetCount() const;

	void Clear();
    BOOL AddObject( const CsWaitableObject *pObject );
	BOOL AddObject( const CsWaitableObject &object );
	BOOL AddObject( const HANDLE handle );
	BOOL AddCollection( const CsWaitableCollection & rhs );

	DWORD Wait( BOOL bWaitAll, DWORD dwMilliseconds = INFINITE ) const;

private:
	HANDLE m_aHandles[ MAXIMUM_WAIT_OBJECTS ];
	LONG m_lObjects;
};