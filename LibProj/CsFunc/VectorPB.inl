

template < typename T >
CsVectorPB< T >::CsVectorPB()
{
	m_nAddAllocCount = 0;
	m_nAllocCount = 0;
	m_nElementCount = 0;
	m_AllocPlag = AP_DOUBLE;

	m_Data = NULL;
}

template < typename T >
CsVectorPB< T >::CsVectorPB( int nCount, DWORD AllocPlag )
{
	m_nAddAllocCount = nCount;
	m_nAllocCount = nCount;
	m_nElementCount = 0;
	m_AllocPlag = AllocPlag;

	m_Data = (T*)malloc( sizeof( T )* nCount );
	assert_cs( m_Data != NULL );
}

template < typename T >
CsVectorPB< T >::~CsVectorPB()
{
	Destroy();
}

template < typename T >
void CsVectorPB< T >::Destroy()
{
	if( m_nAllocCount == 0 )
		return;

	if( m_Data != NULL )
	{
		free( m_Data );
		m_Data = NULL;
	}
	m_nAddAllocCount = 0;
	m_nAllocCount = 0;
	m_nElementCount = 0;
}

template < typename T >
void CsVectorPB< T >::DeleteElement( int nIndex )
{
	assert_cs( m_nElementCount != 0 );
	for( int i=nIndex+1; i<m_nElementCount; ++i )
	{
		m_Data[ i-1 ] = m_Data[ i ];
	}
	--m_nElementCount;
}

template < typename T >
void CsVectorPB< T >::DeleteElement_FromData( T data )
{
	for( int i=0; i<m_nElementCount; ++i )
	{
		if( m_Data[ i ] == data )
		{
			DeleteElement( i );
			return;
		}		
	}
}

template < typename T >
void CsVectorPB<T>::Init( int nAllocCount, DWORD AllocPlag )
{
	m_nAddAllocCount = nAllocCount;
	m_nAllocCount = 0;
	m_nElementCount = 0;
	m_AllocPlag = AllocPlag;

	assert_cs( m_nAddAllocCount != 0 );
	switch( m_AllocPlag )
	{
	case AP_ADD:
		{
			m_nAllocCount += m_nAddAllocCount;
		}		
		break;
	case AP_DOUBLE:
		{
			if( m_nAllocCount == 0)
				m_nAllocCount = m_nAddAllocCount;
			else
				m_nAllocCount *= 2;
		}		
		break;
	default:
		assert_cs( false );
	}

	m_Data = (T*)malloc( sizeof( T )*m_nAllocCount );
	assert_cs( m_Data != NULL );	
}

template < typename T >
void CsVectorPB< T >::PushBack( T data )
{
	if( m_nAllocCount == m_nElementCount )
	{
		if( m_nAddAllocCount == 0 )
		{
			Init( 1, AP_DOUBLE );
		}
		else
		{
			Realloc();
		}
	}
	m_Data[ m_nElementCount ] = data;
	++m_nElementCount;	
}

template < typename T >
void CsVectorPB< T >::PushFront( T data )
{
	if( m_nAllocCount == m_nElementCount )
	{
		if( m_nAddAllocCount == 0 )
		{
			Init( 1, AP_DOUBLE );
		}
		else
		{
			Realloc();
		}
	}

	for( int i=m_nElementCount-1; i>-1; --i )
	{
		m_Data[ i+1 ] = m_Data[ i ];
	}
	m_Data[ 0 ] = data;
	++m_nElementCount;
}

template < typename T >
void CsVectorPB< T >::PushInsert( int nIndex, T data )
{
	while( m_nAllocCount <= ( m_nElementCount + 1 ) )
	{
		if( m_nAddAllocCount == 0 )
		{
			Init( 1, AP_DOUBLE );
		}
		else
		{
			Realloc();
		}
	}

	for( int i=m_nElementCount-1; i>nIndex; --i )
	{
		m_Data[ i+1 ] = m_Data[ i ];
	}
	m_Data[ nIndex + 1 ] = data;
	++m_nElementCount;	
}

template < typename T >
void CsVectorPB< T >::Realloc()
{
	assert_cs( m_nAddAllocCount != 0 );
	switch( m_AllocPlag )
	{
	case AP_ADD:
		{
			m_nAllocCount += m_nAddAllocCount;
		}		
		break;
	case AP_DOUBLE:
		{
			if( m_nAllocCount == 0)
				m_nAllocCount = m_nAddAllocCount;
			else
				m_nAllocCount *= 2;
		}		
		break;
	default:
		assert_cs( false );
	}

	m_Data = (T*)realloc( m_Data, sizeof( T )*m_nAllocCount );
	assert_cs( m_Data != NULL );	
}

template < typename T >
bool CsVectorPB< T >::IsExistData( T data )
{
	for( int i=0; i<m_nElementCount; ++i )
	{
		if( m_Data[ i ] == data )
			return true;
	}
	return false;
}

template < typename T >
bool CsVectorPB< T >::IsExistElement( int nIndex )
{
	return ( 0 <= nIndex && nIndex < m_nElementCount );
}

template < typename T >
bool CsVectorPB< T >::IsExistAlloc( int nIndex )
{
	return ( 0 <= nIndex && nIndex < m_nAllocCount );
}
