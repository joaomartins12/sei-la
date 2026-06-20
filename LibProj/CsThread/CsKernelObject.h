/********************************************************************
	created:	2006/03/06
	created:	6:3:2006   17:55
	filename: 	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB\CsKernelObjecth
	file path:	d:\work\Lsp\CsNetworkLIB\CsNetworkLIB
	file base:	CsKernelObject
	file ext:	h
	author:		celes
	
	Windows 커널객체의 특징은 HANDLE을 이용해서 접근하는데 있습니다.
	CsKernelObject는 이러한 HANDLE Warpping class 입니다.
*********************************************************************/

#pragma once

#include"CsKernelGlobal.h"

class CsKernelObject
{
public:
	CsKernelObject();
	virtual ~CsKernelObject();

	void ThrowError( DWORD dwStatus );
	DWORD GetStatus() const;
	HANDLE GetHandle() const;

	operator HANDLE() const;

private:
	// copy constructor와 assign operator를 쓸 수 없게 막아놓음.
	// 생성된 녀석만이 CloseHandle()를 해야 하기 때문에
	// CsKernelObject는 pointer로만 다루는게 좋다.
	CsKernelObject( const CsKernelObject& lhObject );
	CsKernelObject& operator=( const CsKernelObject& lhObject );

protected:
	HANDLE m_hHandle;
	DWORD m_dwStatus;

private:
	BOOL m_bCopy;		// copy constructor에 의해서 생성된 녀석인가?
};

inline CsKernelObject::CsKernelObject()
{
	m_hHandle = NULL;
	m_dwStatus = ERROR_INVALID_HANDLE;
	m_bCopy = FALSE;
}

inline CsKernelObject::CsKernelObject( const CsKernelObject& lhObject )
{
	m_hHandle = lhObject.m_hHandle;
	m_dwStatus = ERROR_INVALID_HANDLE;
	m_bCopy = TRUE;
}

inline CsKernelObject::~CsKernelObject()
{
	if( CsKIsValidHandle( m_hHandle ) && !m_bCopy )
	{
		::CloseHandle( m_hHandle );
	}
	m_hHandle = NULL;
}

inline void CsKernelObject::ThrowError( DWORD dwStatus )
{
	CsKThrowError( dwStatus );
}

inline DWORD CsKernelObject::GetStatus(void) const
{
	return m_dwStatus;
}

inline HANDLE CsKernelObject::GetHandle() const
{
	return m_hHandle;
}

inline CsKernelObject& CsKernelObject::operator=( const CsKernelObject& lhObject )
{
	if( CsKIsValidHandle( m_hHandle ) && !m_bCopy )
	{
		::CloseHandle( m_hHandle );
	}

	m_hHandle = lhObject.m_hHandle;
	m_dwStatus = ERROR_INVALID_HANDLE;
	m_bCopy = TRUE;
	return *this;
}

inline CsKernelObject::operator HANDLE() const
{
	return m_hHandle;
}