/********************************************************************
	created:	2006/03/06
	created:	6:3:2006   18:41
	filename: 	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB\CsKernelGlobal.h
	file path:	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB
	file base:	CsKernelGlobal
	file ext:	h
	author:		celes
	
	KERNEL HANDLE utility functions

*********************************************************************/

#pragma once

#include<process.h>
#include<winerror.h>

#ifndef __CsKERNEL_THROW_EXCEPTION__
#define __CsKERNEL_THROW_EXCEPTION__ FALSE
#endif

#if __CsKERNEL_THROW_EXCEPTION__
#define Cs_CHECK_CREATION_STATUS( SCODE, STATUS ) {}
#else
#define Cs_CHECK_CREATION_STATUS( SCODE, STATUS ) if ( ( (SCODE) != NO_ERROR ) && ( (SCODE) != ERROR_ALREADY_EXISTS ) ) { STATUS = (SCODE); return; }
#endif

class CsKException
{
public:
	CsKException( DWORD dwStatus ){ m_dwStatus = dwStatus; }
	DWORD GetExceptionStatus(){ return m_dwStatus; }
	operator DWORD(){ return m_dwStatus; }

protected:
	DWORD m_dwStatus;
};

// 예외를 날림.
#define CsKThrowError( dwStatus ) CsKInternalThrowError( ( dwStatus ), __FILE__, __LINE__ )

void CsKInternalThrowError( DWORD dwStatus, const char* lpFileName, int line );

// 유효한 Handle 인지 알아낸다.
extern inline BOOL CsKIsValidHandle( HANDLE hHandle );

// Wait가 Success 되었는지 알아낸다.
extern inline BOOL CsKWaitSucceeded( DWORD dwWaitResult, DWORD dwHandleCount );

// Wait가 Abandon 되었는지 알아낸다.
extern inline BOOL CsKWaitAbandoned( DWORD dwWaitResult, DWORD dwHandleCount );

// Wait Timeout 되었는지 알아낸다.
extern inline BOOL CsKWaitTimeout( DWORD dwWaitResult );

// Wait Fail의 여부를 알아낸다.
extern inline BOOL CsKWaitFailed( DWORD dwWaitResult );

// Wait Success된 Kernel Object의 Index를 얻는다.
extern inline DWORD CsKWaitSucceededIndex( DWORD dwWaitResult );

// Abandon된 Kernel Object의 Index를 얻는다.
extern inline DWORD CsKWaitAbandonedIndex( DWORD dwWaitResult );

