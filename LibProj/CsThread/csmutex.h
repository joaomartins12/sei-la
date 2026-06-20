/********************************************************************
	created:	2006/03/06
	created:	6:3:2006   20:35
	filename: 	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB\CsMutex.h
	file path:	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB
	file base:	CsMutex
	file ext:	h
	author:		celes
*********************************************************************/

#pragma once

#include"CsWaitableObject.h"

class CsMutex : public CsWaitableObject
{
public:
	CsMutex( BOOL bInitOwner = FALSE, LPCTSTR lpName = NULL, LPSECURITY_ATTRIBUTES lpSA = NULL );
	CsMutex( LPCTSTR lpName, BOOL bInheritHandle = FALSE, DWORD dwDesiredAccess = MUTEX_ALL_ACCESS );

	BOOL Release();
};
