
#pragma once

//=====================================================================================================
//
//		생성자 소멸자 호출 안됨
//
//=====================================================================================================

template <typename T>
class CsVectorPB
{	
protected:
	int			m_nElementCount;
	int			m_nAllocCount;
	int			m_nAddAllocCount;
	DWORD		m_AllocPlag;
	T*			m_Data;
public:
	CsVectorPB();
	CsVectorPB( int nCount, DWORD AllocPlag = AP_DOUBLE );
	~CsVectorPB();

public:
	void		Destroy();
	void		DeleteElement( int nIndex );
	void		DeleteElement_FromData( T data );
	void		ClearElement(){ m_nElementCount = 0; }	
	void		Init( int nAllocCount, DWORD AllocPlag );

	int			Size(){ return m_nElementCount; }
	int			GetAllocCount(){ return m_nAllocCount; }

	void		PushBack( T data );
	void		PushFront( T data );
	void		PushInsert( int nIndex, T data );

	void		Realloc();
	bool		IsInit(){ return ( m_nAllocCount != 0 ); }
	bool		IsExistData( T data );
	bool		IsExistElement( int nIndex );
	bool		IsExistAlloc( int nIndex );
	void		SetMemZero(){ assert_cs( m_nAllocCount != 0 );	memset( m_Data, 0, sizeof( T )*m_nAllocCount ); }

	void		SetElementCount( int nCount ){ assert_cs( nCount <= m_nAllocCount ); m_nElementCount = nCount; }
	T&			GetData( int nIndex ){ assert_cs( nIndex < m_nAllocCount ); return (T&)m_Data[ nIndex ]; }
	T*			GetDataArray(){ return m_Data; }

public:
	T&			operator[](int nIndex){ assert_cs( nIndex < m_nAllocCount ); return (T&)m_Data[ nIndex ]; }	
};

#include "VectorPB.inl"