/********************************************************************
	created:	2006/02/24
	created:	24:2:2006   18:35
	filename: 	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB\CsEvent.h
	file path:	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB
	file base:	CsEvent
	file ext:	h
	author:		celes
	
	Event Warpping Class.
*********************************************************************/

#pragma once

#include"CsWaitableObject.h"

class CsEvent : public CsWaitableObject
{
public:
	CsEvent( BOOL bManualReset = FALSE, BOOL bInitState = FALSE, LPCTSTR lpName = NULL, LPSECURITY_ATTRIBUTES lpSA = NULL );
	CsEvent( LPCTSTR lpName, BOOL bInheritHandle = FALSE, DWORD dwDesiredAccess = EVENT_ALL_ACCESS );

	BOOL Set() const;
	BOOL Reset() const;
};
