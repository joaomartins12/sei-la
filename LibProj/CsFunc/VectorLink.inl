

template < typename T >
CsVectorLink< T >::CsVectorLink()
{
	m_nElementCount = 0;
	m_nAllocCount = 0;
	m_DataArray = NULL;

	m_DataBegin.s_pPrev = NULL;
	m_DataBegin.s_pNext = &m_DataEnd;
	m_DataBegin.s_bHaveElement = true;

	m_DataEnd.s_pPrev = &m_DataBegin;
	m_DataEnd.s_pNext = NULL;
	m_DataEnd.s_bHaveElement = true;
}

template < typename T >
CsVectorLink< T >::CsVectorLink( int nCount )
{
	m_nElementCount = 0;
	m_nAllocCount = 0;
	m_DataArray = NULL;

	m_DataBegin.s_pPrev = NULL;
	m_DataBegin.s_pNext = &m_DataEnd;
	m_DataBegin.s_bHaveElement = true;

	m_DataEnd.s_pPrev = &m_DataBegin;
	m_DataEnd.s_pNext = NULL;
	m_DataEnd.s_bHaveElement = true;

	Init( nCount );	
}

template < typename T >
CsVectorLink< T >::~CsVectorLink()
{
	Destroy();
}

template < typename T >
void CsVectorLink< T >::Destroy()
{
	SAFE_DELETE_ARRAY( m_DataArray );
	m_DataBegin.Delete();
	m_DataEnd.Delete();
	m_nAllocCount = 0;
	m_nElementCount = 0;
}

template < typename T >
void CsVectorLink<T>::Init( int nAllocCount )
{
	assert_cs( m_DataArray == NULL );
	assert_cs( m_nElementCount == 0 );

	m_nAllocCount = nAllocCount;
	m_DataArray = csnew iterator[ m_nAllocCount ];
}

template < typename T >
void CsVectorLink<T>::Reset()
{
	assert_cs( m_DataArray != NULL );
	m_DataBegin.s_pPrev = NULL;
	m_DataBegin.s_pNext = &m_DataEnd;
	m_DataBegin.s_bHaveElement = true;

	m_DataEnd.s_pPrev = &m_DataBegin;
	m_DataEnd.s_pNext = NULL;
	m_DataEnd.s_bHaveElement = true;

	m_nElementCount = 0;
}

template < typename T >
void CsVectorLink<T>::SetData( int nIndexArray, T Element )
{
	assert_cs( m_DataArray[ nIndexArray ].s_bHaveElement == false );
	assert_cs( m_DataArray[ nIndexArray ].s_pPrev == NULL );
	assert_cs( m_DataArray[ nIndexArray ].s_pNext == NULL );
	assert_cs( m_DataArray[ nIndexArray ].s_nKey == 0 );

	iterator* pData = &m_DataArray[ nIndexArray ];
	pData->s_bHaveElement = true;
	pData->s_Element = Element;
	pData->s_nKey = (unsigned short)nIndexArray;

	for( int i=nIndexArray-1; i>-1; --i )
	{
		if( m_DataArray[ i ].s_bHaveElement == true )
		{
			iterator* pPrev = &m_DataArray[ i ];
			pData->s_pPrev = pPrev;
			pData->s_pNext = pPrev->s_pNext;
			pPrev->s_pNext = pData;
			pData->s_pNext->s_pPrev = pData;
			break;
		}
	}
	// 앞에 달린게 없다면
	if( pData->s_pPrev == NULL )
	{
		pData->s_pPrev = &m_DataBegin;
		pData->s_pNext = m_DataBegin.s_pNext;
		m_DataBegin.s_pNext = pData;
		pData->s_pNext->s_pPrev = pData;		
	}

	++m_nElementCount;
}