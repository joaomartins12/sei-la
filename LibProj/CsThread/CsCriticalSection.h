/********************************************************************
	created:	2006/02/23
	created:	23:2:2006   20:37
	filename: 	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB\CsCriticalSection.h
	file path:	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB
	file base:	CsCriticalSection
	file ext:	h
	author:		celes
	
	CRITICAL_SECTION Warpping class
	CsAutoLock´Â 
*********************************************************************/

#pragma once

class CsCriticalSection
{
public:
	CsCriticalSection(){ InitializeCriticalSection( &m_CS ); }
	~CsCriticalSection(){ DeleteCriticalSection( &m_CS ); }

	void Lock(){ EnterCriticalSection( &m_CS ); }
	void Unlock(){ LeaveCriticalSection( &m_CS ); }

protected:
	CRITICAL_SECTION m_CS;
};